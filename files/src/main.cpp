#include <iostream>
#include <stdexcept>
#include <limits>

#include "mpml/vectors/special_overloads/iostream_vectors.hpp"

#include "window_and_inputs/window.hpp"
#include "window_and_inputs/inputs.hpp"

#include "rendering/shader.hpp"
#include "rendering/utilities/camera.hpp"

// Block Hitting
#include "rendering/utilities/ray.hpp"
#include <utility>
//

#include "gameplay/world/chunk.hpp"
#include "rendering/world_managing/data/chunk/chunkMesh.hpp"
#include "gameplay/world/chunkGrid.hpp"

#include "rendering/assets_managing/texturing/texture.hpp"
#include "window_and_inputs/inputs.hpp"


//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) noexcept;

static void framebuffersize_callback(GLFWwindow* window, int width, int height) noexcept;

static void mouse_callback(GLFWwindow* window, double xpos, double ypos) noexcept;

static void inputs(const Wai::Window& window) noexcept;

Render::Utils::Camera camera{};

float deltaTime{};

// Returns the location of the block that was hit by the ray alongside its corresponding chunk, if no block was hit, std::nullopt is returned
static inline std::optional<Render::Utils::RayCastResult> raycast(const types::pos& origin, const types::pos& dir, const Gameplay::World::ChunkGrid& grid, uint64 maxLength) noexcept
{
	Render::Utils::Ray ray{origin, dir};
	vec3f normal{};

	while ((origin - ray.pos).length() < maxLength)
	{
		if (!grid.is_empty(ray.pos))
		{
			return std::make_optional<Render::Utils::RayCastResult>({origin, ray.pos, normal});
		}


		if (ray.tMax.x < ray.tMax.y)
		{
			if (ray.tMax.x < ray.tMax.z)
			{
				ray.pos.x += ray.step.x;
				ray.tMax.x += ray.tDelta.x;
				normal = { -ray.step.x, 0.f, 0.f };
			}
			else
			{
				ray.pos.z += ray.step.z;
				ray.tMax.z += ray.tDelta.z;
				normal = { 0.f, 0.f, -ray.step.z };
			}
		}
		else
		{
			if (ray.tMax.y < ray.tMax.z)
			{
				ray.pos.y += ray.step.y;
				ray.tMax.y += ray.tDelta.y;
				normal = { 0.f, -ray.step.y, 0.f };
			}
			else
			{
				ray.pos.z += ray.step.z;
				ray.tMax.z += ray.tDelta.z;
				normal = { 0.f, 0.f, -ray.step.z };
			}
		}

	}

	return std::nullopt;
}


