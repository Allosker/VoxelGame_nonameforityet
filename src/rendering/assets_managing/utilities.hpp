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
	inline types::uvs fromAtlasToPixels(vec2f point, vec2f unitSize) noexcept
	{
		return { point.x * unitSize.x, point.y * unitSize.y };
	}

	inline types::Rect<types::uvs> fromAtlasToPixels(const types::Rect<types::atlas_units>& atlas) noexcept
	{
		return { { atlas.pos.x * atlas.size.x, atlas.pos.y * atlas.size.y }, atlas.size };
	}

}