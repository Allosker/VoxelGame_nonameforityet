#include "world/world.hpp"

#include <random>

#include "entities/player/player.hpp"
#include "terrain/terrain.hpp"

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


// =====================
// Construction/Initialization
// =====================

World::World(const utils::Camera* cam)
	: player{ ItemTypeManager::get(), cam }
{
	crossair.setPosition({ -crossair.getSize().x / 2, -crossair.getSize().y / 2 });
}


// =====================
// Major World Updates
// =====================

void World::update(const Window& window, float dt)
{
	// Player
	update_player(window, dt);

	types::loc current_player_loc{ chunks::ChunkGrid::to_loc(player.getPosition()) };

	// Cunkgrid
	update_chunks(current_player_loc);

	// Entities
	update_entities(current_player_loc);


	reset_flags();
}

void World::reset_flags() noexcept
{
	// Debug
	debug_flags.force_reload = {};
}

void World::update_chunks(const types::loc& current_player_loc)
{
	static types::loc last_player_loc{};

	if (debug_flags.force_reload)
		grid.discard_all_chunks();

	if (debug_flags.update_world && (debug_flags.force_reload || last_player_loc != current_player_loc))
	{
		// Allocate memory for new chunks
		auto m = grid.allocate_chunks(current_player_loc);
		grid.discard_chunks(current_player_loc);


		// Generate World Map with Perlin Noise
		generate_world(m);


		for (auto& i : grid.getChunks())
			i.second.make_empty();

		// Generate Sunlight for corresponding chunks
		//gen_nodes_sunlight(m);

		last_player_loc = current_player_loc;
	}

	if (debug_flags.update_world)
	{
		update_sunlight();

		if (sunlightBfsQueue.empty())
			grid.update(*this, VoxelTypeManager::get(), player);
	}


	if (debug_flags.draw_chunk_borders)
		for (const auto& c : grid.getChunks())
			render::debug::aabb(c.second.getPos() + 16.f, { 16.f }, (c.second.isEmpty() ? vec3f{ 0, 1, 1 } : vec3f{ 1, 1, 0 }), 0, false);
}

void World::update_entities(const types::loc& current_player_loc)
{
	entity_chunkGrid.update(player, ItemTypeManager::get());
}

void World::update_player(const Window& window, float dt)
{
	player.update(*this, ItemTypeManager::get(), dt);

	player.resetMovement();
}


// =====================
// Minor World Updates
// =====================

void World::update_blocklight() noexcept
{
	/*update_rLighting();
	update_gLighting();
	update_bLighting();*/
}

void World::update_rLighting() noexcept
{
	while (!rRemovalBfsQueue.empty())
	{
		LightRemovalNode& node = rRemovalBfsQueue.front();

		types::pos pos = node.pos;
		auto lightlevel = node.val;

		rRemovalBfsQueue.pop();

		const std::array<types::pos, 6> dirs
		{
			types::pos
			{pos.x + 1, pos.y, pos.z},
			{pos.x - 1, pos.y, pos.z},
			{pos.x, pos.y + 1, pos.z},
			{pos.x, pos.y - 1, pos.z},
			{pos.x, pos.y, pos.z + 1},
			{pos.x, pos.y, pos.z - 1}
		};

		for (const auto& d : dirs)
			if (auto* v = try_get_block(d))
			{
				if (auto neighborLevel = v->getR();
					neighborLevel != 0 && neighborLevel < lightlevel)
				{
					v->setR(0);

					rRemovalBfsQueue.emplace(d, neighborLevel);

					if (auto* cmesh = grid.chunkmesh_at(d))
						cmesh->flags.dirty = true;
				}
				else if (neighborLevel >= lightlevel)
				{
					rBfsQueue.emplace(d);
				}
			}

	}

	while (!rBfsQueue.empty())
	{
		LightNode& node = rBfsQueue.front();

		types::pos pos = node.pos;

		rBfsQueue.pop();

		auto ptr = try_get_block(pos);
		uint8 lightlevel{};
		if (ptr)
			lightlevel = ptr->getR();
		else
			lightlevel = 0;

		const std::array<types::pos, 6> dirs
		{
			types::pos
			{pos.x + 1, pos.y, pos.z},
			{pos.x - 1, pos.y, pos.z},
			{pos.x, pos.y + 1, pos.z},
			{pos.x, pos.y - 1, pos.z},
			{pos.x, pos.y, pos.z + 1},
			{pos.x, pos.y, pos.z - 1}
		};

		for (const auto& d : dirs)
			if (auto* v = try_get_block(d))
			{
				if (VoxelTypeManager::get().isTypeTransparent(v) &&
					v->getR() < lightlevel - 1)
				{
					v->setR(lightlevel - 1);

					rBfsQueue.emplace(d);

					if (auto* cmesh = grid.chunkmesh_at(d))
						cmesh->flags.dirty = true;
				}
			}
	}
}

