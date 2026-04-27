#pragma once // ray.hpp
// MIT
// Allosker ------------------------------
// =========+
// Define a basic ray using DDA, once cast, the ray continues until it either hits a non-empty block or hits its distance cap
// ---------------------------------------

#include <optional>

#include "world/voxels/chunkGrid.hpp"
#include "uHeaders/types.hpp"


namespace GameWorld::Voxels::Utils
{

	struct Ray
	{
	public:

		// = Initialization

		Ray() noexcept = default;

		Ray(const vec3f& origin_, const vec3f& dir) noexcept
			: pos{ origin_ }, step{ 1 }, nextBound{ std::floor(pos.x), std::floor(pos.y), std::floor(pos.z) }
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

		// = Members

		vec3f	pos{};
		vec3f	step{};
		vec3f	nextBound{};
		vec3f	tMax{};
		vec3f	tDelta{};

	};

	struct RayCastResult
	{
		vec3f origin{};
		vec3f pos{};
		vec3f normal{};
	};

	// Returns the location of the block that was hit by the ray alongside its corresponding chunk, if no block was hit, std::nullopt is returned
	static inline std::optional<RayCastResult> raycast(const types::pos& origin, const types::pos& dir, const GameWorld::Voxels::ChunkGrid& grid, uint64 maxLength, const Render::Data::Types::VoxelTypeManager& type_manager) noexcept
	{
		Ray ray{ origin, dir };
		vec3f normal{};

		while ((origin - ray.pos).length() < maxLength)
		{
			if (!grid.is_empty(ray.pos, type_manager))
			{
				return std::make_optional<RayCastResult>({ origin, ray.pos, normal });
			}


			if (ray.tMax.x < ray.tMax.y)
			{
				if (ray.tMax.x < ray.tMax.z)
				{
					ray.pos.x += ray.step.x;
					ray.tMax.x += ray.tDelta.x;
					normal = { -ray.step.x, 0.f, 0.f };
				}
				else
				{
					ray.pos.z += ray.step.z;
					ray.tMax.z += ray.tDelta.z;
					normal = { 0.f, 0.f, -ray.step.z };
				}
			}
			else
			{
				if (ray.tMax.y < ray.tMax.z)
				{
					ray.pos.y += ray.step.y;
					ray.tMax.y += ray.tDelta.y;
					normal = { 0.f, -ray.step.y, 0.f };
				}
				else
				{
					ray.pos.z += ray.step.z;
					ray.tMax.z += ray.tDelta.z;
					normal = { 0.f, 0.f, -ray.step.z };
				}
			}

		}

		return std::nullopt;
	}

	static inline std::optional<RayCastResult> raycast_chunk(const types::pos& origin, const types::pos& dir, const GameWorld::Voxels::ChunkGrid& grid, uint64 render_distance, const Render::Data::Types::VoxelTypeManager& type_manager) noexcept
	{
		Ray ray{ origin, dir };
		vec3f normal{};

		while ((origin - ray.pos).length() < render_distance)
		{
			if (ray.tMax.x < ray.tMax.y)
			{
				if (ray.tMax.x < ray.tMax.z)
				{
					ray.pos.x += ray.step.x;
					ray.tMax.x += ray.tDelta.x;
					normal = { -ray.step.x, 0.f, 0.f };
				}
				else
				{
					ray.pos.z += ray.step.z;
					ray.tMax.z += ray.tDelta.z;
					normal = { 0.f, 0.f, -ray.step.z };
				}
			}
			else
			{
				if (ray.tMax.y < ray.tMax.z)
				{
					ray.pos.y += ray.step.y;
					ray.tMax.y += ray.tDelta.y;
					normal = { 0.f, -ray.step.y, 0.f };
				}
				else
				{
					ray.pos.z += ray.step.z;
					ray.tMax.z += ray.tDelta.z;
					normal = { 0.f, 0.f, -ray.step.z };
				}
			}

		}

		return std::nullopt;
	}


} // Render::Utils