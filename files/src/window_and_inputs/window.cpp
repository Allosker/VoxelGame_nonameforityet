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
		throw std::runtime_error("ERROR::WINDOW_CREATION::Window or context creation failed");


	glfwMakeContextCurrent(m_window);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
		throw std::runtime_error("ERROR::WINDOW_CREATION::Couldn't set glad proc address properly");

	glfwSetWindowUserPointer(m_window, this);

	glfwGetFramebufferSize(m_window, &m_size.x, &m_size.y);
	glViewport(0, 0, m_size.x, m_size.y);
}


Wai::Window::~Window() noexcept
{
	glfwDestroyWindow(m_window);
}


// =====================
// Actors
// =====================

void Wai::Window::clearEvents() const noexcept
{
	updateKeys();
	updateMouseButtons();
	glfwPollEvents();
}


// =====================
// CallBacks
// =====================

void Wai::Window::updateInputs(const std::function<void()>& inputs)
{
	inputs();
}

void Wai::Window::onFramebufferResize(const vec2i& newSize) noexcept
{
	m_size = newSize;
	glViewport(0, 0, newSize.x, newSize.y);
}


// =====================
// Getters
// =====================

std::uint32_t Wai::Window::getMods() const noexcept
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


// =====================
// Predicates
// =====================

bool Wai::Window::isKeyPressedOnce(int key) const noexcept
{
	m_keyDowns[key] = glfwGetKey(m_window, key) == Buttons::Pressed;

	return m_keyDowns[key] && !m_lastKeyDowns[key];
}

bool Wai::Window::isMouseButtonPressedOnce(int key) const noexcept
{
	m_mouseButtonsDown[key] = glfwGetMouseButton(m_window, key) == Buttons::Pressed;

	return m_mouseButtonsDown[key] && !m_lastMouseButtonsDown[key];
}


// =====================
// Setters
// =====================

void Wai::Window::resize(const mpml::Vector2<int>& new_size) noexcept
{
	glfwSetWindowSize(m_window, new_size.x, new_size.y);
	m_size = new_size;
}




