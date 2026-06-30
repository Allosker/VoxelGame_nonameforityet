#pragma once // player.hpp
// MIT
// Allosker ------------------------------
// =========+
// Basic class for Player, this manages everything about the player and offers a higher level of abstraction
// ---------------------------------------


#include "rendering/shader.hpp"

#include "inventory.hpp"

#include "utilities/resourceManaging.hpp"

#include "physics/collisions/basicHitbox.hpp"
#include "world/entities/entity.hpp"

#include "utilities/camera.hpp"


class World;

namespace entities
{

	class Player
		: public Entity
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

		Player(const ItemTypeManager& itm, const utils::Camera* cam);


		// = Actors

		void update(const World& world, const ItemTypeManager& itm, float deltaTime) noexcept;

		void updatePosition(const World& world, float deltaTime) noexcept;

		void resolve_collisions(const World& world) noexcept;

		void draw(const ItemTypeManager& itm) noexcept;


		// = Mutators

		void move(const Direction& dir, float deltaTime) noexcept;

		void resetMovement() noexcept;


		// = Getters

		const vec3f& getVelocity() const noexcept { return m_velocity; }


		Inventory& getInventory() noexcept { return m_inventory; }
		const Inventory& getInventory() const noexcept { return m_inventory; }

		/*render::gui::Hotbar& getHotbar() noexcept { return m_hotbar; }
		const render::gui::Hotbar& getHotbar() const noexcept { return m_hotbar; }*/


		// = Setters

		void setCamera(const utils::Camera* new_camera) noexcept { m_bound_camera = new_camera; }


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

		Inventory				m_inventory;
		//render::gui::Hotbar					m_hotbar;


		vec3f								m_velocity{};


		const utils::Camera* m_bound_camera;

	};
}