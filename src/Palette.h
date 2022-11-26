#pragma once
#include "Image.h"

class Palette 
{
public:
	Palette(Image& image);
	~Palette();

	inline int getColorCount() const { return m_colorCount; }
	inline std::vector<Pixel>& getPixels() { return m_pixels; }
private:
	void create();

	Image& m_image;
	std::vector<Pixel> m_pixels{};
	std::unordered_map<Pixel, bool, PixelHashFn> m_pixelsBoolCache{};
	int m_colorCount{};
};