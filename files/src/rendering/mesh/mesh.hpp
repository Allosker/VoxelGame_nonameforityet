#pragma once // mesh.hpp
// MIT
// Allosker ------------------------------
// =========+
// Small class that creates a buffer on the GPU for drawing stuff on screen
// ---------------------------------------

#include "uHeaders/opengl.hpp"

#include "rendering/shader.hpp"
#include "rendering/world_managing/data/basic/vertex.hpp"

#include <vector>

namespace Render
{


	class Mesh
	{
	public:

		// = Destruction/Creation

		Mesh(const std::vector<Data::Vertex>& vertices, GLenum draw_mode = GL_STATIC_DRAW);

		Mesh(const std::vector<Data::VertexColor>& vertices, GLenum draw_mode = GL_STATIC_DRAW);

		Mesh(const std::vector<vec3f>& positions, const std::vector<vec2f>& uvs, GLenum draw_mode = GL_STATIC_DRAW);

		Mesh(const Mesh&) = delete;
		Mesh(Mesh&& other) noexcept;

		Mesh& operator=(const Mesh&) = delete;
		Mesh& operator=(Mesh&& other) noexcept;

		~Mesh() noexcept;


		// = Actors
		
		void updateBuffer(const auto& vertices, GLsizeiptr size_data, GLenum draw_mode = GL_STATIC_DRAW) noexcept;

		void updateBuffer(const std::vector<vec3f>& positions, const std::vector<vec2f>& uvs, GLenum draw_mode = GL_STATIC_DRAW);

		void draw(GLenum mode = GL_TRIANGLES) const noexcept;


	private:

		std::vector<Data::Vertex> makeVertices(const std::vector<vec3f>& positions, const std::vector<vec2f>& uvs) const;

		GLuint m_vao{};
		GLuint m_vbo{};

		GLsizei m_nbVertices{};

		bool m_moved{ false };

	};


} // Render