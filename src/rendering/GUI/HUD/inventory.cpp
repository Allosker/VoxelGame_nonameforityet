#include "inventory.hpp"



// =====================
// Construction/Destruction
// =====================

Render::GUI::Inventory::Inventory(const Texturing::Texture& texture_inventory, const Texturing::Texture& texture_slot, const Font* font_)
	: m_inventory{ g_size_inventory, {g_size_inventory.x / 2, g_size_inventory.y / 2}, types::Rect<types::uvs>{{}, texture_inventory.getSize()} },
	m_moving_item{ ItemTypeManager::g_size_gui_block, { ItemTypeManager::g_size_gui_block.x / 2, ItemTypeManager::g_size_gui_block.y / 2 }, types::Rect<types::uvs>{}, {}, *font },
	font{ font_ }
{
	create_slots(texture_slot);
	create_slots(texture_slot);
	create_slots(texture_slot);

	m_moving_item.setScale(Hotbar::g_scale_item_coef_hover);
	m_moving_item.text.setScale(ItemStack2D::g_scale_text_hover);
}


// =====================
// Actors
// =====================

void Render::GUI::Inventory::update(const Window& window, const ItemTypeManager& itm, Hotbar& hotbar) noexcept
{
	if (!m_activated)
		return;

	bool isWithinSlot{ false };
	bool isWithinHotbar{ false };

	vec2f point{ Window::toGUICoordinates(window, window.getMousePos()) };

	for (std::size_t i{}; i < m_slots.size(); i++)
		if (m_slots[i].contains(point))
		{
			m_cursor = i;
			m_items_slots[i].setScale(Hotbar::g_scale_item_coef_hover);
			m_items_slots[i].text.setScale(ItemStack2D::g_scale_text_hover);
			isWithinSlot = true;
		}
		else
		{
			m_items_slots[i].setScale(Hotbar::g_scale_item_coef_rest);
			m_items_slots[i].text.setScale(ItemStack2D::g_scale_text_rest);
		}

	if (!isWithinSlot)
		for (std::size_t i{}; i < hotbar.getSlots().size(); i++)
			if (hotbar.getSlots()[i].contains(point))
			{
				m_cursor = i;
				hotbar.getItems()[i].setScale(Hotbar::g_scale_item_coef_hover);
				hotbar.getItems()[i].text.setScale(ItemStack2D::g_scale_text_hover);
				hotbar.getSlots()[i].setScale(Hotbar::g_scale_hotbar_coef_hover);	
				isWithinHotbar = true;
				isWithinSlot = true;
			}
			else
			{
				hotbar.getItems()[i].setScale(Hotbar::g_scale_item_coef_rest);
				hotbar.getSlots()[i].setScale(Hotbar::g_scale_hotbar_coef_rest);
				hotbar.getItems()[i].text.setScale(ItemStack2D::g_scale_text_rest);
			}

	// Set Moving Object
	if (window.isMouseButtonPressedOnce(Buttons::Mouse::Left) && !m_clickedOnce)
	{
		if (isWithinSlot)
		{
			m_clicked_slot = m_cursor;
			m_clickedOnce = true;

			m_wasWithinHotbar = isWithinHotbar;

			auto& item{ isWithinHotbar ? hotbar.getItems()[m_cursor] : m_items_slots[m_cursor] };
			if (item.stack_item.id)
			{
				m_moving_item.updateSprite(mapTextureUvs(item.stack_item.id, itm));
				m_moving_item.stack_item = item.stack_item;
				m_moving_item.count = item.count;
				m_moving_item.text.setPosition(item.text.getPosition());
				m_moving_item.update_text();

				item.shouldBeDrawn = false;

				m_draw_moving = true;
			}
		}
	}

	if (window.isMouseButtonReleased(Buttons::Mouse::Left) && m_clickedOnce)
	{
		auto& old_slot_item = (m_wasWithinHotbar ? hotbar.getItems()[m_clicked_slot] : m_items_slots[m_clicked_slot]);

		if (isWithinSlot)
		{
			auto& slot_item = (isWithinHotbar ? hotbar.getItems()[m_cursor] : m_items_slots[m_cursor]);
			
			if (slot_item.stack_item.id == 0 || slot_item.stack_item.id == m_moving_item.stack_item.id)
			{
				slot_item.stack_item = m_moving_item.stack_item;
				
				// If there are extra items
				if (auto extra = addItems(slot_item, m_moving_item.count); extra && m_clicked_slot != m_cursor)
				{
					slot_item.count = slot_item.max_count;
					m_moving_item.count = extra;

					slot_item.text.setPosition(m_moving_item.text.getPosition());
					slot_item.update_text();

					old_slot_item.count = extra;
					old_slot_item.shouldBeDrawn = true;
					old_slot_item.update_text();
				}
				else
				{
					int x = 0;
					if (m_clicked_slot != m_cursor)
						slot_item.count += m_moving_item.count;

					slot_item.text.setPosition(m_moving_item.text.getPosition());
					slot_item.update_text();

					if (m_clicked_slot != m_cursor)
						old_slot_item.disable();
				}


				slot_item.shouldBeDrawn = true;
				m_moving_item.disable();

				if (!isWithinHotbar && slot_item.stack_item.id)
					slot_item.updateSprite(mapTextureUvs(slot_item.stack_item.id, itm));
			}
			else
			{
				auto stack_item = slot_item.stack_item;
				auto count = slot_item.count;
				auto textpos = slot_item.text.getPosition();

				slot_item.updateSprite(mapTextureUvs(old_slot_item.stack_item.id, itm));
				slot_item.stack_item = old_slot_item.stack_item;
				slot_item.count = old_slot_item.count;
				slot_item.text.setPosition(old_slot_item.text.getPosition());
				slot_item.update_text();
				
				old_slot_item.updateSprite(mapTextureUvs(stack_item.id, itm));
				old_slot_item.stack_item = stack_item;
				old_slot_item.count = count;
				old_slot_item.text.setPosition(textpos);
				old_slot_item.update_text();

				old_slot_item.shouldBeDrawn = true;
			}
		}
		else
			old_slot_item.shouldBeDrawn = true;

		m_clickedOnce = false;
		m_draw_moving = false;
	}


	if (m_draw_moving && window.isMouseButtonPressed(Buttons::Mouse::Left))
			m_moving_item.setPosition(point);
}

