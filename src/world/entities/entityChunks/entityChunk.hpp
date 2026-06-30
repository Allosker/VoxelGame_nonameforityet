#pragma once // entityChunk.hpp
// MIT
// Allosker ------------------------------
// =========+
// Manages a chunk of entities to scan through them and update each one of them
// ---------------------------------------

#include "world/entities/items/itemEntity.hpp"
#include "utilities/types.hpp"

#include "rendering/shader.hpp"

#include "world/entities/player/player.hpp"
#include "utilities/resourceManaging.hpp"

namespace entities
{

	class EntityChunk
	{
	public:

		EntityChunk() = default;

		EntityChunk(EntityChunk&&) noexcept = default;
		EntityChunk& operator=(EntityChunk&&) noexcept = default;

		// = Actors

		// Updates the items the player has as well
		void update_items(entities::Player& player, const ItemTypeManager& itm) noexcept;


		// = Mass Overloads

			void addEntity(render::ItemEntity&& item, const vec3f& pos) noexcept;


		void draw(const render::Shader& shader, const render::Texture& items_texture) noexcept;


	private:


		std::vector<render::ItemEntity> m_items{};

	public:

		static constexpr std::uint8_t	g_size{ 16 };
		static constexpr std::uint32_t	g_maxSize{ g_size * g_size * g_size };

	};

}