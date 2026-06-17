#pragma once // basic_entity.hpp
// MIT
// Allosker ------------------------------
// =========+
// Basic entity, this entity can be rotated but its hitbox is basic and it cannot
// ---------------------------------------

#include "utilities/opengl.hpp"

#include "rendering/mesh/mesh.hpp"

#include "rendering/shader.hpp"

#include "rendering/assets_managing/utilities.hpp"

#include "transforms/collisions/basicHitbox.hpp"

#include "transforms/transform3D.hpp"


namespace GameWorld::Entities
{
	
	class BasicEntity
		: public Render::Mesh, public Transform3D
	{
	public:

		// = Construction/Destruction

		BasicEntity(
			const types::Rect<vec3f>& entity_rect, 
			const vec3f& hitbox_size);


		BasicEntity(BasicEntity&& other) noexcept;

		BasicEntity& operator=(BasicEntity&& other) noexcept;


		virtual ~BasicEntity() = default;


		// = Setters

		void setPosition(const vec3f& pos) noexcept override
		{
			Transform3D::setPosition(pos); 
			m_hitbox.setHitbox(pos);
		}


		// = Getters

		const Physics::Collisions::BasicHitbox& getHitbox() const noexcept { return m_hitbox; }


		// = Predicates
		
		bool isWithin(const Physics::Collisions::BasicHitbox& container) const noexcept;

		
		// = Actors

		void draw(const Render::Shader& shader, GLenum mode = GL_TRIANGLES) noexcept;

		void draw_transparent(const Render::Shader& shader, GLenum mode = GL_TRIANGLES) noexcept;


	private:

		Physics::Collisions::BasicHitbox	m_hitbox				{};

	};

} // Render::GUI