#include "itemRenderer.hpp"

Render::Item3DMesh::Item3DMesh(const Image& constructor, const types::Rect<types::pixels>& uv_rect_pixels)
	: GameWorld::Entities::BasicEntity{ types::Rect<vec3f>{{}, g_size_item}, types::Rect<vec3f>{{}, g_hitbox_item} }
{
	createBuffers(create_data(constructor, uv_rect_pixels), GL_STREAM_DRAW, { 3, 2 });
}

void Render::Item3DMesh::draw(const Shader& shader, const Texturing::Texture& item_texture) noexcept
{
	shader.use();
	item_texture.bind();

	GameWorld::Entities::BasicEntity::draw(shader);
}

/*private*/ std::vector<Render::Data::Vertex> Render::Item3DMesh::create_data(const Image& image, const types::Rect<types::pixels>& uv_rect_pixels)
{
	if (image.getSize().x != image.getSize().y)
		throw std::runtime_error("ERROR::Size of Item Image must be square");
	// Can change that with a lot of pain

	types::Rect<types::uvs> uvs{ uv_rect_pixels };

	std::vector<Render::Data::Vertex> item_data
	{
		// First top
		Render::Data::Vertex
		{
			{0, 0, 0},
			uvs.pos
		},
		{
			vec3f{g_size_item.x, 0, 0} / static_cast<float>(image.getSize().y),
			{uvs.pos + types::uvs{ uvs.size.x, 0}}
		},
		{
			vec3f{g_size_item.x, g_size_item.y, 0} / static_cast<float>(image.getSize().y),
			{uvs.pos + types::uvs{ uvs.size.x, uvs.size.y}}
		},
		{
			{0, 0, 0},
			uvs.pos
		},
		{
			vec3f{g_size_item.x, g_size_item.y, 0} / static_cast<float>(image.getSize().y),
			{uvs.pos + types::uvs{ uvs.size.x, uvs.size.y}}
		},
		{
			vec3f{0, g_size_item.y, 0} / static_cast<float>(image.getSize().y),
			{uvs.pos + types::uvs{ 0, uvs.size.y}}
		},

		// Second bottom
		Render::Data::Vertex
		{
			vec3f{0, 0, g_size_item_wall.z} / static_cast<float>(image.getSize().y),
			uvs.pos
		},
		{
			vec3f{g_size_item.x, g_size_item.y, g_size_item_wall.z} / static_cast<float>(image.getSize().y),
			{uvs.pos + types::uvs{ uvs.size.x, uvs.size.y}}
		},
		{
			vec3f{g_size_item.x, 0, g_size_item_wall.z} / static_cast<float>(image.getSize().y),
			{uvs.pos + types::uvs{ uvs.size.x, 0}}
		},
		{
			vec3f{0, 0, g_size_item_wall.z} / static_cast<float>(image.getSize().y),
			uvs.pos
		},
		{
			vec3f{0, g_size_item.y, g_size_item_wall.z} / static_cast<float>(image.getSize().y),
			{uvs.pos + types::uvs{ 0, uvs.size.y}}
		},
		{
			vec3f{g_size_item.x, g_size_item.y, g_size_item_wall.z} / static_cast<float>(image.getSize().y),
			{uvs.pos + types::uvs{ uvs.size.x, uvs.size.y}}
		},
	};

	const auto& data{ image.getData() };

	
	
	const vec2f subset_size{ uv_rect_pixels.pos + uv_rect_pixels.size };
	for (float y{ uv_rect_pixels.pos.y }; y < subset_size.y; y++)
	{
		for (float x{ uv_rect_pixels.pos.x }; x < subset_size.x; x++)
		{
			uint8 current{ data[(x + y * image.getSize().x) * 4 + 3] };


			float p = x + 1;
			float p2 = y + 1;

			vec2f pos{ x - uv_rect_pixels.pos.x, y - uv_rect_pixels.pos.y };


			if (current != 0)
			{
				int8 up{}, down{}, right{}, left{};

				if (x + 1 < image.getSize().x)
					right = data[((x + 1) + y * image.getSize().x) * 4 + 3];

				if (x - 1 > 0)
					left = data[((x - 1) + y * image.getSize().x) * 4 + 3];

				if (y + 1 < image.getSize().y)
					up = data[(x + (y + 1) * image.getSize().x) * 4 + 3];

				if (y - 1 > 0)
					down = data[(x + (y - 1) * image.getSize().x) * 4 + 3];


				if (left == 0 && right != 0)
					item_data.insert(item_data.end(), 
					{
						{
							vec3f{pos.x, pos.y, 0} / static_cast<float>(image.getSize().y),
							vec2f{x, y}
						},
						{
							(vec3f{0, g_size_item_wall.y, g_size_item_wall.z} + vec3f{pos.x, pos.y, 0}) / static_cast<float>(image.getSize().y),
							vec2f{p, p2}
						},
						{
							(vec3f{0, 0, g_size_item_wall.z} + vec3f{pos.x, pos.y, 0}) / static_cast<float>(image.getSize().y),
							vec2f{p,y}
						},
						
						{
							vec3f{pos.x, pos.y, 0} / static_cast<float>(image.getSize().y),
							vec2f{x, y}
						},
						{
							(vec3f{0, g_size_item_wall.y, 0} + vec3f{pos.x, pos.y, 0}) / static_cast<float>(image.getSize().y),
							vec2f{x, p2}
						},
						{
							(vec3f{0, g_size_item_wall.y, g_size_item_wall.z} + vec3f{pos.x, pos.y, 0}) / static_cast<float>(image.getSize().y),
							vec2f{p, p2} 
						},
					});

				if (right == 0 && left != 0)
					item_data.insert(item_data.end(),
					{
						{
							vec3f{pos.x + 1, pos.y, 0} / static_cast<float>(image.getSize().y),
							vec2f{x, y} 
						},
						{
							(vec3f{0, 0, g_size_item_wall.z} + vec3f{pos.x + 1, pos.y, 0}) / static_cast<float>(image.getSize().y),
							vec2f{p, y} 
						},
						{
							(vec3f{0, g_size_item_wall.y, g_size_item_wall.z} + vec3f{pos.x + 1, pos.y, 0}) / static_cast<float>(image.getSize().y),
							vec2f{p, p2}
						},
						{
							vec3f{pos.x + 1, pos.y, 0} / static_cast<float>(image.getSize().y),
							vec2f{x, y} 
						},
						{
							(vec3f{0, g_size_item_wall.y, g_size_item_wall.z} + vec3f{pos.x + 1, pos.y, 0}) / static_cast<float>(image.getSize().y),
							vec2f{p, p2} 
						},
						{
							(vec3f{0, g_size_item_wall.y, 0} + vec3f{pos.x + 1, pos.y, 0}) / static_cast<float>(image.getSize().y),
							vec2f{x, p2}
						},
					});

				if (down == 0 && up != 0)
					item_data.insert(item_data.end(),
						{
							{
								(vec3f{0, g_size_item_wall.y, 0} + vec3f{pos.x, pos.y - 1, 0}) / static_cast<float>(image.getSize().y),
								vec2f{x, y}
							},
							{
								(vec3f{g_size_item_wall.x, g_size_item_wall.y, g_size_item_wall.z} + vec3f{pos.x, pos.y - 1, 0}) / static_cast<float>(image.getSize().y),
								vec2f{p, y}
							},
							{
								(vec3f{g_size_item_wall.x, g_size_item_wall.y, 0} + vec3f{pos.x, pos.y - 1, 0}) / static_cast<float>(image.getSize().y),
								vec2f{p, p2} 
							}, 
							
							{
								(vec3f{0, g_size_item_wall.y, 0} + vec3f{pos.x, pos.y - 1, 0}) / static_cast<float>(image.getSize().y),
								vec2f{x, y} 
							},
							{
								(vec3f{0, g_size_item_wall.y, g_size_item_wall.z} + vec3f{pos.x, pos.y - 1, 0}) / static_cast<float>(image.getSize().y),
								vec2f{x, p2}
							},
							{
								(vec3f{g_size_item_wall.x, g_size_item_wall.y, g_size_item_wall.z} + vec3f{pos.x, pos.y - 1, 0}) / static_cast<float>(image.getSize().y),
								vec2f{p, p2} 
							},
							
						});

				if (up == 0 && down != 0)
					item_data.insert(item_data.end(),
						{
							{
								(vec3f{0, g_size_item_wall.y, 0} + vec3f{pos.x, pos.y, 0}) / static_cast<float>(image.getSize().y),
								vec2f{x, y}
							},
							{
								(vec3f{g_size_item_wall.x, g_size_item_wall.y, 0} + vec3f{pos.x, pos.y, 0}) / static_cast<float>(image.getSize().y),
								vec2f{p, p2}
							},
							{
								(vec3f{g_size_item_wall.x, g_size_item_wall.y, g_size_item_wall.z} + vec3f{pos.x, pos.y, 0}) / static_cast<float>(image.getSize().y),
								vec2f{p, y} 
							},
							
							{
								(vec3f{0, g_size_item_wall.y, 0} + vec3f{pos.x, pos.y, 0}) / static_cast<float>(image.getSize().y),
								vec2f{x, y}
							},
							{
								(vec3f{g_size_item_wall.x, g_size_item_wall.y, g_size_item_wall.z} + vec3f{pos.x, pos.y, 0}) / static_cast<float>(image.getSize().y),
								vec2f{p, p2}
							},
							{
								(vec3f{0, g_size_item_wall.y, g_size_item_wall.z} + vec3f{pos.x, pos.y, 0}) / static_cast<float>(image.getSize().y),
								vec2f{x, p2}
							},
							
						});
			}


		}
	}
	


	return item_data;
}
