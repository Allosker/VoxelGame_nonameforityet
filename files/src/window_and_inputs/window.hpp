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

#include "rendering/utilities/camera.hpp"

#include <string>
#include <array>
#include <functional>

namespace Wai
{
	class Window
	{
	public:


		// = Construction/Initialization

		explicit Window(const mpml::Vector2<int>& size_, const std::string& name, GLFWmonitor* monitor, GLFWwindow* share);

		~Window() noexcept;

	
		// = Actors

		void close() const noexcept { glfwSetWindowShouldClose(m_window, true); }

		void display() const noexcept { glfwSwapBuffers(m_window); }

		void clearEvents() const noexcept;

		void updateKeys() const noexcept { m_lastKeyDowns = m_keyDowns; }

		void updateMouseButtons() const noexcept { m_lastMouseButtonsDown = m_mouseButtonsDown; }

		void updateInputs(const std::function<void()>& inputs);


		// = CallBacks

		void onFramebufferResize(const vec2i& newSize) noexcept;


		// = Getters

		GLFWwindow* get() noexcept { return m_window; }
		const GLFWwindow* const get() const noexcept { return m_window; }

		const mpml::Vector2<int>& getSize() const noexcept { return m_size; }

		std::uint8_t getKeyState(int key) const noexcept { return glfwGetKey(m_window, key); }

		std::uint32_t getMods() const noexcept;


		// = Predicates

		bool isOpen() const noexcept { return !glfwWindowShouldClose(m_window); }

		bool are_mods_set(int mods) const noexcept { return getMods() & mods; }


		bool isKeyPressedOnce(int key) const noexcept;

		bool isKeyPressed(int key) const noexcept { return glfwGetKey(m_window, key) == Buttons::Pressed; }
		bool isKeyReleased(int key) const noexcept { return glfwGetKey(m_window, key) == Buttons::Released; }


		bool isMouseButtonPressedOnce(int key) const noexcept;

		bool isMouseButtonPressed(int button) const noexcept { return glfwGetMouseButton(m_window, button) == Buttons::Pressed; }
		bool isMouseButtonReleased(int button) const noexcept { return glfwGetMouseButton(m_window, button) == Buttons::Released; }


		// = Setters

		void resize(const mpml::Vector2<int>& new_size) noexcept;


	private:

		// Make that better
		mutable std::array<bool, 348> m_keyDowns{};
		mutable std::array<bool, 348> m_lastKeyDowns{};

		mutable std::array<bool, 8> m_mouseButtonsDown{};
		mutable std::array<bool, 8> m_lastMouseButtonsDown{};



		vec2i m_size{};

		GLFWwindow* m_window{ nullptr };

	};

} // WAI