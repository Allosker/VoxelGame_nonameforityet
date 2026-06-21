#include <iostream>
#include <stdexcept>
#include <limits>
#include <thread>

#include "mpml/vectors/special_overloads/print_vectors.hpp"

#include "utilities/window.hpp"
#include "utilities/inputs.hpp" 

#include "rendering/shader.hpp"

// Block Hitting

#include <utility>

#include "world/world.hpp"

#include "rendering/texture.hpp"



#include "rendering/mesh.hpp"

#include "rendering/gui/elements/rectangle.hpp"

#include "world/entities/basicEntity.hpp"

#include "rendering/skybox.hpp"

#include "utilities/debug.hpp"

#include "physics/collisions/basicHitbox.hpp"
#include "world/entities/player/player.hpp"

#include "world/entities/items/itemEntity.hpp"
#include "world/types/itemTypeManager.hpp"

#include "rendering/text/text.hpp"




/* TOFIXLIST -- TODOLIST on Trello
* 
*	== Fix it so that when you scroll the mouse wheel swiftly, it doesn't break the hotbar
*	== Fix it so that bounding boxes adapt to the size of the object
*	== Problem: chunks that were empty and are now full and get their blocks removed won't be empty again
*	== When a block is place in another chunk than in the chunk containing light, it doesn't get updated properly
* 
* 
*	== Get the camera out of the player and into the world instead
*/

static void framebuffersize_callback(GLFWwindow* window, int width, int height) noexcept;

static void mouse_callback(GLFWwindow* window, double xpos, double ypos) noexcept;

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) noexcept;

float deltaTime{};

