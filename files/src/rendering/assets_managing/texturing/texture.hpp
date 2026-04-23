#pragma once // texture.hpp
// MIT
// Allosker ------------------------------
// =========+
// Implement a small class that allocates a texture on the GPU (be it an atlas or anything). Based on a dynamic array of image data.
// ---------------------------------------

#include "uHeaders/opengl.hpp"
#include "uHeaders/types.hpp"
#include <filesystem>
#include "STB/stb_image.h"

#include "rendering/shader.hpp"
#include "rendering/assets_managing/image.hpp"

namespace Render::Texturing
{

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

		Texture(const types::path& tex_path, Type type = tex2D);

		Texture(const Image& image, Type type = tex2D);

		~Texture() noexcept;


	// = Actors

		void loadTexture(const types::path& tex_path);

		void deleteTexture() const noexcept;

		void bind() const noexcept;


	// = Getters

		GLuint ID() const noexcept { return m_tex; }

		vec2f getSize() const noexcept { return vec2f{ static_cast<float>(m_width), static_cast<float>(m_height) }; }


	private:

		Type m_type{};

		std::int32_t m_width{}, m_height{};

		GLuint m_tex{};

	};

} // Render::Texturing