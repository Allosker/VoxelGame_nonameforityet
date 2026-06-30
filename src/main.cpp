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

#include "world/entities/entity.hpp"


#include "utilities/debug.hpp"

#include "physics/collisions/basicHitbox.hpp"
#include "world/entities/player/player.hpp"

#include "world/entities/items/itemEntity.hpp"
#include "utilities/resourceManaging.hpp"

#include "rendering/text/text.hpp"

#include "world/terrain/terrain.hpp"

#include "utilities/camera.hpp"


#include "world/entities/player/inventory.hpp"
#include "rendering/GUI/guiInventory.hpp"


/* TOFIXLIST -- TODOLIST on Trello
* 
*	== Fix it so that when you scroll the mouse wheel swiftly, it doesn't break the hotbar
*	== Fix it so that bounding boxes adapt to the size of the object
*	== Problem: chunks that were empty and are now full and get their blocks removed won't be empty again
*	== When a block is place in another chunk than in the chunk containing light, it doesn't get updated properly
* 
* 
*	== Fix it so that the window has a queue event
*/

namespace
{
	void framebuffersize_callback(GLFWwindow* window, int width, int height) noexcept;

	void mouse_callback(GLFWwindow* window, double xpos, double ypos) noexcept;

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) noexcept;

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) noexcept;

	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) noexcept;
}


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

		

	glfwSetFramebufferSizeCallback(window.get(), ::framebuffersize_callback);
	glfwSetCursorPosCallback(window.get(), ::mouse_callback);
	glfwSetScrollCallback(window.get(), ::scroll_callback);
	glfwSetKeyCallback(window.get(), ::key_callback);
	glfwSetMouseButtonCallback(window.get(), ::mouse_button_callback);


	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window.get(), true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();


	// Shaders 
	render::Shader shaderCubeDisplay{ SHADER_PATH"shader.vert", SHADER_PATH"shader.frag" };
	

	

	// Test					 		

	render::Font font{ FONT_PATH"pixelated.ttf" };

	render::Image noise_img{ vec2i{}, GL_RED };
	render::Texture noise_tex{ noise_img };


	utils::Camera	camera{};
	camera.setFramebufferSize(window.getSize());

	vec2f last_mouse_window_pos{};
	float yaw{ -90 }, pitch{};

	// World
	render::Skybox skybox{};
	World world{ &camera };


	Inventory inv{};
	render::gui::GuiInventory guiInv{ inv };


	float deltaTime{};
	float lastFrame{};
	float fps{};
	//constexpr float fps_cap{ 120 };
	while (window.isOpen())
	{
		// Frames
		float frameStart = glfwGetTime();

		deltaTime = frameStart - lastFrame;
		lastFrame = frameStart;

		fps = 1.f / deltaTime;

		deltaTime = std::min(deltaTime, 1.F / 30.f); // limit delta time to avoid explosions
		


		//-== Debug
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		//~== Debug

		/*=============================*/
		// Event processing
		/*=============================*/

		bool imgui_compute_noisemap{ false };

		bool raycast_select_block{ false };
		bool raycast_place_block{ false };
		bool raycast_destroy_block{ false };


		window.clearStates();
		while(auto event = window.pollEvent())
		{

			if (auto button = event->get_if<Event::MouseButtonPressed>())
			{
				if (button->scancode == MouseButtons::Middle)
					raycast_select_block = true;

				if (button->scancode == MouseButtons::Right)
					raycast_place_block = true;

				if (button->scancode == MouseButtons::Left)
					raycast_destroy_block = true;

			}

			if (auto key = event->get_if<Event::KeyPressed>())
			{

				if (key->scancode == Keys::Escape)
					window.close();


				/*if (key->scancode == Keys::Tab)
					world.player.getInventory().process(window, world.player.getHotbar());*/

				if (key->scancode == Keys::E)
					world.debug_flags.force_reload = true;

				if (key->scancode == Keys::R)
				{
					/*if (world.player.getHotbar().getSelectedItem().id)
						world.set_voxel_at(world.player.getPosition(), world.player.getHotbar().getSelectedItem().id);*/

					imgui_compute_noisemap = true;
				}
				/*{
					player.getCamera().free = !player.getCamera().free;
					world.debug_flags.update_world = !world.debug_flags.update_world;
				}*/

				if (key->scancode == Keys::T)
					world.debug_flags.draw_chunk_borders = !world.debug_flags.draw_chunk_borders;

				if (key->scancode == Keys::F)
				{
					window.alternateCursorVisibility();
					camera.toggle_dir_updates();
				}

				if (key->scancode == Keys::Tab)
				{
					guiInv.toggle();
					window.alternateCursorVisibility();
					camera.toggle_dir_updates();
				}

				if (key->scancode == Keys::Q)
					inv.setBoardSize(Inventory::BoardSize::Big);
			}
			

			/*if (auto d = event->get_if<Event::MouseWheelScrolled>())
				world.player.getHotbar().nextSlot(d->delta.y);*/

			if (auto f = event->get_if<Event::Resized>())
				camera.setFramebufferSize(f->size);

			if (auto p = event->get_if<Event::MouseMoved>())
			{
				vec2f offset{ p->pos - last_mouse_window_pos };
				last_mouse_window_pos = p->pos;

				// Do after last_mouse_window_pos was updated to avoid jumps
				if (camera.should_update_dirs())
				{

					offset *= world.player.attributes.playability.sensitivity;

					yaw += offset.x;
					pitch += offset.y;


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

					camera.setDirections(direction.normal());
				}
			}
		}

		if (window.isKeyPressed(Keys::W))
			world.player.move(entities::Player::Forward, deltaTime);

		if (window.isKeyPressed(Keys::S))
			world.player.move(entities::Player::Backward, deltaTime);

		if (window.isKeyPressed(Keys::D))
			world.player.move(entities::Player::Right, deltaTime);

		if (window.isKeyPressed(Keys::A))
			world.player.move(entities::Player::Left, deltaTime);



		if (window.isKeyPressed(Keys::Left_shift))
			world.player.move(entities::Player::Downward, deltaTime);

		if (!world.player.attributes.flags.flying)
		{
			if (window.isKeyPressed(Keys::Space) && world.player.getVelocity().y == 0)
				world.player.move(entities::Player::Upward, deltaTime);
		}
		else
			if (window.isKeyPressed(Keys::Space))
				world.player.move(entities::Player::Upward, deltaTime);


		camera.setPosition(world.player.getPosition());


		/*=============================*/
		// ImGUI Debug					
		/*=============================*/

		{
			if (ImGui::Begin("Noise", nullptr, ImGuiWindowFlags_MenuBar))
			{  // Window beginning

				if (ImGui::BeginMenuBar())
				{

					if (ImGui::BeginMenu("Parameters"))
					{
						if (ImGui::BeginMenu("Continentalness"))
						{
							const double l1{ 0.0000001 }, l2{ 0.01 };
							ImGui::SliderScalar("freq", ImGuiDataType_Double, &terrain::continentalness_.freq, &l1, &l2);
							ImGui::SliderFloat("amp", &terrain::continentalness_.amp , 1.f, 100.f);

						ImGui::EndMenu();
						}

						if (ImGui::BeginMenu("Thresholds"))
						{
							ImGui::SliderFloat("water", &terrain::thresholds.water, 0.f, 1.f);
							ImGui::SliderFloat("sand", &terrain::thresholds.sand, 0.f, 1.f);
							ImGui::SliderFloat("grass", &terrain::thresholds.grass, 0.f, 1.f);
							ImGui::SliderFloat("stone", &terrain::thresholds.stone, 0.f, 1.f);

							ImGui::EndMenu();
						}

							

					ImGui::EndMenu();
					}

				ImGui::EndMenuBar();
				}

				static uint64 renderdistance{};
				const uint64 l1{ 0 }, l2{ 100 };
				ImGui::SliderScalar("Render Distance", ImGuiDataType_S64, &renderdistance, &l1, &l2);


				auto player_loc = chunks::ChunkGrid::to_loc(world.player.getPosition());
				vec3i min{ player_loc * (int64)chunks::Chunk::g_size - (int64)renderdistance * chunks::Chunk::g_size };
				vec3i max{ player_loc * (int64)chunks::Chunk::g_size + (int64)renderdistance * chunks::Chunk::g_size };
					
				if (imgui_compute_noisemap)
				{
					vec3i gap{ max - min };

					noise_img = render::Image{ vec2i{gap.x, gap.z}, GL_RGB };


					for (int32 x{ min.x }; x < max.x; x++)
						for (int32 z{ min.z }; z < max.z; z++)
						{
							vec3i rgb{};

							auto t = terrain::generate_height(x, z) / terrain::height_factor;
									
							if (t < terrain::thresholds.water)
								rgb = (vec3i)vec3d{ 0, 0, t * 255 };
							else if(t > terrain::thresholds.water && t < terrain::thresholds.sand)
								rgb = (vec3i)vec3d{ t * 255, t * 255, 6 };
							else if (t > terrain::thresholds.sand && t < terrain::thresholds.grass)
								rgb = (vec3i)vec3d{ 10, t * 255, 6 };
							else if (t > terrain::thresholds.grass && t < terrain::thresholds.stone)
								rgb = (vec3i)vec3d{ t * 50, t * 50, t * 50 };

							noise_img.getData().at(((x - min.x) + (z - min.z) * gap.x) * noise_img.getChannel()    ) = rgb.x;
							noise_img.getData().at(((x - min.x) + (z - min.z) * gap.x) * noise_img.getChannel() + 1) = rgb.y;
							noise_img.getData().at(((x - min.x) + (z - min.z) * gap.x) * noise_img.getChannel() + 2) = rgb.z;
						}

					noise_tex.update(noise_img);
				}

				static float scale{ 10.f };
				ImGui::SliderFloat("Noise scale: ", &scale, 0.0001f, 10.0f);
				ImGui::Image(noise_tex.ID(), ImVec2(noise_tex.getSize().x * scale, noise_tex.getSize().y * scale));

			
			}
			ImGui::End();

			{
				ImGui::Begin("Values"); // Window beginning

				ImGui::Text("FPS: %f", fps);
				//ImGui::Text("Sunlight: (%i) Blocklight: (%i, %i, %i)", (DEBUG_light_value >> 12) & 0xF, (DEBUG_light_value >> 8) & 0xF, (DEBUG_light_value >> 4) & 0xF, DEBUG_light_value & 0xF);

				ImGui::Checkbox("Flying", &world.player.attributes.flags.flying);
				ImGui::Checkbox("Ghost ", &world.player.attributes.flags.ghost);

				if (const auto* cl{ world.chunk_at(world.player.getPosition()) })
				{
					const types::loc c{ chunks::ChunkGrid::to_loc(cl->getPos()) };

					ImGui::Text("chunks::Chunk Loc : %ld %ld %ld", c.x, c.y, c.z);
				}

				size_t selectedIndex = 0;
				if (ImGui::BeginCombo("spawn item", ItemTypeManager::get().getTypeList()[selectedIndex].name.c_str())) {
					for (size_t i{}; i < ItemTypeManager::get().getTypeList().size(); ++i) {
						const bool isSelected = (selectedIndex == i);

						if (ImGui::Selectable(ItemTypeManager::get().getTypeList()[i].name.c_str(), isSelected))
						{
							selectedIndex = i;
							/*world.player.getHotbar().setCurrentSlot({ (uint16)(i + 1) }, ItemTypeManager::get().;*/
						}

						// Set the initial focus when opening the combo
						// (scrolling + keyboard navigation focus)
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				ImGui::End(); // Window end
			}

			{
				ImGui::Begin("Player"); // Window beginning

				ImGui::Text("Player Pos	: %f %f %f", world.player.getPosition().x, world.player.getPosition().y, world.player.getPosition().z);
				ImGui::Text("Velocity	: %f %f %f", world.player.getVelocity().x, world.player.getVelocity().y, world.player.getVelocity().z);

				ImGui::SliderFloat("Speed   ", &world.player.attributes.physics.speed, 0.0f, 500.0f);
				ImGui::SliderFloat("MaxSpeed", &world.player.attributes.physics.maxSpeed, 0.0f, 1000.f);
				ImGui::SliderFloat("JUmpHeight", &world.player.attributes.physics.jumpHeight, 0.0f, 1000.f);

				ImGui::SliderScalar("Ray Distance", ImGuiDataType_U64, &world.debug.rayDist, &world.debug.rayDist_min, &world.debug.rayDist_max);

				ImGui::Checkbox("Instant Vox. Break", &world.debug.instant_voxel_breaking);
				ImGui::Checkbox("Instant Vox. Place", &world.debug.instant_voxel_placing);

				ImGui::SliderScalar("render Distance", ImGuiDataType_S64, &ChunkSettings::world_render_distance, &world.debug.rm, &world.debug.rma);

				ImGui::End(); // Window end
			}
		}

		
		/*=============================*/
		// World updating
		/*=============================*/

		const auto& ray_result{ utils::raycast(camera.getPosition(), camera.getFrontDir(), world.grid, world.debug.rayDist, VoxelTypeManager::get())};

		world.flags.draw_cubehighlight = false;
		// Change that so that you only have a currently selected world block which avoids this ugly code part altogether and can thus enables the player to manage the world
		// Make it so that the player holds the raycastresult, the world takes in a player object, and sets the current voxel based on that
		if (!window.isCursorHidden())
			if (ray_result)
			{
				 
				world.ch.update(mpml::Identity4<float>, camera.getViewProj(), ray_result->pos);


				if (raycast_select_block)
				{
					//auto id{ world.block_at(ray_result->pos)->id };

					/*world.player.getHotbar().setCurrentSlot({ id }, ItemTypeManager::get().;*/
				}

				if (!world.debug.instant_voxel_breaking)
				{
					if (raycast_destroy_block)
					{
						auto id{ world.block_at(ray_result->pos)->id };

						world.entity_chunkGrid.addEntity(
							{ Resources::get().atlas_im_guiblocks, toPixelUnits(id, ItemTypeManager::get()), id },
							ray_result->pos);

						world.set_voxel_at(ray_result->pos, 0);
					}
				}
				else
				{
					if (raycast_destroy_block)
						world.set_voxel_at(ray_result->pos, 0);
				}

				if (!world.debug.instant_voxel_placing)
				{
					/*if (world.player.getHotbar().getSelectedItem().id != 0 && raycast_place_block)
						world.set_voxel_at(ray_result->pos + ray_result->normal, world.player.place_voxel().id);*/
				}
				else
				{
					/*if (raycast_place_block)
						world.set_voxel_at(ray_result->pos + ray_result->normal, world.player.getHotbar().getSelectedItem().id);*/
				}

				world.flags.draw_cubehighlight = true;
			}

			world.update(window, deltaTime);


			// Debug
			render::debug::DebugRenderer::get().update(glfwGetTime());

			vec2d cursor_pos{};
			glfwGetCursorPos(window.get(), &cursor_pos.x, &cursor_pos.y);

			guiInv.update(inv, Window::toGUICoordinates(window, static_cast<vec2f>(cursor_pos)));


		/*=============================*/
		// Rendering
		/*=============================*/

			// Clear everything
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			

			// Skybox
			float daytime = glfwGetTime() / 60.f;
			skybox.draw({ camera.getViewProj() }, camera.getPosition());

			// World
			world.draw({ camera.getViewProj() });

			// Frustum
			/*static auto i = (player.getCamera().view* player.getCamera().proj).inverse()->transpose();

			if (!player.getCamera().free)
				i = (player.getCamera().view * player.getCamera().proj).inverse()->transpose();


			render::debug::obb({}, { 1 }, { 1, 1, 1 }, i, 0, false);*/

			// Debug

			const auto& vp = camera.getViewProj();
			render::debug::DebugRenderer::get().render(vp);

			// UI
			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);

			guiInv.draw();

		
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

void ::framebuffersize_callback(GLFWwindow* window, int width, int height) noexcept
{
	auto* current_window = static_cast<Window*>(glfwGetWindowUserPointer(window));
	vec2i newsize = { width, height };

	current_window->addEvent(Event::Resized{ newsize });
	current_window->resize(newsize);
}

void ::mouse_callback(GLFWwindow* window, double xpos, double ypos) noexcept
{
	static_cast<Window*>(glfwGetWindowUserPointer(window))->addEvent(Event::MouseMoved{ { static_cast<float>(xpos), static_cast<float>(ypos) } });
}

void ::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) noexcept
{
	static_cast<Window*>(glfwGetWindowUserPointer(window))->addEvent(Event::MouseWheelScrolled{ { static_cast<float>(xoffset), static_cast<float>(yoffset) } });
}

void ::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) noexcept
{
	auto* current_window = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (action == GLFW_PRESS)
		current_window->addEvent(Event::KeyPressed{ static_cast<Keys>(key), static_cast<KeyboardModes>(mods) });
	else if (action == GLFW_RELEASE)
		current_window->addEvent(Event::KeyReleased{ static_cast<Keys>(key), static_cast<KeyboardModes>(mods) });
}

void ::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) noexcept
{
	auto* current_window = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (action == GLFW_PRESS)
		current_window->addEvent(Event::MouseButtonPressed{ static_cast<MouseButtons>(button), static_cast<KeyboardModes>(mods) });
	else if (action == GLFW_RELEASE)
		current_window->addEvent(Event::MouseButtonReleased{ static_cast<MouseButtons>(button), static_cast<KeyboardModes>(mods) });
}