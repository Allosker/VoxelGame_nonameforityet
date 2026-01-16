#pragma once // camera.hpp
// MIT
// Allosker ------------------------------
// =========+
// Define a basic Camera struct with three most basic positions. Note: this is subject to changes.
// ---------------------------------------

#include "mpml/mpml.hpp"

namespace Render
{

	struct Camera
	{
		Camera(vec3f pos_Start) noexcept
			: pos{ pos_Start }
		{
		}

		Camera() noexcept = default;

		vec3f pos{ 0.f, 0.f, -1.f };
		vec3f up_dir{ vec3f{0.f, 1.f, 0.f}.normal() };
		vec3f front_dir{ 0.f, 0.f, 0.f };

	};

}