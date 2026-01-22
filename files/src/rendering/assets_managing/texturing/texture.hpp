#pragma once // texture.hpp
// MIT
// Allosker ------------------------------
// =========+
// Implement a small class that allocates a texture on the GPU (be it an atlas or anything). Based on a dynamic array of image data.
// ---------------------------------------

#include "uHeaders/opengl.hpp"
#include <filesystem>
#include "STB/stb_image.h"

#include "rendering/shader.hpp"

namespace Render::Texturing
{
	using Path = std::filesystem::path;

	class Texture
	{
	public:

		enum Type
		{
			tex1D = GL_TEXTURE_1D,
			tex2D = GL_TEXTURE_2D,
			tex3D = GL_TEXTURE_3D
		};

	public:

	// = Construction/Destruction

		Texture(const Path& tex_path, Type type = tex2D);

		~Texture() noexcept;

	// = Actors

		void loadTexture(const Path& tex_path);

		void deleteTexture() const noexcept;

		void bind() const noexcept;


	// = Getters

		GLuint ID() const noexcept { return m_tex; }


	private:

		Type m_type{};

		std::int32_t m_width{}, m_height{};

		GLuint m_tex{};

	};

} // Render::Texturing