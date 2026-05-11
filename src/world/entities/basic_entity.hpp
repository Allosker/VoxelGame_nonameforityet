#pragma once // basic_entity.hpp
// MIT
// Allosker ------------------------------
// =========+
// Basic entity, this entity can be rotated but its hitbox is basic and it cannot
// ---------------------------------------

#include "uHeaders/opengl.hpp"

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
			const types::Rect<vec3f>& hitbox);


		BasicEntity(BasicEntity&& other) noexcept;

		BasicEntity& operator=(BasicEntity&& other) noexcept;


		virtual ~BasicEntity() = default;


		// = Predicates
		
		bool isWithin(const Physics::Collisions::BasicHitbox& container) const noexcept;

		
		// = Actors

		void draw(const Render::Shader& shader, GLenum mode = GL_TRIANGLES) noexcept;

		void draw_transparent(const Render::Shader& shader, GLenum mode = GL_TRIANGLES) noexcept;


	private:

		Physics::Collisions::BasicHitbox	m_hitbox				{};

	};

} // Render::GUI