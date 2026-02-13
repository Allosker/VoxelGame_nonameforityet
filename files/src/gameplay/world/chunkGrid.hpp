#pragma once // chunkGrid.hpp
// MIT
// Allosker ------------------------------
// =========+
// Generates a grid of chunks around the player, discards all chunks that are outside of the grid
// ---------------------------------------

#include "uHeaders/types.hpp"
#include "gameplay/world/chunk.hpp"
#include "rendering/world_managing/data/chunk/chunkMesh.hpp"
#include <optional>
#include<map>


namespace Gameplay::World
{

	namespace ChunkSettings
	{
		inline int32 world_render_distance{ 0 };

	} // ChunkSettings
	
	class ChunkGrid
	{
	public:


		explicit ChunkGrid() noexcept = default;


		// Actors

		void update(const types::pos& camPos) noexcept;

		void discard_outside_chunks(const types::loc& camPos) noexcept;

		void generate_new_chunks(const types::loc& camPos) noexcept;


		void draw_all() const noexcept;


		// Getters

		const bool generatedNewChunks() const noexcept
		{
			return m_generated_new_chunks;
		}


	private:


		std::map<types::loc, Gameplay::World::Chunk> m_chunks{};
		std::map<types::loc, Render::Data::ChunkMesh> m_chunk_meshes{};


		bool m_generated_new_chunks{};
	};


} // Gample::World