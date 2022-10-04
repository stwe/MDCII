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

#include "WorldRenderer.h"
#include "RenderUtils.h"
#include "world/World.h"
#include "map/TileAtlas.h"
#include "ogl/OpenGL.h"
#include "ogl/resource/ResourceManager.h"
#include "ogl/resource/TextureUtils.h"
#include "ogl/buffer/Ssbo.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::renderer::WorldRenderer::WorldRenderer(world::World* t_world)
    : m_world{ t_world }
{
    Log::MDCII_LOG_DEBUG("[WorldRenderer::WorldRenderer()] Create WorldRenderer.");

    MDCII_ASSERT(m_world, "[WorldRenderer::WorldRenderer()] Null pointer.")

    Init();
}

mdcii::renderer::WorldRenderer::~WorldRenderer() noexcept
{
    Log::MDCII_LOG_DEBUG("[WorldRenderer::~WorldRenderer()] Destruct WorldRenderer.");
}

//-------------------------------------------------
// Render
//-------------------------------------------------

void mdcii::renderer::WorldRenderer::Render(
    const world::WorldLayerType t_layerType,
    const map::Zoom t_zoom,
    const map::Rotation t_rotation,
    const ogl::Window& t_window,
    const camera::Camera& t_camera
) const
{
    const auto layerType{ magic_enum::enum_integer(t_layerType) };
    const auto zoom{ magic_enum::enum_integer(t_zoom) };
    const auto rotation{ magic_enum::enum_integer(t_rotation) };

    const auto& shaderProgram{ ogl::resource::ResourceManager::LoadShaderProgram("shader/world") };
    shaderProgram.Bind();

    shaderProgram.SetUniform("view", t_camera.GetViewMatrix());
    shaderProgram.SetUniform("projection", t_window.GetOrthographicProjectionMatrix());
    shaderProgram.SetUniform("diffuseMap", 0);
    shaderProgram.SetUniform("selected", false);
    shaderProgram.SetUniform("rotation", rotation);

    const auto maxY{ map::TileAtlas::HEIGHTS.at(zoom) };
    const auto nrOfRows{ static_cast<float>(map::TileAtlas::ROWS.at(zoom)) };

    shaderProgram.SetUniform("maxY", maxY);
    shaderProgram.SetUniform("nrOfRows", nrOfRows);

    m_vaos.at(layerType).at(zoom)->Bind();

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        MODEL_MATRICES_BINDING,
        m_vaos.at(layerType).at(zoom)->ssbos.at(rotation)->id
    );

    ogl::resource::TextureUtils::BindForReading(m_world->tileAtlas->textureIds.at(zoom), GL_TEXTURE0, GL_TEXTURE_2D_ARRAY);
    m_vaos.at(layerType).at(zoom)->DrawInstanced(m_instances);

    ogl::buffer::Vao::Unbind();
}

