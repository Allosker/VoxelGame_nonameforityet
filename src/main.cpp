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

#include "world/entities/basic_entity.hpp"

/* TODOLIST
* 
*	== Fix it so that when you scroll the mouse wheel swiftly, it doesn't break the hotbar
*
*/


#include "uHeaders/debug.hpp"

#include "physics/collisions/basicHitbox.hpp"
#include "world/players/player/player.hpp"

#include "rendering/GUI/Items/itemRenderer.hpp"
#include "rendering/GUI/Items/itemTypeManager.hpp"

#include "rendering/GUI/HUD/hotbar.hpp"
#include "rendering/GUI/HUD/inventory.hpp"

#include "rendering/GUI/Items/itemRenderer.hpp"

#include <fstream>


#include <ft2build.h>
#include FT_FREETYPE_H


static void framebuffersize_callback(GLFWwindow* window, int width, int height) noexcept;

static void mouse_callback(GLFWwindow* window, double xpos, double ypos) noexcept;

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) noexcept;


GameWorld::Player player{};

float deltaTime{};


struct WorldOptions
{
	uint64 rayDist{ 5 };
	int64 rayDist_min{ 1 };
	int64 rayDist_max{ 2000 };

	bool instant_voxel_breaking{ false }, instant_voxel_placing{ false };

	uint64 rm{ 0 }, rma{ 8 };
};


struct Character
{
	vec2iu pos{};
	vec2iu size{};
	vec2i bearing{};
	uint32 advance{};
};


class Text
{
public:

	Text()
	{
		// Create Bitmap
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		FT_Library ft;
		if (FT_Init_FreeType(&ft))
			throw std::runtime_error("ERROR::FREETYPE: Could not init FreeType Library");

		FT_Face face;
		if (FT_New_Face(ft, FONT_PATH"arial.ttf", 0, &face))
			throw std::runtime_error("ERROR::FREETYPE: Failed to load");

		FT_Set_Pixel_Sizes(face, 0, 48);

		Render::Image bitmap{ vec2iu{48, 48 } };

		for (uint8 c{}; c < 1; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::println("ERROR::FREETYPE: Failed to load Glyph");
				continue;
			}

			Render::Image glyph{
				{ face->glyph->bitmap.width, face->glyph->bitmap.rows },
				face->glyph->bitmap.buffer,
				face->glyph->bitmap.buffer + static_cast<size_t>(face->glyph->bitmap.width) * static_cast<size_t>(face->glyph->bitmap.rows)
			};
			
			bitmap.insert({0, 0}, glyph);
			


			Character character{
				vec2iu{c, size_glyphs },
				vec2iu{face->glyph->bitmap.width, face->glyph->bitmap.rows},
				vec2i{face->glyph->bitmap_left, face->glyph->bitmap_top},
				face->glyph->advance.x
			};
			characters.insert(std::pair<uint8, Character>{c, character});
		}

