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

#include <magic_enum.hpp>
#include "TerrainRenderer.h"
#include "RenderUtils.h"
#include "map/Map.h"
#include "state/State.h"
#include "file/OriginalResourcesManager.h"
#include "ogl/resource/ResourceManager.h"
#include "ogl/resource/TextureUtils.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::renderer::TerrainRenderer::TerrainRenderer(mdcii::map::Map* t_map)
    : m_map{ t_map }
{
    Log::MDCII_LOG_DEBUG("[TerrainRenderer::TerrainRenderer()] Create TerrainRenderer.");

    MDCII_ASSERT(m_map, "[TerrainRenderer::TerrainRenderer()] Null pointer.")

    Init();
}

mdcii::renderer::TerrainRenderer::~TerrainRenderer() noexcept
{
    Log::MDCII_LOG_DEBUG("[TerrainRenderer::~TerrainRenderer()] Destruct TerrainRenderer.");
}

//-------------------------------------------------
// Render
//-------------------------------------------------

void mdcii::renderer::TerrainRenderer::RenderTiles(
    const map::Zoom t_zoom,
    const map::Rotation t_rotation,
    const ogl::Window& t_window,
    const camera::Camera& t_camera
) const
{
    ogl::OpenGL::EnableAlphaBlending();

    const auto& shaderProgram{ ogl::resource::ResourceManager::LoadShaderProgram("shader/terrain") };
    shaderProgram.Bind();

    shaderProgram.SetUniform("view", t_camera.GetViewMatrix());
    shaderProgram.SetUniform("projection", t_window.GetOrthographicProjectionMatrix());
    shaderProgram.SetUniform("diffuseMap", 0);
    shaderProgram.SetUniform("selected", false);
    shaderProgram.SetUniform("rotation", magic_enum::enum_integer(t_rotation));

    const auto& stadtfldBshTextures{ m_map->context->originalResourcesManager->GetStadtfldBshByZoom(t_zoom) };
    const auto textureId{ stadtfldBshTextures[4]->textureId }; // Id 101 -> Gfx 4

    glBindVertexArray(m_vaos.at(magic_enum::enum_integer(t_zoom)));
    ogl::resource::TextureUtils::BindForReading(textureId, GL_TEXTURE0);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, m_instances);
    glBindVertexArray(0);

    ogl::OpenGL::DisableBlending();
}

//-------------------------------------------------
// Instancing
//-------------------------------------------------

void mdcii::renderer::TerrainRenderer::AddModelMatrices(
    const map::Zoom t_zoom,
    const map::MapLayer::Model_Matrices_For_Each_Rotation& t_modelMatrices,
    const int32_t t_instances
)
{
    MDCII_ASSERT(t_modelMatrices.size() == map::NR_OF_ROTATIONS, "[TerrainRenderer::AddModelMatrices()] Invalid number of model matrices.")
    MDCII_ASSERT(t_instances, "[TerrainRenderer::AddModelMatrices()] Invalid number of instances.")

    Log::MDCII_LOG_DEBUG("[TerrainRenderer::AddModelMatrices()] Add {} model matrices for zoom {}.", t_modelMatrices.size(), magic_enum::enum_name(t_zoom));

    // may be mandatory in some cases
    m_instances = t_instances;

    // bind Vao of the given zoom
    MDCII_ASSERT(m_vaos.at(magic_enum::enum_integer(t_zoom)), "[TerrainRenderer::AddModelMatrices()] Invalid Vao handle.")
    glBindVertexArray(m_vaos.at(magic_enum::enum_integer(t_zoom)));

    // generate a Vbo for each rotation and store the model matrices
    auto i{ 1 };
    magic_enum::enum_for_each<map::Rotation>([&](const map::Rotation t_rotation) {
        // get the model matrices of the rotation
        const auto& modelMatrices{ t_modelMatrices.at(magic_enum::enum_integer(t_rotation)) };

        // create Vbo
        uint32_t vboId;
        glGenBuffers(1, &vboId);
        MDCII_ASSERT(vboId, "[TerrainRenderer::AddModelMatrices()] Invalid Vbo handle.")
        Log::MDCII_LOG_DEBUG("[TerrainRenderer::AddModelMatrices()] A new Vbo was created with the Id: {}.", vboId);

        // bind Vbo
        glBindBuffer(GL_ARRAY_BUFFER, vboId);

        // store data
        glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), modelMatrices.data(), GL_STATIC_DRAW);

        // set buffer layout
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, 4, GL_FLOAT, false, 4 * sizeof(glm::vec4), nullptr);

        glEnableVertexAttribArray(i + 1);
        glVertexAttribPointer(i + 1, 4, GL_FLOAT, false, 4 * sizeof(glm::vec4), (void*) (1 * sizeof(glm::vec4)));

        glEnableVertexAttribArray(i + 2);
        glVertexAttribPointer(i + 2, 4, GL_FLOAT, false, 4 * sizeof(glm::vec4), (void*) (2 * sizeof(glm::vec4)));

        glEnableVertexAttribArray(i + 3);
        glVertexAttribPointer(i + 3, 4, GL_FLOAT, false, 4 * sizeof(glm::vec4), (void*) (3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(i, 1);
        glVertexAttribDivisor(i + 1, 1);
        glVertexAttribDivisor(i + 2, 1);
        glVertexAttribDivisor(i + 3, 1);

        i += map::NR_OF_ROTATIONS;

        // unbind Vbo
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    });

    // unbind Vao
    glBindVertexArray(0);
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::renderer::TerrainRenderer::Init()
{
    uint32_t vao{ 0 };
    magic_enum::enum_for_each<map::Zoom>([&](const map::Zoom t_zoom) {
        RenderUtils::CreateRectangleVao(&vao);
        m_vaos.at(magic_enum::enum_integer(t_zoom)) = vao;
    });
}
