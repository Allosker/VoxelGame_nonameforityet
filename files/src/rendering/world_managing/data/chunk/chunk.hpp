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
#include "rendering/world_managing/data/basic/cube.hpp"
#include "rendering/world_managing/data/basic/vertex.hpp"

namespace Render
{

	namespace Data
	{
		class Chunk
		{
		public:

		// = Construction/Destruction

			Chunk(const vec3f& pos) noexcept;

			~Chunk() noexcept;

		// = Actors

			void draw() const noexcept;

			void generateChunkMesh() noexcept;


		// = Getters

			// debug
			size_t getSize()
			{
				return m_cubes_chunk.size();
			}
			

		private:

			std::vector<Cube::Filling> m_cubes_chunk{};

			vec3f m_pos{};

			std::uint32_t m_nbVertices{};

			GLuint m_vao{};
			GLuint m_vbo{};
			//GLuint m_ebo{};


			static constexpr std::uint32_t g_dimensions{ 32 };
		};

	} // data


} // render