void World::update_gLighting() noexcept
{
	while (!gRemovalBfsQueue.empty())
	{
		LightRemovalNode& node = gRemovalBfsQueue.front();

		types::pos pos = node.pos;
		auto lightlevel = node.val;

		gRemovalBfsQueue.pop();

		const std::array<types::pos, 6> dirs
		{
			types::pos
			{pos.x + 1, pos.y, pos.z},
			{pos.x - 1, pos.y, pos.z},
			{pos.x, pos.y + 1, pos.z},
			{pos.x, pos.y - 1, pos.z},
			{pos.x, pos.y, pos.z + 1},
			{pos.x, pos.y, pos.z - 1}
		};

		for (const auto& d : dirs)
			if (auto* v = try_get_block(d))
			{
				if (auto neighborLevel = v->getG();
					neighborLevel != 0 && neighborLevel < lightlevel)
				{
					v->setG(0);

					gRemovalBfsQueue.emplace(d, neighborLevel);

					if (auto* cmesh = grid.chunkmesh_at(d))
						cmesh->flags.dirty = true;
				}
				else if (neighborLevel >= lightlevel)
				{
					gBfsQueue.emplace(d);
				}
			}

	}

	while (!gBfsQueue.empty())
	{
		LightNode& node = gBfsQueue.front();

		types::pos pos = node.pos;

		gBfsQueue.pop();

		auto ptr = try_get_block(pos);
		uint8 lightlevel{};
		if (ptr)
			lightlevel = ptr->getG();
		else
			lightlevel = 0;

		const std::array<types::pos, 6> dirs
		{
			types::pos
			{pos.x + 1, pos.y, pos.z},
			{pos.x - 1, pos.y, pos.z},
			{pos.x, pos.y + 1, pos.z},
			{pos.x, pos.y - 1, pos.z},
			{pos.x, pos.y, pos.z + 1},
			{pos.x, pos.y, pos.z - 1}
		};

		for (const auto& d : dirs)
			if (auto* v = try_get_block(d))
			{
				if (VoxelTypeManager::get().isTypeTransparent(v) &&
					v->getG() < lightlevel - 1)
				{
					v->setG(lightlevel - 1);

					gBfsQueue.emplace(d);

					if (auto* cmesh = grid.chunkmesh_at(d))
						cmesh->flags.dirty = true;
				}
			}
	}
}

