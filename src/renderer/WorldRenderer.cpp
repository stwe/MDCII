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

#include <glm/gtx/hash.hpp>
#include "WorldRenderer.h"
#include "RenderUtils.h"
#include "world/World.h"
#include "world/TileAtlas.h"
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
// Logic
//-------------------------------------------------

void mdcii::renderer::WorldRenderer::Update()
{
    // The animation speeds of the buildings are finally defined.
    // So after 90, 130, 150, 180, 220 milliseconds the next frame from the animation is used.
    // This update method is called approximately every 16.7 milliseconds.

    if (m_updates % 6 == 0) // ca. 90ms
    {
        m_timeCounter[0]++;
    }
    if (m_updates % 8 == 0) // ca. 130ms
    {
        m_timeCounter[1]++;
    }
    if (m_updates % 9 == 0) // ca. 150ms
    {
        m_timeCounter[2]++;
    }
    if (m_updates % 11 == 0) // ca. 180ms
    {
        m_timeCounter[3]++;
    }
    if (m_updates % 13 == 0) // ca. 220ms
    {
        m_timeCounter[4]++;
    }

    m_updates++; // is incremented every 16.7 milliseconds

    // reset counter after an arbitrary specified time
    if (m_updates > 4096)
    {
        m_updates = 0;
        memset(&m_timeCounter[0], 0, m_timeCounter.size() * sizeof m_timeCounter[0]);
    }
}

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

    shaderProgram.SetUniform("projectionView", t_window.GetOrthographicProjectionMatrix() * t_camera.GetViewMatrix());
    shaderProgram.SetUniform("diffuseMap", 0);
    shaderProgram.SetUniform("selected", false);
    shaderProgram.SetUniform("worldRotation", rotationInt);
    shaderProgram.SetUniform("updates", m_timeCounter);

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
        HEIGHTS_BINDING,
        m_vaos.at(layerTypeInt).at(zoomInt)->ssbos.at(HEIGHTS_BINDING).at(0)->id
    );

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        GFX_NUMBERS_BINDING,
        m_vaos.at(layerTypeInt).at(zoomInt)->ssbos.at(GFX_NUMBERS_BINDING).at(0)->id
    );

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        BUILDING_IDS_BINDING,
        m_vaos.at(layerTypeInt).at(zoomInt)->ssbos.at(BUILDING_IDS_BINDING).at(0)->id
    );

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        ANIMATIONS_BINDING,
        m_vaos.at(layerTypeInt).at(zoomInt)->ssbos.at(ANIMATIONS_BINDING).at(0)->id
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

//-------------------------------------------------
// Add / remove Tile Gpu data
//-------------------------------------------------

void mdcii::renderer::WorldRenderer::DeleteBuildingFromGpu(const world::Tile& t_tile)
{
    MDCII_ASSERT(t_tile.HasBuilding(), "[WorldRenderer::DeleteBuildingFromGpu()] No building to delete.")

    Log::MDCII_LOG_DEBUG("[WorldRenderer::DeleteBuildingFromGpu()] Delete building Id {} Gpu data of Tile ({}, {}).", t_tile.buildingId, t_tile.worldXDeg0, t_tile.worldYDeg0);

    const auto& terrainLayer{ m_world->GetLayer(world::WorldLayerType::TERRAIN) };

    magic_enum::enum_for_each<world::Zoom>([this, &terrainLayer, &t_tile](const world::Zoom t_zoom) {
        magic_enum::enum_for_each<world::Rotation>([this, &terrainLayer, &t_tile, &t_zoom](const world::Rotation t_rotation) {
            const auto rotationInt{ magic_enum::enum_integer(t_rotation) };
            const auto zoomInt{ magic_enum::enum_integer(t_zoom) };

            // to override TERRAIN_AND_BUILDINGS layer
            const auto& tm{ terrainLayer.modelMatrices[zoomInt][rotationInt] };
            const auto& tg{ terrainLayer.gfxInfo };
            const auto& tb{ terrainLayer.buildingInfo };

            // delete: update Gpu data of BUILDINGS Layer
            UpdateGpuData(
                t_tile.instanceIds[rotationInt],
                world::WorldLayerType::BUILDINGS,
                t_zoom, t_rotation,
                glm::mat4(),   // model matrix
                -1,            // gfx
                -1             // building
            );

            // delete: update Gpu data from TERRAIN_AND_BUILDINGS Layer
            UpdateGpuData(
                t_tile.instanceIds[rotationInt],
                world::WorldLayerType::TERRAIN_AND_BUILDINGS,
                t_zoom, t_rotation,
                tm[t_tile.instanceIds[rotationInt]],
                tg[t_tile.instanceIds[rotationInt]][rotationInt],
                tb[t_tile.instanceIds[rotationInt]][rotationInt]
            );
        });
    });
}

