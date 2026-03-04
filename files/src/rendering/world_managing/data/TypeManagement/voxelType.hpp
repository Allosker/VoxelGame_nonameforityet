#pragma once // type.hpp
// MIT
// Allosker ------------------------------
// =========+
// Small structure for cube type, types are definitions of blocks. So, a block in its integrity is defined by the type corresponding to its ID. 
// Note: blocks may share similar IDs.
// ---------------------------------------

#include "uHeaders/opengl.hpp"
#include "uHeaders/types.hpp"

namespace Render::Data::Types
{

	namespace TextureUVperFace
	{

		constexpr std::array<vec2f, 6> c_dirtGrass{ vec2f{ 0, 1 }, { 0, 1 }, { 0, 0 }, { 0, 2 }, { 0, 1 }, { 0, 1 } };

		constexpr std::array<vec2f, 6> c_dirt{ vec2f{ 0, 2 }, { 0, 2 }, { 0, 2 }, { 0, 2 }, { 0, 2 }, { 0, 2 } };
		
		constexpr std::array<vec2f, 6> c_stone{ vec2f{ 1, 2 }, { 1, 2 }, { 1, 2 }, { 1, 2 }, { 1, 2 }, { 1, 2 } };

	}

	inline std::array<std::array<vec2f, 4>, 6> mapTextureUVs(const std::array<vec2f, 6>& texturePos, const vec2f& atlas_size = { 96, 96 })
	{
		std::array<std::array<vec2f, 4>, 6> mappedUVs{
			std::array<vec2f, 4>
			{
				vec2f
				{ 0, 0 },
				{ 1, 0 },
				{ 0, 1 },
				{ 1, 1 }
			},

			{
				vec2f
				{ 0, 0 },
				{ 1, 0 },
				{ 0, 1 },
				{ 1, 1 }
			},


			{
				vec2f
				{ 0, 0 },
				{ 1, 0 },
				{ 0, 1 },
				{ 1, 1 }
			},


			{
				vec2f
				{ 0, 0 },
				{ 1, 0 },
				{ 0, 1 },
				{ 1, 1 }
			},


			{
				vec2f
				{ 0, 0 },
				{ 1, 0 },
				{ 0, 1 },
				{ 1, 1 }
			},


			{
				vec2f
				{ 0, 0 },
				{ 1, 0 },
				{ 0, 1 },
				{ 1, 1 }
			}
		};

		for (size_t i{}; i < mappedUVs.size(); i++)
		{

			for (auto& uv : mappedUVs[i])
			{

				uv = uv * 32.f + texturePos[i] * 32.f;

				//if (uv.x > 1) uv.x -= 1; else uv.x += 1;
				//if (uv.y < 1) uv.y -= 1; else uv.y += 1;

				uv.x /= atlas_size.x;
				uv.y /= atlas_size.y;
			}

		}

		return mappedUVs;
	}

	struct VoxelType
	{

		VoxelType() = delete;

		VoxelType(/*uint64 id_,*/ const std::array<vec2f, 6>& tex_pos)
			: /*id{id_},*/ uvs{mapTextureUVs(tex_pos)}
		{ }


		std::array<std::array<vec2f, 4>, 6> uvs{};

		/*uint64 id{};*/
	};


} // Render::Types