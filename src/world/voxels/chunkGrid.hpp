#pragma once // chunkGrid.hpp
// MIT
// Allosker ------------------------------
// =========+
// Generates a grid of chunks around the player, discards all chunks that are outside of the grid
// ---------------------------------------

#include "utilities/types.hpp"
#include "world/types/voxelTypeManager.hpp"
#include "rendering/utilities/frustum_culling.hpp"

#include "world/voxels/chunk.hpp"

#include "rendering/mesh.hpp"

#include <optional>
#include <map>
#include <unordered_map>
#include <vector>


class Player;

namespace Render
{
	class ChunkMesh;
}


class World;

namespace Voxels
{

	namespace ChunkSettings
	{
		inline int64 world_render_distance{ 1 };

	} // ChunkSettings
	
	class ChunkGrid
	{
	public:

		// Initialization

		explicit ChunkGrid() noexcept;


		// Actors

		void update(const World& world, const Render::Types::VoxelTypeManager& type_manager, const Player& player) noexcept;

		void discard_outside_chunks(const types::loc& camPos) noexcept;

		void discard_all_chunks() noexcept;

		std::vector<types::loc> generate_new_chunks(const types::loc& camPos) noexcept;


		void draw_all(const Player& player) const noexcept;


		// = Getters

		const bool generatedNewChunks() const noexcept { return m_generated_new_chunks; }

		
		std::unordered_map<types::loc, Voxels::Chunk>& getChunks() noexcept { return m_chunks; }
		const std::unordered_map<types::loc, Voxels::Chunk>& getChunks() const noexcept { return m_chunks; }

		std::unordered_map<types::loc, Render::ChunkMesh>& getChunkMeshes() noexcept { return m_chunk_meshes; }
		const std::unordered_map<types::loc, Render::ChunkMesh>& getChunkMeshes() const noexcept { return m_chunk_meshes; }

		// No guards against invalid loc
		Voxels::Chunk& chunk_at_loc(const types::loc& loc) noexcept;
		// No guard against invalid loc
		const Voxels::Chunk& chunk_at_loc(const types::loc& loc) const noexcept;

		Voxels::Chunk* chunk_at_loc_ptr(const types::loc& loc) noexcept;
		const Voxels::Chunk* chunk_at_loc_ptr(const types::loc& loc) const noexcept;


		// No guards against invalid loc
		Render::ChunkMesh& chunkmesh_at_loc(const types::loc& loc) noexcept;
		// No guards against invalid loc
		const Render::ChunkMesh& chunkmesh_at_loc(const types::loc& loc) const noexcept;

		Render::ChunkMesh* chunkmesh_at_loc_ptr(const types::loc& loc) noexcept;
		const Render::ChunkMesh* chunkmesh_at_loc_ptr(const types::loc& loc) const noexcept;

		Voxels::Chunk* chunk_at(const types::pos& pos) noexcept;
		const Voxels::Chunk* chunk_at(const types::pos& pos) const noexcept;

		Render::ChunkMesh* chunkmesh_at(const types::pos& pos) noexcept;
		const Render::ChunkMesh* chunkmesh_at(const types::pos& pos) const noexcept;

		
		types::type_id block_id_at(const types::pos& block_pos) const noexcept;


		// = Predicates

		bool is_empty(const types::pos& block_pos, const Render::Types::VoxelTypeManager& type_manager) const noexcept;


		// = Mutators

		void create_chunkMesh_for_chunk_at(const types::loc& key);


		// Predicates

		static types::loc to_loc(const types::pos& pos) noexcept;


	private:


		std::unordered_map<types::loc, Voxels::Chunk> m_chunks{};
		std::unordered_map<types::loc, Render::ChunkMesh> m_chunk_meshes{};


		bool m_generated_new_chunks{};

	};


} // Gample::World