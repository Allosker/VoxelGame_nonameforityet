#pragma once // resourceManager.hpp
// MIT
// Allosker ------------------------------
// =========+
// Manages all Shaders and Textures that live on the GPU.
// ---------------------------------------

#include "rendering/shader.hpp"
#include "rendering/texture.hpp"

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

	static Resources& get()
	{
		static Resources res;
		return res;
	};

private:

	Resources() = default;

};