#pragma once // transform2D.hpp
// MIT
// Allosker ------------------------------
// =========+
// Basic 2D transform class passing 4x4 transform matrices
// ---------------------------------------

#include "utilities/opengl.hpp"

namespace physics
{

	class Transformable2D
	{
	public:

		Transformable2D() = delete;

		Transformable2D(vec2f size, vec2f ori)
			: m_origin{ ori }, m_baseSize{ size }, m_scale{ 1.f, 1.f }
		{
			if (m_origin.x != 0 || m_origin.y != 0)
				m_transformNeedUpdate = true;
		}

		Transformable2D(Transformable2D&&) = delete;
		Transformable2D(Transformable2D&) = default;

		Transformable2D& operator=(Transformable2D&&) = delete;
		Transformable2D& operator=(Transformable2D&) = default;

		virtual ~Transformable2D() = default;


		// = Getters

		const mpml::Matrix4<float>& getTransformation() noexcept
		{
			// Recompute the combined transform if needed
			if (m_transformNeedUpdate)
			{
				const float angle = -m_rotation.asRadians();
				const float cosine = std::cos(angle);
				const float sine = std::sin(angle);
				const float sxc = m_scale.x * cosine;
				const float syc = m_scale.y * cosine;
				const float sxs = m_scale.x * sine;
				const float sys = m_scale.y * sine;
				const float tx = -m_origin.x * sxc - m_origin.y * sys + m_position.x;
				const float ty = m_origin.x * sxs - m_origin.y * syc + m_position.y;

				m_transformations = mpml::Matrix4<float>
				{
					sxc ,	sxs,	0.f, 0.f,
					-sys,	syc,	0.f, 0.f,
					0.f ,	0.f,	1.f, 0.f,
					tx  ,	ty ,	0.f, 1.f
				};

				m_transformNeedUpdate = false;
			}

			return m_transformations;
		}

		vec2f getSize() const noexcept { return { m_baseSize.x * m_scale.x, m_baseSize.y * m_scale.y }; }

		mpml::Angle<> getRotation() const noexcept { return m_rotation; }

		vec2f getPosition() const noexcept { return m_position; }
	

		// = Setters

		virtual void setPosition(vec2f pos) noexcept
		{
			m_position = pos;
			m_transformNeedUpdate = true;
		}

		virtual void setScale(vec2f scale) noexcept
		{
			m_scale = scale;
			m_transformNeedUpdate = true;
		}

		virtual void setSize(vec2f size) noexcept
		{
			setScale({ size.x / m_baseSize.x, size.y / m_baseSize.y });
		}

		virtual void setRotation(mpml::Angle<> rotation) noexcept
		{
			m_rotation = rotation;
			m_transformNeedUpdate = true;
		}


		void move(vec2f offset) noexcept
		{
			setPosition(m_position + offset);
		}

		void scale(vec2f factor) noexcept
		{
			setScale({ m_scale.x * factor.x, m_scale.y * factor.y });
		}

		void rotate(mpml::Angle<> theta) noexcept
		{
			setRotation(m_rotation + theta);
		}


	protected:

		mat4f				m_transformations{ mpml::Identity4<float> };

		vec2f				m_scale{};
		vec2f				m_baseSize{};
		vec2f				m_origin{};
		vec2f				m_position{};

		mpml::Angle<>		m_rotation{ mpml::Angle<>::fromRadians(0) };

		bool				m_transformNeedUpdate{ false };

	};

}