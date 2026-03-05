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

		void draw_chunkGrid() const noexcept;


		bool set_voxel_at(const types::pos& block_pos, types::type_id id) noexcept;

		const Render::Data::Voxel const* block_at(const types::pos& block_pos) const noexcept;

		const Render::Data::Types::VoxelTypeManager& getTypeManager() const noexcept { return type_manager; }


		Render::Data::Types::VoxelTypeManager type_manager{};
		Voxels::ChunkGrid grid{};

		double y_min{ -32 };
		double y_max{ 2 };
	};


}