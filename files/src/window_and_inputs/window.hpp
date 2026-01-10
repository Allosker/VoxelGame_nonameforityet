#pragma once // window.hpp
// MIT
// Allosker ------------------------------
// =========+
// Define a Window class for easier management of ressources
// Defines all keyboard inputs needed and different mods for them
// ---------------------------------------

#include "uHeaders/opengl.hpp"
#include "inputs.hpp"
#include "utilities/time/clock.hpp"

#include <string>
#include <array>

namespace Wai
{
	class Window
	{
	public:



		explicit Window(const mpml::Vector2<int>& size_, const std::string& name, GLFWmonitor* monitor, GLFWwindow* share);


		~Window() noexcept;

		bool isOpen() const noexcept
		{
			return !glfwWindowShouldClose(m_window);
		}

		const mpml::Vector2<int>& getSize() const noexcept
		{
			return m_size;
		}

		void close() const noexcept
		{
			glfwSetWindowShouldClose(m_window, true);
		}

		void display() const noexcept
		{
			glfwSwapBuffers(m_window);
		}

		void clearEvents() const noexcept
		{
			updateKeys();
			glfwPollEvents();
		}

		GLFWwindow* get() noexcept
		{
			return m_window;
		}

		void resize(const mpml::Vector2<int>& new_size) noexcept
		{
			glfwSetWindowSize(m_window, new_size.x, new_size.y);
			m_size = new_size;
		}

		std::uint32_t getMods() const noexcept
		{
			using b = Buttons;

			std::uint32_t mask{};

			if (glfwGetKey(m_window, b::Left_shift) || glfwGetKey(m_window, b::Right_shift))
				mask |= b::Shift;

			if (glfwGetKey(m_window, b::Left_control) || glfwGetKey(m_window, b::Right_control))
				mask |= b::Control;

			if (glfwGetKey(m_window, b::Left_alt) || glfwGetKey(m_window, b::Right_alt))
				mask |= b::Alt;

			if (glfwGetKey(m_window, b::Left_super) || glfwGetKey(m_window, b::Right_super))
				mask |= b::Super;

			return mask;
		}

		bool modsSet(int mods) const noexcept
		{
			return getMods() & mods;
		}

		std::uint8_t getKey(int key) const noexcept
		{
			return glfwGetKey(m_window, key);	
		}

		void updateKeys() const noexcept
		{
			m_lastKeyDowns = m_keyDowns;
		}

		bool keyPressed(int key) const noexcept
		{
			return glfwGetKey(m_window, key) == Buttons::Pressed;
		}

		bool keyPressedOnce(int key) const noexcept
		{
			m_keyDowns[key] = glfwGetKey(m_window, key) == Buttons::Pressed;

			return m_keyDowns[key] && !m_lastKeyDowns[key];
		}

		bool keyReleased(int key) const noexcept
		{
			return glfwGetKey(m_window, key) == Buttons::Released;
		}

	private:

		// Make that better
		mutable std::array<bool, 348> m_keyDowns{};
		mutable std::array<bool, 348> m_lastKeyDowns{};



		mpml::Vector2<int> m_size{};

		GLFWwindow* m_window{ nullptr };

	};

} // WAI