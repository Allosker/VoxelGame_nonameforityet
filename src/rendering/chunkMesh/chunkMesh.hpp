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

#include "utilities/opengl.hpp"
#include "data/voxel.hpp"
#include "data/vertex.hpp"

#include "utilities/resourceManaging.hpp"

#include "world/chunks/chunk.hpp"


class World;

namespace render
{
	class ChunkMesh
	{
	private:

		struct VoxelVertex
		{
			vec3f pos;
			vec2f uv;
			float shadow{ 1.f };
			uint16 light{};
		};

		using Mesh = std::vector<VoxelVertex>;
		using TMesh = std::vector<std::pair<vec3f, std::array<VoxelVertex, 6>>>;
		using MeshPair = std::pair<Mesh, TMesh>;

		

	public:

	// = Construction/Destruction


		explicit ChunkMesh() noexcept;
		 
		ChunkMesh(ChunkMesh&& other) noexcept;
		ChunkMesh& operator=(ChunkMesh&& other) noexcept;

		ChunkMesh(const ChunkMesh&) noexcept = delete;
		ChunkMesh& operator=(const ChunkMesh&) noexcept = delete;

		~ChunkMesh() noexcept;
		

	// = Actors

		void draw() const noexcept;

		MeshPair buildMesh(
			const chunks::Chunk& chunk,
			const VoxelTypeManager& type_manager,
			const World& world
		) noexcept;

		void updateMeshBuffer(Mesh&& mesh) noexcept;
		void updateTransparentMeshBuffer(TMesh&& tmesh, const types::pos& camPos) noexcept;

		void updateBuffers(MeshPair&& mp, const types::pos& camPos) noexcept;

		// Free GPU ressources
		void free() noexcept;


	// = Getters

			

	private:

		size_t m_nbVertices{};
		size_t m_nbVertices_Transparent{};

		GLuint m_vao{};
		GLuint m_vbo{};

		GLuint m_vaoTransparent{};
		GLuint m_vboTransparent{};
		

	public:

		struct
		{
			bool has_transparency{};
			bool dirty{ true };
		} flags;

	public:

		static constexpr std::uint32_t sizeVertex{ sizeof(VoxelVertex) };
	};

} // render::Data