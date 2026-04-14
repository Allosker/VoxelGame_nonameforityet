#pragma once // hotbar.hpp
// MIT
// Allosker ------------------------------
// =========+
// Manages the horbar within the HUD and each item associated with it
// ---------------------------------------

#include <optional>

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

		Hotbar(const Render::Texturing::Texture& textureHotbar, const ItemTypeManager& itemTypeManager);


		Rectangle* getSlotRender_at(std::size_t index)
		{
			if (index < 0 || index >= m_slots.size())
				return nullptr;

			return &m_slots[index].first;
		}

		std::optional<GameWorld::Inventory::Item> getSlot_at(std::size_t index)
		{
			if (index < 0 || index >= m_slots.size())
				return std::nullopt;

			return std::make_optional(m_slots[index].second);
		}


		void draw(const Shader& shader, const Render::Texturing::Texture& textureHotbar, const Render::Texturing::Texture& atlasItems) noexcept;

	public:


		static constexpr vec2f slot_size{ 145, 144 };

	private:

		std::array<std::pair<Rectangle, GameWorld::Inventory::Item>, 10> m_slots;

		Render::GUI::Rectangle m_hotbar;

		static constexpr vec2f g_size_GUI_item{ 99.f };

	};
}