#pragma once // chunk.hpp
// MIT
// Allosker ------------------------------
// =========+
// Implement a naive chunk class, with basic meshing.
// is ready to be implemented within a greater world.
// ---------------------------------------

#include <cstddef>
#include <bit>
#include <map>

#include "uHeaders/opengl.hpp"
#include "rendering/world_managing/data/basic/voxel.hpp"
#include "rendering/world_managing/data/basic/vertex.hpp"

#include "rendering/world_managing/data/TypeManagement/voxelTypeManager.hpp"

#include "world/voxels/chunk.hpp"


namespace Render::Data
{
	class ChunkMesh
	{
	public:

	// = Construction/Destruction

		ChunkMesh() = delete;

		explicit ChunkMesh(GameWorld::Voxels::Chunk& chunk, const types::pos& camPos, const Render::Data::Types::VoxelTypeManager& type_manager) noexcept;

		ChunkMesh(ChunkMesh&& other) noexcept;
		ChunkMesh& operator=(ChunkMesh&& other) noexcept;

		ChunkMesh(const ChunkMesh&) noexcept = delete;
		ChunkMesh& operator=(const ChunkMesh&) noexcept = delete;

		~ChunkMesh() noexcept;
		

	// = Actors

		void draw() const noexcept;

		void buildMesh(const GameWorld::Voxels::Chunk& chunk, const Render::Data::Types::VoxelTypeManager& type_manager) noexcept;

		void updateMeshBuffer() noexcept;
		void updateTransparentMeshBuffer(const types::pos& camPos) noexcept;

		void updateBuffers(const types::pos& camPos) noexcept;

		void destroy() const noexcept;


	// = Getters

			

	private:

		size_t m_nbVertices{};
		size_t m_nbVertices_Transparent{};

		std::vector< std::pair<vec3f, std::array<Vertex, 6>>> m_transparent_mesh{};
		std::vector<Vertex> m_mesh{};

		GLuint m_vao{};
		GLuint m_vbo{};

		GLuint m_vaoTransparent{};
		GLuint m_vboTransparent{};
		

	public:

		bool has_transparency{};

	public:

		static constexpr std::uint32_t sizeVertex{ sizeof(Vertex) };
	};

} // Render::Data