void Render::GUI::Inventory::newPairOfSlots(const Texturing::Texture& texture_slot) noexcept
{
	if (m_slots.size() < g_max_nb_slots)
		create_slots(texture_slot);
}

bool Render::GUI::Inventory::addItem(const GameWorld::Inventory::Item& item, uint16 count, const ItemTypeManager& itm) noexcept
{
	for (auto& i : m_items_slots)
	{
		if (i.stack_item.id == item.id && (i.count + count) <= i.max_count)
		{
			i.stack_item = item;
			i.updateSprite(mapTextureUvs(i.stack_item.id, itm));
			i.count += count;
			i.text.setPosition(vec3f{ i.getPosition(), 0 });
			i.shouldBeDrawn = true;
			i.update_text();
			return true;
		}
	}

	for (auto& i : m_items_slots)
	{
		if (i.stack_item.id == 0 && (i.count + count) <= i.max_count)
		{
			i.stack_item = item;
			i.updateSprite(mapTextureUvs(i.stack_item.id, itm));
			i.count += count;
			i.text.setPosition(vec3f{ i.getPosition(), 0 });
			i.shouldBeDrawn = true;
			i.update_text();
			return true;
		}
	}

	return false;
}

uint16 Render::GUI::Inventory::addItems(const ItemStack2D& item_stack, uint16 count) noexcept
{
	if (item_stack.count + count <= item_stack.max_count)
		return 0;

	return count - (item_stack.max_count - item_stack.count);
}

bool Render::GUI::Inventory::removeItem(const GameWorld::Inventory::Item& item, uint16 count) noexcept
{
	for (auto& i : m_items_slots)
	{
		if ((i.stack_item.id == item.id || i.stack_item.id == 0) && count <= i.count)
		{
			i.count -= count;
			return true;
		}
	}

	return false;
}

void Render::GUI::Inventory::enable(Window& window, Hotbar& hotbar) noexcept
{
	m_activated = true;
	window.alternateCursorVisibility();
	hotbar.disable();
}

void Render::GUI::Inventory::disable(Window& window, Hotbar& hotbar) noexcept
{
	m_activated = false;
	window.alternateCursorVisibility();
	hotbar.enable();
}

void Render::GUI::Inventory::process(Window& window, Hotbar& hotbar) noexcept
{
	if (m_activated)
		disable(window, hotbar);
	else
		enable(window, hotbar);
}

void Render::GUI::Inventory::draw(const Shader& shader, const Shader& text_shader, const Texturing::Texture& texture_inventory, const Texturing::Texture& texture_slot, const Render::Texturing::Texture& texture_block_gui_atlas, const ItemTypeManager& itm) noexcept
{
	if (!m_activated)
		return;

	texture_inventory.bind();
	m_inventory.draw_transparent(shader);


	texture_slot.bind();
	for (auto& i : m_slots)
		i.draw_transparent(shader);


	for (std::size_t i{}; i < m_items_slots.size(); i++)
		if (m_items_slots[i].shouldBeDrawn && m_items_slots[i].stack_item.id != 0 && m_items_slots[i].count != 0)
		{
			texture_block_gui_atlas.bind();
			m_items_slots[i].draw(shader, text_shader);
		}

	texture_block_gui_atlas.bind();
	if (m_draw_moving)
		m_moving_item.draw(shader, text_shader);
}


// =====================
// Private Actors
// =====================

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
			ItemTypeManager::g_size_gui_block, { ItemTypeManager::g_size_gui_block.x / 2, ItemTypeManager::g_size_gui_block.y / 2 }, types::Rect<types::uvs>{}, {}, *font
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
			ItemTypeManager::g_size_gui_block, { ItemTypeManager::g_size_gui_block.x / 2, ItemTypeManager::g_size_gui_block.y / 2 }, types::Rect<types::uvs>{}, {}, *font
		}
	);
	m_items_slots.back().setPosition(m_slots.back().getPosition());
}
