#include "inventory.hpp"


Render::GUI::Inventory::Inventory(const Texturing::Texture& texture_inventory, const Texturing::Texture& texture_slot, const Font* font_)
	: m_inventory{ g_size_inventory, {g_size_inventory.x / 2, g_size_inventory.y / 2}, types::Rect<types::uvs>{{}, texture_inventory.getSize()} },
	m_moving_item{ ItemTypeManager::g_size_gui_block, { ItemTypeManager::g_size_gui_block.x / 2, ItemTypeManager::g_size_gui_block.y / 2 }, types::Rect<types::uvs>{} },
	font{ font_ }
{
	create_slots(texture_slot);
	create_slots(texture_slot);
	create_slots(texture_slot);

	m_moving_item.setScale(g_scale_coef);
}

void Render::GUI::Inventory::update(const Wai::Window& window, const ItemTypeManager& itm, Hotbar& hotbar) noexcept
{
	if (!m_activated)
		return;

	bool isWithinSlot{ false };
	bool isWithinHotbar{ false };

	vec2f point{ Wai::Window::toGUICoordinates(window, window.getMousePos()) };

	for (std::size_t i{}; i < m_slots.size(); i++)
		if (m_slots[i].contains(point))
		{
			m_cursor = i;
			m_items_slots[i].setScale(g_scale_coef);
			isWithinSlot = true;
		}
		else
			m_items_slots[i].setScale(1);

	for (std::size_t i{}; i < hotbar.getSlots().size(); i++)
		if (hotbar.getSlots()[i].contains(point))
		{
			m_cursor = i;
			hotbar.getItemsRenders()[i].setScale(g_scale_coef);
			isWithinHotbar = true;
			isWithinSlot = true;
		}
		else
			hotbar.getItemsRenders()[i].setScale(1);


	if (window.isMouseButtonPressedOnce(Wai::Buttons::Mouse::Left) && !m_clickedOnce)
	{
		if (isWithinSlot)
		{
			m_clicked_slot = m_cursor;
			m_clickedOnce = true;

			m_wasWithinHotbar = isWithinHotbar;

			types::type_id current_id{ m_wasWithinHotbar ? hotbar.getSlots()[m_clicked_slot].stack_item.id : m_items_slots[m_clicked_slot].stack_item.id };
			if (current_id)
				m_moving_item.updateSprite(mapTextureUvs(current_id, itm));

			m_draw_moving = true;
		}
	}

	if (window.isMouseButtonReleased(Wai::Buttons::Mouse::Left) && m_clickedOnce)
	{
		if (isWithinSlot)
		{
			const auto& item = m_wasWithinHotbar ? hotbar.getSlots()[m_clicked_slot] : m_items_slots[m_clicked_slot];

			if (!isWithinHotbar)
			{
				m_items_slots[m_cursor].stack_item = item.stack_item;
				m_items_slots[m_cursor].count = item.count;
				m_items_slots[m_cursor].text.setPosition(item.text.getPosition());
				m_items_slots[m_cursor].update_text();
			}
			else
			{
				hotbar.getSlots()[m_cursor].stack_item = item.stack_item;
				hotbar.getSlots()[m_cursor].count = item.count;
				hotbar.getSlots()[m_cursor].text.setPosition(item.text.getPosition());
				hotbar.getSlots()[m_cursor].update_text();
			}
		}

		m_clickedOnce = false;
		m_draw_moving = false;
	}


	if (window.isMouseButtonPressed(Wai::Buttons::Mouse::Left))
			m_moving_item.setPosition(point);
}

void Render::GUI::Inventory::newPairOfSlots(const Texturing::Texture& texture_slot) noexcept
{
	if (m_slots.size() < g_max_nb_slots)
		create_slots(texture_slot);
}

bool Render::GUI::Inventory::addItem(const GameWorld::Inventory::Item& item, int64 count) noexcept
{
	for (auto& i : m_items_slots)
	{
		if (i.stack_item.id == item.id || i.stack_item.id == 0 && count >= 1)
		{
			i.stack_item = item;
			count -= 1;
			return true;
		}
	}

	return false;
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
		i.draw_transparent(shader);


	for (std::size_t i{}; i < m_items_slots.size(); i++)
		if (m_items_slots[i].stack_item.id != 0)
		{
			texture_block_gui_atlas.bind();
			m_items_slots[i].updateSprite(mapTextureUvs(m_items_slots[i].stack_item.id, itm));
			m_items_slots[i].draw(shader);
		}

	texture_block_gui_atlas.bind();
	if (m_draw_moving)
		m_moving_item.draw_transparent(shader);
}

/*private*/ void Render::GUI::Inventory::create_slots(const Texturing::Texture& texture_slot) noexcept
{
	int64 size_mult = static_cast<int64>(m_items_slots.size() / 2);

	// Create first slot left
	m_slots.emplace_back(Rectangle{ g_slot_size, { g_slot_size.x / 2, g_slot_size.y / 2 }, types::Rect<types::uvs>{ {}, texture_slot.getSize() } });
	m_slots.back().setPosition({
		173,
		336 - g_slot_size.y * size_mult });
	// Create corresponding item
	m_items_slots.emplace_back
	(ItemStack2D
		{
			ItemTypeManager::g_size_gui_block, { ItemTypeManager::g_size_gui_block.x / 2, ItemTypeManager::g_size_gui_block.y / 2 }, types::Rect<types::uvs>{}, {},* font
		} 
	);
	m_items_slots.back().setPosition(m_slots.back().getPosition());

	// Create second slot right
	m_slots.emplace_back(Rectangle{ g_slot_size, { g_slot_size.x / 2, g_slot_size.y / 2 }, types::Rect<types::uvs>{ {}, texture_slot.getSize() } });
	m_slots.back().setPosition({
		173 + g_slot_size.x,
		336 - g_slot_size.y * size_mult });
	// Create corresponding item
	m_items_slots.emplace_back
	(ItemStack2D
		{
			ItemTypeManager::g_size_gui_block, { ItemTypeManager::g_size_gui_block.x / 2, ItemTypeManager::g_size_gui_block.y / 2 }, types::Rect<types::uvs>{}, {},*font
		}
	);
	m_items_slots.back().setPosition(m_slots.back().getPosition());
}
