#pragma once // chunk.hpp
// MIT
// Allosker ------------------------------
// =========+
// Basic chunk class responsible for everything related to gameplay that happens within it.
// ---------------------------------------

#include "uHeaders/types.hpp"
#include "rendering/world_managing/data/basic/cube.hpp"

namespace Gameplay::World
{

	class Chunk
	{
	public: 

		explicit Chunk(const vec3f& pos) noexcept;

		explicit Chunk() noexcept = default;

		Chunk(Chunk&&) noexcept = default;
		Chunk& operator=(Chunk&&) noexcept = default;

		Chunk(const Chunk&) = delete;
		Chunk& operator=(const Chunk&) = delete;


		const std::vector<Render::Data::Cube>& getVoxelData() const noexcept
		{
			return m_voxels;
		}

		std::vector<Render::Data::Cube>& getVoxelData() noexcept
		{
			return m_voxels;
		}

		const types::loc getPos() const noexcept
		{
			return m_pos;
		}

		const types::loc getOppositeCorner() const noexcept
		{
			return m_pos + 32;
		}

		const bool isWithinChunk(const types::loc& point) const noexcept
		{
			const types::loc corner{ getOppositeCorner() };

			if (
			(point.x > m_pos.x && point.y > m_pos.y && point.z > m_pos.z) &&
			(point.x < corner.x && point.y < corner.y && point.z < corner.z))
				return true;

			return false;
		}


	private:




		std::vector<Render::Data::Cube> m_voxels;

		
		types::loc m_pos{};

		

	public:

		static constexpr std::uint8_t g_size{ 32 };
	};

}