int main()
try
{

	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_DEBUG, true);

	//glfwWindowHint(GLFW_SAMPLES, 4);

	Wai::Window window{ {640, 480}, "Test", nullptr, nullptr };


	//glfwSetKeyCallback(window.get(), key_callback);

	glfwSetFramebufferSizeCallback(window.get(), framebuffersize_callback);

	glfwSetCursorPosCallback(window.get(), mouse_callback);

	glfwSetInputMode(window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	Render::Shader shader{ SHADER_PATH"shader.vert", SHADER_PATH"shader.frag" };

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_MULTISAMPLE);

	Render::Texturing::Texture texture{ ASSET_PATH"atlas.png"};

	texture.bind();

	std::vector<Render::Data::ChunkMesh> chunkms;

	Gameplay::World::Chunk chunk{ {0,0,0} };
	Render::Data::ChunkMesh chunkm{ chunk };

	
	std::vector<Render::Data::VertexColor> CH
	{
		Render::Data::VertexColor
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


		Render::Data::VertexColor
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
		


		Render::Data::VertexColor
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


	GLuint vao{};
	GLuint vbo{};

	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo); 
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, CH.size() * sizeof(Render::Data::VertexColor), CH.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Render::Data::VertexColor), std::bit_cast<void*>(offsetof(Render::Data::VertexColor, pos)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Render::Data::VertexColor), std::bit_cast<void*>(offsetof(Render::Data::VertexColor, color)));
	glEnableVertexAttribArray(1);


	Render::Shader CHshader{ SHADER_PATH"color.vert", SHADER_PATH"color.frag" };


	Gameplay::World::ChunkGrid grid{};

	float lastFrame{};
	while (window.isOpen())
	{
		deltaTime = glfwGetTime() - lastFrame;
		lastFrame = glfwGetTime();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		inputs(window);

		shader.use();

		mat4f model{ mpml::Identity4<float> };
		mat4f view{ mpml::lookAt(camera.pos, camera.front_dir + camera.pos, camera.up_dir) };
		mat4f proj{ mpml::perspective(mpml::Angle::fromDegrees(45), window.getSize().x, window.getSize().y, 0.1f, 100.f, true) };

		//model = mpml::rotate(model, mpml::Angle::fromDegrees(glfwGetTime() * 10), { 0.3, 0.5, 0.9 });

		//model = mpml::translate(model, { 2, 3, -4 });

		shader.setValue("model", model);
		shader.setValue("view", view);
		shader.setValue("proj", proj);



		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		grid.update(camera.pos);
		
		grid.draw_all();

		CHshader.use();
		CHshader.setValue("model", model);
		CHshader.setValue("view", view);
		CHshader.setValue("proj", proj);
		

		const auto& ray_result{ raycast(camera.pos, camera.front_dir, grid, 2000) };

		if (ray_result)
		{

			CHshader.setValue("model", mpml::translate(mpml::scale(model, 1.008f), { std::floor(ray_result->pos.x), std::floor(ray_result->pos.y), std::floor(ray_result->pos.z) }));

			if (window.keyPressed(Wai::Buttons::F))
				grid.break_block_at(ray_result->pos);

			if (window.keyPressed(Wai::Buttons::R))
				grid.place_block_at(ray_result->pos, ray_result->normal);
		}


		
		
		glBindVertexArray(vao);
		glDrawArrays(GL_LINES, 0, CH.size());

		shader.use();
		//
		//chunkm.draw();

		window.clearEvents();
		window.display();
	}


	glfwTerminate();

	return 0;
}
catch (const std::runtime_error& e)
{
	std::cout << e.what() << '`n';

	return -1;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) noexcept
{
	static float lastX{ static_cast<float>(xpos) }, lastY{ static_cast<float>(ypos) };

	float xoffset = xpos - lastX;
	float yoffset = ypos - lastY;

	lastX = xpos;
	lastY = ypos;

	float sensitivity{ 0.1f };

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	static float yaw{-90}, pitch{};

	yaw += xoffset;
	pitch += yoffset;


	if (pitch > 89.f)
		pitch = 89.f;
	if (pitch < -89.f)
		pitch = -89.f;

	vec3f direction{};

	float radPitch{ mpml::toRadians(pitch) };
	float cosPitch{ std::cos(radPitch) };
	float radYaw{ mpml::toRadians(yaw) };

	direction.x = std::cos(radYaw) * cosPitch;
	direction.y = -std::sin(radPitch);
	direction.z = std::sin(radYaw) * cosPitch;

	camera.front_dir = direction.normal();
}

void inputs(const Wai::Window& window) noexcept
{
	using namespace Wai;
	using b = Buttons;

	const auto campos = camera.pos;

	float camSpeed{ 5.F * deltaTime };

	if (window.keyPressed(b::Escape))
		window.close();

	if (window.keyPressed(b::W))
		camera.pos += camera.front_dir * camSpeed;

	if (window.keyPressed(b::S))
		camera.pos -= camera.front_dir * camSpeed;

	if (window.keyPressed(b::D))
		camera.pos += camera.front_dir.cross(camera.up_dir).normal() * camSpeed;

	if (window.keyPressed(b::A))
		camera.pos -= camera.front_dir.cross(camera.up_dir).normal() * camSpeed;

	if (window.keyPressed(b::Left_shift))
		camera.pos -= camera.up_dir * camSpeed;

	if (window.keyPressed(b::Space))
		camera.pos += camera.up_dir * camSpeed;

	// if (campos != camera.pos)
	// 	std::cout << "CamPos: " << camera.pos << std::endl;
}

void framebuffersize_callback(GLFWwindow* window, int width, int height) noexcept
{
	glViewport(0, 0, width, height);
}