void mdcii::renderer::WorldRenderer::DeleteBuildingFromGpu(std::vector<std::unique_ptr<world::Tile>>& t_tiles)
{
    MDCII_ASSERT(!t_tiles.empty(), "[WorldRenderer::DeleteBuildingFromGpu()] No Tile objects given.")
    for (const auto& tile : t_tiles)
    {
        DeleteBuildingFromGpu(*tile);
    }

    // clear vector
    std::vector<std::unique_ptr<world::Tile>>().swap(t_tiles);
}

void mdcii::renderer::WorldRenderer::DeleteBuildingFromGpu(const std::vector<int32_t>& t_tileIndices)
{
    MDCII_ASSERT(!t_tileIndices.empty(), "[WorldRenderer::DeleteBuildingFromGpu()] No Tile indices given.")
    const auto& layer{ m_world->GetLayer(world::WorldLayerType::BUILDINGS) };
    for (const auto tileIndex : t_tileIndices)
    {
        DeleteBuildingFromGpu(*layer.tiles[tileIndex]);
    }
}

void mdcii::renderer::WorldRenderer::DeleteBuildingFromCpu(world::Tile& t_tile)
{
    MDCII_ASSERT(t_tile.HasBuilding(), "[WorldRenderer::DeleteBuildingFromCpu()] No building to delete.")

    Log::MDCII_LOG_DEBUG("[WorldRenderer::DeleteBuildingFromCpu()] Delete building Id {} Cpu data of Tile ({}, {}).", t_tile.buildingId, t_tile.worldXDeg0, t_tile.worldYDeg0);

    t_tile.ResetBuildingInfo();
}

void mdcii::renderer::WorldRenderer::DeleteBuildingFromCpu(const std::vector<int32_t>& t_tileIndices) const
{
    MDCII_ASSERT(!t_tileIndices.empty(), "[WorldRenderer::DeleteBuildingFromCpu()] No Tile indices given.")
    const auto& layer{ m_world->GetLayer(world::WorldLayerType::BUILDINGS) };
    for (const auto tileIndex : t_tileIndices)
    {
        DeleteBuildingFromCpu(*layer.tiles[tileIndex]);
    }
}

