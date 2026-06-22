#include "rectangle.hpp"

// =====================
// Construction/Destruction
// =====================

render::gui::elems::Rectangle::Rectangle(vec2f size, vec2f ori, const types::Rect<types::uvs>& attributes)
	: physics::Transformable2D(size, ori),
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

render::gui::elems::Rectangle::Rectangle(Rectangle&& other) noexcept
	: Mesh(std::move(other)), physics::Transformable2D(other)
{
}

render::gui::elems::Rectangle& render::gui::elems::Rectangle::operator=(Rectangle&& other) noexcept
{
	if (this == &other)
		return *this;

	Mesh::operator=(std::move(other));
	physics::Transformable2D::operator=(other);

	return *this;
}


// =====================
// Setters
// =====================

void render::gui::elems::Rectangle::updateSprite(const types::Rect<types::uvs>& attributes) noexcept
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
		{getBaseSize().x, 0},
		{attributes.pos + types::uvs{ attributes.size.x, 0}}
	},
	{
		{getBaseSize().x, getBaseSize().y},
		{attributes.pos + types::uvs{ attributes.size.x, attributes.size.y}}
	},
	{
		{0, 0},
		attributes.pos
	},
	{
		{0, getBaseSize().y},
		{attributes.pos + types::uvs{ 0, attributes.size.y}}
	},
	{
		{getBaseSize().x, getBaseSize().y},
		{attributes.pos + types::uvs{ attributes.size.x, attributes.size.y}}
	},
	}, sizeof(Data::Vertex2D), GL_STREAM_DRAW);
}


// =====================
// Predicates
// =====================

bool render::gui::elems::Rectangle::contains(vec2f point) const noexcept
{
	auto size{ getSize() };

	return
		(point.x > getPosition().x - getOrigin().x && point.x < (getPosition().x - getOrigin().x + size.x)) &&
		(point.y > getPosition().y - getOrigin().y && point.y < (getPosition().y - getOrigin().y + size.y));
}


// =====================
// Actors
// =====================

void render::gui::elems::Rectangle::draw(const Shader& shader, GLenum mode) noexcept
{
	shader.use();

	shader.setValue("model", getTransformation());

	Mesh::draw(mode);
}

void render::gui::elems::Rectangle::draw_transparent(const Shader& shader, GLenum mode) noexcept
{
	shader.use();

	shader.setValue("model", getTransformation());

	Mesh::draw_transparent(mode);
}