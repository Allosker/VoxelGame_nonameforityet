#include "rendering/world_managing/data/chunk/chunkMesh.hpp"

// =====================
// Construction/Destruction
// =====================

Render::Data::ChunkMesh::ChunkMesh(GameWorld::Voxels::Chunk& chunk, const Render::Data::Types::VoxelTypeManager& type_manager) noexcept
{
	glGenVertexArrays(1, &m_vao);
	glCreateBuffers(1, &m_vbo);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	updateBuffer(buildMesh(chunk, type_manager));

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeVertex, std::bit_cast<void*>(offsetof(Vertex, pos)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeVertex, std::bit_cast<void*>(offsetof(Vertex, uv)));
	glEnableVertexAttribArray(1);

}

Render::Data::ChunkMesh::ChunkMesh(ChunkMesh&& other) noexcept
{
	*this = std::move(other);
}

Render::Data::ChunkMesh& Render::Data::ChunkMesh::operator=(ChunkMesh&& other) noexcept
{
	if (this == &other)
		return *this;

	m_nbVertices = other.m_nbVertices;
	m_vao = other.m_vao;
	m_vbo = other.m_vbo;

	other.m_nbVertices = 0;
	other.m_vao = 0;
	other.m_vbo = 0;

	return *this;
}

Render::Data::ChunkMesh::~ChunkMesh() noexcept
{
	destroy();
}


// =====================
// Actors
// =====================

void Render::Data::ChunkMesh::draw() const noexcept
{
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, m_nbVertices);
}

