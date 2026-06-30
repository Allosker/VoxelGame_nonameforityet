#pragma once // skybox.hpp
// MIT
// Allosker ------------------------------
// =========+
// Defines its own textures, this is a dynamic skybox class which rotates slowly as time passes by.
// ---------------------------------------

#include "utilities/opengl.hpp"
#include "data/image.hpp"
#include "rendering/shader.hpp"
#include "data/renderStates.hpp"


namespace render
{

	class Skybox
	{
	public:


		Skybox()
		{
			glGenTextures(1, &m_cubemap_id);
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap_id);

            const char* texture_paths[]
            {
                ASSET_PATH"skybox/left.png",
                ASSET_PATH"skybox/right.png",
                

                ASSET_PATH"skybox/top.png",
                ASSET_PATH"skybox/bottom.png",
                
                
                       
                ASSET_PATH"skybox/front.png",
                ASSET_PATH"skybox/back.png",
                
            };

            for (GLuint i{}; i < 6; i++)
            {
                Image image{ texture_paths[i] };

                glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, GL_RGB, image.getSize().x, image.getSize().y, 0, GL_RGB, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(image.getData().data())
                );
            }

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


            glCreateVertexArrays(1, &m_vao);
            glGenBuffers(1, &m_vbo);

            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

            glBufferData(GL_ARRAY_BUFFER, g_skybox_vertices.size() * sizeof(float), g_skybox_vertices.data(), GL_STATIC_DRAW);

            glBindVertexArray(m_vao);

            glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(float) * 3, nullptr);
            glEnableVertexAttribArray(0);

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

		}

        void draw(const RenderStates& render, const vec3f& cam_pos) const noexcept
        {
            glDepthMask(false);

            shader.use();
            shader.setValue("vp", render.vp);
            shader.setValue("cameraPos", cam_pos);

            glBindVertexArray(m_vao);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap_id);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


	private:


		GLuint m_cubemap_id{};

        GLuint m_vao{};
        GLuint m_vbo{};

        Shader shader{ SHADER_PATH"simple/skybox.vert", SHADER_PATH"simple/skybox.frag" };

        static constexpr std::array<float, 108> g_skybox_vertices = {
            // positions          
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };
	};
}