void mdcii::renderer::WorldRenderer::AddBuildingToGpu(
    const data::Building& t_building,
    world::Rotation t_buildingRotation,
    const int t_x, const int t_y,
    std::vector<std::unique_ptr<world::Tile>>& t_tiles
)
{
    MDCII_ASSERT(t_tiles.empty(), "[WorldRenderer::AddBuildingToGpu()] Invalid number of tiles.")

    Log::MDCII_LOG_DEBUG("[WorldRenderer::AddBuildingToGpu()] Add building Id {} to Gpu on ({}, {}).", t_building.id, t_x, t_y);

    const auto& terrainLayer{ m_world->GetLayer(world::WorldLayerType::TERRAIN) };
    const auto& mixedLayer{ m_world->GetLayer(world::WorldLayerType::TERRAIN_AND_BUILDINGS) };

    for (auto y{ 0 }; y < t_building.size.h; ++y)
    {
        for (auto x{ 0 }; x < t_building.size.w; ++x)
        {
            // rotate building
            auto rp{ world::rotate_position(x, y, t_building.size.h, t_building.size.w, t_buildingRotation) };
            if (t_buildingRotation == world::Rotation::DEG0 || t_buildingRotation == world::Rotation::DEG180)
            {
                rp = world::rotate_position(x, y, t_building.size.w, t_building.size.h, t_buildingRotation);
            }

            // create a Tile pointer for each part of the building
            auto tile{ std::make_unique<world::Tile>() };
            tile->buildingId = t_building.id;
            tile->rotation = t_buildingRotation;
            tile->x = rp.x;
            tile->y = rp.y;

            // pre-calc and set screen positions / indices / gfx
            m_world->PreCalcTile(*tile, t_x + rp.x, t_y + rp.y);

            // copy the instances Ids from Terrain Layer Tile at the same position
            magic_enum::enum_for_each<world::Rotation>([&terrainLayer, &tile, &t_x, &t_y, &rp](const world::Rotation t_rotation) {
                const auto r{ magic_enum::enum_integer(t_rotation) };
                tile->instanceIds[r] = terrainLayer.instanceIds.at(glm::ivec3(t_x + rp.x, t_y + rp.y, r));
            });

            // create Gpu data for each zoom and each rotation
            magic_enum::enum_for_each<world::Zoom>([this, &mixedLayer, &tile](const world::Zoom t_zoom) {
                magic_enum::enum_for_each<world::Rotation>([this, &mixedLayer, &tile, &t_zoom](const world::Rotation t_rotation) {
                    const auto rotationInt{ magic_enum::enum_integer(t_rotation) };

                    // create new Gpu data
                    const auto modelMatrix{ mixedLayer.GetModelMatrix(*tile, t_zoom, t_rotation) };
                    const auto gfxNumber{ mixedLayer.CalcGfx(*tile, t_rotation) };

                    // add: update Gpu data BUILDINGS
                    UpdateGpuData(
                        tile->instanceIds[rotationInt],
                        world::WorldLayerType::BUILDINGS,
                        t_zoom, t_rotation,
                        modelMatrix,
                        gfxNumber,
                        tile->buildingId
                    );

                    // add: update Gpu data TERRAIN_AND_BUILDINGS
                    UpdateGpuData(
                        tile->instanceIds[rotationInt],
                        world::WorldLayerType::TERRAIN_AND_BUILDINGS,
                        t_zoom, t_rotation,
                        modelMatrix,
                        gfxNumber,
                        tile->buildingId
                    );
                });
            });

            t_tiles.emplace_back(std::move(tile));
        }
    }

    MDCII_ASSERT(t_tiles.size() == t_building.size.w * t_building.size.h, "[WorldRenderer::AddBuildingToGpu()] Invalid number of created tiles.")

    std::vector<int32_t> connected;
    for (const auto& tile : t_tiles)
    {
        connected.push_back(tile->indices[0]);
    }

    for (const auto& tile : t_tiles)
    {
        tile->connectedTiles = connected;
    }
}