std::vector<Render::Data::Vertex> Render::Data::ChunkMesh::buildMesh(const GameWorld::Voxels::Chunk& chunk, const Render::Data::Types::VoxelTypeManager& type_manager) noexcept
{
	std::vector<Vertex> meshes{};

	std::uint32_t z_stride{ GameWorld::Voxels::Chunk::g_size * GameWorld::Voxels::Chunk::g_size };

	   
	for (std::int32_t x{}; x < GameWorld::Voxels::Chunk::g_size; x++)
	{
		for (std::int32_t y{}; y < GameWorld::Voxels::Chunk::g_size; y++)
		{
			for (std::int32_t z{}; z < GameWorld::Voxels::Chunk::g_size; z++)
			{
				std::int32_t block_index{ static_cast<std::int32_t>((z * z_stride) + (y * GameWorld::Voxels::Chunk::g_size) + x) };

				auto& current_block{ chunk.getVoxelData()[block_index] };

				if (!current_block.id)
					continue;


				std::array<size_t, 6> CF_block_dirs{};


				if (z + 1 < 32)
					CF_block_dirs[0] = chunk.getVoxelData()[block_index + z_stride].id;
				else
					CF_block_dirs[0] = 0;

				if (z - 1 >= 0)
					CF_block_dirs[1] = chunk.getVoxelData()[block_index - z_stride].id;
				else
					CF_block_dirs[1] = 0;


				if (y + 1 < 32)
					CF_block_dirs[2] = chunk.getVoxelData()[block_index + GameWorld::Voxels::Chunk::g_size].id;
				else
					CF_block_dirs[2] = 0;

				if (y - 1 >= 0)
					CF_block_dirs[3] = chunk.getVoxelData()[block_index - GameWorld::Voxels::Chunk::g_size].id;
				else
					CF_block_dirs[3] = 0;


				if (x + 1 < 32)
					CF_block_dirs[4] = chunk.getVoxelData()[block_index + 1].id;
				else
					CF_block_dirs[4] = 0;

				if (x - 1 >= 0)
					CF_block_dirs[5] = chunk.getVoxelData()[block_index - 1].id;
				else
					CF_block_dirs[5] = 0;


				vec3f translation{ static_cast<float>(x + chunk.getPos().x), static_cast<float>(y + chunk.getPos().y), static_cast<float>(z + chunk.getPos().z)};


				const auto& uvs{ type_manager.getType(current_block.id).uvs };

																																
				if (!CF_block_dirs[0])
				{
					meshes.push_back({ Voxel::faces[0 ] + translation, uvs[0][0] });
					meshes.push_back({ Voxel::faces[1 ] + translation, uvs[0][1] });
					meshes.push_back({ Voxel::faces[2 ] + translation, uvs[0][2] });
					meshes.push_back({ Voxel::faces[3 ] + translation, uvs[0][1] });
					meshes.push_back({ Voxel::faces[4 ] + translation, uvs[0][3] });
					meshes.push_back({ Voxel::faces[5 ] + translation, uvs[0][2] });
				}																					 					   
																									 					   
				if (!CF_block_dirs[1])
				{																					 					   
					meshes.push_back({ Voxel::faces[6 ] + translation, uvs[1][0] });
					meshes.push_back({ Voxel::faces[7 ] + translation, uvs[1][1] });
					meshes.push_back({ Voxel::faces[8 ] + translation, uvs[1][2] });
					meshes.push_back({ Voxel::faces[9 ] + translation, uvs[1][1] });
					meshes.push_back({ Voxel::faces[10] + translation, uvs[1][3] });
					meshes.push_back({ Voxel::faces[11] + translation, uvs[1][2] });
				}																					 					   
																									 					   
				if (!CF_block_dirs[2])
				{																					 					   
					meshes.push_back({ Voxel::faces[12] + translation, uvs[2][0] });
					meshes.push_back({ Voxel::faces[13] + translation, uvs[2][1] });
					meshes.push_back({ Voxel::faces[14] + translation, uvs[2][2] });
					meshes.push_back({ Voxel::faces[15] + translation, uvs[2][1] });
					meshes.push_back({ Voxel::faces[16] + translation, uvs[2][3] });
					meshes.push_back({ Voxel::faces[17] + translation, uvs[2][2] });
				}																					 					   
																									 					   
				if (!CF_block_dirs[3])
				{																					 					   
					meshes.push_back({ Voxel::faces[18] + translation, uvs[3][0] });
					meshes.push_back({ Voxel::faces[19] + translation, uvs[3][1] });
					meshes.push_back({ Voxel::faces[20] + translation, uvs[3][2] });
					meshes.push_back({ Voxel::faces[21] + translation, uvs[3][1] });
					meshes.push_back({ Voxel::faces[22] + translation, uvs[3][3] });
					meshes.push_back({ Voxel::faces[23] + translation, uvs[3][2] });
				}																					 					   
																									 					   
				if (!CF_block_dirs[4])													 					   
				{																					 					   
					meshes.push_back({ Voxel::faces[24] + translation, uvs[4][0] });
					meshes.push_back({ Voxel::faces[25] + translation, uvs[4][1] });
					meshes.push_back({ Voxel::faces[26] + translation, uvs[4][2] });
					meshes.push_back({ Voxel::faces[27] + translation, uvs[4][1] });
					meshes.push_back({ Voxel::faces[28] + translation, uvs[4][3] });
					meshes.push_back({ Voxel::faces[29] + translation, uvs[4][2] });
				}																					 					   
																									 					   
				if (!CF_block_dirs[5])													 					   
				{																					 					   
					meshes.push_back({ Voxel::faces[30] + translation, uvs[5][0] });
					meshes.push_back({ Voxel::faces[31] + translation, uvs[5][1] });
					meshes.push_back({ Voxel::faces[32] + translation, uvs[5][2] });
					meshes.push_back({ Voxel::faces[33] + translation, uvs[5][1] });
					meshes.push_back({ Voxel::faces[34] + translation, uvs[5][3] });
					meshes.push_back({ Voxel::faces[35] + translation, uvs[5][2] });
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

	glBufferData(GL_ARRAY_BUFFER, meshes.size() * sizeof(Vertex), meshes.data(), GL_DYNAMIC_DRAW);
	m_nbVertices = meshes.size();
}

void Render::Data::ChunkMesh::destroy() const noexcept
{
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
}
