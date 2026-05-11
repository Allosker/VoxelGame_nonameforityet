#include "text.hpp"



// =====================
// Construction/Destruction
// =====================

Render::GUI::Text::Text(const types::path& path)
	: Transform3D({}, {})
{
	load_font(path);
	update_buffer();
}


// =====================
// Actors
// =====================

void Render::GUI::Text::load_font(const types::path& path) noexcept
{
	// Disable Defulat Texture Alignment
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	Image temp{ std::move(create_bitmap(path)) };

	glGenTextures(1, &m_texture_id);
	glBindTexture(GL_TEXTURE_2D, m_texture_id);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RED,
		temp.getSize().x,
		temp.getSize().y,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		temp.data()
	);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	// create GPU data

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(Data::Vertex2D), std::bit_cast<void*>(offsetof(Data::Vertex2D, pos)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Data::Vertex2D), std::bit_cast<void*>(offsetof(Data::Vertex2D, uv)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

void Render::GUI::Text::draw(const Render::Shader& shader)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shader.use();
	glBindTexture(GL_TEXTURE_2D, m_texture_id);

	shader.setValue("textColor", m_color);
	shader.setValue("model", getTransformation());

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, m_size_data);
}


/*private*/ Render::Image Render::GUI::Text::create_bitmap(const types::path& path) noexcept
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		throw std::runtime_error("ERROR::FREETYPE: Could not init FreeType Library");

	FT_Face face;
	if (FT_New_Face(ft, FONT_PATH"pixelated.ttf", 0, &face))
		throw std::runtime_error("ERROR::FREETYPE: Failed to load");

	FT_Set_Pixel_Sizes(face, 0, 48);

	Render::Image bitmap{ size_bitmap, 1, GL_RED };


	vec2iu pos_btmp{};
	uint32 highest{};
	for (uint8 c{}; c < nb_glyphs; c++)
	{
		// Load one char at a time
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			continue;

		Render::Image glyph{
			{ face->glyph->bitmap.width, face->glyph->bitmap.rows },
			face->glyph->bitmap.buffer,
			1,
			GL_RED
		};

		highest = std::max(highest, (uint32)face->glyph->bitmap.rows);

		if (pos_btmp.x >= size_bitmap - face->glyph->bitmap.width)
		{
			pos_btmp.x = 0;
			pos_btmp.y += highest;
		}

		bitmap.insert(vec2iu{ pos_btmp.x, pos_btmp.y }, glyph);


		Character character{
			vec2iu{pos_btmp.x , pos_btmp.y },
			glyph.getSize(),
			vec2i{face->glyph->bitmap_left, face->glyph->bitmap_top},
			face->glyph->advance.x
		};
		characters.insert(std::pair<uint8, Character>{c, character});

		pos_btmp.x += (uint32)face->glyph->bitmap.width + g_padding_pixels;
	}

	// Release Resources
	FT_Done_Face(face);
	FT_Done_FreeType(ft);


	return bitmap;
}

/*private*/ void Render::GUI::Text::update_buffer() noexcept
{
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	std::vector<Data::Vertex2D> data{};

	vec2f tPos{};

	std::string::const_iterator c{};
	for (c = m_text.begin(); c != m_text.end(); c++)
	{
		Character ch{ characters[*c] };

		vec2f pos{ tPos.x + ch.bearing.x * m_scale_text, tPos.y - (ch.size.y - ch.bearing.y) * m_scale_text };
		vec2f size{ ch.size.x * m_scale_text, ch.size.y * m_scale_text };

		data.insert(data.end(),
			{
				Data::Vertex2D
				{ vec2f{pos.x, pos.y + size.y},						static_cast<vec2f>(ch.pos)													},
				{ pos,												static_cast<vec2f>(vec2iu {ch.pos.x, ch.pos.y + ch.size.y})					},
				{ vec2f{pos.x + size.x, pos.y},						static_cast<vec2f>(vec2iu {ch.pos.x + ch.size.x,	ch.pos.y + ch.size.y})	},

				{ vec2f{pos.x,				pos.y + size.y},		static_cast<vec2f>(vec2iu {ch.pos.x,				ch.pos.y})				},
				{ vec2f{pos.x + size.x,		pos.y},					static_cast<vec2f>(vec2iu {ch.pos.x + ch.size.x,	ch.pos.y + ch.size.y})	},
				{ vec2f{pos.x + size.x,		pos.y + size.y},		static_cast<vec2f>(vec2iu {ch.pos.x + ch.size.x,	ch.pos.y})				},
			});

		tPos.x += (ch.advance >> 6) * m_scale_text;
	}

	m_size_data = data.size();

	glBufferData(GL_ARRAY_BUFFER, sizeof(Data::Vertex2D) * data.size(), data.data(), GL_STREAM_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
