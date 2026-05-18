#pragma once // player.hpp
// MIT
// Allosker ------------------------------
// =========+
// Basic class for Player, this manages everything about the player and offers a higher level of abstraction
// ---------------------------------------

#include "transforms/collisions/basicHitbox.hpp"

#include "rendering/utilities/camera.hpp"

#include "rendering/shader.hpp"

#include "world/world.hpp"

#include "rendering/GUI/HUD/hotbar.hpp"
#include "rendering/GUI/HUD/inventory.hpp"


namespace GameWorld
{

	class Player
	{
	public:

		enum Direction
		{
			Forward,
			Backward,
			Left,
			Right,

			Upward,
			Downward
		};

	public:

		// = Construction/Destruction

		Player(
			const types::path& p_hotbarSlot,
			const types::path& p_inv,
			const types::path& p_slotInv,
			const Render::GUI::ItemTypeManager& itm,
			const types::path& p_font
		);

		// = Actors

		void update(const Wai::Window& window, const GameWorld::World& world, const Render::GUI::ItemTypeManager& itm, float deltaTime) noexcept;

		void updatePosition(const GameWorld::World& world, float deltaTime) noexcept;

		void resolve_collisions(const GameWorld::World& world) noexcept;

		void draw_attributes(const Render::Shader& shader,
			const Render::Texturing::Texture& gui_block_atlas,
			const Render::GUI::ItemTypeManager& itm) noexcept;


		// = Mutators

		void move(const Direction& dir, float deltaTime) noexcept;

		void resetMovement() noexcept;


		// = Getters

		const vec3f& getVelocity() const noexcept { return m_velocity; }

		const vec3f& getPos() const noexcept { return m_camera.pos; }

		Physics::Collisions::BasicHitbox& getHitbox() noexcept { return m_hitbox; }
		const Physics::Collisions::BasicHitbox& getHitbox() const noexcept { return m_hitbox; }

		Render::Utils::Camera& getCamera() noexcept { return m_camera; }
		const Render::Utils::Camera& getCamera() const noexcept { return m_camera; }

		Render::GUI::Inventory& getInventory() noexcept { return m_inventory; }
		const Render::GUI::Inventory& getInventory() const noexcept { return m_inventory; }

		Render::GUI::Hotbar& getHotbar() noexcept { return m_hotbar; }
		const Render::GUI::Hotbar& getHotbar() const noexcept { return m_hotbar; }


	public:

		struct
		{
			struct
			{
				float speed{ 20.f };
				float maxSpeed{ 5.f };
				float jumpHeight{ 10.f };
				float friction{ 15.f };
			} physics;

			struct
			{
				float sensitivity{ 0.1f };
			} playability;

			struct
			{
				bool moving{ false };
				bool flying{ true };
				bool ghost{ true };
			} flags;

		} attributes;


	public:

		

		Render::Utils::Camera				m_camera{};
		Physics::Collisions::BasicHitbox	m_hitbox{};

		Render::Texturing::Texture			m_texHotbarSlot;
		Render::Texturing::Texture			m_texInv;
		Render::Texturing::Texture			m_texInvSlot;

		Render::GUI::Font					m_font;

		Render::GUI::Inventory				m_inventory;
		Render::GUI::Hotbar					m_hotbar;


		vec3f								m_velocity{};

	};


} // World