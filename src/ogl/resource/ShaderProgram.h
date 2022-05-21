#pragma once

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

//-------------------------------------------------
// Shader
//-------------------------------------------------

namespace mdcii::ogl::resource
{
    class ShaderProgram
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        uint32_t id{ 0 };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        ShaderProgram() = delete;
        explicit ShaderProgram(std::string t_path);

        ShaderProgram(const ShaderProgram& t_other) = delete;
        ShaderProgram(ShaderProgram&& t_other) noexcept = delete;
        ShaderProgram& operator=(const ShaderProgram& t_other) = delete;
        ShaderProgram& operator=(ShaderProgram&& t_other) noexcept = delete;

        ~ShaderProgram() noexcept;

        //-------------------------------------------------
        // Bind / unbind
        //-------------------------------------------------

        void Bind() const;
        static void Unbind() ;

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
                , name{ std::move(t_name) } {}
        };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        std::string m_path;

        uint32_t m_vertexShaderId{ 0 };
        uint32_t m_fragmentShaderId{ 0 };

        std::vector<Uniform> m_foundUniforms;
        std::unordered_map<std::string, int32_t> m_uniforms;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void Init();

        //-------------------------------------------------
        // Create
        //-------------------------------------------------

        void CreateId();

        //-------------------------------------------------
        // Add shader types
        //-------------------------------------------------

        void AddVertexShader(const std::string& t_shaderCode);
        void AddFragmentShader(const std::string& t_shaderCode);

        //-------------------------------------------------
        // Shader
        //-------------------------------------------------

        static uint32_t CreateShaderObject(int32_t t_shaderType);
        static void CompileShader(uint32_t t_shaderId, const std::string& t_shaderCode);
        static void CheckCompileStatus(uint32_t t_shaderId);
        void FindUniforms(const std::string& t_shaderCode);

        uint32_t AddShader(const std::string& t_shaderCode, int32_t t_shaderType);

        //-------------------------------------------------
        // Link
        //-------------------------------------------------

        void LinkAndValidateProgram() const;

        //-------------------------------------------------
        // Uniforms
        //-------------------------------------------------

        void AddFoundUniforms();

        //-------------------------------------------------
        // Clean up
        //-------------------------------------------------

        void CleanUp() const;
    };
}
