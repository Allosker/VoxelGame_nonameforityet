#pragma once //frustum_culling.hpp
// MIT
// Allosker ------------------------------
// =========+
// Define frustum culling for CHUNKS only (as of now)
// ---------------------------------------

#include "camera.hpp"
#include "world/voxels/chunk.hpp"
#include <map>

namespace Render::Utils
{

	struct StaticAABB
	{
		vec3f min{};
		vec3f max{};
	};

	inline bool isWithinFrustum(const mat4f& mv, const StaticAABB& aabb, 
		std::vector<std::vector<bool>>& c
		) noexcept
	{
		vec4f corners[8] = {
			{aabb.min.x, aabb.min.y, aabb.min.z, 1.0}, // base corner
			{aabb.max.x, aabb.min.y, aabb.min.z, 1.0}, // lower left
			{aabb.min.x, aabb.max.y, aabb.min.z, 1.0}, // upper base
			{aabb.max.x, aabb.max.y, aabb.min.z, 1.0}, // upper left

			{aabb.min.x, aabb.min.y, aabb.max.z, 1.0}, // front
			{aabb.max.x, aabb.min.y, aabb.max.z, 1.0}, // left front
			{aabb.min.x, aabb.max.y, aabb.max.z, 1.0}, // upper front
			{aabb.max.x, aabb.max.y, aabb.max.z, 1.0}, // opposite
		};

		bool inside{ false };
		for (size_t i{}; i < 8; i++)
		{
			vec4f corner = mv * corners[i];  

			inside = inside ||
				((-corner.w <= corner.x && corner.x <= corner.w) &&
				(-corner.w <= corner.y && corner.y <= corner.w) &&
				(-corner.w <= corner.z && corner.z <= corner.w));

			c.back().push_back((-corner.w <= corner.x && corner.x <= corner.w) &&
				(-corner.w <= corner.y && corner.y <= corner.w) &&
				(-corner.w <= corner.z && corner.z <= corner.w));
		}

		return inside;
	}

	inline std::vector<types::loc> cull_staticAABB_frustum(
		const Camera& cam,
		const std::map<types::loc, GameWorld::Voxels::Chunk>& chunks,
		std::vector<std::vector<bool>>& c
	) noexcept
	{
		mat4f mv{ cam.view * cam.proj };
		std::vector<types::loc> out{};

		for (const auto& i : chunks)
		{
			StaticAABB aabb{ i.second.getPos(), i.second.getOppositeCorner() };
			
			c.push_back({});

			if (isWithinFrustum(mv, aabb, c))
				out.push_back(i.first);
		}

		return out;
	}

}