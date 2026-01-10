#include "clock.hpp"

// =====================
// Actors
// =====================

GameTime::Clock::Clock(bool start_on_creation) noexcept
{
	start();
}

void GameTime::Clock::start() noexcept
{
	time_start = std::chrono::high_resolution_clock::now();
}

void GameTime::Clock::reset() noexcept
{
	time_start = std::chrono::high_resolution_clock::now();
}

void GameTime::Clock::stop() noexcept
{
	time_start = {};
}


// =====================
// Getters
// =====================

GameTime::Time GameTime::Clock::getTime() const noexcept
{
	return time_start;
}


