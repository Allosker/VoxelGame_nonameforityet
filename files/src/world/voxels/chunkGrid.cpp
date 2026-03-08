#include "world/voxels/chunkGrid.hpp"


// =====================
// Actor
// =====================

void GameWorld::Voxels::ChunkGrid::update(const types::pos& camPos, const Render::Data::Types::VoxelTypeManager& type_manager) noexcept
{
	types::loc camLoc{
		static_cast<int64>(std::floor(camPos.x / Chunk::g_size)),
		static_cast<int64>(std::floor(camPos.y / Chunk::g_size)),
		static_cast<int64>(std::floor(camPos.z / Chunk::g_size)) };

	auto new_chunk_locs{ generate_new_chunks(camLoc) };

	for (const auto& loc : new_chunk_locs)
		create_chunkMesh_for_chunk_at(loc, camPos, type_manager);

	discard_outside_chunks(camLoc);
}

void GameWorld::Voxels::ChunkGrid::discard_outside_chunks(const types::loc& camLoc) noexcept
{
	types::loc min{ camLoc - ChunkSettings::world_render_distance };
	types::loc max{ camLoc + ChunkSettings::world_render_distance };

	for (auto it = m_chunks.begin(); it != m_chunks.end();)
	{
		const auto pos{ it->first };

		if (
			(pos.x < min.x || pos.x > max.x) ||
			(pos.y < min.y || pos.y > max.y) ||
			(pos.z < min.z || pos.z > max.z) )
		{

			it = m_chunks.erase(it);
			m_chunk_meshes.at(pos).destroy();
			m_chunk_meshes.erase(pos);
		}
		else
			++it;
	}
}

std::vector<types::loc> GameWorld::Voxels::ChunkGrid::generate_new_chunks(const types::loc& camLoc) noexcept
{
	std::vector<types::loc> locations{};

	for (int64 x{ camLoc.x - ChunkSettings::world_render_distance }; x <= camLoc.x + ChunkSettings::world_render_distance; x++)
		for (int64 y{ camLoc.y - ChunkSettings::world_render_distance }; y < camLoc.y + ChunkSettings::world_render_distance; y++)
			for (int64 z{ camLoc.z - ChunkSettings::world_render_distance }; z <= camLoc.z + ChunkSettings::world_render_distance; z++)
			{

				if (types::loc location{ x,y,z }; !m_chunks.contains(location))
				{
					m_chunks.emplace(location, GameWorld::Voxels::Chunk{ types::loc{
						location.x * static_cast<int64>(Chunk::g_size),
						location.y * static_cast<int64>(Chunk::g_size),
						location.z * static_cast<int64>(Chunk::g_size)} });

					locations.push_back(location);
				}

			}

	return locations;
}

void GameWorld::Voxels::ChunkGrid::draw_all() const noexcept
{
	for (const auto& chunk_mesh : m_chunk_meshes)
		chunk_mesh.second.draw();
}


// =====================
// Predicates
// =====================-

bool GameWorld::Voxels::ChunkGrid::is_empty(const types::pos& block_pos, const Render::Data::Types::VoxelTypeManager& type_manager) const noexcept
{
	auto* c{ chunk_at(block_pos) };

	if (c == nullptr)
		return true;

	return !c->block_at(getVoxelIndex(block_pos)).id || !type_manager.getType(c->block_at(getVoxelIndex(block_pos)).id).is_destructible;
}


// =====================
// Mutators
// =====================

void GameWorld::Voxels::ChunkGrid::create_chunkMesh_for_chunk_at(const types::loc& key, const types::pos& camPos, const Render::Data::Types::VoxelTypeManager& type_manager)
{
	m_chunk_meshes.emplace(key, Render::Data::ChunkMesh{ m_chunks.at(key), camPos, type_manager });
}


// =====================
// Getters
// =====================

GameWorld::Voxels::Chunk& GameWorld::Voxels::ChunkGrid::chunk_at_loc(const types::loc& loc) noexcept
{
	return m_chunks.at(loc);
}

const GameWorld::Voxels::Chunk& const GameWorld::Voxels::ChunkGrid::chunk_at_loc(const types::loc& loc) const noexcept
{
	return m_chunks.at(loc);
}

Render::Data::ChunkMesh& GameWorld::Voxels::ChunkGrid::chunkmesh_at_loc(const types::loc& loc) noexcept
{
	return m_chunk_meshes.at(loc);
}

const Render::Data::ChunkMesh& GameWorld::Voxels::ChunkGrid::chunkmesh_at_loc(const types::loc& loc) const noexcept
{
	return m_chunk_meshes.at(loc);
}

GameWorld::Voxels::Chunk* GameWorld::Voxels::ChunkGrid::chunk_at(const types::pos& pos) noexcept
{
	if (const auto& loc = to_loc(pos); loc)
		return &m_chunks.at(*loc);

	return nullptr;
}

const GameWorld::Voxels::Chunk const* GameWorld::Voxels::ChunkGrid::chunk_at(const types::pos& pos) const noexcept
{
	if (const auto& loc = to_loc(pos); loc)
		return &m_chunks.at(*loc);

	return nullptr;
}

Render::Data::ChunkMesh* GameWorld::Voxels::ChunkGrid::chunkmesh_at(const types::pos& pos) noexcept
{
	if (const auto& loc = to_loc(pos); loc)
		return &m_chunk_meshes.at(*loc);

	return nullptr;
}

const Render::Data::ChunkMesh const* GameWorld::Voxels::ChunkGrid::chunkmesh_at(const types::pos& pos) const noexcept
{
	if (const auto& loc = to_loc(pos); loc)
		return &m_chunk_meshes.at(*loc);

	return nullptr;
}

types::chunk_index GameWorld::Voxels::ChunkGrid::getVoxelIndex(const types::pos& pos) const
{
	auto fpos = pos - static_cast<types::pos>(chunk_at(pos)->getPos());

	const auto z_stride{ Chunk::g_size * Chunk::g_size };
	auto index = (uint32)(std::abs(std::floor(fpos.z) * z_stride + std::floor(fpos.y) * Chunk::g_size + std::floor(fpos.x)));

	return index;
}

/*private*/ std::optional<types::loc> GameWorld::Voxels::ChunkGrid::to_loc(const types::pos& camPos) const noexcept
{
	const types::loc camLoc{
	static_cast<int>(std::floor(camPos.x / Chunk::g_size)),
	static_cast<int>(std::floor(camPos.y / Chunk::g_size)),
	static_cast<int>(std::floor(camPos.z / Chunk::g_size)) };

	return m_chunks.contains(camLoc) ? std::make_optional(camLoc) : std::nullopt;
}





