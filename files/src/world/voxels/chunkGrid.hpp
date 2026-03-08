#pragma once // chunkGrid.hpp
// MIT
// Allosker ------------------------------
// =========+
// Generates a grid of chunks around the player, discards all chunks that are outside of the grid
// ---------------------------------------

#include "uHeaders/types.hpp"
#include "world/voxels/chunk.hpp"
#include "rendering/world_managing/data/chunk/chunkMesh.hpp"
#include "rendering/world_managing/data/typeManagement/voxelTypeManager.hpp"
#include <optional>
#include <map>
#include <vector>


namespace GameWorld::Voxels
{

	namespace ChunkSettings
	{
		inline int64 world_render_distance{ 1 };

	} // ChunkSettings
	
	class ChunkGrid
	{
	public:

		// Initialization

		explicit ChunkGrid() noexcept = default;


		// Actors

		void update(const types::pos& camPos, const Render::Data::Types::VoxelTypeManager& type_manager) noexcept;

		void discard_outside_chunks(const types::loc& camPos) noexcept;

		std::vector<types::loc> generate_new_chunks(const types::loc& camPos) noexcept;


		void draw_all(const types::loc& camLoc) const noexcept;


		// = Getters

		const bool generatedNewChunks() const noexcept { return m_generated_new_chunks; }
		
		std::map<types::loc, GameWorld::Voxels::Chunk>& getChunks() noexcept { return m_chunks; }
		const std::map<types::loc, GameWorld::Voxels::Chunk>& getChunks() const noexcept { return m_chunks; }

		// No guards against invalid loc
		GameWorld::Voxels::Chunk& chunk_at_loc(const types::loc& loc) noexcept;
		// No guard against invalid loc
		const GameWorld::Voxels::Chunk& const chunk_at_loc(const types::loc& loc) const noexcept;

		// No guards against invalid loc
		Render::Data::ChunkMesh& chunkmesh_at_loc(const types::loc& loc) noexcept;
		// No guards against invalid loc
		const Render::Data::ChunkMesh& chunkmesh_at_loc(const types::loc& loc) const noexcept;

		GameWorld::Voxels::Chunk* chunk_at(const types::pos& pos) noexcept;
		const GameWorld::Voxels::Chunk* chunk_at(const types::pos& pos) const noexcept;

		Render::Data::ChunkMesh* chunkmesh_at(const types::pos& pos) noexcept;
		const Render::Data::ChunkMesh* chunkmesh_at(const types::pos& pos) const noexcept;


		// = Predicates

		bool is_empty(const types::pos& block_pos, const Render::Data::Types::VoxelTypeManager& type_manager) const noexcept;


		// = Mutators

		void create_chunkMesh_for_chunk_at(const types::loc& key, const types::pos& camPos, const Render::Data::Types::VoxelTypeManager& type_manager);


		// = Getters

		types::chunk_index getVoxelIndex(const types::pos& pos) const;


		// Predicates

		static types::loc to_loc(const types::pos& camPos) noexcept;


	private: // = Predicates

		std::optional<types::loc> to_loc_opt(const types::pos& camPos) const noexcept;


	private:


		std::map<types::loc, GameWorld::Voxels::Chunk> m_chunks{};
		std::map<types::loc, Render::Data::ChunkMesh> m_chunk_meshes{};


		bool m_generated_new_chunks{};
	};


} // Gample::World