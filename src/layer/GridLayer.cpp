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

#include "GridLayer.h"
#include "MdciiAssert.h"
#include "state/State.h"
#include "file/OriginalResourcesManager.h"
#include "file/BshFile.h"
#include "renderer/RenderUtils.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::layer::GridLayer::GridLayer(std::shared_ptr<state::Context> t_context, world::World* t_world)
    : GameLayer(std::move(t_context), t_world)
{
    Log::MDCII_LOG_DEBUG("[GridLayer::GridLayer()] Create GridLayer.");
}

mdcii::layer::GridLayer::~GridLayer() noexcept
{
    Log::MDCII_LOG_DEBUG("[GridLayer::~GridLayer()] Destruct GridLayer.");
}

//-------------------------------------------------
// Override
//-------------------------------------------------

void mdcii::layer::GridLayer::CreateModelMatricesContainer()
{
    Log::MDCII_LOG_DEBUG("[GridLayer::CreateModelMatricesContainer()] Create model matrices container.");

    MDCII_ASSERT(modelMatrices.at(0).at(0).empty(), "[GridLayer::CreateModelMatricesContainer()] Invalid model matrices container size.")
    MDCII_ASSERT(!sortedTiles.empty(), "[GridLayer::CreateModelMatricesContainer()] Missing Tile objects.")

    magic_enum::enum_for_each<world::Zoom>([this](const world::Zoom t_zoom) {

        Model_Matrices_For_Each_Rotation matricesForRotations;
        magic_enum::enum_for_each<world::Rotation>([this, &t_zoom, &matricesForRotations](const world::Rotation t_rotation) {
            const auto rotationInt{ magic_enum::enum_integer(t_rotation) };

            std::vector<glm::mat4> matrices;
            for (const auto& tile : sortedTiles.at(rotationInt))
            {
                if (tile->HasBuilding() && m_context->originalResourcesManager->GetBuildingById(tile->buildingId).posoffs > 0)
                {
                    matrices.emplace_back(CreateModelMatrix(*tile, t_zoom, t_rotation));
                }
            }

            matricesForRotations.at(rotationInt) = matrices;
        });

        modelMatrices.at(magic_enum::enum_integer(t_zoom)) = matricesForRotations;
    });

    instancesToRender = static_cast<int32_t>(modelMatrices.at(0).at(0).size());
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

glm::mat4 mdcii::layer::GridLayer::CreateModelMatrix(const layer::Tile& t_tile, const world::Zoom t_zoom, const world::Rotation t_rotation) const
{
    const auto& stadtfldBshTextures{ m_context->originalResourcesManager->GetStadtfldBshByZoom(t_zoom) };
    const auto w{ static_cast<float>(stadtfldBshTextures[GRASS_GFX]->width) };
    const auto h{ static_cast<float>(stadtfldBshTextures[GRASS_GFX]->height) };

    auto screenPosition{ t_tile.screenPositions.at(magic_enum::enum_integer(t_zoom)).at(magic_enum::enum_integer(t_rotation)) };
    screenPosition.y -= h - static_cast<float>(get_tile_height(t_zoom));
    screenPosition.y -= static_cast<float>(get_elevation(t_zoom));

    return renderer::RenderUtils::GetModelMatrix(screenPosition, { w, h });
}
