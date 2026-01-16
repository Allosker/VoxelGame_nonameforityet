#pragma once // color_utils.hpp
// MIT
// Allosker ------------------------------
// =========+
// Define some functions for color formating
// ---------------------------------------

#include "uHeaders/opengl.hpp"
#include <cmath>

namespace Color
{

	inline constexpr vec3f to_rgb(float h, float s, float v) noexcept
	{
		float c{ s * v };
		float hp{ h / 60.f };
		float x{ c * (1.f - (std::fmod(hp, 2.f) - 1.f)) };

		vec3f rgb{};

		if (0 <= hp && hp < 1)
			rgb = { c, x, 0 };
		else if(1 <= hp && hp < 2)
			rgb = { x, c, 0 };
		else if(2 <= hp && hp < 3)
			rgb = { 0, c, x };
		else if(3 <= hp && hp < 4)
			rgb = { 0, x, c };
		else if (4 <= hp && hp < 5)
			rgb = { x, 0, c };
		else if (5 <= hp && hp < 6)
			rgb = { c, 0, x };

		float m{ v - c };

		return { rgb.r + m, rgb.g + m, rgb.b + m };
	}

	inline constexpr vec3f to_rgb(const vec3f& hsv) noexcept
	{
		return to_rgb(hsv.h, hsv.s, hsv.v);
	}


	inline constexpr vec3f to_hsv(float r, float g, float b) noexcept
	{
		float rp{ r / 255.f };
		float gp{ g / 255.f };
		float bp{ b / 255.f };

		float cmax{ std::max({rp, gp, bp}) };
		float cmin{ std::min({rp, gp, bp}) };

		float delta{ cmax - cmin };

		vec3f hsv{};

		if (cmax < rp + 0.1 && cmax > rp - 0.1)
			hsv.h = 60.f * (std::fmod((gp - bp) / delta, 6.f));
		else if(cmax < gp + 0.1 && cmax > gp - 0.1)
			hsv.h = 60.f * ((bp - rp) / delta + 2.f);
		else if (cmax < bp + 0.1 && cmax > bp - 0.1)
			hsv.h = 60.f * ((bp - rp) / delta + 2.f);

		if (cmax < 0.1f && cmax > -0.1f)
			hsv.s = 0.f;
		else
			hsv.s = delta / cmax;

		hsv.v = cmax;

		return hsv;
	}

	inline constexpr vec3f to_hsv(const vec3f& rgb) noexcept
	{
		return to_hsv(rgb.r, rgb.g, rgb.b);
	}

}