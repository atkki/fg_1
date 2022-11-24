#pragma once

#include <string>
#include <vector>
#include <functional>

#include <turbojpeg.h>
#include <png.h>

struct Pixel
{
	unsigned char r{};
	unsigned char g{};
	unsigned char b{};
	
	bool operator==(const Pixel& p) const 
	{
		return r == p.r && g == p.g && b == p.b;
	}
};

struct PixelHashFn
{
	std::size_t operator() (const Pixel& p) const
	{
		std::size_t h1 = std::hash<unsigned char>()(p.r);
		std::size_t h2 = std::hash<unsigned char>()(p.g);
		std::size_t h3 = std::hash<unsigned char>()(p.b);

		return h1 ^ h2 ^ h3;
	}
};

class Image {
public:
	constexpr static int JPG_PIXEL_FORMAT{ TJPF_RGB };

	Image() = default;
	Image(int width, int height, std::size_t bufferSize);
	~Image();

	void openJPG(const std::string& path);
	void traverse(const std::function<void(int, int)>& callable, int startRange = 0, int endRange = 0);
	void createPalette();
	void savePNG(const std::string& path);

	inline int getWidth() const { return m_width; }
	inline int getHeight() const { return m_height; }
	inline std::vector<unsigned char>& getBuffer() { return m_buffer; }
	inline std::vector<Pixel>& getPalette() { return m_palette; }
	inline int getColorCount() { return m_colorCount; }

	Pixel getPixel(int i, int j);
	inline int getPixelSize() { return m_pixelSize; };
	inline void setPixelSize(int size) { m_pixelSize = size; };

	void setPixel(int i, int j, Pixel& p);

private:
	int m_width{};
	int m_height{};
	int m_pixelSize{};
	std::vector<unsigned char> m_buffer{};
	std::vector<Pixel> m_palette{};
	int m_colorCount{};
};