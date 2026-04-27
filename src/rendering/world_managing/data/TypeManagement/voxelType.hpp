#pragma once // type.hpp
// MIT
// Allosker ------------------------------
// =========+
// Small structure for cube type, types are definitions of blocks. So, a block in its integrity is defined by the type corresponding to its ID. 
// Note: blocks may share similar IDs.
// ---------------------------------------

#include "uHeaders/opengl.hpp"
#include "uHeaders/types.hpp"

namespace Render
{
	inline std::array<std::array<vec2f, 4>, 6> mapTextureUVs_4(const std::array<vec2f, 6>& texturePos, const vec2f& atlas_size = { 96, 96 });
}

namespace Render::Data::Types
{

	namespace TextureUVperFace
	{
		// Each vector represents the position of the texture of a face under the format: 32x32
		// front, back / up, down / left, right

		constexpr std::array<vec2f, 6> c_dirtGrass{ vec2f{ 0,1 }, { 0, 1 }, { 0, 0 }, { 0, 2 }, { 0, 1 }, { 0, 1 } };

		constexpr std::array<vec2f, 6> c_dirt{ vec2f{ 0, 2 }, { 0, 2 }, { 0, 2 }, { 0, 2 }, { 0, 2 }, { 0, 2 } };

		constexpr std::array<vec2f, 6> c_stone{ vec2f{ 1, 2 }, { 1, 2 }, { 1, 2 }, { 1, 2 }, { 1, 2 }, { 1, 2 } };

		constexpr std::array<vec2f, 6> c_deepStone{ vec2f{ 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 } };

		constexpr std::array<vec2f, 6> c_water1{ vec2f{ 2, 2 }, { 2, 2 }, { 2, 2 }, { 2, 2 }, { 2, 2 }, { 2, 2 } };

		constexpr std::array<vec2f, 6> c_tree{ vec2f{ 2, 1 }, { 2, 1 }, { 2, 0 }, { 2, 0 }, { 2, 1 }, { 2, 1 } };

		constexpr std::array<vec2f, 6> c_leaf{ vec2f{ 1, 0 }, { 1, 0 }, { 1, 0 }, { 1, 0 }, { 1, 0 }, { 1, 0 } };

	}

	struct VoxelType
	{

		//VoxelType() = delete;

		VoxelType(const std::array<vec2f, 6>& tex_pos, bool transparent = false, bool destructible = true, const std::array<vec3f, 6>& offsets = { 0 })
			: uvs{ mapTextureUVs_4(tex_pos) }, is_transparent{ transparent }, is_destructible{ destructible }, face_offsets{ offsets }
		{
		}


		std::array<std::array<vec2f, 4>, 6> uvs{};

		// do offset for each face
		std::array<vec3f, 6> face_offsets{};


		bool is_transparent{};
		bool is_destructible{};

		static constexpr float g_texture_voxel_size{ 32.f };
		/*uint64 id{};*/
	};


} // Render::Types


namespace Render
{
	inline std::array<std::array<vec2f, 4>, 6> mapTextureUVs_4(const std::array<vec2f, 6>& texturePos, const vec2f& atlas_size)
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

				uv = (uv + texturePos[i]) * Data::Types::VoxelType::g_texture_voxel_size;

				//if (uv.x > 1) uv.x -= 1; else uv.x += 1;
				//if (uv.y < 1) uv.y -= 1; else uv.y += 1;

				uv.x /= atlas_size.x;
				uv.y /= atlas_size.y;
			}

		}

		return mappedUVs;
	}

	inline std::array<std::array<vec2f, 6>, 6> mapTextureUVs_6(const std::array<vec2f, 6>& texturePos, const vec2f& atlas_size = { 96, 96 })
	{
		std::array<std::array<vec2f, 6>, 6> mappedUVs{
			std::array<vec2f, 6>
			{
				vec2f
				{ 0, 0 },
				{ 1, 0 },
				{ 0, 1 },

				{ 1, 0 },
				{ 1, 1 },
				{ 0, 1 },
			},

			{
				vec2f
				{ 0, 0 },
				{ 1, 0 },
				{ 0, 1 },

				{ 1, 0 },
				{ 1, 1 },
				{ 0, 1 },
			},


			{
				vec2f
				{ 0, 0 },
				{ 1, 0 },
				{ 0, 1 },

				{ 1, 0 },
				{ 1, 1 },
				{ 0, 1 },
			},


			{
				vec2f
				{ 0, 0 },
				{ 1, 0 },
				{ 0, 1 },

				{ 1, 0 },
				{ 1, 1 },
				{ 0, 1 },
			},


			{
				vec2f
				{ 0, 0 },
				{ 1, 0 },
				{ 0, 1 },

				{ 1, 0 },
				{ 1, 1 },
				{ 0, 1 },
			},


			{
				vec2f
				{ 0, 0 },
				{ 1, 0 },
				{ 0, 1 },

				{ 1, 0 },
				{ 1, 1 },
				{ 0, 1 },
			}
		};

		for (size_t i{}; i < mappedUVs.size(); i++)
		{

			for (auto& uv : mappedUVs[i])
			{

				uv = uv * Data::Types::VoxelType::g_texture_voxel_size + texturePos[i] * Data::Types::VoxelType::g_texture_voxel_size;

				//if (uv.x > 1) uv.x -= 1; else uv.x += 1;
				//if (uv.y < 1) uv.y -= 1; else uv.y += 1;

				uv.x /= atlas_size.x;
				uv.y /= atlas_size.y;
			}

		}

		return mappedUVs;
	}
}

