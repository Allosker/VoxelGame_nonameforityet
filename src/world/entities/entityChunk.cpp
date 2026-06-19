#include "entityChunk.hpp"




void Entities::EntityChunk::update_items(Player& player, const Render::GUI::ItemTypeManager& itm) noexcept
{
	for (size_t i{}; i < m_items.size(); i++)
		if (m_items.at(i).getHitbox().intersects(player.getHitbox()))
			if (player.addItem({ m_items.at(i).getId() }, 1, itm))
				m_items.erase(m_items.begin() + i);

	// Update the physics for them
}

void Entities::EntityChunk::addEntity(Render::ItemEntity&& item, const vec3f& pos) noexcept
{
	item.rotate(mpml::Quaternion<float>::fromAxis(vec3f{ 1, 0, 0 }, mpml::Angle<>::fromDegrees(90.f)));
	item.setPosition(mpml::floor(pos) + vec3f{ 0.5, 0.01, 0.5 });

	m_items.emplace_back(std::move(item));
}

void Entities::EntityChunk::draw(const Render::Shader& shader, const Render::Texture& items_texture) noexcept
{
	for (auto& i : m_items)
		i.draw(shader, items_texture);
}
