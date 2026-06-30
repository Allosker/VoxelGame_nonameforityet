#include "itemStack.hpp"



ItemStack ItemStack::add_item(ItemStack other) noexcept
{
	if (other.empty())
		return other;

	if (match(other))
	{
		if (empty())
		{
			*this = other;
			return {};
		}

	
		if (m_count + other.m_count <= c_max_count)
		{
			add(other.m_count);
			return {};
		}

		uint8 leftover = (m_count + other.m_count) - c_max_count;

		m_count = c_max_count;

		return ItemStack{ m_type, leftover };
	}

	return other;
}

ItemStack ItemStack::take_item(uint8 n) noexcept
{
	if (empty() || n == 0)
		return {};

	if (n >= m_count)
	{
		ItemStack temp{ *this };
		clear();
		return temp;
	}

	remove(n);
	return { m_type, n };
}

bool ItemStack::match(ItemStack other) const noexcept
{
	return m_type.id == other.m_type.id;
}
