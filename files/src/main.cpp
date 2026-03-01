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

#include "rendering/utilities/cubeHighlight.hpp"

/* TODOLIST
* 
*	== Go through all files (especially cubeHighlight) and reorganize things a little bit
*	== Look through world managing to set the height of cubes (do that at first with sine function) and then you're gonne go through perlin noise mate
*
*/


//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) noexcept;

static void framebuffersize_callback(GLFWwindow* window, int width, int height) noexcept;

static void mouse_callback(GLFWwindow* window, double xpos, double ypos) noexcept;

static void inputs(const Wai::Window& window) noexcept;

Render::Utils::Camera camera{};

float deltaTime{};


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



	Render::Utils::CubeHighlight ch;


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
		mat4f proj{ mpml::perspective(mpml::Angle::fromDegrees(90), window.getSize().x, window.getSize().y, 0.1f, 100.f) };



		shader.setValue("model", model);
		shader.setValue("view", view);
		shader.setValue("proj", proj);



		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		   
		grid.update(camera.pos);
		
		grid.draw_all();
		

		const auto& ray_result{ Render::Utils::raycast(camera.pos, camera.front_dir, grid, 2000) };

		if (ray_result)
		{
			ch.update(model, view, proj, ray_result->pos);

			if (window.keyPressedOnce(Wai::Buttons::F))
				grid.set_voxel_at(ray_result->pos, Render::Data::Voxel::Empty);

			if (window.keyPressedOnce(Wai::Buttons::R))
				grid.set_voxel_at(ray_result->pos + ray_result->normal, Render::Data::Voxel::Full);

			ch.draw();
		}

		
		

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
