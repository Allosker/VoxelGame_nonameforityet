#pragma once // utilities.hpp
// MIT
// Allosker ------------------------------
// =========+
// A set of utilities for texture managing
// ---------------------------------------

#include "uHeaders/openGL.hpp"
#include "uHeaders/types.hpp"

#include <cmath>

namespace Render
{
	struct UvAtlas
	{
		types::atlas_units pos_in_atlas{};
		types::pixels subset_size{};
	};

	struct UvPixels
	{
		types::pixels pos_in_texture{};
		types::pixels subset_size{};
	};


	inline types::uvs mapTextureUvs(vec2f point, vec2f unitSize) noexcept
	{
		return { point.x * unitSize.x, point.y * unitSize.y };
	}

	inline UvPixels mapTextureUvs(const UvAtlas& atlas) noexcept
	{
		return { { atlas.pos_in_atlas.x * atlas.subset_size.x, atlas.pos_in_atlas.y * atlas.subset_size.y }, atlas.subset_size };
	}

}