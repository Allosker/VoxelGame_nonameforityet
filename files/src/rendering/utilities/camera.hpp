#pragma once // camera.hpp
// MIT
// Allosker ------------------------------
// =========+
// Define a basic Camera struct with three basic positions. Note: this is subject to changes.
// ---------------------------------------

#include "uHeaders/opengl.hpp"

namespace Render::Utils
{

	struct Camera
	{
		Camera(vec3f pos_Start) noexcept
			: pos{ pos_Start }
		{
		}

		Camera() noexcept = default;

		vec3f right_dir() const noexcept { return front_dir.cross(up_dir); }

		vec3f pos{ 0.f, 0.f, -1.f };
		vec3f up_dir{ vec3f{0.f, 1.f, 0.f}.normal() };
		vec3f front_dir{ 0.f, 0.f, 0.f };
	};

}