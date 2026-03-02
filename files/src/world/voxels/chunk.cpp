#include "world/voxels/chunk.hpp"


// =====================
// Construction/Destruction
// =====================

World::Voxels::Chunk::Chunk(const vec3f& pos) noexcept
	: m_pos(pos), m_voxels(32'768)
{
	for (size_t x{}; x < 32; x++)
		for (size_t y{}; y < 32; y++)
			for (size_t z{}; z < 32; z++)
			{
				auto& current_block{ m_voxels.at(static_cast<std::int32_t>((z * 32.f * 32.f) + (y * World::Voxels::Chunk::g_size) + x)) };

				if (y >= 16)
					current_block.filling = Render::Data::Voxel::Empty;

				if (y == 15)
					current_block.updateSpritePos(Render::Data::VoxelType::c_dirtGrass);
				else
					current_block.updateSpritePos(Render::Data::VoxelType::c_dirt);
			}
}


// =====================
// Predicates
// =====================

const bool World::Voxels::Chunk::isWithinChunk(const types::pos& point) const noexcept
{
	const types::loc corner{ getOppositeCorner() };

	if (
		(point.x > m_pos.x && point.y > m_pos.y && point.z > m_pos.z) &&
		(point.x < corner.x && point.y < corner.y && point.z < corner.z))
		return true;

	return false;
}