		glGenTextures(1, &m_texture_id);
		glBindTexture(GL_TEXTURE_2D, m_texture_id);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			bitmap.getSize().x,
			bitmap.getSize().y,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			bitmap.data()
		);
			

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		FT_Done_Face(face);
		FT_Done_FreeType(ft);
		// create GPU data

		glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);

		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(0, 4, GL_FLOAT, false, 4 * sizeof(float), 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}

	void setText(const std::string& str) noexcept { m_text = str; }

	void draw(const Render::Shader& shader)
	{
		/*glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); */

		shader.use();
		glBindTexture(GL_TEXTURE_2D, m_texture_id);

		shader.setValue("texColor", m_color);

		glBindVertexArray(m_vao);
		

		/*vec2f tPos{ m_pos };

		std::string::const_iterator c{};
		for (c = m_text.begin(); c != m_text.end(); c++)
		{
			Character ch{ characters[*c] };

			vec2f pos{ tPos.x + ch.bearing.x * m_scale, tPos.x - (ch.size.y - ch.bearing.y) * m_scale };
			vec2f size{ ch.size.x * m_scale, ch.size.y * m_scale };


			float vertices[6][4]
			{
				{ pos.x,			pos.y + size.y,			0,			0 },
				{ pos.x,			pos.y,					0,			ch.pos.y },
				{ pos.x + size.x,	pos.y,					ch.pos.x,	ch.pos.y },

				{ pos.x,			pos.y + size.y,			0,			0 },
				{ pos.x + size.x,	pos.y,					ch.pos.x,	ch.pos.y },
				{ pos.x + size.x,	pos.y + size.y,			ch.pos.x,	0 },
			};

			

			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			
			glBindBuffer(GL_ARRAY_BUFFER, 0); 
			glDrawArrays(GL_TRIANGLES, 0, 6);

			tPos.x += (ch.advance >> 6) * m_scale;
		}*/

		float vertices[6][4]
		{
			{ 0,			48,			0,	0 },
			{ 0,			0,							0,	1 },
			{ 48,0,				1,	1 },

			{ 0,			48,			0,	0 },
			{ 48,		0,			1,	1 },
			{ 48,48,	1,	0 },
		};

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_BLEND);
	}

