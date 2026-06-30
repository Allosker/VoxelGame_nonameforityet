#include "guiInventory.hpp"

#include <print>

// =====================
// Construction/Destruction
// =====================

render::gui::GuiInventory::GuiInventory(const Inventory& inv)
{
	change_board(inv);
}


// =====================
// Actors
// =====================

void render::gui::GuiInventory::update(const Inventory& inv, vec2f mouse_pos) noexcept
{
	if (!m_visible)
		return;

	if (inv.getBoardSize() != m_size)
		change_board(inv);

	update_index(mouse_pos);

	if (m_index)
		m_item_slots.at(*m_index).change_count(1);
}

void render::gui::GuiInventory::change_board(const Inventory& inv) noexcept
{
	// Board
	m_board.setScale(c_scale);

	m_size = inv.getBoardSize();
	m_size_in_slots = inv.getSize();
	const auto* board = getCurrentBoardTexture();

	m_board.updateSprite({ {}, board->getSize() }, board->getSize());
	m_board.setPosition(-m_board.getSize() / 2.f + vec2f{0, m_board.getSize().y / 3.f});


	// Slots
	m_item_slots.clear();
	m_item_slots.reserve(inv.getItemStacks().size());

	constexpr float outline{ c_outline_thickness_px * c_scale };

	const vec2f board_size_slots = inv.getSize();

	m_slot_size = {
		(m_board.getSize().x - outline * 2.f) / board_size_slots.x,
		(m_board.getSize().y - outline * 2.f) / board_size_slots.y
	};
	

	const vec2f base{
		m_board.getPosition().x + outline,
		m_board.getPosition().y + m_board.getSize().y - outline };
	

	for (int32 y{ (int32)board_size_slots.y }; y > 0; y--)
		for (int32 x{}; x < board_size_slots.x; x++)
		{
			vec2f slot_pos{
				base.x + m_slot_size.x * x,
				base.y - m_slot_size.y * y };

			m_item_slots.emplace_back(m_slot_size, slot_pos);
		}
	
}

void render::gui::GuiInventory::update_index(vec2f mouse_pos) noexcept
{
	constexpr float outline{ c_outline_thickness_px * c_scale };

	vec2f board_start{ m_board.getPosition() + outline };
	vec2f board_end  { m_board.getPosition() + m_board.getSize() - outline };

	if (mouse_pos.x > board_start.x && mouse_pos.x < board_end.x &&
		mouse_pos.y > board_start.y && mouse_pos.y < board_end.y
		)
	{
		vec2f converted_mp{ mouse_pos.x - board_start.x, mouse_pos.y - board_start.y };
		vec2iu index2d{ static_cast<uint32>(converted_mp.x / m_slot_size.x), static_cast<uint32>(converted_mp.y / m_slot_size.y) };

		m_index = std::make_optional(index2d.x + index2d.y * m_size_in_slots.x);
	}
	else
		m_index = std::nullopt;
}

void render::gui::GuiInventory::draw() noexcept
{
	if (!m_visible)
		return;

	getCurrentBoardTexture()->bind();
	m_board.draw_transparent(Resources::get().s_gui);


	for (auto& i : m_item_slots)
		i.draw();
}


const render::Texture* const render::gui::GuiInventory::getCurrentBoardTexture() const noexcept
{
	switch (m_size)
	{
	case Inventory::BoardSize::Small:
		return &Resources::get().t_inventory_small;
		break;

	case Inventory::BoardSize::Medium:
		return &Resources::get().t_inventory_medium;
		break;

	case Inventory::BoardSize::Big:
		return &Resources::get().t_inventory_big;
		break;
	}
}
