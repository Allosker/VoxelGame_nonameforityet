#pragma once // basic_entity.hpp
// MIT
// Allosker ------------------------------
// =========+
// Basic entity, this entity can be rotated but its hitbox is basic and it cannot
// ---------------------------------------

#include "utilities/opengl.hpp"

#include "rendering/mesh.hpp"

#include "rendering/shader.hpp"

#include "rendering/utilities/atlas.hpp"

#include "physics/collisions/basicHitbox.hpp"

#include "physics/transforms/transformable3D.hpp"


namespace entities
{
	
	class Entity
		: public render::Mesh, public physics::Transformable3D
	{
	public:

		// = Construction/Destruction

		Entity(
			const types::Rect<vec3f>& entity_rect, 
			const vec3f& hitbox_size) noexcept;

		Entity(const types::Rect<vec3f>& entity_rect, const physics::collisions::BasicHitbox& hitbox) noexcept;

		Entity(Entity&&) noexcept = default;
		Entity& operator=(Entity&&) noexcept = default;


		virtual ~Entity() = default;


		// = Setters

		void setPosition(const vec3f& pos) noexcept override
		{
			physics::Transformable3D::setPosition(pos); 
			m_hitbox.setHitbox(pos);
		}


		// = Getters

		physics::collisions::BasicHitbox& getHitbox() noexcept { return m_hitbox; }

		const physics::collisions::BasicHitbox& getHitbox() const noexcept { return m_hitbox; }


		// = Predicates
		
		bool is_within(const physics::collisions::BasicHitbox& container) const noexcept;

		
		// = Actors

		void draw(const render::Shader& shader, GLenum mode = GL_TRIANGLES) noexcept;

		void draw_transparent(const render::Shader& shader, GLenum mode = GL_TRIANGLES) noexcept;


	protected:

		physics::collisions::BasicHitbox	m_hitbox{};

	};

} // entities