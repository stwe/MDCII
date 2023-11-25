// This file is part of the MDCII project.
//
// Copyright (c) 2023. stwe <https://github.com/stwe/MDCII>
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

#include "Renderer.h"
#include "Game.h"
#include "GameState.h"
#include "MdciiAssert.h"
#include "world/Island.h"
#include "world/DeepWater.h"
#include "world/World.h"
#include "world/Tile.h"
#include "world/Layer.h"
#include "resource/TileAtlas.h"
#include "camera/Camera.h"
#include "resource/Buildings.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::renderer::Renderer::Renderer(const world::World* t_world)
    : m_world{ t_world }
{
    MDCII_LOG_DEBUG("[Renderer::Renderer()] Create Renderer.");

    MDCII_ASSERT(m_world, "[Renderer::Renderer()] Null pointer.")
}

mdcii::renderer::Renderer::~Renderer() noexcept
{
    MDCII_LOG_DEBUG("[Renderer::~Renderer()] Destruct Renderer.");
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

float mdcii::renderer::Renderer::CalcOffset(const world::Tile* t_tile, const int t_gfx) const
{
    auto offset{ 0.0f };
    const auto zoomInt{ magic_enum::enum_integer(m_world->camera->zoom) };
    auto tileHeight{ get_tile_height(m_world->camera->zoom) };
    const auto gfxHeight{ m_world->gameState->game->tileAtlas->heights[zoomInt][t_gfx] };

    if (m_world->camera->zoom == world::Zoom::GFX)
    {
        tileHeight = 31;
    }
    if (gfxHeight > tileHeight)
    {
        offset = static_cast<float>(gfxHeight) - static_cast<float>(tileHeight);
    }
    if (t_tile->building->posoffs > 0) // above water
    {
        offset += world::ELEVATIONS[zoomInt];
    }

    return offset;
}

int mdcii::renderer::Renderer::GetGfxForCurrentRotation(const world::Tile* t_tile) const
{
    MDCII_ASSERT(!t_tile->gfxs.empty(), "[Renderer::GetGfxForCurrentRotation()] Missing gfx values.")

    if (t_tile->gfxs.size() == 1)
    {
        return t_tile->gfxs[0];
    }

    const auto worldRotation{ m_world->camera->rotation + world::int_to_rotation(t_tile->rotation) };
    return t_tile->gfxs[magic_enum::enum_integer(worldRotation)];
}

void mdcii::renderer::Renderer::RenderBuilding(const int t_startX, const int t_startY, const world::Tile* t_tile) const
{
    const auto screenPosition{ m_world->ToScreen(t_tile->posX +  t_startX, t_tile->posY + t_startY) };
    const auto zoomInt{ magic_enum::enum_integer(m_world->camera->zoom) };
    const auto gfx{ GetGfxForCurrentRotation(t_tile) };
    const auto atlasOffset{ GetAtlasOffset(gfx, resource::TileAtlas::NR_OF_ROWS[zoomInt]) };

    m_world->gameState->game->DrawPartialDecal(
        olc::vf2d(
            static_cast<float>(screenPosition.x),
            static_cast<float>(screenPosition.y) - CalcOffset(t_tile, gfx)
        ),
        m_world->gameState->game->tileAtlas->atlas[zoomInt][GetAtlasIndex(gfx, resource::TileAtlas::NR_OF_ROWS[zoomInt])]->Decal(),
        olc::vf2d(
            static_cast<float>(atlasOffset.x) * resource::TileAtlas::LARGEST_SIZE[zoomInt].second.first,
            static_cast<float>(atlasOffset.y) * resource::TileAtlas::LARGEST_SIZE[zoomInt].second.second
        ),
        olc::vf2d(
            resource::TileAtlas::LARGEST_SIZE[zoomInt].second.first,
            resource::TileAtlas::LARGEST_SIZE[zoomInt].second.second
        )
    );
}

void mdcii::renderer::Renderer::RenderIsland(const world::Island* t_island, const world::LayerType t_layerType) const
{
    for (const auto& tile : t_island->layers[magic_enum::enum_integer(t_layerType)]->currentTiles)
    {
        if (tile.HasBuilding())
        {
            RenderBuilding(t_island->startX, t_island->startY, &tile);
        }
    }
}

void mdcii::renderer::Renderer::RenderIslands() const
{
    for (auto const& island : m_world->currentIslands)
    {
        if (m_world->HasRenderLayerOption(world::RenderLayer::RENDER_MIXED_LAYER))
        {
            RenderIsland(island, world::LayerType::MIXED);
        }
        else
        {
            if (m_world->HasRenderLayerOption(world::RenderLayer::RENDER_COAST_LAYER))
            {
                RenderIsland(island, world::LayerType::COAST);
            }
            if (m_world->HasRenderLayerOption(world::RenderLayer::RENDER_TERRAIN_LAYER))
            {
                RenderIsland(island, world::LayerType::TERRAIN);
            }
            if (m_world->HasRenderLayerOption(world::RenderLayer::RENDER_BUILDINGS_LAYER))
            {
                RenderIsland(island, world::LayerType::BUILDINGS);
            }
        }
    }
}

void mdcii::renderer::Renderer::RenderDeepWater() const
{
    if (m_world->HasRenderLayerOption(world::RenderLayer::RENDER_DEEP_WATER_LAYER))
    {
        for (const auto& tile : m_world->deepWater->layer->currentTiles)
        {
            RenderBuilding(0, 0, &tile);
        }
    }
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

int mdcii::renderer::Renderer::GetAtlasIndex(const int t_gfx, const int t_rows)
{
    return t_gfx / (t_rows * t_rows);
}

olc::vi2d mdcii::renderer::Renderer::GetAtlasOffset(const int t_gfx, const int t_rows)
{
    auto picInPic{ t_gfx % (t_rows * t_rows) };

    return { picInPic % t_rows, picInPic / t_rows };
}

int mdcii::renderer::Renderer::GetMapIndex(
    const int t_x, const int t_y,
    const int t_width, const int t_height,
    const world::Rotation t_rotation
)
{
    const auto position{ rotate_position(t_x, t_y, t_width, t_height, t_rotation) };

    if (t_rotation == world::Rotation::DEG0 || t_rotation == world::Rotation::DEG180)
    {
        return position.y * t_width + position.x;
    }

    return position.y * t_height + position.x;
}
