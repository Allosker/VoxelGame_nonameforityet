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

	const SeedType seed{ 1232356u };
	siv::PerlinNoise perlin{ seed };


	const auto sampleSpline = [](const std::vector<vec2d>& pts, double x) -> double
		{
			if (pts.empty()) return {};

			if (x <= pts.front().x) return pts.front().y;
			if (x >= pts.back().x) return pts.back().y;

			for (size_t i{}; i < pts.size() - 1; i++)
			{
				if (x >= pts[i].x && x <= pts[i + 1].x)
				{
					double t{ (x - pts[i].x) / (pts[i + 1].x - pts[i].x) };
					return pts[i].y + t * (pts[i + 1].y - pts[i].y);
				}
			}

			return {};
		};

	for (const auto& loc : new_chunks_loc)
	{
		auto& chunk{ grid.chunk_at_loc(loc) };

		// Every Block of the Chunk
		for (int64 x{}; x < 32; x++)
			for (int64 z{}; z < 32; z++)
			{
				// Generate Depth for the world

				// frequency: how many waves on a scale
				// amplitude: size of waves

				vec2d bpz{ x + chunk.getPos().x, z + chunk.getPos().z };

				double continetalness = perlin.octave2D_11(bpz.x * 0.001, bpz.y * 0.001, 6);

				
				y_max = sampleSpline(
					{
						{-1.f, -20.f},
						{-0.8, -10.f},
						{-0.6, 10.f},
						{-0.4, 30.f},
						{-0.3, 45.f},
						{-0.1, 50.f},
						{0.1f, 55.f},
						{0.2f, 60.f},
						{0.255f, 90.f},
						{0.355f, 100.f},
						{0.389f, 80.f},
						{0.4f, 70.f},
						{0.5f, 80.f},
						{0.8f, 65.f},
						{1.f , 75.f}
					}, continetalness);
			

				const auto sea_level{ 75. };
				// Choose Which block type goes where
				for (int64 y{}; y < 32; y++)
				{
					auto& current_block{ chunk.block_at(z * z_stride + y * GameWorld::Voxels::Chunk::g_size + x) };

					double bp = y + chunk.getPos().y; // block pos in world coords

					if (current_block.id != 6)
					{
						if (bp < y_max && bp > y_max - 1)
							current_block.id = 1;

						if (bp < y_max - 1 && bp > y_max - 4)
							current_block.id = 2;

						if (bp < y_max - 4 && bp > y_max - 80)
							current_block.id = 3;

						if (bp < y_max - 80)
							current_block.id = 4;

						if (bp > y_max && bp < sea_level)
							current_block.id = 5;
					}

					if (bp > y_max && /*condition for tree to spawn*/ && bp < y_max + 1)
					{
						if (auto& b = chunk.block_at({ x, y, z }); b.id != 5)
							b.id = 6;
						else
							continue;


						if (auto* b = chunk.block_at_ptr({ x, y + 1, z }))
							b->id = 6;

						if (auto* b = chunk.block_at_ptr({ x, y + 2, z }))
							b->id = 6;

						if (auto* b = chunk.block_at_ptr({ x, y + 3, z }))
							b->id = 7;

						// add index version to private
						if (auto* b = chunk.block_at_ptr({ x, y + 2, z + 1 }))
							b->id = 7;

						if (auto* b = chunk.block_at_ptr({ x, y + 2, z - 1 }))
							b->id = 7;

							
						if (auto* b = chunk.block_at_ptr({ x + 1, y + 2, z }))
							b->id = 7;

						if (auto* b = chunk.block_at_ptr({ x - 1, y + 2, z }))
							b->id = 7;
					}
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
