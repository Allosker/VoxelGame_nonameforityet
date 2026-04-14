#include "rendering/world_managing/data/chunk/chunkMesh.hpp"

// =====================
// Construction/Destruction
// =====================

Render::Data::ChunkMesh::ChunkMesh() noexcept
{
	glGenVertexArrays(1, &m_vao);
	glCreateBuffers(1, &m_vbo);
	
	glGenVertexArrays(1, &m_vaoTransparent);
	glCreateBuffers(1, &m_vboTransparent);


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

	flags.dirty = other.flags.dirty;
	flags.has_transparency = other.flags.has_transparency;

	other.m_nbVertices = 0;
	other.m_vao = 0;
	other.m_vbo = 0;

	other.m_nbVertices_Transparent = 0;
	other.m_vaoTransparent = 0;
	other.m_vboTransparent = 0;

	other.flags.dirty = false;
	other.flags.has_transparency = false;
	other.flags.destroy = true;

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

void Render::Data::ChunkMesh::buildMesh(
	const GameWorld::Voxels::Chunk& chunk,
	const Render::Data::Types::VoxelTypeManager& type_manager,
	const GameWorld::Voxels::ChunkGrid& grid
) noexcept
{
	const auto z_stride{ GameWorld::Voxels::Chunk::g_size * GameWorld::Voxels::Chunk::g_size };

	for (int32 x{}; x < GameWorld::Voxels::Chunk::g_size; x++)
	for (int32 y{}; y < GameWorld::Voxels::Chunk::g_size; y++)
	for (int32 z{}; z < GameWorld::Voxels::Chunk::g_size; z++)
	{
		int32 block_index{ static_cast<int32>((z * z_stride) + (y * GameWorld::Voxels::Chunk::g_size) + x) };

		auto& current_block{ chunk.getVoxelData()[block_index] };

		if (!current_block.id)
			continue;


		std::array<size_t, 6> CF_block_dirs{};


		if (z + 1 < GameWorld::Voxels::Chunk::g_size)
			CF_block_dirs[0] = chunk.getVoxelData()[block_index + z_stride].id;
		else 
			if (const auto* c{grid.chunk_at(static_cast<types::pos>(vec3i{x, y, z + 1} + static_cast<vec3i>(chunk.getPos())))})
			CF_block_dirs[0] = c->block_at(static_cast<types::loc>(vec3i{ x, y, 0 })).id;

		if (z - 1 >= 0)
			CF_block_dirs[1] = chunk.getVoxelData()[block_index - z_stride].id;
		else
			if (const auto* c{ grid.chunk_at(static_cast<types::pos>(vec3i{x, y, z - 1} + static_cast<vec3i>(chunk.getPos()))) })
				CF_block_dirs[1] = c->block_at(types::loc{ x, y, GameWorld::Voxels::Chunk::g_size - 1 }).id;


		if (y + 1 < GameWorld::Voxels::Chunk::g_size)
			CF_block_dirs[2] = chunk.getVoxelData()[block_index + GameWorld::Voxels::Chunk::g_size].id;
		else
			if (const auto* c{ grid.chunk_at(static_cast<types::pos>(vec3i{x, y + 1, z} + static_cast<vec3i>(chunk.getPos()))) })
				CF_block_dirs[2] = c->block_at(types::loc{ x, 0, z }).id;

		if (y - 1 >= 0)
			CF_block_dirs[3] = chunk.getVoxelData()[block_index - GameWorld::Voxels::Chunk::g_size].id;
		else
			if (const auto* c{ grid.chunk_at(static_cast<types::pos>(vec3i{x, y - 1, z} + static_cast<vec3i>(chunk.getPos()))) })
				CF_block_dirs[3] = c->block_at(types::loc{ x, GameWorld::Voxels::Chunk::g_size - 1, z }).id;


		if (x + 1 < GameWorld::Voxels::Chunk::g_size)
			CF_block_dirs[4] = chunk.getVoxelData()[block_index + 1].id;
		else
			if (const auto* c{ grid.chunk_at(static_cast<types::pos>(vec3i{x + 1, y, z} + static_cast<vec3i>(chunk.getPos()))) })
				CF_block_dirs[4] = c->block_at(types::loc{ 0, y, z }).id;

		if (x - 1 >= 0)
			CF_block_dirs[5] = chunk.getVoxelData()[block_index - 1].id;
		else
			if (const auto* c{ grid.chunk_at(static_cast<types::pos>(vec3i{x - 1, y, z} + static_cast<vec3i>(chunk.getPos()))) })
				CF_block_dirs[5] = c->block_at(types::loc{ GameWorld::Voxels::Chunk::g_size - 1, y, z }).id;



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
	flags.has_transparency = !m_transparent_mesh.empty();

	if (!flags.has_transparency)
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
