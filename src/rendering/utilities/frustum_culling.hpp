#pragma once //frustum_culling.hpp
// MIT
// Allosker ------------------------------
// =========+
// Define frustum culling for CHUNKS only (as of now)
// ---------------------------------------

#include "utilities/camera.hpp"
#include "world/chunks/chunk.hpp"
#include <map>
#include <unordered_map>

#include "rendering/debug/debugRenderer.hpp"

namespace render::utils
{

	struct Plan
	{
		vec3f normal{};
		float distance{};
	};
	
	struct Frustum
	{
		Frustum() noexcept = default;

		Plan left;
		Plan right;
		Plan bottom;
		Plan top;
		Plan near;
		Plan far;
	};

	struct StaticAABB
	{
		vec3f center{ 0.f, 0.f, 0.f };
		float extent{ 0.f };

		StaticAABB(const vec3f& inCenter, float inExtent)
			: center{ inCenter }, extent{ inExtent }
		{
		}

		bool isOnOrForwardPlan(const Plan& plan) const
		{
			// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
			const float r = extent * std::abs(plan.normal.x) + extent * std::abs(plan.normal.y) + extent * std::abs(plan.normal.z);
			return -r <= plan.normal.dot(center) - plan.distance;
		}

		bool isOnFrustum(const Frustum& camFrustum) const
		{


			return (
				isOnOrForwardPlan(camFrustum.left) &&
				isOnOrForwardPlan(camFrustum.right) &&
				isOnOrForwardPlan(camFrustum.top) &&
				isOnOrForwardPlan(camFrustum.bottom) &&
				isOnOrForwardPlan(camFrustum.near) &&
				isOnOrForwardPlan(camFrustum.far));
		};
	};

	inline std::vector<types::loc> getChunksVisibleInView(
		const mat4f& vp,
		const std::unordered_map<types::loc, chunks::Chunk>& chunks) noexcept
	{
		Frustum     frustum;

		for (int i = 3; i--;) { frustum.left.normal[i] = vp[i][3] + vp[i][0]; }
		for (int i = 3; i--;) { frustum.right.normal[i] = vp[i][3] - vp[i][0]; }
		for (int i = 3; i--;) { frustum.bottom.normal[i] = vp[i][3] + vp[i][1]; }
		for (int i = 3; i--;) { frustum.top.normal[i] = vp[i][3] - vp[i][1]; }
		for (int i = 3; i--;) { frustum.near.normal[i] = vp[i][3] + vp[i][2]; }
		for (int i = 3; i--;) { frustum.far.normal[i] = vp[i][3] - vp[i][2]; }

		frustum.left.distance = vp[3][3] + vp[3][0];
		frustum.right.distance = vp[3][3] - vp[3][0];
		frustum.bottom.distance = vp[3][3] + vp[3][1];
		frustum.top.distance = vp[3][3] - vp[3][1];
		frustum.near.distance = vp[3][3] + vp[3][2];
		frustum.far.distance = vp[3][3] - vp[3][2];


		frustum.left.distance /= -frustum.left.normal.length();
		frustum.right.distance /= -frustum.right.normal.length();
		frustum.bottom.distance /= -frustum.bottom.normal.length();
		frustum.top.distance /= -frustum.top.normal.length();
		frustum.near.distance /= -frustum.near.normal.length();
		frustum.far.distance /= -frustum.far.normal.length();

		frustum.left.normal = frustum.left.normal.normal();
		frustum.right.normal = frustum.right.normal.normal();
		frustum.bottom.normal = frustum.bottom.normal.normal();
		frustum.top.normal = frustum.top.normal.normal();
		frustum.near.normal = frustum.near.normal.normal();
		frustum.far.normal = frustum.far.normal.normal();


		std::vector<types::loc> visible_chunks{};
		for (const auto& i : chunks)
		{
			StaticAABB aabb{ i.second.getPos() + i.second.g_size / 2.f, i.second.g_size / 2.f };
					

			if (aabb.isOnFrustum(frustum))
				visible_chunks.push_back(i.first);
		}

		return visible_chunks;
	}
} // render::utils