#include "hotbar.hpp"

// =====================
// Construction/Destruction
// =====================

render::gui::Hotbar::Hotbar(const render::Texture& texture_slot, const ItemTypeManager& itm, const Font* font_)
	: font{ font_ }
{
	create_new_slots(texture_slot);
}


// =====================
// Getters
// =====================

render::gui::elems::Rectangle* render::gui::Hotbar::getSlotRender_at(std::size_t index) noexcept
{
	if (index < 0 || index >= m_items_slots.size())
		return nullptr;

	return &m_items_slots[index];
}

std::optional<Data::Item> render::gui::Hotbar::getSlot_at(std::size_t index) const noexcept
{
	if (index < 0 || index >= m_items_slots.size())
		return std::nullopt;

	return std::make_optional(m_items_slots[index].stack_item);
}


// =====================
// Setters
// =====================

void render::gui::Hotbar::setCurrentSlot(Data::Item item, const ItemTypeManager& itm) noexcept
{
	m_items_slots[m_cursor_position].stack_item = item;
	m_items_slots[m_cursor_position].count = 1;
	m_items_slots[m_cursor_position].shouldBeDrawn = true;
}


// =====================
// Actors
// =====================

Data::Item render::gui::Hotbar::nextSlot(int8 index) noexcept
{
	if (m_disabled)
		return m_items_slots[m_cursor_position].stack_item;

	m_cursor_position += index;

	int64 size = m_items_slots.size();
	m_cursor_position = (m_cursor_position % size + size) % size;

	m_cursor_changed = true;

	return m_items_slots[m_cursor_position].stack_item;
}

void render::gui::Hotbar::newPairOfSlots(const render::Texture& texture_slot) noexcept
{
	if (m_items_slots.size() < g_max_nb_slots)
	{
		create_new_slots(texture_slot);

		if (m_cursor_position > m_items_slots.size() / 2)
			m_cursor_position--;
		else
			m_cursor_position++;

		last_cursor = m_cursor_position;
	}
}

