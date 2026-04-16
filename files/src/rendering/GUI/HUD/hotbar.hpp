#pragma once // hotbar.hpp
// MIT
// Allosker ------------------------------
// =========+
// Manages the horbar within the HUD and each item associated with it
// ---------------------------------------

#include <optional>
#include <vector>

#include "rendering/GUI/Items/itemTypeManager.hpp"
#include "rendering/GUI/shapes/rectangle.hpp"
#include "window_and_inputs/window.hpp"

#include "world/items/item.hpp"

#include "rendering/assets_managing/texturing/texture.hpp"

namespace Render::GUI
{
	class Hotbar
	{
	public:

		// = Construction/Destruction

		Hotbar(const Render::Texturing::Texture& texture_slot, const ItemTypeManager& itemTypeManager);


		// = Getters

		Rectangle* getSlotRender_at(std::size_t index) noexcept;

		std::optional<GameWorld::Inventory::Item> getSlot_at(std::size_t index) const noexcept;


		GameWorld::Inventory::Item getSelectedItem() const noexcept { return m_slots[m_cursor_position].second; }


		std::vector<std::pair<Rectangle, GameWorld::Inventory::Item>>& getSlots() noexcept { return m_slots; }
		const std::vector<std::pair<Rectangle, GameWorld::Inventory::Item>>& getSlots() const noexcept { return m_slots; }


		// = Setters

		void setCurrentSlot(GameWorld::Inventory::Item item, const ItemTypeManager& itm) noexcept;


		// = Actors

		// Returns the next slot based on the index which must be either 1 or -1
		GameWorld::Inventory::Item nextSlot(int8 index) noexcept;

		void newPairOfSlots(const Render::Texturing::Texture& texture_slot) noexcept;

		void disable() noexcept;
		void enable() noexcept;

		void draw(const Shader& shader, const Render::Texturing::Texture& texture_slot, const Render::Texturing::Texture& texture_block_gui_atlas, const ItemTypeManager& itm) noexcept;


	private:

		void create_new_slots(const Render::Texturing::Texture& texture_slot) noexcept;

	public:

		static constexpr vec2f g_slot_size{ 80 };

		static constexpr std::size_t g_max_nb_slots{ 10 };

	private:

		std::vector<std::pair<Rectangle, GameWorld::Inventory::Item>> m_slots;
		std::vector<Rectangle> m_items_slots{};


		int64 m_cursor_position{};
		int64 last_cursor{ -1 };

		bool m_cursor_changed{ false };
		bool m_disabled{ false };

	};

} // Render::GUI