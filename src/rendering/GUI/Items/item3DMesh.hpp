#pragma once // itemRenderers.hpp
// MIT
// Allosker ------------------------------
// =========+
// Manages a 3D mesh that is supposed to be an item
// ---------------------------------------


#include "rendering/assets_managing/image.hpp"
#include "rendering/assets_managing/texturing/texture.hpp"
#include "world/entities/basic_entity.hpp"

#include "rendering/GUI/items/itemTypeManager.hpp"
#include "world/entities/items/item.hpp"

namespace Render
{

	class Item3DMesh
		: public GameWorld::Entities::BasicEntity
	{
	public:


		Item3DMesh(const Image& constructor, const types::Rect<types::pixels>& uv_rect_pixels);

		void setId(size_t newId) noexcept { m_id = newId; }

		size_t getId() const noexcept { return m_id; }

		void draw(const Shader& shader, const Texturing::Texture& item_texture) noexcept;


	private:

		std::vector<Data::Vertex> create_data(const Image& item, const types::Rect<types::pixels>& uv_rect_pixels);

		size_t m_id{};


		//static constexpr vec3f g_size_item{ 1, 1, 0.1 };
		static constexpr vec3f g_size_item{ 33, 33, 16 };
		static constexpr vec3f g_size_item_wall{ 1, 1, -1 };

		static constexpr vec3f g_hitbox_item{ 0.02, 0.02, 0.02 };

	};

}