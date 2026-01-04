#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdint>

#include "window_and_inputs/window.hpp"
#include "window_and_inputs/inputs.hpp"

#include "rendering/shader.hpp"
#include "mpml/matrices/special_overloads/iostream_matrices.hpp"

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) noexcept;

static void framebuffersize_callback(GLFWwindow* window, int width, int height) noexcept;


int main()
{

	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	
	wai::Window window{ {640, 480}, "Test", nullptr, nullptr };

	
	glfwSetKeyCallback(window.get(), key_callback);

	glfwSetFramebufferSizeCallback(window.get(), framebuffersize_callback);

	
	

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

		shader.use();

		mat4f model{ mpml::Identity4<float> };
		mat4f view{ mpml::Identity4<float> };
		mat4f proj{ mpml::perspective(mpml::Angle::fromDegrees(45), window.getSize().x, window.getSize().y, 0.1f, 100.f, true) };

		model = mpml::rotate<float>(mpml::Angle::fromDegrees(std::sin(mpml::constants::PI_F * glfwGetTime())), { 0.f, 1.f, 0.f });
		  
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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) noexcept
{
	using namespace wai;
	using b = Buttons;

	if (key == b::Escape && action == b::Pressed)
		glfwSetWindowShouldClose(window, true);
}

void framebuffersize_callback(GLFWwindow* window, int width, int height) noexcept
{
	glViewport(0, 0, width, height);
}
