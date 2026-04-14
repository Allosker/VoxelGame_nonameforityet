#include "hotbar.hpp"

// =====================
// Construction/Destruction
// =====================


Render::GUI::Hotbar::Hotbar(const Render::Texturing::Texture& textureHotbar, const ItemTypeManager& itm)
	: m_slots
	{
		std::pair<Rectangle, GameWorld::Inventory::Item>
		{Render::GUI::Rectangle { g_size_GUI_item, {g_size_GUI_item.x / 2, g_size_GUI_item.y / 2}, Render::GUI::to_UvAtlas(1, itm) }, {}},
		{Render::GUI::Rectangle { g_size_GUI_item, {g_size_GUI_item.x / 2, g_size_GUI_item.y / 2}, Render::GUI::to_UvAtlas(1, itm) }, {}},
		{Render::GUI::Rectangle { g_size_GUI_item, {g_size_GUI_item.x / 2, g_size_GUI_item.y / 2}, Render::GUI::to_UvAtlas(1, itm) }, {}},
		{Render::GUI::Rectangle { g_size_GUI_item, {g_size_GUI_item.x / 2, g_size_GUI_item.y / 2}, Render::GUI::to_UvAtlas(1, itm) }, {}},
		{Render::GUI::Rectangle { g_size_GUI_item, {g_size_GUI_item.x / 2, g_size_GUI_item.y / 2}, Render::GUI::to_UvAtlas(1, itm) }, {}},
		{Render::GUI::Rectangle { g_size_GUI_item, {g_size_GUI_item.x / 2, g_size_GUI_item.y / 2}, Render::GUI::to_UvAtlas(1, itm) }, {}},
		{Render::GUI::Rectangle { g_size_GUI_item, {g_size_GUI_item.x / 2, g_size_GUI_item.y / 2}, Render::GUI::to_UvAtlas(1, itm) }, {}},
		{Render::GUI::Rectangle { g_size_GUI_item, {g_size_GUI_item.x / 2, g_size_GUI_item.y / 2}, Render::GUI::to_UvAtlas(1, itm) }, {}},
		{Render::GUI::Rectangle { g_size_GUI_item, {g_size_GUI_item.x / 2, g_size_GUI_item.y / 2}, Render::GUI::to_UvAtlas(1, itm) }, {}},
		{Render::GUI::Rectangle { g_size_GUI_item, {g_size_GUI_item.x / 2, g_size_GUI_item.y / 2}, Render::GUI::to_UvAtlas(1, itm) }, {}}
	}, 
	m_hotbar{ textureHotbar.getSize(), {0, 0}, Render::UvPixels{textureHotbar.getSize(), textureHotbar.getSize() } }
{
	m_hotbar.setPosition({ -m_hotbar.getSize().x / 2, -Wai::Window::framebuffer_GUI_size.y / 2 });

	vec2f right{  Render::GUI::Hotbar::slot_size.x / 2, Render::GUI::Hotbar::slot_size.y / 2 };
	vec2f left { -Render::GUI::Hotbar::slot_size.x / 2, Render::GUI::Hotbar::slot_size.y / 2 };

	m_slots[0].first.setPosition( vec2f{93 - Wai::Window::framebuffer_GUI_size.x / 2, 254 - Wai::Window::framebuffer_GUI_size.y / 2} - left );
	m_slots[1].first.setPosition( vec2f{ 1'827 - Wai::Window::framebuffer_GUI_size.x / 2, 254 - Wai::Window::framebuffer_GUI_size.y / 2 } - right );

	// 337, 154
	// 157

	for (std::size_t i{2}; i < m_slots.size(); i++)
		m_slots[i].first.setPosition(vec2f{ 324 - Wai::Window::framebuffer_GUI_size.x / 2, 154 - Wai::Window::framebuffer_GUI_size.y / 2 } - right + vec2f{ 157.f * (i - 1), 0});

}

void Render::GUI::Hotbar::draw(const Shader& shader, const Render::Texturing::Texture& textureHotbar, const Render::Texturing::Texture& atlasItems) noexcept
{
	textureHotbar.bind();

	m_hotbar.draw_transparent(shader);

	atlasItems.bind();

	for (std::size_t i{}; i < m_slots.size(); i++)
		//if (m_slots[i].second.id != 0)
			m_slots[i].first.draw_transparent(shader);
}
