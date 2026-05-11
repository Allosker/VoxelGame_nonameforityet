#pragma once
// MIT
// Allosker ------------------------------
// =========+
// Basic 2D transform class passing 4x4 transform matrices
// ---------------------------------------

#include "uHeaders/opengl.hpp"
#include "mpml/quaternions/quaternion.hpp"

namespace
{

	class Transform3D
	{
	public:

		Transform3D() = delete;

		Transform3D(const vec3f& size, const vec3f& ori)
			: m_origin{ ori }, m_baseSize{ size }, m_scale{ 1.f, 1.f, 1.f }
		{
			if (m_origin.x != 0 || m_origin.y != 0)
				m_transformNeedUpdate = true;
		}

		Transform3D(Transform3D&&) = delete;
		Transform3D(Transform3D&) = default;

		Transform3D& operator=(Transform3D&&) = delete;
		Transform3D& operator=(Transform3D&) = default;

		virtual ~Transform3D() = default;


		// = Getters

		const mpml::Matrix4<float>& getTransformation() noexcept
		{
			if (m_transformNeedUpdate)
			{
				mpml::Matrix4<float> transforms{ mpml::Identity4<float> };

				transforms = mpml::scale(transforms, m_scale);
				transforms = mpml::rotate(transforms, m_rotation);
				transforms = mpml::translate(transforms, m_position);

				m_transformations = transforms;

				m_transformNeedUpdate = false;
			}

			return m_transformations;
		}

		const vec3f& getSize() const noexcept { return { m_baseSize.x * m_scale.x, m_baseSize.y * m_scale.y, m_baseSize.z * m_scale.z }; }

		mpml::Quaternion<float> getRotation() const noexcept { return m_rotation; }

		const vec3f& getPosition() const noexcept { return m_position; }


		// = Setters

		void setPosition(const vec3f& pos) noexcept
		{
			m_position = pos;
			m_transformNeedUpdate = true;
		}

		void setScale(const vec3f& scale) noexcept
		{
			m_scale = scale;
			m_transformNeedUpdate = true;
		}

		void setSize(const vec3f& size) noexcept
		{
			setScale({ size.x / m_baseSize.x, size.y / m_baseSize.y, size.z / m_baseSize.z });
			m_transformNeedUpdate = true;
		}

		void setRotation(const mpml::Quaternion<float>& q) noexcept
		{
			m_rotation = q;
			m_transformNeedUpdate = true;
		}

		void move(const vec3f& offset) noexcept
		{
			setPosition(m_position + offset);
		}

		void scale(const vec3f& factor) noexcept
		{
			setScale({ m_scale.x * factor.x, m_scale.y * factor.y, m_scale.z * factor.z });
		}

		void rotate(const mpml::Quaternion<float>& q) noexcept
		{
			setRotation(m_rotation * q);
		}


	protected:

		mpml::Matrix4<float>				m_transformations{ mpml::Identity4<float> };


		vec3f								m_scale{};
		vec3f								m_baseSize{};
		vec3f								m_origin{};
		vec3f								m_position{};

		mpml::Quaternion<float>				m_rotation{ 1.f, 0, 0, 0 };

		bool								m_transformNeedUpdate{ false };

	};


} // global