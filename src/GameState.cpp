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

#include "GameState.h"
#include "Log.h"
#include "Game.h"
#include "world/MousePicker.h"
#include "world/World.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::GameState::GameState(Game* t_game)
    : State(t_game)
{
    MDCII_LOG_DEBUG("[GameState::GameState()] Create GameState.");
}

mdcii::GameState::~GameState() noexcept
{
    MDCII_LOG_DEBUG("[GameState::~GameState()] Destruct GameState.");
}

//-------------------------------------------------
// Override
//-------------------------------------------------

bool mdcii::GameState::OnUserCreate()
{
    MDCII_LOG_DEBUG("[GameState::OnUserCreate()] Init GameState.");

    mousePicker = std::make_unique<world::MousePicker>(this);
    world = std::make_unique<world::World>(this);

    return true;
}

bool mdcii::GameState::OnUserUpdate(const float t_elapsedTime)
{
    // zoom
    if (game->GetMouseWheel() > 0)
    {
        --zoom;
        MDCII_LOG_DEBUG("Zoom-- {}", magic_enum::enum_name(zoom));
    }
    if (game->GetMouseWheel() < 0)
    {
        ++zoom;
        MDCII_LOG_DEBUG("Zoom++ {}", magic_enum::enum_name(zoom));
    }

    // rotation
    if (game->GetKey(olc::Key::PGUP).bPressed)
    {
        ++rotation;
        MDCII_LOG_DEBUG("World rotation++ {}", magic_enum::enum_name(rotation));
    }
    if (game->GetKey(olc::Key::PGDN).bPressed)
    {
        --rotation;
        MDCII_LOG_DEBUG("World rotation-- {}", magic_enum::enum_name(rotation));
    }

    // camera
    olc::vf2d vel{ 0.0f, 0.0f };
    if (game->GetKey(olc::Key::W).bHeld)
    {
        vel += { 0, -1 };
    }
    if (game->GetKey(olc::Key::S).bHeld)
    {
        vel += { 0, +1 };
    }
    if (game->GetKey(olc::Key::A).bHeld)
    {
        vel += { -1, 0 };
    }
    if (game->GetKey(olc::Key::D).bHeld)
    {
        vel += { +1, 0 };
    }

    // exit
    if (game->GetKey(olc::Key::ESCAPE).bHeld)
    {
        return false;
    }

    // world
    game->SetGameLayer();
    game->Clear(olc::BLACK);
    world->OnUserUpdate(t_elapsedTime, vel);

    // mouse picker
    mousePicker->OnUserUpdate();

    // ImGui
    ImGui::SetNextWindowPos(
        { ImGui::GetMainViewport()->Size.x - 150.0f, 0.0f },
        ImGuiCond_Appearing
    );
    constexpr ImGuiWindowFlags flags{ ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings };

    ImGui::Begin("GameMenu", nullptr, flags);
    if (ImGui::Button("Exit Game"))
    {
        ImGui::End();
        return false;
    }

    ImGui::End();

    return true;
}
