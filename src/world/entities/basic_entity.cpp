#include "basic_entity.hpp"

// =====================
// Construction/Destruction
// =====================

GameWorld::Entities::BasicEntity::BasicEntity(
	const types::Rect<vec3f>& entity_rect,
	const types::Rect<vec3f>& hitbox)
	:
	  m_origin{ entity_rect.pos }
	, m_baseSize{ entity_rect.size }
	, m_scale { 1.f, 1.f }
	, m_hitbox{ hitbox.pos, -hitbox.size / 2.f, hitbox.size / 2.f }
{
	if (m_origin.x != 0 || m_origin.y != 0)
		m_transformNeedUpdate = true;
}

GameWorld::Entities::BasicEntity::BasicEntity(BasicEntity&& other) noexcept
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

GameWorld::Entities::BasicEntity& GameWorld::Entities::BasicEntity::operator=(BasicEntity&& other) noexcept
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

const mpml::Matrix4<float>& GameWorld::Entities::BasicEntity::getTransformation() noexcept
{
	// Recompute the combined transform if needed
	if (m_transformNeedUpdate)
	{
		mpml::Matrix4<float> transforms{ mpml::Identity4<float> };
		
		transforms = mpml::scale(transforms, m_scale);
		transforms = mpml::rotate(transforms, m_rotation);
		transforms = mpml::translate(transforms, m_position);


		m_transformations = transforms;

		m_transformNeedUpdate = false;
	}

	return m_transformations;
}


// =====================
// Setters
// =====================

void GameWorld::Entities::BasicEntity::setPosition(vec3f pos) noexcept
{
	m_position = pos;
	m_transformNeedUpdate = true;
}

void GameWorld::Entities::BasicEntity::setScale(vec3f scale) noexcept
{
	m_scale = scale;
	m_transformNeedUpdate = true;
}

void GameWorld::Entities::BasicEntity::setSize(vec3f size) noexcept
{
	setScale({ size.x / m_baseSize.x, size.y / m_baseSize.y, size.z / m_baseSize.z });
}

void GameWorld::Entities::BasicEntity::setRotation(const mpml::Quaternion<float>& q) noexcept
{
	m_rotation = q;
	m_transformNeedUpdate = true;
}


void GameWorld::Entities::BasicEntity::move(vec3f offset) noexcept
{
	setPosition(m_position + offset);
}

void GameWorld::Entities::BasicEntity::scale(vec3f factor) noexcept
{
	setScale({ m_scale.x * factor.x,  m_scale.y * factor.y, m_scale.z * factor.z });
}

void GameWorld::Entities::BasicEntity::rotate(const mpml::Quaternion<float>& q) noexcept
{
	setRotation(m_rotation * q);
}


// =====================S
// Predicates
// =====================

bool GameWorld::Entities::BasicEntity::isWithin(const Physics::Collisions::BasicHitbox& container) const noexcept
{
	return m_hitbox.intersects(container);
}


// =====================
// Actors
// =====================

void GameWorld::Entities::BasicEntity::draw(const Render::Shader& shader, GLenum mode) noexcept
{
	shader.use();

	shader.setValue("model", getTransformation());

	Mesh::draw(mode);
}

void GameWorld::Entities::BasicEntity::draw_transparent(const Render::Shader& shader, GLenum mode) noexcept
{
	shader.use();

	shader.setValue("model", getTransformation());

	Mesh::draw_transparent(mode);
}