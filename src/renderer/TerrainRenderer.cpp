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
#include "ogl/OpenGL.h"
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

    m_vaos.at(magic_enum::enum_integer(t_zoom))->Bind();
    ogl::resource::TextureUtils::BindForReading(m_map->tileAtlas->textureIds.at(magic_enum::enum_integer(t_zoom)), GL_TEXTURE0, GL_TEXTURE_2D_ARRAY);
    m_vaos.at(magic_enum::enum_integer(t_zoom))->DrawInstanced(m_instances);
    ogl::buffer::Vao::Unbind();

    ogl::OpenGL::DisableBlending();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::renderer::TerrainRenderer::Init()
{
    magic_enum::enum_for_each<map::Zoom>([&](const map::Zoom t_zoom) {
        m_vaos.at(magic_enum::enum_integer(t_zoom)) = RenderUtils::CreateRectangleVao();
    });

    magic_enum::enum_for_each<map::Zoom>([&](const map::Zoom t_zoom) {
        //AddModelMatrices(t_zoom, map::LayerType::TERRAIN);
        AddModelMatrices(t_zoom, map::LayerType::BUILDINGS);
    });

    magic_enum::enum_for_each<map::Zoom>([&](const map::Zoom t_zoom) {
        //AddTextureInfo(t_zoom, map::LayerType::TERRAIN);
        AddTextureInfo(t_zoom, map::LayerType::BUILDINGS);
    });
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::renderer::TerrainRenderer::AddModelMatrices(const map::Zoom t_zoom, const map::LayerType t_layerType)
{
    Log::MDCII_LOG_DEBUG(
        "[TerrainRenderer::AddModelMatrices()] Add model matrices to the Gpu for layer {} and zoom {}.",
        magic_enum::enum_name(t_layerType),
        magic_enum::enum_name(t_zoom)
    );

    // get the layer
    const auto& layer{ m_map->mapContent->GetLayer(t_layerType) };

    // set number of instances to render
    m_instances = layer.GetInstances();

    // bind Vao of the given zoom
    m_vaos.at(magic_enum::enum_integer(t_zoom))->Bind();

    // generate a Vbo for each rotation and store the model matrices
    auto i{ MODEL_MATRICES0_LOCATION };
    magic_enum::enum_for_each<map::Rotation>([&](const map::Rotation t_rotation) {
        // get the model matrices of the rotation
        const auto& modelMatrices{ layer.GetModelMatrices(t_zoom).at(magic_enum::enum_integer(t_rotation)) };

        // create && bind Vbo
        auto vbo{ std::make_unique<ogl::buffer::Vbo>() };
        vbo->Bind();

        // store data
        ogl::buffer::Vbo::StoreStaticData(static_cast<uint32_t>(modelMatrices.size()) * sizeof(glm::mat4), modelMatrices.data());

        // set buffer layout
        ogl::buffer::Vbo::AddFloatAttribute(i, 4, 16, 0, true);
        ogl::buffer::Vbo::AddFloatAttribute(i + 1, 4, 16, 4, true);
        ogl::buffer::Vbo::AddFloatAttribute(i + 2, 4, 16, 8, true);
        ogl::buffer::Vbo::AddFloatAttribute(i + 3, 4, 16, 12, true);

        // next rotation
        i += map::NR_OF_ROTATIONS;

        // unbind Vbo
        ogl::buffer::Vbo::Unbind();

        // store Vbo
        m_vaos.at(magic_enum::enum_integer(t_zoom))->vbos.emplace_back(std::move(vbo));
    });

    // unbind Vao
    ogl::buffer::Vao::Unbind();
}

void mdcii::renderer::TerrainRenderer::AddTextureInfo(const map::Zoom t_zoom, const map::LayerType t_layerType) const
{
    Log::MDCII_LOG_DEBUG(
        "[TerrainRenderer::AddTextureInfo()] Add texture info to the Gpu for layer {} and zoom {}.",
        magic_enum::enum_name(t_layerType),
        magic_enum::enum_name(t_zoom)
    );

    // get the layer
    const auto& layer{ m_map->mapContent->GetLayer(t_layerType) };

    // zoom as int
    const auto zoom{ magic_enum::enum_integer(t_zoom) };

    // bind Vao
    m_vaos.at(zoom)->Bind();

    // texture atlas indices

    // create && bind Vbo
    auto vboText{ std::make_unique<ogl::buffer::Vbo>() };
    vboText->Bind();

    // store data
    ogl::buffer::Vbo::StoreStaticData(static_cast<uint32_t>(layer.textureAtlasIndices.at(zoom).size()) * sizeof(glm::ivec4), layer.textureAtlasIndices.at(zoom).data());

    // set buffer layout
    ogl::buffer::Vbo::AddIntAttribute(TEXTURE_ATLAS_LOCATION, 4, 4, 0, true);

    // unbind Vbo
    ogl::buffer::Vbo::Unbind();

    // store Vbo
    m_vaos.at(zoom)->vbos.emplace_back(std::move(vboText));

    // offsets
    magic_enum::enum_for_each<map::Rotation>([&](const map::Rotation t_rotation) {
        const auto rotation{ magic_enum::enum_integer(t_rotation) };

        // create && bind Vbo
        auto vboOff{ std::make_unique<ogl::buffer::Vbo>() };
        vboOff->Bind();

        // store data
        ogl::buffer::Vbo::StoreStaticData(static_cast<uint32_t>(layer.offsets.at(zoom).at(rotation).size()) * sizeof(float), layer.offsets.at(zoom).at(rotation).data());

        // set buffer layout
        ogl::buffer::Vbo::AddFloatAttribute(OFFSETS0_LOCATION + rotation, 2, 2, 0, true);

        // unbind Vbo
        ogl::buffer::Vbo::Unbind();

        // store Vbo
        m_vaos.at(zoom)->vbos.emplace_back(std::move(vboOff));
    });

    // heights

    // create && bind Vbo
    auto vboHeight{ std::make_unique<ogl::buffer::Vbo>() };
    vboHeight->Bind();

    // store data
    ogl::buffer::Vbo::StoreStaticData(static_cast<uint32_t>(layer.heights.at(zoom).size()) * sizeof(glm::vec4), layer.heights.at(zoom).data());

    // set buffer layout
    ogl::buffer::Vbo::AddFloatAttribute(HEIGHTS_LOCATION, 4, 4, 0, true);

    // unbind Vbo
    ogl::buffer::Vbo::Unbind();

    // store Vbo
    m_vaos.at(zoom)->vbos.emplace_back(std::move(vboHeight));

    // unbind Vao
    ogl::buffer::Vao::Unbind();
}
