#pragma once // basic_entity.hpp
// MIT
// Allosker ------------------------------
// =========+
// Basic entity, this entity can be rotated but its hitbox is basic and it cannot
// ---------------------------------------

#include "uHeaders/openGL.hpp"

#include "rendering/mesh/mesh.hpp"

#include "rendering/shader.hpp"

#include "rendering/assets_managing/utilities.hpp"

#include "physics/collisions/basicHitbox.hpp"


namespace Render::GUI
{
	
	class BasicEntity
		: protected Mesh
	{
	public:

		// = Construction/Destruction

		BasicEntity(const types::Rect<vec3f>& entity_rect, const types::Rect<vec3f>& hitbox, const types::Rect<types::uvs>& attributes);


		BasicEntity(BasicEntity&& other) noexcept;

		BasicEntity& operator=(BasicEntity&& other) noexcept;


		virtual ~BasicEntity() = default;


		// = Getters

		const mpml::Matrix4<float>& getTransformation() noexcept;

		vec3f getSize() const noexcept { return { m_baseSize.x * m_scale.x, m_baseSize.y * m_scale.y, m_baseSize.z * m_scale.z }; }
		const vec3f& getPosition() const noexcept { return m_position; }


		// = Setters

		void setPosition(vec3f pos) noexcept;

		void setScale(vec3f scale) noexcept;

		void setSize(vec3f size) noexcept;

		void setRotation(mpml::Angle<> rotation) noexcept;

		void move(vec3f offset) noexcept;

		void scale(vec3f factor) noexcept;

		void rotate(mpml::Angle<> theta) noexcept;



		void updateSprite(const types::Rect<types::uvs>& attributes) noexcept;


		// = Predicates
		
		bool isWithin(const Physics::Collisions::BasicHitbox& container) const noexcept;

		
		// = Actors

		void draw(const Shader& shader, GLenum mode = GL_TRIANGLES) noexcept;

		void draw_transparent(const Shader& shader, GLenum mode = GL_TRIANGLES) noexcept;


	private:

		mpml::Matrix4<float>				m_transformations		{ mpml::Identity4<float> };

		Physics::Collisions::BasicHitbox	m_hitbox				{};


		vec3f								m_scale					{};
		vec3f								m_baseSize				{};
		vec3f								m_origin				{};
		vec3f								m_position				{};

		mpml::Angle<>						m_rotation				{ mpml::Angle<>::fromRadians(0) };

		bool								m_transformNeedUpdate	{ false };

	};

} // Render::GUI