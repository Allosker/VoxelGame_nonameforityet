#pragma once // cube.hpp
// MIT
// Allosker ------------------------------
// =========+
// basic Cube class at the center of everything
// ---------------------------------------

#include "uHeaders/opengl.hpp"
#include "rendering/world_managing/data/basic/vertex.hpp"

// STILL ONGOING WORK

namespace Render::Data
{
	struct TextureFace
	{
		std::array<vec2f, 6> tex_pos{};
	};
	
	
	struct Voxel
	{
		types::type_id id{ 0 };
		uint16 light{};

		uint8 getR() const noexcept
		{							  
			return (light >> 8) & 0xF;
		}

		void setR(uint8 val) noexcept
		{                                         
			light = (light & 0xF0FF) | (val << 8);
		}

		uint8 getG() const noexcept
		{
			return (light >> 4) & 0xF;
		}

		void setG(uint8 val) noexcept
		{
			light = (light & 0xFF0F) | (val << 4);
		}

		uint8 getB() const noexcept
		{
			return light & 0xF;
		}

		void setB(uint8 val) noexcept
		{
			light = (light & 0xFFF0) | val;
		}

		uint8 getSunlight() const noexcept
		{
			return (light >> 12) & 0xF;
		}

		void setSunlight(uint8 val) noexcept
		{
			light = (light & 0x0FFF) | (val << 12);
		}

		static constexpr std::array<vec3f, 36> faces
		{
			// Position					   // Colors
			/*Front*/
			vec3f
			{0, 0, 1},  /*Left-Down*/
			{ 1, 0, 1},  /*Right-Down*/
			{0, 1, 1},  /*Left-Up*/

			{ 1, 0, 1},  /*Right-Down*/
			{ 1, 1, 1},  /*Right-Up*/
			{ 0, 1, 1},  /*Left-Up*/

			/*Back*/
			{ 1, 0, 0},  /*Left-Down*/
			{ 0, 0, 0},  /*Right-Down*/
			{ 1, 1, 0},  /*Left-Up*/

			{0, 0, 0},  /*Right-Down*/
			{0, 1, 0},  /*Right-Up*/
			{ 1, 1, 0},  /*Left-Up*/
			

			/*Up*/
			{ 1, 1, 1 },   /*Left-Down*/
			{ 1, 1, 0},  /*Right-Down*/
			{0, 1, 1 },   /*Left-Up*/

			{ 1, 1, 0},  /*Right-Down*/
			{0, 1, 0},  /*Right-Up*/
			{0, 1,  1},   /*Left-Up*/

			/*Down*/
			{ 1, 0, 0},  /*Left-Down*/
			{ 1, 0, 1},  /*Right-Down*/
			{ 0, 0, 0},   /*Left-Up*/
			
			{ 1, 0, 1},  /*Right-Down*/
			{ 0, 0, 1},   /*Right-Up*/
			{ 0, 0, 0},   /*Left-Up*/


			/*Left*/
			{ 1, 0, 1},  /*Left-Down*/
			{ 1, 0, 0},   /*Right-Down*/
			{ 1, 1, 1},  /*Left-Up*/

			{ 1, 0, 0},   /*Right-Down*/
			{ 1, 1, 0},   /*Right-Up*/
			{ 1, 1, 1},  /*Left-Up*/
			

			/*Right*/
			{0, 0, 0},  /*Right-Down*/
			{0, 0, 1},   /*Left-Down*/
			{0, 1, 0},  /*Right-Up*/

			{0, 0, 1},   /*Left-Up*/
			{0, 1, 1},  /*Right-Up*/
			{0, 1, 0},   /*Left-Down*/

		};

	};

} // Render::Data

