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

	Vertex(const vec3f& vec1, const vec3f& vec2, const vec3f& vec3 = {})
		: m_data{ vec1, vec2, vec3 }
	{
	}

	union
	{
		vec3f pos;
		vec3f color;
		vec3f normal;

		std::array<vec3f, 3> m_data;
	};

	const float* data() const noexcept
	{
		return m_data[0].data_ptr();
	}

	float* data() noexcept
	{
		return m_data[0].data_ptr();
	}
};

struct Faces
{
		std::array<Vertex, 24> m_data
		{
			/*Front*/
			Vertex
			{{-0.5, 0, 0},  /*Left-Down*/  {0.1, 0.1, 0.1}},
			{{-0.5, 1, 0},  /*Left-Up*/    {0.1, 0.1, 0.1}},
			{{ 0.5, 0, 0},  /*Right-Down*/ {0.1, 0.1, 0.1}},
			{{ 0.5, 1, 0},  /*Right-Up*/   {0.1, 0.1, 0.1}},

			/*Back*/
			Vertex
			{{ 0.5, 0, -1},  /*Left-Down*/  {0.2, 0.2, 0.2}},
			{{ 0.5, 1, -1},  /*Left-Up*/    {0.2, 0.2, 0.2}},
			{{-0.5, 0, -1},  /*Right-Down*/ {0.2, 0.2, 0.2}},
			{{-0.5, 1, -1},  /*Right-Up*/   {0.2, 0.2, 0.2}},

			/*Up*/
			Vertex
			{{ 0.5, 1, 0},   /*Left-Down*/  {0.3, 0.3, 0.3}},
			{{-0.5, 1, 0},   /*Left-Up*/    {0.3, 0.3, 0.3}},
			{{ 0.5, 1, -1},  /*Right-Down*/ {0.3, 0.3, 0.3}},
			{{-0.5, 1, -1},  /*Right-Up*/   {0.3, 0.3, 0.3}},

			/*Down*/
			Vertex
			{{ 0.5, 0, -1},  /*Right-Down*/ {0.4, 0.4, 0.4}},
			{{-0.5, 0, -1},  /*Right-Up*/   {0.4, 0.4, 0.4}},
			{{ 0.5, 0, 0},   /*Left-Down*/  {0.4, 0.4, 0.4}},
			{{-0.5, 0, 0},   /*Left-Up*/    {0.4, 0.4, 0.4}},

			/*Left*/
			Vertex
			{{ 0.5, 0, 0},   /*Right-Down*/ {0.5, 0.5, 0.5}},
			{{ 0.5, 1, 0},   /*Right-Up*/   {0.5, 0.5, 0.5}},
			{{ 0.5, 0, -1},  /*Left-Down*/  {0.5, 0.5, 0.5}},
			{{ 0.5, 1, -1},  /*Left-Up*/    {0.5, 0.5, 0.5}},

			/*Right*/
			Vertex
			{{-0.5, 0, -1},  /*Right-Down*/ {0.6, 0.6, 0.6}},
			{{-0.5, 1, -1},  /*Right-Up*/   {0.6, 0.6, 0.6}},
			{{-0.5, 0, 0},   /*Left-Down*/  {0.6, 0.6, 0.6}},
			{{-0.5, 1, 0},   /*Left-Up*/    {0.6, 0.6, 0.6}},
		};

	const float* data() const noexcept
	{
		return m_data[0].data();
	}

	float* data() noexcept
	{
		return m_data[0].data();
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

	Chunk(std::vector<std::array<Vertex, 4>> gfaces, GLuint gebo, const vec3f& pos={})
	{
		m_pos = pos;

		// Create VAO for chunk
		glGenVertexArrays(1, &m_vao_id);
		glBindVertexArray(m_vao_id);

		// Create VBO for cube translations
		glGenBuffers(1, &m_vbo_id);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);

		fill();
		

		// Bind Global iVBO -> Faces for each Cube
		GLuint vbo_faces{};

		glGenBuffers(1, &vbo_faces);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_faces);

		std::int32_t dimensions{ 32 };
		std::int32_t dMax{ dimensions * dimensions * dimensions };


