#pragma once

#include <string>
#include <vector>

#include <turbojpeg.h>
#include <png.h>

#include "Pixel.h"

class Image {
public:
	constexpr static int JPG_PIXEL_FORMAT{ TJPF_RGB };

	Image() = default;
	Image(int width, int height, std::size_t bufferSize);
	~Image();

	void openJPG(const std::string& path);
	void traverse(const std::function<void(int, int)>& callable, int startRange = 0, int endRange = 0);
	void savePNG(const std::string& path);

	inline int getWidth() const { return m_width; }
	inline int getHeight() const { return m_height; }
	inline std::vector<unsigned char>& getBuffer() { return m_buffer; }
	inline std::vector<Pixel>& getPalette() { return m_palette; }

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
};