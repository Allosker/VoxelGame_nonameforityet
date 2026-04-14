#include <iostream>
#include <stdexcept>
#include <limits>

#include "mpml/vectors/special_overloads/print_vectors.hpp"

#include "window_and_inputs/window.hpp"
#include "window_and_inputs/inputs.hpp"

#include "rendering/shader.hpp"

// Block Hitting
#include "world/voxels/utilities/ray.hpp"
#include <utility>
//

#include "world/world.hpp"

#include "rendering/assets_managing/texturing/texture.hpp"

#include "rendering/utilities/cubeHighlight.hpp"

#include "rendering/mesh/mesh.hpp"

#include "rendering/GUI/shapes/rectangle.hpp"

/* TODOLIST
* 
*	== Fix it when you go onto a block from a certain direction, you get teleported on the right or left. Only if there is an empty block on the left/right down of it.
*
*/


#include "uHeaders/debug.hpp"

#include "physics/collisions/basicHitbox.hpp"
#include "world/players/player/player.hpp"

#include "rendering/GUI/Items/itemRenderers.hpp"
#include "rendering/GUI/Items/itemTypeManager.hpp"

#include <fstream>


constexpr float DESIGN_WIDTH{ 1920.f };
constexpr float DESIGN_HEIGHT{ 1080.f };

class Hotbar
{
public: 

	Hotbar()
	{
	}


	const vec2f& getSlotPosition_at(std::size_t index)
	{
		return m_slots[index].first;
	}


public:


	static constexpr vec2f slot_size{ 145, 144 };

private:

	std::array<std::pair<vec2f, bool>, 10> m_slots
	{ 
		std::pair<vec2f, bool>
		{{93 - DESIGN_WIDTH / 2, 254 - DESIGN_HEIGHT / 2}, true},
		{{1'827 - DESIGN_WIDTH / 2, 254 - DESIGN_HEIGHT / 2}, true}
	};

};


static void framebuffersize_callback(GLFWwindow* window, int width, int height) noexcept;

static void mouse_callback(GLFWwindow* window, double xpos, double ypos) noexcept;


GameWorld::Player player{};

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


inline bool mouseCanMoveEverything{};
inline bool G_WINDOW_WAS_RESIZED{false};

int main()
try
{

	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_DEBUG, true);

	//glfwWindowHint(GLFW_SAMPLES, 4);

	Wai::Window window{ {640, 480}, "Test", nullptr, nullptr };

	GLint flags{};
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}

	// Example from gettingn started
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImPlot::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

		


	//glfwSetKeyCallback(window.get(), key_callback);

	glfwSetFramebufferSizeCallback(window.get(), framebuffersize_callback);

	glfwSetCursorPosCallback(window.get(), mouse_callback);

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window.get(), true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

	Render::Shader shader3Dworld{ SHADER_PATH"shader.vert", SHADER_PATH"shader.frag" };

	Render::Shader shaderCubeDisplay{ SHADER_PATH"shader.vert", SHADER_PATH"shader.frag" };

