#pragma once // types.hpp
// MIT
// Allosker ------------------------------
// =========+
// Define a list of useful types
// ---------------------------------------

#include <cstdint>
#include <mpml/vectors/vectors.hpp>


using int8		= std::int8_t;
using int16		= std::int16_t;
using int32		= std::int32_t;
using int64		= std::int64_t;

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

namespace types
{
	using pos = mpml::Vector3<float>;
	using loc = mpml::Vector3<int64>;
	using chunk_loc = mpml::Vector3<uint64>;

	using chunk_index = uint16;

	using type_id = std::size_t;

	using mapped_uvs = mpml::Vector2<float>;
	using uvs = mpml::Vector2<float>;
	using atlas_units = mpml::Vector2<float>;
	using pixels = mpml::Vector2<float>;
}