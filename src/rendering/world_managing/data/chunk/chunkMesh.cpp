#include "rendering/world_managing/data/chunk/chunkMesh.hpp"
#include "world/world.hpp"

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
			glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeVertex, std::bit_cast<void*>(offsetof(VoxelVertex, pos)));
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeVertex, std::bit_cast<void*>(offsetof(VoxelVertex, uv)));
			glEnableVertexAttribArray(1); 

			glVertexAttribPointer(2, 1, GL_FLOAT, false, sizeVertex, std::bit_cast<void*>(offsetof(VoxelVertex, shadow)));
			glEnableVertexAttribArray(2);

			// For integers
			glVertexAttribIPointer(3, 1, GL_BYTE, sizeVertex, std::bit_cast<void*>(offsetof(VoxelVertex, blocklight)));
			glEnableVertexAttribArray(3);

			// For integers
			glVertexAttribIPointer(4, 1, GL_BYTE, sizeVertex, std::bit_cast<void*>(offsetof(VoxelVertex, sunlight)));
			glEnableVertexAttribArray(4);
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
	: 
	m_nbVertices				{ other.m_nbVertices },
	m_nbVertices_Transparent	{ other.m_nbVertices_Transparent },
	m_vao						{ other.m_vao },
	m_vbo						{ other.m_vbo },
	m_vaoTransparent			{ other.m_vaoTransparent },
	m_vboTransparent			{ other.m_vboTransparent },
	flags						{ other.flags.has_transparency, other.flags.dirty }
{
	other.m_nbVertices				= 0;
	other.m_nbVertices_Transparent	= 0;
	other.m_vao						= 0;
	other.m_vbo						= 0;
	other.m_vaoTransparent			= 0;
	other.m_vboTransparent			= 0;
	other.flags.has_transparency	= 0;
	other.flags.dirty				= 0;
}

Render::Data::ChunkMesh& Render::Data::ChunkMesh::operator=(ChunkMesh&& other) noexcept
{
	if (this == &other)
		return *this;

	m_nbVertices				=	other.m_nbVertices;
	m_nbVertices_Transparent	=	other.m_nbVertices_Transparent;
	m_vao						=	other.m_vao;
	m_vbo						=	other.m_vbo;
	m_vaoTransparent			=	other.m_vaoTransparent;
	m_vboTransparent			=	other.m_vboTransparent;
	flags.has_transparency		=	other.flags.has_transparency;
	flags.dirty					=	other.flags.dirty;

	other.m_nbVertices = 0;
	other.m_nbVertices_Transparent = 0;
	other.m_vao = 0;
	other.m_vbo = 0;
	other.m_vaoTransparent = 0;
	other.m_vboTransparent = 0;
	other.flags.has_transparency = 0;
	other.flags.dirty = 0;

	return *this;
}	

