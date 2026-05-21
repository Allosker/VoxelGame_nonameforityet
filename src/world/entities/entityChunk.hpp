#pragma once // entityChunk.hpp
// MIT
// Allosker ------------------------------
// =========+
// Manages a chunk of entities to scan through them and update each one of them
// ---------------------------------------

#include "rendering/GUI/Items/item3DMesh.hpp"
#include "uHeaders/types.hpp"

#include "rendering/shader.hpp"

#include "world/players/player/player.hpp"
#include "rendering/GUI/Items/itemTypeManager.hpp"

namespace GameWorld::Entities
{

	class EntityChunk
	{
	public:

		EntityChunk() = default;

		EntityChunk(EntityChunk&&) noexcept = default;
		EntityChunk& operator=(EntityChunk&&) noexcept = default;

		// = Actors

		// Updates the items the player has as well
		void update_items(Player& player, const Render::GUI::ItemTypeManager& itm) noexcept;


		// = Mass Overloads

			void addEntity(Render::Item3DMesh&& item, const vec3f& pos) noexcept;


		void draw(const Render::Shader& shader, const Render::Texturing::Texture& items_texture) noexcept;


	private:


		std::vector<Render::Item3DMesh> m_items{};

	public:

		static constexpr std::uint8_t	g_size{ 16 };
		static constexpr std::uint32_t	g_maxSize{ g_size * g_size * g_size };

	};

}