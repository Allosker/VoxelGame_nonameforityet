#include "world/world.hpp"

#include <random>

#include "PerlinNoiseHO/PerlinNoise.hpp"

void GameWorld::World::update(const types::pos& camPos)
{
	types::loc camLoc{
		static_cast<int64>(std::floor(camPos.x / GameWorld::Voxels::Chunk::g_size)),
		static_cast<int64>(std::floor(camPos.y / GameWorld::Voxels::Chunk::g_size)),
		static_cast<int64>(std::floor(camPos.z / GameWorld::Voxels::Chunk::g_size)) };

	const auto z_stride{ GameWorld::Voxels::Chunk::g_size * GameWorld::Voxels::Chunk::g_size };

	auto new_chunk_locs{ grid.generate_new_chunks(camLoc) };

	const siv::PerlinNoise::seed_type seed{ 123456u };
	const siv::PerlinNoise perlin{ seed };

	for(const auto& loc : new_chunk_locs)
	{
		auto& chunk{ grid.chunk_at_loc(loc) };

		double y_min{ 0 };
		double y_max{ 2 };


		for (int64 x{}; x < 32; x++)
			for (int64 z{}; z < 32; z++)
			{
				const double frequency{ 0.001 }; // lower = bigger hills
				const double amplitude{ 32 }; // Maximum terrain height

				vec2d block_xzPos{ vec2l{x, z} + vec2l{chunk.getPos().x, chunk.getPos().z } };

				auto n1 = perlin.octave2D_01(block_xzPos.x * 0.05, block_xzPos.y * 0.05, 4) * 16.;
				auto n2 = perlin.octave2D_01(block_xzPos.x * 0.001, block_xzPos.y * 0.001, 2) * 32.;
				auto n3 = perlin.octave2D_01(block_xzPos.x * 0.0001, block_xzPos.y * 0.000011, 8) * 2.;

				y_max = n1 + n2 + n3;

				for (int64 y{}; y < 32; y++)
				{
					auto& current_block{ chunk.block_at(z * z_stride + y * GameWorld::Voxels::Chunk::g_size + x) };

					
					current_block.id = 0;

					vec3d block_pos{ vec3l{x,y,z} + chunk.getPos() };




					if (block_pos.y > y_min && block_pos.y <= y_max)
					{
						if(block_pos.y < y_max - 4)
							current_block.id = 3;

						if (block_pos.y > y_max - 1)
							current_block.id = 1;

						if (block_pos.y < y_max - 1 && block_pos.y > y_max - 4)
							current_block.id = 2;
					}
				}
			}
		
	}

	for (const auto& loc : new_chunk_locs)
		grid.create_chunkMesh_for_chunk_at(loc, type_manager);

	grid.discard_outside_chunks(camLoc);
}

void GameWorld::World::draw_chunkGrid() const noexcept
{
	grid.draw_all();
}

bool GameWorld::World::set_voxel_at(const types::pos& block_pos, types::type_id id) noexcept
{
	auto* c{ grid.chunk_at(block_pos) };

	if (c == nullptr)
		return false;

	auto& current_block{ c->block_at(grid.getVoxelIndex(block_pos)) };
	auto* cm{ grid.chunkmesh_at(block_pos) };

	current_block.id = id;
	cm->updateBuffer(cm->buildMesh(*c, type_manager));

	return true;
}