Render::Data::ChunkMesh::~ChunkMesh() noexcept
{
	free();
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

Render::Data::ChunkMesh::MeshPair Render::Data::ChunkMesh::buildMesh(
	const GameWorld::Voxels::Chunk& chunk,
	const Render::Data::Types::VoxelTypeManager& type_manager,
	const GameWorld::World& world
) noexcept
{
	static constexpr auto z_stride{ GameWorld::Voxels::Chunk::g_size * GameWorld::Voxels::Chunk::g_size };

	MeshPair mp{};

	for (int32 x{}; x < GameWorld::Voxels::Chunk::g_size; x++)
	for (int32 y{}; y < GameWorld::Voxels::Chunk::g_size; y++)
	for (int32 z{}; z < GameWorld::Voxels::Chunk::g_size; z++)
	{
		const int32 block_index{ static_cast<int32>((z * z_stride) + (y * GameWorld::Voxels::Chunk::g_size) + x) };

		auto& current_block{ chunk.block_at(block_index) };

		if (!current_block.id)
			continue;


		std::array<size_t, 6> CF_block_dirs{};
		const std::array<types::loc, 6> dirs
		{
			types::loc
			{x, y, z + 1},
			{x, y, z - 1},
			
			{x, y + 1, z},
			{x, y - 1, z},

			{x + 1, y, z},
			{x - 1, y, z},
			
		};
		

		// Voxel Face Visibility
		if (z + 1 < GameWorld::Voxels::Chunk::g_size)
			CF_block_dirs[0] = chunk.block_at(block_index + z_stride).id;
		else
			if (const auto* c{ world.chunk_at(static_cast<types::pos>(dirs[0]) + chunk.getPos())})
				CF_block_dirs[0] = c->block_id_at(static_cast<types::loc>(vec3i{ x, y, 0 }));

		if (z - 1 >= 0)
			CF_block_dirs[1] = chunk.block_at(block_index - z_stride).id;
		else
			if (const auto* c{ world.chunk_at(static_cast<types::pos>(dirs[1]) + chunk.getPos()) })
				CF_block_dirs[1] = c->block_id_at(types::loc{ x, y, GameWorld::Voxels::Chunk::g_size - 1 });


		if (y + 1 < GameWorld::Voxels::Chunk::g_size)
			CF_block_dirs[2] = chunk.block_at(block_index + GameWorld::Voxels::Chunk::g_size).id;
		else
			if (const auto* c{ world.chunk_at(static_cast<types::pos>(dirs[2]) + chunk.getPos()) })
				CF_block_dirs[2] = c->block_id_at(types::loc{ x, 0, z });

		if (y - 1 >= 0)
			CF_block_dirs[3] = chunk.block_at(block_index - GameWorld::Voxels::Chunk::g_size).id;
		else
			if (const auto* c{ world.chunk_at(static_cast<types::pos>(dirs[3]) + chunk.getPos()) })
				CF_block_dirs[3] = c->block_id_at(types::loc{ x, GameWorld::Voxels::Chunk::g_size - 1, z });


		if (x + 1 < GameWorld::Voxels::Chunk::g_size)
			CF_block_dirs[4] = chunk.block_at(block_index + 1).id;
		else
			if (const auto* c{ world.chunk_at(static_cast<types::pos>(dirs[4]) + chunk.getPos()) })
				CF_block_dirs[4] = c->block_id_at(types::loc{ 0, y, z });

		if (x - 1 >= 0)
			CF_block_dirs[5] = chunk.block_at(block_index - 1).id;
		else
			if (const auto* c{ world.chunk_at(static_cast<types::pos>(dirs[5]) + chunk.getPos()) })
				CF_block_dirs[5] = c->block_id_at(types::loc{ GameWorld::Voxels::Chunk::g_size - 1, y, z });



		vec3f translation{ static_cast<float>(x + chunk.getPos().x), static_cast<float>(y + chunk.getPos().y), static_cast<float>(z + chunk.getPos().z)};

		const auto& type{ type_manager.getType(current_block.id) };
					
		for (size_t i{}; i < 6; i++)
			if (!CF_block_dirs[i] || (type_manager.getType(CF_block_dirs[i]).is_transparent && !type.is_transparent))
			{	
				const auto index{ i * 6 };
				const auto& offset{ type.face_offsets };


				uint8 face_light{};
				uint8 face_sunlight{};

				if (const auto* v = world.blockempty_at(static_cast<types::pos>(dirs[i]) + chunk.getPos()))
				{
					face_light = v->getBlocklight();
					face_sunlight = v->getSunlight();
				}


				if (type.is_transparent)
				{
					mp.second.emplace_back( std::pair<vec3f, std::array<VoxelVertex, 6>>
					{
						translation, std::array<VoxelVertex, 6>{
						VoxelVertex{ Voxel::faces[index + 0] + translation + offset[i], type.uvs[i][0], 1.f, face_light, face_sunlight },
						VoxelVertex{ Voxel::faces[index + 1] + translation + offset[i], type.uvs[i][1], 1.f, face_light, face_sunlight },
						VoxelVertex{ Voxel::faces[index + 2] + translation + offset[i], type.uvs[i][2], 1.f, face_light, face_sunlight },
						VoxelVertex{ Voxel::faces[index + 3] + translation + offset[i], type.uvs[i][1], 1.f, face_light, face_sunlight },
						VoxelVertex{ Voxel::faces[index + 4] + translation + offset[i], type.uvs[i][3], 1.f, face_light, face_sunlight },
						VoxelVertex{ Voxel::faces[index + 5] + translation + offset[i], type.uvs[i][2], 1.f, face_light, face_sunlight }
						}
					});
				}
				else
				{
					std::array<float, 4> corners{ 1.f, 1.f, 1.f, 1.f };

					const std::array<std::array<types::loc, 8>, 6> dirs
					{
						std::array<types::loc, 8>
						{
							types::loc
							{ x	   ,	y - 1,	  z + 1 },
							{ x	   ,	y + 1,	  z + 1 },
							{ x - 1,	y    ,	  z + 1 },
							{ x + 1,	y    ,	  z + 1 },
							
							{ x - 1,	y - 1,	  z + 1 },
							{ x - 1,	y + 1,	  z + 1 },
							{ x + 1,	y - 1,	  z + 1 },
							{ x + 1,	y + 1,	  z + 1 },
						},
						{
							types::loc
							{ x	   ,	y - 1,	  z - 1 },
							{ x	   ,	y + 1,	  z - 1 },
							{ x + 1,	y    ,	  z - 1 },
							{ x - 1,	y    ,	  z - 1 },
													
							{ x + 1,	y - 1,	  z - 1 },
							{ x + 1,	y + 1,	  z - 1 },
							{ x - 1,	y - 1,	  z - 1 },
							{ x - 1,	y + 1,	  z - 1 },
							
						},
						{
							types::loc
							{ x + 1,	y + 1,	  z		},
							{ x - 1,	y + 1,	  z		},
							{ x	   ,	y + 1,	  z + 1 },
							{ x	   ,	y + 1,	  z - 1 },
							{ x + 1,	y + 1,	  z + 1 },
							{ x - 1,	y + 1,	  z + 1 },
							{ x + 1,	y + 1,	  z - 1 },
							{ x - 1,	y + 1,	  z - 1 }
						},
						{
							types::loc
							{ x + 1,	y - 1,	  z		},
							{ x - 1,	y - 1,	  z		},
							{ x	   ,	y - 1,	  z - 1 },
							{ x	   ,	y - 1,	  z + 1 },

							{ x + 1,	y - 1,	  z - 1 },
							{ x - 1,	y - 1,	  z - 1 },
							{ x + 1,	y - 1,	  z + 1 },
							{ x - 1,	y - 1,	  z + 1 },
						},
						{
							types::loc
							{ x + 1,	y - 1,	  z		},
							{ x + 1,	y + 1,	  z		},
							{ x	+ 1,	y    ,	  z + 1 },
							{ x	+ 1,	y    ,	  z - 1 },

							{ x + 1,	y - 1,	  z + 1 },
							{ x + 1,	y + 1,	  z + 1 },
							{ x + 1,	y - 1,	  z - 1 },
							{ x + 1,	y + 1,	  z - 1 },
							
						},
						{
							types::loc
							{ x - 1,	y - 1,	  z		},
							{ x - 1,	y + 1,	  z		},
							{ x - 1,	y    ,	  z - 1 },
							{ x - 1,	y    ,	  z + 1 },
							

							{ x - 1,	y - 1,	  z - 1 },
							{ x - 1,	y + 1,	  z - 1 },
							{ x - 1,	y - 1,	  z + 1 },
							{ x - 1,	y + 1,	  z + 1 },
						}
						
					};
					static constexpr std::array<float, 4> s_values{ 0.5f, 0.7f, 0.8f, 1.f };


					bool right	= world.getGrid().block_id_at(static_cast<types::pos>(dirs[i][0]) + chunk.getPos());
					bool left	= world.getGrid().block_id_at(static_cast<types::pos>(dirs[i][1]) + chunk.getPos());
					bool front	= world.getGrid().block_id_at(static_cast<types::pos>(dirs[i][2]) + chunk.getPos());
					bool back	= world.getGrid().block_id_at(static_cast<types::pos>(dirs[i][3]) + chunk.getPos());
					bool rf		= world.getGrid().block_id_at(static_cast<types::pos>(dirs[i][4]) + chunk.getPos());
					bool lf		= world.getGrid().block_id_at(static_cast<types::pos>(dirs[i][5]) + chunk.getPos());
					bool rb		= world.getGrid().block_id_at(static_cast<types::pos>(dirs[i][6]) + chunk.getPos());
					bool lb		= world.getGrid().block_id_at(static_cast<types::pos>(dirs[i][7]) + chunk.getPos());
					//auto sum	= right + left + front + back + rf + lf + rb + lb;


					if (front && left)
						corners[2] = s_values[0];
					else if (left || right || front || back || rf || lf || rb || lb)
						corners[2] = s_values[3 - (front + left + lf)];

					if (front && right)
						corners[0] = s_values[0];
					else if (left || right || front || back || rf || lf || rb || lb)
						corners[0] = s_values[3 - (front + right + rf)];

					if (back && left)
						corners[3] = s_values[0];
					else if (left || right || front || back || rf || lf || rb || lb)
						corners[3] = s_values[3 - (back + left + lb)];

					if (back && right)
						corners[1] = s_values[0];
					else if (left || right || front || back || rf || lf || rb || lb)
						corners[1] = s_values[3 - (back + right + rb)];


					mp.first.push_back({ Voxel::faces[index + 0] + translation + offset[i], type.uvs[i][0], corners[0], face_light, face_sunlight });
					mp.first.push_back({ Voxel::faces[index + 1] + translation + offset[i], type.uvs[i][1], corners[1], face_light, face_sunlight });
					mp.first.push_back({ Voxel::faces[index + 2] + translation + offset[i], type.uvs[i][2], corners[2], face_light, face_sunlight });
					mp.first.push_back({ Voxel::faces[index + 3] + translation + offset[i], type.uvs[i][1], corners[1], face_light, face_sunlight });
					mp.first.push_back({ Voxel::faces[index + 4] + translation + offset[i], type.uvs[i][3], corners[3], face_light, face_sunlight });
					mp.first.push_back({ Voxel::faces[index + 5] + translation + offset[i], type.uvs[i][2], corners[2], face_light, face_sunlight });
				}
			}																					 					   

	}

	return mp;
}

void Render::Data::ChunkMesh::updateMeshBuffer(Mesh&& mesh) noexcept
{
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeVertex, mesh.data(), GL_DYNAMIC_DRAW);

	m_nbVertices = mesh.size();

	glBindVertexArray(0);
}

