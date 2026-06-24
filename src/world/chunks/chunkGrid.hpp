#pragma once // chunkGrid.hpp
// MIT
// Allosker ------------------------------
// =========+
// Generates a grid of chunks around the player, discards all chunks that are outside of the grid
// ---------------------------------------

#include "utilities/types.hpp"
#include "world/types/voxelTypeManager.hpp"
#include "rendering/utilities/frustum_culling.hpp"

#include "world/chunks/chunk.hpp"

#include "rendering/mesh.hpp"

#include "data/renderStates.hpp"

#include <optional>
#include <map>
#include <unordered_map>
#include <vector>

namespace entities
{
	class Player;
}

namespace render
{
	class ChunkMesh;
}


class World;


namespace ChunkSettings
{
	inline int64 world_render_distance{ 1 };

} // ChunkSettings

namespace chunks
{
	class ChunkGrid
	{
	public:

		// Initialization

		explicit ChunkGrid() noexcept = default;


		// Actors

		void update(const World& world, const VoxelTypeManager& type_manager, const entities::Player& player) noexcept;

		void discard_chunks(const types::loc& playerLoc) noexcept;

		void discard_all_chunks() noexcept;

		std::vector<types::loc> allocate_chunks(const types::loc& playerLoc) noexcept;


		void draw_all(const RenderStates& render, const entities::Player& player) const noexcept;


		// = Getters

		const bool generatedNewChunks() const noexcept { return m_generated_new_chunks; }


		std::unordered_map<types::loc, chunks::Chunk>& getChunks() noexcept { return m_chunks; }
		const std::unordered_map<types::loc, chunks::Chunk>& getChunks() const noexcept { return m_chunks; }

		std::unordered_map<types::loc, render::ChunkMesh>& getChunkMeshes() noexcept { return m_chunk_meshes; }
		const std::unordered_map<types::loc, render::ChunkMesh>& getChunkMeshes() const noexcept { return m_chunk_meshes; }

		// No guards against invalid loc
		chunks::Chunk& chunk_at_loc(const types::loc& loc) noexcept;
		// No guard against invalid loc
		const chunks::Chunk& chunk_at_loc(const types::loc& loc) const noexcept;

		chunks::Chunk* chunk_at_loc_ptr(const types::loc& loc) noexcept;
		const chunks::Chunk* chunk_at_loc_ptr(const types::loc& loc) const noexcept;


		// No guards against invalid loc
		render::ChunkMesh& chunkmesh_at_loc(const types::loc& loc) noexcept;
		// No guards against invalid loc
		const render::ChunkMesh& chunkmesh_at_loc(const types::loc& loc) const noexcept;

		render::ChunkMesh* chunkmesh_at_loc_ptr(const types::loc& loc) noexcept;
		const render::ChunkMesh* chunkmesh_at_loc_ptr(const types::loc& loc) const noexcept;

		chunks::Chunk* chunk_at(const types::pos& pos) noexcept;
		const chunks::Chunk* chunk_at(const types::pos& pos) const noexcept;

		render::ChunkMesh* chunkmesh_at(const types::pos& pos) noexcept;
		const render::ChunkMesh* chunkmesh_at(const types::pos& pos) const noexcept;


		types::type_id block_id_at(const types::pos& block_pos) const noexcept;


		// = Predicates

		bool is_empty(const types::pos& block_pos, const VoxelTypeManager& type_manager) const noexcept;


		// = Mutators

		void create_chunkMesh_for_chunk_at(const types::loc& key);


		// Predicates

		static types::loc to_loc(const types::pos& pos) noexcept;


	private:


		std::unordered_map<types::loc, chunks::Chunk> m_chunks{};
		std::unordered_map<types::loc, render::ChunkMesh> m_chunk_meshes{};


		bool m_generated_new_chunks{};

	};
}