#pragma once

#include <chrono>
#include <iostream>
#include <string_view>

class Monitor
{
private: 
	using Clock = std::chrono::steady_clock;
	using Second = std::chrono::duration<double, std::ratio<1>>;
	std::chrono::time_point<Clock> m_init{ Clock::now() };

public:
	inline double elapsed() const { return std::chrono::duration_cast<Second>(Clock::now() - m_init).count(); }
	void reset() 
	{ 
		m_init = Clock::now(); 
	}

	void section(std::string_view type) 
	{
		std::cout << "Time taken (" << type << "): " << this->elapsed() << "s\n";
		this->reset();
	}
};