#include "image.hpp"


Render::Image::Image(const types::path& path_to_image)
{
	load_image(path_to_image);
}

GLenum Render::Image::getFormat() const noexcept
{
	switch (m_nrChannels)
	{
	case 1:
		return GL_RED;
		break;

	case 3:
		return GL_RGB;
		break;

	case 4:
		return GL_RGBA;
		break;
	}
}

std::vector<uint8> Render::Image::crop(vec2iu subset_ori, vec2iu subset_size) noexcept
{
	vec2i subset_place{ subset_ori + subset_size };
	if ((subset_size.x > m_size.x || subset_size.y > m_size.y) || (subset_place.x > m_size.x || subset_place.y > m_size.y))
		throw std::runtime_error("ERROR::Subset out of bounds");

	std::vector<uint8> new_data{};
	new_data.reserve(subset_size.x * subset_size.y * m_nrChannels);

	for (uint32 y{ subset_ori.y }; y < subset_place.y; y++)
	{
		for (uint32 x{ subset_ori.x }; x < subset_place.x; x++)
		{
			for (uint32 p{}; p < m_nrChannels; p++)
				new_data.emplace_back(m_data[(x + y * m_size.x) * m_nrChannels + p]);
		}
	}

	return new_data;
}

void Render::Image::insert(vec2iu pos, const Image& other) noexcept
{
	vec2iu subset_place{ pos + other.getSize() };
	if ((other.getSize().x > m_size.x || other.getSize().y > m_size.y) || (subset_place.x > m_size.x || subset_place.y > m_size.y))
		throw std::runtime_error("ERROR::Subset out of bounds");

	for (uint32 y{ pos.y }; y < subset_place.y; y++)
	{
		for (uint32 x{ pos.x }; x < subset_place.x; x++)
		{
			for (uint32 p{}; p < m_nrChannels; p++)
				m_data[(x + y * m_size.x) * m_nrChannels + p] = other.getData()[(x + y * other.getSize().x) * m_nrChannels + p];
		}
	}

}


/*private*/ void Render::Image::load_image(const types::path& path)
{
	stbi_set_flip_vertically_on_load(true);

	std::uint8_t* data{ stbi_load(path.string().c_str(), &m_size.x, &m_size.y, &m_nrChannels, 0) };
	int32 size{ m_size.x * m_size.y * m_nrChannels };

	m_data.assign(data, data + size);


	stbi_image_free(data);
}
