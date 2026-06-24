#pragma once // camera.hpp
// MIT
// Allosker ------------------------------
// =========+
// Define a basic Camera struct with three basic positions. Note: this is subject to changes.
// ---------------------------------------

#include "utilities/opengl.hpp"

namespace utils
{
	struct Camera
	{
		Camera(vec3f pos_Start) noexcept
			: pos{ pos_Start }
		{
		}

		Camera() noexcept = default;


		void setDirections(const vec3f& front) noexcept
		{
			front_dir = front;

			build_cam_matrix();
		}

		void setPosition(const vec3f& position) noexcept
		{
			pos = position;

			build_cam_matrix();
		}

		void setFramebufferSize(const vec2f& new_size) noexcept
		{
			framebuffer_size = new_size;

			build_cam_matrix();
		}

		void toggle_dir_updates() noexcept { m_update_dirs = !m_update_dirs; }


		const vec3f& getRightDir() const noexcept { return front_dir.cross(up_dir); }

		const vec3f& getUpDir() const noexcept { return up_dir; }

		const vec3f& getFrontDir() const noexcept { return front_dir; }

		const vec3f& getPosition() const noexcept { return pos; }

		const mat4f& getViewProj() const noexcept { return vp; }

		const bool should_update_dirs() const noexcept { return m_update_dirs; }


	private:

		void build_cam_matrix() noexcept
		{
			mat4f view = mpml::lookAt(pos, front_dir + pos, up_dir);
			mat4f proj = mpml::perspective(mpml::Angle<>::fromDegrees(90), framebuffer_size.x, framebuffer_size.y, 0.1f, 1000.f);

			vp = view * proj;
		}


		mat4f vp{};

		vec2f framebuffer_size{};
		vec3f pos{ 0.f, 0.f, -1.f };
		vec3f up_dir{ vec3f{0.f, 1.f, 0.f}.normal() };
		vec3f front_dir{ 0.f, 0.f, -1.f };

		bool m_update_dirs{ false };
	};

}