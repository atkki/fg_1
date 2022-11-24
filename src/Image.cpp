#include <iostream>
#include <exception>
#include <fstream>

#include "Image.h"
#include "Monitor.hpp"

Image::Image(int width, int height, std::size_t bufferSize)
	: m_width { width }, m_height { height }, m_buffer { std::vector<unsigned char>(bufferSize) }
{

}

Image::~Image()
{
	m_buffer.clear();
}

void Image::openJPG(const std::string& path)
{
	std::ifstream file{ path, std::ios::binary | std::ios::ate };

	if (!file)
		throw std::runtime_error(path + " could not be opened");

	// check file size
	std::streamsize size{ file.tellg() };
	file.seekg(0, std::ios::beg);

	// read file to buffer
	std::vector<char> rawBuffer(size);
	file.read(rawBuffer.data(), size);

	// decompressor instance
	tjhandle compressorInstance{ tjInitDecompress() };

	// header
	int width{ 0 };
	int height{ 0 };
	int subsamp;
	int colorspace;

	int decompressResult;
	decompressResult = tjDecompressHeader3(
		compressorInstance,
		reinterpret_cast<const unsigned char*>(rawBuffer.data()),
		static_cast<unsigned long>(size),
		&width,
		&height,
		&subsamp,
		&colorspace
	);

	if (decompressResult == -1)
		throw std::runtime_error("JPEG fail\n" + std::string{ tjGetErrorStr2(compressorInstance) });

	// decompress pixels
	m_pixelSize = tjPixelSize[JPG_PIXEL_FORMAT];

	std::vector<unsigned char> decompressedBuffer(width * height * m_pixelSize);
	decompressResult = tjDecompress2(
		compressorInstance,
		reinterpret_cast<const unsigned char*>(rawBuffer.data()),
		static_cast<unsigned long>(size),
		decompressedBuffer.data(),
		width,
		0,
		height,
		JPG_PIXEL_FORMAT, TJFLAG_FASTDCT
	);

	if (decompressResult == -1)
		throw std::runtime_error("JPEG fail\n" + std::string{ tjGetErrorStr2(compressorInstance) });

	// result
	m_width = width;
	m_height = height;
	m_buffer = decompressedBuffer;
}

void Image::traverse(const std::function<void(int, int)>& callable, int startRange, int endRange)
{
	if (endRange == 0) endRange = m_width;
	for (int i{ 0 }; i < m_height; ++i)
	{
		for (int j{ startRange }; j < endRange; ++j)
		{
			callable(i, j);
		}
	}
}

void Image::createPalette()
{
	std::unordered_map<Pixel, bool, PixelHashFn> pixelCache{};

	this->traverse([&](int i, int j) {
		Pixel pixel{ this->getPixel(i, j) };
		if (pixelCache.count(pixel) == 0) {
			pixelCache[pixel] = true;
		}
	});

	std::vector<Pixel> palette;
	palette.reserve(pixelCache.size());

	for (auto pair : pixelCache)
	{
		palette.push_back(pair.first);
	}

	m_palette = palette;
	m_colorCount = static_cast<int>(pixelCache.size());
}

// C code here for sake of compatibility with libpng
#pragma warning(disable : 4996)
void Image::savePNG(const std::string& path)
{
	FILE* fp{ fopen(path.c_str(), "wb") };
	if (!fp) throw std::runtime_error(path + " could not be created");

	png_structp png{ png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL) };
	if (!png) throw std::runtime_error(path + " load error");

	png_infop info{ png_create_info_struct(png) };
	if (!info) throw std::runtime_error(path + " load error");

	if (setjmp(png_jmpbuf(png))) throw std::runtime_error(path + " load error");

	png_init_io(png, fp);

	png_set_IHDR(
		png,
		info,
		m_width, m_height,
		8,
		PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);
	png_write_info(png, info);

	// write RGB pixels
	png_bytepp row_pointers = (png_bytepp)png_malloc(png, sizeof(png_bytepp) * m_height);
	for (int i{ 0 }; i < m_height; ++i) 
	{
		png_byte* row = (png_bytep)png_malloc(png, sizeof(png_byte) * m_width * m_pixelSize);
		row_pointers[i] = row;

		for (int j{ 0 }; j < m_width; ++j)
		{
			*row++ = m_buffer[(i * m_width * m_pixelSize) + (j * m_pixelSize) + 0];
			*row++ = m_buffer[(i * m_width * m_pixelSize) + (j * m_pixelSize) + 1];
			*row++ = m_buffer[(i * m_width * m_pixelSize) + (j * m_pixelSize) + 2];
		}
	}

	png_write_image(png, row_pointers);
	png_write_end(png, NULL);

	// cleanup
	for (int i{ 0 }; i < m_height; ++i)
	{
		png_free(png, row_pointers[i]);
	}
	png_free(png, row_pointers);

	fclose(fp);
	png_destroy_write_struct(&png, &info);
}

Pixel Image::getPixel(int i, int j)
{
	return Pixel{
		m_buffer[(i * m_width * m_pixelSize) + (j * m_pixelSize) + 0],
		m_buffer[(i * m_width * m_pixelSize) + (j * m_pixelSize) + 1],
		m_buffer[(i * m_width * m_pixelSize) + (j * m_pixelSize) + 2],
	};
}

void Image::setPixel(int i, int j, Pixel& p)
{
	m_buffer[(i * m_width * m_pixelSize) + (j * m_pixelSize) + 0] = p.r;
	m_buffer[(i * m_width * m_pixelSize) + (j * m_pixelSize) + 1] = p.g;
	m_buffer[(i * m_width * m_pixelSize) + (j * m_pixelSize) + 2] = p.b;
}
