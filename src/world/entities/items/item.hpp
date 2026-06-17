#pragma once // item.hpp
// Allosker ------------------------------
// =========+
// Simple struct for the item
// ---------------------------------------

#include "utilities/types.hpp"

namespace GameWorld::Inventory
{
	struct Item
	{
		types::type_id id{ 0 };
	};
}