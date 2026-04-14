#include "world/voxels/chunk.hpp"


// =====================
// Construction/Destruction
// =====================

GameWorld::Voxels::Chunk::Chunk(const types::loc& pos) noexcept
	: m_pos(pos), m_voxels(GameWorld::Voxels::Chunk::g_maxSize)
{
}


// =====================
// Predicates
// =====================

const bool GameWorld::Voxels::Chunk::isWithinChunk(const types::pos& point) const noexcept
{
	const types::loc corner{ getOppositeCorner() };

	if (
		(point.x > m_pos.x && point.y > m_pos.y && point.z > m_pos.z) &&
		(point.x < corner.x && point.y < corner.y && point.z < corner.z))
		return true;

	return false;
}

Render::Data::Voxel* GameWorld::Voxels::Chunk::block_at_ptr(const types::loc& loc) noexcept
{
	if ((loc.x >= g_size || loc.y >= g_size || loc.z >= g_size) || (loc.x < 0 || loc.y < 0 || loc.z < 0))
		return nullptr;

	return block_at_ptr(loc.x + loc.y * g_size + loc.z * g_size * g_size);
}

Render::Data::Voxel* GameWorld::Voxels::Chunk::block_at_ptr(const types::chunk_index& index) noexcept
{
	if (index < 0 || index >= Chunk::g_maxSize)
		return nullptr;

	return &m_voxels.at(index);
}

const Render::Data::Voxel* GameWorld::Voxels::Chunk::block_at_ptr(const types::loc& loc) const noexcept
{
	if ((loc.x >= g_size || loc.y >= g_size || loc.z >= g_size) || (loc.x < 0 || loc.y < 0 || loc.z < 0))
		return nullptr;

	return block_at_ptr(loc.x + loc.y * g_size + loc.z * g_size * g_size);
}

const Render::Data::Voxel* GameWorld::Voxels::Chunk::block_at_ptr(const types::chunk_index& index) const noexcept
{
	if (index < 0 || index >= Chunk::g_maxSize)
		return nullptr;

	return &m_voxels.at(index);
}