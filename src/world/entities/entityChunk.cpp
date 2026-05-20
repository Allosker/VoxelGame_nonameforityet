#include "entityChunk.hpp"




void GameWorld::Entities::EntityChunk::update_items(Player& player, const Render::GUI::ItemTypeManager& itm) noexcept
{
	for (size_t i{}; i < m_items.size(); i++)
		if (m_items.at(i).getHitbox().intersects(player.getHitbox()))
			if (player.addItem({ m_items.at(i).getId() }, 1, itm))
				m_items.erase(m_items.begin() + i);

	// Update the physics for them
}

void GameWorld::Entities::EntityChunk::draw(const Render::Shader& shader, const Render::Texturing::Texture& items_texture) noexcept
{
	for (auto& i : m_items)
		i.draw(shader, items_texture);
}
