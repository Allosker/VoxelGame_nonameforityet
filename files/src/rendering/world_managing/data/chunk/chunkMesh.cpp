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

	buildMesh(chunk, type_manager);
	updateBuffers(camPos);

	const auto setAttribs{ []() 
		{
			glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeVertex, std::bit_cast<void*>(offsetof(Vertex, pos)));
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeVertex, std::bit_cast<void*>(offsetof(Vertex, uv)));
			glEnableVertexAttribArray(1); 
		} }; 

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	setAttribs();


	glBindVertexArray(m_vaoTransparent);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboTransparent);

	setAttribs();


	glBindVertexArray(0);
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

	m_nbVertices_Transparent = other.m_nbVertices_Transparent;
	m_vaoTransparent = other.m_vaoTransparent;
	m_vboTransparent = other.m_vboTransparent;

	other.m_nbVertices = 0;
	other.m_vao = 0;
	other.m_vbo = 0;

	other.m_nbVertices_Transparent = 0;
	other.m_vaoTransparent = 0;
	other.m_vboTransparent = 0;

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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindVertexArray(m_vaoTransparent);
	glDrawArrays(GL_TRIANGLES, 0, m_nbVertices_Transparent);

	glDisable(GL_BLEND);
	glBindVertexArray(0);
}

void Render::Data::ChunkMesh::buildMesh(const GameWorld::Voxels::Chunk& chunk, const Render::Data::Types::VoxelTypeManager& type_manager) noexcept
{
	const auto z_stride{ GameWorld::Voxels::Chunk::g_size * GameWorld::Voxels::Chunk::g_size };

	for (std::int32_t x{}; x < GameWorld::Voxels::Chunk::g_size; x++)
	for (std::int32_t y{}; y < GameWorld::Voxels::Chunk::g_size; y++)
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

		const auto& type{ type_manager.getType(current_block.id) };
					

		for (size_t i{}; i < 6; i++)
			if (!CF_block_dirs[i] || (type_manager.getType(CF_block_dirs[i]).is_transparent && !type.is_transparent))
			{	
				const auto index{ i * 6 };
				const auto& offset{ type.face_offsets };

				if (type.is_transparent)
				{
					m_transparent_mesh.emplace_back( std::pair<vec3f, std::array<Vertex, 6>>
					{
						translation, std::array<Vertex, 6>{
						Vertex{ Voxel::faces[index + 0] + translation + offset[i], type.uvs[i][0] },
						Vertex{ Voxel::faces[index + 1] + translation + offset[i], type.uvs[i][1] },
						Vertex{ Voxel::faces[index + 2] + translation + offset[i], type.uvs[i][2] },
						Vertex{ Voxel::faces[index + 3] + translation + offset[i], type.uvs[i][1] },
						Vertex{ Voxel::faces[index + 4] + translation + offset[i], type.uvs[i][3] },
						Vertex{ Voxel::faces[index + 5] + translation + offset[i], type.uvs[i][2] }
					}});
				}
				else
				{
					m_mesh.push_back({ Voxel::faces[index + 0] + translation + offset[i], type.uvs[i][0] });
					m_mesh.push_back({ Voxel::faces[index + 1] + translation + offset[i], type.uvs[i][1] });
					m_mesh.push_back({ Voxel::faces[index + 2] + translation + offset[i], type.uvs[i][2] });
					m_mesh.push_back({ Voxel::faces[index + 3] + translation + offset[i], type.uvs[i][1] });
					m_mesh.push_back({ Voxel::faces[index + 4] + translation + offset[i], type.uvs[i][3] });
					m_mesh.push_back({ Voxel::faces[index + 5] + translation + offset[i], type.uvs[i][2] });
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

	glBindVertexArray(0);
}

void Render::Data::ChunkMesh::updateTransparentMeshBuffer(const types::pos& camPos) noexcept
{
	if ((has_transparency = m_transparent_mesh.empty()) != false)
		return;

	std::sort(m_transparent_mesh.begin(), m_transparent_mesh.end(), [&](const auto& a, const auto& b)
		{
			return
				vec3f{ a.first - camPos }.length_squared() >
				vec3f{ b.first - camPos }.length_squared();
		});

	std::vector<Vertex> sorted_data/*(m_transparent_mesh.size())*/;

	for (const auto& pair : m_transparent_mesh)
		for (const auto& i : pair.second)
			sorted_data.emplace_back(i);

	glBindVertexArray(m_vaoTransparent);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboTransparent);

	glBufferData(GL_ARRAY_BUFFER, sorted_data.size() * sizeof(Vertex), sorted_data.data(), GL_DYNAMIC_DRAW);

	m_nbVertices_Transparent = sorted_data.size();
	m_transparent_mesh.clear();

	glBindVertexArray(0);
}

void Render::Data::ChunkMesh::updateBuffers(const types::pos& camPos) noexcept
{
	updateMeshBuffer();
	updateTransparentMeshBuffer(camPos);
}

void Render::Data::ChunkMesh::destroy() const noexcept
{
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vboTransparent);
	glDeleteVertexArrays(1, &m_vaoTransparent);
}
