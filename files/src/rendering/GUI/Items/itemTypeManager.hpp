#pragma once // 
// Allosker ------------------------------
// =========+
// Fetches the corresponding attributes for each Item ID
// ---------------------------------------

#include "rendering/GUI/Items/itemType.hpp"

#include <vector>

namespace Render::GUI
{
	class ItemTypeManager
	{
	public:

		// Order msut be same as Voxels
		ItemTypeManager()
		{
			types.push_back(ItemTextureUvsAtlas::c_dirtGrass	);
			types.push_back(ItemTextureUvsAtlas::c_dirt			);
			types.push_back(ItemTextureUvsAtlas::c_stone		);
			types.push_back(ItemTextureUvsAtlas::c_deepStone	);
			types.push_back(ItemTextureUvsAtlas::c_water		);
			types.push_back(ItemTextureUvsAtlas::c_log			);
			types.push_back(ItemTextureUvsAtlas::c_leaf			);
		}


		const ItemType& getType(const types::type_id id) const
		{
			if (id == 0)
				throw std::runtime_error("INDEX_OUT_OF_BOUNDS::Block type 0 doesn't exist");

			return types.at(id - 1);
		}


		std::vector<ItemType> types{};
	};


}