#pragma once // resourceManager.hpp
// MIT
// Allosker ------------------------------
// =========+
// Manages all Shaders and Textures that live on the GPU.
// ---------------------------------------

#include "rendering/shader.hpp"
#include "rendering/texture.hpp"

#include "rendering/text/font.hpp"

#include "world/types/itemType.hpp"
#include "rendering/utilities/atlas.hpp"

#include "world/types/voxelType.hpp"
#include "data/voxel.hpp"

#include <vector>



struct Resources
{
public:

	// Shaders

	render::Shader	s_world{ SHADER_PATH"shader.vert", SHADER_PATH"shader.frag" };
	render::Shader	s_items_world{ SHADER_PATH"shader3Ditem.vert", SHADER_PATH"shader3Ditem.frag" };

	render::Shader	s_gui{ SHADER_PATH"meshTexture2D.vert", SHADER_PATH"meshTexture2D.frag" };
	render::Shader	s_gui_text{ SHADER_PATH"text_render/text2D.vert", SHADER_PATH"text_render/text2D.frag" };


	// Textures

	render::Texture atlas_voxels{ ASSET_PATH"blocks/world/atlas.png" };

	render::Image	atlas_im_guiblocks{ ASSET_PATH"blocks/gui/block_inventory_atlas.png" };
	render::Texture atlas_guiblocks{ atlas_im_guiblocks };

	render::Texture t_crossair{ ASSET_PATH"hud/crossair_atlas.png" };

	render::Texture			t_hotbarSlot{ ASSET_PATH"hud/slot.png" };
	render::Texture			t_inventory_small{ ASSET_PATH"inventory/small.png" };
	render::Texture			t_inventory_medium{ ASSET_PATH"inventory/medium.png" };
	render::Texture			t_inventory_big{ ASSET_PATH"inventory/big.png" };


	// Fonts

	render::Font						f_pixelated{ FONT_PATH"pixelated.ttf" };



	static Resources& get()
	{
		static Resources res;
		return res;
	};

private:

	Resources() = default;

};

// Enables the user to fetch the corresponding data for one voxel
class VoxelTypeManager
{
public:


	const types::VoxelType& getType(types::type_id id) const
	{
		if (id == 0)
			throw std::runtime_error("INDEX_OUT_OF_BOUNDS::Block type 0 doesn't exist");

		return types.at(id - 1);
	}

	const bool isTypeTransparent(const Data::Voxel* voxel) const noexcept
	{
		if (voxel == nullptr)
			return true;

		if (voxel->id == 0)
			return true;

		return getType(voxel->id).is_transparent;
	}

	const bool isTypeTransparent(types::type_id id) const noexcept
	{
		if (id == 0)
			return true;

		return getType(id).is_transparent;
	}

	const bool isTypeLightsource(types::type_id id) const noexcept
	{
		if (id == 0)
			return false;

		return getType(id).is_light_source;
	}


	static VoxelTypeManager& get()
	{
		static VoxelTypeManager vtm;
		return vtm;
	};

private:

	VoxelTypeManager()
	{
		types.push_back(types::TextureUVperFace::c_dirtGrass);
		types.push_back(types::TextureUVperFace::c_dirt);
		types.push_back(types::VoxelType{ types::TextureUVperFace::c_stone });
		types.push_back(types::VoxelType{ types::TextureUVperFace::c_deepStone });
		types.push_back(types::VoxelType{ types::TextureUVperFace::c_water1, true, false, false, {}, { 0, 0, {0, -0.1, 0}, 0, 0, 0 } });
		types.push_back(types::TextureUVperFace::c_tree);
		types.push_back({ types::TextureUVperFace::c_leaf });
	}

	std::vector<types::VoxelType> types{};
};

// Enables the user to fetch the corresponding data for one item
class ItemTypeManager
{
public:


	const types::ItemType& getType(const types::type_id id) const
	{
		if (id == 0)
			throw std::runtime_error("INDEX_OUT_OF_BOUNDS::Block type 0 doesn't exist");

		return types.at(id - 1);
	}

	const std::vector<types::ItemType>& getTypeList() const noexcept { return types; }


	static ItemTypeManager& get()
	{
		static ItemTypeManager itm;
		return itm;
	};


	static constexpr vec2f g_texture_size_item{ 33.f };
	static constexpr vec2f g_size_gui_block{ 66.f };

private:

	ItemTypeManager()
	{
		types.push_back({ types::ItemTextureUvsAtlas::c_dirtGrass	, "grass" });
		types.push_back({ types::ItemTextureUvsAtlas::c_dirt		, "dirt" });
		types.push_back({ types::ItemTextureUvsAtlas::c_stone		, "stone" });
		types.push_back({ types::ItemTextureUvsAtlas::c_deepStone	, "deep stone" });
		types.push_back({ types::ItemTextureUvsAtlas::c_water		, "water" });
		types.push_back({ types::ItemTextureUvsAtlas::c_log			, "log" });
		types.push_back({ types::ItemTextureUvsAtlas::c_leaf		, "leaf" });
	}


	std::vector<types::ItemType> types{};
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
	return render::utils::fromAtlasToPixels(toAtlasUnits(id, manager));
}