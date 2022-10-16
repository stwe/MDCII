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
#include "world/TileAtlas.h"
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
    const world::Zoom t_zoom,
    const world::Rotation t_rotation,
    const ogl::Window& t_window,
    const camera::Camera& t_camera
) const
{
    const auto layerTypeInt{ magic_enum::enum_integer(t_layerType) };
    const auto zoomInt{ magic_enum::enum_integer(t_zoom) };
    const auto rotationInt{ magic_enum::enum_integer(t_rotation) };

    const auto& shaderProgram{ ogl::resource::ResourceManager::LoadShaderProgram("shader/world") };
    shaderProgram.Bind();

    shaderProgram.SetUniform("view", t_camera.GetViewMatrix());
    shaderProgram.SetUniform("projection", t_window.GetOrthographicProjectionMatrix());
    shaderProgram.SetUniform("diffuseMap", 0);
    shaderProgram.SetUniform("selected", false);
    shaderProgram.SetUniform("rotation", rotationInt);

    const auto maxY{ world::TileAtlas::HEIGHTS.at(zoomInt) };
    const auto nrOfRows{ static_cast<float>(world::TileAtlas::ROWS.at(zoomInt)) };

    shaderProgram.SetUniform("maxY", maxY);
    shaderProgram.SetUniform("nrOfRows", nrOfRows);

    m_vaos.at(layerTypeInt).at(zoomInt)->Bind();

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        MODEL_MATRICES_BINDING,
        m_vaos.at(layerTypeInt).at(zoomInt)->ssbos.at(MODEL_MATRICES_BINDING).at(rotationInt)->id
    );

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        OFFSETS_BINDING,
        m_vaos.at(layerTypeInt).at(zoomInt)->ssbos.at(OFFSETS_BINDING).at(rotationInt)->id
    );

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        TEXTURE_ATLAS_BINDING,
        m_vaos.at(layerTypeInt).at(zoomInt)->ssbos.at(TEXTURE_ATLAS_BINDING).at(0)->id
    );

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        HEIGHTS_BINDING,
        m_vaos.at(layerTypeInt).at(zoomInt)->ssbos.at(HEIGHTS_BINDING).at(0)->id
    );

    ogl::resource::TextureUtils::BindForReading(m_world->tileAtlas->textureIds.at(zoomInt), GL_TEXTURE0, GL_TEXTURE_2D_ARRAY);
    m_vaos.at(layerTypeInt).at(zoomInt)->DrawInstanced(m_world->GetLayer(t_layerType).instancesToRender);

    ogl::buffer::Vao::Unbind();
}

void mdcii::renderer::WorldRenderer::Render(
    const world::Zoom t_zoom,
    const world::Rotation t_rotation,
    const ogl::Window& t_window,
    const camera::Camera& t_camera
) const
{
    const auto zoomInt{ magic_enum::enum_integer(t_zoom) };
    const auto rotationInt{ magic_enum::enum_integer(t_rotation) };
    const auto gridLayerTypeInt{ magic_enum::enum_integer(world::WorldLayerType::GRID) };

    ogl::OpenGL::EnableAlphaBlending();

    const auto& shaderProgram{ ogl::resource::ResourceManager::LoadShaderProgram("shader/grid") };
    shaderProgram.Bind();

    shaderProgram.SetUniform("view", t_camera.GetViewMatrix());
    shaderProgram.SetUniform("projection", t_window.GetOrthographicProjectionMatrix());
    shaderProgram.SetUniform("diffuseMap", 0);
    shaderProgram.SetUniform("selected", false);

    m_vaos.at(gridLayerTypeInt).at(zoomInt)->Bind();

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        MODEL_MATRICES_BINDING,
        m_vaos.at(gridLayerTypeInt).at(zoomInt)->ssbos.at(MODEL_MATRICES_BINDING).at(rotationInt)->id
    );

    const auto& textureId{ ogl::resource::ResourceManager::LoadTexture(m_gridFileNames.at(zoomInt)).id };
    ogl::resource::TextureUtils::BindForReading(textureId, GL_TEXTURE0);

    m_vaos.at(gridLayerTypeInt).at(zoomInt)->DrawInstanced(m_world->GetLayer(world::WorldLayerType::GRID).instancesToRender);

    ogl::buffer::Vao::Unbind();

    ogl::OpenGL::DisableBlending();
}

