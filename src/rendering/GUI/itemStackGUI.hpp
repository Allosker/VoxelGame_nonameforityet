#pragma once // item_utils_2D.hpp
// MIT
// Allosker ------------------------------
// =========+
// Small class managing an item
// ---------------------------------------

#include "rendering/gui/elements/rectangle.hpp"
#include "data/item.hpp"
#include "rendering/text/text.hpp"

namespace render::gui
{

	struct ItemStackGUI
		: public elems::Rectangle 
	{

		ItemStackGUI(vec2f size, vec2f ori, const types::Rect<types::uvs>& attributes, Data::Item item, const Font& font)
			: elems::Rectangle{ size, ori, attributes },
			stack_item{ item }, text{ font }
		{
			text.setScale(g_scale_text_rest);
			text.setStr("10");
		}



		ItemStackGUI(ItemStackGUI&&) noexcept = default;
		ItemStackGUI& operator=(ItemStackGUI&&) noexcept = default;

		ItemStackGUI(const ItemStackGUI&) noexcept = delete;
		ItemStackGUI& operator=(const ItemStackGUI&) noexcept = delete;

		void remove() noexcept
		{
			if (count != 0)
				count--;
			update_text();
		}

		void add() noexcept
		{
			if (count < std::numeric_limits<int16>::max() || count < max_count)
				count++;
			update_text();
		}

		void setPosition(vec2f pos) noexcept override
		{
			physics::Transformable2D::setPosition(pos);
			text.setPosition(vec3f{ getPosition() - getSize() / 2.f, 0});
		}

		void setScale(vec2f scale) noexcept override
		{
			physics::Transformable2D::setScale(scale);
			text.setPosition(vec3f{ getPosition() - getSize() / 2.f, 0 });
		}

		void draw(const Shader& shader, const Shader& text_shader) noexcept
		{
			elems::Rectangle::draw_transparent(shader);

			if (count == 1)
				return;

			text.draw(text_shader);
		}

		void disable() noexcept
		{
			stack_item.id = 0;
			count = 0;
		}

		void update_text() noexcept
		{
			text.setStr(std::to_string(count));
			text.setPosition(vec3f{ getPosition() - getSize() / 2.f, 0 });
		}

		Text text;
		// Common id for all items
		Data::Item stack_item{};
		int16 count{};

		bool shouldBeDrawn{ false };

		int16 max_count{ 10 };


		static constexpr float g_scale_text_rest{ 0.5 };
		static constexpr float g_scale_text_hover{ 0.6 };
	};


} //  render::gui