#pragma once // world.hpp
// MIT
// Allosker ------------------------------
// =========+
// This class is responsible for everything that the game has to offer in term of chunks/entities/players
// ---------------------------------------

#include "world/voxels/chunkGrid.hpp"
#include "rendering/world_managing/data/typeManagement/voxelTypeManager.hpp"
#include "rendering/world_managing/data/chunk/chunkMesh.hpp"

#include <queue>

namespace GameWorld
{

	class World
	{
	private:


		struct LightNode
		{
			types::pos pos;
		};

		struct LightRemovalNode
		{
			types::pos pos;
			uint16 val;
		};

	public:



		void update(const Player& player, bool force_reload=false);

		void update_lighting() noexcept;

		void update_blocklight() noexcept;
		void update_sunlight() noexcept;

		void gen_nodes_sunlight(const std::vector<types::loc>& chunk_locs) noexcept;

		void generateWorld(const std::vector<types::loc>& new_chunks_loc);

		void draw_chunkGrid(const Player& player) const noexcept;

		

		// = Setters


		bool set_voxel_at(const types::pos& block_pos, types::type_id id) noexcept;


		Render::Data::Voxel* blockempty_at(const types::pos& block_pos) noexcept;
		const Render::Data::Voxel* blockempty_at(const types::pos& block_pos) const noexcept;

		Render::Data::Voxel* block_at(const types::pos& block_pos) noexcept;
		const Render::Data::Voxel* block_at(const types::pos& block_pos) const noexcept;

		const Render::Data::Voxel* block_at(const types::loc& block_loc) const noexcept;

		const GameWorld::Voxels::Chunk* chunk_at(const types::pos& chunk_pos) const noexcept;

		const Render::Data::Types::VoxelTypeManager& getTypeManager() const noexcept { return type_manager; }

		const Voxels::ChunkGrid& getGrid() const noexcept { return grid; }


		
		Voxels::ChunkGrid grid{};
		Render::Data::Types::VoxelTypeManager type_manager{};
		std::vector<types::loc> newly_generated_chunks{};
		std::vector<std::pair<types::pos, types::type_id>> structure_blocks{};

		std::queue<LightNode> lightBfsQueue;
		std::queue<LightRemovalNode> lightRemovalBfsQueue;

		std::queue<LightNode> sunlightBfsQueue;
		std::queue<LightRemovalNode> sunlightRemovalBfsQueue;

		struct
		{
			float gravity{ -28.5 };
		} settings;

		struct
		{
			double tree_frequency{ 20 };
			double tree_threshold{ 0.2 };

			double sea_level{};

			double continentalness_frequency{ 0.008266 };

			std::vector< double> c_thresholds
			{
				-0.982032,
				-0.889426,
				-0.773324,
				-0.672426,
				-0.535591,
				-0.4651,
				-0.365584,
				-0.253628,
				-0.198074,
				-0.141678,
				-0.0794748,
				-0.0302613,
				0.0288858,
				0.0770289,
				0.127923,
				0.174691,
				0.222834,
				0.276479,
				0.345254,
				0.397673,
				0.438741,
				0.479808,
				0.538672,
				0.581109,
				0.64271,
				0.74538,
				0.802875,
				0.875428,
				0.986311,
			};

			std::vector< double> c_points
			{
				-34.9367,
				-30.8249,
				-31.3389,
				-19.0036,
				-9.75213,
				-8.72419,
				-4.09846,
				-3.07052,
				-4.65439,
				-0.47842,
				1.04124,
				1.8328,
				1.90227,
				3.29169,
				3.70851,
				4.33375,
				4.54216,
				5.09793,
				5.03798,
				7.19689,
				8.14995,
				9.67483,
				10.5802,
				11.5809,
				15.6791,
				17.061,
				18.157,
				18.8242,
				20.3014,
			};

		} debug;


		struct
		{
			bool update_world{ true };
			bool draw_chunk_borders{ false };
			bool create_palettes{ true };
		} debug_flags;


		double y_min{ -100 };
		double y_max{ 0 };

		const double y_base{ 40 };

		static constexpr uint8 g_maxblocklight{ 15 };
		static constexpr uint8 g_blocklight_propagation{ 1 };

		static constexpr uint8 g_maxsunlight{ 15 };

	};


}