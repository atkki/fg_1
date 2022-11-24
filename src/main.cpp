#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <thread>

#include "Image.h"
#include "Monitor.hpp"

using namespace std::literals;

void processPalette(std::unordered_map<Pixel, Pixel, PixelHashFn>& pixelCache, Image& paletteImage, Image& representativeImage, Image& finalImage, int min, int max)
{
	std::vector<Pixel>& palette{ paletteImage.getPalette() };
	auto process{ [&](int i, int j) {
		Pixel sourcePixel{ representativeImage.getPixel(i, j) };

		Pixel closestPixel;
		if (pixelCache.count(sourcePixel) == 0)
		{
			int delta{ std::numeric_limits<int>::max() };
			for (Pixel& palettePixel : palette)
			{
				int calculatedDelta{
					std::abs(palettePixel.r - sourcePixel.r) + std::abs(palettePixel.g - sourcePixel.g) + std::abs(palettePixel.b - sourcePixel.b)
				};

				if (calculatedDelta < delta)
				{
					delta = calculatedDelta;
					closestPixel = palettePixel;
				}
			}

			pixelCache[sourcePixel] = closestPixel;
		}
		else
		{
			closestPixel = pixelCache[sourcePixel];
		}

		finalImage.setPixel(i, j, closestPixel);
	} };

	representativeImage.traverse(process, min, max);

	std::cout << ". ";
}

int main()
{
	try 
	{
		Monitor monitor{};
		Monitor totalTimeMonitor{};

		const std::string pathA{ "media/obraz-A.jpg" };
		const std::string pathB{ "media/obraz-B.jpg" };

		Image paletteImage{};
		paletteImage.openJPG(pathA);

		Image representativeImage{};
		representativeImage.openJPG(pathB);

		Image finalImage{
			representativeImage.getWidth(),
			representativeImage.getHeight(),
			representativeImage.getBuffer().size()
		};
		finalImage.setPixelSize(paletteImage.getPixelSize());
		
		monitor.section("reading files"sv);

		paletteImage.createPalette();
		representativeImage.createPalette();
		std::cout << pathA << ": " << paletteImage.getColorCount() << " colors\n";
		std::cout << pathB << ": " << representativeImage.getColorCount() << " colors\n";
		monitor.section("color info"sv);

		const unsigned int maxThreads{ std::thread::hardware_concurrency() };
		std::vector<std::thread> threads;
		std::cout << "\nApplying palette (" << maxThreads << " threads)\n";

		const unsigned int portion{ (representativeImage.getWidth() / maxThreads) + 1 };

		std::unordered_map<Pixel, Pixel, PixelHashFn> pixelCache;
		pixelCache.reserve(finalImage.getBuffer().size());

		int done{ 0 };
		for (int i{ 0 }; i < representativeImage.getWidth(); i += portion)
		{
			auto thread{ std::thread(
				processPalette,
				std::ref(pixelCache),
				std::ref(paletteImage),
				std::ref(representativeImage),
				std::ref(finalImage),
				i,
				std::min(representativeImage.getWidth(), static_cast<int>(i + portion))
			) };
			threads.push_back(std::move(thread));
		}

		for (auto& thread : threads) 
		{
			thread.join();
		}

		std::cout << '\n';
		monitor.section("palette"sv);

		const std::string pathC{ "media/obraz-C.png" };
		std::cout << "Creating " << pathC << "\n";
		finalImage.savePNG(pathC);

		monitor.section("saving"sv);
		totalTimeMonitor.section("total conversion"sv);

		return 0;
	}
	catch (std::exception& e) 
	{
		std::cerr << "Runtime error: " << e.what() << '\n';
		return 1;
	}
}
