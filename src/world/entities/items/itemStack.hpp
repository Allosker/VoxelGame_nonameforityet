#pragma once // itemStack.hpp
// MIT
// Allosker ------------------------------
// =========+
// Small class dedicated to managing a stack item
// ---------------------------------------

#include "data/item.hpp"
#include <cassert>

class ItemStack
{
public:

	ItemStack(Data::Item type = {}, uint8 count = 0)
		: m_type{type}, m_count{count}
	{ 
	}


	void add(uint8 n) noexcept
	{
		m_count += n;
	}

	void remove(uint8 n)
	{
		assert(n <= m_count && "Cannot remove more than there are items");

		m_count -= n;
		if (m_count == 0)
			clear();
	}

	// If cannot be added, return the Item back
	// If can be partially added, return decremented count
	// If can be added, return empty Item
	ItemStack add_item(ItemStack other) noexcept;

	// Take as many Items as it can and returns the count
	// If cannot take any, return empty Item
	ItemStack take_item(uint8 n) noexcept;

	void clear() noexcept
	{
		m_type = {};
		m_count = 0;
	}

	uint8 item_fits() noexcept
	{

	}


	bool match(ItemStack other) const noexcept;

	bool empty() const noexcept
	{
		return m_count == 0;
	}


public:

	static constexpr uint8 c_max_count{ 100u };


private:

	Data::Item m_type{};
	uint8 m_count{};

};