void mdcii::renderer::WorldRenderer::UpdateGpuData(
    const int32_t t_instance,
    const world::WorldLayerType t_layerType,
    const world::Zoom t_zoom,
    const world::Rotation t_rotation,
    const glm::mat4& t_modelMatrix,
    const int32_t t_gfxNumber,
    const int32_t t_buildingId
)
{
    // enum to int
    const auto zoomInt{ magic_enum::enum_integer(t_zoom) };
    const auto rotationInt{ magic_enum::enum_integer(t_rotation) };
    const auto layerTypeInt{ magic_enum::enum_integer(t_layerType) };

    // bind Vao of the given zoom
    const auto& vao{ m_vaos.at(layerTypeInt).at(zoomInt) };
    vao->Bind();

    // new model matrix
    const auto& modelMatricesSsbo{ vao->ssbos.at(MODEL_MATRICES_BINDING) };
    modelMatricesSsbo.at(rotationInt)->Bind();
    ogl::buffer::Ssbo::StoreSubData(static_cast<int32_t>(sizeof(glm::mat4)) * t_instance, sizeof(glm::mat4), &t_modelMatrix);
    ogl::buffer::Ssbo::Unbind();

    // calc offset
    const auto rotOffset{ rotationInt * static_cast<int32_t>(sizeof(int32_t)) };

    // new gfx number
    const auto& gfxNumbersSsbo{ vao->ssbos.at(GFX_NUMBERS_BINDING) };
    gfxNumbersSsbo.at(0)->Bind();
    ogl::buffer::Ssbo::StoreSubData((static_cast<int32_t>(sizeof(glm::ivec4)) * t_instance) + rotOffset, sizeof(int32_t), &t_gfxNumber);
    ogl::buffer::Ssbo::Unbind();

    // new building
    const auto& buildingsSsbo{ vao->ssbos.at(BUILDING_IDS_BINDING) };
    buildingsSsbo.at(0)->Bind();
    ogl::buffer::Ssbo::StoreSubData((static_cast<int32_t>(sizeof(glm::ivec4)) * t_instance) + rotOffset, sizeof(int32_t), &t_buildingId);
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
    magic_enum::enum_for_each<world::Zoom>([this](const world::Zoom t_zoom) {
        const auto zoomStr{ to_lower_case(std::string(magic_enum::enum_name<world::Zoom>(t_zoom))) };
        const auto fileName{ "textures/" + zoomStr + "/red_" + zoomStr + ".png" };
        m_gridFileNames.at(magic_enum::enum_integer(t_zoom)) = fileName;
    });

    // init Gpu data
    CreateVaos(world::WorldLayerType::TERRAIN);
    CreateVaos(world::WorldLayerType::BUILDINGS);
    CreateVaos(world::WorldLayerType::TERRAIN_AND_BUILDINGS);
    CreateVaos(world::WorldLayerType::GRID);

    CreateModelMatricesSsbos(world::WorldLayerType::TERRAIN);
    CreateModelMatricesSsbos(world::WorldLayerType::BUILDINGS);
    CreateModelMatricesSsbos(world::WorldLayerType::TERRAIN_AND_BUILDINGS);
    CreateModelMatricesSsbos(world::WorldLayerType::GRID);

    CreateHeightsSsbos(world::WorldLayerType::TERRAIN);
    CreateHeightsSsbos(world::WorldLayerType::BUILDINGS);
    CreateHeightsSsbos(world::WorldLayerType::TERRAIN_AND_BUILDINGS);

    CreateGfxInfoSsbos(world::WorldLayerType::TERRAIN);
    CreateGfxInfoSsbos(world::WorldLayerType::BUILDINGS);
    CreateGfxInfoSsbos(world::WorldLayerType::TERRAIN_AND_BUILDINGS);

    CreateBuildingInfoSsbos(world::WorldLayerType::TERRAIN);
    CreateBuildingInfoSsbos(world::WorldLayerType::BUILDINGS);
    CreateBuildingInfoSsbos(world::WorldLayerType::TERRAIN_AND_BUILDINGS);

    CreateAnimationInfoSsbos(world::WorldLayerType::TERRAIN);
    CreateAnimationInfoSsbos(world::WorldLayerType::BUILDINGS);
    CreateAnimationInfoSsbos(world::WorldLayerType::TERRAIN_AND_BUILDINGS);

    Log::MDCII_LOG_DEBUG("[WorldRenderer::Init()] The WorldRenderer was initialized successfully.");
}

//-------------------------------------------------
// Create buffers
//-------------------------------------------------

void mdcii::renderer::WorldRenderer::CreateVaos(const world::WorldLayerType t_layerType)
{
    Log::MDCII_LOG_DEBUG(
        "[WorldRenderer::CreateVaos()] Creates all Vaos for Layer type {}.",
        magic_enum::enum_name(t_layerType)
    );

    magic_enum::enum_for_each<world::Zoom>([this, t_layerType](const world::Zoom t_zoom) {
        m_vaos.at(magic_enum::enum_integer(t_layerType)).at(magic_enum::enum_integer(t_zoom)) = RenderUtils::CreateRectangleVao();
    });
}

