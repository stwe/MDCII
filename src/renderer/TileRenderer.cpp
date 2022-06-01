#include "TileRenderer.h"
#include "MdciiAssert.h"
#include "ogl/OpenGL.h"
#include "ogl/resource/ResourceManager.h"
#include "ogl/resource/TextureUtils.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::renderer::TileRenderer::TileRenderer()
{
    Log::MDCII_LOG_DEBUG("[TileRenderer::TileRenderer()] Create TileRenderer.");

    Init();
}

mdcii::renderer::TileRenderer::~TileRenderer() noexcept
{
    Log::MDCII_LOG_DEBUG("[TileRenderer::~TileRenderer()] Destruct TileRenderer.");
}

//-------------------------------------------------
// Render
//-------------------------------------------------

void mdcii::renderer::TileRenderer::RenderTile(
    const glm::mat4& t_modelMatrix,
    const uint32_t t_textureId,
    const ogl::Window& t_window,
    const camera::Camera& t_camera
) const
{
    ogl::OpenGL::EnableAlphaBlending();

    const auto& shaderProgram{ ogl::resource::ResourceManager::LoadShaderProgram("resources/shader/mesh") };
    shaderProgram.Bind();

    shaderProgram.SetUniform("model", t_modelMatrix);
    shaderProgram.SetUniform("view", t_camera.GetViewMatrix());
    shaderProgram.SetUniform("projection", t_window.GetOrthographicProjectionMatrix());
    shaderProgram.SetUniform("diffuseMap", 0);

    glBindVertexArray(m_vao);
    ogl::resource::TextureUtils::BindForReading(t_textureId, GL_TEXTURE0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    ogl::OpenGL::DisableBlending();
}

void mdcii::renderer::TileRenderer::RenderTileForMousePicking(
    const glm::mat4& t_modelMatrix,
    const uint32_t t_textureId,
    const glm::vec3& t_color,
    const ogl::Window& t_window,
    const camera::Camera& t_camera
) const
{
    ogl::OpenGL::EnableAlphaBlending();

    const auto& shaderProgram{ ogl::resource::ResourceManager::LoadShaderProgram("resources/shader/picking") };
    shaderProgram.Bind();

    shaderProgram.SetUniform("model", t_modelMatrix);
    shaderProgram.SetUniform("view", t_camera.GetViewMatrix());
    shaderProgram.SetUniform("projection", t_window.GetOrthographicProjectionMatrix());
    shaderProgram.SetUniform("diffuseMap", 0);
    shaderProgram.SetUniform("color", t_color);

    glBindVertexArray(m_vao);
    ogl::resource::TextureUtils::BindForReading(t_textureId, GL_TEXTURE0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    ogl::OpenGL::DisableBlending();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::renderer::TileRenderer::Init()
{
    constexpr float vertices[]
    {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &m_vao);

    uint32_t vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(m_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
