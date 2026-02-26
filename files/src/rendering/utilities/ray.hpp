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

	struct RayCastResult
	{
		vec3f origin{};
		vec3f pos{};
		vec3f normal{};
	};

	class Ray
	{
	public:

		// Initialization

		Ray() noexcept = default;

		Ray(const vec3f& origin_, const vec3f& dir) noexcept
			: pos{ origin_ }, step{1}, nextBound{ std::floor(pos.x), std::floor(pos.y), std::floor(pos.z) }
		{
			if (dir.x < 0)
				step.x = -1;
			if (dir.y < 0)
				step.y = -1;
			if (dir.z < 0)
				step.z = -1;

			// Next Bound
			if (dir.x > 0)
				nextBound.x++;
			if (dir.y > 0)
				nextBound.y++;
			if (dir.z > 0)
				nextBound.z++;


			tMax.x = (nextBound.x - pos.x) / dir.x;
			tMax.y = (nextBound.y - pos.y) / dir.y;
			tMax.z = (nextBound.z - pos.z) / dir.z;

			tDelta.x = 1 / std::abs(dir.x);
			tDelta.y = 1 / std::abs(dir.y);
			tDelta.z = 1 / std::abs(dir.z);
		}


	public:

		vec3f	pos{};
		vec3f	step{};
		vec3f	nextBound{};
		vec3f	tMax{};
		vec3f	tDelta{};

	};

} // Render::Utils