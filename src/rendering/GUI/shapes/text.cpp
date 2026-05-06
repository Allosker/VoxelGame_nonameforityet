#include "text.hpp"

Render::GUI::Text::Text(const types::path& path)
{
	load_font(path);
}

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

	glVertexAttribPointer(0, 4, GL_FLOAT, false, 4 * sizeof(float), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

Render::Image Render::GUI::Text::create_bitmap(const types::path& path) noexcept
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
			vec2iu{face->glyph->bitmap.width, face->glyph->bitmap.rows},
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

void Render::GUI::Text::update_buffer(const std::vector<Data::Vertex2D>& data) noexcept
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Data::Vertex2D), data.data(), GL_STREAM_DRAW);
	glEnableVertexAttribArray(0);
}
