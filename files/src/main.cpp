#include <iostream>
#include <stdexcept>
#include <limits>

#include "mpml/vectors/special_overloads/iostream_vectors.hpp"

#include "window_and_inputs/window.hpp"
#include "window_and_inputs/inputs.hpp"

#include "rendering/shader.hpp"
#include "rendering/utilities/camera.hpp"

#include "gameplay/world/chunk.hpp"
#include "rendering/world_managing/data/chunk/chunkMesh.hpp"
#include "gameplay/world/chunkGrid.hpp"

#include "rendering/assets_managing/texturing/texture.hpp"
#include "window_and_inputs/inputs.hpp"


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

	std::vector<Render::Data::VertexColor> cubeHighlight
	{
		{
			{0, 0, 0,},
			{0.5, 0.2, 0.5}
		},

		{
				{0, 0, 0,},
				{0.5, 0.2, 0.5}
		}
	};

	GLuint vao{};
	glGenVertexArrays(1, &vao);
	GLuint vbo{};
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, cubeHighlight.size() * sizeof(Render::Data::VertexColor), cubeHighlight.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Render::Data::VertexColor), std::bit_cast<void*>(offsetof(Render::Data::VertexColor, pos)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Render::Data::VertexColor), std::bit_cast<void*>(offsetof(Render::Data::VertexColor, color)));
	glEnableVertexAttribArray(1);

	Render::Shader r1{ SHADER_PATH"color.vert", SHADER_PATH"color.frag" };



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

		//model = mpml::translate(model, { 2, 3, -4 });

		shader.setValue("model", model);
		shader.setValue("view", view);
		shader.setValue("proj", proj);




		r1.use();
		r1.setValue("model", model);
		r1.setValue("view", view);
		r1.setValue("proj", proj);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// grid.update(camera.pos);
		//
		// grid.draw_all();



		static vec3f
		ray{},
		step{1},
		nextBound{},
		tMax{},
		tDelta{};

		static float t{};

		if (window.keyPressedOnce(Wai::Buttons::F))
		{
			const auto dir{camera.front_dir};
			ray = camera.pos;
			cubeHighlight.at(0).pos = ray; // DEBUG

			step = vec3f{1,1,1};

			if (dir.x < 0)
				step.x = -1;
			if (dir.y < 0)
				step.y = -1;
			if (dir.z < 0)
				step.z = -1;

			nextBound = vec3f{std::floor(ray.x), std::floor(ray.y), std::floor(ray.z)};

			if (dir.x > 0)
				nextBound.x++;
			if (dir.y > 0)
				nextBound.y++;
			if (dir.z > 0)
				nextBound.z++;


			tMax.x = (nextBound.x - ray.x) / dir.x;
			tMax.y = (nextBound.y - ray.y) / dir.y;
			tMax.z = (nextBound.z - ray.z) / dir.z;

			t = std::min(std::min(tMax.x, tMax.y), tMax.z);

			tDelta.x = 1 / std::abs(dir.x);
			tDelta.y = 1 / std::abs(dir.y);
			tDelta.z = 1 / std::abs(dir.z);

			while (true)
			{
				if (tMax.x < tMax.y)
				{
					if (tMax.x < tMax.z)
					{
						ray.x += step.x;
						tMax.x += tDelta.x;
					}
					else
					{
						ray.z += step.z;
						tMax.z += tDelta.z;
					}
				}
				else
				{
					if (tMax.y < tMax.z)
					{
						ray.y += step.y;
						tMax.y += tDelta.y;
					}
					else
					{
						ray.z += step.z;
						tMax.z += tDelta.z;
					}
				}

				if ((camera.pos - ray).length() >= 50)
					break;

				if (chunk.isWithinChunk(ray))
				{
					if (chunk.break_at(chunk.getLocWithinChunk(ray)))
					{
						chunkm.updateBuffer(chunkm.buildMesh(chunk));
						break;
					}

				}

				cubeHighlight.at(1).pos = ray; // DEBUG
			}
		}




		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, cubeHighlight.size() * sizeof(Render::Data::VertexColor), cubeHighlight.data(), GL_STATIC_DRAW);


		glDisable(GL_CULL_FACE);
		glBindVertexArray(vao);
		glDrawArrays(GL_LINES, 0, cubeHighlight.size());
		//glEnable(GL_CULL_FACE);

		shader.use();
		//
		chunkm.draw();

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
