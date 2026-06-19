#pragma once // inventory.hpp
// MIT
// Allosker ------------------------------
// =========+
// Small class for inventory management
// ---------------------------------------

#include <vector>

#include "world/types/itemTypeManager.hpp"
#include "rendering/GUI/elements/rectangle.hpp"
#include "utilities/window.hpp"

#include "hotbar.hpp"

#include "data/item.hpp"

#include "rendering/texture.hpp"

#include "itemStackGUI.hpp"


namespace Render::GUI
{

	class Inventory
	{
	public:

		// = Construction/Destruction

		Inventory(const Texture& texture_inventory, const Texture& texture_slot, const Font* font_);


		// = Actors

		void update(const Window& window, const ItemTypeManager& itm, Hotbar& hotbar) noexcept;

		void newPairOfSlots(const Texture& texture_slot) noexcept;

		bool addItem(const Data::Item& item, uint16 count, const ItemTypeManager& itm) noexcept;
		uint16 addItems(const ItemStackGUI& item_stack, uint16 count) noexcept;
		bool removeItem(const Data::Item& item, uint16 count) noexcept;


		void enable(Window& window, Hotbar& hotbar) noexcept;
		void disable(Window& window, Hotbar& hotbar) noexcept;
		void process(Window& window, Hotbar& hotbar) noexcept;

		void draw(const Shader& shader, const Shader& text_shader, const Texture& texture_inventory, const Texture& texture_slot, const Render::Texture& texture_block_gui_atlas, const ItemTypeManager& itm) noexcept;


	private: // = Actors

		void create_slots(const Texture& texture_slot) noexcept;


	public: // = Global Constants

		static constexpr vec2f g_slot_size{ 80 * 1.5 };
		static constexpr vec2f g_size_inventory{ Window::g_guiViewSize.x / 4 * 1.5,  Window::g_guiViewSize.y / 2 * 1.5 }; 

		static constexpr std::size_t g_max_nb_slots{ 12 };


	private:

		const Font* font;

		Elems::Rectangle m_inventory;
		ItemStackGUI m_moving_item;
		std::vector<Elems::Rectangle> m_slots;
		std::vector<ItemStackGUI> m_items_slots;

		
		size_t m_clicked_slot{};
		std::size_t m_cursor{};

		bool m_wasWithinHotbar{ false };
		bool m_clickedOnce{ false };

		bool m_draw_moving{ false };
		

		bool m_activated{ false };

	};

}