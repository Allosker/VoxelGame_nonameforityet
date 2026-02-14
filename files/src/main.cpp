#include <iostream>
#include <stdexcept>
#include <map>

#include "mpml/vectors/special_overloads/iostream_vectors.hpp"

#include "window_and_inputs/window.hpp"
#include "window_and_inputs/inputs.hpp"

#include "rendering/shader.hpp"
#include "rendering/utilities/camera.hpp"

#include "gameplay/world/chunk.hpp"
#include "rendering/world_managing/data/chunk/chunkMesh.hpp"
#include "gameplay/world/chunkGrid.hpp"

#include "rendering/assets_managing/texturing/texture.hpp"


//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) noexcept;

static void framebuffersize_callback(GLFWwindow* window, int width, int height) noexcept;

static void mouse_callback(GLFWwindow* window, double xpos, double ypos) noexcept;

static void inputs(const Wai::Window& window) noexcept;

Render::Camera camera{};

float deltaTime{};



int main()
try
{

	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_DEBUG, true);

	Wai::Window window{ {640, 480}, "Test", nullptr, nullptr };


	//glfwSetKeyCallback(window.get(), key_callback);

	glfwSetFramebufferSizeCallback(window.get(), framebuffersize_callback);

	glfwSetCursorPosCallback(window.get(), mouse_callback);

	glfwSetInputMode(window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);



	Render::Shader shader{ SHADER_PATH"shader.vert", SHADER_PATH"shader.frag" };

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	Render::Texturing::Texture texture{ ASSET_PATH"atlas.png"};

	texture.bind();

	std::vector<Render::Data::ChunkMesh> chunkms;

	Gameplay::World::Chunk chunk{ {0,0,0} };
	Render::Data::ChunkMesh chunkm{ chunk };

	//Gameplay::World::ChunkGrid grid{};

	std::vector<vec3f> cubeHighlight
	{
		{
			0, 0.5, 0,
			-0.5, 0, 0,
			0.5, 0, 0
		}
	};

	GLuint vao{};
	glGenVertexArrays(1, &vao);
	GLuint vbo{};
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, cubeHighlight.size() * sizeof(vec3f), cubeHighlight.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vec3f), nullptr);
	glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, false, 2 * sizeof(vec3f), reinterpret_cast<void*>(sizeof(vec3f)));
	//glEnableVertexAttribArray(1);

	Render::Shader cubehigh_shader{ SHADER_PATH"color.vert", SHADER_PATH"color.frag" };




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

		cubehigh_shader.use();
		cubehigh_shader.setValue("model", model);
		cubehigh_shader.setValue("view", view);
		cubehigh_shader.setValue("proj", proj);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//grid.update(camera.pos);

		//grid.draw_all();

		static vec3f ray{};
		static vec3f camDir{};

		/*ray = camera.pos;
		camDir = camera.front_dir;

		if (chunk.isWithinChunk(ray))
			if (window.keyPressed(Wai::Buttons::F))
				if (chunk.break_at(chunk.getLocWithinChunk(ray)))
					chunkm.updateBuffer(chunkm.buildMesh(chunk));

		ray += camDir;
			  */

		glDisable(GL_CULL_FACE);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, cubeHighlight.size());

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
	
}

void framebuffersize_callback(GLFWwindow* window, int width, int height) noexcept
{
	glViewport(0, 0, width, height);
}