void mdcii::renderer::WorldRenderer::Render(
    const mdcii::map::Zoom t_zoom,
    const mdcii::map::Rotation t_rotation,
    const mdcii::ogl::Window& t_window,
    const mdcii::camera::Camera& t_camera
) const
{
    ogl::OpenGL::EnableAlphaBlending();

    const auto& shaderProgram{ ogl::resource::ResourceManager::LoadShaderProgram("shader/grid") };
    shaderProgram.Bind();

    shaderProgram.SetUniform("view", t_camera.GetViewMatrix());
    shaderProgram.SetUniform("projection", t_window.GetOrthographicProjectionMatrix());
    shaderProgram.SetUniform("diffuseMap", 0);
    shaderProgram.SetUniform("selected", false);
    shaderProgram.SetUniform("rotation", magic_enum::enum_integer(t_rotation));

    m_vaos.at(magic_enum::enum_integer(world::WorldLayerType::GRID)).at(magic_enum::enum_integer(t_zoom))->Bind();

    const auto& textureId{ ogl::resource::ResourceManager::LoadTexture(m_gridFileNames.at(magic_enum::enum_integer(t_zoom))).id };
    ogl::resource::TextureUtils::BindForReading(textureId, GL_TEXTURE0);

    m_vaos.at(magic_enum::enum_integer(world::WorldLayerType::GRID)).at(magic_enum::enum_integer(t_zoom))->DrawInstanced(m_instances);

    ogl::buffer::Vao::Unbind();

    ogl::OpenGL::DisableBlending();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::renderer::WorldRenderer::Init()
{
    Log::MDCII_LOG_DEBUG("[WorldRenderer::Init()] Starts initializing WorldRenderer...");

    // create and store the filenames to show an isometric grid for each zoom
    magic_enum::enum_for_each<map::Zoom>([&](const map::Zoom t_zoom) {
        const auto zoomStr{ to_lower_case(std::string(magic_enum::enum_name<map::Zoom>(t_zoom))) };
        const auto fileName{ "textures/" + zoomStr + "/red_" + zoomStr + ".png" };
        m_gridFileNames.at(magic_enum::enum_integer(t_zoom)) = fileName;
    });

    // create Vaos
    magic_enum::enum_for_each<map::Zoom>([&](const map::Zoom t_zoom) {
        m_vaos.at(magic_enum::enum_integer(world::WorldLayerType::TERRAIN)).at(magic_enum::enum_integer(t_zoom)) = RenderUtils::CreateRectangleVao();
        m_vaos.at(magic_enum::enum_integer(world::WorldLayerType::BUILDINGS)).at(magic_enum::enum_integer(t_zoom)) = RenderUtils::CreateRectangleVao();
        m_vaos.at(magic_enum::enum_integer(world::WorldLayerType::TERRAIN_AND_BUILDINGS)).at(magic_enum::enum_integer(t_zoom)) = RenderUtils::CreateRectangleVao();
        //m_vaos.at(magic_enum::enum_integer(world::WorldLayerType::GRID)).at(magic_enum::enum_integer(t_zoom)) = RenderUtils::CreateRectangleVao();

        AddModelMatrices(t_zoom, world::WorldLayerType::TERRAIN);
        AddModelMatrices(t_zoom, world::WorldLayerType::BUILDINGS);
        AddModelMatrices(t_zoom, world::WorldLayerType::TERRAIN_AND_BUILDINGS);
        //AddModelMatrices(t_zoom, world::WorldLayerType::GRID);

        AddTextureInfo(t_zoom, world::WorldLayerType::TERRAIN);
        AddTextureInfo(t_zoom, world::WorldLayerType::BUILDINGS);
        AddTextureInfo(t_zoom, world::WorldLayerType::TERRAIN_AND_BUILDINGS);
    });

    Log::MDCII_LOG_DEBUG("[WorldRenderer::Init()] The WorldRenderer was initialized successfully.");
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::renderer::WorldRenderer::AddModelMatrices(const map::Zoom t_zoom, const world::WorldLayerType t_layerType)
{
    Log::MDCII_LOG_DEBUG(
        "[WorldRenderer::AddModelMatrices()] Add model matrices to the Gpu for layer {} and zoom {}.",
        magic_enum::enum_name(t_layerType),
        magic_enum::enum_name(t_zoom)
    );

    // get the layer
    const auto& layer{ m_world->GetLayer(t_layerType) };

    // set number of instances to render
    m_instances = layer.GetInstances();

    // bind Vao of the given zoom
    m_vaos.at(magic_enum::enum_integer(t_layerType)).at(magic_enum::enum_integer(t_zoom))->Bind();

    // generate a Ssbo for each rotation to store the model matrices
    magic_enum::enum_for_each<map::Rotation>([&](const map::Rotation t_rotation) {
        // get the model matrices of the rotation
        const auto& modelMatrices{ layer.GetModelMatrices(t_zoom).at(magic_enum::enum_integer(t_rotation)) };

        // create and bind Ssbo object
        auto ssbo{ std::make_unique<ogl::buffer::Ssbo>() };
        ssbo->Bind();

        // store the model matrices in the Ssbo
        ssbo->StoreStaticData(modelMatrices.size() * sizeof(glm::mat4), modelMatrices.data());

        // unbind Ssbo
        ssbo->Unbind();

        // store Ssbo object
        m_vaos.at(magic_enum::enum_integer(t_layerType)).at(magic_enum::enum_integer(t_zoom))->ssbos.emplace_back(std::move(ssbo));
    });

    // unbind Vao
    ogl::buffer::Vao::Unbind();
}

void mdcii::renderer::WorldRenderer::AddTextureInfo(const map::Zoom t_zoom, const world::WorldLayerType t_layerType) const
{
    Log::MDCII_LOG_DEBUG(
        "[WorldRenderer::AddTextureInfo()] Add texture info to the Gpu for layer {} and zoom {}.",
        magic_enum::enum_name(t_layerType),
        magic_enum::enum_name(t_zoom)
    );

    // get the layer
    const auto& layer{ m_world->GetLayer(t_layerType) };

    // zoom as int
    const auto zoom{ magic_enum::enum_integer(t_zoom) };

    // bind Vao
    m_vaos.at(magic_enum::enum_integer(t_layerType)).at(zoom)->Bind();

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
    m_vaos.at(magic_enum::enum_integer(t_layerType)).at(zoom)->vbos.emplace_back(std::move(vboText));

    // offsets
    magic_enum::enum_for_each<map::Rotation>([&](const map::Rotation t_rotation) {
        const auto rotation{ magic_enum::enum_integer(t_rotation) };

        // create && bind Vbo
        auto vboOff{ std::make_unique<ogl::buffer::Vbo>() };
        vboOff->Bind();

        // store data
        ogl::buffer::Vbo::StoreStaticData(static_cast<uint32_t>(layer.offsets.at(zoom).at(rotation).size()) * sizeof(glm::vec2), layer.offsets.at(zoom).at(rotation).data());

        // set buffer layout
        ogl::buffer::Vbo::AddFloatAttribute(OFFSETS0_LOCATION + rotation, 2, 2, 0, true);

        // unbind Vbo
        ogl::buffer::Vbo::Unbind();

        // store Vbo
        m_vaos.at(magic_enum::enum_integer(t_layerType)).at(zoom)->vbos.emplace_back(std::move(vboOff));
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
    m_vaos.at(magic_enum::enum_integer(t_layerType)).at(zoom)->vbos.emplace_back(std::move(vboHeight));

    // unbind Vao
    ogl::buffer::Vao::Unbind();
}