void World::update_bLighting() noexcept
{
	while (!bRemovalBfsQueue.empty())
	{
		LightRemovalNode& node = bRemovalBfsQueue.front();

		types::pos pos = node.pos;
		auto lightlevel = node.val;

		bRemovalBfsQueue.pop();

		const std::array<types::pos, 6> dirs
		{
			types::pos
			{pos.x + 1, pos.y, pos.z},
			{pos.x - 1, pos.y, pos.z},
			{pos.x, pos.y + 1, pos.z},
			{pos.x, pos.y - 1, pos.z},
			{pos.x, pos.y, pos.z + 1},
			{pos.x, pos.y, pos.z - 1}
		};

		for (const auto& d : dirs)
			if (auto* v = try_get_block(d))
			{
				if (auto neighborLevel = v->getB();
					neighborLevel != 0 && neighborLevel < lightlevel)
				{
					v->setB(0);

					bRemovalBfsQueue.emplace(d, neighborLevel);

					if (auto* cmesh = grid.chunkmesh_at(d))
						cmesh->flags.dirty = true;
				}
				else if (neighborLevel >= lightlevel)
				{
					bBfsQueue.emplace(d);
				}
			}

	}

	while (!bBfsQueue.empty())
	{
		LightNode& node = bBfsQueue.front();

		types::pos pos = node.pos;

		bBfsQueue.pop();

		auto ptr = try_get_block(pos);
		uint8 lightlevel{};
		if (ptr)
			lightlevel = ptr->getB();
		else
			lightlevel = 0;

		const std::array<types::pos, 6> dirs
		{
			types::pos
			{pos.x + 1, pos.y, pos.z},
			{pos.x - 1, pos.y, pos.z},
			{pos.x, pos.y + 1, pos.z},
			{pos.x, pos.y - 1, pos.z},
			{pos.x, pos.y, pos.z + 1},
			{pos.x, pos.y, pos.z - 1}
		};

		for (const auto& d : dirs)
			if (auto* v = try_get_block(d))
			{
				if (VoxelTypeManager::get().isTypeTransparent(v) &&
					v->getB() < lightlevel - 1)
				{
					v->setB(lightlevel - 1);

					bBfsQueue.emplace(d);

					if (auto* cmesh = grid.chunkmesh_at(d))
						cmesh->flags.dirty = true;
				}
			}
	}
}




void World::update_sunlight() noexcept
{
	while (!sunlightRemovalBfsQueue.empty())
	{
		LightRemovalNode& node = sunlightRemovalBfsQueue.front();

		types::pos pos = node.pos;
		auto lightlevel = node.val;

		sunlightRemovalBfsQueue.pop();

		const std::array<types::pos, 6> dirs
		{
			types::pos
			{pos.x + 1, pos.y, pos.z},
			{pos.x - 1, pos.y, pos.z},
			{pos.x, pos.y + 1, pos.z},
			{pos.x, pos.y - 1, pos.z},
			{pos.x, pos.y, pos.z + 1},
			{pos.x, pos.y, pos.z - 1}
		};

		for (size_t j{}; j < dirs.size(); j++)
			if (auto* v = try_get_block(dirs[j]))
			{
				if (auto neighborLevel = v->getSunlight();
					(neighborLevel != 0 && neighborLevel < lightlevel) || (j == 3 && lightlevel == g_maxsunlight))
				{
					v->setSunlight(0);

					sunlightRemovalBfsQueue.emplace(dirs[j], neighborLevel);

					if (auto* cmesh = grid.chunkmesh_at(dirs[j]))
						cmesh->flags.dirty = true;
				}
				else if (neighborLevel >= lightlevel)
				{
					sunlightBfsQueue.emplace(dirs[j]);
				}
			}

	}

	size_t i{};
	while (!sunlightBfsQueue.empty())
	{
		LightNode& node = sunlightBfsQueue.front();

		types::pos pos = node.pos;

		sunlightBfsQueue.pop();

		auto ptr = try_get_block(pos);
		uint8 lightlevel{};
		if (ptr)
			lightlevel = ptr->getSunlight();
		else
			lightlevel = 0;

		const std::array<types::pos, 6> dirs
		{
			types::pos
			{pos.x + 1, pos.y, pos.z},
			{pos.x - 1, pos.y, pos.z},
			{pos.x, pos.y + 1, pos.z},
			{pos.x, pos.y - 1, pos.z},
			{pos.x, pos.y, pos.z + 1},
			{pos.x, pos.y, pos.z - 1}
		};

		for (size_t j{}; j < dirs.size(); j++)
			if (auto* v = try_get_block(dirs[j]))
			{
				if (j == 3 && VoxelTypeManager::get().isTypeTransparent(v) && try_get_block(pos)->getSunlight() == g_maxsunlight)
				{
					v->setSunlight(g_maxsunlight);
					sunlightBfsQueue.emplace(dirs[j]);
				}

				if (VoxelTypeManager::get().isTypeTransparent(v) &&
					v->getSunlight() < lightlevel - 1)
				{
					v->setSunlight(lightlevel - 1);

					sunlightBfsQueue.emplace(dirs[j]);

					if (auto* cmesh = grid.chunkmesh_at(dirs[j]))
						cmesh->flags.dirty = true;
				}
			}


		if (i > 5000)
			break;
		i++;
	}
}


