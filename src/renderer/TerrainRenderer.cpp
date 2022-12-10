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

#include "TerrainRenderer.h"
#include "RenderUtils.h"
#include "state/State.h"
#include "file/OriginalResourcesManager.h"
#include "camera/Camera.h"
#include "ogl/Window.h"
#include "ogl/resource/ResourceManager.h"
#include "ogl/resource/TextureUtils.h"
#include "ogl/buffer/Ssbo.h"
#include "world/TileAtlas.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::renderer::TerrainRenderer::TerrainRenderer(std::shared_ptr<state::Context> t_context, std::shared_ptr<world::TileAtlas> t_tileAtlas)
    : m_context{ std::move(t_context) }
    , m_tileAtlas{ std::move(t_tileAtlas) }
{
    Log::MDCII_LOG_DEBUG("[TerrainRenderer::TerrainRenderer()] Create TerrainRenderer.");

    MDCII_ASSERT(m_context, "[TerrainRenderer::TerrainRenderer()] Null pointer.")
    MDCII_ASSERT(m_tileAtlas, "[TerrainRenderer::TerrainRenderer()] Null pointer.")

    Init();
}

mdcii::renderer::TerrainRenderer::~TerrainRenderer() noexcept
{
    Log::MDCII_LOG_DEBUG("[TerrainRenderer::~TerrainRenderer()] Destruct TerrainRenderer.");
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::renderer::TerrainRenderer::Update()
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

void mdcii::renderer::TerrainRenderer::Render(const layer::TerrainLayer& t_terrainLayer, const world::Zoom t_zoom, const world::Rotation t_rotation) const
{
    const auto zoomInt{ magic_enum::enum_integer(t_zoom) };
    const auto rotationInt{ magic_enum::enum_integer(t_rotation) };

    const auto& shaderProgram{ ogl::resource::ResourceManager::LoadShaderProgram("shader/world") };
    shaderProgram.Bind();

    shaderProgram.SetUniform("projectionView", m_context->window->GetOrthographicProjectionMatrix() * m_context->camera->GetViewMatrix());
    shaderProgram.SetUniform("diffuseMap", 0);
    shaderProgram.SetUniform("selected", false);
    shaderProgram.SetUniform("worldRotation", rotationInt);
    shaderProgram.SetUniform("updates", m_timeCounter);

    const auto maxY{ world::TileAtlas::HEIGHTS.at(zoomInt) };
    const auto nrOfRows{ static_cast<float>(world::TileAtlas::ROWS.at(zoomInt)) };

    shaderProgram.SetUniform("maxY", maxY);
    shaderProgram.SetUniform("nrOfRows", nrOfRows);

    m_vaos.at(zoomInt)->Bind();

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        MODEL_MATRICES_BINDING,
        t_terrainLayer.modelMatricesSsbos.at(zoomInt).at(rotationInt)->id
    );

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        GFX_NUMBERS_BINDING,
        t_terrainLayer.gfxNumbersSsbo->id
    );

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        BUILDING_IDS_BINDING,
        t_terrainLayer.buildingIdsSsbo->id
    );

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        HEIGHTS_BINDING,
        m_heightsSsbos.at(zoomInt)->id
    );

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        ANIMATIONS_BINDING,
        m_animationSsbo->id
    );

    ogl::resource::TextureUtils::BindForReading(m_tileAtlas->textureIds.at(zoomInt), GL_TEXTURE0, GL_TEXTURE_2D_ARRAY);
    m_vaos.at(zoomInt)->DrawInstanced(t_terrainLayer.instancesToRender);

    ogl::buffer::Vao::Unbind();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::renderer::TerrainRenderer::Init()
{
    Log::MDCII_LOG_DEBUG("[TerrainRenderer::Init()] Starts initializing TerrainRenderer...");

    CreateVaos();
    CreateHeightsSsbos();
    CreateAnimationInfoSsbo();

    Log::MDCII_LOG_DEBUG("[TerrainRenderer::Init()] The TerrainRenderer was initialized successfully.");
}

//-------------------------------------------------
// Create buffers
//-------------------------------------------------

void mdcii::renderer::TerrainRenderer::CreateVaos()
{
    Log::MDCII_LOG_DEBUG("[TerrainRenderer::CreateVaos()] Creates all Vaos.");

    magic_enum::enum_for_each<world::Zoom>([this](const world::Zoom t_zoom) {
        m_vaos.at(magic_enum::enum_integer(t_zoom)) = RenderUtils::CreateRectangleVao();
    });
}

void mdcii::renderer::TerrainRenderer::CreateHeightsSsbos()
{
    Log::MDCII_LOG_DEBUG("[TerrainRenderer::CreateHeightsSsbos()] Creates a Ssbo for each zoom level which holding Stadtfld Bsh-Image heights.");

    // for each zoom, read in all Stadtfld Bsh-Image heights
    std::array<std::vector<float>, 3> allHeights;
    magic_enum::enum_for_each<world::Zoom>([this, &allHeights](const world::Zoom t_zoom) {
        const auto& stadtfldBshTextures{ m_context->originalResourcesManager->GetStadtfldBshByZoom(t_zoom) };

        std::vector<float> zoomHeights(stadtfldBshTextures.size(), 0);
        auto i{ 0 };
        for (const auto& bshTexture : stadtfldBshTextures)
        {
            zoomHeights.at(i) = static_cast<float>(bshTexture->height);
            i++;
        }

        allHeights.at(magic_enum::enum_integer(t_zoom)) = zoomHeights;
    });

    // for each zoom, store all Stadtfld Bsh-Image heights for Gpu access
    magic_enum::enum_for_each<world::Zoom>([this, &allHeights](const world::Zoom t_zoom) {
        const auto zoomInt{ magic_enum::enum_integer(t_zoom) };

        m_heightsSsbos.at(zoomInt) = std::make_unique<ogl::buffer::Ssbo>(std::string("Heights-Ssbo_") + magic_enum::enum_name(t_zoom).data());
        m_heightsSsbos.at(zoomInt)->Bind();
        ogl::buffer::Ssbo::StoreData(static_cast<uint32_t>(allHeights.at(zoomInt).size()) * sizeof(float), allHeights.at(zoomInt).data());
        ogl::buffer::Ssbo::Unbind();
    });
}

void mdcii::renderer::TerrainRenderer::CreateAnimationInfoSsbo()
{
    Log::MDCII_LOG_DEBUG("[TerrainRenderer::CreateAnimationInfoSsbo()] Creates a Ssbo which holding animation info for each building.");

    const auto& buildings{ m_context->originalResourcesManager->buildings->buildingsMap };
    std::vector<glm::ivec4> animationInfo(buildings.rbegin()->first + 1, glm::ivec4(-1));

    for (const auto& [k, v] : buildings)
    {
        animationInfo.at(k) = glm::ivec4(v.animAnz, v.animTime, v.animFrame, v.animAdd);
    }

    m_animationSsbo = std::make_unique<ogl::buffer::Ssbo>("Animation-Ssbo");
    m_animationSsbo->Bind();
    ogl::buffer::Ssbo::StoreData(static_cast<uint32_t>(animationInfo.size()) * sizeof(glm::ivec4), animationInfo.data());
    ogl::buffer::Ssbo::Unbind();
}
