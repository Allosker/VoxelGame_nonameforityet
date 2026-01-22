#include "Rendering/world_managing/Data/chunk/chunkMesh.hpp"

// =====================
// Construction/Destruction
// =====================

Render::Data::ChunkMesh::ChunkMesh(const vec3f& pos) noexcept
{
	glGenVertexArrays(1, &m_vao);
	glCreateBuffers(1, &m_vbo);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	updateBuffer(generateChunkMesh());

	

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeVertex, std::bit_cast<void*>(offsetof(Vertex, pos)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeVertex, std::bit_cast<void*>(offsetof(Vertex, color)));
	glEnableVertexAttribArray(1);
}

#include <iostream>
Render::Data::ChunkMesh::~ChunkMesh() noexcept
{
	/*glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);*/
	std::cout << "Don't forget to create a move constructor: message from ~Chunk()\n";
}


// =====================
// Actors
// =====================

void Render::Data::ChunkMesh::draw() const noexcept
{
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, m_nbVertices);
}

std::vector<Render::Data::Vertex> Render::Data::ChunkMesh::buildMesh(const Gameplay::World::Chunk& chunk) noexcept
{
	std::vector<Vertex> meshes{};

	std::uint32_t z_stride{ Gameplay::World::Chunk::g_size * Gameplay::World::Chunk::g_size };

	// Put translation in a translation later on
	for (std::int32_t x{}; x < Gameplay::World::Chunk::g_size; x++)
	{
		for (std::int32_t y{}; y < Gameplay::World::Chunk::g_size; y++)
		{
			for (std::int32_t z{}; z < Gameplay::World::Chunk::g_size; z++)
			{
				using F = Cube::Filling;

				std::int32_t block_index{ static_cast<std::int32_t>((z * z_stride) + (y * Gameplay::World::Chunk::g_size) + x) };

				auto& current_block{ chunk.getCubeData()[block_index] };

				if (current_block.filling == F::Empty)
					continue;


				std::array<F, 6> CF_block_dirs{};


				if (z + 1 < 32)
					CF_block_dirs[0] = chunk.getCubeData()[block_index + z_stride].filling;
				else
					CF_block_dirs[0] = F::Empty;

				if (z - 1 >= 0)
					CF_block_dirs[1] = chunk.getCubeData()[block_index - z_stride].filling;
				else
					CF_block_dirs[1] = F::Empty;


				if (y + 1 < 32)
					CF_block_dirs[2] = chunk.getCubeData()[block_index + Gameplay::World::Chunk::g_size].filling;
				else
					CF_block_dirs[2] = F::Empty;

				if (y - 1 >= 0)
					CF_block_dirs[3] = chunk.getCubeData()[block_index - Gameplay::World::Chunk::g_size].filling;
				else
					CF_block_dirs[3] = F::Empty;


				if (x + 1 < 32)
					CF_block_dirs[4] = chunk.getCubeData()[block_index + 1].filling;
				else
					CF_block_dirs[4] = F::Empty;

				if (x - 1 >= 0)
					CF_block_dirs[5] = chunk.getCubeData()[block_index - 1].filling;
				else
					CF_block_dirs[5] = F::Empty;

				vec3f translation{ x + m_pos.x, y + m_pos.y, z + m_pos.z };

				if(y == 31)
					current_block.updateSpritePos(BlockType::c_dirtGrass);
				else
					current_block.updateSpritePos(BlockType::c_dirt);


																																
				if (CF_block_dirs[0] == F::Empty)
				{
					meshes.push_back({ Cube::faces[0 ] + translation, current_block.sprite_coords[0][0] });
					meshes.push_back({ Cube::faces[1 ] + translation, current_block.sprite_coords[0][1] });
					meshes.push_back({ Cube::faces[2 ] + translation, current_block.sprite_coords[0][2] });
					meshes.push_back({ Cube::faces[3 ] + translation, current_block.sprite_coords[0][3] });
					meshes.push_back({ Cube::faces[4 ] + translation, current_block.sprite_coords[0][4] });
					meshes.push_back({ Cube::faces[5 ] + translation, current_block.sprite_coords[0][5] });
				}																					 					   
																									 					   
				if (CF_block_dirs[1] == F::Empty)													 					   
				{																					 					   
					meshes.push_back({ Cube::faces[6 ] + translation, current_block.sprite_coords[1][0] });
					meshes.push_back({ Cube::faces[7 ] + translation, current_block.sprite_coords[1][1] });
					meshes.push_back({ Cube::faces[8 ] + translation, current_block.sprite_coords[1][2] });
					meshes.push_back({ Cube::faces[9 ] + translation, current_block.sprite_coords[1][3] });
					meshes.push_back({ Cube::faces[10] + translation, current_block.sprite_coords[1][4] });
					meshes.push_back({ Cube::faces[11] + translation, current_block.sprite_coords[1][5] });
				}																					 					   
																									 					   
				if (CF_block_dirs[2] == F::Empty)													 					   
				{																					 					   
					meshes.push_back({ Cube::faces[12] + translation, current_block.sprite_coords[2][0] });
					meshes.push_back({ Cube::faces[13] + translation, current_block.sprite_coords[2][1] });
					meshes.push_back({ Cube::faces[14] + translation, current_block.sprite_coords[2][2] });
					meshes.push_back({ Cube::faces[15] + translation, current_block.sprite_coords[2][3] });
					meshes.push_back({ Cube::faces[16] + translation, current_block.sprite_coords[2][4] });
					meshes.push_back({ Cube::faces[17] + translation, current_block.sprite_coords[2][5] });
				}																					 					   
																									 					   
				if (CF_block_dirs[3] == F::Empty)													 					   
				{																					 					   
					meshes.push_back({ Cube::faces[18] + translation, current_block.sprite_coords[3][0] });
					meshes.push_back({ Cube::faces[19] + translation, current_block.sprite_coords[3][1] });
					meshes.push_back({ Cube::faces[20] + translation, current_block.sprite_coords[3][2] });
					meshes.push_back({ Cube::faces[21] + translation, current_block.sprite_coords[3][3] });
					meshes.push_back({ Cube::faces[22] + translation, current_block.sprite_coords[3][4] });
					meshes.push_back({ Cube::faces[23] + translation, current_block.sprite_coords[3][5] });
				}																					 					   
																									 					   
				if (CF_block_dirs[4] == F::Empty)													 					   
				{																					 					   
					meshes.push_back({ Cube::faces[24] + translation, current_block.sprite_coords[4][0] });
					meshes.push_back({ Cube::faces[25] + translation, current_block.sprite_coords[4][1] });
					meshes.push_back({ Cube::faces[26] + translation, current_block.sprite_coords[4][2] });
					meshes.push_back({ Cube::faces[27] + translation, current_block.sprite_coords[4][3] });
					meshes.push_back({ Cube::faces[28] + translation, current_block.sprite_coords[4][4] });
					meshes.push_back({ Cube::faces[29] + translation, current_block.sprite_coords[4][5] });
				}																					 					   
																									 					   
				if (CF_block_dirs[5] == F::Empty)													 					   
				{																					 					   
					meshes.push_back({ Cube::faces[30] + translation, current_block.sprite_coords[5][0] });
					meshes.push_back({ Cube::faces[31] + translation, current_block.sprite_coords[5][1] });
					meshes.push_back({ Cube::faces[32] + translation, current_block.sprite_coords[5][2] });
					meshes.push_back({ Cube::faces[33] + translation, current_block.sprite_coords[5][3] });
					meshes.push_back({ Cube::faces[34] + translation, current_block.sprite_coords[5][4] });
					meshes.push_back({ Cube::faces[35] + translation, current_block.sprite_coords[5][5] });
				}

			}
		}

	}

	return meshes;
}

void Render::Data::ChunkMesh::updateBuffer(const std::vector<Vertex>& meshes) noexcept
{
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, meshes.size() * sizeof(Vertex), meshes.data(), GL_STATIC_DRAW);
	m_nbVertices = meshes.size();
}