/*private*/ void World::gen_nodes_sunlight(const std::vector<types::loc>&chunk_locs) noexcept
{
	for(auto& loc : chunk_locs)
	{
		auto& chunk = grid.chunk_at_loc(loc);
		if (!chunk.isEmpty() && !chunk.generated_sunlight)
		{
			if (auto* c = grid.chunk_at_loc_ptr(loc + types::loc{ 0, 1 , 0 }))
			{
				for (int32 x{}; x < chunks::Chunk::g_size; x++)
					for (int32 z{}; z < chunks::Chunk::g_size; z++)
					{
						if (VoxelTypeManager::get().isTypeTransparent(c->block_at_ptr({ x, chunks::Chunk::g_size - 1, z })))
						{
							c->block_at_ptr({ x, chunks::Chunk::g_size - 1, z })->setSunlight(g_maxsunlight);
							sunlightBfsQueue.emplace(static_cast<types::pos>(types::loc{ x, chunks::Chunk::g_size - 1, z }) + c->getPos());
						}
					}
			}
			else // if(current chunk is above ground)
			{
				/*
				*   1.2.1 If we assume that there is no sunlight, i.e. we are underground, then we are done.
				*
				*	1.2.2 If we assume that there is sunlight, then we iterate the voxels at the very top of the chunk.
				*
				*/
			}

			chunk.generated_sunlight = true;
		}
	}
}

/*private*/ void World::generate_world(const std::vector<types::loc>& new_chunks_loc)
{
	static const auto z_stride{ chunks::Chunk::g_size * chunks::Chunk::g_size };



	for (const auto& loc : new_chunks_loc)
	{
		auto& chunk{ grid.chunk_at_loc(loc) };

		// Every Block of the chunks::Chunk
		for (int64 lx{}; lx < chunks::Chunk::g_size; lx++)
			for (int64 lz{}; lz < chunks::Chunk::g_size; lz++)
			{
				// Generate Depth for the world

				double x{ lx + chunk.getPos().x };
				double z{ lz + chunk.getPos().z };

				
				height_max = terrain::generate_height(x, z);



				// Choose Which block type goes where
				for (int64 ly{}; ly < chunks::Chunk::g_size; ly++)
				{
					auto& current_block{ chunk.block_at(lz * z_stride + ly * chunks::Chunk::g_size + lx) };

					double bp = ly + chunk.getPos().y; // block pos in world coords


					if (bp <= height_max)
					{
						if (!current_block.id && height_max > terrain::thresholds.water * terrain::height_factor && height_max < terrain::thresholds.sand * terrain::height_factor)
							if (bp <= height_max && bp >= height_max - 2)
								current_block.id = 6; // sand

						if (!current_block.id && height_max > terrain::thresholds.sand * terrain::height_factor && height_max < terrain::thresholds.grass * terrain::height_factor)
							if (bp <= height_max && bp >= height_max - 4)
								current_block.id = 1; // grass

						if (!current_block.id && height_max > terrain::thresholds.grass * terrain::height_factor && height_max < terrain::thresholds.stone * terrain::height_factor)
							if (bp <= height_max && bp >= height_max - 50)
								current_block.id = 3; // stone

						if (!current_block.id && bp >= height_max - 50)
							current_block.id = 3;

						if (!current_block.id)
							current_block.id = 4;

					}

					if (!current_block.id && bp >= height_max && bp <= terrain::thresholds.water * terrain::height_factor)
						current_block.id = 5; // water !current_block.id
					
					
				}

			}
	}

	for (auto it = structure_blocks.begin(); it != structure_blocks.end();)
	{
		auto* chunk{ grid.chunk_at(it->first) };

		if (chunk)
		{
			chunk->block_at(chunk->getVoxelIndex(it->first)).id = it->second;
			it = structure_blocks.erase(it);
		}
		else
			it++;
	}
}


// =====================
// Drawing
// =====================

