#pragma once // world.hpp
// MIT
// Allosker ------------------------------
// =========+
// This class is responsible for everything that the game has to offer in term of chunks/entities/players
// ---------------------------------------

#include "world/voxels/chunkGrid.hpp"
#include "rendering/world_managing/data/typeManagement/voxelTypeManager.hpp"

namespace GameWorld
{

	class World
	{
	public:



		void update(const types::pos& camPos);

		void generateWorld(const std::vector<types::loc>& new_chunks_loc);

		void draw_chunkGrid(const types::loc& camLoc) const noexcept;

		


		bool set_voxel_at(const types::pos& block_pos, types::type_id id, const types::pos& camPos) noexcept;

		const Render::Data::Voxel const* block_at(const types::pos& block_pos) const noexcept;

		const Render::Data::Voxel const* block_at(const types::loc& block_loc) const noexcept;

		const GameWorld::Voxels::Chunk* chunk_at(const types::pos& chunk_pos) const noexcept;

		const Render::Data::Types::VoxelTypeManager& getTypeManager() const noexcept { return type_manager; }


		Render::Data::Types::VoxelTypeManager type_manager{};
		Voxels::ChunkGrid grid{};

		struct
		{
			float gravity{ -28.5 };
		} settings;

		double y_min{ -100 };
		double y_max{ 0 };

		const double y_base{ 40 };
	};


}