#pragma once

#include <functional>
#include <unordered_map>

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

using PixelCacheMap = std::unordered_map<Pixel, Pixel, PixelHashFn>;
