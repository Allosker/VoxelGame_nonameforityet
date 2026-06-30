#pragma once // inventory.hpp
// MIT
// Allosker ------------------------------
// =========+
// Logic side of the inventory that manages Items only without carring about the rendering side of things.
// ---------------------------------------

#include "utilities/opengl.hpp"
#include "world/entities/items/itemStack.hpp"

class Inventory
{
public:

	enum class BoardSize
		: uint8
	{
		Small = 0,
		Medium = 1,
		Big = 2
	};

public:

	// = Construction/Destruction

	Inventory();


	vec2u8 getSize() const noexcept { return c_number_slots[static_cast<size_t>(m_size)]; }

	BoardSize getBoardSize() const noexcept { return m_size; }
	
	void setBoardSize(BoardSize size) noexcept
	{
		m_size = size;
		m_items.resize(static_cast<uint16>(c_number_slots[static_cast<size_t>(m_size)].x) * static_cast<uint16>(c_number_slots[static_cast<size_t>(m_size)].y));
	}


	const std::vector<ItemStack>& getItemStacks() const noexcept { return m_items; }


public: // = Global Constants

	static constexpr vec2u8 c_number_slots[]{ {6,4}, {8,5}, {12,6} };


private:

	std::vector<ItemStack> m_items;
	BoardSize m_size;


	bool m_dirty{ false };

};