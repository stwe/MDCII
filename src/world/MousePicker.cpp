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

#include "MousePicker.h"
#include "MdciiAssert.h"
#include "Game.h"
#include "resource/AssetManager.h"
#include "world/World.h"
#include "camera/Camera.h"
#include "state/State.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::MousePicker::MousePicker(World* t_world)
    : m_world{ t_world }
{
    MDCII_LOG_DEBUG("[MousePicker::MousePicker()] Create MousePicker.");

    MDCII_ASSERT(m_world, "[MousePicker::MousePicker()] Null pointer.")
}

mdcii::world::MousePicker::~MousePicker() noexcept
{
    MDCII_LOG_DEBUG("[MousePicker::~MousePicker()] Destruct MousePicker.");
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::world::MousePicker::OnUserUpdate()
{
    UpdateMousePosition();

    const auto* cheatSprite{ m_world->state->game->assetManager->GetAsset(resource::Asset::CHEAT, m_world->camera->zoom)->Sprite() };

    selected = CalculateSelected();
    selected += CalculateSelectedCheatColorOffset(cheatSprite->GetPixel(m_cellOffset.x, m_cellOffset.y));

    olc::vf2d screenPosition{ m_world->ToScreen(selected.x, selected.y) };
    if (m_calcForTerrain)
    {
        screenPosition.y -= get_elevation(m_world->camera->zoom);
    }
    auto* mouseSprite{ m_world->state->game->assetManager->GetAsset(resource::Asset::PURPLE_ISO, m_world->camera->zoom)->Decal() };

    m_world->state->game->DrawDecal(screenPosition, mouseSprite);

    m_world->state->game->DrawString(4, 4, fmt::format("Tile: {}, {}", std::to_string(selected.x), std::to_string(selected.y)), olc::WHITE);
}

//-------------------------------------------------
// ImGui
//-------------------------------------------------

void mdcii::world::MousePicker::RenderImGui()
{
    ImGui::Separator();
    ImGui::Checkbox("Mouse For Terrain", &m_calcForTerrain);
    ImGui::Separator();
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::world::MousePicker::UpdateMousePosition()
{
    m_mousePosition = olc::vi2d(m_world->state->game->GetMouseX(), m_world->state->game->GetMouseY());
    if (m_calcForTerrain)
    {
        m_cell = olc::vi2d(
            m_mousePosition.x / get_tile_width(m_world->camera->zoom),
            (m_mousePosition.y + static_cast<int>(get_elevation(m_world->camera->zoom))) / get_tile_height(m_world->camera->zoom)
        );
        m_cellOffset = olc::vi2d(
            m_mousePosition.x % get_tile_width(m_world->camera->zoom),
            (m_mousePosition.y + static_cast<int>(get_elevation(m_world->camera->zoom))) % get_tile_height(m_world->camera->zoom)
        );
    }
    else
    {
        m_cell = olc::vi2d(
            m_mousePosition.x / get_tile_width(m_world->camera->zoom),
            m_mousePosition.y / get_tile_height(m_world->camera->zoom)
        );
        m_cellOffset = olc::vi2d(
            m_mousePosition.x % get_tile_width(m_world->camera->zoom),
            m_mousePosition.y % get_tile_height(m_world->camera->zoom)
        );
    }
}

olc::vi2d mdcii::world::MousePicker::CalculateSelected() const
{
    switch(m_world->camera->rotation)
    {
    case Rotation::DEG0:
        return {
            (m_cell.y - static_cast<int>(m_world->camera->origin.y))
            + (m_cell.x - static_cast<int>(m_world->camera->origin.x)),
            (m_cell.y - static_cast<int>(m_world->camera->origin.y))
            - (m_cell.x - static_cast<int>(m_world->camera->origin.x))
        };
    case Rotation::DEG90:
        return {
            (m_cell.y - static_cast<int>(m_world->camera->origin.y))
            - (m_cell.x - static_cast<int>(m_world->camera->origin.x)),
            m_world->worldWidth - 1 - ((m_cell.x - static_cast<int>(m_world->camera->origin.x))
                                                 + (m_cell.y - static_cast<int>(m_world->camera->origin.y)))
        };
    case Rotation::DEG180:
        return {
            m_world->worldWidth - 1 - ((m_cell.y - static_cast<int>(m_world->camera->origin.y))
                                                 + (m_cell.x - static_cast<int>(m_world->camera->origin.x))),
            m_world->worldHeight - 1 - ((m_cell.y - static_cast<int>(m_world->camera->origin.y))
                                                 - (m_cell.x - static_cast<int>(m_world->camera->origin.x)))
        };
    case Rotation::DEG270:
        return {
            m_world->worldHeight - 1 - ((m_cell.y - static_cast<int>(m_world->camera->origin.y))
                                                  - (m_cell.x - static_cast<int>(m_world->camera->origin.x))),
            (m_cell.y - static_cast<int>(m_world->camera->origin.y))
            + (m_cell.x - static_cast<int>(m_world->camera->origin.x))
        };
    }

    return {};
}

olc::vi2d mdcii::world::MousePicker::CalculateSelectedCheatColorOffset(const olc::Pixel& t_cheatColor) const
{
    olc::vi2d offset;

    switch (m_world->camera->rotation)
    {
    case Rotation::DEG0:
        if (t_cheatColor == olc::RED)
        {
            offset = { -1, 0 };
        }
        else if (t_cheatColor == olc::BLUE)
        {
            offset = { 0, 1 };
        }
        else if (t_cheatColor == olc::GREEN)
        {
            offset = { 0, -1 };
        }
        else if (t_cheatColor == olc::YELLOW)
        {
            offset = { 1, 0 };
        }
        break;
    case Rotation::DEG90:
        if (t_cheatColor == olc::RED)
        {
            offset = { 0, 1 };
        }
        else if (t_cheatColor == olc::BLUE)
        {
            offset = { 1, 0 };
        }
        else if (t_cheatColor == olc::GREEN)
        {
            offset = { -1, 0 };
        }
        else if (t_cheatColor == olc::YELLOW)
        {
            offset = { 0, -1 };
        }
        break;
    case Rotation::DEG180:
        if (t_cheatColor == olc::RED)
        {
            offset = { 1, 0 };
        }
        else if (t_cheatColor == olc::BLUE)
        {
            offset = { 0, -1 };
        }
        else if (t_cheatColor == olc::GREEN)
        {
            offset = { 0, 1 };
        }
        else if (t_cheatColor == olc::YELLOW)
        {
            offset = { -1, 0 };
        }
        break;
    case Rotation::DEG270:
        if (t_cheatColor == olc::RED)
        {
            offset = { 0, -1 };
        }
        else if (t_cheatColor == olc::BLUE)
        {
            offset = { -1, 0 };
        }
        else if (t_cheatColor == olc::GREEN)
        {
            offset = { 1, 0 };
        }
        else if (t_cheatColor == olc::YELLOW)
        {
            offset = { 0, 1 };
        }
        break;
    }

    return offset;
}
