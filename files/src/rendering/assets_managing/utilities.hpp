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

	// Doesn't work, fix it
	inline types::uvs mapTextureUvs(vec2f point, vec2f unitSize) noexcept
	{
		return { point.x * unitSize.x, point.y * unitSize.y };
	}

}