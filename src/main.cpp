#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <thread>

#include "Process.h"
#include "Monitor.hpp"

using namespace std::literals;

int main()
{
	try 
	{
		Monitor monitor{};
		Monitor totalTimeMonitor{};

		const std::string pathA{ "media/obraz-A.jpg" };
		const std::string pathB{ "media/obraz-B.jpg" };

		Image patternImage{};
		patternImage.openJPG(pathA);

		Image representativeImage{};
		representativeImage.openJPG(pathB);

		Image finalImage{
			representativeImage.getWidth(),
			representativeImage.getHeight(),
			representativeImage.getBuffer().size()
		};
		finalImage.setPixelSize(patternImage.getPixelSize());
		monitor.section("reading files"sv);

		Palette palette{ patternImage };
		Palette* representativePalette{ new Palette{ representativeImage } };
		std::cout << pathA << ": " << palette.getColorCount() << " colors\n";
		std::cout << pathB << ": " << representativePalette->getColorCount() << " colors\n";
		representativePalette = nullptr;
		delete representativePalette;
		monitor.section("color info"sv);

		Process transformProcess{ patternImage, representativeImage, finalImage, palette };
		transformProcess.start();
		std::cout << '\n';
		monitor.section("palette"sv);

		const std::string pathC{ "media/obraz-C.png" };
		finalImage.savePNG(pathC);
		monitor.section("saving media/obraz-C.png"sv);
		totalTimeMonitor.section("total conversion"sv);

		return 0;
	}
	catch (std::exception& e) 
	{
		std::cerr << "Runtime error: " << e.what() << '\n';
		return 1;
	}
}
