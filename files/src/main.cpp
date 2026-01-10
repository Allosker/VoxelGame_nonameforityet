#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdint>

#include <array>

#include "window_and_inputs/window.hpp"
#include "window_and_inputs/inputs.hpp"

#include <mpml/vectors/special_overloads/iostream_vectors.hpp>

#include "rendering/shader.hpp"
#include "rendering/utilities/camera.hpp"

#include "utilities/time/clock.hpp"

//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) noexcept;

static void framebuffersize_callback(GLFWwindow* window, int width, int height) noexcept;

static void mouse_callback(GLFWwindow* window, double xpos, double ypos) noexcept;

static void inputs(const Wai::Window& window) noexcept;

render::Camera camera{};

float deltaTime{};


struct Vertex
{
	vec3f pos{};
	vec3f color{};
	vec3f normal{};
};

struct Faces
{
	/*Faces*/
	std::array<Vertex, 4> front
	{
		/*Front*/
		Vertex
		{{-0.5, 0, 0},  /*Left-Down*/  {0.1, 0.1, 0.1}},
		{{-0.5, 1, 0},  /*Left-Up*/    {0.1, 0.1, 0.1}},
		{{ 0.5, 0, 0},  /*Right-Down*/ {0.1, 0.1, 0.1}},
		{{ 0.5, 1, 0},  /*Right-Up*/   {0.1, 0.1, 0.1}}
	};

	std::array<Vertex, 4> back
	{
		/*Back*/
		Vertex
		{{ 0.5, 0, -1},  /*Left-Down*/  {0.2, 0.2, 0.2}},
		{{ 0.5, 1, -1},  /*Left-Up*/    {0.2, 0.2, 0.2}},
		{{-0.5, 0, -1},  /*Right-Down*/ {0.2, 0.2, 0.2}},
		{{-0.5, 1, -1},  /*Right-Up*/   {0.2, 0.2, 0.2}},
	};

	std::array<Vertex, 4> up
	{
		/*Up*/
		Vertex
		{{ 0.5, 1, 0},   /*Left-Down*/  {0.3, 0.3, 0.3}},
		{{-0.5, 1, 0},   /*Left-Up*/    {0.3, 0.3, 0.3}},
		{{ 0.5, 1, -1},  /*Right-Down*/ {0.3, 0.3, 0.3}},
		{{-0.5, 1, -1},  /*Right-Up*/   {0.3, 0.3, 0.3}},
	};

	std::array<Vertex, 4> down
	{
		/*Down*/
		Vertex
		{{ 0.5, 0, -1},  /*Right-Down*/ {0.4, 0.4, 0.4}},
		{{-0.5, 0, -1},  /*Right-Up*/   {0.4, 0.4, 0.4}},
		{{ 0.5, 0, 0},   /*Left-Down*/  {0.4, 0.4, 0.4}},
		{{-0.5, 0, 0},   /*Left-Up*/    {0.4, 0.4, 0.4}},
	};

	std::array<Vertex, 4> left
	{
		/*Left*/
		Vertex
		{{ 0.5, 0, 0},   /*Right-Down*/ {0.5, 0.5, 0.5}},
		{{ 0.5, 1, 0},   /*Right-Up*/   {0.5, 0.5, 0.5}},
		{{ 0.5, 0, -1},  /*Left-Down*/  {0.5, 0.5, 0.5}},
		{{ 0.5, 1, -1},  /*Left-Up*/    {0.5, 0.5, 0.5}},
	};

	std::array<Vertex, 4> right
	{
		/*Right*/
		Vertex
		{{-0.5, 0, -1},  /*Right-Down*/ {0.6, 0.6, 0.6}},
		{{-0.5, 1, -1},  /*Right-Up*/   {0.6, 0.6, 0.6}},
		{{-0.5, 0, 0},   /*Left-Down*/  {0.6, 0.6, 0.6}},
		{{-0.5, 1, 0},   /*Left-Up*/    {0.6, 0.6, 0.6}},
	};

	const float* begin() const noexcept
	{
		return &front[0].pos[0];
	}

	float* begin() noexcept
	{
		return &front[0].pos[0];
	}

	const float* end() const noexcept
	{
		return &right[3].normal[3];
	}

	float* end() noexcept
	{
		return &right[3].normal[3];
	}
};

struct Cube
{
	enum Type
		: std::uint8_t
	{
		Full,
		Empty,
		Transparent,
	};

	vec3f pos{};

	Type type{};
};

class Chunk
{
public:



	const Vertex* cubeData() const noexcept
	{
		return m_meshes.data();
	}

private:

	std::array<Cube, 32'768> m_cube_data{};
	std::vector<Vertex> m_meshes{};

	std::uint32_t m_vbo_id{};

};

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

	Faces faces{};

	Cube cube{};

	std::vector<std::uint32_t> indices
	{
		/*Front*/
		2, 1, 0, // Left
		1, 2, 3, // Right

		/*Back*/
		6, 5, 4, // Left
		5, 6, 7, // Right

		/*Up*/
		10, 9, 8, // Left
		9, 10, 11, // Right

		/*Down*/
		14, 13, 12, // Left
		13, 14, 15, // Right

		/*Left*/
		18, 17, 16, // Left
		17, 18, 19, // Right

		/*Right*/
		22, 21, 20, // Left
		21, 22, 23, // Right

	};


	std::uint32_t vbo{};

	glGenBuffers(1, &vbo);

	std::uint32_t vao{};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Cube), faces.begin(), GL_STATIC_DRAW);

	std::uint32_t ebo{};

	glGenBuffers(1, &ebo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(*indices.data()), indices.data(), GL_STATIC_DRAW);



	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(sizeof(vec3f)));
	glEnableVertexAttribArray(1);


	render::Shader shader{ SHADER_PATH"shader.vert", SHADER_PATH"shader.frag" };

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	std::vector<vec3f> trans{};

	for (int i{}; i < 100; i++)
	{
		for (int j{}; j < 100; j++)
		{
			for (int k{}; k < 100; k++)
				trans.push_back({ (float)i, (float)k, (float)j});
		}
	}

	std::uint32_t ivbo{};
	glGenBuffers(1, &ivbo);
	glBindBuffer(GL_ARRAY_BUFFER, ivbo); 

	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3f) * trans.size(), trans.data(), GL_STATIC_DRAW);


	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vec3f), nullptr);
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


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
		mat4f proj{ mpml::perspective(mpml::Angle::fromDegrees(45), window.getSize().x, window.getSize().y, 0.1f, 100.f) };

		//model = mpml::rotate(model, mpml::Angle::fromDegrees(glfwGetTime() * 10), { 0, 0.5, 0 });

		//model = translate(model, { 2, 3, -4 });

		shader.setValue("model", model);
		shader.setValue("view", view);
		shader.setValue("proj", proj);

		
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr, trans.size());

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
