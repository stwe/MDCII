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
#include "MdciiUtils.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::MousePicker::MousePicker(GameState* t_gameState)
    : m_gameState{ t_gameState }
{
    MDCII_LOG_DEBUG("[MousePicker::MousePicker()] Create MousePicker.");

    MDCII_ASSERT(m_gameState, "[MousePicker::MousePicker()] Null pointer.")

    Init();
}

mdcii::world::MousePicker::~MousePicker() noexcept
{
    MDCII_LOG_DEBUG("[MousePicker::~MousePicker()] Destruct MousePicker.");
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::world::MousePicker::Render()
{
    Update();

    m_gameState->game->DrawDecal(
        olc::vf2d(
            static_cast<float>(m_cell.x) * static_cast<float>(get_tile_width(m_gameState->zoom)),
            static_cast<float>(m_cell.y) * static_cast<float>(get_tile_height(m_gameState->zoom))
        ),
        m_rectSprites[magic_enum::enum_integer(m_gameState->zoom)]->Decal()
    );

    m_gameState->game->DrawString(4, 4, fmt::format("Mouse: {}, {}", std::to_string(m_mousePosition.x), std::to_string(m_mousePosition.y)), olc::WHITE);
    m_gameState->game->DrawString(4, 14, fmt::format("Cell: {}, {}", std::to_string(m_cell.x), std::to_string(m_cell.y)), olc::WHITE);
    m_gameState->game->DrawString(4, 24, fmt::format("Offset: {}, {}", std::to_string(m_cellOffset.x), std::to_string(m_cellOffset.y)), olc::WHITE);
}

void mdcii::world::MousePicker::Update()
{
    m_mousePosition = olc::vi2d(m_gameState->game->GetMouseX(), m_gameState->game->GetMouseY());
    m_cell = olc::vi2d(m_mousePosition.x / get_tile_width(m_gameState->zoom), m_mousePosition.y / get_tile_height(m_gameState->zoom));
    m_cellOffset = olc::vi2d( m_mousePosition.x % get_tile_width(m_gameState->zoom), m_mousePosition.y % get_tile_height(m_gameState->zoom));
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::world::MousePicker::Init()
{
    MDCII_LOG_DEBUG("[MousePicker::Init()] Generate sprites needed by mouse picker ...");

    for (const auto zoom : magic_enum::enum_values<world::Zoom>())
    {
        const auto zoomInt{ magic_enum::enum_integer(zoom) };
        const auto zoomStr{ to_lower_case(std::string(magic_enum::enum_name(zoom))) };

        // rect sprites
        auto rectSprite{ std::make_unique<olc::Renderable>() };
        if (rectSprite->Load(fmt::format("{}textures/{}/frame_{}.png", Game::RESOURCES_REL_PATH, zoomStr, zoomStr)) != olc::OK)
        {
            throw MDCII_EXCEPTION("[MousePicker::Init()] Error while loading file.");
        }
        MDCII_ASSERT(rectSprite->Decal(), "[MousePicker::Init()] Null pointer.")
        m_rectSprites.at(zoomInt) = std::move(rectSprite);

        // cheat sprites
        auto cheatSprite{ std::make_unique<olc::Renderable>() };
        if (cheatSprite->Load(fmt::format("{}textures/{}/corner_{}.png", Game::RESOURCES_REL_PATH, zoomStr, zoomStr)) != olc::OK)
        {
            throw MDCII_EXCEPTION("[MousePicker::Init()] Error while loading file.");
        }
        MDCII_ASSERT(cheatSprite->Decal(), "[MousePicker::Init()] Null pointer.")
        m_cheatSprites.at(zoomInt) = std::move(cheatSprite);
    }

    MDCII_LOG_DEBUG("[MousePicker::Init()] The sprites have been created successfully.");
}
