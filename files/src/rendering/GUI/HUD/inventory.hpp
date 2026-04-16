#pragma once // inventory.hpp
// MIT
// Allosker ------------------------------
// =========+
// Small class for inventory management
// ---------------------------------------

#include <vector>

#include "rendering/GUI/Items/itemTypeManager.hpp"
#include "rendering/GUI/shapes/rectangle.hpp"
#include "window_and_inputs/window.hpp"

#include "hotbar.hpp"

#include "world/items/item.hpp"

#include "rendering/assets_managing/texturing/texture.hpp"


namespace Render::GUI
{

	class Inventory
	{
	public:


		Inventory(const Texturing::Texture& texture_inventory, const Texturing::Texture& texture_slot);




		void enable(Wai::Window& window, Hotbar& hotbar) noexcept;
		void disable(Wai::Window& window, Hotbar& hotbar) noexcept;
		void process(Wai::Window& window, Hotbar& hotbar) noexcept;

		void draw(const Shader& shader, const Texturing::Texture& texture_inventory, const Texturing::Texture& texture_slot, const Render::Texturing::Texture& texture_block_gui_atlas, const ItemTypeManager& itm) noexcept;


	private:

		void create_slots(const Texturing::Texture& texture_slot) noexcept;


	public:

		static constexpr vec2f g_slot_size{ 80 * 1.5 };
		static constexpr vec2f g_size_inventory{ Wai::Window::g_guiViewSize.x / 4 * 1.5,  Wai::Window::g_guiViewSize.y / 2 * 1.5 }; 

		static constexpr std::size_t g_max_nb_slots{ 12 };

	private:

		Rectangle m_inventory;
		std::vector<std::pair<Rectangle, GameWorld::Inventory::Item>> m_slots;
		std::vector<Rectangle> m_items_slots;

		bool m_activated{ true };

	};

}