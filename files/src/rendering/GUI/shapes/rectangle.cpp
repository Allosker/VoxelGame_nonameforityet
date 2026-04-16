#include "rectangle.hpp"

// =====================
// Construction/Destruction
// =====================

Render::GUI::Rectangle::Rectangle(vec2f size, vec2f ori, const UvPixels& attributes)
	: m_origin{ ori }, m_baseSize{size}, m_scale { 1.f, 1.f },
	Mesh(std::array<Data::Vertex2D, 6>
{
	Render::Data::Vertex2D
	{
		{0, 0},
		attributes.pos_in_texture
	},
	{
		{size.x, 0},
		{attributes.pos_in_texture + types::uvs{ attributes.subset_size.x, 0}}
	},
	{
		{size.x, size.y},
		{attributes.pos_in_texture + types::uvs{ attributes.subset_size.x, attributes.subset_size.y}}
	},
	{
		{0, 0},
		attributes.pos_in_texture
	},
	{
		{0, size.y},
		{attributes.pos_in_texture + types::uvs{ 0, attributes.subset_size.y}}
	},
	{
		{size.x, size.y},
		{attributes.pos_in_texture + types::uvs{ attributes.subset_size.x, attributes.subset_size.y}}
	},
})
{
	if (m_origin.x != 0 || m_origin.y != 0)
		m_transformNeedUpdate = true;
}

Render::GUI::Rectangle::Rectangle(Rectangle&& other) noexcept
	: Mesh(std::move(other))
	, m_transformations		{ other.m_transformations }
	, m_scale{ other.m_scale }
	, m_baseSize{ other.m_baseSize }
	, m_origin{ other.m_origin }
	, m_position{ other.m_position }
	, m_rotation{ other.m_rotation }
	, m_transformNeedUpdate{ other.m_transformNeedUpdate }
{
}

Render::GUI::Rectangle& Render::GUI::Rectangle::operator=(Rectangle&& other) noexcept
{
	if (this == &other)
		return *this;

	Mesh::operator=(std::move(other));

	m_transformations		= other.m_transformations;
	m_scale					= other.m_scale;
	m_baseSize				= other.m_baseSize;
	m_origin				= other.m_origin;
	m_position				= other.m_position;
	m_rotation				= other.m_rotation;
	m_transformNeedUpdate	= other.m_transformNeedUpdate;

	return *this;
}


// =====================
// Getters
// =====================

const mpml::Matrix4<float>& Render::GUI::Rectangle::getTransformation() noexcept
{
	// Recompute the combined transform if needed
	if (m_transformNeedUpdate)
	{
		const float angle = -m_rotation.asRadians();
		const float cosine = std::cos(angle);
		const float sine = std::sin(angle);
		const float sxc = m_scale.x * cosine;
		const float syc = m_scale.y * cosine;
		const float sxs = m_scale.x * sine;
		const float sys = m_scale.y * sine;
		const float tx = -m_origin.x * sxc - m_origin.y * sys + m_position.x;
		const float ty = m_origin.x * sxs - m_origin.y * syc + m_position.y;

		m_transformations = mpml::Matrix4<float>
		{
			sxc, -sys, 0.f, tx,
			sxs, syc, 0.f, ty,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f
		};

		m_transformNeedUpdate = false;
	}

	return m_transformations;
}


// =====================
// Setters
// =====================

void Render::GUI::Rectangle::setPosition(vec2f pos) noexcept
{
	m_position = pos;
	m_transformNeedUpdate = true;
}

void Render::GUI::Rectangle::setScale(vec2f scale) noexcept
{
	m_scale = scale;
	m_transformNeedUpdate = true;
}

void Render::GUI::Rectangle::setSize(vec2f size) noexcept
{
	setScale({ size.x / m_baseSize.x, size.y / m_baseSize.y });
}

void Render::GUI::Rectangle::setRotation(mpml::Angle<> rotation) noexcept
{
	m_rotation = rotation;
	m_transformNeedUpdate = true;
}


void Render::GUI::Rectangle::move(vec2f offset) noexcept
{
	setPosition(m_position + offset);
}

void Render::GUI::Rectangle::scale(vec2f factor) noexcept
{
	setScale({ m_scale.x * factor.x,  m_scale.y * factor.y });
}

void Render::GUI::Rectangle::rotate(mpml::Angle<> theta) noexcept
{
	setRotation(mpml::Angle<>::fromRadians(m_rotation.asRadians() + theta.asRadians()));
}


void Render::GUI::Rectangle::updateSprite(const UvPixels& attributes) noexcept
{
	Mesh::updateBuffer<Render::Data::Vertex2D>
		(
			std::array<Data::Vertex2D, 6>
	{
		Render::Data::Vertex2D
		{
			{0, 0},
			attributes.pos_in_texture
		},
	{
		{m_baseSize.x, 0},
		{attributes.pos_in_texture + types::uvs{ attributes.subset_size.x, 0}}
	},
	{
		{m_baseSize.x, m_baseSize.y},
		{attributes.pos_in_texture + types::uvs{ attributes.subset_size.x, attributes.subset_size.y}}
	},
	{
		{0, 0},
		attributes.pos_in_texture
	},
	{
		{0, m_baseSize.y},
		{attributes.pos_in_texture + types::uvs{ 0, attributes.subset_size.y}}
	},
	{
		{m_baseSize.x, m_baseSize.y},
		{attributes.pos_in_texture + types::uvs{ attributes.subset_size.x, attributes.subset_size.y}}
	},
	}, sizeof(Render::Data::Vertex2D), GL_STREAM_DRAW);
}


// =====================
// Predicates
// =====================

bool Render::GUI::Rectangle::contains(vec2f point) const noexcept
{
	auto size{ getSize() };
	return
		(point.x > m_position.x && point.x < (m_position.x + size.x)) &&
		(point.y > m_position.y && point.y < (m_position.y + size.y));
}


// =====================
// Actors
// =====================

void Render::GUI::Rectangle::draw(const Shader& shader, GLenum mode) noexcept
{
	shader.use();

	shader.setValue("model", getTransformation().transpose());

	Mesh::draw(mode);
}

void Render::GUI::Rectangle::draw_transparent(const Shader& shader, GLenum mode) noexcept
{
	shader.use();

	shader.setValue("model", getTransformation().transpose());

	Mesh::draw_transparent(mode);
}