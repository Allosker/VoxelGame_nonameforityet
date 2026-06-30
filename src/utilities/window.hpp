#pragma once // window.hpp
// MIT
// Allosker ------------------------------
// =========+
// Define a Window class for easier management of ressources
// Defines all keyboard inputs needed and different mods for them
// ---------------------------------------

#include "utilities/opengl.hpp"
#include "inputs.hpp"
#include "utilities/time/clock.hpp"
#include "utilities/event.hpp"

#include <string>
#include <array>
#include <functional>
#include <queue>


class Window
{
public:


	// = Construction/Initialization

	explicit Window(const vec2i& size_, const std::string& name, GLFWmonitor* monitor, GLFWwindow* share);

	~Window() noexcept;

	
	// = Actors

	void close() const noexcept { glfwSetWindowShouldClose(m_window, true); }

	void display() const noexcept { glfwSwapBuffers(m_window); }

	void clearStates() noexcept;

	void processInputs(const std::function<void()>& inputs);

	bool alternateCursorVisibility() noexcept;

	std::optional<Event> pollEvent() noexcept
	{
		if(!m_queue_events.empty())
		{
			Event temp{ m_queue_events.front() };

			m_queue_events.pop();

			return std::make_optional(temp);
		}

		return std::nullopt;
	}
	
	template<typename EventSubType>
	void addEvent(const EventSubType& type) noexcept
	{
		m_queue_events.push(type);
	}


	// = Getters

	GLFWwindow* get() noexcept { return m_window; }
	const GLFWwindow* const get() const noexcept { return m_window; }

	const mpml::Vector2<int>& getSize() const noexcept { return m_size; }


	// = Predicates

	bool isOpen() const noexcept { return !glfwWindowShouldClose(m_window); }

	bool isCursorHidden() const noexcept { return m_cursorHidden; }

	bool isKeyPressed(Keys key) const noexcept { return glfwGetKey(m_window, static_cast<int>(key)) == GLFW_PRESS; }
	bool isKeyReleased(Keys key) const noexcept { return glfwGetKey(m_window, static_cast<int>(key)) == GLFW_RELEASE; }

	bool isMouseButtonPressed(MouseButtons button) const noexcept { return glfwGetMouseButton(m_window, static_cast<int>(button)) == GLFW_PRESS; }
	bool isMouseButtonReleased(MouseButtons button) const noexcept { return glfwGetMouseButton(m_window, static_cast<int>(button)) == GLFW_RELEASE; }


	// = Setters

	void resize(const mpml::Vector2<int>& new_size) noexcept;


public:

	static vec2f toGUICoordinates(const Window& window, vec2f point) noexcept;

	static constexpr vec2f g_guiViewSize{ 1920.f,1080.f };
		 

private:

	bool m_cursorHidden{ true };

	vec2i m_size{};

	GLFWwindow* m_window{ nullptr };

	std::queue<Event> m_queue_events{};

};