void World::draw(const RenderStates& render) noexcept
{
	glEnable(GL_CULL_FACE);

	//model = mpml::rotate(m, mpml::Angle<>::fromDegrees((daytime * 360.f) ), { 0.5, 0, 0.5 }); example only


	glEnable(GL_DEPTH_TEST);


	Resources::get().s_world.use();
	Resources::get().s_world.setValue("model", mpml::Identity4<float>);
	Resources::get().s_world.setValue("vp", render.vp);

	Resources::get().atlas_voxels.bind();

	grid.draw_all(render, player);
	

	Resources::get().s_items_world.use();
	Resources::get().s_items_world.setValue("model", mpml::Identity4<float>);
	Resources::get().s_items_world.setValue("vp", render.vp);

	entity_chunkGrid.draw(Resources::get().s_items_world, Resources::get().atlas_guiblocks);

	if (flags.draw_cubehighlight)
	{
		ch.useShader();
		ch.draw();
	}

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);


	Resources::get().s_gui_text.use();
	Resources::get().s_gui_text.setValue("proj", ma_gui_ortho);
	Resources::get().s_gui_text.setValue("view", mpml::Identity4<float>);

	Resources::get().s_gui.use();
	Resources::get().s_gui.setValue("proj", ma_gui_ortho);
	Resources::get().s_gui.setValue("view", mpml::Identity4<float>);

	Resources::get().t_crossair.bind();
	crossair.draw_transparent(Resources::get().s_gui);


	player.draw(ItemTypeManager::get());
}


// =====================
// Setters
// =====================

bool World::set_voxel_at(const types::pos& block_pos, types::type_id id) noexcept
{
	const auto chunk_loc = chunks::ChunkGrid::to_loc(block_pos);
	auto* chunk{ grid.chunk_at_loc_ptr(chunk_loc) };

	if (chunk == nullptr)
		return false;
	
	if (id != 0)
	{
		chunk->make_full();
		gen_nodes_sunlight({ chunk_loc });
	}

	if (chunk->isEmpty() && id != 0)
		grid.create_chunkMesh_for_chunk_at(chunks::ChunkGrid::to_loc(chunk->getPos()));

	const auto& voxel_index = chunk->getVoxelIndex(block_pos);
	auto& current_block = chunk->block_at(voxel_index);
	
	
	// Update source lights
	if (id != 0 && VoxelTypeManager::get().getType(id).is_light_source)
	{
		const auto& new_block_type = VoxelTypeManager::get().getType(id);

		rBfsQueue.emplace(block_pos);
		gBfsQueue.emplace(block_pos);
		bBfsQueue.emplace(block_pos);

		current_block.setR(new_block_type.light.x);
		current_block.setG(new_block_type.light.y);
		current_block.setB(new_block_type.light.z);
	}
	else if (current_block.id != 0 && VoxelTypeManager::get().getType(current_block.id).is_light_source)
	{
		rRemovalBfsQueue.emplace(block_pos, current_block.getR());
		gRemovalBfsQueue.emplace(block_pos, current_block.getG());
		bRemovalBfsQueue.emplace(block_pos, current_block.getB());

		current_block.setR(0);
		current_block.setG(0);
		current_block.setB(0);
	}

	for (int32 x{}; x < chunks::Chunk::g_size; x++)
		for (int32 y{}; y < chunks::Chunk::g_size; y++)
			for (int32 z{}; z < chunks::Chunk::g_size; z++)
			{
				const auto& block = chunk->block_at({ x,y,z });

				if (block.id != 0 && VoxelTypeManager::get().getType(block.id).is_light_source)
				{
					const auto pos = static_cast<types::pos>(types::loc{ x,y,z }) + chunk->getPos();

					rRemovalBfsQueue.emplace(pos, block.getR());
					gRemovalBfsQueue.emplace(pos, block.getG());
					bRemovalBfsQueue.emplace(pos, block.getB());

					rBfsQueue.emplace(pos);
					gBfsQueue.emplace(pos);
					bBfsQueue.emplace(pos);
				}
			}

	chunk->block_at(voxel_index).id = id;

	update_blocklight();

	

	// Sunlight update
	if (id == 0)
	{
		const std::array<types::pos, 6> dirs
		{
			types::pos
			{block_pos.x + 1, block_pos.y, block_pos.z},
			{block_pos.x - 1, block_pos.y, block_pos.z},
			{block_pos.x, block_pos.y + 1, block_pos.z},
			{block_pos.x, block_pos.y - 1, block_pos.z},
			{block_pos.x, block_pos.y, block_pos.z + 1},
			{block_pos.x, block_pos.y, block_pos.z - 1}
		};

		if (auto* tb = try_get_block({ block_pos.x, block_pos.y + 1, block_pos.z }); tb && tb->getSunlight() != g_maxsunlight)
		{
			uint8 highest{};
			for (const auto& p : dirs)
				if (const auto* b = try_get_block(p))
					highest = std::max(highest, b->getSunlight());

			current_block.setSunlight(highest == 0 ? 0 : highest - 1);
		}
		else
			current_block.setSunlight(g_maxsunlight);

		sunlightBfsQueue.emplace(block_pos);
	}
	else
	{
		sunlightRemovalBfsQueue.emplace(block_pos, current_block.getSunlight());
		current_block.setSunlight(0);
	}

	
		

	// - Update dirtiness of chunk meshes to update them
	if (auto* c = grid.chunkmesh_at(block_pos))
		c->flags.dirty = true;

	// Dirten up all needed chunks
	auto b{ static_cast<types::pos>(grid.to_loc(block_pos)) };
	vec3f block_chunk_pos{ mpml::floor(block_pos - b * (float)chunks::Chunk::g_size) };

	if (block_chunk_pos.x == chunks::Chunk::g_size - 1)
		if (auto* c = grid.chunkmesh_at(block_pos + vec3f{ 1, 0, 0 }))	
			c->flags.dirty = true;

	if (block_chunk_pos.x == 0)
		if (auto* c = grid.chunkmesh_at(block_pos - vec3f{ 1, 0, 0 }))
			c->flags.dirty = true;

	if (block_chunk_pos.y == chunks::Chunk::g_size - 1)
		if (auto* c = grid.chunkmesh_at(block_pos + vec3f{ 0, 1, 0 }))
			c->flags.dirty = true;

	if (block_chunk_pos.y == 0)
		if (auto* c = grid.chunkmesh_at(block_pos - vec3f{ 0, 1, 0 }))
			c->flags.dirty = true;

	if (block_chunk_pos.z == chunks::Chunk::g_size - 1)
		if (auto* c = grid.chunkmesh_at(block_pos + vec3f{ 0, 0, 1 }))
			c->flags.dirty = true;

	if (block_chunk_pos.z == 0)
		if (auto* c = grid.chunkmesh_at(block_pos - vec3f{ 0, 0, 1 }))
			c->flags.dirty = true;


	return true;
}


