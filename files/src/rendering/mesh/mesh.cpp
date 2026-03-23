#include "mesh.hpp"



// =====================
// Construction/Destruction
// =====================

Render::Mesh::Mesh(const std::vector<Data::Vertex>& vertices, GLenum draw_mode)
{
	glCreateVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	updateBuffer(vertices, sizeof(Data::Vertex), draw_mode);

	glBindVertexArray(m_vao);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Data::Vertex), std::bit_cast<void*>(offsetof(Data::Vertex, pos)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Data::Vertex), std::bit_cast<void*>(offsetof(Data::Vertex, uv)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

Render::Mesh::Mesh(const std::vector<Data::VertexColor>& vertices, GLenum draw_mode)
{
	glCreateVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	updateBuffer(vertices, sizeof(Data::VertexColor), draw_mode);

	glBindVertexArray(m_vao);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Data::VertexColor), std::bit_cast<void*>(offsetof(Data::VertexColor, pos)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Data::VertexColor), std::bit_cast<void*>(offsetof(Data::VertexColor, color)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

Render::Mesh::Mesh(const std::vector<vec3f>& positions, const std::vector<vec2f>& uvs, GLenum draw_mode)
	: Mesh{makeVertices(positions, uvs), draw_mode}
{
}

Render::Mesh::Mesh(Mesh&& other) noexcept
{
	*this = std::move(other);
}

Render::Mesh& Render::Mesh::operator=(Mesh&& other) noexcept
{
	if (this == &other)
		return *this;

	m_vao = other.m_vao;
	m_vbo = other.m_vbo;
	m_nbVertices = other.m_nbVertices;

	other.m_moved = true;
	other.m_vao = 0;
	other.m_vbo = 0;
	other.m_nbVertices = 0;

	return *this;
}

Render::Mesh::~Mesh() noexcept
{
	if (!m_moved)
	{
		glDeleteBuffers(1, &m_vbo);
		glDeleteVertexArrays(1, &m_vao);
	}
}


// =====================
// Actors 
// =====================

void Render::Mesh::updateBuffer(const auto& vertices, GLsizeiptr size_data, GLenum draw_mode) noexcept
{
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	m_nbVertices = static_cast<GLsizei>(vertices.size());

	glBufferData(GL_ARRAY_BUFFER, m_nbVertices * size_data, vertices.data(), draw_mode);
	glBindVertexArray(0);
}

void Render::Mesh::updateBuffer(const std::vector<vec3f>& positions, const std::vector<vec2f>& uvs, GLenum draw_mode)
{
	updateBuffer(makeVertices(positions, uvs), sizeof(Data::Vertex), draw_mode);
}

void Render::Mesh::draw(GLenum mode) const noexcept
{
	glBindVertexArray(m_vao);
	glDrawArrays(mode, 0, m_nbVertices);
	glBindVertexArray(0);
}

/*private*/ std::vector<Render::Data::Vertex> Render::Mesh::makeVertices(const std::vector<vec3f>& positions, const std::vector<vec2f>& uvs) const
{
	if (positions.size() != uvs.size()) throw std::runtime_error{ "ERROR::SINGLE_MESH::There must be the same number of positions and uvs" };

	std::vector<Data::Vertex> vertices(positions.size());

	for (size_t i{}; i < positions.size(); i++)
		vertices[i] = Data::Vertex{ positions[i], uvs[i] };

	return vertices;
}
