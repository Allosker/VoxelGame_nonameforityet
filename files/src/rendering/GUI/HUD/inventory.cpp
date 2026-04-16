#include "inventory.hpp"


Render::GUI::Inventory::Inventory(const Texturing::Texture& texture_inventory, const Texturing::Texture& texture_slot)
	: m_inventory{ g_size_inventory, {g_size_inventory.x / 2, g_size_inventory.y / 2}, UvPixels{{}, texture_inventory.getSize()} }
{
	create_slots(texture_slot);
	create_slots(texture_slot);
	create_slots(texture_slot);
}


void Render::GUI::Inventory::enable(Wai::Window& window, Hotbar& hotbar) noexcept
{
	m_activated = true;
	window.alternateCursorVisibility();
	hotbar.disable();
}

void Render::GUI::Inventory::disable(Wai::Window& window, Hotbar& hotbar) noexcept
{
	m_activated = false;
	window.alternateCursorVisibility();
	hotbar.enable();
}

void Render::GUI::Inventory::process(Wai::Window& window, Hotbar& hotbar) noexcept
{
	if (m_activated)
		disable(window, hotbar);
	else
		enable(window, hotbar);
}

void Render::GUI::Inventory::draw(const Shader& shader, const Texturing::Texture& texture_inventory, const Texturing::Texture& texture_slot, const Render::Texturing::Texture& texture_block_gui_atlas, const ItemTypeManager& itm) noexcept
{
	if (!m_activated)
		return;

	texture_inventory.bind();
	m_inventory.draw_transparent(shader);


	texture_slot.bind();
	for (auto& i : m_slots)
		i.first.draw_transparent(shader);


	for (std::size_t i{}; i < m_slots.size(); i++)
		if (m_slots[i].second.id != 0)
		{
			m_items_slots[i].updateSprite(mapTextureUvs(m_slots[i].second.id, itm));

			texture_block_gui_atlas.bind();
			m_items_slots[i].draw_transparent(shader);
		}
}

/*private*/ void Render::GUI::Inventory::create_slots(const Texturing::Texture& texture_slot) noexcept
{
	int64 size_mult = static_cast<int64>(m_slots.size() / 2);

	// Create first slot left
	m_slots.emplace_back
	(std::pair<Rectangle, GameWorld::Inventory::Item> 
	{
		{
			g_slot_size, { g_slot_size.x / 2, g_slot_size.y / 2 }, UvPixels{ {}, texture_slot.getSize() }
		}, {1}
	});
	m_slots.back().first.setPosition({
		173,
		336 - g_slot_size.y * size_mult });
	// Create corresponding item
	m_items_slots.emplace_back(Rectangle{ ItemTypeManager::g_size_gui_block, { ItemTypeManager::g_size_gui_block.x / 2, ItemTypeManager::g_size_gui_block.y / 2 }, UvPixels{} });
	m_items_slots.back().setPosition(m_slots.back().first.getPosition());

	// Create second slot right
	m_slots.emplace_back
	(std::pair<Rectangle, GameWorld::Inventory::Item>
	{	
		{
			g_slot_size, { g_slot_size.x / 2, g_slot_size.y / 2 }, UvPixels{ {}, texture_slot.getSize() }
		}, {1}
	});
	m_slots.back().first.setPosition({
		173 + g_slot_size.x,
		336 - g_slot_size.y * size_mult });
	// Create corresponding item
	m_items_slots.emplace_back(Rectangle{ ItemTypeManager::g_size_gui_block, { ItemTypeManager::g_size_gui_block.x / 2, ItemTypeManager::g_size_gui_block.y / 2 }, UvPixels{} });
	m_items_slots.back().setPosition(m_slots.back().first.getPosition());
}