private:

	std::string m_text{ "This is sample text" };
	

	std::map<uint8, Character> characters{};

	vec3f m_color{ 0.5, 0.8f, 0.2f };
	vec2f m_pos  {25.f, 25.f};

	vec2iu m_texture_size{};
	float m_scale{ 1.f };

	GLuint m_vao{};
	GLuint m_vbo{};

	GLuint m_texture_id{};

	static constexpr size_t nb_glyphs{ 128 };
	static constexpr size_t size_glyph_squared{ 512 * 512 };
	static constexpr size_t size_glyphs{ 48 };

	static constexpr size_t padding_pixels{ 2 };

};

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
	glfwSetScrollCallback(window.get(), scroll_callback);


	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window.get(), true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

	Render::Shader shader3Dworld{ SHADER_PATH"shader.vert", SHADER_PATH"shader.frag" };

	Render::Shader shader3Ditem{ SHADER_PATH"shader3Ditem.vert", SHADER_PATH"shader3Ditem.frag" };

	Render::Shader shaderCubeDisplay{ SHADER_PATH"shader.vert", SHADER_PATH"shader.frag" };

	Render::Shader shader2Drectangle{ SHADER_PATH"meshTexture2D.vert", SHADER_PATH"meshTexture2D.frag" };

	Render::Shader shader2Dtext{ SHADER_PATH"text_render/text2D.vert", SHADER_PATH"text_render/text2D.frag" };



	Render::Texturing::Texture textureAtlas{ ASSET_PATH"blocks/world/atlas.png"};

	Render::Texturing::Texture crossAirAtlas{ ASSET_PATH"hud/crossair_atlas.png" };

	Render::Texturing::Texture atlas_guiBlocks{ ASSET_PATH"blocks/gui/block_inventory_atlas.png" };

	Render::Texturing::Texture texture_guiSlot{ ASSET_PATH"hud/slot.png" };


	
	Render::Texturing::Texture texture_guiInventorySlot{ ASSET_PATH"hud/slot_inventory.png" };
	Render::Texturing::Texture texture_guiInventory{ ASSET_PATH"hud/inventory.png" };



	Render::Utils::CubeHighlight ch;

	GameWorld::World world{};

	Render::GUI::ItemTypeManager itemTypeManager{};

	
	WorldOptions WO{};


	Render::GUI::Hotbar hotbar{ texture_guiSlot, itemTypeManager };

	Render::GUI::Inventory inventory{ texture_guiInventory, texture_guiInventorySlot };

	Render::GUI::Rectangle test{ texture_guiInventory.getSize(), {texture_guiInventory.getSize().x / 2, texture_guiInventory.getSize().y / 2}, types::Rect<types::uvs>{{}, texture_guiInventory.getSize()} };

	// Test
	Render::Image imageTest{ ASSET_PATH"apple_test.png" };

	Render::Texturing::Texture textureTest{ imageTest };

	Render::GUI::Rectangle rectTest{ {500, 500}, {}, { {}, imageTest.getSize()} };


	Render::Item3DMesh ItemTest{ imageTest, Render::GUI::toPixelUnits(2, itemTypeManager) };
	ItemTest.setPosition({ 10, 50, 10 });


	Render::GUI::Rectangle crossair{ {50, 50}, {0, 0}, types::Rect<types::uvs>{{0, 17}, {17, 17}} };
	crossair.setPosition({ -crossair.getSize().x / 2, -crossair.getSize().y / 2 });

	
	std::vector<Render::Item3DMesh> itemsTest;

	Text text{};
	float lastFrame{};
	while (window.isOpen())
	{
		deltaTime = glfwGetTime() - lastFrame;
		lastFrame = glfwGetTime();

		deltaTime = std::min(deltaTime, 1.F / 30.f);


		window.clearEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		player.resetMovement();
		window.processInputs(
			[&]()
			{
				using namespace Wai;
				using namespace GameWorld;
				using b = Buttons;

				if (window.isKeyPressed(b::Escape))
					window.close();

				if (vec2f delta = window.getMouseWheelDelta(); delta.y != 0)
					hotbar.nextSlot(delta.y).id;


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

				if (window.isKeyPressedOnce(b::Tab))
					inventory.process(window, hotbar);

				if (window.isKeyPressedOnce(b::E))
					world.update(player.getPos(), true);

				if (window.isKeyPressedOnce(b::P))
					hotbar.newPairOfSlots(texture_guiSlot);

				if (window.isKeyPressedOnce(b::O))
					inventory.newPairOfSlots(texture_guiInventorySlot);

				if (window.isKeyPressedOnce(b::M))
					hotbar.disable();

				if (window.isKeyPressedOnce(b::L))
					hotbar.enable();

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
					window.alternateCursorVisibility();
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

			
			/* {
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

			ImGui::End(); // Window end
		}

		
		inventory.update(window, itemTypeManager, hotbar, Wai::Window::toGUICoordinates(window, window.getMousePos()));
		//std::println("{}", Wai::Window::toGUICoordinates(window, window.getMousePos()));

		mat4f model{ mpml::Identity4<float> };
		mat4f view{ mpml::lookAt(player.getPos(), player.getCamera().front_dir + player.getPos(), player.getCamera().up_dir)};
		mat4f proj{ mpml::perspective(mpml::Angle<>::fromDegrees(90), window.getSize().x, window.getSize().y, 0.1f, 1000.f) };



		{
			// Chunks
			shader3Dworld.use();

			shader3Dworld.setValue("model", model);
			shader3Dworld.setValue("view", view);
			shader3Dworld.setValue("proj", proj);

			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			world.update(player.getPos());

			// 3 Item Renders

			shader3Ditem.use();

			shader3Ditem.setValue("model", model);
			shader3Ditem.setValue("view", view);
			shader3Ditem.setValue("proj", proj);
		}
		

		const auto& ray_result{ GameWorld::Voxels::Utils::raycast(player.getPos(), player.getCamera().front_dir, world.grid, WO.rayDist, world.getTypeManager())};

		bool drawHighlight{ false };
		if(!window.isCursorHidden())
			if (ray_result)
			{

				ch.update(model, view, proj, ray_result->pos);



				if (window.isMouseButtonPressedOnce(Wai::Buttons::Mouse::Middle))
				{
					auto id{ world.block_at(ray_result->pos)->id };

					hotbar.setCurrentSlot({ id }, itemTypeManager);
					hotbar.setCurrentSlot({ id }, itemTypeManager);
				}

				if (!WO.instant_voxel_breaking)
				{
					if (window.isMouseButtonPressedOnce(Wai::Buttons::Mouse::Left))
					{
						auto id{ world.block_at(ray_result->pos)->id };

						itemsTest.emplace_back(Render::Item3DMesh{ imageTest, Render::GUI::toPixelUnits(id, itemTypeManager) });
						itemsTest.back().rotate(mpml::Quaternion<float>::fromAxis(vec3f{1, 0, 0}, mpml::Angle<>::fromDegrees(90.f)));
						itemsTest.back().setPosition(mpml::floor(ray_result->pos) + vec3f{ 0.5, 0.01, 0.5 });

						world.set_voxel_at(ray_result->pos, 0);
					}
				}
				else
				{
					if (window.isMouseButtonPressed(Wai::Buttons::Mouse::Left))
						world.set_voxel_at(ray_result->pos, 0);
				}

				if (!WO.instant_voxel_placing)
				{
					if (window.isMouseButtonPressedOnce(Wai::Buttons::Mouse::Right))
						world.set_voxel_at(ray_result->pos + ray_result->normal, hotbar.getSelectedItem().id);
				}
				else
				{
					if (window.isMouseButtonPressed(Wai::Buttons::Mouse::Right))
						world.set_voxel_at(ray_result->pos + ray_result->normal, hotbar.getSelectedItem().id);
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
		shader3Dworld.use();

		textureAtlas.bind();

		
		world.draw_chunkGrid(GameWorld::Voxels::ChunkGrid::to_loc(player.getPos()));

		// Cube Highlight

		ch.useShader();
		if(drawHighlight)
			ch.draw();
		

		// draw test 
		ItemTest.draw(shader3Ditem, textureTest);

		for(auto& i : itemsTest)
			i.draw(shader3Ditem, textureTest);

		// UI
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		


		mat4f proj2D{ mpml::orthographic_projection(Wai::Window::g_guiViewSize.x, Wai::Window::g_guiViewSize.y, 0.f, 1.f) };
		//proj2D = mpml::translate(proj2D, { -Wai::Window::g_guiViewSize.x / 2, Wai::Window::g_guiViewSize.y / 2, 0 });

		shader2Dtext.use();
		shader2Dtext.setValue("proj", proj2D);

		shader2Drectangle.use();
		shader2Drectangle.setValue("proj", proj2D);
		
		/*textureTest.bind();
		rectTest.draw(shader2Drectangle);*/


		crossAirAtlas.bind();

		crossair.draw_transparent(shader2Drectangle);



		hotbar.draw(shader2Drectangle, texture_guiSlot, atlas_guiBlocks, itemTypeManager);

		inventory.draw(shader2Drectangle, texture_guiInventory, texture_guiInventorySlot, atlas_guiBlocks, itemTypeManager);

		text.draw(shader2Dtext);

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

void framebuffersize_callback(GLFWwindow* window, int width, int height) noexcept
{
	static_cast<Wai::Window*>(glfwGetWindowUserPointer(window))->onFramebufferResize({ width, height });
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) noexcept
{
	static vec2f last{ static_cast<vec2f>(vec2d{xpos, ypos}) };

	vec2f offset{ static_cast<vec2f>(vec2d{xpos - last.x, ypos - last.y}) };

	last = static_cast<vec2f>(vec2d{ xpos, ypos });

	const auto sensitivity = 0.1f;

	offset *= sensitivity;

	if (!static_cast<Wai::Window*>(glfwGetWindowUserPointer(window))->isCursorHidden())
		static_cast<Wai::Window*>(glfwGetWindowUserPointer(window))->onMouseMovement(offset, player);

	static_cast<Wai::Window*>(glfwGetWindowUserPointer(window))->onMouseCursorPosChange({ static_cast<float>(xpos), static_cast<float>(ypos) });
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) noexcept
{
	static_cast<Wai::Window*>(glfwGetWindowUserPointer(window))->onMouseWheelScroll({ static_cast<float>(xoffset), static_cast<float>(yoffset) });
}



