#pragma once // vertex.hpp
// MIT
// Allosker ------------------------------
// =========+
// Define a simple vertex struct which holds the most basic and important information for the program.
// ---------------------------------------

#include "uHeaders/opengl.hpp"

namespace render
{

	namespace data
	{

		struct Vertex
		{
			vec3f pos{};
			vec3f color{};
		};


	} // data 

} // render