// =====================
// Getters
// =====================

Data::Voxel* World::try_get_block(const types::pos& block_pos) noexcept
{
	auto* c{ grid.chunk_at(block_pos) };

	if (!c)
		return nullptr;

	return c->block_at_ptr(c->getVoxelIndex(block_pos));
}

const Data::Voxel* World::try_get_block(const types::pos& block_pos) const noexcept
{
	auto* c{ grid.chunk_at(block_pos) };

	if (!c)
		return nullptr;

	return c->block_at_ptr(c->getVoxelIndex(block_pos));
}

Data::Voxel* World::block_at(const types::pos& block_pos) noexcept
{
	auto* ptr = try_get_block(block_pos);

	if (!ptr)
		return nullptr;

	if (!ptr->id)
		return nullptr;

	return ptr;
}

const Data::Voxel* World::block_at(const types::pos& block_pos) const noexcept
{
	auto* ptr = try_get_block(block_pos);

	if (!ptr)
		return nullptr;

	if (!ptr->id)
		return nullptr;

	return ptr;
}

const Data::Voxel* World::block_at(const types::loc& block_loc) const noexcept
{
	return block_at(static_cast<types::pos>(block_loc));
}

Data::Voxel World::blockout_at(const types::pos& block_pos) const noexcept
{
	auto* c{ grid.chunk_at(block_pos) };

	if (!c)
		return Data::Voxel{0, 15};

	auto block = c->block_at(c->getVoxelIndex(block_pos));

	return block;
}

const chunks::Chunk* World::chunk_at(const types::pos& pos) const noexcept
{
	return grid.chunk_at(pos);
}
