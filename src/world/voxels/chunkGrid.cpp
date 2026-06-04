#include "world/voxels/chunkGrid.hpp"

#include "rendering/world_managing/data/chunk/chunkMesh.hpp"
#include "world/players/player/player.hpp"
#include "world/world.hpp"

namespace Render::Data
{
	class ChunkMesh;
}

// =====================
// Actor
// =====================

GameWorld::Voxels::ChunkGrid::ChunkGrid() noexcept
{
}

void GameWorld::Voxels::ChunkGrid::update(const World& world, const Render::Data::Types::VoxelTypeManager& type_manager, const Player& player) noexcept
{
	bool shouldBreak{};

	for (auto& [k, c] : m_chunks)
	{
		if (!c.isEmpty() && c.generated_sunlight)
		{
			if (!m_chunk_meshes.contains(k))
				create_chunkMesh_for_chunk_at(k);

			if (m_chunk_meshes[k].flags.dirty)
			{
				m_chunk_meshes[k].updateBuffers(m_chunk_meshes[k].buildMesh(chunk_at_loc(k), type_manager, world), player.getPos());
				m_chunk_meshes[k].flags.dirty = false;
				shouldBreak = true;
			}
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
			if (m_chunk_meshes.contains(loc))
			{
				m_chunk_meshes.at(loc).free();
				m_chunk_meshes.erase(loc);
			}
		}
		else
			++it;
	}
}

void GameWorld::Voxels::ChunkGrid::discard_all_chunks() noexcept
{
	for (auto& key : m_chunk_meshes)
		key.second.free();

	std::map<types::loc, GameWorld::Voxels::Chunk> empty_chunks{};
	m_chunks.swap(empty_chunks);

	std::map<types::loc, Render::Data::ChunkMesh> empty_chunk_meshes{};
	m_chunk_meshes.swap(empty_chunk_meshes);
}

std::vector<types::loc> GameWorld::Voxels::ChunkGrid::generate_new_chunks(const types::loc& camLoc) noexcept
{
	std::vector<types::loc> locations{};

	if (ChunkSettings::world_render_distance == 0)
	{
		m_chunks.emplace(
			camLoc, GameWorld::Voxels::Chunk{ types::loc{
			camLoc.x * static_cast<int64>(Chunk::g_size),
			camLoc.y * static_cast<int64>(Chunk::g_size),
			camLoc.z * static_cast<int64>(Chunk::g_size)} 
		});

		locations.push_back(camLoc);
	}
	else
		for (int64 x{ camLoc.x - ChunkSettings::world_render_distance }; x <= camLoc.x + ChunkSettings::world_render_distance; x++)
			for (int64 y{ camLoc.y - ChunkSettings::world_render_distance }; y <= camLoc.y + ChunkSettings::world_render_distance; y++)
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

void GameWorld::Voxels::ChunkGrid::draw_all(const GameWorld::Player& player) const noexcept
{
	static std::vector<types::loc> visible_chunks{};

	if (!player.getCamera().free)
		visible_chunks = Render::Utils::createViewFrustum(player.getCamera(), m_chunks);

	// Sort transparent chunks to draw them last 
	std::vector<types::loc> chunk_transparents{};
	std::vector<types::loc> chunks{};

	for (const auto& i : visible_chunks)
		if(m_chunk_meshes.contains(i))
		{
			if (m_chunk_meshes.at(i).flags.has_transparency)
				chunk_transparents.emplace_back(i);
			else
				chunks.emplace_back(i);
		}

	auto playerLoc = GameWorld::Voxels::ChunkGrid::to_loc(player.getPos());
	std::sort(chunk_transparents.begin(), chunk_transparents.end(), [&](const auto& a, const auto& b)
		{
			return
				vec3f{ a - playerLoc }.length_squared() >
				vec3f{ b - playerLoc }.length_squared();
		});

	for (const auto& loc : chunks)
		m_chunk_meshes.at(loc).draw();

	for (const auto& loc : chunk_transparents)
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


GameWorld::Voxels::Chunk* GameWorld::Voxels::ChunkGrid::chunk_at_loc_ptr(const types::loc& loc) noexcept
{
	return m_chunks.contains(loc) ? &m_chunks.at(loc) : nullptr;
}

const GameWorld::Voxels::Chunk* GameWorld::Voxels::ChunkGrid::chunk_at_loc_ptr(const types::loc& loc) const noexcept
{
	return m_chunks.contains(loc) ? &m_chunks.at(loc) : nullptr;
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
	if (const auto& loc = to_loc_opt_chunks(pos); loc)
		return &m_chunks.at(*loc);

	return nullptr;
}

const GameWorld::Voxels::Chunk const* GameWorld::Voxels::ChunkGrid::chunk_at(const types::pos& pos) const noexcept
{
	if (const auto& loc = to_loc_opt_chunks(pos); loc)
		return &m_chunks.at(*loc);

	return nullptr;
}

Render::Data::ChunkMesh* GameWorld::Voxels::ChunkGrid::chunkmesh_at(const types::pos& pos) noexcept
{
	if (const auto& loc = to_loc_opt_chunkMeshes(pos); loc)
		return &m_chunk_meshes.at(*loc);

	return nullptr;
}

const Render::Data::ChunkMesh const* GameWorld::Voxels::ChunkGrid::chunkmesh_at(const types::pos& pos) const noexcept
{
	if (const auto& loc = to_loc_opt_chunkMeshes(pos); loc)
		return &m_chunk_meshes.at(*loc);

	return nullptr;
}

types::type_id GameWorld::Voxels::ChunkGrid::block_id_at(const types::pos& block_pos) const noexcept
{
	const Chunk* chunk{ chunk_at(block_pos) };

	if (!chunk)
		return {};

	return chunk->block_at(getVoxelIndex(block_pos)).id;
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

/*private*/ std::optional<types::loc> GameWorld::Voxels::ChunkGrid::to_loc_opt_chunks(const types::pos& camPos) const noexcept
{
	const types::loc camLoc{ ChunkGrid::to_loc(camPos) };

	return m_chunks.contains(camLoc) ? std::make_optional(camLoc) : std::nullopt;
}

/*private*/ std::optional<types::loc> GameWorld::Voxels::ChunkGrid::to_loc_opt_chunkMeshes(const types::pos& camPos) const noexcept
{
	const types::loc camLoc{ ChunkGrid::to_loc(camPos) };

	return m_chunk_meshes.contains(camLoc) ? std::make_optional(camLoc) : std::nullopt;
}







