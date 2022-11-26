#pragma once
#include <thread>
#include <future>
#include <mutex>

#include "Image.h"
#include "Palette.h"

using ProcessResult = std::vector<std::tuple<int, int, unsigned char, unsigned char, unsigned char>>;

class Process
{
public:
	Process(Image& patternImage, Image& representativeImage, Image& finalImage, Palette& palette);
	~Process();

	void start();
private: 
	ProcessResult transformImage(PixelCacheMap& cache, int min, int max);

	const unsigned int m_maxThreads;
	std::vector<std::future<ProcessResult>> m_futures;
	std::mutex m_mutex;

	Image& m_pattern;
	Image& m_representative;
	Image& m_final;
	Palette& m_palette;
	PixelCacheMap m_sharedCache{};
};