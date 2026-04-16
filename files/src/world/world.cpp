#include "world/world.hpp"

#include <random>

#include "PerlinNoiseHO/PerlinNoise.hpp"

using Spline = std::vector<vec2d>;

static inline double sampleSpline(const Spline& pts, double x)
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

static inline double pickSpline(const std::vector<std::pair<vec2d, std::vector<vec2d>>>& splines, double continentalness, double erosion)
{
	if (splines.empty()) return {};

	if (continentalness <= splines.front().first.x) return splines.front().first.y;
	if (continentalness >= splines.back().first.x) return splines.back().first.y;

	for (size_t i{}; i < splines.size() - 1; i++)
	{
		if (continentalness >= splines[i].first.x && continentalness <= splines[i + 1].first.x)
		{
			double first { sampleSpline(splines[i].second, erosion) };
			double second{ sampleSpline(splines[i + 1].second, erosion) };

			return std::lerp(first, second, (continentalness - splines[i].first.y) / (splines[i + 1].first.y - splines[i].first.y));
		}
	}

	return {};
};


void GameWorld::World::update(const types::pos& playerPos, bool force_reload)
{
	static types::loc lastCamLoc{};

	types::loc camLoc{ GameWorld::Voxels::ChunkGrid::to_loc(playerPos) };

	if (force_reload)
		grid.discard_all_chunks();


	if (force_reload || (lastCamLoc != camLoc))
	{
		auto m = grid.generate_new_chunks(camLoc);
		for (const auto& i : m)
			grid.create_chunkMesh_for_chunk_at(i);

		generateWorld(m);

		grid.discard_outside_chunks(camLoc);

		lastCamLoc = camLoc;
	}

	grid.update(type_manager, playerPos);
}

void GameWorld::World::generateWorld(const std::vector<types::loc>& new_chunks_loc)
{
	using SeedType = siv::PerlinNoise::seed_type;

	static const auto z_stride{ GameWorld::Voxels::Chunk::g_size * GameWorld::Voxels::Chunk::g_size };

	const SeedType seed{ 1232356u };
	siv::PerlinNoise perlin{ seed };

	/*
	Spline spline1
	{
		Spline
		{
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
		}
	};

	Spline spline2
	{
		Spline
		{
			vec2d
			{-0.982032,		-34.9367, },
			{-0.889426,		-30.8249, },
			{-0.773324,		-31.3389, },
			{-0.672426,		-19.0036, },
			{-0.535591,		-9.75213, },
			{-0.449794,		0.573507, },
			{-0.371389,		5.692,    },
			{-0.246217,		9.23496,  },
			{-0.171939,		14.7805,  },
			{-0.10729,		18.9396,  },
			{-0.00275103,	21.4043,  },
			{0.065024,		22.0204,  },
			{0.114305,		22.2515,  },
			{0.18412,		22.0204,  },
			{0.238877,		22.3285,  },
			{0.318275,		22.1745,  },
			{0.364819,		22.0975,  },
			{0.420945,		24.4851,  },
			{0.497604,		25.1013,  },
			{0.570157,		24.9472,  },
			{0.653662,		24.9472,  },
			{0.71937,		22.7136,  },
			{0.748118,		21.8664,  },
			{0.774127,		22.0975,  },
			{0.819302,		23.4068,  },
			{0.86037,		24.7162,  },
			{0.900068,		27.0268,  },
			{0.932923,		29.0293,  },
			{0.979466,		31.1089,  },
		}
	};

	std::vector<std::pair<vec2d, Spline>> splineOfSpline
	{
		{ 0, std::move(spline1) },
		{ 0.4, std::move(spline2) }
	};*/


	Spline spline{};
	spline.reserve(debug.c_points.size());

	for (size_t i{}; i < debug.c_points.size(); i++)
		spline.emplace_back(vec2d{ debug.c_thresholds[i], debug.c_points[i] });

	for (const auto& loc : new_chunks_loc)
	{
		auto& chunk{ grid.chunk_at_loc(loc) };

		// Every Block of the Chunk
		for (int64 x{}; x < GameWorld::Voxels::Chunk::g_size; x++)
			for (int64 z{}; z < GameWorld::Voxels::Chunk::g_size; z++)
			{
				// Generate Depth for the world

				vec2d bpz{ x + chunk.getPos().x, z + chunk.getPos().z };




				double continentalness = perlin.octave2D_11(bpz.x * debug.continentalness_frequency, bpz.y * debug.continentalness_frequency, 6);
				
				//double erosion = perlin.octave2D_11(bpz.x * debug.continentalness_frequency, bpz.y * debug.continentalness_frequency, 6);
			
				y_max = sampleSpline(spline, continentalness);
			

				// Choose Which block type goes where
				for (int64 y{}; y < GameWorld::Voxels::Chunk::g_size; y++)
				{
					auto& current_block{ chunk.block_at(z * z_stride + y * GameWorld::Voxels::Chunk::g_size + x) };

					double bp = y + chunk.getPos().y; // block pos in world coords

					if (current_block.id != 6)
					{
						if (bp <= y_max && bp >= y_max - 1)
							if (bp < debug.sea_level - 1)
								current_block.id = 2;
							else
								current_block.id = 1;

						if (bp <= y_max - 1 && bp >= y_max - 4)
							current_block.id = 2;

						if (bp <= y_max - 4 && bp >= y_max - 80)
							current_block.id = 3;

						if (bp <= y_max - 80)
							current_block.id = 4;

						if (bp > y_max && bp < debug.sea_level)
							current_block.id = 5;

					}
					
					if (
						bp > y_max && bp < y_max + 1 && bp > debug.sea_level && 
						perlin.octave2D_01(x * debug.tree_frequency, z * debug.tree_frequency, 4) <= debug.tree_threshold
						)
					{

						if (auto& b = chunk.block_at({ x, y, z }); b.id != 5)
							b.id = 6;
						else
							continue;


						if (auto* b = chunk.block_at_ptr({ x, y + 1, z }))
							b->id = 6;
						else
							structure_blocks.emplace_back(
								std::pair<types::pos, types::type_id>{ static_cast<types::pos>(vec3l{ x, y + 1, z }) + chunk.getPos(),
								static_cast<types::type_id>(6) 
						});


						if (auto* b = chunk.block_at_ptr({ x, y + 2, z }))
							b->id = 6;
						else
							structure_blocks.emplace_back(
								std::pair<types::pos, types::type_id>{ static_cast<types::pos>(vec3l{ x, y + 2, z }) + chunk.getPos(),
								static_cast<types::type_id>(6)
						});

						if (auto* b = chunk.block_at_ptr({ x, y + 3, z }))
							b->id = 7;
						else
							structure_blocks.emplace_back(
								std::pair<types::pos, types::type_id>{ static_cast<types::pos>(vec3l{ x, y + 3, z }) + chunk.getPos(),
								static_cast<types::type_id>(7)
						});

						// add index version to private
						if (auto* b = chunk.block_at_ptr({ x, y + 2, z + 1 }))
							b->id = 7;
						else
							structure_blocks.emplace_back(
								std::pair<types::pos, types::type_id>{ static_cast<types::pos>(vec3l{ x, y + 2, z + 1 }) + chunk.getPos(),
								static_cast<types::type_id>(7)
						});

						if (auto* b = chunk.block_at_ptr({ x, y + 2, z - 1 }))
							b->id = 7;
						else
							structure_blocks.emplace_back(
								std::pair<types::pos, types::type_id>{ static_cast<types::pos>(vec3l{ x, y + 2, z - 1 }) + chunk.getPos(),
								static_cast<types::type_id>(7)
						});

							
						if (auto* b = chunk.block_at_ptr({ x + 1, y + 2, z }))
							b->id = 7;
						else
							structure_blocks.emplace_back(
								std::pair<types::pos, types::type_id>{ static_cast<types::pos>(vec3l{ x + 1, y + 2, z }) + chunk.getPos(),
								static_cast<types::type_id>(7)
						});

						if (auto* b = chunk.block_at_ptr({ x - 1, y + 2, z }))
							b->id = 7;
						else
							structure_blocks.emplace_back(
								std::pair<types::pos, types::type_id>{ static_cast<types::pos>(vec3l{ x - 1, y + 2, z }) + chunk.getPos(),
								static_cast<types::type_id>(7)
						});
					}
				}

			}
	}

	for (auto it = structure_blocks.begin(); it != structure_blocks.end();)
	{
		if (set_voxel_at(it->first, it->second))
			it = structure_blocks.erase(it);
		else
			it++;
	}
}

