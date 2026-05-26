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

	struct Plan
	{
		vec3f normal{};
		float distance{};

		Plan(const vec3f& p1, const vec3f& norm)
			: normal(norm.normal()),
			distance(normal.dot(p1))
		{
		}

		void normalize()
		{
			auto len = normal.length();

			normal /= len;
			distance /= len;
		}
	};
	
	struct Frustum
	{
		Frustum() noexcept = default;

		Plan left{ {}, {} };
		Plan right{ {}, {} };
		Plan bottom{ {}, {} };
		Plan top{ {}, {} };
		Plan near{ {}, {} };
		Plan far{ {}, {} };
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
			const float r = extent * (std::abs(plan.normal.x) + std::abs(plan.normal.y) + std::abs(plan.normal.z));
			auto e = plan.normal.dot(center) - plan.distance;
			return -r <= e;
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

	inline std::vector<types::loc> createViewFrustum(
		const Camera& cam,
		const std::map<types::loc, GameWorld::Voxels::Chunk>& chunks,
		std::vector<std::vector<bool>>& c) noexcept
	{
		Frustum     frustum;
		/*const float halfVSide = 1000.f * tanf(mpml::Angle<>::fromDegrees(90).asRadians() * .5f);
		const float halfHSide = halfVSide * 2560 / 1440;
		const vec3f frontMultFar = 1000.f * cam.front_dir;

		frustum.near = { cam.pos + 0.1f * cam.front_dir, cam.front_dir };
		frustum.far = { cam.pos + frontMultFar, -cam.front_dir };
		frustum.right = { cam.pos, cam.up_dir.cross(frontMultFar + cam.right_dir() * halfHSide) };
		frustum.left = { cam.pos, (frontMultFar - cam.right_dir() * halfHSide).cross(cam.up_dir)};
		frustum.top = { cam.pos, cam.right_dir().cross(frontMultFar - cam.up_dir * halfVSide) };
		frustum.bottom = { cam.pos, (frontMultFar + cam.up_dir * halfVSide).cross(cam.right_dir())};*/

		auto vp = cam.view * cam.proj;

		for (int i = 3; i--; ) { frustum.left.normal[i]		= vp[3][i] + vp[0][i]; }
		for (int i = 3; i--; ) { frustum.right.normal[i]	= vp[3][i] - vp[0][i]; }
		for (int i = 3; i--; ) { frustum.bottom.normal[i]	= vp[3][i] + vp[1][i]; }
		for (int i = 3; i--; ) { frustum.top.normal[i]		= vp[3][i] - vp[1][i]; }
		for (int i = 3; i--; ) { frustum.near.normal[i]		= vp[3][i] + vp[2][i]; }
		for (int i = 3; i--; ) { frustum.far.normal[i]		= vp[3][i] - vp[2][i]; }

		frustum.left.distance	= vp[3][3] + vp[0][3];
		frustum.right.distance	= vp[3][3] - vp[0][3];
		frustum.bottom.distance = vp[3][3] + vp[1][3]; 
		frustum.top.distance	= vp[3][3] - vp[1][3];
		frustum.near.distance	= vp[3][3] + vp[2][3];
		frustum.far.distance	= vp[3][3] - vp[2][3];

		frustum.left.normalize();
		frustum.right.normalize();
		frustum.bottom.normalize();
		frustum.top.normalize();
		frustum.near.normalize();
		frustum.far.normalize();

		cam.front_dir;

		std::vector<types::loc> visible_chunks{};
		for (const auto& i : chunks)
		{
			StaticAABB aabb{ i.second.getPos(), i.second.g_size / 2.f };
					
			c.push_back({});

			if (aabb.isOnFrustum(frustum))
				visible_chunks.push_back(i.first);
		}

		return visible_chunks;
	}



	/*
	//inline bool isWithinFrustum(const mat4f& mv, const StaticAABB& aabb, 
	//	std::vector<std::vector<bool>>& c
	//	) noexcept
	//{
	//	vec4f corners[8] = {
	//		{aabb.min.x, aabb.min.y, aabb.min.z, 1.0}, // base corner
	//		{aabb.max.x, aabb.min.y, aabb.min.z, 1.0}, // lower left
	//		{aabb.min.x, aabb.max.y, aabb.min.z, 1.0}, // upper base
	//		{aabb.max.x, aabb.max.y, aabb.min.z, 1.0}, // upper left

	//		{aabb.min.x, aabb.min.y, aabb.max.z, 1.0}, // front
	//		{aabb.max.x, aabb.min.y, aabb.max.z, 1.0}, // left front
	//		{aabb.min.x, aabb.max.y, aabb.max.z, 1.0}, // upper front
	//		{aabb.max.x, aabb.max.y, aabb.max.z, 1.0}, // opposite
	//	};

	//	bool inside{ false };
	//	for (size_t i{}; i < 8; i++)
	//	{
	//		vec4f corner = mv * corners[i];  

	//		inside = inside || (
	//			(-corner.w <= corner.x && corner.x <= corner.w) &&
	//			(-corner.w <= corner.y && corner.y <= corner.w) &&
	//			(-corner.w <= corner.z && corner.z <= corner.w));

	//		c.back().push_back(
	//			(-corner.w <= corner.x && corner.x <= corner.w) &&
	//			(-corner.w <= corner.y && corner.y <= corner.w) &&
	//			(-corner.w <= corner.z && corner.z <= corner.w));
	//	}

	//	return inside;
	//}

	//inline std::vector<types::loc> cull_staticAABB_frustum(
	//	const Camera& cam,
	//	const std::map<types::loc, GameWorld::Voxels::Chunk>& chunks,
	//	std::vector<std::vector<bool>>& c
	//) noexcept
	//{
	//	mat4f mv{ cam.view * cam.proj };
	//	std::vector<types::loc> out{};

	//	for (const auto& i : chunks)
	//	{
	//		StaticAABB aabb{ i.second.getPos(), i.second.getOppositeCorner() };
	//		
	//		c.push_back({});

	//		if (isWithinFrustum(mv, aabb, c))
	//			out.push_back(i.first);
	//	}

	//	return out;
	//}
	*/



}