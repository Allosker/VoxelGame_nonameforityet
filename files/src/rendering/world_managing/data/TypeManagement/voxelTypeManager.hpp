#pragma once
// MIT
// Allosker ------------------------------
// =========+
// This class operates as the manager for all types, blocks present their IDs and the type manager fetches information for them.
// ---------------------------------------

#include "rendering/world_managing/data/typeManagement/voxelType.hpp"

#include <vector>

namespace Render::Data::Types
{
	class VoxelTypeManager
	{
	public:

		VoxelTypeManager()
		{
			types.push_back(TextureUVperFace::c_dirtGrass);
			types.push_back(TextureUVperFace::c_dirt);
			types.push_back(TextureUVperFace::c_stone);
		}


		const VoxelType& getType(const types::type_id id) const
		{
			if (id == 0)
				throw std::runtime_error("INDEX_OUT_OF_BOUNDS::Block type 0 doesn't exist");

			return types.at(id - 1);
		}


		std::vector<VoxelType> types{};
	};


}