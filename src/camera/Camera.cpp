// This file is part of the MDCII project.
//
// Copyright (c) 2024. stwe <https://github.com/stwe/MDCII>
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

#include "Camera.h"
#include "Game.h"
#include "MdciiAssert.h"
#include "Intl.h"
#include "world/World.h"
#include "state/State.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::camera::Camera::Camera(const world::World* t_world)
    : m_world{ t_world }
{
    MDCII_LOG_DEBUG("[Camera::Camera()] Create Camera.");

    MDCII_ASSERT(m_world, "[Camera::Camera()] Null pointer.")
}

mdcii::camera::Camera::~Camera() noexcept
{
    MDCII_LOG_DEBUG("[Camera::~Camera()] Destruct Camera.");
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

bool mdcii::camera::Camera::OnUserUpdate(const float t_elapsedTime)
{
    // the array containing the tiles currently being rendered must be updated
    // if the flag is set
    auto dirty{ false };

    // zoom
    if (m_world->state->game->GetMouseWheel() > 0)
    {
        --zoom;
        dirty = true;
        MDCII_LOG_DEBUG("[Camera::OnUserUpdate()] Zoom-- {}", magic_enum::enum_name(zoom));
    }
    if (m_world->state->game->GetMouseWheel() < 0)
    {
        ++zoom;
        dirty = true;
        MDCII_LOG_DEBUG("[Camera::OnUserUpdate()] Zoom++ {}", magic_enum::enum_name(zoom));
    }

    // world
    olc::vf2d vel{ 0.0f, 0.0f };
    if (m_world->state->game->GetKey(olc::Key::UP).bHeld)
    {
        vel += { 0, -1 };
        dirty = true;
    }
    if (m_world->state->game->GetKey(olc::Key::DOWN).bHeld)
    {
        vel += { 0, 1 };
        dirty = true;
    }
    if (m_world->state->game->GetKey(olc::Key::LEFT).bHeld)
    {
        vel += { -1, 0 };
        dirty = true;
    }
    if (m_world->state->game->GetKey(olc::Key::RIGHT).bHeld)
    {
        vel += { 1, 0 };
        dirty = true;
    }

    origin.x += vel.x * t_elapsedTime * SPEED;
    origin.y += vel.y * t_elapsedTime * SPEED;

    worldPosition.x = static_cast<int>(origin.x);
    worldPosition.y = static_cast<int>(origin.y);

    screenPosition.x = worldPosition.x * get_tile_width(zoom);
    screenPosition.y = worldPosition.y * get_tile_height(zoom);

    m_world->state->game->DrawString(4, 4, fmt::format("Camera world: {}, {}", std::to_string(worldPosition.x), std::to_string(worldPosition.y)), olc::WHITE);
    m_world->state->game->DrawString(4, 14, fmt::format("Camera screen: {}, {}", std::to_string(screenPosition.x), std::to_string(screenPosition.y)), olc::WHITE);

    return dirty;
}

//-------------------------------------------------
// ImGui
//-------------------------------------------------

bool mdcii::camera::Camera::RenderImGui()
{
    ImGui::Text("Map rotation %s", magic_enum::enum_name(rotation).data());

    return RotateGui();
}

//-------------------------------------------------
// Gui Helper
//-------------------------------------------------

bool mdcii::camera::Camera::RotateGui()
{
    auto dirty{ false };

    static auto r{ false };
    static auto l{ false };
    rotation == world::Rotation::DEG270 ? r = true : r = false;
    rotation == world::Rotation::DEG0 ? l = true : l = false;

    ImGui::BeginDisabled(r);
    if (ImGui::Button("->##CamRot"))
    {
        ++rotation;

        MDCII_LOG_DEBUG("[Camera::RotateGui()] Camera rotation++ {}", magic_enum::enum_name(rotation));

        dirty = true;
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(66, 104, 188, 255));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        ImGui::SetTooltip(_("Rotate map right"));
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
    ImGui::EndDisabled();

    ImGui::SameLine();

    ImGui::BeginDisabled(l);
    if (ImGui::Button("<-##CamRot"))
    {
        --rotation;

        MDCII_LOG_DEBUG("[Camera::RotateGui()] Camera rotation-- {}", magic_enum::enum_name(rotation));

        dirty = true;
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(66, 104, 188, 255));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        ImGui::SetTooltip(_("Rotate map left"));
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
    ImGui::EndDisabled();

    return dirty;
}
