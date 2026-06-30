#pragma once // inventory.hpp
// MIT
// Allosker ------------------------------
// =========+
// Small class for inventory management
// ---------------------------------------

#include <vector>

#include "utilities/resourceManaging.hpp"
#include "utilities/resourceManaging.hpp"
#include "rendering/texture.hpp"
#include "rendering/GUI/elements/rectangle.hpp"
#include "rendering/text/text.hpp"

#include "utilities/window.hpp"

#include "world/entities/player/inventory.hpp"

namespace render::gui
{

	class GuiItemStack;

	class GuiInventory
	{
	public:

		// = Construction/Destruction

		GuiInventory(const Inventory& inv);


		// = Actors

		// Mouse Pos in GUI coordinates
		void update(const Inventory& inv, vec2f mouse_pos) noexcept;
		
		void change_board(const Inventory& inv) noexcept;

		// Relative to the Mouse
		void update_index(vec2f mouse_pos) noexcept;

		void toggle() noexcept { m_visible = !m_visible; }


		void draw() noexcept;


	public: // = Global Constants

		static constexpr float c_outline_thickness_px{ 13.f };

		static constexpr float c_absolute_slot_size_px{ 33.f };

		static constexpr float c_scale{ 2.5f };


	private:

		const Texture* const getCurrentBoardTexture() const noexcept;


	private:

		elems::Rectangle m_board{ {}, {}, {} };
		// Same layout as the Inventory's items
		std::vector<GuiItemStack> m_item_slots;

		// Represents the index of the item slot the mouse is on
		std::optional<size_t> m_index{};

		vec2f m_slot_size{};

		vec2f m_size_in_slots{};
		Inventory::BoardSize m_size{};

		bool m_visible{ false };
	};


	struct GuiItemStack 
		: public elems::Rectangle
	{

		GuiItemStack(vec2f size, vec2f pos)
			: Rectangle(size, {}, { pos, {} }), text{ &Resources::get().f_pixelated, "100"}
		{
			text.setScale(0.5f);
			vec2f relative_pos{ pos.x + size.x - text.getSize().x, pos.y };

			
			text.setPosition(vec3f{ relative_pos, 0.f });
			setPosition(pos);
		}

		void change_count(int32 n)
		{
			text.setScale(0.5f);
			vec2f relative_pos{ getPosition().x + getSize().x - text.getSize().x, getPosition().y};
			text.setPosition(vec3f{ relative_pos, 0.f });
			text.setString(std::to_string(n));
		}

		void draw() noexcept
		{
			//Rectangle::draw_transparent(Resources::get().s_gui);
			text.draw(Resources::get().s_gui_text);
		}

		Text text;
	};

}