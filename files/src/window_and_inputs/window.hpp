#pragma once // window.hpp
// MIT
// Allosker ------------------------------
// =========+
// Define a Window class for easier management of ressources
// ---------------------------------------

#include "uHeaders/opengl.hpp"
#include <string>

namespace wai
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

		void close() noexcept
		{
			glfwSetWindowShouldClose(m_window, true);
		}

		void display() noexcept
		{
			glfwSwapBuffers(m_window);
		}

		void clearEvents() noexcept
		{
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


	private:



		GLFWwindow* m_window{ nullptr };

		mpml::Vector2<int> m_size{};
	};

} // WAI