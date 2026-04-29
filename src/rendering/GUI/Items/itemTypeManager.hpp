#pragma once // 
// Allosker ------------------------------
// =========+
// Fetches the corresponding attributes for each Item ID
// ---------------------------------------

#include "rendering/GUI/Items/itemType.hpp"

#include "rendering/assets_managing/utilities.hpp"

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

		static constexpr vec2f g_texture_size_item{ 33.f };
		static constexpr vec2f g_size_gui_block{ 66.f };
	};

	inline types::Rect<types::atlas_units> toAtlasUnits(types::type_id id, const ItemTypeManager& manager) noexcept
	{
		return { manager.getType(id).pos_in_atlas, ItemTypeManager::g_texture_size_item };
	}

	inline types::Rect<types::atlas_units> toPixelUnits(types::type_id id, const ItemTypeManager& manager) noexcept
	{
		vec2f pos_atlas{ manager.getType(id).pos_in_atlas };

		return { 
			{ pos_atlas.x * ItemTypeManager::g_texture_size_item.x, pos_atlas.y * ItemTypeManager::g_texture_size_item.y},
			ItemTypeManager::g_texture_size_item
		};
	}

	inline types::Rect<types::uvs> mapTextureUvs(types::type_id id, const ItemTypeManager& manager) noexcept
	{
		return fromAtlasToPixels(toAtlasUnits(id, manager));
	}

}