void mdcii::renderer::WorldRenderer::UpdateGpuData(
    const int32_t t_instance,
    const world::WorldLayerType t_layerType,
    const world::Zoom t_zoom,
    const world::Rotation t_rotation,
    const glm::mat4& t_modelMatrix,
    const int t_atlasNr,
    const glm::vec2& t_offset,
    const float t_height
)
{
    // enum to int
    const auto zoomInt{ magic_enum::enum_integer(t_zoom) };
    const auto rotationInt{ magic_enum::enum_integer(t_rotation) };
    const auto layerTypeInt{ magic_enum::enum_integer(t_layerType) };

    // get the layer
    const auto& layer{ m_world->GetLayer(t_layerType) };

    // bind Vao of the given zoom
    const auto& vao{ m_vaos.at(layerTypeInt).at(zoomInt) };
    vao->Bind();

    // new model matrix
    const auto& modelMatricesSsbo{ vao->ssbos.at(MODEL_MATRICES_BINDING) };
    modelMatricesSsbo.at(rotationInt)->Bind();
    modelMatricesSsbo.at(rotationInt)->StoreSubData(static_cast<int32_t>(sizeof(glm::mat4)) * t_instance, sizeof(glm::mat4), &t_modelMatrix);
    ogl::buffer::Ssbo::Unbind();

    // new offset
    const auto& offsetsSsbo{ vao->ssbos.at(OFFSETS_BINDING) };
    offsetsSsbo.at(rotationInt)->Bind();
    offsetsSsbo.at(rotationInt)->StoreSubData(static_cast<int32_t>(sizeof(glm::vec2)) * t_instance, sizeof(glm::vec2), &t_offset);
    ogl::buffer::Ssbo::Unbind();

    const auto oi{ rotationInt * static_cast<int32_t>(sizeof(int32_t)) };
    const auto of{ rotationInt * static_cast<int32_t>(sizeof(float)) };

    // new texture atlas number
    const auto& atlasSsbo{ vao->ssbos.at(TEXTURE_ATLAS_BINDING) };
    atlasSsbo.at(0)->Bind();
    atlasSsbo.at(0)->StoreSubData((static_cast<int32_t>(sizeof(glm::ivec4)) * t_instance) + oi, sizeof(int32_t), &t_atlasNr);
    ogl::buffer::Ssbo::Unbind();

    // new height
    const auto& heightsSsbo{ vao->ssbos.at(HEIGHTS_BINDING) };
    heightsSsbo.at(0)->Bind();
    heightsSsbo.at(0)->StoreSubData((static_cast<int32_t>(sizeof(glm::vec4)) * t_instance) + of, sizeof(float), &t_height);
    ogl::buffer::Ssbo::Unbind();

    // unbind Vao
    ogl::buffer::Vao::Unbind();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::renderer::WorldRenderer::Init()
{
    Log::MDCII_LOG_DEBUG("[WorldRenderer::Init()] Starts initializing WorldRenderer...");

    // create and store the filenames to show an isometric grid for each zoom
    magic_enum::enum_for_each<world::Zoom>([&](const world::Zoom t_zoom) {
        const auto zoomStr{ to_lower_case(std::string(magic_enum::enum_name<world::Zoom>(t_zoom))) };
        const auto fileName{ "textures/" + zoomStr + "/red_" + zoomStr + ".png" };
        m_gridFileNames.at(magic_enum::enum_integer(t_zoom)) = fileName;
    });

    // create Vaos
    magic_enum::enum_for_each<world::Zoom>([&](const world::Zoom t_zoom) {
        const auto zoomInt{ magic_enum::enum_integer(t_zoom) };

        m_vaos.at(magic_enum::enum_integer(world::WorldLayerType::TERRAIN)).at(zoomInt) = RenderUtils::CreateRectangleVao();
        m_vaos.at(magic_enum::enum_integer(world::WorldLayerType::BUILDINGS)).at(zoomInt) = RenderUtils::CreateRectangleVao();
        m_vaos.at(magic_enum::enum_integer(world::WorldLayerType::TERRAIN_AND_BUILDINGS)).at(zoomInt) = RenderUtils::CreateRectangleVao();
        m_vaos.at(magic_enum::enum_integer(world::WorldLayerType::GRID)).at(zoomInt) = RenderUtils::CreateRectangleVao();

        AddModelMatrices(t_zoom, world::WorldLayerType::TERRAIN);
        AddModelMatrices(t_zoom, world::WorldLayerType::BUILDINGS);
        AddModelMatrices(t_zoom, world::WorldLayerType::TERRAIN_AND_BUILDINGS);
        AddModelMatrices(t_zoom, world::WorldLayerType::GRID);

        AddTextureInfo(t_zoom, world::WorldLayerType::TERRAIN);
        AddTextureInfo(t_zoom, world::WorldLayerType::BUILDINGS);
        AddTextureInfo(t_zoom, world::WorldLayerType::TERRAIN_AND_BUILDINGS);
    });

    Log::MDCII_LOG_DEBUG("[WorldRenderer::Init()] The WorldRenderer was initialized successfully.");
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::renderer::WorldRenderer::AddModelMatrices(const world::Zoom t_zoom, const world::WorldLayerType t_layerType)
{
    Log::MDCII_LOG_DEBUG(
        "[WorldRenderer::AddModelMatrices()] Add model matrices to the Gpu for layer {} and zoom {}.",
        magic_enum::enum_name(t_layerType),
        magic_enum::enum_name(t_zoom)
    );

    // enum to int
    const auto zoomInt{ magic_enum::enum_integer(t_zoom) };
    const auto layerTypeInt{ magic_enum::enum_integer(t_layerType) };

    // get the layer
    const auto& layer{ m_world->GetLayer(t_layerType) };

    // bind Vao of the given zoom
    m_vaos.at(layerTypeInt).at(zoomInt)->Bind();

    // store model matrices in [0]
    std::vector<std::unique_ptr<ogl::buffer::Ssbo>> ssbos;
    magic_enum::enum_for_each<world::Rotation>([&](const world::Rotation t_rotation) {
        const auto& modelMatrices{ layer.GetModelMatrices(t_zoom).at(magic_enum::enum_integer(t_rotation)) };

        auto ssbo{ std::make_unique<ogl::buffer::Ssbo>() };
        ssbo->Bind();
        ssbo->StoreData(modelMatrices.size() * sizeof(glm::mat4), modelMatrices.data());
        ssbo->Unbind();
        ssbos.emplace_back(std::move(ssbo));
    });
    m_vaos.at(layerTypeInt).at(zoomInt)->ssbos.emplace_back(std::move(ssbos));

    // unbind Vao
    ogl::buffer::Vao::Unbind();
}

void mdcii::renderer::WorldRenderer::AddTextureInfo(const world::Zoom t_zoom, const world::WorldLayerType t_layerType) const
{
    Log::MDCII_LOG_DEBUG(
        "[WorldRenderer::AddTextureInfo()] Add texture info to the Gpu for layer {} and zoom {}.",
        magic_enum::enum_name(t_layerType),
        magic_enum::enum_name(t_zoom)
    );

    // enum to int
    const auto zoomInt{ magic_enum::enum_integer(t_zoom) };
    const auto layerTypeInt{ magic_enum::enum_integer(t_layerType) };

    // get the layer
    const auto& layer{ m_world->GetLayer(t_layerType) };

    // bind Vao of the given zoom
    m_vaos.at(layerTypeInt).at(zoomInt)->Bind();

    // store texture offsets in [1]
    std::vector<std::unique_ptr<ogl::buffer::Ssbo>> offsetsSsbos;
    magic_enum::enum_for_each<world::Rotation>([&](const world::Rotation t_rotation) {
        const auto rotationInt{ magic_enum::enum_integer(t_rotation) };

        auto offsetSsbo{ std::make_unique<ogl::buffer::Ssbo>() };
        offsetSsbo->Bind();
        offsetSsbo->StoreData(static_cast<uint32_t>(layer.offsets.at(zoomInt).at(rotationInt).size()) * sizeof(glm::vec2), layer.offsets.at(zoomInt).at(rotationInt).data());
        offsetSsbo->Unbind();
        offsetsSsbos.emplace_back(std::move(offsetSsbo));
    });
    m_vaos.at(layerTypeInt).at(zoomInt)->ssbos.emplace_back(std::move(offsetsSsbos));

    // store texture atlas indices in [2]
    std::vector<std::unique_ptr<ogl::buffer::Ssbo>> atlasIndicesSsbos;
    auto atlasSsbo{ std::make_unique<ogl::buffer::Ssbo>() };
    atlasSsbo->Bind();
    atlasSsbo->StoreData(static_cast<uint32_t>(layer.textureAtlasNumbers.at(zoomInt).size()) * sizeof(glm::ivec4), layer.textureAtlasNumbers.at(zoomInt).data());
    atlasSsbo->Unbind();
    atlasIndicesSsbos.emplace_back(std::move(atlasSsbo));
    m_vaos.at(layerTypeInt).at(zoomInt)->ssbos.emplace_back(std::move(atlasIndicesSsbos));

    // store heights in [3]
    std::vector<std::unique_ptr<ogl::buffer::Ssbo>> heightsSsbos;
    auto heightSsbo{ std::make_unique<ogl::buffer::Ssbo>() };
    heightSsbo->Bind();
    heightSsbo->StoreData(static_cast<uint32_t>(layer.heights.at(zoomInt).size()) * sizeof(glm::vec4), layer.heights.at(zoomInt).data());
    heightSsbo->Unbind();
    heightsSsbos.emplace_back(std::move(heightSsbo));
    m_vaos.at(layerTypeInt).at(zoomInt)->ssbos.emplace_back(std::move(heightsSsbos));

    // unbind Vao
    ogl::buffer::Vao::Unbind();
}
