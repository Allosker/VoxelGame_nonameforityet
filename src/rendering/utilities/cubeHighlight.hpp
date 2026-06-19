#pragma once

#include "utilities/opengl.hpp"
#include "rendering/shader.hpp"
#include "data/vertex.hpp"

#include <vector>

namespace render::utils
{ 
	struct CubeHighlight
	{

		// = Destruction/Initialization

		CubeHighlight()
			: CHshader{ SHADER_PATH"color.vert", SHADER_PATH"color.frag" }
		{
			glCreateVertexArrays(1, &vao);
			glBindVertexArray(vao);

			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(Data::VertexRGB), data.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Data::VertexRGB), std::bit_cast<void*>(offsetof(Data::VertexRGB, pos)));
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Data::VertexRGB), std::bit_cast<void*>(offsetof(Data::VertexRGB, color)));
			glEnableVertexAttribArray(1);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);	
		}


		// = Actors

		void update(const mat4f& model, const mat4f& view, const mat4f& proj, const vec3f& pos)
		{
			CHshader.use();

			setPos(model, pos);
			CHshader.setValue("view", view);
			CHshader.setValue("proj", proj);
		}

		void draw()
		{
			CHshader.use();

			glBindVertexArray(vao);
			glDrawArrays(GL_LINES, 0, data.size());
			glBindVertexArray(0);
		}

		void useShader() const noexcept { CHshader.use(); }

		// = Setters 

		void setPos(const mat4f& model, const vec3f& pos)
		{
			CHshader.setValue("model", mpml::translate(mpml::scale(model, scale), { std::floor(pos.x), std::floor(pos.y), std::floor(pos.z) }));
		}

		void setScale(float newScale) noexcept { scale = newScale; }
		
	private:

		render::Shader CHshader;

		float scale{ 1.008f };

		GLuint vao{};
		GLuint vbo{};

		const std::vector<Data::VertexRGB> data
		{
			Data::VertexRGB
			{
				vec3f
				{
					0, 0, 0
				},
				vec3f {0}
			},

			{
				vec3f
				{
					1, 0, 0
				},
				vec3f {0}
			},


			{
				vec3f
				{
					0, 1, 0
				},
				vec3f {0}
			},

			{
				vec3f
				{
					1, 1, 0
				},
				vec3f {0}
			},


			{
				vec3f
				{
					0, 0, 1
				},
				vec3f {0}
			},

			{
				vec3f
				{
					1, 0, 1
				},
				vec3f {0}
			},


			{
				vec3f
				{
					0, 1, 1
				},
				vec3f {0}
			},

			{
				vec3f
				{
					1, 1, 1
				},
				vec3f {0}
			},


			Data::VertexRGB
			{
				vec3f
				{
					0, 0, 1
				},
				vec3f {0}
			},

			{
				vec3f
				{
					0, 0, 0
				},
				vec3f {0}
			},


			{
				vec3f
				{
					1, 0, 1
				},
				vec3f {0}
			},

			{
				vec3f
				{
					1, 0, 0
				},
				vec3f {0}
			},


			{
				vec3f
				{
					0, 1, 1
				},
				vec3f {0}
			},

			{
				vec3f
				{
					0, 1, 0
				},
				vec3f {0}
			},


			{
				vec3f
				{
					1, 1, 1
				},
				vec3f {0}
			},

			{
				vec3f
				{
					1, 1, 0
				},
				vec3f {0}
			},



			Data::VertexRGB
			{
				vec3f
				{
					0, 1, 0
				},
				vec3f {0}
			},

			{
				vec3f
				{
					0, 0, 0
				},
				vec3f {0}
			},


			{
				vec3f
				{
					1, 1, 0
				},
				vec3f {0}
			},

			{
				vec3f
				{
					1, 0, 0
				},
				vec3f {0}
			},


			{
				vec3f
				{
					0, 1, 1
				},
				vec3f {0}
			},

			{
				vec3f
				{
					0, 0, 1
				},
				vec3f {0}
			},


			{
				vec3f
				{
					1, 1, 1
				},
				vec3f {0}
			},

			{
				vec3f
				{
					1, 0, 1
				},
				vec3f {0}
			},
		};

	};
}