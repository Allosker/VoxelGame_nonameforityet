#pragma once // basicHitbox.hpp
// MIT
// Allosker ------------------------------
// =========+
// The aim of the class is to provide a basic system for AABB vs. AABB collision tests and response
// ---------------------------------------

#include "uHeaders/opengl.hpp"
#include "world/world.hpp"

namespace Physics::Collisions
{

	// AABB vs. AABB
	struct BasicHitbox
	{

		// = Construction/Destruction

		BasicHitbox(const vec3f& pos, const vec3f& min_, const vec3f& max_) noexcept
			: min{ pos - min_}, max{ pos + max_}, realMin{ min_ }, realMax{ max_ }
		{ }

		BasicHitbox(const vec3f& min_, const vec3f& max_)
			: min{ min_ }, max{ max_ }
		{ }

		BasicHitbox() noexcept = default;

		// = Actors

		constexpr bool intersects(const BasicHitbox& outer) const noexcept
		{
			return
				min.x < outer.max.x && max.x > outer.min.x &&

				min.y < outer.max.y && max.y > outer.min.y &&

				min.z < outer.max.z && max.z > outer.min.z;
		}

		constexpr vec3f findIntersection(const BasicHitbox& outer) const noexcept
		{
			const float right	{ max.x - outer.min.x };
			const float down	{ max.y - outer.min.y };
			const float back	{ max.z - outer.min.z };

			const float left	{ outer.max.x - min.x };
			const float up		{ outer.max.y - min.y };
			const float front	{ outer.max.z - min.z };

			vec3f result	{};
			float bestDist	{ -1 };

			if (left > 0 && (left < bestDist || bestDist < 0))
			{
				bestDist = left;
				result = -vec3f{ left, 0, 0 };
			}

			if (right > 0 && (right < bestDist || bestDist < 0))
			{
				bestDist = right;
				result = vec3f{ right, 0, 0 };
			}


			if (up > 0 && (up < bestDist || bestDist < 0))
			{
				bestDist = up;
    			result = -vec3f{ 0, up, 0 };
			}

			if (down > 0 && (down < bestDist || bestDist < 0))
			{
				bestDist = down;
				result = vec3f{ 0, down, 0 };
			}


			if (front > 0 && (front < bestDist || bestDist < 0))
			{
				bestDist = front;
				result = -vec3f{ 0, 0, front };
			}

			if (back > 0 && (back < bestDist || bestDist < 0))
			{
				bestDist = back;
 				result = vec3f{ 0, 0, back };
			}


			return result;
		}

		
		// = Setters

		void setHitbox(const vec3f& pos)
		{
			min = pos - realMin;
			max = pos + realMax;
		}

		// = Members

		vec3f min{};
		vec3f max{};

	private:

		vec3f realMin{};
		vec3f realMax{};
	};

} // Physics::Collisions