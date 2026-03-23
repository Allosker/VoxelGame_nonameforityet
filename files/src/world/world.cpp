#include "world/world.hpp"

#include <random>

#include "PerlinNoiseHO/PerlinNoise.hpp"

void GameWorld::World::update(const types::pos& camPos)
{
	static types::loc lastCamLoc{};

	types::loc camLoc{ GameWorld::Voxels::ChunkGrid::to_loc(camPos) };


	if (lastCamLoc != camLoc)
	{
		auto new_chunk_locs{ grid.generate_new_chunks(camLoc) };

		generateWorld(new_chunk_locs);

		for (const auto& loc : new_chunk_locs)
			grid.create_chunkMesh_for_chunk_at(loc, camPos, type_manager);

		grid.discard_outside_chunks(camLoc);

		lastCamLoc = camLoc;
	}
}

void GameWorld::World::generateWorld(const std::vector<types::loc>& new_chunks_loc)
{
	using SeedType = siv::PerlinNoise::seed_type;

	static const auto z_stride{ GameWorld::Voxels::Chunk::g_size * GameWorld::Voxels::Chunk::g_size };

	const SeedType seed{ 123456u };
	siv::PerlinNoise perlin{ seed };

	for (const auto& loc : new_chunks_loc)
	{
		auto& chunk{ grid.chunk_at_loc(loc) };

		// Every Block of the Chunk
		for (int64 x{}; x < 32; x++)
			for (int64 z{}; z < 32; z++)
			{
				// Generate Depth for the world

				//const double frequency{ 0.001 }; // lower = bigger hills
				//const double amplitude{ 32 }; // Maximum terrain height

				vec2d bpz{ vec2d{(double)x, (double)z} + vec2d{chunk.getPos().x, chunk.getPos().z } };

				double scale_p = 0.01;

				vec2d bpz_p{ bpz * scale_p };
				double plains
				{
					 2. * perlin.octave2D_01(1. * bpz_p.x, 1. * bpz_p.y, 0.1) +
					 1. * perlin.octave2D_01(1. * bpz_p.x, 1. * bpz_p.y, 1) +
					 0.5 * perlin.octave2D_01(2. * bpz_p.x, 2. * bpz_p.y, 2) +
					 0.25 * perlin.octave2D_01(4. * bpz_p.x, 4. * bpz_p.y, 4)
				};

				plains /= (2. + 1. + 00.5 + 0.25);
				plains = std::pow(plains, 2.);


				double scale_m = 0.01;

				vec2d bpz_m{ bpz * scale_m };
				double mountains
				{
					 1. * perlin.octave2D_01(1. * bpz_m.x, 1. * bpz_m.y, 1) +
					 0.5 * perlin.octave2D_01(2. * bpz_m.x, 2. * bpz_m.y, 2) +
					 3. * perlin.octave2D_01(4. * bpz_m.x, 4. * bpz_m.y, 3)
				};

				mountains /= (1. + 0.5 + 3.);
				mountains = std::pow(mountains, 1.5);

				double sum = plains;

				y_max = sum * 190.;


				// Choose Which block type goes where
				for (int64 y{}; y < 32; y++)
				{
					auto& current_block{ chunk.block_at(z * z_stride + y * GameWorld::Voxels::Chunk::g_size + x) };


					current_block.id = 0;

					double bp = y + chunk.getPos().y; // block pos in world coords


					if (bp >= y_max - 5 && bp <= y_max)
					{
						current_block.id = 2;

						if (bp >= y_max - 1)
							if(bp < y_base - 1)
							{
								current_block.id = 2;
							}
							else
								current_block.id = 1;

						
					}

					if (bp <= y_max - 5 && bp > y_max - 40)
						current_block.id = 3;

					if (bp <= y_max - 40)
						current_block.id = 4;

					if (bp > y_max && bp < y_base)
						current_block.id = 5;


				}
			}

	}
}

void GameWorld::World::draw_chunkGrid(const types::loc& camLoc) const noexcept
{
	grid.draw_all(camLoc);
}

bool GameWorld::World::set_voxel_at(const types::pos& block_pos, types::type_id id, const types::pos& camPos) noexcept
{
	auto* c{ grid.chunk_at(block_pos) };

	if (c == nullptr)
		return false;

	auto& current_block{ c->block_at(grid.getVoxelIndex(block_pos)) };
	auto* cm{ grid.chunkmesh_at(block_pos) };

	current_block.id = id;
	cm->buildMesh(*c, type_manager);
	cm->updateBuffers(camPos);

	return true;
}

const Render::Data::Voxel const* GameWorld::World::block_at(const types::pos& block_pos) const noexcept
{
	auto* c{ grid.chunk_at(block_pos) };

	if (!c)
		return nullptr;

	auto* ptr =  &c->block_at(grid.getVoxelIndex(block_pos));

	if (!ptr->id)
		return nullptr;

	return ptr;
}

Render::Data::Voxel const* GameWorld::World::block_at(const types::loc& block_loc) const noexcept
{
	return block_at(static_cast<types::pos>(block_loc));
}

const GameWorld::Voxels::Chunk* GameWorld::World::chunk_at(const types::pos& chunk_pos) const noexcept
{
	return grid.chunk_at(chunk_pos);
}
