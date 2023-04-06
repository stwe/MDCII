// This file is part of the MDCII project.
//
// Copyright (c) 2022. stwe <https://github.com/stwe/MDCII>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#include <glm/gtc/type_ptr.hpp>
#include "ShaderProgram.h"
#include "ogl/OpenGL.h"
#include "MdciiAssert.h"
#include "ResourceUtil.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::ogl::resource::ShaderProgram::ShaderProgram(std::string t_path)
    : m_path{ std::move(t_path) }
{
    Log::MDCII_LOG_DEBUG("[ShaderProgram::ShaderProgram()] Create ShaderProgram for {}.", m_path);

    Init();
}

mdcii::ogl::resource::ShaderProgram::~ShaderProgram() noexcept
{
    Log::MDCII_LOG_DEBUG("[ShaderProgram::~ShaderProgram()] Destruct ShaderProgram.");

    CleanUp();
}

//-------------------------------------------------
// Bind / unbind
//-------------------------------------------------

void mdcii::ogl::resource::ShaderProgram::Bind() const
{
    glUseProgram(id);
}

void mdcii::ogl::resource::ShaderProgram::Unbind()
{
    glUseProgram(0);
}

//-------------------------------------------------
// Set uniforms
//-------------------------------------------------

void mdcii::ogl::resource::ShaderProgram::SetUniform(const std::string& t_uniformName, const int32_t t_value) const
{
    glUniform1i(m_uniforms.at(t_uniformName), t_value);
}

void mdcii::ogl::resource::ShaderProgram::SetUniform(const std::string& t_uniformName, const float t_value) const
{
    glUniform1f(m_uniforms.at(t_uniformName), t_value);
}

void mdcii::ogl::resource::ShaderProgram::SetUniform(const std::string& t_uniformName, const bool t_value) const
{
    // if value == true load 1 else 0 as float
    glUniform1f(m_uniforms.at(t_uniformName), t_value ? 1.0f : 0.0f);
}

void mdcii::ogl::resource::ShaderProgram::SetUniform(const std::string& t_uniformName, const glm::vec2& t_value) const
{
    glUniform2f(m_uniforms.at(t_uniformName), t_value.x, t_value.y);
}

void mdcii::ogl::resource::ShaderProgram::SetUniform(const std::string& t_uniformName, const glm::vec3& t_value) const
{
    glUniform3f(m_uniforms.at(t_uniformName), t_value.x, t_value.y, t_value.z);
}

void mdcii::ogl::resource::ShaderProgram::SetUniform(const std::string& t_uniformName, const glm::vec4& t_value) const
{
    glUniform4f(m_uniforms.at(t_uniformName), t_value.x, t_value.y, t_value.z, t_value.w);
}

void mdcii::ogl::resource::ShaderProgram::SetUniform(const std::string& t_uniformName, const glm::mat4& t_value) const
{
    glUniformMatrix4fv(m_uniforms.at(t_uniformName), 1, GL_FALSE, value_ptr(t_value));
}

void mdcii::ogl::resource::ShaderProgram::SetUniform(const std::string& t_uniformName, const glm::mat3& t_value) const
{
    glUniformMatrix3fv(m_uniforms.at(t_uniformName), 1, GL_FALSE, value_ptr(t_value));
}

