#include "rendering/assets_managing/texturing/texture.hpp"

// =====================
// Construction/Destruction
// =====================

Render::Texturing::Texture::Texture(const Path& tex_path, Type type)
	: m_type{type}
{
	glGenTextures(1, &m_tex);
	glBindTexture(GL_TEXTURE_2D, m_tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	loadTexture(tex_path);
}

Render::Texturing::Texture::~Texture() noexcept
{
	deleteTexture();
}


// =====================
// Actors
// =====================

void Render::Texturing::Texture::loadTexture(const Path& tex_path)
{
	std::int32_t nrChannels{};

	stbi_set_flip_vertically_on_load(true);
	std::uint8_t* data{ stbi_load(tex_path.string().c_str(), &m_width, &m_height, &nrChannels, 0)};

	GLenum color_channel{};

	switch (nrChannels)
	{
	case 1:
		color_channel = GL_RED;
		break;

	case 2:
		color_channel = GL_RGB;
		break;

	case 4:
		color_channel = GL_RGBA;
		break;
	} 

	glTexImage2D(m_type, 0, color_channel, m_width, m_height, 0, color_channel, GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(m_type);

	stbi_image_free(data);
}

void Render::Texturing::Texture::deleteTexture() const noexcept
{
	glDeleteTextures(1, &m_tex);
}

void Render::Texturing::Texture::bind() const noexcept
{
	glBindTexture(m_type, m_tex);
}