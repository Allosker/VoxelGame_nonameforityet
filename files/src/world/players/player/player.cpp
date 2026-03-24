#include "player.hpp"


// =====================
// Actors
// =====================

void GameWorld::Player::update(const GameWorld::World& world, float deltaTime) noexcept
{
	updatePosition(world, deltaTime);
	resolve_collisions(world);
}

void GameWorld::Player::updatePosition(const GameWorld::World& world, float deltaTime) noexcept
{
	if (!attributes.flying)
		m_velocity.y += world.settings.gravity * deltaTime;


	if (vec2f{ m_velocity.x, m_velocity.z }.length() > attributes.maxSpeed)
	{
		const auto tempY{ m_velocity.y };

		m_velocity = vec3f{ m_velocity.x, 0, m_velocity.z }.normal() * attributes.maxSpeed;
		m_velocity.y = tempY;
	}


	if (!attributes.moving)
	{
		m_velocity.x *= (1 - attributes.friction * deltaTime);
		m_velocity.z *= (1 - attributes.friction * deltaTime);
	}

	if (!attributes.flying)
		m_camera.pos += m_velocity * deltaTime;
	else
		m_camera.pos += vec3f{m_velocity.x, 0, m_velocity.z } * deltaTime;
}

void GameWorld::Player::resolve_collisions(const GameWorld::World& world) noexcept
{
	if (attributes.ghost)
		return;

	m_hitbox = { m_camera.pos, { 0.25, 1.75, 0.25 }, { 0.25, 0.20, 0.25 } };

	std::vector<types::loc> camPoss;

	const auto flooredPosMin{ static_cast<vec3i>(mpml::floor(m_hitbox.min)) };
	const auto flooredPosMax{ static_cast<vec3i>(mpml::floor(m_hitbox.max)) };

	for (int32 x{ flooredPosMin.x }; x <= flooredPosMax.x; x++)
		for (int32 y{ flooredPosMin.y }; y <= flooredPosMax.y; y++)
			for (int32 z{ flooredPosMin.z }; z <= flooredPosMax.z; z++)
				camPoss.emplace_back(vec3i{ x, y, z });

	for (const auto& pos : camPoss)
		if (const auto* cubeptr{ world.block_at(pos) }; cubeptr && cubeptr->id)
		{
			Physics::Collisions::BasicHitbox cube{ static_cast<vec3f>(pos), static_cast<vec3f>(pos + types::loc{1}) };

			if (m_hitbox.intersect(cube))
			{

				auto offset = m_hitbox.findIntersection(cube, world, pos);

				if (offset.y != 0)
					m_velocity.y = 0;

				if (offset.x != 0)
					m_velocity.x = 0;

				if (offset.z != 0)
					m_velocity.z = 0;

				m_camera.pos -= offset;

				m_hitbox.setHitbox(m_camera.pos);
			}
		}
}


// =====================
// Mutators
// =====================

void GameWorld::Player::move(const Direction& dir, float deltaTime) noexcept
{
	const auto speed{ attributes.speed * deltaTime };

	switch (dir)
	{
	case Forward:
		m_velocity += vec3f{ m_camera.front_dir.x, 0.f, m_camera.front_dir.z }.normal() * speed;
		break;

	case Backward:
		m_velocity -= vec3f{ m_camera.front_dir.x, 0.f, m_camera.front_dir.z }.normal() * speed;
		break;

	case Right:
		m_velocity += vec3f{ m_camera.right_dir().x, 0.f, m_camera.right_dir().z}.normal() * speed;
		break;

	case Left:
		m_velocity -= vec3f{ m_camera.right_dir().x, 0.f, m_camera.right_dir().z }.normal() * speed;
		break;

	case Downward:
		if (attributes.flying)
			m_camera.pos -= vec3f{0, attributes.jumpHeight, 0} * deltaTime;
		break;

	case Upward:
		if (!attributes.flying)
			m_velocity.y += attributes.jumpHeight;
		else
			m_camera.pos += vec3f{ 0, attributes.jumpHeight, 0 } * deltaTime;
		break;
	}

	attributes.moving = true;
}

void GameWorld::Player::resetMovement() noexcept
{
	attributes.moving = false;
}


// =====================
// Getters
// =====================

const vec3f& GameWorld::Player::getVelocity() const noexcept
{
	return m_velocity;
}

const vec3f& GameWorld::Player::getPos() const noexcept
{
	return m_camera.pos;
}

const Render::Utils::Camera& GameWorld::Player::getCamera() const noexcept
{
	return m_camera;
}

Render::Utils::Camera& GameWorld::Player::getCamera() noexcept
{
	return m_camera;
}
