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

		void generate_new_chunks(const types::loc& camPos) noexcept;


		void draw_all() const noexcept;


		// = Getters

		const bool generatedNewChunks() const noexcept
		{
			return m_generated_new_chunks;
		}
		
		std::map<types::loc, Gameplay::World::Chunk>& getChunks() noexcept
		{
			return m_chunks;
		}

		const std::map<types::loc, Gameplay::World::Chunk>& getChunks() const noexcept
		{
			return m_chunks;
		}


		// = Predicates

		bool is_empty(const types::pos& block_pos) const
		{
			auto* c{ chunk_at(block_pos) };

			if (c == nullptr)
				return true;

			return c->block_at(getBlockIndex(block_pos)).filling == Render::Data::Voxel::Empty;
		}


		// = Mutators

		Gameplay::World::Chunk* chunk_at(const types::pos& pos) noexcept
		{
			if(const auto& loc = to_loc(pos); loc)
				return &m_chunks.at(*loc);
			
			return nullptr;
		}

		const Gameplay::World::Chunk* chunk_at(const types::pos& pos) const
		{
			if (const auto& loc = to_loc(pos); loc)
				return &m_chunks.at(*loc);

			return nullptr;
		}

		Render::Data::ChunkMesh* chunkmesh_at(const types::pos& pos)
		{
			if (const auto& loc = to_loc(pos); loc)
				return &m_chunk_meshes.at(*loc);

			return nullptr;
		}

		const Render::Data::ChunkMesh const* chunkmesh_at(const types::pos& pos) const
		{
			if (const auto& loc = to_loc(pos); loc)
				return &m_chunk_meshes.at(*loc);

			return nullptr;
		}


		bool place_block_at(const types::pos& block_pos, const types::pos& offset = {}) noexcept
		{
			auto* c{ chunk_at(block_pos + offset) };

			if (c == nullptr)
				return false;

			auto& current_block{ c->block_at(getBlockIndex(block_pos + offset)) };

			if (current_block.filling == Render::Data::Voxel::Empty) 
			{
				auto* cm{ chunkmesh_at(block_pos + offset) };

				current_block.filling = Render::Data::Voxel::Full;
				cm->updateBuffer(cm->buildMesh(*c));
				return true;
			}

			return false;
		}

		bool break_block_at(const types::pos& block_pos) noexcept
		{
			auto* c{ chunk_at(block_pos) };

			if (c == nullptr)
				return false;

			auto& current_block{ c->block_at(getBlockIndex(block_pos)) };

			if (current_block.filling != Render::Data::Voxel::Empty)
			{
				auto* cm{ chunkmesh_at(block_pos) };

				current_block.filling = Render::Data::Voxel::Empty;
				cm->updateBuffer(cm->buildMesh(*c));
				return true;
			}

			return false;
		}


	private: // = Predicates

		std::optional<types::loc> to_loc(const types::pos& camPos) const noexcept
		{
			const types::loc camLoc{
			static_cast<int>(std::floor(camPos.x / Chunk::g_size)),
			static_cast<int>(std::floor(camPos.y / Chunk::g_size)),
			static_cast<int>(std::floor(camPos.z / Chunk::g_size)) };

			return m_chunks.contains(camLoc) ? std::make_optional(camLoc) : std::nullopt;
		}


	private: // = Getters

		types::chunk_index getBlockIndex(const types::pos& pos) const
		{
			auto fpos = pos - static_cast<types::pos>(chunk_at(pos)->getPos());

			const auto z_stride{ Chunk::g_size * Chunk::g_size };
			auto index = (uint32)(std::abs(std::floor(fpos.z) * z_stride + std::floor(fpos.y) * Chunk::g_size + std::floor(fpos.x)));

			return index;
		}


	private:


		std::map<types::loc, Gameplay::World::Chunk> m_chunks{};
		std::map<types::loc, Render::Data::ChunkMesh> m_chunk_meshes{};


		bool m_generated_new_chunks{};
	};


} // Gample::World