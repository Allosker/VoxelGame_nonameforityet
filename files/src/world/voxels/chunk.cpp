#include "world/voxels/chunk.hpp"


// =====================
// Construction/Destruction
// =====================

GameWorld::Voxels::Chunk::Chunk(const types::loc& pos) noexcept
	: m_pos(pos), m_voxels(32'768)
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
