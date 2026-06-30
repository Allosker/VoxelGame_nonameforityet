#pragma once // text.hpp
// MIT
// Allosker ------------------------------
// =========+
// This class manages a text entity that can be transformed
// ---------------------------------------


#include "utilities/opengl.hpp"

#include "data/image.hpp"
#include "rendering/shader.hpp"

#include "data/vertex.hpp"

#include "physics/transforms/transformable3D.hpp"

#include "font.hpp"

namespace render
{


class Text
	: public physics::Transformable3D
{
public:

	// = Construction/Destruction

	Text(const Font* font, std::string_view str = {}, const vec3f& pos = {}, const vec3f& ori = {});

	Text(Text&&) noexcept;
	Text& operator=(Text&&) noexcept;

	virtual ~Text() noexcept;


	// = Setters

	void setString(const std::string& str)
	{
		m_text = str;
		update_buffer();
	}

	void setColor(const vec3f& color) noexcept { m_color = color; }
	void setScaleText(float scale) noexcept { m_scale_text = scale; }

	void setFont(const Font& font)
	{
		p_font = &font;
		update_buffer();
	}


	// = Actors

	void draw(const render::Shader& shader);

	void reload()
	{
		update_buffer();
	}


private:

	// = Actors

	void update_buffer();


private:


	std::string m_text{};

	const Font* p_font{ nullptr };

	vec3f m_color		{ 0.5, 0.8f, 0.2f };
	float m_scale_text	{ 1.f };


	size_t m_size_data{};

	GLuint m_vao{};
	GLuint m_vbo{};

	

};


} // namespace render::gui