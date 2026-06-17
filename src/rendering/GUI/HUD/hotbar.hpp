#pragma once // hotbar.hpp
// MIT
// Allosker ------------------------------
// =========+
// Manages the hotbar within the HUD and each item associated with it
// ---------------------------------------

#include <optional>
#include <vector>

#include "rendering/GUI/Items/itemTypeManager.hpp"
#include "rendering/GUI/shapes/rectangle.hpp"
#include "utilities/window.hpp"

#include "world/entities/items/item.hpp"

#include "rendering/assets_managing/texturing/texture.hpp"

#include "item_utils_2D.hpp"

namespace Render::GUI
{
	class Hotbar
	{
	public:

		// = Construction/Destruction

		Hotbar(const Render::Texturing::Texture& texture_slot, const ItemTypeManager& itemTypeManager, const Font* font);


		// = Getters

		Rectangle* getSlotRender_at(std::size_t index) noexcept;

		std::optional<GameWorld::Inventory::Item> getSlot_at(std::size_t index) const noexcept;


		GameWorld::Inventory::Item getSelectedItem() const noexcept { return m_items_slots[m_cursor_position].stack_item; }


		std::vector<Rectangle>& getSlots() noexcept { return m_slots; }
		const std::vector<Rectangle>& getSlots() const noexcept { return m_slots; }

		std::vector<ItemStack2D>& getItems() noexcept { return m_items_slots; }
		const std::vector<ItemStack2D>& getItems() const noexcept { return m_items_slots; }


		// = Setters

		void setCurrentSlot(GameWorld::Inventory::Item item, const ItemTypeManager& itm) noexcept;


		// = Actors

		// Returns the next slot based on the index which must be either 1 or -1
		GameWorld::Inventory::Item nextSlot(int8 index) noexcept;

		void newPairOfSlots(const Render::Texturing::Texture& texture_slot) noexcept;

		bool addItem(const GameWorld::Inventory::Item& item, int64 count, const ItemTypeManager& itm) noexcept;
		bool removeItem(const GameWorld::Inventory::Item& item, int64 count) noexcept;


		void disable() noexcept;
		void enable() noexcept;

		void draw(const Shader& shader, const Shader& text_shader, const Render::Texturing::Texture& texture_slot, const Render::Texturing::Texture& texture_block_gui_atlas, const ItemTypeManager& itm) noexcept;


	private:

		void create_new_slots(const Render::Texturing::Texture& texture_slot) noexcept;


	public:

		static constexpr vec2f g_slot_size{ 80 };

		static constexpr std::size_t g_max_nb_slots{ 10 };

		static constexpr float g_scale_item_coef_hover{ 1.4 };
		static constexpr float g_scale_item_coef_rest{ 0.9 };

		static constexpr float g_scale_hotbar_coef_hover{ 1.3 };
		static constexpr float g_scale_hotbar_coef_rest{ 0.9 };


	private:

		std::vector<Rectangle> m_slots;
		std::vector<ItemStack2D> m_items_slots{};

		const Font* font;


		int64 m_cursor_position{};
		int64 last_cursor{ -1 };

		bool m_cursor_changed{ false };
		bool m_disabled{ false };

	};

} // Render::GUI