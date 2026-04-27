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

		constexpr bool intersect(const BasicHitbox& outer) const noexcept
		{
			return
				min.x < outer.max.x && max.x > outer.min.x &&

				min.y < outer.max.y && max.y > outer.min.y &&

				min.z < outer.max.z && max.z > outer.min.z;
		}

		constexpr vec3f findIntersection(const BasicHitbox& outer, const GameWorld::World& world, const types::loc& block_loc) const noexcept
		{
			const vec3f right	{ max.x - outer.min.x, 0, 0 };
			const vec3f down	{ 0, max.y - outer.min.y, 0 };
			const vec3f back	{ 0, 0, max.z - outer.min.z };

			const vec3f left	{ outer.max.x - min.x, 0, 0 };
			const vec3f up		{ 0, outer.max.y - min.y, 0 };
			const vec3f front	{ 0, 0, outer.max.z - min.z };

			vec3f result	{};
			float bestDist	{ -1 };

			if (left.x > 0 && (left.x < bestDist || bestDist < 0))
			{
				if (!world.block_at(block_loc + types::loc{ 1, 0, 0 }))
				{
					result = -left;
					bestDist = left.x;
				}
			}

			if (right.x > 0 && (right.x < bestDist || bestDist < 0))
			{
				if (!world.block_at(block_loc + types::loc{ -1, 0, 0 }))
				{
					result = right;
					bestDist = right.x;
				}
			}


			if (up.y > 0 && (up.y < bestDist || bestDist < 0))
			{
				if (!world.block_at(block_loc + types::loc{ 0, 1, 0 }))
				{
					result = -up;
					bestDist = up.y;
				}
			}

			if (down.y > 0 && (down.y < bestDist || bestDist < 0))
			{
				if (!world.block_at(block_loc + types::loc{ 0, -1, 0 }))
				{
					result = down;
					bestDist = down.y;
				}
			}


			if (front.z > 0 && (front.z < bestDist || bestDist < 0))
			{
				if (!world.block_at(block_loc + types::loc{ 0, 0, 1 }))
				{
					result = -front;
					bestDist = front.z;
				}
			}

			if (back.z > 0 && (back.z < bestDist || bestDist < 0))
			{
				if (!world.block_at(block_loc + types::loc{ 0, 0, -1 }))
				{
					result = back;
					bestDist = back.z;
				}
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