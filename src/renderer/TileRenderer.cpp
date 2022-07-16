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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

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
    const camera::Camera& t_camera,
    const bool t_selected
) const
{
    ogl::OpenGL::EnableAlphaBlending();

    const auto& shaderProgram{ ogl::resource::ResourceManager::LoadShaderProgram("shader/mesh") };
    shaderProgram.Bind();

    shaderProgram.SetUniform("model", t_modelMatrix);
    shaderProgram.SetUniform("view", t_camera.GetViewMatrix());
    shaderProgram.SetUniform("projection", t_window.GetOrthographicProjectionMatrix());
    shaderProgram.SetUniform("diffuseMap", 0);
    shaderProgram.SetUniform("selected", t_selected);

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

    const auto& shaderProgram{ ogl::resource::ResourceManager::LoadShaderProgram("shader/picking") };
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
