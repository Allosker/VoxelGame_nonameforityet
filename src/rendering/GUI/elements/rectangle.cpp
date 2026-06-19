#include "rectangle.hpp"

// =====================
// Construction/Destruction
// =====================

Render::GUI::Elems::Rectangle::Rectangle(vec2f size, vec2f ori, const types::Rect<types::uvs>& attributes)
	: Transform2D(size, ori),
	Mesh(std::array<Data::Vertex2D, 6>
{
	Data::Vertex2D
	{
		{0, 0},
		attributes.pos
	},
	{
		{size.x, 0},
		{attributes.pos + types::uvs{ attributes.size.x, 0}}
	},
	{
		{size.x, size.y},
		{attributes.pos + types::uvs{ attributes.size.x, attributes.size.y}}
	},
	{
		{0, 0},
		attributes.pos
	},
	{
		{0, size.y},
		{attributes.pos + types::uvs{ 0, attributes.size.y}}
	},
	{
		{size.x, size.y},
		{attributes.pos + types::uvs{ attributes.size.x, attributes.size.y}}
	},
})
{
}

Render::GUI::Elems::Rectangle::Rectangle(Rectangle&& other) noexcept
	: Mesh(std::move(other)), Transform2D(other)
{
}

Render::GUI::Elems::Rectangle& Render::GUI::Elems::Rectangle::operator=(Rectangle&& other) noexcept
{
	if (this == &other)
		return *this;

	Mesh::operator=(std::move(other));
	Transform2D::operator=(other);

	return *this;
}


// =====================
// Setters
// =====================

void Render::GUI::Elems::Rectangle::updateSprite(const types::Rect<types::uvs>& attributes) noexcept
{
	Mesh::updateBuffer<Data::Vertex2D>
		(
			std::array<Data::Vertex2D, 6>
	{
	Data::Vertex2D
	{
		{0, 0},
		attributes.pos
	},
	{
		{m_baseSize.x, 0},
		{attributes.pos + types::uvs{ attributes.size.x, 0}}
	},
	{
		{m_baseSize.x, m_baseSize.y},
		{attributes.pos + types::uvs{ attributes.size.x, attributes.size.y}}
	},
	{
		{0, 0},
		attributes.pos
	},
	{
		{0, m_baseSize.y},
		{attributes.pos + types::uvs{ 0, attributes.size.y}}
	},
	{
		{m_baseSize.x, m_baseSize.y},
		{attributes.pos + types::uvs{ attributes.size.x, attributes.size.y}}
	},
	}, sizeof(Data::Vertex2D), GL_STREAM_DRAW);
}


// =====================
// Predicates
// =====================

bool Render::GUI::Elems::Rectangle::contains(vec2f point) const noexcept
{
	auto size{ getSize() };

	return
		(point.x > m_position.x - m_origin.x && point.x < (m_position.x - m_origin.x + size.x)) &&
		(point.y > m_position.y - m_origin.y && point.y < (m_position.y - m_origin.y + size.y));
}


// =====================
// Actors
// =====================

void Render::GUI::Elems::Rectangle::draw(const Shader& shader, GLenum mode) noexcept
{
	shader.use();

	shader.setValue("model", getTransformation());

	Mesh::draw(mode);
}

void Render::GUI::Elems::Rectangle::draw_transparent(const Shader& shader, GLenum mode) noexcept
{
	shader.use();

	shader.setValue("model", getTransformation());

	Mesh::draw_transparent(mode);
}