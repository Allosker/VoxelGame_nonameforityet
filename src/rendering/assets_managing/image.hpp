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

		Image(vec2iu size, uint8* ptr, int32 channel, GLenum format);

		Image(vec2iu allocate_size, int32 channel, GLenum format);


		Image(Image&&) = default;
		Image& operator=(Image&&) = default;

		Image() = delete;
		Image(Image&) = delete;
		Image& operator=(Image&) = delete;


		// = Getters 

		std::vector<uint8>& getData() noexcept { return m_data; }
		const std::vector<uint8>& getData() const noexcept { return m_data; }

		vec2iu getSize() const noexcept { return m_size; }

		GLenum getFormat() const noexcept;

		int32 getBytesPerPixel() const noexcept { return m_nrChannels; }

		uint8* data() noexcept { return m_data.data(); }
		const uint8* data() const noexcept { return m_data.data(); }


		// = Setters

		void resize(vec2iu new_size) noexcept 
		{
			m_data.resize(new_size.x * new_size.y); 
			m_size = new_size;
		}

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