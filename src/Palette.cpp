#include "Palette.h"
#include <iostream>

Palette::Palette(Image& image)
	: m_image{ image }, m_pixelsBoolCache{ std::unordered_map<Pixel, bool, PixelHashFn>{} }
{
	this->create();
}

void Palette::create()
{
	m_image.traverse([&](int i, int j) {
		Pixel pixel{ m_image.getPixel(i, j) };
		if (m_pixelsBoolCache.count(pixel) == 0) {
			m_pixelsBoolCache[pixel] = true;
		}
	});

	std::vector<Pixel> palette{};
	palette.reserve(m_pixelsBoolCache.size());

	for (auto pair : m_pixelsBoolCache)
	{
		palette.push_back(pair.first);
	}

	m_pixels = palette;
	m_colorCount = static_cast<int>(palette.size());
}

Palette::~Palette()
{
	m_pixels.clear();
	m_pixelsBoolCache.clear();
}