void mdcii::renderer::WorldRenderer::CreateModelMatricesSsbos(const world::WorldLayerType t_layerType) const
{
    Log::MDCII_LOG_DEBUG(
        "[WorldRenderer::CreateModelMatricesSsbos()] Creates all Ssbos which holding model matrices for Layer type {}.",
        magic_enum::enum_name(t_layerType)
    );

    magic_enum::enum_for_each<world::Zoom>([this, t_layerType](const world::Zoom t_zoom) {
        // enum to int
        const auto zoomInt{ magic_enum::enum_integer(t_zoom) };
        const auto layerTypeInt{ magic_enum::enum_integer(t_layerType) };

        // get the layer
        const auto& layer{ m_world->GetLayer(t_layerType) };

        // bind Vao of the given zoom
        m_vaos.at(layerTypeInt).at(zoomInt)->Bind();

        // store model matrices Ssbos in [0][0/DEG0] .. [3/DEG270]
        std::vector<std::unique_ptr<ogl::buffer::Ssbo>> ssbos;
        magic_enum::enum_for_each<world::Rotation>([&layer, &t_zoom, &ssbos](const world::Rotation t_rotation) {
            const auto& modelMatrices{ layer.GetModelMatrices(t_zoom).at(magic_enum::enum_integer(t_rotation)) };

            auto ssbo{ std::make_unique<ogl::buffer::Ssbo>() };
            ssbo->Bind();
            ogl::buffer::Ssbo::StoreData(static_cast<uint32_t>(modelMatrices.size()) * sizeof(glm::mat4), modelMatrices.data());
            ogl::buffer::Ssbo::Unbind();
            ssbos.emplace_back(std::move(ssbo));
        });
        m_vaos.at(layerTypeInt).at(zoomInt)->ssbos.emplace_back(std::move(ssbos));

        // unbind Vao
        ogl::buffer::Vao::Unbind();
    });
}

void mdcii::renderer::WorldRenderer::CreateHeightsSsbos(const world::WorldLayerType t_layerType) const
{
    // todo: data independent of the Layer -> create only 1x

    Log::MDCII_LOG_DEBUG(
        "[WorldRenderer::CreateHeightInfoSsbos()] Creates all Ssbos which holding image heights for Layer type {}.",
        magic_enum::enum_name(t_layerType)
    );

    magic_enum::enum_for_each<world::Zoom>([this, t_layerType](const world::Zoom t_zoom) {
        // enum to int
        const auto zoomInt{ magic_enum::enum_integer(t_zoom) };
        const auto layerTypeInt{ magic_enum::enum_integer(t_layerType) };

        // get the layer
        const auto& layer{ m_world->GetLayer(t_layerType) };

        // bind Vao of the given zoom
        m_vaos.at(layerTypeInt).at(zoomInt)->Bind();

        // store heights in [1][0]
        std::vector<std::unique_ptr<ogl::buffer::Ssbo>> ssbos;
        auto ssbo{ std::make_unique<ogl::buffer::Ssbo>() };
        ssbo->Bind();
        ogl::buffer::Ssbo::StoreData(static_cast<uint32_t>(layer.heights.at(zoomInt).size()) * sizeof(uint32_t), layer.heights.at(zoomInt).data());
        ogl::buffer::Ssbo::Unbind();
        ssbos.emplace_back(std::move(ssbo));
        m_vaos.at(layerTypeInt).at(zoomInt)->ssbos.emplace_back(std::move(ssbos));

        // unbind Vao
        ogl::buffer::Vao::Unbind();
    });
}

void mdcii::renderer::WorldRenderer::CreateGfxInfoSsbos(const world::WorldLayerType t_layerType) const
{
    // todo: create only 1x and then use the buffer for each zoom

    Log::MDCII_LOG_DEBUG(
        "[WorldRenderer::CreateGfxInfoSsbos()] Creates all Ssbos which holding gfx numbers for Layer type {}.",
        magic_enum::enum_name(t_layerType)
    );

    magic_enum::enum_for_each<world::Zoom>([this, t_layerType](const world::Zoom t_zoom) {
        // enum to int
        const auto zoomInt{ magic_enum::enum_integer(t_zoom) };
        const auto layerTypeInt{ magic_enum::enum_integer(t_layerType) };

        // get the layer
        const auto& layer{ m_world->GetLayer(t_layerType) };

        // bind Vao of the given zoom
        m_vaos.at(layerTypeInt).at(zoomInt)->Bind();

        // store gfx numbers in [2][0]
        std::vector<std::unique_ptr<ogl::buffer::Ssbo>> ssbos;
        auto ssbo{ std::make_unique<ogl::buffer::Ssbo>() };
        ssbo->Bind();
        ogl::buffer::Ssbo::StoreData(static_cast<uint32_t>(layer.gfxInfo.size()) * sizeof(glm::ivec4), layer.gfxInfo.data());
        ogl::buffer::Ssbo::Unbind();
        ssbos.emplace_back(std::move(ssbo));
        m_vaos.at(layerTypeInt).at(zoomInt)->ssbos.emplace_back(std::move(ssbos));

        // unbind Vao
        ogl::buffer::Vao::Unbind();
    });
}

