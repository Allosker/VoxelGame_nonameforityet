#pragma once // ray.hpp
// MIT
// Allosker ------------------------------
// =========+
// Define a basic ray using DDA, once cast, the ray continues until it either hits a non-empty block or hits its distance cap
// ---------------------------------------

#include <optional>

#include "GamePlay/World/chunk.hpp"
#include "uHeaders/types.hpp"

namespace Render::Utils
{

	class Ray
	{
	public:

		// Initialization

		Ray() noexcept = default;

		Ray(const vec3f& origin, const vec3f& dir) noexcept
			: m_origin{ origin }, m_ray{ origin }, m_step{1}, m_nextBound{ std::floor(m_ray.x), std::floor(m_ray.y), std::floor(m_ray.z) }
		{
			if (dir.x < 0)
				m_step.x = -1;
			if (dir.y < 0)
				m_step.y = -1;
			if (dir.z < 0)
				m_step.z = -1;

			// Next Bound
			if (dir.x > 0)
				m_nextBound.x++;
			if (dir.y > 0)
				m_nextBound.y++;
			if (dir.z > 0)
				m_nextBound.z++;


			m_tMax.x = (m_nextBound.x - m_ray.x) / dir.x;
			m_tMax.y = (m_nextBound.y - m_ray.y) / dir.y;
			m_tMax.z = (m_nextBound.z - m_ray.z) / dir.z;

			m_tDelta.x = 1 / std::abs(dir.x);
			m_tDelta.y = 1 / std::abs(dir.y);
			m_tDelta.z = 1 / std::abs(dir.z);
		}


		// Actors

		void update() noexcept
		{
			if (m_tMax.x < m_tMax.y)
			{
				if (m_tMax.x < m_tMax.z)
				{
					m_ray.x += m_step.x;
					m_tMax.x += m_tDelta.x;
				}
				else
				{
					m_ray.z += m_step.z;
					m_tMax.z += m_tDelta.z;
				}
			}
			else
			{
				if (m_tMax.y < m_tMax.z)
				{
					m_ray.y += m_step.y;
					m_tMax.y += m_tDelta.y;
				}
				else
				{
					m_ray.z += m_step.z;
					m_tMax.z += m_tDelta.z;
				}
			}
		}
		

		// Getters

		const types::pos& getPos() const noexcept { return m_ray; }

		const types::pos& getOrigin() const noexcept { return m_origin; }


	protected:

		vec3f	m_ray{};
		vec3f	m_origin{};
		vec3f	m_step{};
		vec3f	m_nextBound{};
		vec3f	m_tMax{};
		vec3f	m_tDelta{};


	};

} // Render::Utils