#pragma once // itemRenderers.hpp
// MIT
// Allosker ------------------------------
// =========+
// Two classes to manage 3D world items and 2D GUI items
// ---------------------------------------


#include "rendering/assets_managing/image.hpp"
#include "rendering/assets_managing/texturing/texture.hpp"
#include "rendering/mesh/mesh.hpp"

#include "rendering/GUI/items/itemTypeManager.hpp"
#include "world/items/item.hpp"

namespace Render
{

	class Item3DMesh
	{
	public:


		Item3DMesh(const Image& constructor, const types::Rect<types::pixels>& uv_rect_pixels);



		void draw(const Shader& shader, const Texturing::Texture& item_texture) noexcept;


	private:

		std::vector<Data::Vertex> create_data(const Image& item, const types::Rect<types::pixels>& uv_rect_pixels);


		Mesh m_mesh{};

		//static constexpr vec3f g_size_item{ 1, 1, 0.1 };
		static constexpr vec3f g_size_item{ 33, 33, 16 };
		static constexpr vec3f g_size_item_wall{ 1, 1, -1 };

	};

}