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

		Mesh(const std::vector<Data::Vertex2D>& vertices, GLenum draw_mode = GL_STATIC_DRAW);

		template<std::size_t SIZE>
		Mesh(const std::array<Data::Vertex2D, SIZE>& vertices, GLenum draw_mode = GL_STATIC_DRAW);

		Mesh(const std::vector<Data::VertexColor>& vertices, GLenum draw_mode = GL_STATIC_DRAW);

		Mesh(const std::vector<vec3f>& positions, const std::vector<vec2f>& uvs, GLenum draw_mode = GL_STATIC_DRAW);

		Mesh(const Mesh&) = delete;
		Mesh(Mesh&& other) noexcept;

		Mesh& operator=(const Mesh&) = delete;
		Mesh& operator=(Mesh&& other) noexcept;

		virtual ~Mesh() noexcept;


		// = Actors
		
		template<typename T>
		void updateBuffer(const std::vector<T>& vertices, GLsizeiptr size_data, GLenum draw_mode = GL_STATIC_DRAW) noexcept;

		template<typename T, std::size_t SIZE>
		void updateBuffer(const std::array<T, SIZE>& vertices, GLsizeiptr size_data, GLenum draw_mode = GL_STATIC_DRAW) noexcept;

		void updateBuffer(const std::vector<vec3f>& positions, const std::vector<vec2f>& uvs, GLenum draw_mode = GL_STATIC_DRAW);

		virtual void draw(GLenum mode = GL_TRIANGLES) const noexcept;

		virtual void draw_transparent(GLenum mode = GL_TRIANGLES) const noexcept;


	private:

		std::vector<Data::Vertex> makeVertices(const std::vector<vec3f>& positions, const std::vector<vec2f>& uvs) const;

		template<typename T>
		void createBuffers(const std::vector<T>& vertices, GLenum draw_mode, vec2i components) noexcept;

		template<typename T, std::size_t SIZE>
		void createBuffers(const std::array < T, SIZE>& vertices, GLenum draw_mode, vec2i components) noexcept;

		GLuint m_vao{};
		GLuint m_vbo{};

		GLsizei m_nbVertices{};

		bool m_moved{ false };

	};

	// Construction/Destruction

	template<std::size_t SIZE>
	inline Mesh::Mesh(const std::array<Data::Vertex2D, SIZE>& vertices, GLenum draw_mode)
	{
		createBuffers<Data::Vertex2D, SIZE>(vertices, draw_mode, { 2, 2 });
	}

	// Actors

	template<typename T>
	inline void Mesh::updateBuffer(const std::vector<T>& vertices, GLsizeiptr size_data, GLenum draw_mode) noexcept
	{
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		m_nbVertices = static_cast<GLsizei>(vertices.size());

		glBufferData(GL_ARRAY_BUFFER, m_nbVertices * size_data, vertices.data(), draw_mode);
		glBindVertexArray(0);
	}

	template<typename T, std::size_t SIZE>
	inline void Mesh::updateBuffer(const std::array<T, SIZE>& vertices, GLsizeiptr size_data, GLenum draw_mode) noexcept
	{
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		m_nbVertices = static_cast<GLsizei>(vertices.size());

		glBufferData(GL_ARRAY_BUFFER, m_nbVertices * size_data, vertices.data(), draw_mode);
		glBindVertexArray(0);
	}

	// Private Init

	template<typename T>
	inline void Mesh::createBuffers(const std::vector<T>& vertices, GLenum draw_mode, vec2i components) noexcept
	{
		glCreateVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);

		updateBuffer(vertices, sizeof(T), draw_mode);

		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		glVertexAttribPointer(0, components.x, GL_FLOAT, false, sizeof(T), std::bit_cast<void*>(offsetof(T, pos)));
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, components.y, GL_FLOAT, false, sizeof(T), std::bit_cast<void*>(offsetof(T, uv)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}

	template<typename T, std::size_t SIZE>
	inline void Mesh::createBuffers(const std::array<T, SIZE>& vertices, GLenum draw_mode, vec2i components) noexcept
	{
		glCreateVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);

		updateBuffer(vertices, sizeof(T), draw_mode);

		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		glVertexAttribPointer(0, components.x, GL_FLOAT, false, sizeof(T), std::bit_cast<void*>(offsetof(T, pos)));
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, components.y, GL_FLOAT, false, sizeof(T), std::bit_cast<void*>(offsetof(T, uv)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}

} // Render