#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdint>

#include <array>

#include "window_and_inputs/window.hpp"
#include "window_and_inputs/inputs.hpp"

#include "rendering/shader.hpp"
#include "rendering/utilities/camera.hpp"
#include "rendering/world_managing/data/chunk/chunk.hpp"

//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) noexcept;

static void framebuffersize_callback(GLFWwindow* window, int width, int height) noexcept;

static void mouse_callback(GLFWwindow* window, double xpos, double ypos) noexcept;

static void inputs(const Wai::Window& window) noexcept;

render::Camera camera{};

float deltaTime{};





int main()
{

	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_DEBUG, true);

	Wai::Window window{ {640, 480}, "Test", nullptr, nullptr };


	//glfwSetKeyCallback(window.get(), key_callback);

	glfwSetFramebufferSizeCallback(window.get(), framebuffersize_callback);

	glfwSetCursorPosCallback(window.get(), mouse_callback);

	glfwSetInputMode(window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);



	//std::vector<std::uint32_t> indices
	//{
	//	/*Front*/
	//	2, 1, 0, // Left
	//	1, 2, 3, // Right

	//	///*Back*/
	//	//6, 5, 4, // Left
	//	//5, 6, 7, // Right

	//	///*Up*/
	//	//10, 9, 8, // Left
	//	//9, 10, 11, // Right

	//	///*Down*/
	//	//14, 13, 12, // Left
	//	//13, 14, 15, // Right

	//	///*Left*/
	//	//18, 17, 16, // Left
	//	//17, 18, 19, // Right

	//	///*Right*/
	//	//22, 21, 20, // Left
	//	//21, 22, 23, // Right

	//};



	//std::uint32_t gebo{};

	//glGenBuffers(1, &gebo);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gebo);

	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(*indices.data()), indices.data(), GL_STATIC_DRAW);

	render::Shader shader{ SHADER_PATH"shader.vert", SHADER_PATH"shader.frag" };

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);


	render::data::Chunk chunk{ {0,0,0} };

	render::data::Chunk chunk2{ {0, 0, 34} };


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

		//model = mpml::rotate(model, mpml::Angle::fromDegrees(glfwGetTime() * 10), { 0, 0.5, 0 });

		//model = translate(model, { 2, 3, -4 });

		shader.setValue("model", model);
		shader.setValue("view", view);
		shader.setValue("proj", proj);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		chunk.draw();
		chunk2.draw();
			

		window.clearEvents();
		window.display();
	}


	glfwTerminate();

	return 0;
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

	float camSpeed{ 5.f * deltaTime };

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
	
}

void framebuffersize_callback(GLFWwindow* window, int width, int height) noexcept
{
	glViewport(0, 0, width, height);
}
