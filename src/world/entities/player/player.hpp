#pragma once // player.hpp
// MIT
// Allosker ------------------------------
// =========+
// Basic class for Player, this manages everything about the player and offers a higher level of abstraction
// ---------------------------------------



#include "camera.hpp"

#include "rendering/shader.hpp"

#include "rendering/gui/hotbar.hpp"
#include "rendering/gui/inventory.hpp"

#include "world/types/itemTypeManager.hpp"

#include "physics/collisions/basicHitbox.hpp"
#include "world/entities/basicEntity.hpp"


class World;

namespace entities
{

	class Player
		: public BasicEntity
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
			const ItemTypeManager& itm,
			const types::path& p_font
		);

		// = Actors

		void update(const Window& window, const World& world, const ItemTypeManager& itm, float deltaTime) noexcept;

		void updatePosition(const World& world, float deltaTime) noexcept;

		void resolve_collisions(const World& world) noexcept;

		// Trasnform to pick up item with item chunk manager or smth
		bool addItem(const Data::Item& item, int64 count, const ItemTypeManager& itm) noexcept;

		bool removeItem(const Data::Item& item, int64 count, const ItemTypeManager& itm) noexcept;

		const Data::Item& place_voxel() noexcept;

		void draw_attributes(
			const render::Shader& shader,
			const render::Shader& text_shader,
			const render::Texture& gui_block_atlas,
			const ItemTypeManager& itm) noexcept;


		// = Mutators

		void move(const Direction& dir, float deltaTime) noexcept;

		void resetMovement() noexcept;


		// = Getters

		const vec3f& getVelocity() const noexcept { return m_velocity; }

		const vec3f& getPos() const noexcept { return m_camera.pos; }


		render::utils::Camera& getCamera() noexcept { return m_camera; }
		const render::utils::Camera& getCamera() const noexcept { return m_camera; }

		render::gui::Inventory& getInventory() noexcept { return m_inventory; }
		const render::gui::Inventory& getInventory() const noexcept { return m_inventory; }

		render::gui::Hotbar& getHotbar() noexcept { return m_hotbar; }
		const render::gui::Hotbar& getHotbar() const noexcept { return m_hotbar; }


	private:


		void p_move(const vec3f& offset) noexcept;


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

		render::utils::Camera				m_camera{};

		render::Texture			m_texHotbarSlot;
		render::Texture			m_texInv;
		render::Texture			m_texInvSlot;

		render::Font						m_font;

		render::gui::Inventory				m_inventory;
		render::gui::Hotbar					m_hotbar;


		vec3f								m_velocity{};

	};
}