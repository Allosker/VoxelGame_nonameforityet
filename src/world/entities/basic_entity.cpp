#include "basic_entity.hpp"

// =====================
// Construction/Destruction
// =====================

Render::GUI::BasicEntity::BasicEntity(const types::Rect<vec3f>& entity_rect, const types::Rect<vec3f>& hitbox, const types::Rect<types::uvs>& attributes)
	: m_origin{ entity_rect.pos }, m_baseSize{ entity_rect.size }, m_scale { 1.f, 1.f },
	m_hitbox{ hitbox.pos, -hitbox.size / 2.f, hitbox.size / 2.f },
	Mesh(std::array<Data::Vertex2D, 6>
{
	Render::Data::Vertex2D
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
})
{
	if (m_origin.x != 0 || m_origin.y != 0)
		m_transformNeedUpdate = true;
}

Render::GUI::BasicEntity::BasicEntity(BasicEntity&& other) noexcept
	: Mesh(std::move(other))
	, m_transformations		{ other.m_transformations }
	, m_scale{ other.m_scale }
	, m_baseSize{ other.m_baseSize }
	, m_origin{ other.m_origin }
	, m_position{ other.m_position }
	, m_rotation{ other.m_rotation }
	, m_transformNeedUpdate{ other.m_transformNeedUpdate }
	, m_hitbox{ other.m_hitbox }
{
}

Render::GUI::BasicEntity& Render::GUI::BasicEntity::operator=(BasicEntity&& other) noexcept
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
	m_hitbox				= other.m_hitbox;

	return *this;
}


// =====================
// Getters
// =====================

const mpml::Matrix4<float>& Render::GUI::BasicEntity::getTransformation() noexcept
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

void Render::GUI::BasicEntity::setPosition(vec3f pos) noexcept
{
	m_position = pos;
	m_transformNeedUpdate = true;
}

void Render::GUI::BasicEntity::setScale(vec3f scale) noexcept
{
	m_scale = scale;
	m_transformNeedUpdate = true;
}

void Render::GUI::BasicEntity::setSize(vec3f size) noexcept
{
	setScale({ size.x / m_baseSize.x, size.y / m_baseSize.y, size.z / m_baseSize.z });
}

void Render::GUI::BasicEntity::setRotation(mpml::Angle<> rotation) noexcept
{
	m_rotation = rotation;
	m_transformNeedUpdate = true;
}


void Render::GUI::BasicEntity::move(vec3f offset) noexcept
{
	setPosition(m_position + offset);
}

void Render::GUI::BasicEntity::scale(vec3f factor) noexcept
{
	setScale({ m_scale.x * factor.x,  m_scale.y * factor.y, m_scale.z * factor.z });
}

void Render::GUI::BasicEntity::rotate(mpml::Angle<> theta) noexcept
{
	setRotation(mpml::Angle<>::fromRadians(m_rotation.asRadians() + theta.asRadians()));
}

// 3D CUBE
void Render::GUI::BasicEntity::updateSprite(const types::Rect<types::uvs>& attributes) noexcept
{
	Mesh::updateBuffer<Render::Data::Vertex>
		(
			std::array<Data::Vertex, 6>
	{
	Render::Data::Vertex
	{
		{0, 0, 0},
		attributes.pos
	},
	{
		{m_baseSize.x, 0, 0},
		{attributes.pos + types::uvs{ attributes.size.x, 0}}
	},
	{
		{m_baseSize.x, m_baseSize.y},
		{attributes.pos + types::uvs{ attributes.size.x, attributes.size.y}}
	},
	{
		{0, 0, 0},
		attributes.pos
	},
	{
		{0, m_baseSize.y, 0},
		{attributes.pos + types::uvs{ 0, attributes.size.y}}
	},
	{
		{m_baseSize.x, m_baseSize.y, 0},
		{attributes.pos + types::uvs{ attributes.size.x, attributes.size.y}}
	},
	}, sizeof(Render::Data::Vertex2D), GL_STREAM_DRAW);
}


// =====================
// Predicates
// =====================

bool Render::GUI::BasicEntity::isWithin(const Physics::Collisions::BasicHitbox& container) const noexcept
{
	return container.intersects(m_hitbox);
}


// =====================
// Actors
// =====================

void Render::GUI::BasicEntity::draw(const Shader& shader, GLenum mode) noexcept
{
	shader.use();

	shader.setValue("model", getTransformation().transpose());

	Mesh::draw(mode);
}

void Render::GUI::BasicEntity::draw_transparent(const Shader& shader, GLenum mode) noexcept
{
	shader.use();

	shader.setValue("model", getTransformation().transpose());

	Mesh::draw_transparent(mode);
}