#include "world/chunks/chunk.hpp"

// =====================
// Construction/Destruction
// =====================

chunks::Chunk::Chunk(const types::loc& pos) noexcept
	: m_pos(pos), m_voxels(g_maxSize)
{
}


// =====================
// Getters
// =====================

types::block_index chunks::Chunk::getVoxelIndex(const types::pos& pos) const
{
	const auto floored_pos = static_cast<types::loc>(mpml::floor(pos)) - m_pos;
	const auto z_stride{ chunks::Chunk::g_size * chunks::Chunk::g_size };
	const auto index = static_cast<uint32>(floored_pos.z * z_stride + floored_pos.y * chunks::Chunk::g_size + floored_pos.x);

	return index;
}

// =====================
// Predicates
// =====================

const bool chunks::Chunk::is_within_chunk(const types::pos& point) const noexcept
{
	const types::loc corner{ getOppositeCorner() };

	if (
		(point.x > m_pos.x && point.y > m_pos.y && point.z > m_pos.z) &&
		(point.x < corner.x && point.y < corner.y && point.z < corner.z))
		return true;

	return false;
}

Data::Voxel* chunks::Chunk::block_at_ptr(const types::loc& loc) noexcept
{
	if ((loc.x >= g_size || loc.y >= g_size || loc.z >= g_size) || (loc.x < 0 || loc.y < 0 || loc.z < 0))
		return nullptr;

	return block_at_ptr(loc.x + loc.y * g_size + loc.z * g_size * g_size);
}

Data::Voxel* chunks::Chunk::block_at_ptr(types::block_index index) noexcept
{
	if (index < 0 || index >= chunks::Chunk::g_maxSize)
		return nullptr;

	if (m_empty)
		return &m_voxels.front();

	return &m_voxels.at(index);
}

const Data::Voxel* chunks::Chunk::block_at_ptr(const types::loc& loc) const noexcept
{
	if ((loc.x >= g_size || loc.y >= g_size || loc.z >= g_size) || (loc.x < 0 || loc.y < 0 || loc.z < 0))
		return nullptr;

	return block_at_ptr(loc.x + loc.y * g_size + loc.z * g_size * g_size);
}

const Data::Voxel* chunks::Chunk::block_at_ptr(types::block_index index) const noexcept
{
	if (index < 0 || index >= chunks::Chunk::g_maxSize || m_empty)
		return nullptr;

	if (m_empty)
		return &m_voxels.front();

	return &block_at(index);
}


// =====================
// Actors
// =====================


void chunks::Chunk::make_empty() noexcept
{
	if (!m_empty)
	{
		bool should_not_be_resized{ false };
		for (const auto& i : m_voxels)
			if (i.id)
			{
				should_not_be_resized = true;
				break;
			}

		if (!should_not_be_resized)
		{
			m_voxels.resize(1);
			m_voxels.shrink_to_fit();

			m_empty = true;
		}
	}
}

void chunks::Chunk::make_full() noexcept
{
	if (m_empty)
	{
		m_voxels.resize(g_maxSize);
		m_empty = false;
	}
}
