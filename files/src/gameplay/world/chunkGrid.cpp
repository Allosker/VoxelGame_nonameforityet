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
	for (int x{ camLoc.x - ChunkSettings::world_render_distance }; x <= camLoc.x + ChunkSettings::world_render_distance; x++)
		for (int y{ camLoc.y }; y < camLoc.y + 1; y++) // <= for symetry
			for (int z{ camLoc.z - ChunkSettings::world_render_distance }; z <= camLoc.z + ChunkSettings::world_render_distance; z++)
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
// Getters
// =====================

