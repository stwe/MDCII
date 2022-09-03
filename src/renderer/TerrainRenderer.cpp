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

#include "TerrainRenderer.h"
#include "RenderUtils.h"
#include "map/Map.h"
#include "map/MapContent.h"
#include "map/TileAtlas.h"
#include "ogl/resource/ResourceManager.h"
#include "ogl/resource/TextureUtils.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::renderer::TerrainRenderer::TerrainRenderer(map::Map* t_map)
    : m_map{ t_map }
{
    Log::MDCII_LOG_DEBUG("[TerrainRenderer::TerrainRenderer()] Create TerrainRenderer.");

    MDCII_ASSERT(m_map, "[TerrainRenderer::TerrainRenderer()] Null pointer.")

    Init();
}

mdcii::renderer::TerrainRenderer::~TerrainRenderer() noexcept
{
    Log::MDCII_LOG_DEBUG("[TerrainRenderer::~TerrainRenderer()] Destruct TerrainRenderer.");

    CleanUp();
}

//-------------------------------------------------
// Render
//-------------------------------------------------

void mdcii::renderer::TerrainRenderer::Render(
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
    shaderProgram.SetUniform("sampler", 0);
    shaderProgram.SetUniform("selected", false);
    shaderProgram.SetUniform("rotation", magic_enum::enum_integer(t_rotation));

    const auto maxY{ map::TileAtlas::HEIGHTS.at(magic_enum::enum_integer(t_zoom)) };
    const auto nrOfRows{ static_cast<float>(map::TileAtlas::ROWS.at(magic_enum::enum_integer(t_zoom))) };

    shaderProgram.SetUniform("maxY", maxY);
    shaderProgram.SetUniform("nrOfRows", nrOfRows);

    glBindVertexArray(m_vaos.at(magic_enum::enum_integer(t_zoom)));
    ogl::resource::TextureUtils::BindForReading(m_map->tileAtlas->textureIds.at(magic_enum::enum_integer(t_zoom)), GL_TEXTURE0, GL_TEXTURE_2D_ARRAY);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, m_instances);
    glBindVertexArray(0);

    ogl::OpenGL::DisableBlending();
}

//-------------------------------------------------
// Data to Gpu
//-------------------------------------------------

void mdcii::renderer::TerrainRenderer::AddModelMatrices()
{
    magic_enum::enum_for_each<map::Zoom>([&](const map::Zoom t_zoom) {
        AddModelMatrices(t_zoom);
    });
}

void mdcii::renderer::TerrainRenderer::AddTextureInfo() const
{
    magic_enum::enum_for_each<map::Zoom>([&](const map::Zoom t_zoom) {
        AddTextureInfo(t_zoom);
    });
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

    magic_enum::enum_for_each<map::Zoom>([&](const map::Zoom t_zoom) {
        AddModelMatrices(t_zoom);
    });

    magic_enum::enum_for_each<map::Zoom>([&](const map::Zoom t_zoom) {
        AddTextureInfo(t_zoom);
    });
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::renderer::TerrainRenderer::AddModelMatrices(const map::Zoom t_zoom)
{
    Log::MDCII_LOG_DEBUG("[TerrainRenderer::AddModelMatrices()] Add model matrices to the Gpu for zoom {}.", magic_enum::enum_name(t_zoom));

    // get the terrain layer
    const auto& layer{ m_map->mapContent->GetLayer(map::LayerType::TERRAIN) };

    // set number of instances to render
    m_instances = layer.GetInstances();

    // bind Vao of the given zoom
    MDCII_ASSERT(m_vaos.at(magic_enum::enum_integer(t_zoom)), "[TerrainRenderer::AddModelMatrices()] Invalid Vao handle.")
    glBindVertexArray(m_vaos.at(magic_enum::enum_integer(t_zoom)));

    // generate a Vbo for each rotation and store the model matrices
    auto i{ 1 };
    magic_enum::enum_for_each<map::Rotation>([&](const map::Rotation t_rotation) {
        // get the model matrices of the rotation
        const auto& modelMatrices{ layer.GetModelMatrices(t_zoom).at(magic_enum::enum_integer(t_rotation)) };

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

void mdcii::renderer::TerrainRenderer::AddTextureInfo(const map::Zoom t_zoom) const
{
    Log::MDCII_LOG_DEBUG("[TerrainRenderer::AddTextureInfo()] Add texture info to the Gpu for zoom {}.", magic_enum::enum_name(t_zoom));

    constexpr auto textLocation{ 17 };
    constexpr auto offLocation0{ 18 };
    constexpr auto heightLocation{ 22 };

    const auto& layer{ m_map->mapContent->GetLayer(map::LayerType::TERRAIN) };

    const auto zoom{ magic_enum::enum_integer(t_zoom) };

    // texture atlas indices
    glBindVertexArray(m_vaos.at(zoom));

    uint32_t vboTextId;
    glGenBuffers(1, &vboTextId);
    glBindBuffer(GL_ARRAY_BUFFER, vboTextId);

    glBufferData(
        GL_ARRAY_BUFFER,
        layer.textureAtlasIndices.at(zoom).size() * sizeof(glm::ivec4),
        layer.textureAtlasIndices.at(zoom).data(),
        GL_STATIC_DRAW
    );
    glEnableVertexAttribArray(textLocation);
    glVertexAttribIPointer(textLocation, 4, GL_INT, sizeof(glm::ivec4), nullptr);
    glVertexAttribDivisor(textLocation, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // offsets
    magic_enum::enum_for_each<map::Rotation>([&](const map::Rotation t_rotation) {
        const auto rotation{ magic_enum::enum_integer(t_rotation) };

        uint32_t vboOffId;
        glGenBuffers(1, &vboOffId);
        glBindBuffer(GL_ARRAY_BUFFER, vboOffId);

        glBufferData(
            GL_ARRAY_BUFFER,
            layer.offsets.at(zoom).at(rotation).size() * sizeof(float),
            layer.offsets.at(zoom).at(rotation).data(), GL_STATIC_DRAW
        );
        glEnableVertexAttribArray(offLocation0 + rotation);
        glVertexAttribPointer(offLocation0 + rotation, 2, GL_FLOAT, false, 2 * sizeof(float), nullptr);
        glVertexAttribDivisor(offLocation0 + rotation, 1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

    });

    // heights
    uint32_t vboHeightId;
    glGenBuffers(1, &vboHeightId);
    glBindBuffer(GL_ARRAY_BUFFER, vboHeightId);

    glBufferData(
        GL_ARRAY_BUFFER,
        layer.heights.at(zoom).size() * sizeof(glm::vec4),
        layer.heights.at(zoom).data(), GL_STATIC_DRAW
    );
    glEnableVertexAttribArray(heightLocation);
    glVertexAttribPointer(heightLocation, 4, GL_FLOAT, false, sizeof(glm::vec4), nullptr);
    glVertexAttribDivisor(heightLocation, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::renderer::TerrainRenderer::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[TerrainRenderer::CleanUp()] Clean up.");

    for (const auto id : m_vaos)
    {
        glDeleteVertexArrays(1, &id);
    }
}
