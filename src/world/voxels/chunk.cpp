#include "world/voxels/chunk.hpp"


// =====================
// Construction/Destruction
// =====================

GameWorld::Voxels::Chunk::Chunk(const types::loc& pos) noexcept
	: m_pos(pos), m_voxels(GameWorld::Voxels::Chunk::g_maxSize)
{
}


// =====================
// Actors
// =====================

void GameWorld::Voxels::Chunk::createPalette() noexcept
{
	const auto& palette_voxel = m_voxels.back(); 

	for (const auto& v : m_voxels)
		if (palette_voxel.id != v.id)
			return;

	m_voxels.resize(1);
	m_voxels.shrink_to_fit();

	m_isPaletteChunk = true;
}

void GameWorld::Voxels::Chunk::recreateChunkFromPalette() noexcept
{
	m_voxels.resize(GameWorld::Voxels::Chunk::g_maxSize);

	for (auto& v : m_voxels)
		v.id = m_voxels[0].id;

	m_isPaletteChunk = false;
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

	if (!m_isPaletteChunk)
		return &m_voxels.at(index);
	return &m_voxels.back();
}