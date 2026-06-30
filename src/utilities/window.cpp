#include "window.hpp"


// =====================
// Construction/Destruction
// =====================

Window::Window(const vec2i& size_, const std::string& name, GLFWmonitor* monitor, GLFWwindow* share)
	: m_size{ size_ }
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_window = glfwCreateWindow(size_.x, size_.y, name.c_str(), monitor, share);

	if (!m_window)
		throw std::runtime_error("ERROR::WINDOW_CREATION::Window or context creation failed");


	glfwMakeContextCurrent(m_window);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
		throw std::runtime_error("ERROR::WINDOW_CREATION::Couldn't set glad proc address properly");

	glfwSetWindowUserPointer(m_window, this);

	glfwGetFramebufferSize(m_window, &m_size.x, &m_size.y);
	glViewport(0, 0, m_size.x, m_size.y);
}


Window::~Window() noexcept
{
	glfwDestroyWindow(m_window);
}


// =====================
// Actors
// =====================

void Window::clearStates() noexcept
{
	glfwPollEvents();
}


// =====================
// CallBacks
// =====================

void Window::processInputs(const std::function<void()>& inputs)
{
	inputs();
}

bool Window::alternateCursorVisibility() noexcept
{
	m_cursorHidden = !m_cursorHidden;

	glfwSetInputMode(m_window, GLFW_CURSOR, m_cursorHidden ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	return m_cursorHidden;
}


// =====================
// Setters
// =====================

void Window::resize(const mpml::Vector2<int>& new_size) noexcept
{
	glfwSetWindowSize(m_window, new_size.x, new_size.y);
	glViewport(0, 0, new_size.x, new_size.y);
	m_size = new_size;
}

vec2f Window::toGUICoordinates(const Window& window, vec2f point) noexcept
{
	vec2f mult1{ point - vec2f(window.m_size / 2) };
	vec2f mult2{ g_guiViewSize.x / window.m_size.x,  g_guiViewSize.y / -window.m_size.y };

	return { mult1.x * mult2.x, mult1.y * mult2.y };
}




