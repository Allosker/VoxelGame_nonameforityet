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

#include "rendering/utilities/cubeHighlight.hpp"

#include "rendering/mesh/mesh.hpp"

/* TODOLIST
* 
*	== Look through world managing to set the height of cubes (do that at first with sine function) and then you're gonne go through perlin noise mate
*
*/


#include "uHeaders/debug.hpp"


//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) noexcept;

static void framebuffersize_callback(GLFWwindow* window, int width, int height) noexcept;

static void mouse_callback(GLFWwindow* window, double xpos, double ypos) noexcept;

Render::Utils::Camera camera{};

float deltaTime{};


struct WorldOptions
{
	uint64 selected_voxel{ 1 };

	uint64 rayDist{ 5 };
	int64 rayDist_min{ 1 };
	int64 rayDist_max{ 2000 };

	double y_minMin{ -2000 };
	double y_minMax{ 10 };

	bool instant_voxel_breaking{ false }, instant_voxel_placing{ false };

	uint64 rm{ 0 }, rma{ 8 };
};


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

		


	//glfwSetKeyCallback(window.get(), key_callback);

	glfwSetFramebufferSizeCallback(window.get(), framebuffersize_callback);

	glfwSetCursorPosCallback(window.get(), mouse_callback);

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window.get(), true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

	Render::Shader shader{ SHADER_PATH"shader.vert", SHADER_PATH"shader.frag" };

	Render::Shader cubeDisplay{ SHADER_PATH"shader.vert", SHADER_PATH"shader.frag" };

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_MULTISAMPLE);

	Render::Texturing::Texture texture{ ASSET_PATH"atlas.png"};

	texture.bind();



	Render::Utils::CubeHighlight ch;

	GameWorld::World world{};

	
	WorldOptions WO{};


	std::vector<vec3f> pos
	{
		// Position					   // Colors
		vec3f
		{ 0, 0, 1 },  /*Left-Down*/
		{ 1, 0, 1 },  /*Right-Down*/
		{ 0, 1, 1 },  /*Left-Up*/

		{ 1, 0, 1 },  /*Right-Down*/
		{ 1, 1, 1 },  /*Right-Up*/
		{ 0, 1, 1 },  /*Left-Up*/

			/*Back*/
		{ 1, 0, 0 },  /*Left-Down*/
		{ 0, 0, 0 },  /*Right-Down*/
		{ 1, 1, 0 },  /*Left-Up*/

		{ 0, 0, 0 },  /*Right-Down*/
		{ 0, 1, 0 },  /*Right-Up*/
		{ 1, 1, 0 },  /*Left-Up*/


			/*Up*/
		{ 1, 1, 1 },   /*Left-Down*/
		{ 1, 1, 0 },  /*Right-Down*/
		{ 0, 1, 1 },   /*Left-Up*/

		{ 1, 1, 0 },  /*Right-Down*/
		{ 0, 1, 0 },  /*Right-Up*/
		{ 0, 1,  1 },   /*Left-Up*/

			/*Down*/
		{ 1, 0, 0 },  /*Left-Down*/
		{ 1, 0, 1 },  /*Right-Down*/
		{ 0, 0, 0 },   /*Left-Up*/

		{ 1, 0, 1 },  /*Right-Down*/
		{ 0, 0, 1 },   /*Right-Up*/
		{ 0, 0, 0 },   /*Left-Up*/


			/*Left*/
		{ 1, 0, 1 },  /*Left-Down*/
		{ 1, 0, 0 },   /*Right-Down*/
		{ 1, 1, 1 },  /*Left-Up*/

		{ 1, 0, 0 },   /*Right-Down*/
		{ 1, 1, 0 },   /*Right-Up*/
		{ 1, 1, 1 },  /*Left-Up*/


			/*Right*/
		{ 0, 0, 0 },  /*Right-Down*/
		{ 0, 0, 1 },   /*Left-Down*/
		{ 0, 1, 0 },  /*Right-Up*/

		{ 0, 0, 1 },   /*Left-Up*/
		{ 0, 1, 1 },  /*Right-Up*/
		{ 0, 1, 0 },   /*Left-Down*/
	};

	std::vector<vec2f> uvs{};

	for (const auto& i : Render::mapTextureUVs_6(Render::Data::Types::TextureUVperFace::c_deepStone))
	{
		for (const auto& j : i)
			uvs.push_back(j);
	}

	std::vector<Render::Data::Vertex> vertices{};


	Render::Mesh mesh{ pos, uvs };

	Render::Mesh cbpos{ std::vector<Render::Data::VertexColor>{ { {0, 0, 0}, {0,0,0} }, {{100, 100, 100}, {0,0,0}} } };
	Render::Shader cbshader{ SHADER_PATH"color.vert", SHADER_PATH"color.frag" };
	
	camera.pos = 100;
	
	float lastFrame{};
	while (window.isOpen())
	{
		deltaTime = glfwGetTime() - lastFrame;
		lastFrame = glfwGetTime();

		window.clearEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		

		window.processInputs(
			[&]()
			{
				using namespace Wai;
				using b = Buttons;

				if (window.isKeyPressed(b::Escape))
					window.close();
			}
		);

		static const auto& move{ [&]()
			{
				using namespace Wai;
				using b = Buttons;

				float camSpeed{ camera.speed * deltaTime };

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
			} };

		
		{
			ImGui::Begin("Debug"); // Window beginning
			
			ImGui::Text("Camera Pos: %f %f %f", camera.pos.x, camera.pos.y, camera.pos.z);

			const GameWorld::Voxels::Chunk* cl{ world.chunk_at(camera.pos) };

			if(cl)
			{
				const types::loc c{ GameWorld::Voxels::ChunkGrid::to_loc(cl->getPos()) };

				ImGui::Text("Chunk Loc : %ld %ld %ld", c.x, c.y, c.z);
			}

					ImGui::SliderFloat("Camera Speed", &camera.speed, 0.0f, 100.0f);
			ImGui::SliderScalar("Ray Distance", ImGuiDataType_U64, &WO.rayDist, &WO.rayDist_min, &WO.rayDist_max);

			ImGui::Checkbox("Instant Vox. Break", &WO.instant_voxel_breaking);
			ImGui::Checkbox("Instant Vox. Place", &WO.instant_voxel_placing);

			ImGui::VSliderScalar("Render Distance", { 15, 100 }, ImGuiDataType_S64, &GameWorld::Voxels::ChunkSettings::world_render_distance, &WO.rm, &WO.rma);
			ImGui::VSliderScalar("World Depth", { 15, 100 }, ImGuiDataType_::ImGuiDataType_Double, &world.y_min, &WO.y_minMin, &WO.y_minMax);

			ImGui::End(); // Window end
		}


		if(window.isKeyPressedOnce(Wai::Buttons::F))
		{
			static bool hiddenCursor{ true };
			hiddenCursor = !hiddenCursor;

			glfwSetInputMode(window.get(), GLFW_CURSOR, hiddenCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
		}

		

		shader.use();

		mat4f model{ mpml::Identity4<float> };
		mat4f view{ mpml::lookAt(camera.pos, camera.front_dir + camera.pos, camera.up_dir) };
		mat4f proj{ mpml::perspective(mpml::Angle::fromDegrees(90), window.getSize().x, window.getSize().y, 0.1f, 1000.f) };

		shader.setValue("model", model);
		shader.setValue("view", view);
		shader.setValue("proj", proj);

		cbshader.use();
		cbshader.setValue("model", model);
		cbshader.setValue("view", view);
		cbshader.setValue("proj", proj);

		mat4f model_cubeDisplay{ mpml::Identity4<float> };

		model_cubeDisplay = mpml::translate(model_cubeDisplay, { -0.5, -0.5, -0.5 });
		model_cubeDisplay = mpml::scale(model_cubeDisplay, 0.5f);
		model_cubeDisplay = mpml::rotate(model_cubeDisplay, mpml::Angle::fromDegrees(5.f), vec3f{ 0, 0, 1 });
		float temp{ static_cast<float>(std::sin(glfwGetTime())) };
		model_cubeDisplay = mpml::rotate(model_cubeDisplay, mpml::Angle::fromDegrees((temp > 1 ? temp : temp + 1.f) * 240.f), vec3f{ 0, 1, 0 });
		

		model_cubeDisplay = mpml::translate(model_cubeDisplay, { 4.5, -4.5, -2 });


		cubeDisplay.use();
		cubeDisplay.setValue("model", model_cubeDisplay);
		cubeDisplay.setValue("view", mpml::Identity4<float>);

		cubeDisplay.setValue("proj", mpml::orthographic_projection(10u, 10u, 0.1f, 100.f));

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		   
	
		world.update(camera.pos);

		

		const auto& ray_result{ GameWorld::Voxels::Utils::raycast(camera.pos, camera.front_dir, world.grid, WO.rayDist, world.getTypeManager()) };

		bool drawHighlight{ true };
		if (ray_result)
		{

				//ch.update(model, view, proj, ray_result->pos);



				if (window.isMouseButtonPressedOnce(Wai::Buttons::Mouse::Middle))
				{
					auto lambda = [&]()
						{
							std::vector<vec2f> b_uvs{};

							for (const auto& i : world.getTypeManager().getType(world.block_at(ray_result->pos)->id).uvs)
							{

								b_uvs.push_back(i[0]);
								b_uvs.push_back(i[1]);
								b_uvs.push_back(i[2]);
								b_uvs.push_back(i[1]);
								b_uvs.push_back(i[3]);
								b_uvs.push_back(i[2]);
							}

							return b_uvs;
						};

					WO.selected_voxel = world.block_at(ray_result->pos)->id;
					mesh.updateBuffer(pos, lambda());
				}

				if (!WO.instant_voxel_breaking)
				{
					if (window.isMouseButtonPressedOnce(Wai::Buttons::Mouse::Left))
						world.set_voxel_at(ray_result->pos, 0, camera.pos);
				}
				else
				{
					if (window.isMouseButtonPressed(Wai::Buttons::Mouse::Left))
						world.set_voxel_at(ray_result->pos, 0, camera.pos);
				}

				if (!WO.instant_voxel_placing)
				{
					if (window.isMouseButtonPressedOnce(Wai::Buttons::Mouse::Right))
						world.set_voxel_at(ray_result->pos + ray_result->normal, WO.selected_voxel, camera.pos);
				}
				else
				{
					if (window.isMouseButtonPressed(Wai::Buttons::Mouse::Right))
						world.set_voxel_at(ray_result->pos + ray_result->normal, WO.selected_voxel, camera.pos);
				}

				drawHighlight = true;
		}


		struct BoundingBox
		{
			bool intersect(const BoundingBox& outer)
			{
				return
					min.x <= outer.max.x && max.x >= outer.min.x && 

					min.y <= outer.max.y && max.y >= outer.min.y &&

					min.z <= outer.max.z && max.z >= outer.min.z;
			}

			vec3f findIntersection(const BoundingBox& outer, const GameWorld::World& world, const types::loc& block_loc)
			{
				vec3f right	{ max.x - outer.min.x, 0, 0 };
				vec3f down	{ 0, max.y - outer.min.y, 0 };
				vec3f back	{ 0, 0, max.z - outer.min.z };

				vec3f left	{ outer.max.x - min.x, 0, 0 };
				vec3f up	{ 0, outer.max.y - min.y, 0 };
				vec3f front	{ 0, 0, outer.max.z - min.z };

				vec3f result{};
				float bestDist{-1};

				if (left.x > 0 && (left.x < bestDist || bestDist < 0))
				{
					if (!world.block_at(block_loc + types::loc{ 1, 0, 0 }))
					{
						result = -left;
						bestDist = left.x;
					}
				}

				if (right.x > 0 && (right.x < bestDist || bestDist < 0))
				{
					if (!world.block_at(block_loc + types::loc{ -1, 0, 0 }))
					{
						result = right;
						bestDist = right.x;
					}
				}


				if (up.y > 0 && (up.y < bestDist || bestDist < 0))
				{
					if (!world.block_at(block_loc + types::loc{ 0, 1, 0 }))
					{
						result = -up;
						bestDist = up.y;
					}
				}

				if (down.y > 0 && (down.y < bestDist || bestDist < 0))
				{
					if (!world.block_at(block_loc + types::loc{ 0, -1, 0 }))
					{
						result = down;
						bestDist = down.y;
					}
				}


				if (front.z > 0 && (front.z < bestDist || bestDist < 0))
				{
					if (!world.block_at(block_loc + types::loc{ 0, 0, 1 }))
					{
						result = -front;
						bestDist = front.z;
					}
				}

				if (back.z > 0 && (back.z < bestDist || bestDist < 0))
				{
					if (!world.block_at(block_loc + types::loc{ 0, 0, -1 }))
					{
						result = back;
						bestDist = back.z;
					}
				}


				return result;
			}

			vec3f min{};
			vec3f max{};
		};

		BoundingBox cam{ camera.pos - vec3f{1}, camera.pos + vec3f{1} };

		std::vector<types::loc> camPoss;

		move();

		const auto camFlooredMin{ mpml::floor(cam.min) };
		const auto camFlooredMax{ mpml::floor(cam.max) };

		for (int x{ (int)camFlooredMin.x }; x <= camFlooredMax.x; x++)
			for (int y{ (int)camFlooredMin.y }; y <= camFlooredMax.y; y++)
				for (int z{ (int)camFlooredMin.z }; z <= camFlooredMax.z; z++)
					camPoss.emplace_back(vec3i{ x, y, z });

		for (const auto& pos : camPoss)
			if(const auto* cubeptr{ world.block_at(pos) }; cubeptr && cubeptr->id)
			{
				BoundingBox cube{ pos, pos + types::loc{1} };

				if(cam.intersect(cube))
				{

					camera.pos -= cam.findIntersection(cube, world, pos);

					cam = BoundingBox{ camera.pos - vec3f{1}, camera.pos + vec3f{1} };
				}
			}
			
		

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		world.draw_chunkGrid(GameWorld::Voxels::ChunkGrid::to_loc(camera.pos));

		glDisable(GL_DEPTH_TEST);
		cubeDisplay.use();
		mesh.draw();

		cbshader.use();
		cbpos.draw(GL_LINE_STRIP);
		glEnable(GL_DEPTH_TEST);

		ch.useShader();
		if(drawHighlight)
			ch.draw();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		window.display();
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

void framebuffersize_callback(GLFWwindow* window, int width, int height) noexcept
{
	static_cast<Wai::Window*>(glfwGetWindowUserPointer(window))->onFramebufferResize({ width, height });
}
