#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdint>

#include "window_and_inputs/window.hpp"
#include "window_and_inputs/inputs.hpp"

#include "rendering/shader.hpp"

#include "utilities/time/clock.hpp"

//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) noexcept;

static void framebuffersize_callback(GLFWwindow* window, int width, int height) noexcept;

float camSpeed{ 0.5f };
vec3f camFront{ 0.f, 0.f, -1.f };
vec3f camUp{ 0.f, 1.f, 0.f };
vec3f camPos{ 0.f, 0.f, 0.f };

static void mouse_callback(GLFWwindow* window, double xpos, double ypos) noexcept;

static void inputs(const wai::Window& window) noexcept;

game_time::Clock my_clock;

int main()
{

	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	
	wai::Window window{ {640, 480}, "Test", nullptr, nullptr };

	
	//glfwSetKeyCallback(window.get(), key_callback);

	glfwSetFramebufferSizeCallback(window.get(), framebuffersize_callback);

	glfwSetCursorPosCallback(window.get(), mouse_callback);

	glfwSetInputMode(window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	

	std::vector<float> vertices
	{
		0, 0.5, 0,
		-0.5, -0.5, 0,
		0.5, -0.5, 0
	};


	std::uint32_t vbo{};

	glGenBuffers(1, &vbo);

	std::uint32_t vao{};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(*vertices.data()), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(*vertices.data()), nullptr);
	glEnableVertexAttribArray(0);

	render::Shader shader{ SHADER_PATH"shader.vert", SHADER_PATH"shader.frag" };

	glEnable(GL_DEPTH_TEST);


	while (window.isOpen())
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		inputs(window);

		shader.use();

		mat4f model{ mpml::Identity4<float> };
		mat4f view{ mpml::lookAt<float>(camPos, camFront + camPos, camUp) };
		mat4f proj{ mpml::perspective(mpml::Angle::fromDegrees(45), window.getSize().x, window.getSize().y, 0.1f, 100.f, true) };

		model = mpml::rotate(model, mpml::Angle::fromDegrees(glfwGetTime() * 10), { 1, 1, 1 });
		  
		
		model = mpml::translate(model, { 0, 0, -3 });



		shader.setValue("model", model);
		shader.setValue("view", view);
		shader.setValue("proj", proj);

		
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);


		window.display();
		window.clearEvents();
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

	direction.x = -(std::cos(radYaw) * cosPitch);
	direction.y = std::sin(radPitch);
	direction.z = std::sin(radYaw) * cosPitch;

	camFront = direction.normal();
}

#include <iostream>
void inputs(const wai::Window& window) noexcept
{
	using namespace wai;
	using b = Buttons;

	if (window.keyPressed(b::Escape))
		window.close();

	if (window.keyPressed(b::Up))
		std::cout << my_clock.asSeconds();

	if (window.keyPressed(b::Down))
		my_clock.start();

	/*if (window.keyPressed(b::Right))
		camPos += camFront.cross(camUp) * camSpeed;

	if (window.keyPressed(b::Left))
		camPos += -camFront.cross(camUp) * camSpeed;*/
}

void framebuffersize_callback(GLFWwindow* window, int width, int height) noexcept
{
	glViewport(0, 0, width, height);
}
