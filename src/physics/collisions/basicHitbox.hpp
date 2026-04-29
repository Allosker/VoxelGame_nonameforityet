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

		constexpr vec3f findIntersection(const BasicHitbox& outer, const GameWorld::World& world, const types::loc& block_loc) const noexcept
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

				if (!world.block_at(block_loc + types::loc{ 1, 0, 0 }))
					result = -vec3f{ left, 0, 0 };
				else
					result = {};
			}

			if (right > 0 && (right < bestDist || bestDist < 0))
			{
				bestDist = right;

				if (!world.block_at(block_loc + types::loc{ -1, 0, 0 }))
					result = vec3f{ right, 0, 0 };
				else
					result = {};
			}


			if (up > 0 && (up < bestDist || bestDist < 0))
			{
				bestDist = up;

				if (!world.block_at(block_loc + types::loc{ 0, 1, 0 }))
    				result = -vec3f{ 0, up, 0 };
				else
					result = {};
			}

			if (down > 0 && (down < bestDist || bestDist < 0))
			{
				bestDist = down;

				if (!world.block_at(block_loc + types::loc{ 0, -1, 0 }))
					result = vec3f{ 0, down, 0 };
				else
					result = {};
			}


			if (front > 0 && (front < bestDist || bestDist < 0))
			{
				bestDist = front;

				if (!world.block_at(block_loc + types::loc{ 0, 0, 1 }))
					result = -vec3f{ 0, 0, front };
				else
					result = {};
			}

			if (back > 0 && (back < bestDist || bestDist < 0))
			{
				bestDist = back;

				if (!world.block_at(block_loc + types::loc{ 0, 0, -1 }))
 					result = vec3f{ 0, 0, back };
				else
					result = {};
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