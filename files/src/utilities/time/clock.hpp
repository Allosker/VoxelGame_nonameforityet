#pragma once //clock.hpp
// MIT
// Allosker ------------------------------
// =========+
// Small class for a ready-to-use, all-in-one clock!
// ---------------------------------------

#include "uHeaders/opengl.hpp"

namespace game_time
{

	class Clock
	{
	public:

		explicit Clock(bool start_on_creation = true) noexcept;

		~Clock() noexcept;


		void start() noexcept;

		void reset() noexcept;

		void stop() noexcept;



		double asMilliseconds() const noexcept;

		double asSeconds() const noexcept;

		double asMinutes() const noexcept;

		double asHours() const noexcept;


	private:
		
		double time_start{};

	};


} // time