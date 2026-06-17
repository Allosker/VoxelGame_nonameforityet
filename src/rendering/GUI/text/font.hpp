#pragma once // text.hpp
// MIT
// Allosker ------------------------------
// =========+
// This class manages a text entity that can be transformed
// ---------------------------------------


#include "utilities/opengl.hpp"

#include "rendering/assets_managing/image.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>

namespace Render::GUI
{

	struct Character
	{
		vec2iu	pos{};
		vec2iu	size{};
		vec2i	bearing{};
		uint32	advance{};
	};

	class Font
	{
	public:

		Font(const types::path& path, uint8 btmp_size = 48) noexcept;

		~Font() noexcept;


		// = Getters

		const Character& getCharacter(uint8 c) const noexcept { return m_characters.at(c); }


		// = Setters

		void update_resolution(const types::path& path, uint8 btmp_size) noexcept
		{
			glDeleteTextures(1, &m_texture_id);
			m_characters.clear();
			m_btmp_size = btmp_size;
			load_font(path);
		}
	

		// = actors

		void load_font(const types::path& path) noexcept; 

		void bind() const noexcept { glBindTexture(GL_TEXTURE_2D, m_texture_id); }


	private:

		// = Actors

		Image create_bitmap(const types::path& path) noexcept;
		

	private:

		std::map<uint8, Character> m_characters	{};

		GLuint m_texture_id						{};
		uint8 m_btmp_size						{};


		static constexpr size_t nb_glyphs{ 128 };
		static constexpr size_t size_bitmap{ 512 };
		static constexpr size_t g_padding_pixels{ 2 };

	};

}