	Render::Shader shader2Drectangle{ SHADER_PATH"meshTexture2D.vert", SHADER_PATH"meshTexture2D.frag" };


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_MULTISAMPLE);

	Render::Texturing::Texture textureAtlas{ ASSET_PATH"atlas.png"};

	Render::Texturing::Texture crossAirAtlas{ ASSET_PATH"crossair_atlas.png" };

	Render::Texturing::Texture textureBlockInventoryAtlas{ ASSET_PATH"block_inventory_atlas.png" };

	Render::Texturing::Texture textureInventoryAtlas{ ASSET_PATH"inventory.png" };



	Render::Utils::CubeHighlight ch;

	GameWorld::World world{};

	
	WorldOptions WO{};


	Hotbar hotbar_obj{};

	Render::GUI::ItemTypeManager itemManager{};

	Render::GUI::ItemGUI item{ {1}, itemManager };

	auto temp = hotbar_obj.getSlotPosition_at(0);
	item.setPosition(temp - vec2f{-Hotbar::slot_size.x / 2, Hotbar::slot_size.y / 2});

	Render::GUI::Rectangle crossair{ {50, 50}, {0, 0}, Render::UvPixels{{0, 17}, {17, 17}} };
	crossair.setPosition({ -crossair.getSize().x / 2, -crossair.getSize().y / 2 });

	Render::GUI::Rectangle hotbar{ textureInventoryAtlas.getSize(), {0, 0}, Render::UvPixels{textureInventoryAtlas.getSize(), textureInventoryAtlas.getSize() } };

	


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

		static bool hiddenCursor{ true };
		player.resetMovement();
		window.processInputs(
			[&]()
			{
				using namespace Wai;
				using namespace GameWorld;
				using b = Buttons;

				if (window.isKeyPressed(b::Escape))
					window.close();


				if (window.isKeyPressed(b::W))
					player.move(Player::Forward, deltaTime);

				if (window.isKeyPressed(b::S))
					player.move(Player::Backward, deltaTime);

				if (window.isKeyPressed(b::D))
					player.move(Player::Right, deltaTime);

				if (window.isKeyPressed(b::A))
					player.move(Player::Left, deltaTime);


				if (window.isKeyPressed(b::Left_shift))
					player.move(Player::Downward, deltaTime);

				if(!player.attributes.flying)
				{
					if (window.isKeyPressedOnce(b::Space))
						player.move(Player::Upward, deltaTime);
				}
				else
					if (window.isKeyPressed(b::Space))
						player.move(Player::Upward, deltaTime);

				if (window.isKeyPressedOnce(b::E))
					world.update(player.getPos(), true);

				/*if (window.isKeyPressedOnce(b::R))
				{
					std::ofstream file{ASSET_PATH"continentalness.txt"};

					file.clear();

					file << "Continentalness: " << world.debug.continentalness_frequency << "\n";
					file << "Sea level		: " << world.debug.sea_level << "\n";

					file << "std::vector< double> c_points \n{\n";
					for (const auto& i : world.debug.c_points)
						file << i << ",\n";
					file << "\n}";

					file << "\n\nstd::vector< double> c_thresholds \n{\n";
					for (const auto& i : world.debug.c_thresholds)
						file << i << ",\n";
					file << "\n}";

					file.close();
				}*/

				if (window.isKeyPressedOnce(Wai::Buttons::F))
				{
					hiddenCursor = !hiddenCursor;
					mouseCanMoveEverything = hiddenCursor;

					glfwSetInputMode(window.get(), GLFW_CURSOR, hiddenCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
				}
			}
		);

		
		{
			ImGui::Begin("Debug"); // Window beginning
			
			ImGui::Text("Camera Pos	: %f %f %f", player.getPos().x, player.getPos().y, player.getPos().z);
			ImGui::Text("Velocity	: %f %f %f", player.getVelocity().x, player.getVelocity().y, player.getVelocity().z);

			ImGui::Checkbox("Flying", &player.attributes.flying);
			ImGui::Checkbox("Ghost ", &player.attributes.ghost);

			if(const auto* cl{ world.chunk_at(player.getPos()) })
			{
				const types::loc c{ GameWorld::Voxels::ChunkGrid::to_loc(cl->getPos()) };

				ImGui::Text("Chunk Loc : %ld %ld %ld", c.x, c.y, c.z);
			}

			
			/*{
				const double c_mi{ 0.00001 }, c_ma{ 1 };

				const double s_mi{ 0 }, s_ma{ 1000 };
				ImGui::SliderScalar("Sea level: ", ImGuiDataType_Double, &world.debug.sea_level, &s_mi, &s_ma);

				if (ImGui::SliderScalar("Continentalness: ", ImGuiDataType_Double, &world.debug.continentalness_frequency, &c_mi, &c_ma))
					world.update(player.getPos(), true);

				if (ImGui::Button("Add element to vector: ", { 20, 20 }))
				{
					world.debug.c_thresholds.push_back({});
					world.debug.c_points.push_back({});
				}

				if (ImGui::Button("Remove element to vector: ", { 20, 20 }))
				{
					world.debug.c_thresholds.pop_back();
					world.debug.c_points.pop_back();
				}

				if (ImPlot::BeginPlot(" ", {1500, 700}))
				{
					ImPlot::SetupAxisLimits(ImAxis_X1, -1, 1, ImGuiCond_Always);

					ImPlot::PlotLine("##line", world.debug.c_thresholds.data(), world.debug.c_points.data(), world.debug.c_thresholds.size());

					

					for (int i{}; i < world.debug.c_thresholds.size(); i++)
					{
						ImPlot::DragPoint(
							i,
							&world.debug.c_thresholds[i],
							&world.debug.c_points[i],
							{ 1, 0, 0, 1 },
							5.f
						);
					}

					ImPlot::EndPlot();
				}
			}*/


			ImGui::SliderFloat("Player Speed   ", &player.attributes.speed, 0.0f, 500.0f);
			ImGui::SliderFloat("Player MaxSpeed", &player.attributes.maxSpeed, 0.0f, 1000.f);
			ImGui::SliderFloat("Player JUmpHeight", &player.attributes.jumpHeight, 0.0f, 1000.f);


			ImGui::SliderScalar("Ray Distance", ImGuiDataType_U64, &WO.rayDist, &WO.rayDist_min, &WO.rayDist_max);

			ImGui::Checkbox("Instant Vox. Break", &WO.instant_voxel_breaking);
			ImGui::Checkbox("Instant Vox. Place", &WO.instant_voxel_placing);

			ImGui::VSliderScalar("Render Distance", { 15, 100 }, ImGuiDataType_S64, &GameWorld::Voxels::ChunkSettings::world_render_distance, &WO.rm, &WO.rma);
			ImGui::VSliderScalar("World Depth", { 15, 100 }, ImGuiDataType_::ImGuiDataType_Double, &world.y_min, &WO.y_minMin, &WO.y_minMax);

			ImGui::End(); // Window end
		}

		

		

		mat4f model{ mpml::Identity4<float> };
		mat4f view{ mpml::lookAt(player.getPos(), player.getCamera().front_dir + player.getPos(), player.getCamera().up_dir)};
		mat4f proj{ mpml::perspective(mpml::Angle<>::fromDegrees(90), window.getSize().x, window.getSize().y, 0.1f, 1000.f) };


		{
			shader3Dworld.use();

			shader3Dworld.setValue("model", model);
			shader3Dworld.setValue("view", view);
			shader3Dworld.setValue("proj", proj);

			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			world.update(player.getPos());
		}
		

		const auto& ray_result{ GameWorld::Voxels::Utils::raycast(player.getPos(), player.getCamera().front_dir, world.grid, WO.rayDist, world.getTypeManager())};

		bool drawHighlight{ false };
		if (ray_result)
		{

			ch.update(model, view, proj, ray_result->pos);



			if (window.isMouseButtonPressedOnce(Wai::Buttons::Mouse::Middle))
				WO.selected_voxel = world.block_at(ray_result->pos)->id;

			if (!WO.instant_voxel_breaking)
			{
				if (window.isMouseButtonPressedOnce(Wai::Buttons::Mouse::Left))
					world.set_voxel_at(ray_result->pos, 0);
			}
			else
			{
				if (window.isMouseButtonPressed(Wai::Buttons::Mouse::Left))
					world.set_voxel_at(ray_result->pos, 0);
			}

			if (!WO.instant_voxel_placing)
			{
				if (window.isMouseButtonPressedOnce(Wai::Buttons::Mouse::Right))
					world.set_voxel_at(ray_result->pos + ray_result->normal, WO.selected_voxel);
			}
			else
			{
				if (window.isMouseButtonPressed(Wai::Buttons::Mouse::Right))
					world.set_voxel_at(ray_result->pos + ray_result->normal, WO.selected_voxel);
			}

			drawHighlight = true;
		}
			

		player.update(world, deltaTime);
		

		// Rendering

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		// World
		textureAtlas.bind();

		shader3Dworld.use();
		world.draw_chunkGrid(GameWorld::Voxels::ChunkGrid::to_loc(player.getPos()));

		// Cube Highlight

		ch.useShader();
		if(drawHighlight)
			ch.draw();

		// UI
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		shader2Drectangle.use();
		


		mat4f proj2D{ mpml::orthographic_projection((float)DESIGN_WIDTH, (float)DESIGN_HEIGHT, 0.f, 1.f) };
		shader2Drectangle.setValue("proj", proj2D);


		crossAirAtlas.bind();

		crossair.draw_transparent(shader2Drectangle);


		textureInventoryAtlas.bind();

		
		hotbar.setPosition({ -hotbar.getSize().x / 2, -DESIGN_HEIGHT / 2});

		hotbar.draw_transparent(shader2Drectangle);

		textureBlockInventoryAtlas.bind();

		item.draw_transparent(shader2Drectangle);
		

		glEnable(GL_CULL_FACE);

		// Debug 

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		window.display();
	}


	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	ImPlot::DestroyContext();

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
	static vec2f last{ static_cast<vec2f>(vec2d{xpos, ypos}) };

	vec2f offset{ static_cast<vec2f>(vec2d{xpos - last.x, ypos - last.y}) };

	last = static_cast<vec2f>(vec2d{ xpos, ypos });

	const auto sensitivity = 0.1f;

	offset *= sensitivity;

	if (!mouseCanMoveEverything)
		static_cast<Wai::Window*>(glfwGetWindowUserPointer(window))->onMouseMovement(offset, player);
}

void framebuffersize_callback(GLFWwindow* window, int width, int height) noexcept
{
	static_cast<Wai::Window*>(glfwGetWindowUserPointer(window))->onFramebufferResize({ width, height });
}
