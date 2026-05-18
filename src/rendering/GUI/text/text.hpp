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

#include "transforms/transform3D.hpp"

#include "font.hpp"

namespace Render::GUI
{


class Text
	: public Transform3D
{
public:

	// = Construction/Destruction

	Text(const Font& font, std::string_view str = {}, const vec3f& pos = {}, const vec3f& ori = {});

	Text(Text&& other) noexcept;
	Text& operator=(Text&& other) noexcept;

	virtual ~Text() noexcept;


	// = Setters

	void setStr(const std::string& str)
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

	void draw(const Render::Shader& shader);

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


} // namespace Render::GUI