void Render::Data::ChunkMesh::updateTransparentMeshBuffer(TMesh&& tmesh, const types::pos& camPos) noexcept
{
	flags.has_transparency = !tmesh.empty();

	if (!flags.has_transparency)
		return;


	std::vector <std::pair<types::pos, std::array<VoxelVertex, 6 >>> sorted_data/*(m_transparent_mesh.size())*/;

	for (const auto& pair : tmesh)
			sorted_data.emplace_back(pair);

	std::sort(sorted_data.begin(), sorted_data.end(), [&](const auto& a, const auto& b)
		{
			return
				vec3f{ a.first - camPos }.length_squared() >
				vec3f{ b.first - camPos }.length_squared();
		});

	std::vector<VoxelVertex> vertex_data(sorted_data.size() * 6);

	for (const auto& pair : sorted_data)
		for (const auto& i : pair.second)
			vertex_data.emplace_back(i);

	glBindVertexArray(m_vaoTransparent);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboTransparent);

	glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeVertex, vertex_data.data(), GL_DYNAMIC_DRAW);

	m_nbVertices_Transparent = vertex_data.size();

	glBindVertexArray(0);
}

void Render::Data::ChunkMesh::updateBuffers(MeshPair&& mp, const types::pos& camPos) noexcept
{
	updateMeshBuffer(std::move(mp.first));
	updateTransparentMeshBuffer(std::move(mp.second), camPos);
}

void Render::Data::ChunkMesh::free() noexcept
{
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vboTransparent);
	glDeleteVertexArrays(1, &m_vaoTransparent);

	m_vbo = 0;
	m_vao = 0;
	m_vboTransparent = 0;
	m_vaoTransparent = 0;
}
