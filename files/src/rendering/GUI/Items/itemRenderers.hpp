#pragma once // itemRenderers.hpp
// MIT
// Allosker ------------------------------
// =========+
// Two classes to manage 3D world items and 2D GUI items
// ---------------------------------------


#include "rendering/GUI/shapes/rectangle.hpp"

#include "rendering/GUI/items/itemTypeManager.hpp"
#include "world/items/item.hpp"


//namespace Render::GUI
//{
//	class ItemGUI
//		: public Rectangle 
//	{
//	public:
//
//		ItemGUI(const GameWorld::Inventory::Item& item, const ItemTypeManager& manager)
//			: Rectangle(g_GUI_size, { g_GUI_size.x / 2, g_GUI_size.y / 2 }, Render::UvAtlas{ manager.getType(item.id).pos_in_atlas, g_texture_size})
//		{
//		}
//
//		void draw(const Shader& shader)
//		{
//			Rectangle::draw(shader);
//		}
//
//	};
//}

namespace Render
{
	class ItemWorld
	{

	};

}