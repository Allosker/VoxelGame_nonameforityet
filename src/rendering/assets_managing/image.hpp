#pragma once // image.hpp
// MIT
// Allosker ------------------------------
// =========+
// This class manages an image on RAM to allow for trasnformation thereof. This class is mainly a helper for managing image data before sending it back to the GPU as a Texture.
// ---------------------------------------

#include "uHeaders/types.hpp"
#include "uHeaders/opengl.hpp"*

#include "STB/stb_image.h"

#include <vector>
#include <filesystem>

namespace Render
{

	class Image
	{
	public:

		Image(const types::path& path_to_image);


		// = Getters 

		std::vector<uint8>& getData() noexcept { return m_data; }
		const std::vector<uint8>& getData() const noexcept { return m_data; }

		vec2iu getSize() const noexcept { return m_size; }

		GLenum getFormat() const noexcept;

		int32 getBytesPerPixel() const noexcept { return m_nrChannels; }


		// = Setters

		std::vector<uint8> crop(vec2iu subset_ori, vec2iu subset_size) noexcept;

		void insert(vec2iu pos, const Image& other) noexcept;






	private:

		void load_image(const types::path& path);


	private:

		std::vector<uint8> m_data{};
		vec2iu m_size{};

		GLenum m_format{};
		int32 m_nrChannels{};

	};

}