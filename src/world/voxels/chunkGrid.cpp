#include "world/voxels/chunkGrid.hpp"

#include "rendering/world_managing/data/chunk/chunkMesh.hpp"

namespace Render::Data
{
	class ChunkMesh;
}

// =====================
// Actor
// =====================

void GameWorld::Voxels::ChunkGrid::update(const Render::Data::Types::VoxelTypeManager& type_manager, const types::pos& playerPos) noexcept
{
	bool shouldBreak{};

	for (auto& c : m_chunk_meshes)
	{

		if (c.second.flags.dirty)
		{
			c.second.buildMesh(chunk_at_loc(c.first), type_manager, *this);
			c.second.updateBuffers(playerPos);
			c.second.flags.dirty = false;
			shouldBreak = true;
		}

		if (shouldBreak)
			break;
	}

}

void GameWorld::Voxels::ChunkGrid::discard_outside_chunks(const types::loc& camLoc) noexcept
{
	types::loc min{ camLoc - ChunkSettings::world_render_distance };
	types::loc max{ camLoc + ChunkSettings::world_render_distance };

	for (auto it = m_chunks.begin(); it != m_chunks.end();)
	{
		const auto loc{ it->first };

		if (
			(loc.x < min.x || loc.x > max.x) ||
			(loc.y < min.y || loc.y > max.y) ||
			(loc.z < min.z || loc.z > max.z) )
		{

			it = m_chunks.erase(it);
			m_chunk_meshes.at(loc).destroy();
			m_chunk_meshes.erase(loc);
		}
		else
			++it;
	}
}

void GameWorld::Voxels::ChunkGrid::discard_all_chunks() noexcept
{
	for (auto& key : m_chunk_meshes)
		key.second.destroy();

	std::map<types::loc, GameWorld::Voxels::Chunk> empty_chunks{};
	m_chunks.swap(empty_chunks);

	std::map<types::loc, Render::Data::ChunkMesh> empty_chunk_meshes{};
	m_chunk_meshes.swap(empty_chunk_meshes);
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


					if (auto c{ chunkmesh_at_loc_ptr(location + types::loc{1, 0, 0}) })
						c->flags.dirty = true;

					if (auto c{ chunkmesh_at_loc_ptr(location - types::loc{1, 0, 0}) })
						c->flags.dirty = true;

					if (auto c{ chunkmesh_at_loc_ptr(location + types::loc{0, 1, 0}) })
						c->flags.dirty = true;

					if (auto c{ chunkmesh_at_loc_ptr(location - types::loc{0, 1, 0}) })
						c->flags.dirty = true;
				
					if (auto c{ chunkmesh_at_loc_ptr(location + types::loc{0, 0, 1}) })
						c->flags.dirty = true;

					if (auto c{ chunkmesh_at_loc_ptr(location - types::loc{0, 0, 1}) })
						c->flags.dirty = true;
				}

			}

	return locations;
}

void GameWorld::Voxels::ChunkGrid::draw_all(const types::loc& playerLoc) const noexcept
{
	std::vector<types::loc> chunk_transparents{};
	std::vector<types::loc> chunks{};

	for (const auto& i : m_chunk_meshes)
		if (i.second.flags.has_transparency)
			chunk_transparents.emplace_back(i.first);
		else
			chunks.emplace_back(i.first);

	std::sort(chunk_transparents.begin(), chunk_transparents.end(), [&](const auto& a, const auto& b)
		{
			return
				vec3f{ a - playerLoc }.length_squared() >
				vec3f{ b - playerLoc }.length_squared();
		});

	for (const auto& loc : chunks)
		m_chunk_meshes.at(loc).draw();

	for(const auto& loc : chunk_transparents)
		m_chunk_meshes.at(loc).draw();
}


// =====================
// Predicates
// =====================-

bool GameWorld::Voxels::ChunkGrid::is_empty(const types::pos& block_pos, const Render::Data::Types::VoxelTypeManager& type_manager) const noexcept
{
	auto* c{ chunk_at(block_pos) };

	if (c == nullptr)
		return true;

	auto index = getVoxelIndex(block_pos);

	if (!c->block_at_ptr(index))
		return true;

	return !c->block_at(index).id || !type_manager.getType(c->block_at(index).id).is_destructible;
}


// =====================
// Mutators
// =====================

void GameWorld::Voxels::ChunkGrid::create_chunkMesh_for_chunk_at(const types::loc& key)
{
	m_chunk_meshes.emplace(key, Render::Data::ChunkMesh{});
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

Render::Data::ChunkMesh* GameWorld::Voxels::ChunkGrid::chunkmesh_at_loc_ptr(const types::loc& loc) noexcept
{
	return m_chunk_meshes.contains(loc) ? &m_chunk_meshes.at(loc) : nullptr;
}

const Render::Data::ChunkMesh* GameWorld::Voxels::ChunkGrid::chunkmesh_at_loc_ptr(const types::loc& loc) const noexcept
{
	return m_chunk_meshes.contains(loc) ? &m_chunk_meshes.at(loc) : nullptr;
}

GameWorld::Voxels::Chunk* GameWorld::Voxels::ChunkGrid::chunk_at(const types::pos& pos) noexcept
{
	if (const auto& loc = to_loc_opt(pos); loc)
		return &m_chunks.at(*loc);

	return nullptr;
}

const GameWorld::Voxels::Chunk const* GameWorld::Voxels::ChunkGrid::chunk_at(const types::pos& pos) const noexcept
{
	if (const auto& loc = to_loc_opt(pos); loc)
		return &m_chunks.at(*loc);

	return nullptr;
}

Render::Data::ChunkMesh* GameWorld::Voxels::ChunkGrid::chunkmesh_at(const types::pos& pos) noexcept
{
	if (const auto& loc = to_loc_opt(pos); loc)
		return &m_chunk_meshes.at(*loc);

	return nullptr;
}

const Render::Data::ChunkMesh const* GameWorld::Voxels::ChunkGrid::chunkmesh_at(const types::pos& pos) const noexcept
{
	if (const auto& loc = to_loc_opt(pos); loc)
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

types::loc GameWorld::Voxels::ChunkGrid::to_loc(const types::pos& pos) noexcept
{
	return { 
		static_cast<int32>(std::floor(pos.x / Chunk::g_size)),
		static_cast<int32>(std::floor(pos.y / Chunk::g_size)),
		static_cast<int32>(std::floor(pos.z / Chunk::g_size)) };
}

/*private*/ std::optional<types::loc> GameWorld::Voxels::ChunkGrid::to_loc_opt(const types::pos& camPos) const noexcept
{
	const types::loc camLoc{ ChunkGrid::to_loc(camPos) };

	return m_chunks.contains(camLoc) ? std::make_optional(camLoc) : std::nullopt;
}





