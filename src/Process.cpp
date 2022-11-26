#include "Process.h"
#include <iostream>

Process::Process(Image& patternImage, Image& representativeImage, Image& finalImage, Palette& palette)
	: m_pattern { patternImage }, m_representative { representativeImage }, m_final { finalImage }, m_palette { palette }
	, m_mutex{ std::mutex{} }, m_maxThreads{ std::thread::hardware_concurrency() }, m_futures{ std::vector<std::future<ProcessResult>>{} }
	, m_sharedCache{ PixelCacheMap{}  }
{
	m_sharedCache.reserve(m_final.getWidth() * m_final.getHeight());
}

Process::~Process()
{
	m_futures.clear();
	m_sharedCache.clear();
}

void Process::start()
{
	const unsigned int portion{ (m_representative.getWidth() / m_maxThreads) + 1 };
	std::cout << "\nApplying palette (" << m_maxThreads << " threads)\n";

	for (int i{ 0 }; i < m_representative.getWidth(); i += portion)
	{
		std::future<ProcessResult> future{ 
			std::async(
				std::launch::async, 
				&Process::transformImage,
				std::ref(*this),
				std::ref(m_sharedCache),
				i,
				std::min(m_representative.getWidth(), static_cast<int>(i + portion))
			)
		};

		m_futures.push_back(std::move(future));
	}

	for (auto& future : m_futures)
	{
		ProcessResult result{ future.get() };
		for (auto& [ i, j, r, g, b ] : result)
		{
			Pixel p{ r, g, b };
			m_final.setPixel(i, j, p);
		}
	}
}

ProcessResult Process::transformImage(PixelCacheMap& cache, int min, int max)
{
	ProcessResult result{};
	result.reserve((max - min) * m_representative.getHeight());

	m_representative.traverse([&](int i, int j) {
		Pixel sourcePixel{ m_representative.getPixel(i, j) };

		Pixel closestPixel;
		if (cache.count(sourcePixel) == 0)
		{
			int delta{ std::numeric_limits<int>::max() };
			for (Pixel& palettePixel : m_palette.getPixels())
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

			m_mutex.lock();
			cache[sourcePixel] = closestPixel;
			m_mutex.unlock();
		}
		else
		{
			closestPixel = cache[sourcePixel];
		}

		result.push_back(std::make_tuple(i, j, closestPixel.r, closestPixel.g, closestPixel.b));
	}, min, max);

	std::cout << ". ";
	return result;
}