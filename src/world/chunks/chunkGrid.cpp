#include "world/chunks/chunkGrid.hpp"

#include "rendering/chunkMesh/chunkMesh.hpp"
#include "world/entities/player/player.hpp"
#include "world/world.hpp"

namespace render::Data
{
	class ChunkMesh;
}

// =====================
// Actor
// =====================

chunks::ChunkGrid::ChunkGrid() noexcept
{
}

void chunks::ChunkGrid::update(const World& world, const VoxelTypeManager& type_manager, const entities::Player& player) noexcept
{
	bool shouldBreak{};

	for (auto& [k, c] : m_chunks)
	{
		if (!c.isEmpty() /*&& c.generated_sunlight*/)
		{
			if (!m_chunk_meshes.contains(k))
				create_chunkMesh_for_chunk_at(k);

			if (m_chunk_meshes[k].flags.dirty)
			{
				m_chunk_meshes[k].updateBuffers(m_chunk_meshes[k].buildMesh(chunk_at_loc(k), type_manager, world), player.getPosition());
				m_chunk_meshes[k].flags.dirty = false;
				shouldBreak = true;
			}
		}

		if (shouldBreak)
			break;
	}
}

void chunks::ChunkGrid::discard_chunks(const types::loc& playerLoc) noexcept
{
	types::loc min{ playerLoc - ChunkSettings::world_render_distance };
	types::loc max{ playerLoc + ChunkSettings::world_render_distance };

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

void chunks::ChunkGrid::discard_all_chunks() noexcept
{
	for (auto& key : m_chunk_meshes)
		key.second.free();

	std::unordered_map<types::loc, chunks::Chunk> empty_chunks{};
	m_chunks.swap(empty_chunks);

	std::unordered_map<types::loc, render::ChunkMesh> empty_chunk_meshes{};
	m_chunk_meshes.swap(empty_chunk_meshes);
}

std::vector<types::loc> chunks::ChunkGrid::allocate_chunks(const types::loc& playerLoc) noexcept
{
	std::vector<types::loc> locations{};

	if (ChunkSettings::world_render_distance == 0)
	{
		m_chunks.emplace(
			playerLoc, chunks::Chunk{ types::loc{
			playerLoc.x * static_cast<int64>(chunks::Chunk::g_size),
			playerLoc.y * static_cast<int64>(chunks::Chunk::g_size),
			playerLoc.z * static_cast<int64>(chunks::Chunk::g_size)}
		});

		locations.push_back(playerLoc);
	}
	else
	{
		vec3i min{ playerLoc - ChunkSettings::world_render_distance };
		vec3i max{ playerLoc + ChunkSettings::world_render_distance };

		for (int64 x{ min.x }; x <= max.x; x++)
			for (int64 y{ min.y }; y <= max.y; y++)
				for (int64 z{ min.z }; z <= max.z; z++)
				{
					if (types::loc location{ x,y,z }; !m_chunks.contains(location))
					{
						const auto& chunk = m_chunks.emplace(location, chunks::Chunk{ types::loc{
							location.x * static_cast<int64>(chunks::Chunk::g_size),
							location.y * static_cast<int64>(chunks::Chunk::g_size),
							location.z * static_cast<int64>(chunks::Chunk::g_size)} }).first->second;

						locations.push_back(location);


						if (!chunk.isEmpty())
						{
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

				}
	}

	return locations;
}

void chunks::ChunkGrid::draw_all(const render::utils::Camera& cam, const entities::Player& player) const noexcept
{
	static std::vector<types::loc> visible_chunks{};

	if (!cam.free)
		visible_chunks = render::utils::createViewFrustum(cam, m_chunks);

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

	auto playerLoc = chunks::ChunkGrid::to_loc(player.getPosition());
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

bool chunks::ChunkGrid::is_empty(const types::pos& block_pos, const VoxelTypeManager& type_manager) const noexcept
{
	auto* c{ chunk_at(block_pos) };

	if (c == nullptr)
		return true;

	auto index = c->getVoxelIndex(block_pos);

	if (!c->block_at_ptr(index))
		return true;

	return !c->block_at(index).id || !type_manager.getType(c->block_at(index).id).is_destructible;
}


// =====================
// Mutators
// =====================

void chunks::ChunkGrid::create_chunkMesh_for_chunk_at(const types::loc& key)
{
	m_chunk_meshes.try_emplace(key, render::ChunkMesh{});
}


// =====================
// Getters
// =====================

chunks::Chunk& chunks::ChunkGrid::chunk_at_loc(const types::loc& loc) noexcept
{
	return m_chunks.at(loc);
}

const chunks::Chunk& const chunks::ChunkGrid::chunk_at_loc(const types::loc& loc) const noexcept
{
	return m_chunks.at(loc);
}


chunks::Chunk* chunks::ChunkGrid::chunk_at_loc_ptr(const types::loc& loc) noexcept
{
	return m_chunks.contains(loc) ? &m_chunks.at(loc) : nullptr;
}

const chunks::Chunk* chunks::ChunkGrid::chunk_at_loc_ptr(const types::loc& loc) const noexcept
{
	return m_chunks.contains(loc) ? &m_chunks.at(loc) : nullptr;
}

render::ChunkMesh& chunks::ChunkGrid::chunkmesh_at_loc(const types::loc& loc) noexcept
{
	return m_chunk_meshes.at(loc);
}

const render::ChunkMesh& chunks::ChunkGrid::chunkmesh_at_loc(const types::loc& loc) const noexcept
{
	return m_chunk_meshes.at(loc);
}

render::ChunkMesh* chunks::ChunkGrid::chunkmesh_at_loc_ptr(const types::loc& loc) noexcept
{
	return m_chunk_meshes.contains(loc) ? &m_chunk_meshes.at(loc) : nullptr;
}

const render::ChunkMesh* chunks::ChunkGrid::chunkmesh_at_loc_ptr(const types::loc& loc) const noexcept
{
	return m_chunk_meshes.contains(loc) ? &m_chunk_meshes.at(loc) : nullptr;
}

chunks::Chunk* chunks::ChunkGrid::chunk_at(const types::pos& pos) noexcept
{
	auto loc = chunks::ChunkGrid::to_loc(pos);
	if (m_chunks.contains(loc))
		return &m_chunks.at(loc);

	return nullptr;
}

const chunks::Chunk const* chunks::ChunkGrid::chunk_at(const types::pos& pos) const noexcept
{
	auto loc = chunks::ChunkGrid::to_loc(pos);
	if (m_chunks.contains(loc))
		return &m_chunks.at(loc);

	return nullptr;
}

render::ChunkMesh* chunks::ChunkGrid::chunkmesh_at(const types::pos& pos) noexcept
{
	auto loc = chunks::ChunkGrid::to_loc(pos);
	if (m_chunk_meshes.contains(loc))
		return &m_chunk_meshes.at(loc);

	return nullptr;
}

const render::ChunkMesh const* chunks::ChunkGrid::chunkmesh_at(const types::pos& pos) const noexcept
{
	auto loc = chunks::ChunkGrid::to_loc(pos);
	if (m_chunk_meshes.contains(loc))
		return &m_chunk_meshes.at(loc);

	return nullptr;
}

types::type_id chunks::ChunkGrid::block_id_at(const types::pos& block_pos) const noexcept
{
	const chunks::Chunk* chunk{ chunk_at(block_pos) };

	if (!chunk)
		return {};

	return chunk->block_at(chunk->getVoxelIndex(block_pos)).id;
}

types::loc chunks::ChunkGrid::to_loc(const types::pos& pos) noexcept
{
	return { 
		static_cast<int32>(std::floor(pos.x / chunks::Chunk::g_size)),
		static_cast<int32>(std::floor(pos.y / chunks::Chunk::g_size)),
		static_cast<int32>(std::floor(pos.z / chunks::Chunk::g_size)) };
}