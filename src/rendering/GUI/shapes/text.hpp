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

	void setText(const std::string& str) noexcept
	{
		m_text = str;
		update_buffer();
	}

	void setScale(float scale) noexcept { m_scale = scale; }

	void setColor(const vec3f& color) noexcept { m_color = color; }


	// = Actors

	void load_font(const types::path& path) noexcept;

	void draw(const Render::Shader& shader);


private:

	// = Actors

	Image create_bitmap(const types::path& path) noexcept;

	void update_buffer() noexcept;


private:


	std::string m_text{ "azerty" };

	std::map<uint8, Character> characters{};

	vec3f m_color		{ 0.5, 0.8f, 0.2f };
	vec2f m_pos			{};
	float m_scale		{ 1.f };


	size_t m_size_data{};

	GLuint m_vao{};
	GLuint m_vbo{};

	GLuint m_texture_id{};

	static constexpr size_t nb_glyphs{ 128 };
	static constexpr size_t size_bitmap{ 512 };

	static constexpr size_t g_padding_pixels{ 2 };

};


} // namespace Render::GUI