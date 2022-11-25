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

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

//-------------------------------------------------
// Shader
//-------------------------------------------------

namespace mdcii::ogl::resource
{
    /**
     * Creates and represents a shader program with a vertex and a fragment shader.
     */
    class ShaderProgram
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The shader program handle.
         */
        uint32_t id{ 0 };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        ShaderProgram() = delete;

        /**
         * Constructs a new ShaderProgram object.
         *
         * @param t_path The path to the shader files.
         */
        explicit ShaderProgram(std::string t_path);

        ShaderProgram(const ShaderProgram& t_other) = delete;
        ShaderProgram(ShaderProgram&& t_other) noexcept = delete;
        ShaderProgram& operator=(const ShaderProgram& t_other) = delete;
        ShaderProgram& operator=(ShaderProgram&& t_other) noexcept = delete;

        ~ShaderProgram() noexcept;

        //-------------------------------------------------
        // Bind / unbind
        //-------------------------------------------------

        /**
         * Binds a shader program.
         */
        void Bind() const;

        /**
         * Unbinds a shader program.
         */
        static void Unbind();

        //-------------------------------------------------
        // Set uniforms
        //-------------------------------------------------

        void SetUniform(const std::string& t_uniformName, int32_t t_value) const;
        void SetUniform(const std::string& t_uniformName, float t_value) const;
        void SetUniform(const std::string& t_uniformName, bool t_value) const;
        void SetUniform(const std::string& t_uniformName, const glm::vec2& t_value) const;
        void SetUniform(const std::string& t_uniformName, const glm::vec3& t_value) const;
        void SetUniform(const std::string& t_uniformName, const glm::vec4& t_value) const;
        void SetUniform(const std::string& t_uniformName, const glm::mat4& t_value) const;
        void SetUniform(const std::string& t_uniformName, const glm::mat3& t_value) const;
        void SetUniform(const std::string& t_uniformName, const std::vector<int32_t>& t_container) const;

    protected:

    private:
        //-------------------------------------------------
        // Type
        //-------------------------------------------------

        struct Uniform
        {
            std::string type;
            std::string name;

            Uniform(std::string t_type, std::string t_name)
                : type{ std::move(t_type) }
                , name{ std::move(t_name) }
            {}
        };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The path to the shader files.
         */
        std::string m_path;

        /**
         * The vertex shader handle.
         */
        uint32_t m_vertexShaderId{ 0 };

        /**
         * The fragment shader handle.
         */
        uint32_t m_fragmentShaderId{ 0 };

        /**
         * A list of all found uniforms.
         */
        std::vector<Uniform> m_foundUniforms;

        /**
         * A uniform handle for each uniform.
         */
        std::unordered_map<std::string, int32_t> m_uniforms;

        /**
         * Stores array uniforms.
         */
        std::unordered_map<std::string, std::vector<std::string>> m_arrayUniformNames;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Generates the shader program.
         */
        void Init();

        //-------------------------------------------------
        // Create
        //-------------------------------------------------

        /**
         * Creates a new shader program handle.
         */
        void CreateId();

        //-------------------------------------------------
        // Add shader types
        //-------------------------------------------------

        /**
         * Creates a vertex shader from given shader code.
         *
         * @param t_shaderCode The shader code.
         */
        void AddVertexShader(const std::string& t_shaderCode);

        /**
         * Creates a fragment shader from given shader code.
         *
         * @param t_shaderCode The shader code.
         */
        void AddFragmentShader(const std::string& t_shaderCode);

        //-------------------------------------------------
        // Shader
        //-------------------------------------------------

        /**
         * Creates a shader handle for the given type.
         *
         * @param t_shaderType The type of the shader (vertex / fragment).
         *
         * @return The shader handle.
         */
        static uint32_t CreateShaderObject(int32_t t_shaderType);

        /**
         * Compiles a shader.
         *
         * @param t_shaderId The shader handle.
         * @param t_shaderCode The shader code.
         */
        static void CompileShader(uint32_t t_shaderId, const std::string& t_shaderCode);

        /**
         * Checks the compile status of a shader.
         *
         * @param t_shaderId The shader handle.
         */
        static void CheckCompileStatus(uint32_t t_shaderId);

        /**
         * Stores all uniforms in m_foundUniforms.
         *
         * @param t_shaderCode The shader code.
         */
        void FindUniforms(const std::string& t_shaderCode);

        /**
         * Creates a shader from given code and type.
         * Attaches this shader to the program.
         *
         * @param t_shaderCode The shader code.
         * @param t_shaderType The type of the shader (vertex / fragment).
         *
         * @return The shader handle.
         */
        uint32_t AddShader(const std::string& t_shaderCode, int32_t t_shaderType);

        //-------------------------------------------------
        // Link
        //-------------------------------------------------

        /**
         * Links the program and check for errors before.
         */
        void LinkAndValidateProgram() const;

        //-------------------------------------------------
        // Uniforms
        //-------------------------------------------------

        /**
         * Creates a uniform handle for each found uniform.
         */
        void AddFoundUniforms();

        //-------------------------------------------------
        // Clean up
        //-------------------------------------------------

        /**
         * Clean up / delete shader.
         */
        void CleanUp() const;
    };
}
