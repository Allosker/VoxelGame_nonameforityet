#pragma once
// MIT
// Allosker ------------------------------
// =========+
// This class operates as the manager for all types, blocks present their IDs and the type manager fetches information for them.
// ---------------------------------------

#include "world/types/voxelType.hpp"

#include <vector>

#include "data/voxel.hpp"

namespace Render::Types
{
	class VoxelTypeManager
	{
	public:

		VoxelTypeManager()
		{
			types.push_back(TextureUVperFace::c_dirtGrass);
			types.push_back(TextureUVperFace::c_dirt);
			types.push_back({ TextureUVperFace::c_stone, false, true, true, {0, 0, 15} });
			types.push_back({ TextureUVperFace::c_deepStone, false, true, true, {15, 0, 0} });
			types.push_back({ TextureUVperFace::c_water1, true, false, false, {}, { 0, 0, {0, -0.1, 0}, 0, 0, 0 } });
			types.push_back(TextureUVperFace::c_tree);
			types.push_back({ TextureUVperFace::c_leaf });
		}


		const VoxelType& getType(types::type_id id) const
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

			return getType(id).is_lightSource;
		}


		std::vector<VoxelType> types{};
	};


}