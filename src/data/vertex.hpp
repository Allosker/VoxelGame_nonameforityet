#pragma once // ::data::vertex.hpp
// MIT
// Allosker ------------------------------
// =========+
// Define a simple ::data::vertex struct which holds the most basic and important information for the program.
// ---------------------------------------

#include "utilities/opengl.hpp"

namespace Data
{
	struct Vertex
	{
		vec3f pos{};
		vec2f uv{};
	};

	struct Vertex2D
	{
		vec2f pos{};
		vec2f uv{};
	};

	struct VertexRGB
	{
		vec3f pos{};
		vec3f color{};
	};

	struct VertexRGBA
	{
		vec3f pos;
		vec4f color;
	};


} // data 
