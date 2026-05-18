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

#include "transforms/transform2D.hpp"

namespace Render::GUI
{
	
	class Rectangle
		: protected Mesh, public Transform2D
	{
	public:

		// = Construction/Destruction

		Rectangle(vec2f size, vec2f ori, const types::Rect<types::uvs>& attributes);


		Rectangle(Rectangle&& other) noexcept;

		Rectangle& operator=(Rectangle&& other) noexcept;


		virtual ~Rectangle() = default;


		// = Setters

		void updateSprite(const types::Rect<types::uvs>& attributes) noexcept;


		// = Predicates
		
		bool contains(vec2f point) const noexcept;

		
		// = Actors

		virtual void draw(const Shader& shader, GLenum mode = GL_TRIANGLES) noexcept;

		void draw_transparent(const Shader& shader, GLenum mode = GL_TRIANGLES) noexcept;

	};

} // Render::GUI