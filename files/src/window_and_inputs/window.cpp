#include "window.hpp"

// =====================
// Construction/Destruction
// =====================


Wai::Window::Window(const mpml::Vector2<int>& size_, const std::string& name, GLFWmonitor* monitor, GLFWwindow* share)
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_window = glfwCreateWindow(size_.x, size_.y, name.c_str(), monitor, share);

	if (!m_window)
		throw std::runtime_error("ERROR::WINDOW_CREATION::Window or context creation failded");


	glfwMakeContextCurrent(m_window);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
		throw std::runtime_error("ERROR::WINDOW_CREATION::Couldn't set glad proc address properly");

	glfwGetFramebufferSize(m_window, &m_size.x, &m_size.y);
	glViewport(0, 0, m_size.x, m_size.y);
}


Wai::Window::~Window() noexcept
{
	glfwDestroyWindow(m_window);
}


