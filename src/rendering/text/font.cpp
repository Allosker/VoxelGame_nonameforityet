#include "font.hpp"


render::Font::Font(const types::path& path, uint8 btmp_size) noexcept
	: m_btmp_size{ btmp_size }
{
	load_font(path);
}

render::Font::~Font() noexcept
{
	glDeleteTextures(1, &m_texture_id);
}

void render::Font::load_font(const types::path& path) noexcept
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
}

/*private*/ render::Image render::Font::create_bitmap(const types::path& path) noexcept
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		throw std::runtime_error("ERROR::FREETYPE: Could not init FreeType Library");

	FT_Face face;
	if (FT_New_Face(ft, path.string().c_str(), 0, &face))
		throw std::runtime_error("ERROR::FREETYPE: Failed to load");

	FT_Set_Pixel_Sizes(face, 0, m_btmp_size);

	render::Image bitmap{ size_bitmap, 1, GL_RED };


	vec2iu pos_btmp{};
	uint32 highest{};
	for (uint8 c{}; c < nb_glyphs; c++)
	{
		// Load one char at a time
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			continue;

		render::Image glyph{
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
		m_characters.insert(std::pair<uint8, Character>{c, character});

		pos_btmp.x += (uint32)face->glyph->bitmap.width + g_padding_pixels;
	}

	// Release Resources
	FT_Done_Face(face);
	FT_Done_FreeType(ft);


	return bitmap;
}
