#pragma once // chunk.hpp
// MIT
// Allosker ------------------------------
// =========+
// Basic chunk class responsible for everything related to gameplay that happens within it.
// ---------------------------------------

#include "rendering/world_managing/data/basic/cube.hpp"

namespace Gameplay::World
{

	class Chunk
	{
	public: 



		const std::array<Render::Data::Cube, 32'768>& getCubeData() const noexcept
		{
			return m_cubes;
		}

	private:




		std::array<Render::Data::Cube, 32'768> m_cubes;

	public:

		static constexpr std::uint8_t g_size{ 32 };
	};

}