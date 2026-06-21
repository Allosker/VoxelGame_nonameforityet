#pragma once // itemRenderers.hpp
// MIT
// Allosker ------------------------------
// =========+
// Manages a 3D mesh that is supposed to be an item
// ---------------------------------------


#include "data/image.hpp"
#include "rendering/texture.hpp"
#include "world/entities/basicEntity.hpp"

#include "world/types/itemTypeManager.hpp"
#include "data/item.hpp"

namespace render
{

	class ItemEntity
		: public entities::BasicEntity
	{
	public:

		// = Construction 
		ItemEntity(const Image& constructor, const types::Rect<types::pixels>& uv_rect_pixels, types::type_id newID);

		ItemEntity(ItemEntity&&) noexcept = default;
		ItemEntity& operator=(ItemEntity&&) noexcept = default;

		void setId(types::type_id newId) noexcept { m_id = newId; }

		types::type_id getId() const noexcept { return m_id; }

		void draw(const Shader& shader, const Texture& item_texture) noexcept;


	private:

		std::vector<Data::Vertex> create_data(const Image& item, const types::Rect<types::pixels>& uv_rect_pixels);

		types::type_id m_id{};


		//static constexpr vec3f g_size_item{ 1, 1, 0.1 };
		static constexpr vec3f g_size_item{ 33, 33, 16 };
		static constexpr vec3f g_size_item_wall{ 1, 1, -1 };

		static constexpr vec3f g_hitbox_item{ 0.02, 0.02, 0.02 };

	};

}