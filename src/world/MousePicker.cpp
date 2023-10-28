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

#include <fmt/format.h>
#include "MousePicker.h"
#include "Game.h"
#include "Log.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::MousePicker::MousePicker()
{
    MDCII_LOG_DEBUG("[MousePicker::MousePicker()] Create MousePicker.");

    m_rect = std::make_unique<olc::Renderable>();
    m_rect->Load(fmt::format("{}textures/gfx/frame_gfx.png", Game::RESOURCES_REL_PATH));
}

mdcii::world::MousePicker::~MousePicker() noexcept
{
    MDCII_LOG_DEBUG("[MousePicker::~MousePicker()] Create MousePicker.");
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::world::MousePicker::Update(const Game* t_game)
{
    m_mousePosition = olc::vi2d(t_game->GetMouseX(), t_game->GetMouseY());
    m_cell = olc::vi2d(m_mousePosition.x / 64, m_mousePosition.y / 32);
    m_cellOffset = olc::vi2d( m_mousePosition.x % 64, m_mousePosition.y % 32);
}

void mdcii::world::MousePicker::Render(Game* t_game) const
{
    t_game->DrawDecal(olc::vf2d(static_cast<float>(m_cell.x) * 64.0f, static_cast<float>(m_cell.y) * 32.0f), m_rect->Decal());

    t_game->DrawString(4, 4, fmt::format("Mouse: {}, {}", std::to_string(m_mousePosition.x), std::to_string(m_mousePosition.y)), olc::WHITE);
    t_game->DrawString(4, 14, fmt::format("Cell: {}, {}", std::to_string(m_cell.x), std::to_string(m_cell.y)), olc::WHITE);
    t_game->DrawString(4, 24, fmt::format("Offset: {}, {}", std::to_string(m_cellOffset.x), std::to_string(m_cellOffset.y)), olc::WHITE);
}
