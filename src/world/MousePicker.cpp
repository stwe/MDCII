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
#include "GameState.h"
#include "resource/AssetManager.h"
#include "world/World.h"
#include "camera/Camera.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::MousePicker::MousePicker(GameState* t_gameState)
    : m_gameState{ t_gameState }
{
    MDCII_LOG_DEBUG("[MousePicker::MousePicker()] Create MousePicker.");

    MDCII_ASSERT(m_gameState, "[MousePicker::MousePicker()] Null pointer.")
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
    m_mousePosition = olc::vi2d(m_gameState->game->GetMouseX(), m_gameState->game->GetMouseY());
    m_cell = olc::vi2d(m_mousePosition.x / get_tile_width(m_gameState->zoom), m_mousePosition.y / get_tile_height(m_gameState->zoom));
    m_cellOffset = olc::vi2d( m_mousePosition.x % get_tile_width(m_gameState->zoom), m_mousePosition.y % get_tile_height(m_gameState->zoom));

    const auto* cheatSprite{ m_gameState->game->assetManager->GetAsset(resource::Asset::CHEAT, m_gameState->zoom)->Sprite() };
    const auto cheatColor{ cheatSprite->GetPixel(m_cellOffset.x, m_cellOffset.y) };

    olc::vi2d selected;
    if (m_gameState->rotation == Rotation::DEG0)
    {
        selected = {
            (m_cell.y - static_cast<int>(m_gameState->world->camera->origin.y))
            + (m_cell.x - static_cast<int>(m_gameState->world->camera->origin.x)),
            (m_cell.y - static_cast<int>(m_gameState->world->camera->origin.y))
            - (m_cell.x - static_cast<int>(m_gameState->world->camera->origin.x))
        };

        if (cheatColor == olc::RED)
        {
            selected += { -1, 0 };
        }
        if (cheatColor == olc::BLUE)
        {
            selected += { 0, 1 };
        }
        if (cheatColor == olc::GREEN)
        {
            selected += { 0, -1 };
        }
        if (cheatColor == olc::YELLOW)
        {
            selected += { 1, 0 };
        }
    }
    if (m_gameState->rotation == Rotation::DEG90)
    {
        selected = {
            (m_cell.y - static_cast<int>(m_gameState->world->camera->origin.y))
            - (m_cell.x - static_cast<int>(m_gameState->world->camera->origin.x)),
            m_gameState->world->worldWidth - 1 - ((m_cell.x - static_cast<int>(m_gameState->world->camera->origin.x))
                                                  + (m_cell.y - static_cast<int>(m_gameState->world->camera->origin.y)))
        };

        if (cheatColor == olc::RED)
        {
            selected += { 0, 1 };
        }
        if (cheatColor == olc::BLUE)
        {
            selected += { 1, 0 };
        }
        if (cheatColor == olc::GREEN)
        {
            selected += { -1, 0 };
        }
        if (cheatColor == olc::YELLOW)
        {
            selected += { 0, -1 };
        }
    }
    if (m_gameState->rotation == Rotation::DEG180)
    {
        selected = {
            m_gameState->world->worldWidth - 1 - ((m_cell.y - static_cast<int>(m_gameState->world->camera->origin.y))
                                                  + (m_cell.x
                                                     - static_cast<int>(m_gameState->world->camera->origin.x))),
            m_gameState->world->worldHeight - 1 - ((m_cell.y - static_cast<int>(m_gameState->world->camera->origin.y))
                                                   - (m_cell.x
                                                      - static_cast<int>(m_gameState->world->camera->origin.x)))
        };

        if (cheatColor == olc::RED)
        {
            selected += { 1, 0 };
        }
        if (cheatColor == olc::BLUE)
        {
            selected += { 0, -1 };
        }
        if (cheatColor == olc::GREEN)
        {
            selected += { 0, 1 };
        }
        if (cheatColor == olc::YELLOW)
        {
            selected += { -1, 0 };
        }
    }
    if (m_gameState->rotation == Rotation::DEG270)
    {
        selected = {
            m_gameState->world->worldHeight - 1 - ((m_cell.y - static_cast<int>(m_gameState->world->camera->origin.y))
                                                   - (m_cell.x - static_cast<int>(m_gameState->world->camera->origin.x))),
            (m_cell.y - static_cast<int>(m_gameState->world->camera->origin.y))
            + (m_cell.x - static_cast<int>(m_gameState->world->camera->origin.x))
        };

        if (cheatColor == olc::RED)
        {
            selected += { 0, -1 };
        }
        if (cheatColor == olc::BLUE)
        {
            selected += { -1, 0 };
        }
        if (cheatColor == olc::GREEN)
        {
            selected += { 1, 0 };
        }
        if (cheatColor == olc::YELLOW)
        {
            selected += { 0, 1 };
        }
    }

    const auto toScreen = [&](int t_x, int t_y)
    {
        const auto position{ rotate_position(t_x, t_y, m_gameState->world->worldWidth, m_gameState->world->worldHeight, m_gameState->rotation) };

        return olc::vi2d
            {
                (static_cast<int>(m_gameState->world->camera->origin.x) * get_tile_width(m_gameState->zoom)) + (position.x - position.y) * get_tile_width_half(m_gameState->zoom),
                (static_cast<int>(m_gameState->world->camera->origin.y) * get_tile_height(m_gameState->zoom)) + (position.x + position.y) * get_tile_height_half(m_gameState->zoom)
            };
    };

    const auto ts{ toScreen(selected.x, selected.y) };
    auto* mouseSprite{ m_gameState->game->assetManager->GetAsset(resource::Asset::PURPLE_ISO, m_gameState->zoom)->Decal() };

    m_gameState->game->DrawDecal(
        olc::vf2d(
            static_cast<float>(ts.x),
            static_cast<float>(ts.y)
        ),
        mouseSprite
    );

    m_gameState->game->DrawString(4, 4, fmt::format("Mouse: {}, {}", std::to_string(m_mousePosition.x), std::to_string(m_mousePosition.y)), olc::WHITE);
    m_gameState->game->DrawString(4, 14, fmt::format("Selected: {}, {}", std::to_string(selected.x), std::to_string(selected.y)), olc::WHITE);
}
