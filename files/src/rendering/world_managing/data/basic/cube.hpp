#pragma once // cube.hpp
// MIT
// Allosker ------------------------------
// =========+
// basic Cube class at the center of everything
// ---------------------------------------

#include "uHeaders/opengl.hpp"
#include "rendering/world_managing/data/basic/vertex.hpp"
#include <iostream>

namespace Render::Data
{
	struct textureFace
	{
		std::array<vec2f, 6> tex_pos{};
	};

	namespace BlockType
	{

		constexpr std::array<vec2f, 6> c_dirtGrass{ vec2f{ 0, 1 }, { 0, 1 }, { 0, 0 }, { 0, 2 }, { 0, 1 }, { 0, 1 } };

		constexpr std::array<vec2f, 6> c_dirt{ vec2f{ 0, 2 }, { 0, 2 }, { 0, 2 }, { 0, 2 }, { 0, 2 }, { 0, 2 } };

	}
	
	struct Cube
	{
		enum Filling
			: std::uint8_t
		{
			//Not_A_Block,
			Empty,
			//Transparent,
			Full
		};


		Cube(Filling _filling = Full)
			: filling{ _filling }
		{
		}


		void updateSpritePos(const std::array<vec2f, 6>& spritePos, const vec2f& atlas_size = { 96, 96 })
		{

			for (size_t i{}; i < sprite_coords.size(); i++)
			{
				
				for (auto& uv : sprite_coords[i])
				{
					uv = uv * 32.f + spritePos[i] * 32.f;

					uv.x /= atlas_size.x;
					uv.y /= atlas_size.y;
				}

			}
		}

	
		Filling filling{};

		std::array<std::array<vec2f, 6>, 6> sprite_coords
		{
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


		static constexpr std::array<vec3f, 36> faces
		{
			// Position					   // Colors
			/*Front*/
			vec3f
			{-0.5, 0, 0},  /*Left-Down*/
			{ 0.5, 0, 0},  /*Right-Down*/
			{-0.5, 1, 0},  /*Left-Up*/	

			{ 0.5, 0, 0},  /*Right-Down*/
			{ 0.5, 1, 0},  /*Right-Up*/
			{-0.5, 1, 0},  /*Left-Up*/	

			/*Back*/
			{ 0.5, 0, -1},  /*Left-Down*/ 
			{-0.5, 0, -1},  /*Right-Down*/
			{ 0.5, 1, -1},  /*Left-Up*/   

			{-0.5, 0, -1},  /*Right-Down*/
			{-0.5, 1, -1},  /*Right-Up*/  
			{ 0.5, 1, -1},  /*Left-Up*/   
			

			/*Up*/
			{ 0.5, 1, 0 },   /*Left-Down*/ 
			{ 0.5, 1, -1},  /*Right-Down*/
			{-0.5, 1, 0 },   /*Left-Up*/   

			{ 0.5, 1, -1},  /*Right-Down*/
			{-0.5, 1, -1},  /*Right-Up*/  
			{-0.5, 1, 0 },   /*Left-Up*/   

			/*Down*/
			{ 0.5, 0, -1},  /*Left-Down*/
			{ 0.5, 0, 0},  /*Right-Down*/
			{ -0.5, 0, -1},   /*Left-Up*/
			
			{ 0.5, 0, 0},  /*Right-Down*/
			{ -0.5, 0, 0},   /*Right-Up*/
			{ -0.5, 0, -1},   /*Left-Up*/


			/*Left*/
			{ 0.5, 0, 0},  /*Left-Down*/
			{ 0.5, 0, -1},   /*Right-Down*/
			{ 0.5, 1, 0},  /*Left-Up*/

			{ 0.5, 0, -1},   /*Right-Down*/
			{ 0.5, 1, -1},   /*Right-Up*/
			{ 0.5, 1, 0},  /*Left-Up*/
			

			/*Right*/
			{-0.5, 0, -1},  /*Right-Down*/
			{-0.5, 0, 0},   /*Left-Down*/
			{-0.5, 1, -1},  /*Right-Up*/  

			{-0.5, 0, 0},   /*Left-Up*/   
			{-0.5, 1, 0},  /*Right-Up*/  
			{-0.5, 1, -1},   /*Left-Down*/

		};

	};

} // Render::Data

