#pragma once // type.hpp
// MIT
// Allosker ------------------------------
// =========+
// Small structure for cube type, types are definitions of blocks. So, a block in its integrity is defined by the type corresponding to its ID. 
// Note: blocks may share similar IDs.
// ---------------------------------------

#include "uHeaders/opengl.hpp"
#include "uHeaders/types.hpp"

#include "rendering/assets_managing/utilities.hpp"

namespace Render::GUI
{

	namespace ItemTextureUvsAtlas
	{
		// Each vector represents the position of the texture of a face under the format: 32x32
		// front, back / up, down / left, right

		constexpr types::uvs c_dirtGrass{ 0, 3 };
				 
		constexpr types::uvs c_dirt{ 1, 3 };
				
		constexpr types::uvs c_stone{ 2, 3 };
				
		constexpr types::uvs c_deepStone{ 1, 0 };
				  
		constexpr types::uvs c_water{ 4, 4 };
				
		constexpr types::uvs c_log{ 0, 0 };
				 
		constexpr types::uvs c_leaf{ 3, 3 };

	}

	struct ItemType
	{
		ItemType(const types::atlas_units& atlas_pos)
			: pos_in_atlas{ atlas_pos }
		{ }


		types::atlas_units pos_in_atlas{};


	};


} // Render::Types