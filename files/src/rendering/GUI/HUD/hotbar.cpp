#include "hotbar.hpp"

// =====================
// Construction/Destruction
// =====================

Render::GUI::Hotbar::Hotbar(const Render::Texturing::Texture& texture_slot, const ItemTypeManager& itm)
{
	create_new_slots(texture_slot);
}


// =====================
// Getters
// =====================

Render::GUI::Rectangle* Render::GUI::Hotbar::getSlotRender_at(std::size_t index) noexcept
{
	if (index < 0 || index >= m_slots.size())
		return nullptr;

	return &m_slots[index].first;
}

std::optional<GameWorld::Inventory::Item> Render::GUI::Hotbar::getSlot_at(std::size_t index) const noexcept
{
	if (index < 0 || index >= m_slots.size())
		return std::nullopt;

	return std::make_optional(m_slots[index].second);
}


// =====================
// Setters
// =====================

void Render::GUI::Hotbar::setCurrentSlot(GameWorld::Inventory::Item item, const ItemTypeManager& itm) noexcept
{
	m_slots[m_cursor_position].second = item;
}


// =====================
// Actors
// =====================

GameWorld::Inventory::Item Render::GUI::Hotbar::nextSlot(int8 index) noexcept
{
	if (m_disabled)
		return m_slots[m_cursor_position].second;

	m_cursor_position += index;

	int64 size = m_slots.size();
	m_cursor_position = (m_cursor_position % size + size) % size;

	m_cursor_changed = true;

	return m_slots[m_cursor_position].second;
}

void Render::GUI::Hotbar::newPairOfSlots(const Render::Texturing::Texture& texture_slot) noexcept
{
	if (m_slots.size() < g_max_nb_slots)
	{
		create_new_slots(texture_slot);

		if (m_cursor_position > m_slots.size() / 2)
			m_cursor_position--;
		else
			m_cursor_position++;

		last_cursor = m_cursor_position;
	}
}

void Render::GUI::Hotbar::disable() noexcept
{
	m_cursor_changed = false;
	last_cursor = -1;
	m_disabled = true;

	m_slots[m_cursor_position].first.setScale(1);
	m_slots[m_cursor_position].first.move({ 0, -g_slot_size.x / 2 });

	m_items_slots[m_cursor_position].setScale(1);
	m_items_slots[m_cursor_position].move({ 0, -g_slot_size.x / 2 });

	for (size_t i{}; i < m_slots.size(); i++)
	{
		m_slots[i].first.setScale(1.3);
		m_items_slots[i].setScale(1.3);
	}
}

void Render::GUI::Hotbar::enable() noexcept
{
	m_disabled = false;

	for (size_t i{}; i < m_slots.size(); i++)
	{
		m_slots[i].first.setScale(1);
		m_items_slots[i].setScale(1);
	}
}

void Render::GUI::Hotbar::draw(
	const Shader& shader, 
	const Render::Texturing::Texture& texture_slot, const Render::Texturing::Texture& texture_block_gui_atlas, 
	const ItemTypeManager& itm) noexcept
{
	for (std::size_t i{}; i < m_slots.size(); i++)
	{
		if (!m_disabled)
		{

			if (!m_cursor_changed && m_cursor_position != last_cursor && i == m_cursor_position)
			{
				m_slots[i].first.setScale(1.3);
				m_slots[i].first.move({ 0, g_slot_size.x / 2 });

				m_items_slots[i].setScale(1.3);
				m_items_slots[i].move({ 0, g_slot_size.x / 2 });


				last_cursor = m_cursor_position;
			}

			if (m_cursor_changed && i == last_cursor)
			{
				m_slots[i].first.setScale(1);
				m_slots[i].first.move({ 0, -g_slot_size.x / 2 });

				m_items_slots[i].setScale(1);
				m_items_slots[i].move({ 0, -g_slot_size.x / 2 });

				m_cursor_changed = false;
				last_cursor = -1;
			}
		}

		if (m_slots[i].second.id != 0)
		{
			m_items_slots[i].updateSprite(mapTextureUvs(m_slots[i].second.id, itm));

			texture_block_gui_atlas.bind();
			m_items_slots[i].draw_transparent(shader);
		}

		texture_slot.bind();
		m_slots[i].first.draw_transparent(shader);
	}
}

/*private*/ void Render::GUI::Hotbar::create_new_slots(const Render::Texturing::Texture& texture_slot) noexcept
{
	int64 size_mult = m_slots.size() / 2 + 1;

	// Set first Slot left
	m_slots.emplace_back
	(std::pair<Rectangle, GameWorld::Inventory::Item>
	{
		{
			g_slot_size, { g_slot_size.x / 2, g_slot_size.y / 2 }, UvPixels{ {}, texture_slot.getSize() }
		}, {}
	});
	m_slots.back().first.setPosition({
		static_cast<float>(-g_slot_size.x / 2) * size_mult - g_slot_size.x * size_mult,
		-Wai::Window::g_guiViewSize.y / 2 + g_slot_size.y / 1.5 });
	// Set its corresponding item
	m_items_slots.emplace_back(Rectangle{ {ItemTypeManager::g_size_gui_block}, {ItemTypeManager::g_size_gui_block.x / 2, ItemTypeManager::g_size_gui_block.y / 2 }, UvPixels{} });
	m_items_slots.back().setPosition(m_slots.back().first.getPosition());


	// Set second Slot right
	m_slots.emplace_back
	(std::pair<Rectangle, GameWorld::Inventory::Item>
	{
		{
			g_slot_size, { g_slot_size.x / 2, g_slot_size.y / 2}, UvPixels{ {}, texture_slot.getSize() }
		}, {}
	});
	m_slots.back().first.setPosition({
		static_cast<float>(g_slot_size.x / 2) * size_mult + g_slot_size.x * size_mult,
		-Wai::Window::g_guiViewSize.y / 2 + g_slot_size.y / 1.5 });
	// Set its corresponding item
	m_items_slots.emplace_back(Rectangle{ {ItemTypeManager::g_size_gui_block}, {ItemTypeManager::g_size_gui_block.x / 2, ItemTypeManager::g_size_gui_block.y / 2 }, UvPixels{} });
	m_items_slots.back().setPosition(m_slots.back().first.getPosition());


	// Sort the array for left-to-right wrapping
	std::sort(m_slots.begin(), m_slots.end(), [](const auto& a, const auto& b) 
		{
			return a.first.getPosition().x < b.first.getPosition().x;
		});

	// Sort the array item list for the same purpose
	std::sort(m_items_slots.begin(), m_items_slots.end(), [](const auto& a, const auto& b)
		{
			return a.getPosition().x < b.getPosition().x;
		});
}
