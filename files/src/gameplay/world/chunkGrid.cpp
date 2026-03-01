#include "gameplay/world/chunkGrid.hpp"

// =====================
// Construction/Destruction
// =====================



// =====================
// Actor
// =====================

void Gameplay::World::ChunkGrid::update(const types::pos& camPos) noexcept
{
	types::loc camLoc{
		static_cast<int>(std::floor(camPos.x / Chunk::g_size)),
		static_cast<int>(std::floor(camPos.y / Chunk::g_size)),
		static_cast<int>(std::floor(camPos.z / Chunk::g_size)) };

	generate_new_chunks(camLoc);
	discard_outside_chunks(camLoc);
}

void Gameplay::World::ChunkGrid::discard_outside_chunks(const types::loc& camLoc) noexcept
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

void Gameplay::World::ChunkGrid::generate_new_chunks(const types::loc& camLoc) noexcept
{
	for (int64 x{ camLoc.x - ChunkSettings::world_render_distance }; x <= camLoc.x + ChunkSettings::world_render_distance; x++)
		for (int64 y{ camLoc.y }; y < camLoc.y + 1; y++) // <= for symetry
			for (int64 z{ camLoc.z - ChunkSettings::world_render_distance }; z <= camLoc.z + ChunkSettings::world_render_distance; z++)
			{

				if (types::loc location{ x,y,z }; !m_chunks.contains(location))
				{
					m_chunks.emplace(location, Gameplay::World::Chunk{ types::pos{
						location.x * static_cast<float>(Chunk::g_size),
						location.y * static_cast<float>(Chunk::g_size), 
						location.z * static_cast<float>(Chunk::g_size)} });

					m_chunk_meshes.emplace(location, Render::Data::ChunkMesh{ m_chunks.at(location) });
				}

			}
}

void Gameplay::World::ChunkGrid::draw_all() const noexcept
{
	for (const auto& chunk_mesh : m_chunk_meshes)
		chunk_mesh.second.draw();
}


// =====================
// Predicates
// =====================

bool Gameplay::World::ChunkGrid::is_empty(const types::pos& block_pos) const noexcept
{
	auto* c{ chunk_at(block_pos) };

	if (c == nullptr)
		return true;

	return c->block_at(getVoxelIndex(block_pos)).filling == Render::Data::Voxel::Empty;
}


// =====================
// Mutators
// =====================

Gameplay::World::Chunk* Gameplay::World::ChunkGrid::chunk_at(const types::pos& pos) noexcept
{
	if (const auto& loc = to_loc(pos); loc)
		return &m_chunks.at(*loc);

	return nullptr;
}

const Gameplay::World::Chunk* Gameplay::World::ChunkGrid::chunk_at(const types::pos& pos) const noexcept
{
	if (const auto& loc = to_loc(pos); loc)
		return &m_chunks.at(*loc);

	return nullptr;
}

Render::Data::ChunkMesh* Gameplay::World::ChunkGrid::chunkmesh_at(const types::pos& pos) noexcept
{
	if (const auto& loc = to_loc(pos); loc)
		return &m_chunk_meshes.at(*loc);

	return nullptr;
}

Render::Data::ChunkMesh const* Gameplay::World::ChunkGrid::chunkmesh_at(const types::pos& pos) const noexcept
{
	if (const auto& loc = to_loc(pos); loc)
		return &m_chunk_meshes.at(*loc);

	return nullptr;
}

bool Gameplay::World::ChunkGrid::set_voxel_at(const types::pos& block_pos, Render::Data::Voxel::Filling fill_with) noexcept
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

/*private*/ std::optional<types::loc> Gameplay::World::ChunkGrid::to_loc(const types::pos& camPos) const noexcept
{
	const types::loc camLoc{
	static_cast<int>(std::floor(camPos.x / Chunk::g_size)),
	static_cast<int>(std::floor(camPos.y / Chunk::g_size)),
	static_cast<int>(std::floor(camPos.z / Chunk::g_size)) };

	return m_chunks.contains(camLoc) ? std::make_optional(camLoc) : std::nullopt;
}

/*private*/ types::chunk_index Gameplay::World::ChunkGrid::getVoxelIndex(const types::pos& pos) const
{
	auto fpos = pos - static_cast<types::pos>(chunk_at(pos)->getPos());

	const auto z_stride{ Chunk::g_size * Chunk::g_size };
	auto index = (uint32)(std::abs(std::floor(fpos.z) * z_stride + std::floor(fpos.y) * Chunk::g_size + std::floor(fpos.x)));

	return index;
}




