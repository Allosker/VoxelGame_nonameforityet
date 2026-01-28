#include "gameplay/world/chunk.hpp"

Gameplay::World::Chunk::Chunk(const vec3f& pos) noexcept
	: m_pos(pos), m_voxels(32'768), m_init{true}
{

	for (size_t x{}; x < 32; x++)
		for (size_t y{}; y < 32; y++)
			for (size_t z{}; z < 32; z++)
			{
				if (y >= 16)
					m_voxels.at(static_cast<std::int32_t>((z * 32.f * 32.f) + (y * Gameplay::World::Chunk::g_size) + x)).filling = Render::Data::Cube::Empty;
			}
}
