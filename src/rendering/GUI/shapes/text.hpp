#pragma once // text.hpp
// MIT
// Allosker ------------------------------
// =========+
// This class manages a text entity that can be transformed
// ---------------------------------------


#include "uHeaders/opengl.hpp"

#include "rendering/assets_managing/image.hpp"
#include "rendering/shader.hpp"

#include "rendering/world_managing/data/basic/vertex.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>

namespace Render::GUI
{


struct Character
{
	vec2iu	pos		{};
	vec2iu	size	{};
	vec2i	bearing	{};
	uint32	advance	{};
};


class Text
{
public:

	// = Construction/Destruction

	Text(const types::path& path);


	// = Setters


	// = Actors

	void load_font(const types::path& path) noexcept;




	void setText(const std::string& str) noexcept { m_text = str; }

	void draw(const Render::Shader& shader)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		shader.use();
		glBindTexture(GL_TEXTURE_2D, m_texture_id);

		shader.setValue("texColor", m_color);

		glBindVertexArray(m_vao);


		vec2f tPos{ m_pos };

		std::string::const_iterator c{};
		for (c = m_text.begin(); c != m_text.end(); c++)
		{
			Character ch{ characters[*c] };

			vec2f pos{ tPos.x + ch.bearing.x * m_scale, tPos.y - (ch.size.y - ch.bearing.y) * m_scale };
			vec2f size{ ch.size.x * m_scale, ch.size.y * m_scale };


			std::vector<Data::Vertex2D> temp
			{
				Data::Vertex2D
				{ static_cast<vec2f>(vec2iu{pos.x, pos.y + size.y}),static_cast<vec2f>(ch.pos) },
				{ static_cast<vec2f>(pos),						static_cast<vec2f>(vec2iu{ch.pos.x, ch.pos.y + ch.size.y}) },
				{ static_cast<vec2f>(vec2iu {pos.x + size.x, pos.y}),	static_cast<vec2f>(vec2iu {ch.pos.x + ch.size.x,	ch.pos.y + ch.size.y}) },

				{ static_cast<vec2f>(vec2iu {pos.x,				pos.y + size.y}), static_cast<vec2f>(vec2iu{ch.pos.x,				ch.pos.y}) },
				{ static_cast<vec2f>(vec2iu {pos.x + size.x,		pos.y}),	static_cast<vec2f>(vec2iu {ch.pos.x + ch.size.x,	ch.pos.y + ch.size.y}) },
				{ static_cast<vec2f>(vec2iu {pos.x + size.x,		pos.y + size.y}),	static_cast<vec2f>(vec2iu {ch.pos.x + ch.size.x,	ch.pos.y})},
			};



			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			update_buffer(temp);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			tPos.x += (ch.advance >> 6) * m_scale;
		}
	}


private:

	// = Actors

	Image create_bitmap(const types::path& path) noexcept;

	void update_buffer(const std::vector<Data::Vertex2D>& data) noexcept;


private:


	std::string m_text{ "azerty" };

	std::map<uint8, Character> characters{};

	vec3f m_color{ 0.5, 0.8f, 0.2f };
	vec2f m_pos{ 25.f, 25.f };

	vec2iu m_texture_size{};
	float m_scale{ 1.f };

	GLuint m_vao{};
	GLuint m_vbo{};

	GLuint m_texture_id{};

	static constexpr size_t nb_glyphs{ 128 };
	static constexpr size_t size_bitmap{ 512 };

	static constexpr size_t g_padding_pixels{ 2 };

};


} // namespace Render::GUI