int main()
try
{
	if (!glfwInit())
		return -1;


	glfwWindowHint(GLFW_CONTEXT_DEBUG, true);

	//glfwWindowHint(GLFW_SAMPLES, 4);

	Window window{ {640, 480}, "Test", nullptr, nullptr };

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
	glfwSetScrollCallback(window.get(), scroll_callback);


	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window.get(), true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

	// Shaders 
	render::Shader shaderCubeDisplay{ SHADER_PATH"shader.vert", SHADER_PATH"shader.frag" };
	render::Shader S_skybox{ SHADER_PATH"simple/skybox.vert", SHADER_PATH"simple/skybox.frag" };

	// Textures/Images
	

	
	// World
	World world{ window };

	// Test					 		

	render::Font font{ FONT_PATH"pixelated.ttf" };


	render::Skybox skybox{};

	float lastFrame{};
	float fps{};
	//constexpr float fps_cap{ 120 };
	while (window.isOpen())
	{
		// Frames
		float frameStart = glfwGetTime();

		deltaTime = frameStart - lastFrame;
		lastFrame = frameStart;

		deltaTime = std::min(deltaTime, 1.F / 30.f); // limit delta time to avoid explosions
		fps = 1.f / deltaTime;


		//-== Debug
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		//~== Debug

		/*=============================*/
		// Event processing
		/*=============================*/


			window.clearEvents();
			{

				using b = Buttons;

				if (window.isKeyPressed(b::Escape))
					window.close();

				if (vec2f delta = window.getMouseWheelDelta(); delta.y != 0)
					world.player.getHotbar().nextSlot(delta.y).id;


				if (window.isKeyPressed(b::W))
					world.player.move(entities::Player::Forward, deltaTime);

				if (window.isKeyPressed(b::S))
					world.player.move(entities::Player::Backward, deltaTime);

				if (window.isKeyPressed(b::D))
					world.player.move(entities::Player::Right, deltaTime);

				if (window.isKeyPressed(b::A))
					world.player.move(entities::Player::Left, deltaTime);


				if (window.isKeyPressed(b::Left_shift))
					world.player.move(entities::Player::Downward, deltaTime);

				if (!world.player.attributes.flags.flying)
				{
					if (window.isKeyPressedOnce(b::Space) && world.player.getVelocity().y == 0)
						world.player.move(entities::Player::Upward, deltaTime);
				}
				else
					if (window.isKeyPressed(b::Space))
						world.player.move(entities::Player::Upward, deltaTime);

				if (window.isKeyPressedOnce(b::Tab))
					world.player.getInventory().process(window, world.player.getHotbar());

				if (window.isKeyPressedOnce(b::E))
					world.debug_flags.force_reload = true;

				if (window.isKeyPressedOnce(b::R))
				{
					world.set_voxel_at(world.player.getPos(), world.player.getHotbar().getSelectedItem().id);
				}
				/*{
					player.getCamera().free = !player.getCamera().free;
					world.debug_flags.update_world = !world.debug_flags.update_world;
				}*/

				if (window.isKeyPressedOnce(b::T))
					world.debug_flags.draw_chunk_borders = !world.debug_flags.draw_chunk_borders;

				if (window.isKeyPressedOnce(Buttons::F))
					window.alternateCursorVisibility();

				// File filling
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

				


				if (window.wasFrameBufferResized())
					world.player.getCamera().updateProjMatrix(window.getSize());

				if (window.hasDirChanged())
					world.player.getCamera().front_dir = window.getNewFrontDir();
			}

		/*=============================*/
		// ImGUI Debug
		/*=============================*/
			{
				ImGui::Begin("Debug"); // Window beginning

				ImGui::Text("FPS: %f", fps);
			
				ImGui::Text("Camera Pos	: %f %f %f", world.player.getPos().x, world.player.getPos().y, world.player.getPos().z);
				ImGui::Text("Velocity	: %f %f %f", world.player.getVelocity().x, world.player.getVelocity().y, world.player.getVelocity().z);

				//ImGui::Text("Sunlight: (%i) Blocklight: (%i, %i, %i)", (DEBUG_light_value >> 12) & 0xF, (DEBUG_light_value >> 8) & 0xF, (DEBUG_light_value >> 4) & 0xF, DEBUG_light_value & 0xF);

				ImGui::Checkbox("Flying", &world.player.attributes.flags.flying);
				ImGui::Checkbox("Ghost ", &world.player.attributes.flags.ghost);

				if(const auto* cl{ world.chunk_at(world.player.getPos()) })
				{
					const types::loc c{ chunks::ChunkGrid::to_loc(cl->getPos()) };

					ImGui::Text("chunks::Chunk Loc : %ld %ld %ld", c.x, c.y, c.z);
				}

				size_t selectedIndex = 0;
				if (ImGui::BeginCombo("spawn item", world.m_itm.getTypeList()[selectedIndex].name.c_str())) {
					for (size_t i{}; i < world.m_itm.getTypeList().size(); ++i) {
						const bool isSelected = (selectedIndex == i);

						if (ImGui::Selectable(world.m_itm.getTypeList()[i].name.c_str(), isSelected))
						{
							selectedIndex = i;
							world.player.getHotbar().setCurrentSlot({ (uint16)(i + 1) }, world.m_itm);
						}

						// Set the initial focus when opening the combo
						// (scrolling + keyboard navigation focus)
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

			
				 {
					const double c_mi{ 0.00001 }, c_ma{ 1 };

					const double s_mi{ 0 }, s_ma{ 1000 };
					ImGui::SliderScalar("Sea level: ", ImGuiDataType_Double, &world.debug.sea_level, &s_mi, &s_ma);

					if (ImGui::SliderScalar("Continentalness: ", ImGuiDataType_Double, &world.debug.continentalness_frequency, &c_mi, &c_ma))
						world.debug_flags.force_reload = true;

					/*if (ImGui::Button("Add element to vector: ", { 20, 20 }))
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
					}*/
				}


				ImGui::SliderFloat("entities::Player Speed   ", &world.player.attributes.physics.speed, 0.0f, 500.0f);
				ImGui::SliderFloat("entities::Player MaxSpeed", &world.player.attributes.physics.maxSpeed, 0.0f, 1000.f);
				ImGui::SliderFloat("entities::Player JUmpHeight", &world.player.attributes.physics.jumpHeight, 0.0f, 1000.f);


				ImGui::SliderScalar("Ray Distance", ImGuiDataType_U64, &world.debug.rayDist, &world.debug.rayDist_min, &world.debug.rayDist_max);

				ImGui::Checkbox("Instant Vox. Break", &world.debug.instant_voxel_breaking);
				ImGui::Checkbox("Instant Vox. Place", &world.debug.instant_voxel_placing);

				ImGui::VSliderScalar("render Distance", { 15, 100 }, ImGuiDataType_S64, &ChunkSettings::world_render_distance, &world.debug.rm, &world.debug.rma);

				ImGui::End(); // Window end
			}

		/*=============================*/
		// Misc.
		/*=============================*/


			
			
		/*=============================*/
		// World updating
		/*=============================*/

			
			world.update(window, deltaTime);

			/*put it into world class*/ 


			// Debug
			render::debug::DebugRenderer::get().update(glfwGetTime());

		/*=============================*/
		// Rendering
		/*=============================*/

			// Clear everything
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glEnable(GL_CULL_FACE);

			// Skybox
			float daytime = glfwGetTime() / 60.f;

			S_skybox.use();
			//model = mpml::rotate(m, mpml::Angle<>::fromDegrees((daytime * 360.f) ), { 0.5, 0, 0.5 }); example only 

			S_skybox.setValue("model", mpml::Identity4<float>);
			S_skybox.setValue("view", world.player.getCamera().view);
			S_skybox.setValue("proj", world.player.getCamera().proj);

			skybox.draw(S_skybox);

			glEnable(GL_DEPTH_TEST);

			// World
			world.draw();

			// Frustum
			/*static auto i = (player.getCamera().view* player.getCamera().proj).inverse()->transpose();

			if (!player.getCamera().free)
				i = (player.getCamera().view * player.getCamera().proj).inverse()->transpose();


			render::debug::obb({}, { 1 }, { 1, 1, 1 }, i, 0, false);*/

			// Debug

			auto vp = world.player.getCamera().view * world.player.getCamera().proj;
			render::debug::DebugRenderer::get().render(vp);

			// UI
			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);


		
			// Debug ImGUI

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		

			// render all to window
			window.display();

		// Limit FrameRate
		/*auto frameExecutionTime = glfwGetTime() - frameStart;
		if (frameExecutionTime < 1 / fps_cap)
		{
			std::this_thread::sleep_for(std::chrono::duration<float>(1 / fps_cap - frameExecutionTime));
		}*/
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

void framebuffersize_callback(GLFWwindow* window, int width, int height) noexcept
{
	static_cast<Window*>(glfwGetWindowUserPointer(window))->onFramebufferResize({ width, height });
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) noexcept
{
	static vec2f last{ static_cast<vec2f>(vec2d{xpos, ypos}) };

	vec2f offset{ static_cast<vec2f>(vec2d{xpos - last.x, ypos - last.y}) };

	last = static_cast<vec2f>(vec2d{ xpos, ypos });

	const auto sensitivity = 0.1f;

	offset *= sensitivity;

	if (!static_cast<Window*>(glfwGetWindowUserPointer(window))->isCursorHidden())
		static_cast<Window*>(glfwGetWindowUserPointer(window))->onMouseMovement(offset);

	static_cast<Window*>(glfwGetWindowUserPointer(window))->onMouseCursorPosChange({ static_cast<float>(xpos), static_cast<float>(ypos) });
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) noexcept
{
	static_cast<Window*>(glfwGetWindowUserPointer(window))->onMouseWheelScroll({ static_cast<float>(xoffset), static_cast<float>(yoffset) });
}