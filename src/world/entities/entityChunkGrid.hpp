#pragma once // entityChunkGrid.hpp
// MIT
// Allosker ------------------------------
// =========+
// Manages a chunk of entities to scan through them and update each one of them
// ---------------------------------------

#include "entityChunk.hpp"

#include "world/chunks/chunkGrid.hpp"

namespace entities
{

	class EntityChunkGrid
	{
	public:


		void update(Player& player, const render::gui::ItemTypeManager& itm) noexcept;
		
		void update_items(Player& player, const render::gui::ItemTypeManager& itm) noexcept;

		void addEntity(render::ItemEntity&& item, const vec3f& pos) noexcept { chunk_at_emplace(pos).addEntity(std::move(item), pos); }

		void draw(const render::Shader& shader, const render::Texture& items_texture) noexcept;



		EntityChunk* chunk_at(const types::loc& loc) noexcept
		{
			return m_chunks.contains(loc) ? &m_chunks.at(loc) : nullptr;
		}
		const EntityChunk* chunk_at(const types::loc& loc) const noexcept
		{
			return m_chunks.contains(loc) ? &m_chunks.at(loc) : nullptr;
		}

		EntityChunk* chunk_at(const types::pos& pos) noexcept
		{
			const auto& loc = to_loc(pos);
			return m_chunks.contains(loc) ? &m_chunks.at(loc) : nullptr;
		}
		const EntityChunk* chunk_at(const types::pos& pos) const noexcept
		{
			const auto& loc = to_loc(pos);
			return m_chunks.contains(loc) ? &m_chunks.at(loc) : nullptr;
		}


	private:

		EntityChunk& chunk_at_emplace(const types::pos& pos) noexcept
		{
			const auto& loc = to_loc(pos);

			if (!chunk_at(loc))
				m_chunks.try_emplace(loc);

			return m_chunks.at(loc);
		}

		static types::loc to_loc(const types::pos& pos) noexcept;

		static size_t to_index(const types::loc& loc) noexcept;


	private:

		std::map<types::loc, EntityChunk> m_chunks{};

	};

}