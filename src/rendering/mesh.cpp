#include "mesh.hpp"



// =====================
// Construction/Destruction
// =====================

render::Mesh::Mesh(const std::vector<Data::Vertex>& vertices, GLenum draw_mode)
{
	createBuffers<Data::Vertex>(vertices, draw_mode, {3, 2});
}

render::Mesh::Mesh(const std::vector<Data::Vertex2D>& vertices, GLenum draw_mode)
{
	createBuffers<Data::Vertex2D>(vertices, draw_mode, {2, 2});
}

render::Mesh::Mesh(const std::vector<Data::VertexRGB>& vertices, GLenum draw_mode)
{
	glCreateVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	updateBuffer(vertices, sizeof(Data::VertexRGB), draw_mode);

	glBindVertexArray(m_vao);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Data::VertexRGB), std::bit_cast<void*>(offsetof(Data::VertexRGB, pos)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Data::VertexRGB), std::bit_cast<void*>(offsetof(Data::VertexRGB, color)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

render::Mesh::Mesh(const std::vector<vec3f>& positions, const std::vector<vec2f>& uvs, GLenum draw_mode)
	: Mesh{makeVertices(positions, uvs), draw_mode}
{
}

render::Mesh::Mesh(Mesh&& other) noexcept
	: m_vao { other.m_vao }
	,m_vbo { other.m_vbo }
	,m_nbVertices { other.m_nbVertices }
{
	other.m_vao = 0;
	other.m_vbo = 0;
}

render::Mesh& render::Mesh::operator=(Mesh&& other) noexcept
{
	if (this == &other)
		return *this;

	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);

	m_vao			= other.m_vao;
	m_vbo			= other.m_vbo;
	m_nbVertices	= other.m_nbVertices;

	other.m_vao = 0;
	other.m_vbo = 0;
	other.m_nbVertices = 0;

	return *this;
}

render::Mesh::~Mesh() noexcept
{
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
}


// =====================
// Actors 
// =====================

void render::Mesh::updateBuffer(const std::vector<vec3f>& positions, const std::vector<vec2f>& uvs, GLenum draw_mode)
{
	updateBuffer(makeVertices(positions, uvs), sizeof(Data::Vertex), draw_mode);
}

void render::Mesh::draw(GLenum mode) const noexcept
{
	glBindVertexArray(m_vao);
	glDrawArrays(mode, 0, m_nbVertices);
	glBindVertexArray(0);
}

void render::Mesh::draw_transparent(GLenum mode) const noexcept
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindVertexArray(m_vao);
	glDrawArrays(mode, 0, m_nbVertices);
	glBindVertexArray(0);

	glDisable(GL_BLEND);
}

/*private*/ std::vector<Data::Vertex> render::Mesh::makeVertices(const std::vector<vec3f>& positions, const std::vector<vec2f>& uvs) const
{
	if (positions.size() != uvs.size()) throw std::runtime_error{ "ERROR::SINGLE_MESH::There must be the same number of positions and uvs" };

	std::vector<Data::Vertex> vertices(positions.size());

	for (size_t i{}; i < positions.size(); i++)
		vertices[i] = Data::Vertex{ positions[i], uvs[i] };

	return vertices;
}
