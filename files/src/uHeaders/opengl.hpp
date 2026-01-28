#pragma once // opengl.hpp
// MIT
// Allosker ------------------------------
// =========+
// Define what is needed to use openGL
// ---------------------------------------

#define GLFW_INCLUDE_NONE
#include "GLAD/glad/glad.h"
#include <GLFW/glfw3.h>
#include <mpml/mpml.hpp>

#include <cstdint>
#include <stdexcept>

#include <filesystem>



using vec2f = mpml::Vector2<float>;
using vec3f = mpml::Vector3<float>;
using vec4f = mpml::Vector4<float>;

using vec3i = mpml::Vector3<int>;

using mat2f = mpml::Matrix2<float>;
using mat3f = mpml::Matrix3<float>;
using mat4f = mpml::Matrix4<float>;