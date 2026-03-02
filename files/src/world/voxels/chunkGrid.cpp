#include "world/voxels/chunkGrid.hpp"


// =====================
// Actor
// =====================

void World::Voxels::ChunkGrid::update(const types::pos& camPos) noexcept
{
	types::loc camLoc{
		static_cast<int>(std::floor(camPos.x / Chunk::g_size)),
		static_cast<int>(std::floor(camPos.y / Chunk::g_size)),
		static_cast<int>(std::floor(camPos.z / Chunk::g_size)) };

	auto new_chunk_locs{ generate_new_chunks(camLoc) };

	for (const auto& loc : new_chunk_locs)
		create_chunkMesh_for_chunk_at(loc);

	discard_outside_chunks(camLoc);
}

void World::Voxels::ChunkGrid::discard_outside_chunks(const types::loc& camLoc) noexcept
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

std::vector<types::loc> World::Voxels::ChunkGrid::generate_new_chunks(const types::loc& camLoc) noexcept
{
	std::vector<types::loc> locations{};

	for (int64 x{ camLoc.x - ChunkSettings::world_render_distance }; x <= camLoc.x + ChunkSettings::world_render_distance; x++)
		for (int64 y{ camLoc.y }; y < camLoc.y + 1; y++) // <= for symetry
			for (int64 z{ camLoc.z - ChunkSettings::world_render_distance }; z <= camLoc.z + ChunkSettings::world_render_distance; z++)
			{

				if (types::loc location{ x,y,z }; !m_chunks.contains(location))
				{
					m_chunks.emplace(location, World::Voxels::Chunk{ types::pos{
						location.x * static_cast<float>(Chunk::g_size),
						location.y * static_cast<float>(Chunk::g_size), 
						location.z * static_cast<float>(Chunk::g_size)} });

					locations.push_back(location);
				}

			}

	return locations;
}

void World::Voxels::ChunkGrid::draw_all() const noexcept
{
	for (const auto& chunk_mesh : m_chunk_meshes)
		chunk_mesh.second.draw();
}


// =====================
// Predicates
// =====================

bool World::Voxels::ChunkGrid::is_empty(const types::pos& block_pos) const noexcept
{
	auto* c{ chunk_at(block_pos) };

	if (c == nullptr)
		return true;

	return c->block_at(getVoxelIndex(block_pos)).filling == Render::Data::Voxel::Empty;
}


// =====================
// Mutators
// =====================

void World::Voxels::ChunkGrid::create_chunkMesh_for_chunk_at(const types::loc& key)
{
	m_chunk_meshes.emplace(key, Render::Data::ChunkMesh{ m_chunks.at(key) });
}

bool World::Voxels::ChunkGrid::set_voxel_at(const types::pos& block_pos, Render::Data::Voxel::Filling fill_with) noexcept
{
	auto* c{ chunk_at(block_pos) };

	if (c == nullptr)
		return false;

	auto& current_block{ c->block_at(getVoxelIndex(block_pos)) };
	auto* cm{ chunkmesh_at(block_pos) };

	current_block.filling = fill_with;
	cm->updateBuffer(cm->buildMesh(*c));

	return true;
}


// =====================
// Getters
// =====================

World::Voxels::Chunk* World::Voxels::ChunkGrid::chunk_at(const types::pos& pos) noexcept
{
	if (const auto& loc = to_loc(pos); loc)
		return &m_chunks.at(*loc);

	return nullptr;
}

const World::Voxels::Chunk* World::Voxels::ChunkGrid::chunk_at(const types::pos& pos) const noexcept
{
	if (const auto& loc = to_loc(pos); loc)
		return &m_chunks.at(*loc);

	return nullptr;
}

Render::Data::ChunkMesh* World::Voxels::ChunkGrid::chunkmesh_at(const types::pos& pos) noexcept
{
	if (const auto& loc = to_loc(pos); loc)
		return &m_chunk_meshes.at(*loc);

	return nullptr;
}

Render::Data::ChunkMesh const* World::Voxels::ChunkGrid::chunkmesh_at(const types::pos& pos) const noexcept
{
	if (const auto& loc = to_loc(pos); loc)
		return &m_chunk_meshes.at(*loc);

	return nullptr;
}

/*private*/ std::optional<types::loc> World::Voxels::ChunkGrid::to_loc(const types::pos& camPos) const noexcept
{
	const types::loc camLoc{
	static_cast<int>(std::floor(camPos.x / Chunk::g_size)),
	static_cast<int>(std::floor(camPos.y / Chunk::g_size)),
	static_cast<int>(std::floor(camPos.z / Chunk::g_size)) };

	return m_chunks.contains(camLoc) ? std::make_optional(camLoc) : std::nullopt;
}

/*private*/ types::chunk_index World::Voxels::ChunkGrid::getVoxelIndex(const types::pos& pos) const
{
	auto fpos = pos - static_cast<types::pos>(chunk_at(pos)->getPos());

	const auto z_stride{ Chunk::g_size * Chunk::g_size };
	auto index = (uint32)(std::abs(std::floor(fpos.z) * z_stride + std::floor(fpos.y) * Chunk::g_size + std::floor(fpos.x)));

	return index;
}