void mdcii::renderer::WorldRenderer::CreateBuildingInfoSsbos(const world::WorldLayerType t_layerType) const
{
    // todo: create only 1x and then use the buffer for each zoom

    Log::MDCII_LOG_DEBUG(
        "[WorldRenderer::CreateBuildingInfoSsbos()] Creates all Ssbos which holding building Ids for Layer type {}.",
        magic_enum::enum_name(t_layerType)
    );

    magic_enum::enum_for_each<world::Zoom>([this, t_layerType](const world::Zoom t_zoom) {
        // enum to int
        const auto zoomInt{ magic_enum::enum_integer(t_zoom) };
        const auto layerTypeInt{ magic_enum::enum_integer(t_layerType) };

        // get the layer
        const auto& layer{ m_world->GetLayer(t_layerType) };

        // bind Vao of the given zoom
        m_vaos.at(layerTypeInt).at(zoomInt)->Bind();

        // store gfx numbers in [3][0]
        std::vector<std::unique_ptr<ogl::buffer::Ssbo>> ssbos;
        auto ssbo{ std::make_unique<ogl::buffer::Ssbo>() };
        ssbo->Bind();
        ogl::buffer::Ssbo::StoreData(static_cast<uint32_t>(layer.buildingInfo.size()) * sizeof(glm::ivec4), layer.buildingInfo.data());
        ogl::buffer::Ssbo::Unbind();
        ssbos.emplace_back(std::move(ssbo));
        m_vaos.at(layerTypeInt).at(zoomInt)->ssbos.emplace_back(std::move(ssbos));

        // unbind Vao
        ogl::buffer::Vao::Unbind();
    });
}

void mdcii::renderer::WorldRenderer::CreateAnimationInfoSsbos(const world::WorldLayerType t_layerType) const
{
    // todo: data independent of the Layer -> create only 1x

    Log::MDCII_LOG_DEBUG(
        "[WorldRenderer::CreateAnimationInfoSsbos()] Creates all Ssbos which holding animation info for Layer type {}.",
        magic_enum::enum_name(t_layerType)
    );

    magic_enum::enum_for_each<world::Zoom>([this, t_layerType](const world::Zoom t_zoom) {
        // enum to int
        const auto zoomInt{ magic_enum::enum_integer(t_zoom) };
        const auto layerTypeInt{ magic_enum::enum_integer(t_layerType) };

        // get the layer
        const auto& layer{ m_world->GetLayer(t_layerType) };

        // bind Vao of the given zoom
        m_vaos.at(layerTypeInt).at(zoomInt)->Bind();

        // store gfx numbers in [4][0]
        std::vector<std::unique_ptr<ogl::buffer::Ssbo>> ssbos;
        auto ssbo{ std::make_unique<ogl::buffer::Ssbo>() };
        ssbo->Bind();
        ogl::buffer::Ssbo::StoreData(static_cast<uint32_t>(layer.animationInfo.size()) * sizeof(glm::ivec4), layer.animationInfo.data());
        ogl::buffer::Ssbo::Unbind();
        ssbos.emplace_back(std::move(ssbo));
        m_vaos.at(layerTypeInt).at(zoomInt)->ssbos.emplace_back(std::move(ssbos));

        // unbind Vao
        ogl::buffer::Vao::Unbind();
    });
}
