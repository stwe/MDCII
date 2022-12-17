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

#include "WorldGridLayer.h"
#include "MdciiAssert.h"
#include "state/State.h"
#include "file/OriginalResourcesManager.h"
#include "renderer/RenderUtils.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::layer::WorldGridLayer::WorldGridLayer(std::shared_ptr<state::Context> t_context)
    : GameLayer(std::move(t_context))
{
    Log::MDCII_LOG_DEBUG("[WorldGridLayer::WorldGridLayer()] Create WorldGridLayer.");

    width = worldWidth;
    height = worldHeight;
    instancesToRender = width * height;
}

mdcii::layer::WorldGridLayer::~WorldGridLayer() noexcept
{
    Log::MDCII_LOG_DEBUG("[WorldGridLayer::~WorldGridLayer()] Destruct WorldGridLayer.");
}

//-------------------------------------------------
// Override
//-------------------------------------------------

void mdcii::layer::WorldGridLayer::CreateTiles()
{
    Log::MDCII_LOG_DEBUG("[WorldGridLayer::CreateTiles()] Create and prepare Tile objects for rendering.");

    for (auto i{ 0 }; i < instancesToRender; ++i)
    {
        tiles.emplace_back(std::make_unique<Tile>());
    }

    for (auto y{ 0 }; y < height; ++y)
    {
        for (auto x{ 0 }; x < width; ++x)
        {
            PreCalcTile(*tiles.at(GetMapIndex(x, y, world::Rotation::DEG0)), x, y);
        }
    }
}

void mdcii::layer::WorldGridLayer::SortTiles()
{
    Log::MDCII_LOG_DEBUG("[WorldGridLayer::SortTiles()] Sorting Tile objects by index.");

    MDCII_ASSERT(!tiles.empty(), "[WorldGridLayer::SortTiles()] Missing Tile objects.")

    magic_enum::enum_for_each<world::Rotation>([this](const world::Rotation t_rotation) {
        const auto rotationInt{ magic_enum::enum_integer(t_rotation) };

        // sort tiles by index
        std::sort(tiles.begin(), tiles.end(), [&](const std::shared_ptr<Tile>& t_a, const std::shared_ptr<Tile>& t_b) {
            return t_a->indices[rotationInt] < t_b->indices[rotationInt];
        });

        // copy sorted tiles
        sortedTiles.at(rotationInt) = tiles;
    });

    // revert tiles sorting = sortedTiles DEG0
    tiles = sortedTiles.at(magic_enum::enum_integer(world::Rotation::DEG0));
}

void mdcii::layer::WorldGridLayer::CreateModelMatricesContainer()
{
    Log::MDCII_LOG_DEBUG("[WorldGridLayer::CreateModelMatricesContainer()] Create model matrices container.");

    MDCII_ASSERT(modelMatrices.at(0).at(0).empty(), "[WorldGridLayer::CreateModelMatricesContainer()] Invalid model matrices container size.")
    MDCII_ASSERT(!sortedTiles.empty(), "[WorldGridLayer::CreateModelMatricesContainer()] Missing Tile objects.")

    magic_enum::enum_for_each<world::Zoom>([this](const world::Zoom t_zoom) {

        Model_Matrices_For_Each_Rotation matricesForRotations;
        magic_enum::enum_for_each<world::Rotation>([this, &t_zoom, &matricesForRotations](const world::Rotation t_rotation) {
            const auto rotationInt{ magic_enum::enum_integer(t_rotation) };

            std::vector<glm::mat4> matrices;
            int32_t instance{ 0 };
            for (const auto& tile : sortedTiles.at(rotationInt))
            {
                matrices.emplace_back(CreateModelMatrix(*tile, t_zoom, t_rotation));
                tile->instanceIds.at(rotationInt) = instance;

                instance++;
            }

            matricesForRotations.at(rotationInt) = matrices;
        });

        modelMatrices.at(magic_enum::enum_integer(t_zoom)) = matricesForRotations;
    });
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::layer::WorldGridLayer::PreCalcTile(layer::Tile& t_tile, const int32_t t_x, const int32_t t_y) const
{
    // set world position for Deg0
    t_tile.worldXDeg0 = t_x;
    t_tile.worldYDeg0 = t_y;

    // pre-calculate the position on the screen for each zoom and each rotation
    magic_enum::enum_for_each<world::Zoom>([this, t_x, t_y, &t_tile](const world::Zoom t_zoom) {
        std::array<glm::vec2, world::NR_OF_ROTATIONS> positions{};

        positions[0] = WorldToScreen(t_x, t_y, t_zoom, world::Rotation::DEG0);
        positions[1] = WorldToScreen(t_x, t_y, t_zoom, world::Rotation::DEG90);
        positions[2] = WorldToScreen(t_x, t_y, t_zoom, world::Rotation::DEG180);
        positions[3] = WorldToScreen(t_x, t_y, t_zoom, world::Rotation::DEG270);

        t_tile.screenPositions.at(magic_enum::enum_integer(t_zoom)) = positions;
    });

    // pre-calculate the index for each rotation for sorting
    t_tile.indices[0] = GetMapIndex(t_x, t_y, world::Rotation::DEG0);
    t_tile.indices[1] = GetMapIndex(t_x, t_y, world::Rotation::DEG90);
    t_tile.indices[2] = GetMapIndex(t_x, t_y, world::Rotation::DEG180);
    t_tile.indices[3] = GetMapIndex(t_x, t_y, world::Rotation::DEG270);
}

glm::mat4 mdcii::layer::WorldGridLayer::CreateModelMatrix(const layer::Tile& t_tile, const world::Zoom t_zoom, const world::Rotation t_rotation) const
{
    const auto& stadtfldBshTextures{ m_context->originalResourcesManager->GetStadtfldBshByZoom(t_zoom) };
    const auto w{ static_cast<float>(stadtfldBshTextures[GRASS_GFX]->width) };
    const auto h{ static_cast<float>(stadtfldBshTextures[GRASS_GFX]->height) };

    auto screenPosition{ t_tile.screenPositions.at(magic_enum::enum_integer(t_zoom)).at(magic_enum::enum_integer(t_rotation)) };
    screenPosition.y -= h - static_cast<float>(get_tile_height(t_zoom));

    return renderer::RenderUtils::GetModelMatrix(screenPosition, { w, h });
}
