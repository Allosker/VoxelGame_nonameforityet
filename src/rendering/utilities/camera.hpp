#pragma once // camera.hpp
// MIT
// Allosker ------------------------------
// =========+
// Define a basic Camera struct with three basic positions. Note: this is subject to changes.
// ---------------------------------------

#include "utilities/opengl.hpp"

namespace Render::Utils
{
	struct Camera
	{
		Camera(vec3f pos_Start) noexcept
			: pos{ pos_Start }
		{
		}

		Camera() noexcept = default;

		void updateProjMatrix(vec2i size) noexcept
		{
			proj = mpml::perspective(mpml::Angle<>::fromDegrees(90), size.x, size.y, 0.1f, 1000.f);
		}

		vec3f right_dir() const noexcept { return front_dir.cross(up_dir); }

		mat4f model{ mpml::Identity4<float> };
		mat4f view {};
		mat4f proj {};

		vec3f pos{ 0.f, 0.f, -1.f };
		vec3f up_dir{ vec3f{0.f, 1.f, 0.f}.normal() };
		vec3f front_dir{ 0.f, 0.f, -1.f };

		bool free{ false };
	};

}