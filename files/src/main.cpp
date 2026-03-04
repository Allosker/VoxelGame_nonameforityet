#include <iostream>
#include <stdexcept>
#include <limits>

#include "mpml/vectors/special_overloads/iostream_vectors.hpp"

#include "window_and_inputs/window.hpp"
#include "window_and_inputs/inputs.hpp"

#include "rendering/shader.hpp"
#include "rendering/utilities/camera.hpp"

// Block Hitting
#include "world/voxels/utilities/ray.hpp"
#include <utility>
//

#include "world/world.hpp"

#include "rendering/assets_managing/texturing/texture.hpp"
#include "window_and_inputs/inputs.hpp"

#include "rendering/utilities/cubeHighlight.hpp"

/* TODOLIST
* 
*	== Look through world managing to set the height of cubes (do that at first with sine function) and then you're gonne go through perlin noise mate
*
*/


#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


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

	// Example from gettingn started
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(window.get(), true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
		ImGui_ImplOpenGL3_Init();


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



	Render::Utils::CubeHighlight ch;

	GameWorld::World world{};

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
		mat4f proj{ mpml::perspective(mpml::Angle::fromDegrees(90), window.getSize().x, window.getSize().y, 0.1f, 1000.f) };



		shader.setValue("model", model);
		shader.setValue("view", view);
		shader.setValue("proj", proj);



		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		   
	
		world.update(camera.pos);

		world.draw_chunkGrid();

		const auto& ray_result{ GameWorld::Voxels::Utils::raycast(camera.pos, camera.front_dir, world.grid, 2000) };


		if (ray_result)
		{
			ch.update(model, view, proj, ray_result->pos);

			if (window.isMouseButtonPressedOnce(Wai::Buttons::Mouse::Left))
				world.set_voxel_at(ray_result->pos, 0);

			if (window.isMouseButtonPressedOnce(Wai::Buttons::Mouse::Right))
				world.set_voxel_at(ray_result->pos + ray_result->normal, 3);

			ch.draw();
		}

		
		

		window.clearEvents();
			// (Your code calls glfwPollEvents())
			// ...
			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			ImGui::ShowDemoWindow(); // Show demo window! :)

		window.display();

			// Rendering
			// (Your code clears your framebuffer, renders your other stuff etc.)
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			// (Your code calls glfwSwapBuffers() etc.)
	}


	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

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

	float camSpeed{ camera.speed * deltaTime };

	if (window.isKeyPressed(b::Escape))
		window.close();

	if (window.isKeyPressed(b::W))
		camera.pos += camera.front_dir * camSpeed;

	if (window.isKeyPressed(b::S))
		camera.pos -= camera.front_dir * camSpeed;

	if (window.isKeyPressed(b::D))
		camera.pos += camera.front_dir.cross(camera.up_dir).normal() * camSpeed;

	if (window.isKeyPressed(b::A))
		camera.pos -= camera.front_dir.cross(camera.up_dir).normal() * camSpeed;

	if (window.isKeyPressed(b::Left_shift))
		camera.pos -= camera.up_dir * camSpeed;

	if (window.isKeyPressed(b::Space))
		camera.pos += camera.up_dir * camSpeed;

	// if (campos != camera.pos)
	// 	std::cout << "CamPos: " << camera.pos << std::endl;
}

void framebuffersize_callback(GLFWwindow* window, int width, int height) noexcept
{
	glViewport(0, 0, width, height);
}
