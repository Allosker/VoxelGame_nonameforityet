#pragma once //clock.hpp
// MIT
// Allosker ------------------------------
// =========+
// Small class for a ready-to-use, all-in-one clock!
// ---------------------------------------

#include "uHeaders/opengl.hpp"
#include <chrono>

namespace GameTime
{

	struct Time
	{
		Time(std::chrono::time_point<std::chrono::steady_clock> time)
			: time_start{ time }
		{ }

		std::uint64_t asMicroseconds() const noexcept
		{
			const auto now = std::chrono::steady_clock::now();
			return std::chrono::duration_cast<std::chrono::microseconds>(now - time_start).count();
		}

		float asMilliseconds() const noexcept
		{
			const auto now = std::chrono::steady_clock::now();
			return static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(now - time_start).count());
		}

		float asSeconds() const noexcept
		{
			const auto now = std::chrono::steady_clock::now();
			return static_cast<float>(std::chrono::duration_cast<std::chrono::seconds>(now - time_start).count());
		}

		float asMinutes() const noexcept
		{
			const auto now = std::chrono::steady_clock::now();
			return static_cast<float>(std::chrono::duration_cast<std::chrono::minutes>(now - time_start).count());
		}

		float asHours() const noexcept
		{
			const auto now = std::chrono::steady_clock::now();
			return static_cast<float>(std::chrono::duration_cast<std::chrono::hours>(now - time_start).count());
		}


	private:

		std::chrono::time_point<std::chrono::steady_clock> time_start{};
	};


	class Clock
	{
	public:

	// = Construction/Destruction

		explicit Clock(bool start_on_creation = false) noexcept;

		~Clock() noexcept = default;


	// = Actiors

		void start() noexcept;

		void reset() noexcept;

		void stop() noexcept;


	// = Getters

		Time getTime() const noexcept;


	private:
		
		std::chrono::time_point<std::chrono::steady_clock> time_start{};

	};


} // time