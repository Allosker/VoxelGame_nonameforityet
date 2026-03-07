#include "rendering/world_managing/data/chunk/chunkMesh.hpp"

// =====================
// Construction/Destruction
// =====================

Render::Data::ChunkMesh::ChunkMesh(GameWorld::Voxels::Chunk& chunk, const types::pos& camPos, const Render::Data::Types::VoxelTypeManager& type_manager) noexcept
{
	glGenVertexArrays(1, &m_vao);
	glCreateBuffers(1, &m_vbo);
	
	glGenVertexArrays(1, &m_vaoTransparent);
	glCreateBuffers(1, &m_vboTransparent);

	buildMesh(chunk, camPos, type_manager);
	updateBuffers();

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
	other.m_vbo = 0; // TODO: UPDATE THIS

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
	glBindVertexArray(0);
}

void Render::Data::ChunkMesh::buildMesh(const GameWorld::Voxels::Chunk& chunk, const types::pos& camPos, const Render::Data::Types::VoxelTypeManager& type_manager) noexcept
{
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

				static const auto push_mesh = [](size_t block_face, auto& mesh, const auto& translation, const auto& uvs)
				{
					const auto index{ block_face * 6 };

					mesh.push_back({ Voxel::faces[index + 0] + translation, uvs[block_face][0] });
					mesh.push_back({ Voxel::faces[index + 1] + translation, uvs[block_face][1] });
					mesh.push_back({ Voxel::faces[index + 2] + translation, uvs[block_face][2] });
					mesh.push_back({ Voxel::faces[index + 3] + translation, uvs[block_face][1] });
					mesh.push_back({ Voxel::faces[index + 4] + translation, uvs[block_face][3] });
					mesh.push_back({ Voxel::faces[index + 5] + translation, uvs[block_face][2] });
				};
				static const auto emplace_m_transparent_mesh = [](size_t block_face, auto& m_transparent_mesh, const auto& translation, const auto& uvs)
				{
					const auto index{ block_face * 6 };

					m_transparent_mesh.emplace(std::pair<vec3f, Vertex>{ translation, { Voxel::faces[index + 0] + translation, uvs[block_face][0] } });
					m_transparent_mesh.emplace(std::pair<vec3f, Vertex>{ translation, { Voxel::faces[index + 1] + translation, uvs[block_face][1] } });
					m_transparent_mesh.emplace(std::pair<vec3f, Vertex>{ translation, { Voxel::faces[index + 2] + translation, uvs[block_face][2] } });
					m_transparent_mesh.emplace(std::pair<vec3f, Vertex>{ translation, { Voxel::faces[index + 3] + translation, uvs[block_face][1] } });
					m_transparent_mesh.emplace(std::pair<vec3f, Vertex>{ translation, { Voxel::faces[index + 4] + translation, uvs[block_face][3] } });
					m_transparent_mesh.emplace(std::pair<vec3f, Vertex>{ translation, { Voxel::faces[index + 5] + translation, uvs[block_face][2] } });
				};
																																
				if (!CF_block_dirs[0] || (type_manager.getType(CF_block_dirs[0]).is_transparent && !type_manager.getType(current_block.id).is_transparent))
				{	
					if (type_manager.getType(current_block.id).is_transparent)
						emplace_m_transparent_mesh(0, m_transparent_mesh, translation, uvs);
					else
						push_mesh(0, m_mesh, translation, uvs);
				}																					 					   
																									 					   
				if (!CF_block_dirs[1] || (type_manager.getType(CF_block_dirs[1]).is_transparent && !type_manager.getType(current_block.id).is_transparent))
				{																					 					   
					push_mesh(1, m_mesh, translation, uvs);
				}																					 					   
																									 					   
				if (!CF_block_dirs[2] || (type_manager.getType(CF_block_dirs[2]).is_transparent && !type_manager.getType(current_block.id).is_transparent))
				{																					 					   
					push_mesh(2, m_mesh, translation, uvs);
				}																					 					   
																									 					   
				if (!CF_block_dirs[3] || (type_manager.getType(CF_block_dirs[3]).is_transparent && !type_manager.getType(current_block.id).is_transparent))
				{																					 					   
					push_mesh(3, m_mesh, translation, uvs);
				}																					 					   
																									 					   
				if (!CF_block_dirs[4] || (type_manager.getType(CF_block_dirs[4]).is_transparent && !type_manager.getType(current_block.id).is_transparent))
				{																					 					   
					push_mesh(4, m_mesh, translation, uvs);
				}																					 					   
																									 					   
				if (!CF_block_dirs[5] || (type_manager.getType(CF_block_dirs[5]).is_transparent && !type_manager.getType(current_block.id).is_transparent))
				{																					 					   
					push_mesh(5, m_mesh, translation, uvs);
				}

			}
		}

	}

}

void Render::Data::ChunkMesh::updateMeshBuffer() noexcept
{
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, m_mesh.size() * sizeof(Vertex), m_mesh.data(), GL_DYNAMIC_DRAW);

	m_nbVertices = m_mesh.size();
	std::vector<Vertex>{}.swap(m_mesh);
}

void Render::Data::ChunkMesh::updateTransparentMeshBuffer() noexcept
{
	glBindVertexArray(m_vaoTransparent);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboTransparent);

	std::vector<Vertex> sortedData;

	for (auto& i : m_transparent_mesh)
	{

	}

	glBufferData(GL_ARRAY_BUFFER, m_transparent_mesh.size() * sizeof(Vertex), m_transparent_mesh.data(), GL_DYNAMIC_DRAW)
}

void Render::Data::ChunkMesh::updateBuffers() noexcept
{
	updateMeshBuffer();
}

void Render::Data::ChunkMesh::destroy() const noexcept
{
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
}
