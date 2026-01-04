#pragma once // shader.hpp
// MIT
// Allosker ------------------------------
// =========+
// Get a shader up and running like it should
// ---------------------------------------

#include "uHeaders/opengl.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <string_view>

namespace render
{

	class Shader
	{
	public: 

	// = Construction/Destruction

		Shader(const std::filesystem::path& vertShader, const std::filesystem::path& fragShader, const std::filesystem::path& geomShader = "");

		~Shader() noexcept;


	// = Actiors

		void use() const noexcept;


	// = Getters

		std::uint32_t ID() const noexcept;

		std::int32_t getUniformLocation(const std::string& name) const noexcept;


	// = Setters

		void setValue(const std::string& name, float value) const noexcept;

		void setValue(const std::string& name, const vec3f& value) const noexcept;
		void setValue(const std::string& name, const vec4f& value) const noexcept;

		void setValue(const std::string& name, const mat3f& value) const noexcept;
		void setValue(const std::string& name, const mat4f& value) const noexcept;


	private:

		void compile(std::uint32_t s_id, const std::string& name);

		void link(std::uint32_t s_id);

		std::uint32_t m_id;

	};


}