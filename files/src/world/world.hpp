#pragma once // world.hpp
// MIT
// Allosker ------------------------------
// =========+
// This class is responsible for everything that the game has to offer in term of chunks/entities/players
// ---------------------------------------

#include "world/voxels/chunkGrid.hpp"

namespace World
{

	class World
	{
	public:



		void update(const types::pos& camLoc);


		Voxels::ChunkGrid grid{};
	};


}