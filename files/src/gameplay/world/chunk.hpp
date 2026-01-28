#pragma once // chunk.hpp
// MIT
// Allosker ------------------------------
// =========+
// Basic chunk class responsible for everything related to gameplay that happens within it.
// ---------------------------------------

#include "uHeaders/opengl.hpp"
#include "rendering/world_managing/data/basic/cube.hpp"

namespace Gameplay::World
{
	using WorldPos = vec3f;

	class Chunk
	{
	public: 

		explicit Chunk(const vec3f& pos) noexcept;

		explicit Chunk() noexcept = default;


		const std::vector<Render::Data::Cube>& getVoxelData() const noexcept
		{
			return m_voxels;
		}

		std::vector<Render::Data::Cube>& getVoxelData() noexcept
		{
			return m_voxels;
		}

		const WorldPos getPos() const noexcept
		{
			return m_pos;
		}

		const WorldPos getOppositeCorner() const noexcept
		{
			return m_pos + 32.f;
		}

		const bool isInit() const noexcept
		{
			return m_init;
		}

		const bool isWithinChunk(const WorldPos& point) const noexcept
		{
			const WorldPos corner{ getOppositeCorner() };

			if (
			(point.x > m_pos.x && point.y > m_pos.y && point.z > m_pos.z) &&
			(point.x < corner.x && point.y < corner.y && point.z < corner.z))
				return true;

			return false;
		}


	private:




		std::vector<Render::Data::Cube> m_voxels;

		WorldPos m_pos{};

		bool m_init{ false };


	public:

		static constexpr std::uint8_t g_size{ 32 };
	};

}