bool render::gui::Hotbar::addItem(const Data::Item& item, int64 count, const ItemTypeManager& itm) noexcept
{
	for (auto& i : m_items_slots)
	{
		if (count >= 1 && i.stack_item.id == item.id && (i.count + count) <= i.max_count)
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
		if (count >= 1 && i.stack_item.id == 0 && (i.count + count) <= i.max_count)
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

bool render::gui::Hotbar::removeItem(const Data::Item& item, int64 count) noexcept
{
	auto& i = m_items_slots[m_cursor_position];
	if (count <= i.count && (i.stack_item.id == item.id || i.stack_item.id == 0))
	{
		i.count -= count;

		if (i.count <= 0)
		{
			i.stack_item.id = 0;
			i.shouldBeDrawn = false;
			return true;
		}

		i.update_text();
		return true;
	}

	return false;
}

void render::gui::Hotbar::disable() noexcept
{
	m_cursor_changed = false;
	last_cursor = -1;
	m_disabled = true;

	m_items_slots[m_cursor_position].setScale(g_scale_item_coef_rest);
	m_items_slots[m_cursor_position].move({ 0, -g_slot_size.x / 2 });

	m_slots[m_cursor_position].setScale(g_scale_hotbar_coef_rest);
	m_slots[m_cursor_position].move({ 0, -g_slot_size.x / 2 });
}

void render::gui::Hotbar::enable() noexcept
{
	m_disabled = false;

	for (size_t i{}; i < m_items_slots.size(); i++)
	{
		m_items_slots[i].setScale(g_scale_item_coef_rest);
		m_slots[i].setScale(g_scale_hotbar_coef_rest);
	}
} 

void render::gui::Hotbar::draw(
	const Shader& shader, 
	const Shader& text_shader,
	const render::Texture& texture_slot, const render::Texture& texture_block_gui_atlas, 
	const ItemTypeManager& itm) noexcept
{
	for (std::size_t i{}; i < m_items_slots.size(); i++)
	{
		if (!m_disabled)
		{

			if (!m_cursor_changed && m_cursor_position != last_cursor && i == m_cursor_position)
			{
				m_slots[i].setScale(g_scale_hotbar_coef_hover);
				m_slots[i].move({ 0, g_slot_size.x / 2 });

				m_items_slots[i].setScale(g_scale_item_coef_hover);
				m_items_slots[m_cursor_position].text.setScale(ItemStackGUI::g_scale_text_hover);
				m_items_slots[i].move({ 0, g_slot_size.x / 2 });


				last_cursor = m_cursor_position;
			}

			if (m_cursor_changed && i == last_cursor)
			{
				m_slots[i].setScale(g_scale_hotbar_coef_rest);
				m_slots[i].move({ 0, -g_slot_size.x / 2 });

				m_items_slots[i].setScale(g_scale_item_coef_rest);
				m_items_slots[m_cursor_position].text.setScale(ItemStackGUI::g_scale_text_rest);
				m_items_slots[i].move({ 0, -g_slot_size.x / 2 });

				m_cursor_changed = false;
				last_cursor = -1;
			}
		}

		if (m_items_slots[i].shouldBeDrawn && m_items_slots[i].count != 0 && m_items_slots[i].stack_item.id != 0)
		{
			m_items_slots[i].updateSprite(mapTextureUvs(m_items_slots[i].stack_item.id, itm));

			texture_block_gui_atlas.bind();
			m_items_slots[i].draw(shader, text_shader);
		}

		texture_slot.bind();
		m_slots[i].draw_transparent(shader);
	}
}

/*private*/ void render::gui::Hotbar::create_new_slots(const render::Texture& texture_slot) noexcept
{
	int64 size_mult = m_items_slots.size() / 2 + 1;

	// Set first Slot left
	m_items_slots.emplace_back
	(ItemStackGUI
	{
		ItemTypeManager::g_size_gui_block, { ItemTypeManager::g_size_gui_block.x / 2, ItemTypeManager::g_size_gui_block.y / 2 }, types::Rect<types::uvs>{}, {},* font
	});
	m_items_slots.back().setPosition({
		static_cast<float>(-g_slot_size.x / 2) * size_mult - g_slot_size.x * size_mult,
		-Window::g_guiViewSize.y / 2 + g_slot_size.y / 1.5 });
	// Set its corresponding item
	m_slots.emplace_back(elems::Rectangle{ g_slot_size, {g_slot_size.x / 2, g_slot_size.y / 2 }, types::Rect<types::uvs>{ {}, texture_slot.getSize() } });
	m_slots.back().setPosition(m_items_slots.back().getPosition());


	// Set second Slot right
	m_items_slots.emplace_back
	(ItemStackGUI
	{
		ItemTypeManager::g_size_gui_block, { ItemTypeManager::g_size_gui_block.x / 2, ItemTypeManager::g_size_gui_block.y / 2 }, types::Rect<types::uvs>{}, {}, *font
	});
	m_items_slots.back().setPosition({
		static_cast<float>(g_slot_size.x / 2) * size_mult + g_slot_size.x * size_mult,
		-Window::g_guiViewSize.y / 2 + g_slot_size.y / 1.5 });
	// Set its corresponding item
	m_slots.emplace_back(elems::Rectangle{ g_slot_size, {g_slot_size.x / 2, g_slot_size.y / 2 }, types::Rect<types::uvs>{ {}, texture_slot.getSize() } });
	m_slots.back().setPosition(m_items_slots.back().getPosition());


	// Sort the array for left-to-right wrapping
	std::sort(m_items_slots.begin(), m_items_slots.end(), [](const auto& a, const auto& b) 
		{
			return a.getPosition().x < b.getPosition().x;
		});

	// Sort the array item list for the same purpose
	std::sort(m_slots.begin(), m_slots.end(), [](const auto& a, const auto& b)
		{
			return a.getPosition().x < b.getPosition().x;
		});
}