void mdcii::ogl::resource::ShaderProgram::SetUniform(const std::string& t_uniformName, const std::vector<int32_t>& t_container) const
{
    const auto& m{ m_arrayUniformNames.at(t_uniformName) };

    auto c{ 0 };
    for (const auto& value : m)
    {
        glUniform1i(m_uniforms.at(value), t_container[c]);
        c++;
    }
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::ogl::resource::ShaderProgram::Init()
{
    CreateId();

    AddVertexShader(ResourceUtil::ReadShaderFile(m_path + "/Vertex.vert"));
    AddFragmentShader(ResourceUtil::ReadShaderFile(m_path + "/Fragment.frag"));

    LinkAndValidateProgram();
    AddFoundUniforms();
}

//-------------------------------------------------
// Create
//-------------------------------------------------

void mdcii::ogl::resource::ShaderProgram::CreateId()
{
    id = glCreateProgram();
    MDCII_ASSERT(id, "[ShaderProgram::CreateId()] Error while creating a new Shader Program.")

    Log::MDCII_LOG_DEBUG("[ShaderProgram::CreateId()] A new Shader Program was created. The Id is {}.", id);
}

//-------------------------------------------------
// Add shader types
//-------------------------------------------------

void mdcii::ogl::resource::ShaderProgram::AddVertexShader(const std::string& t_shaderCode)
{
    m_vertexShaderId = AddShader(t_shaderCode, GL_VERTEX_SHADER);
    Log::MDCII_LOG_DEBUG("[ShaderProgram::AddVertexShader()] A new vertex shader was added. The Id is {}.", m_vertexShaderId);
}

void mdcii::ogl::resource::ShaderProgram::AddFragmentShader(const std::string& t_shaderCode)
{
    m_fragmentShaderId = AddShader(t_shaderCode, GL_FRAGMENT_SHADER);
    Log::MDCII_LOG_DEBUG("[ShaderProgram::AddFragmentShader()] A new fragment shader was added. The Id is {}.", m_fragmentShaderId);
}

//-------------------------------------------------
// Shader
//-------------------------------------------------

uint32_t mdcii::ogl::resource::ShaderProgram::CreateShaderObject(const int32_t t_shaderType)
{
    const uint32_t shaderId{ glCreateShader(t_shaderType) };
    MDCII_ASSERT(shaderId, "[ShaderProgram::CreateShaderObject()] Shader object creation has failed.")

    return shaderId;
}

void mdcii::ogl::resource::ShaderProgram::CompileShader(const uint32_t t_shaderId, const std::string& t_shaderCode)
{
    const auto* const shaderSrc{ t_shaderCode.c_str() };
    glShaderSource(t_shaderId, 1, &shaderSrc, nullptr);
    glCompileShader(t_shaderId);
}

void mdcii::ogl::resource::ShaderProgram::CheckCompileStatus(const uint32_t t_shaderId)
{
    auto isCompiled{ GL_FALSE };
    glGetShaderiv(t_shaderId, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        auto maxLength{ 0 };
        glGetShaderiv(t_shaderId, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<char> errorLog(maxLength);
        glGetShaderInfoLog(t_shaderId, maxLength, &maxLength, errorLog.data());

        glDeleteShader(t_shaderId);

        std::string log;
        for (const auto& value : errorLog)
        {
            log.push_back(value);
        }

        throw MDCII_EXCEPTION("[ShaderProgram::CheckCompileStatus()] Error while compiling shader program. Log: " + log);
    }
}

void mdcii::ogl::resource::ShaderProgram::FindUniforms(const std::string& t_shaderCode)
{
    const std::string uniformKeyword{ "uniform" };
    const auto uniformKeywordLength{ uniformKeyword.length() };

    std::vector<size_t> uniformPositions;
    ResourceUtil::FindAllOccurances(uniformPositions, t_shaderCode, uniformKeyword);

    for (const auto& position : uniformPositions)
    {
        const auto begin{ position + uniformKeywordLength + 1 };
        const auto end{ t_shaderCode.find_first_of(';', begin) };
        const auto uniformLine{ t_shaderCode.substr(begin, end - begin) };

        const auto uniformNamePos{ uniformLine.find_first_of(' ') + 1 };
        auto uniformName{ uniformLine.substr(uniformNamePos, uniformLine.length()) };
        const auto uniformType{ uniformLine.substr(0, uniformNamePos - 1) };

        std::vector<std::string> cache;
        const auto uniformArrayStartPos{ uniformName.find_first_of('[') };
        if (uniformArrayStartPos != std::string::npos)
        {
            const auto uniformArrayEndPos{ uniformName.find_first_of(']') };
            const auto number{ uniformName.substr(uniformArrayStartPos + 1, (uniformArrayEndPos - 1) - uniformArrayStartPos) };
            const auto iNumber{ std::stoi(number) };
            const auto newUniformName{ uniformName.substr(0, uniformArrayStartPos) };

            for (auto i{ 0 }; i < iNumber; ++i)
            {
                fmt::format_int f(i);
                uniformName = newUniformName + "[" + f.c_str() + "]";

                cache.push_back(uniformName);
                m_foundUniforms.emplace_back(uniformType, uniformName);
            }

            m_arrayUniformNames.emplace(newUniformName, cache);
        }
        else
        {
            m_foundUniforms.emplace_back(uniformType, uniformName);
        }
    }
}

uint32_t mdcii::ogl::resource::ShaderProgram::AddShader(const std::string& t_shaderCode, const int32_t t_shaderType)
{
    const uint32_t shaderId{ CreateShaderObject(t_shaderType) };
    CompileShader(shaderId, t_shaderCode);
    CheckCompileStatus(shaderId);
    glAttachShader(id, shaderId);

    FindUniforms(t_shaderCode);

    return shaderId;
}

//-------------------------------------------------
// Link
//-------------------------------------------------

void mdcii::ogl::resource::ShaderProgram::LinkAndValidateProgram() const
{
    // link our program
    glLinkProgram(id);

    // error handling
    auto isLinked{ GL_FALSE };
    glGetProgramiv(id, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE)
    {
        auto maxLength{ 0 };
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<char> infoLog(maxLength);
        glGetProgramInfoLog(id, maxLength, &maxLength, infoLog.data());

        CleanUp();

        std::string log;
        for (const auto& value : infoLog)
        {
            log.push_back(value);
        }

        throw MDCII_EXCEPTION("[ShaderProgram::LinkAndValidate()] Error while linking shader program " + m_path + ". Log: " + log);
    }

    // cleanup: always detach shaders after a successful link
    if (m_vertexShaderId != 0)
    {
        glDetachShader(id, m_vertexShaderId);
    }

    if (m_fragmentShaderId != 0)
    {
        glDetachShader(id, m_fragmentShaderId);
    }

    // validate our program
    glValidateProgram(id);

    auto isValidated{ GL_FALSE };
    glGetProgramiv(id, GL_VALIDATE_STATUS, &isValidated);
    if (isValidated == GL_FALSE)
    {
        auto maxLength{ 0 };
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<char> infoLog(maxLength);
        glGetProgramInfoLog(id, maxLength, &maxLength, infoLog.data());

        CleanUp();

        std::string log;
        for (const auto& value : infoLog)
        {
            log.push_back(value);
        }

        throw MDCII_EXCEPTION("[ShaderProgram::LinkAndValidate()] Shader Program validation error. Log: " + log);
    }
}

//-------------------------------------------------
// Uniforms
//-------------------------------------------------

void mdcii::ogl::resource::ShaderProgram::AddFoundUniforms()
{
    for (const auto& uniform : m_foundUniforms)
    {
        int32_t uniformId{ glGetUniformLocation(id, uniform.name.c_str()) };
        m_uniforms.emplace(uniform.name, uniformId);
    }
}

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::ogl::resource::ShaderProgram::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[ShaderProgram::CleanUp()] Clean up Shader Program Id {}.", id);

    Unbind();

    if (m_vertexShaderId)
    {
        glDeleteShader(m_vertexShaderId);
        Log::MDCII_LOG_DEBUG("[ShaderProgram::CleanUp()] Vertex shader Id {} was deleted.", m_vertexShaderId);
    }

    if (m_fragmentShaderId)
    {
        glDeleteShader(m_fragmentShaderId);
        Log::MDCII_LOG_DEBUG("[ShaderProgram::CleanUp()] Fragment shader Id {} was deleted.", m_fragmentShaderId);
    }

    if (id)
    {
        glDeleteProgram(id);
        Log::MDCII_LOG_DEBUG("[ShaderProgram::CleanUp()] Shader Program Id {} was deleted.", id);
    }
}
