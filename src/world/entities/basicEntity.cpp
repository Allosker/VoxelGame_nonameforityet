#include "basicEntity.hpp"

// =====================
// Construction/Destruction
// =====================

entities::BasicEntity::BasicEntity(
	const types::Rect<vec3f>& entity_rect,
	const vec3f& hitbox_size) noexcept
	: physics::Transformable3D(entity_rect.pos, entity_rect.size)
	, m_hitbox{ entity_rect.pos, hitbox_size / 2.f, hitbox_size / 2.f }
{
	if (m_origin.x != 0 || m_origin.y != 0)
		m_transformNeedUpdate = true;
}

entities::BasicEntity::BasicEntity(const types::Rect<vec3f>& entity_rect, const physics::collisions::BasicHitbox& hitbox) noexcept
	: physics::Transformable3D(entity_rect.pos, entity_rect.size)
	, m_hitbox{ hitbox }
{
}

entities::BasicEntity::BasicEntity(BasicEntity&& other) noexcept
	: Mesh(std::move(other))
	, physics::Transformable3D(other)
	, m_hitbox{ other.m_hitbox }
{
}

entities::BasicEntity& entities::BasicEntity::operator=(BasicEntity&& other) noexcept
{
	if (this == &other)
		return *this;

	Mesh::operator=(std::move(other));
	physics::Transformable3D::operator=(other);
	m_hitbox = other.m_hitbox;

	return *this;
}


// =====================
// Predicates
// =====================

bool entities::BasicEntity::isWithin(const physics::collisions::BasicHitbox& container) const noexcept
{
	return m_hitbox.intersects(container);
}


// =====================
// Actors
// =====================

void entities::BasicEntity::draw(const render::Shader& shader, GLenum mode) noexcept
{
	shader.use();

	shader.setValue("model", getTransformation());

	Mesh::draw(mode);
}

void entities::BasicEntity::draw_transparent(const render::Shader& shader, GLenum mode) noexcept
{
	shader.use();

	shader.setValue("model", getTransformation());

	Mesh::draw_transparent(mode);
}