		for (std::int64_t x{}; x < dimensions; x++)
		{
			for (std::int64_t y{}; y < dimensions; y++)
			{
				for (std::int64_t z{}; z < dimensions; z++)
				{

					typedef Cube::Type t;

					std::array<t, 6> faces{};


					if (z + 1 < 32)
						faces[0] = m_cubes[((z + 1) * dimensions * dimensions) + (y * dimensions) + x];
					else
						faces[0] = t::Empty;

					if(z - 1 >= 0)
						faces[1] = m_cubes[((z - 1) * dimensions * dimensions) + (y * dimensions) + x];
					else
						faces[1] = t::Empty;


					if (y + 1 < 32)
						faces[2] = m_cubes[(z * dimensions * dimensions) + ((y + 1) *dimensions) + x];
					else
						faces[2] = t::Empty;

					if (y - 1 >= 0)
						faces[3] = m_cubes[(z * dimensions * dimensions) + ((y - 1) * dimensions) + x];
					else
						faces[3] = t::Empty;


					if (x + 1 < 32)
						faces[4] = m_cubes[(z * dimensions * dimensions) + (y * dimensions) + x + 1];
					else
						faces[4] = t::Empty;

					if (x - 1 >= 0)
						faces[5] = m_cubes[(z * dimensions * dimensions) + (y * dimensions) + x - 1];
					else
						faces[5] = t::Empty;
					

					
					if (faces[0] == t::Empty)
					{
						m_trans.push_back({ x + m_pos.x, y + m_pos.y, z + m_pos.z });
						m_meshes.push_back(gfaces[0]);
					}

					if (faces[1] == t::Empty)
					{
						m_trans.push_back({ x + m_pos.x, y + m_pos.y, z + m_pos.z + 1 });
						m_meshes.push_back(gfaces[1]);
					}

					if (faces[2] == t::Empty)
					{
						m_trans.push_back({ x + m_pos.x, y + m_pos.y, z + m_pos.z });
						m_meshes.push_back(gfaces[2]);
					}

					if (faces[3] == t::Empty)
					{
						m_trans.push_back({ x + m_pos.x, y + m_pos.y + 1, z + m_pos.z });
						m_meshes.push_back(gfaces[3]);
					}

					if (faces[4] == t::Empty)
					{
						m_trans.push_back({ x + m_pos.x, y + m_pos.y, z + m_pos.z });
						m_meshes.push_back(gfaces[4]);
					}

					if (faces[5] == t::Empty)
					{
						m_trans.push_back({ x + m_pos.x + 1, y + m_pos.y, z + m_pos.z });
						m_meshes.push_back(gfaces[5]);
					}
				}
			}

		}

		for (size_t i{}; i < m_meshes.size(); i++)
		{
			m_meshes[i][0].pos += m_trans[i];
			m_meshes[i][1].pos += m_trans[i];
			m_meshes[i][2].pos += m_trans[i];
			m_meshes[i][3].pos += m_trans[i];
		}

		//glBufferData(GL_ARRAY_BUFFER, m_trans.size() * sizeof(vec3f), m_trans.data(), GL_STATIC_DRAW);

		//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vec3f), nullptr);
		//glEnableVertexAttribArray(2);
		//glVertexAttribDivisor(2, 1);

		glBufferData(GL_ARRAY_BUFFER, m_meshes.size() * (sizeof(Vertex) * 4), m_meshes.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(sizeof(vec3f)));
		glEnableVertexAttribArray(1);

		// Bind Global gEBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gebo);
	}

	void bind()
	{
		glBindVertexArray(m_vao_id);
	}

	size_t getNumberTranslations()
	{
		return m_trans.size();
	}

	void fill()
	{
		for (int i{}; i < 32; i++)
		{
			for (int j{}; j < 32; j++)
			{
				for (int k{}; k < 32; k++)
				{
					m_cubes.push_back(Cube::Type::Empty);
				}
			}
		}
	}

private:

	std::vector<vec3f> m_trans{};
	std::vector<Cube::Type> m_cubes{};
	std::vector<std::array<Vertex, 4>> m_meshes{};

	GLuint m_vbo_id{};
	GLuint m_vao_id{};

	vec3f m_pos{};
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


	

	Faces faces{};
	std::vector<std::array<Vertex, 4>> cube_faces{};

	for (size_t i{}; i < faces.m_data.size(); i += 4)
	{
		cube_faces.push_back({ faces.m_data[i], faces.m_data[i + 1], faces.m_data[i + 2], faces.m_data[i + 3] });
	}


	std::uint32_t gebo{};

	glGenBuffers(1, &gebo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gebo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(*indices.data()), indices.data(), GL_STATIC_DRAW);


	Chunk chunk{ cube_faces, gebo, {0, 0, 0} };

	Chunk chunk2{ cube_faces, gebo, {0, 0, 40} };


	render::Shader shader{ SHADER_PATH"shader.vert", SHADER_PATH"shader.frag" };

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);


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

		

			chunk.bind();
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

			/*chunk2.bind();
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);*/

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

	float camSpeed{ 25.f * deltaTime };

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
