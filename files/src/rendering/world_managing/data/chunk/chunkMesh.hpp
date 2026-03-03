#pragma once // chunk.hpp
// MIT
// Allosker ------------------------------
// =========+
// Implement a naive chunk class, with basic meshing.
// is ready to be implemented within a greater world.
// ---------------------------------------

#include <cstddef>
#include <bit>

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

		explicit ChunkMesh(GameWorld::Voxels::Chunk& chunk, const Render::Data::Types::VoxelTypeManager& type_manager) noexcept;

		ChunkMesh(ChunkMesh&& other) noexcept;
		ChunkMesh& operator=(ChunkMesh&& other) noexcept;

		ChunkMesh(const ChunkMesh&) noexcept = delete;
		ChunkMesh& operator=(const ChunkMesh&) noexcept = delete;

		~ChunkMesh() noexcept;
		

	// = Actors

		void draw() const noexcept;

		std::vector<Vertex> buildMesh(const GameWorld::Voxels::Chunk& chunk, const Render::Data::Types::VoxelTypeManager& type_manager) noexcept;

		void updateBuffer(const std::vector<Vertex>& meshes) noexcept;

		void destroy() const noexcept;


	// = Getters

			

	private:

		std::uint32_t m_nbVertices{};

		GLuint m_vao{};
		GLuint m_vbo{};
		//GLuint m_ebo{};

	public:

		static constexpr std::uint32_t sizeVertex{ sizeof(Vertex) };
	};

} // Render::Data