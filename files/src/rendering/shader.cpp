#include "shader.hpp"

// =====================
// Construction/Destruction
// =====================

Render::Shader::Shader(const std::filesystem::path& vertShader, const std::filesystem::path& fragShader, const std::filesystem::path& geomShader)
{
	std::string vertCode{}, fragCode{}, geomCode{};

	std::ifstream vSFile{}, fSFile{}, gSFile{};

	vSFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fSFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gSFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	// Read Content from Files
	try
	{
		// Open Files
		vSFile.open(vertShader);
		fSFile.open(fragShader);
		if (!geomShader.empty())
			gSFile.open(geomShader);

		// Move Streams
		std::stringstream vSStream{}, fSStream{}, gSStream{};

		vSStream << vSFile.rdbuf();
		fSStream << fSFile.rdbuf();
		if (!geomShader.empty())
			gSStream << gSFile.rdbuf();

		// CLose Files
		vSFile.close();
		fSFile.close();
		if (!geomShader.empty())
			gSFile.close();

		// Get Actual Code
		vertCode = vSStream.str();
		fragCode = fSStream.str();
		if (!geomShader.empty())
			geomCode = gSStream.str();

	}
	catch (...)
	{
		throw;
	}

	try
	{

		const char* vCode{ vertCode.c_str() };
		const char* fCode{ fragCode.c_str() };
		const char* gCode{ geomCode.c_str() };

		// Create
		std::uint32_t v_ID{ glCreateShader(GL_VERTEX_SHADER) }, f_ID{ glCreateShader(GL_FRAGMENT_SHADER) }, g_ID{};
		if (!geomShader.empty())
			g_ID = glCreateShader(GL_GEOMETRY_SHADER);

		glShaderSource(v_ID, 1, &vCode, nullptr);
		glShaderSource(f_ID, 1, &fCode, nullptr);
		if (!geomShader.empty())
			glShaderSource(g_ID, 1, &gCode, nullptr);


		// Compile
		compile(v_ID, "vertex");
		compile(f_ID, "fragment");
		if (!geomShader.empty())
			compile(g_ID, "geometry");


		// Link Shaders 
		m_id = glCreateProgram();

		glAttachShader(m_id, v_ID);
		glAttachShader(m_id, f_ID);
		if (!geomShader.empty())
			glAttachShader(m_id, g_ID);

		link(m_id);


		// Delete
		glDeleteShader(v_ID);
		glDeleteShader(f_ID);
		if (!geomShader.empty())
			glDeleteShader(g_ID);
	}
	catch (...)
	{
		throw;
	}
}

Render::Shader::~Shader() noexcept
{
	glDeleteProgram(m_id);
}


// =====================
// Actors
// =====================

void Render::Shader::use() const noexcept
{
	glUseProgram(m_id);
}


// =====================
// Getters
// =====================

std::uint32_t Render::Shader::ID() const noexcept
{
	return m_id;
}

std::int32_t Render::Shader::getUniformLocation(const std::string& name) const noexcept
{
	return glGetUniformLocation(m_id, name.c_str());
}


// =====================
// Setters
// =====================

void Render::Shader::setValue(const std::string& name, float value) const noexcept
{
	glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void Render::Shader::setValue(const std::string& name, const vec3f& value) const noexcept
{
	glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, value.data_ptr());
}

void Render::Shader::setValue(const std::string& name, const vec4f& value) const noexcept
{
	glUniform4fv(glGetUniformLocation(m_id, name.c_str()), 1, value.data_ptr());
}

void Render::Shader::setValue(const std::string& name, const mat3f& value) const noexcept
{
	glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()), 1, false, value.data_ptr());
}

void Render::Shader::setValue(const std::string& name, const mat4f& value) const noexcept
{
	glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, false, value.data_ptr());
}


// =====================
// Private
// =====================

void Render::Shader::compile(std::uint32_t s_id, const std::string& name)
{
	glCompileShader(s_id);

	std::int32_t success;
	char infoLog[512];

	glGetShaderiv(s_id, GL_COMPILE_STATUS, &success); 

	if (!success)
	{
		glGetShaderInfoLog(s_id, 512, nullptr, infoLog);

		throw std::runtime_error("ERROR::SHADER::COMPILATION_FAILED::" + name + '\n' + infoLog + '\n');
	}
}

void Render::Shader::link(std::uint32_t s_id)
{
	glLinkProgram(s_id);

	std::int32_t success;
	char infoLog[512];

	glGetShaderiv(s_id, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(s_id, 512, nullptr, infoLog);

		throw std::runtime_error(std::string{"ERROR::SHADER::LINKAGE_FAILED::Shader_Program::"} + infoLog + '\n');
	}
}
