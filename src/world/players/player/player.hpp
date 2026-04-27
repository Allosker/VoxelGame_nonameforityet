#pragma once // player.hpp
// MIT
// Allosker ------------------------------
// =========+
// Basic class for Player, this manages everything about the player and offers a higher level of abstraction
// ---------------------------------------

#include "physics/collisions/basicHitbox.hpp"

#include "rendering/utilities/camera.hpp"

#include "world/world.hpp"


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


		// = Actors

		void update(const GameWorld::World& world, float deltaTime) noexcept;

		void updatePosition(const GameWorld::World& world, float deltaTime) noexcept;

		void resolve_collisions(const GameWorld::World& world) noexcept;


		// = Mutators

		void move(const Direction& dir, float deltaTime) noexcept;

		void resetMovement() noexcept;


		// = Getters

		const vec3f& getVelocity() const noexcept;

		const vec3f& getPos() const noexcept;

		const Render::Utils::Camera& getCamera() const noexcept;
		Render::Utils::Camera& getCamera() noexcept;


	public:

		struct
		{
			float speed{ 20.f };
			float maxSpeed{ 5.f };
			float jumpHeight{ 10.f };

			float friction{ 15.f };

			float sensitivity{ 0.1f };

			bool moving{ false };
			bool flying{ true };
			bool ghost{ true };

		} attributes;

	private:

		Render::Utils::Camera m_camera{};
		Physics::Collisions::BasicHitbox m_hitbox{};

		vec3f m_velocity{};

	};


} // World