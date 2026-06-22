#include "window.hpp"


// =====================
// Construction/Destruction
// =====================

Window::Window(const vec2i& size_, const std::string& name, GLFWmonitor* monitor, GLFWwindow* share)
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
	updateKeys();
	updateMouseButtons();
	m_mouseWheel_delta = vec2f{0};
	m_wheelScrolledThisFrame = false;
	m_dirChangedThisFrame = false;
	m_wasFrameBufferResized = false;
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

void Window::onFramebufferResize(vec2i newSize) noexcept
{
	m_wasFrameBufferResized = true;
	m_size = newSize;
	glViewport(0, 0, newSize.x, newSize.y);
}

void Window::onMouseMovement(vec2f offset) noexcept
{
	static float yaw{ -90 }, pitch{};

	yaw += offset.x;
	pitch += offset.y;


	if (pitch > 89.f)
		pitch = 89.f;
	if (pitch < -89.f)
		pitch = -89.f;

	vec3f direction{};

	float radPitch{ mpml::toRadians(pitch) };
	float cosPitch{ std::cos(radPitch) };
	float radYaw{ mpml::toRadians(yaw) };

	direction.x = std::cos(radYaw) * cosPitch;
	direction.y = -std::sin(radPitch);
	direction.z = std::sin(radYaw) * cosPitch;

	m_newDir = direction.normal();
	m_dirChangedThisFrame = true;
}

void Window::onMouseWheelScroll(vec2f delta) noexcept
{
	m_mouseWheel_delta = delta;
}

void Window::onMouseCursorPosChange(vec2f newCursorPos) noexcept
{
	m_mousePos = newCursorPos;
}


// =====================
// Getters
// =====================

std::uint32_t Window::getMods() const noexcept
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

bool Window::isKeyPressedOnce(int key) const noexcept
{
	m_keyDowns[key] = glfwGetKey(m_window, key) == Buttons::Pressed;

	return m_keyDowns[key] && !m_lastKeyDowns[key];
}

bool Window::isMouseButtonPressedOnce(int key) const noexcept
{
	m_mouseButtonsDown[key] = glfwGetMouseButton(m_window, key) == Buttons::Pressed;

	return m_mouseButtonsDown[key] && !m_lastMouseButtonsDown[key];
}


// =====================
// Setters
// =====================

void Window::resize(const mpml::Vector2<int>& new_size) noexcept
{
	glfwSetWindowSize(m_window, new_size.x, new_size.y);
	m_size = new_size;
}

vec2f Window::toGUICoordinates(const Window& window, vec2f point) noexcept
{
	vec2f mult1{ point - vec2f(window.m_size / 2) };
	vec2f mult2{ g_guiViewSize.x / window.m_size.x,  g_guiViewSize.y / -window.m_size.y };

	return { mult1.x * mult2.x, mult1.y * mult2.y };
}




