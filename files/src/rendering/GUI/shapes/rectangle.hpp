#pragma once // rectangle.hpp
// MIT
// Allosker ------------------------------
// =========+
// Basic class for most GUI related meshes, basic rectangle for basically anything and everything displayed on screen for the GUI
// ---------------------------------------

#include "uHeaders/openGL.hpp"

#include "rendering/mesh/mesh.hpp"

#include "rendering/shader.hpp"

#include "rendering/assets_managing/utilities.hpp"

namespace Render::GUI
{
	
	class Rectangle
		: protected Mesh
	{
	public:

		// = Construction/Destruction

		Rectangle(vec2f size, vec2f ori, const UvPixels& attributes);


		Rectangle(Rectangle&& other) noexcept;

		Rectangle& operator=(Rectangle&& other) noexcept;


		virtual ~Rectangle() = default;


		// = Getters

		const mpml::Matrix4<float>& getTransformation() noexcept;

		vec2f getSize() const noexcept { return { m_baseSize.x * m_scale.x, m_baseSize.y * m_scale.y }; }
		const vec2f& getPosition() const noexcept { return m_position; }


		// = Setters

		void setPosition(vec2f pos) noexcept;

		void setScale(vec2f scale) noexcept;

		void setSize(vec2f size) noexcept;

		void setRotation(mpml::Angle<> rotation) noexcept;

		void move(vec2f offset) noexcept;

		void scale(vec2f factor) noexcept;

		void rotate(mpml::Angle<> theta) noexcept;



		void updateSprite(const UvPixels& attributes) noexcept;


		// = Predicates
		
		bool contains(vec2f point) const noexcept;

		
		// = Actors

		void draw(const Shader& shader, GLenum mode = GL_TRIANGLES) noexcept;

		void draw_transparent(const Shader& shader, GLenum mode = GL_TRIANGLES) noexcept;


	private:

		mpml::Matrix4<float>	m_transformations		{ mpml::Identity4<float> };

		vec2f					m_scale					{};
		vec2f					m_baseSize				{};
		vec2f					m_origin				{};
		vec2f					m_position				{};

		mpml::Angle<>			m_rotation				{ mpml::Angle<>::fromRadians(0) };

		bool					m_transformNeedUpdate	{ false };

	};

} // Render::GUI