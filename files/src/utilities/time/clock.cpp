#include "clock.hpp"

// =====================
// Construction/Destruction
// =====================

game_time::Clock::Clock(bool start_on_creation) noexcept
{

}

game_time::Clock::~Clock() noexcept
{
}


// =====================
// Actors
// =====================

void game_time::Clock::start() noexcept
{
	if(time_start != 0)
		time_start = glfwGetTime();
}

void game_time::Clock::reset() noexcept
{
	time_start = glfwGetTime();
}

void game_time::Clock::stop() noexcept
{
	time_start = 0;
}


// =====================
// Getters
// =====================

double game_time::Clock::asMilliseconds() const noexcept
{
	if (time_start == 0)
		return 0;

	return glfwGetTime() - time_start;
}

double game_time::Clock::asSeconds() const noexcept
{
	return (glfwGetTime() - time_start);
}

double game_time::Clock::asMinutes() const noexcept
{
	return (glfwGetTime() - time_start) / 60000;
}

double game_time::Clock::asHours() const noexcept
{
	return (glfwGetTime() - time_start) / 3.6e+6;
}
