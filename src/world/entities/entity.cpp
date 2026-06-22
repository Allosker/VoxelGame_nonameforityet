#include "entity.hpp"

// =====================
// Construction/Destruction
// =====================

entities::Entity::Entity(
	const types::Rect<vec3f>& entity_rect,
	const vec3f& hitbox_size) noexcept
	: physics::Transformable3D(entity_rect.pos, entity_rect.size)
	, m_hitbox{ entity_rect.pos, hitbox_size / 2.f, hitbox_size / 2.f }
{
}

entities::Entity::Entity(const types::Rect<vec3f>& entity_rect, const physics::collisions::BasicHitbox& hitbox) noexcept
	: physics::Transformable3D(entity_rect.pos, entity_rect.size)
	, m_hitbox{ hitbox }
{
}


// =====================
// Predicates
// =====================

bool entities::Entity::is_within(const physics::collisions::BasicHitbox& container) const noexcept
{
	return m_hitbox.intersects(container);
}


// =====================
// Actors
// =====================

void entities::Entity::draw(const render::Shader& shader, GLenum mode) noexcept
{
	shader.use();

	shader.setValue("model", getTransformation());

	Mesh::draw(mode);
}

void entities::Entity::draw_transparent(const render::Shader& shader, GLenum mode) noexcept
{
	shader.use();

	shader.setValue("model", getTransformation());

	Mesh::draw_transparent(mode);
}