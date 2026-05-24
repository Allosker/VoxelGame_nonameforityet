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

	inline bool isWithinFrustum(const mat4f& mv, const StaticAABB& aabb) noexcept
	{
		vec4f corners[8] = {
			{aabb.min.x, aabb.min.y, aabb.min.z, 1.0}, // x y z
			{aabb.max.x, aabb.min.y, aabb.min.z, 1.0}, // X y z
			{aabb.min.x, aabb.max.y, aabb.min.z, 1.0}, // x Y z
			{aabb.max.x, aabb.max.y, aabb.min.z, 1.0}, // X Y z

			{aabb.min.x, aabb.min.y, aabb.max.z, 1.0}, // x y Z
			{aabb.max.x, aabb.min.y, aabb.max.z, 1.0}, // X y Z
			{aabb.min.x, aabb.max.y, aabb.max.z, 1.0}, // x Y Z
			{aabb.max.x, aabb.max.y, aabb.max.z, 1.0}, // X Y Z
		};

		bool inside{ false };
		for (size_t i{}; i < 8; i++)
		{
			vec4f corner = mv * corners[i];  

			inside = inside ||
				(-corner.w <= corner.x && corner.x <= corner.w) &&
				(-corner.w <= corner.y && corner.y <= corner.w) &&
				(0.f	   <= corner.z && corner.z <= corner.w);
		}

		return inside;
	}

	inline std::vector<types::loc> cull_staticAABB_frustum(
		const Camera& cam,
		const std::map<types::loc, GameWorld::Voxels::Chunk>& chunks
	) noexcept
	{
		mat4f mv{ cam.view * cam.proj };
		std::vector<types::loc> out{};

		for (const auto& i : chunks)
		{
			StaticAABB aabb{ i.second.getPos(), i.second.getOppositeCorner() };
			
			if (isWithinFrustum(mv, aabb))
				out.push_back(i.first);
		}

		return out;
	}

}