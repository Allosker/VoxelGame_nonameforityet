#pragma once // chunkGrid.hpp
// MIT
// Allosker ------------------------------
// =========+
// Generates a grid of chunks around the player, discards all chunks that are outside of the grid
// ---------------------------------------

#include "uHeaders/types.hpp"
#include "world/voxels/chunk.hpp"
#include "rendering/world_managing/data/chunk/chunkMesh.hpp"
#include <optional>
#include <map>
#include <vector>


namespace World::Voxels
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

		void update(const types::pos& camPos) noexcept;

		void discard_outside_chunks(const types::loc& camPos) noexcept;

		std::vector<types::loc> generate_new_chunks(const types::loc& camPos) noexcept;


		void draw_all() const noexcept;


		// = Getters

		const bool generatedNewChunks() const noexcept { return m_generated_new_chunks; }
		
		std::map<types::loc, World::Voxels::Chunk>& getChunks() noexcept { return m_chunks; }
		const std::map<types::loc, World::Voxels::Chunk>& getChunks() const noexcept { return m_chunks; }


		World::Voxels::Chunk* chunk_at(const types::pos& pos) noexcept;
		const World::Voxels::Chunk* chunk_at(const types::pos& pos) const noexcept;

		Render::Data::ChunkMesh* chunkmesh_at(const types::pos& pos) noexcept;
		const Render::Data::ChunkMesh const* chunkmesh_at(const types::pos& pos) const noexcept;


		// = Predicates

		bool is_empty(const types::pos& block_pos) const noexcept;


		// = Mutators

		void create_chunkMesh_for_chunk_at(const types::loc& key);

		bool set_voxel_at(const types::pos& block_pos, Render::Data::Voxel::Filling fill_with) noexcept;


	private: // = Predicates

		std::optional<types::loc> to_loc(const types::pos& camPos) const noexcept;


	private: // = Getters

		types::chunk_index getVoxelIndex(const types::pos& pos) const;


	private:


		std::map<types::loc, World::Voxels::Chunk> m_chunks{};
		std::map<types::loc, Render::Data::ChunkMesh> m_chunk_meshes{};


		bool m_generated_new_chunks{};
	};


} // Gample::World