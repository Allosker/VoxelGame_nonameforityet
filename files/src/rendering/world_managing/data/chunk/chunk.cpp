#include "Rendering/world_managing/Data/chunk/chunk.hpp"

// =====================
// Construction/Destruction
// =====================

Render::Data::Chunk::Chunk(const vec3f& pos) noexcept
	: m_cubes_chunk(g_dimensions* g_dimensions* g_dimensions, Cube::Filling::Full),
	m_pos{ pos }
{
	for (size_t i{ 15 }; i < 32; i++)
		m_cubes_chunk[i] = Cube::Filling::Empty;

	glGenVertexArrays(1, &m_vao);
	glCreateBuffers(1, &m_vbo);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	generateChunkMesh();

	constexpr std::uint32_t sizeVertex{ sizeof(Vertex) };

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeVertex, std::bit_cast<void*>(offsetof(Vertex, pos)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeVertex, std::bit_cast<void*>(offsetof(Vertex, color)));
	glEnableVertexAttribArray(1);
}

#include <iostream>
Render::Data::Chunk::~Chunk() noexcept
{
	/*glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);*/
	std::cout << "Don't forget to create a move constructor: message from ~Chunk()\n";
}


// =====================
// Actors
// =====================

void Render::Data::Chunk::draw() const noexcept
{
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, m_nbVertices);
}

void Render::Data::Chunk::generateChunkMesh() noexcept
{
	glBindVertexArray(m_vao);

	std::vector<Vertex> meshes{};

	std::uint32_t z_stride{ g_dimensions * g_dimensions };

	// Put translation in a translation later on
	for (std::int32_t x{}; x < g_dimensions; x++)
	{
		for (std::int32_t y{}; y < g_dimensions; y++)
		{
			for (std::int32_t z{}; z < g_dimensions; z++)
			{
				using F = Cube::Filling;

				std::int32_t block_index{ static_cast<std::int32_t>((z * z_stride) + (y * g_dimensions) + x) };

				if (m_cubes_chunk[block_index] == F::Empty)
					continue;


				std::array<F, 6> CF_block_dirs{};


				if (z + 1 < 32)
					CF_block_dirs[0] = m_cubes_chunk[block_index + z_stride];
				else
					CF_block_dirs[0] = F::Empty;

				if (z - 1 >= 0)
					CF_block_dirs[1] = m_cubes_chunk[block_index - z_stride];
				else
					CF_block_dirs[1] = F::Empty;


				if (y + 1 < 32)
					CF_block_dirs[2] = m_cubes_chunk[block_index + g_dimensions];
				else
					CF_block_dirs[2] = F::Empty;

				if (y - 1 >= 0)
					CF_block_dirs[3] = m_cubes_chunk[block_index - g_dimensions];
				else
					CF_block_dirs[3] = F::Empty;


				if (x + 1 < 32)
					CF_block_dirs[4] = m_cubes_chunk[block_index + 1];
				else
					CF_block_dirs[4] = F::Empty;

				if (x - 1 >= 0)
					CF_block_dirs[5] = m_cubes_chunk[block_index - 1];
				else
					CF_block_dirs[5] = F::Empty;



				if (CF_block_dirs[0] == F::Empty)
				{
					meshes.push_back({ cube_faces[0].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[0].color });
					meshes.push_back({ cube_faces[1].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[1].color });
					meshes.push_back({ cube_faces[2].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[2].color });
					meshes.push_back({ cube_faces[3].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[3].color });
					meshes.push_back({ cube_faces[4].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[4].color });
					meshes.push_back({ cube_faces[5].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[5].color });
				}

				if (CF_block_dirs[1] == F::Empty)
				{
					meshes.push_back({ cube_faces[6].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[6].color });
					meshes.push_back({ cube_faces[7].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[7].color });
					meshes.push_back({ cube_faces[8].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[8].color });
					meshes.push_back({ cube_faces[9].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[9].color });
					meshes.push_back({ cube_faces[10].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[10].color });
					meshes.push_back({ cube_faces[11].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[11].color });
				}

				if (CF_block_dirs[2] == F::Empty)
				{
					meshes.push_back({ cube_faces[12].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[12].color });
					meshes.push_back({ cube_faces[13].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[13].color });
					meshes.push_back({ cube_faces[14].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[14].color });
					meshes.push_back({ cube_faces[15].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[15].color });
					meshes.push_back({ cube_faces[16].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[16].color });
					meshes.push_back({ cube_faces[17].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[17].color });
				}

				if (CF_block_dirs[3] == F::Empty)
				{
					meshes.push_back({ cube_faces[18].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[18].color });
					meshes.push_back({ cube_faces[19].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[19].color });
					meshes.push_back({ cube_faces[20].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[20].color });
					meshes.push_back({ cube_faces[21].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[21].color });
					meshes.push_back({ cube_faces[22].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[22].color });
					meshes.push_back({ cube_faces[23].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[23].color });
				}

				if (CF_block_dirs[4] == F::Empty)
				{
					meshes.push_back({ cube_faces[24].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[24].color });
					meshes.push_back({ cube_faces[25].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[25].color });
					meshes.push_back({ cube_faces[26].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[26].color });
					meshes.push_back({ cube_faces[27].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[27].color });
					meshes.push_back({ cube_faces[28].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[28].color });
					meshes.push_back({ cube_faces[29].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[29].color });
				}

				if (CF_block_dirs[5] == F::Empty)
				{
					meshes.push_back({ cube_faces[30].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[30].color });
					meshes.push_back({ cube_faces[31].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[31].color });
					meshes.push_back({ cube_faces[32].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[32].color });
					meshes.push_back({ cube_faces[33].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[33].color });
					meshes.push_back({ cube_faces[34].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[34].color });
					meshes.push_back({ cube_faces[35].pos + vec3f{ x + m_pos.x, y + m_pos.y, z + m_pos.z }, cube_faces[35].color });
				}

			}
		}

	}

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, meshes.size() * sizeof(Vertex), meshes.data(), GL_STATIC_DRAW);
	m_nbVertices = meshes.size();
}