void GameWorld::World::draw_chunkGrid(const types::loc& playerLoc) const noexcept
{
	grid.draw_all(playerLoc);
}

bool GameWorld::World::set_voxel_at(const types::pos& block_pos, types::type_id id) noexcept
{
	auto* chunk{ grid.chunk_at(block_pos) };

	if (chunk == nullptr)
		return false;

	chunk->block_at(grid.getVoxelIndex(block_pos)).id = id;
	grid.chunkmesh_at(block_pos)->flags.dirty = true;

	// Dirten up all needed chunks
	auto b{ static_cast<types::pos>(grid.to_loc(block_pos)) };
	vec3f block_chunk_pos{ mpml::floor(block_pos - b * (float)Voxels::Chunk::g_size) };

	if (block_chunk_pos.x == Voxels::Chunk::g_size - 1)
		if (auto* c = grid.chunkmesh_at(block_pos + vec3f{ 1, 0, 0 }))	
			c->flags.dirty = true;

	if (block_chunk_pos.x == 0)
		if (auto* c = grid.chunkmesh_at(block_pos - vec3f{ 1, 0, 0 }))
			c->flags.dirty = true;

	if (block_chunk_pos.y == Voxels::Chunk::g_size - 1)
		if (auto* c = grid.chunkmesh_at(block_pos + vec3f{ 0, 1, 0 }))
			c->flags.dirty = true;

	if (block_chunk_pos.y == 0)
		if (auto* c = grid.chunkmesh_at(block_pos - vec3f{ 0, 1, 0 }))
			c->flags.dirty = true;

	if (block_chunk_pos.z == Voxels::Chunk::g_size - 1)
		if (auto* c = grid.chunkmesh_at(block_pos + vec3f{ 0, 0, 1 }))
			c->flags.dirty = true;

	if (block_chunk_pos.z == 0)
		if (auto* c = grid.chunkmesh_at(block_pos - vec3f{ 0, 0, 1 }))
			c->flags.dirty = true;


	return true;
}

const Render::Data::Voxel const* GameWorld::World::block_at(const types::pos& block_pos) const noexcept
{
	auto* c{ grid.chunk_at(block_pos) };

	if (!c)
		return nullptr;

	auto* ptr = c->block_at_ptr(grid.getVoxelIndex(block_pos));

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
