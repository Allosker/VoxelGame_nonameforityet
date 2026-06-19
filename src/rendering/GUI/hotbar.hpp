#pragma once // hotbar.hpp
// MIT
// Allosker ------------------------------
// =========+
// Manages the hotbar within the HUD and each item associated with it
// ---------------------------------------

#include <optional>
#include <vector>

#include "world/types/itemTypeManager.hpp"
#include "rendering/gui/elements/rectangle.hpp"
#include "utilities/window.hpp"

#include "data/item.hpp"

#include "rendering/texture.hpp"

#include "itemStackGUI.hpp"

namespace render::gui
{
	class Hotbar
	{
	public:

		// = Construction/Destruction

		Hotbar(const render::Texture& texture_slot, const ItemTypeManager& itemTypeManager, const Font* font);


		// = Getters

		elems::Rectangle* getSlotRender_at(std::size_t index) noexcept;

		std::optional<Data::Item> getSlot_at(std::size_t index) const noexcept;


		Data::Item getSelectedItem() const noexcept { return m_items_slots[m_cursor_position].stack_item; }


		std::vector<elems::Rectangle>& getSlots() noexcept { return m_slots; }
		const std::vector<elems::Rectangle>& getSlots() const noexcept { return m_slots; }

		std::vector<ItemStackGUI>& getItems() noexcept { return m_items_slots; }
		const std::vector<ItemStackGUI>& getItems() const noexcept { return m_items_slots; }


		// = Setters

		void setCurrentSlot(Data::Item item, const ItemTypeManager& itm) noexcept;


		// = Actors

		// Returns the next slot based on the index which must be either 1 or -1
		Data::Item nextSlot(int8 index) noexcept;

		void newPairOfSlots(const render::Texture& texture_slot) noexcept;

		bool addItem(const Data::Item& item, int64 count, const ItemTypeManager& itm) noexcept;
		bool removeItem(const Data::Item& item, int64 count) noexcept;


		void disable() noexcept;
		void enable() noexcept;

		void draw(const Shader& shader, const Shader& text_shader, const render::Texture& texture_slot, const render::Texture& texture_block_gui_atlas, const ItemTypeManager& itm) noexcept;


	private:

		void create_new_slots(const render::Texture& texture_slot) noexcept;


	public:

		static constexpr vec2f g_slot_size{ 80 };

		static constexpr std::size_t g_max_nb_slots{ 10 };

		static constexpr float g_scale_item_coef_hover{ 1.4 };
		static constexpr float g_scale_item_coef_rest{ 0.9 };

		static constexpr float g_scale_hotbar_coef_hover{ 1.3 };
		static constexpr float g_scale_hotbar_coef_rest{ 0.9 };


	private:

		std::vector<elems::Rectangle> m_slots;
		std::vector<ItemStackGUI> m_items_slots{};

		const Font* font;


		int64 m_cursor_position{};
		int64 last_cursor{ -1 };

		bool m_cursor_changed{ false };
		bool m_disabled{ false };

	};

} // render::gui