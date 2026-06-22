#include "player.hpp"

#include "world/world.hpp"

// =====================
// Construction/Initialization
// =====================


entities::Player::Player(const ItemTypeManager& itm)
	: m_texHotbarSlot{ ASSET_PATH"hud/slot.png" }, m_texInv{ ASSET_PATH"hud/inventory.png" }, m_texInvSlot{ ASSET_PATH"hud/slot_inventory.png" }, m_font{ FONT_PATH"pixelated.ttf" },
	m_hotbar{ m_texHotbarSlot, itm, &m_font }, m_inventory{ m_texInv, m_texInvSlot, &m_font },
	BasicEntity{ {}, {{ 0.25, 1.75, 0.25 }, { 0.25, 0.20, 0.25 }} }
{
}


// =====================
// Actors
// =====================

void entities::Player::update(const Window& window, const World& world, const ItemTypeManager& itm, float deltaTime) noexcept
{
	updatePosition(world, deltaTime);
	resolve_collisions(world);

	m_inventory.update(window, itm, m_hotbar);
}

void entities::Player::updatePosition(const World& world, float deltaTime) noexcept
{
	if (!attributes.flags.flying)
		m_velocity.y += world.settings.gravity * deltaTime;


	if (vec2f{ m_velocity.x, m_velocity.z }.length() > attributes.physics.maxSpeed)
	{
		const auto tempY{ m_velocity.y };

		m_velocity = vec3f{ m_velocity.x, 0, m_velocity.z }.normal() * attributes.physics.maxSpeed;
		m_velocity.y = tempY;
	}


	if (!attributes.flags.moving)
	{
		m_velocity.x *= (1 - attributes.physics.friction * deltaTime);
		m_velocity.z *= (1 - attributes.physics.friction * deltaTime);
	}

	if (!attributes.flags.flying)
		Transformable3D::move(m_velocity * deltaTime);
	else
		Transformable3D::move(vec3f{ m_velocity.x, 0, m_velocity.z } * deltaTime);

}

void entities::Player::resolve_collisions(const World& world) noexcept
{
	if (attributes.flags.ghost)
		return;

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
			physics::collisions::BasicHitbox cube{ static_cast<vec3f>(pos), static_cast<vec3f>(pos + types::loc{1}) };

			if (m_hitbox.intersects(cube))
			{
				auto offset = m_hitbox.findIntersection(cube);

				if (world.block_at(pos - types::loc{ offset.normal() }))
					offset = {};

				if (offset.y != 0)
					m_velocity.y = 0;

				if (offset.x != 0)
					m_velocity.x = 0;

				if (offset.z != 0)
					m_velocity.z = 0;

				Transformable3D::move(-offset);
			}
		}
}

bool entities::Player::addItem(const Data::Item& item, int64 count, const ItemTypeManager& itm) noexcept
{
	if (!m_hotbar.addItem({ item.id }, count, itm))
	{
		if (m_inventory.addItem({ item.id }, count, itm))
			return true;
	}
	else
		return true;
}

bool entities::Player::removeItem(const Data::Item& item, int64 count, const ItemTypeManager& itm) noexcept
{
	return false;
}


const Data::Item& entities::Player::place_voxel() noexcept
{
	const auto& item = m_hotbar.getSelectedItem();
	m_hotbar.removeItem(item, 1);
	return item;
}

void entities::Player::draw_attributes(const render::Shader& shader, const render::Shader& text_shader, const render::Texture& gui_block_atlas, const ItemTypeManager& itm) noexcept
{
	m_hotbar.draw(shader, text_shader, m_texHotbarSlot, gui_block_atlas, itm);
	m_inventory.draw(shader, text_shader, m_texInv, m_texInvSlot, gui_block_atlas, itm);
}


// =====================
// Mutators
// =====================

void entities::Player::move(const Direction& dir, const render::utils::Camera& cam, float deltaTime) noexcept
{
	const auto speed{ attributes.physics.speed * deltaTime };

	switch (dir)
	{
	case Forward:
		m_velocity += vec3f{ cam.front_dir.x, 0.f, cam.front_dir.z }.normal() * speed;
		break;

	case Backward:
		m_velocity -= vec3f{ cam.front_dir.x, 0.f, cam.front_dir.z }.normal() * speed;
		break;

	case Right:
		m_velocity += vec3f{ cam.right_dir().x, 0.f, cam.right_dir().z}.normal() * speed;
		break;

	case Left:
		m_velocity -= vec3f{ cam.right_dir().x, 0.f, cam.right_dir().z }.normal() * speed;
		break;

	case Downward:
		if (attributes.flags.flying)
			Transformable3D::move(-vec3f{ 0, attributes.physics.jumpHeight, 0 } * deltaTime);
		break;

	case Upward:
		if (!attributes.flags.flying)
			m_velocity.y += attributes.physics.jumpHeight;
		else
			Transformable3D::move(vec3f{ 0, attributes.physics.jumpHeight, 0 } * deltaTime);
		break;
	}
	attributes.flags.moving = true;
}

void entities::Player::resetMovement() noexcept
{
	attributes.flags.moving = false;
}