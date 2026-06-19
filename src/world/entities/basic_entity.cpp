#include "basic_entity.hpp"

// =====================
// Construction/Destruction
// =====================

Entities::BasicEntity::BasicEntity(
	const types::Rect<vec3f>& entity_rect,
	const vec3f& hitbox_size)
	: Transform3D(entity_rect.pos, entity_rect.size)
	, m_hitbox{ entity_rect.pos, hitbox_size / 2.f, hitbox_size / 2.f }
{
	if (m_origin.x != 0 || m_origin.y != 0)
		m_transformNeedUpdate = true;
}

Entities::BasicEntity::BasicEntity(BasicEntity&& other) noexcept
	: Mesh(std::move(other))
	, Transform3D(other)
	, m_hitbox{ other.m_hitbox }
{
}

Entities::BasicEntity& Entities::BasicEntity::operator=(BasicEntity&& other) noexcept
{
	if (this == &other)
		return *this;

	Mesh::operator=(std::move(other));
	Transform3D::operator=(other);
	m_hitbox = other.m_hitbox;

	return *this;
}


// =====================
// Predicates
// =====================

bool Entities::BasicEntity::isWithin(const Physics::Collisions::BasicHitbox& container) const noexcept
{
	return m_hitbox.intersects(container);
}


// =====================
// Actors
// =====================

void Entities::BasicEntity::draw(const Render::Shader& shader, GLenum mode) noexcept
{
	shader.use();

	shader.setValue("model", getTransformation());

	Mesh::draw(mode);
}

void Entities::BasicEntity::draw_transparent(const Render::Shader& shader, GLenum mode) noexcept
{
	shader.use();

	shader.setValue("model", getTransformation());

	Mesh::draw_transparent(mode);
}