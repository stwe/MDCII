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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include <imgui.h>
#include "EditorGui.h"
#include "Game.h"
#include "Log.h"
#include "map/Map.h"
#include "map/MapContent.h"
#include "data/Text.h"
#include "event/EventManager.h"
#include "file/OriginalResourcesManager.h"
#include "state/State.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::EditorGui::EditorGui(std::shared_ptr<map::Map> t_map)
    : m_map{ std::move(t_map) }
{
    Log::MDCII_LOG_DEBUG("[EditorGui::EditorGui()] Create EditorGui.");
}

mdcii::EditorGui::~EditorGui() noexcept
{
    Log::MDCII_LOG_DEBUG("[EditorGui::~EditorGui()] Destruct EditorGui.");
}

//-------------------------------------------------
// Gui
//-------------------------------------------------

void mdcii::EditorGui::RotateMapGui() const
{
    std::string rotStr{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "CurrentMapRotation") };
    rotStr.append(std::string(": %s"));
    ImGui::Text(rotStr.c_str(), rotation_to_string(m_map->mapContent->rotation));

    if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "RotateMapRight").c_str()))
    {
        m_map->Rotate(map::ChangeRotation::RIGHT);
    }

    ImGui::SameLine();

    if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "RotateMapLeft").c_str()))
    {
        m_map->Rotate(map::ChangeRotation::LEFT);
    }
}

void mdcii::EditorGui::ZoomMapGui() const
{
    std::string zoomStr{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "CurrentMapZoom") };
    zoomStr.append(std::string(": %s"));
    ImGui::Text(zoomStr.c_str(), magic_enum::enum_name(m_map->mapContent->zoom).data());

    if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "ZoomMapIn").c_str()))
    {
        m_map->Zoom(map::ChangeZoom::ZOOM_IN);
    }

    ImGui::SameLine();

    if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "ZoomMapOut").c_str()))
    {
        m_map->Zoom(map::ChangeZoom::ZOOM_OUT);
    }
}

void mdcii::EditorGui::ShowActionsGui()
{
    magic_enum::enum_for_each<map::Map::Action>([&](auto t_val) {
        constexpr map::Map::Action action{ t_val };
        constexpr int i{ magic_enum::enum_integer(action) };

        if (m_map->actionButtons[i])
        {
            ImGui::PushID(i);
            ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(7.0f, 0.6f, 0.6f)));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(7.0f, 0.7f, 0.7f)));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(7.0f, 0.8f, 0.8f)));

            ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), map::Map::ACTION_NAMES[i].data()).c_str());
            if (ImGui::IsItemClicked(0))
            {
                if (action != m_map->currentAction)
                {
                    m_map->actionButtons[i] = !m_map->actionButtons[i];
                }
            }

            ImGui::PopStyleColor(3);
            ImGui::PopID();

            ImGui::SameLine();
        }
        else
        {
            if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), map::Map::ACTION_NAMES[i].data()).c_str()))
            {
                std::fill(m_map->actionButtons.begin(), m_map->actionButtons.end(), false);
                m_map->actionButtons[i] = true;
                m_map->currentAction = action;

                Log::MDCII_LOG_DEBUG("[EditorGui::ShowActionsGui()] Change to action: {}", magic_enum::enum_name(m_map->currentAction));

                m_info = false;
            }

            ImGui::SameLine();
        }
    });

    ImGui::NewLine();
}

void mdcii::EditorGui::AllWorkshopsGui() const
{
    if (ImGui::TreeNode(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Workshops").c_str()))
    {
        const auto& shops{ data::Text::GetBuildingsTexts(data::Text::Section::WORKSHOPS, Game::INI.Get<std::string>("locale", "lang")) };

        for (const auto& [k, v] : shops)
        {
            if (ImGui::TreeNode(v.c_str()))
            {
                const auto& building{ m_map->context->originalResourcesManager->GetBuildingById(std::stoi(k)) };
                const auto& bauhausBshTextures{ m_map->context->originalResourcesManager->GetBauhausBshByZoom(m_map->mapContent->bauhausZoom) };

                const auto textureWidth{ bauhausBshTextures.at(building.baugfx)->width };
                const auto textureHeight{ bauhausBshTextures.at(building.baugfx)->height };
                const auto textureId{ reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(bauhausBshTextures.at(building.baugfx)->textureId)) };

                if (ImGui::ImageButton(
                        textureId,
                        ImVec2(static_cast<float>(textureWidth), static_cast<float>(textureHeight)),
                        ImVec2(0.0f, 0.0f),
                        ImVec2(1.0f, 1.0f),
                        -1,
                        ImVec4(0.6f, 0.6f, 0.6f, 1.0f)
                    ))
                {
                    event::EventManager::eventDispatcher.dispatch(
                        event::MdciiEventType::BAUGFX_SELECTED,
                        event::BauGfxSelectedEvent({ std::stoi(k), map::Rotation::DEG0, v })
                    );
                }

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }
}

void mdcii::EditorGui::WorkshopGui(event::SelectedBauGfx& t_selectedBauGfx) const
{
    if (!t_selectedBauGfx.HasBuilding())
    {
        return;
    }

    ImGui::TextUnformatted(t_selectedBauGfx.name.c_str());

    std::string rotStr{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "CurrentBuildingRotation") };
    rotStr.append(std::string(": %s"));

    ImGui::Text(rotStr.c_str(), rotation_to_string(t_selectedBauGfx.rotation));

    ImGui::Separator();

    const auto& building{ m_map->context->originalResourcesManager->GetBuildingById(t_selectedBauGfx.buildingId) };
    const auto& bauhausBshTextures{ m_map->context->originalResourcesManager->GetBauhausBshByZoom(m_map->mapContent->bauhausZoom) };

    const auto textureWidth{ bauhausBshTextures.at(building.baugfx)->width };
    const auto textureHeight{ bauhausBshTextures.at(building.baugfx)->height };
    const auto textureId{ reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(
        bauhausBshTextures.at(static_cast<size_t>(building.baugfx) + rotation_to_int(t_selectedBauGfx.rotation))->textureId
    )
    ) };

    if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "RotateBuildingRight").c_str()))
    {
        ++t_selectedBauGfx.rotation;
    }

    ImGui::SameLine();

    ImGui::Image(
        textureId,
        ImVec2(static_cast<float>(textureWidth), static_cast<float>(textureHeight)),
        ImVec2(0.0f, 0.0f),
        ImVec2(1.0f, 1.0f),
        ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        ImVec4(0.6f, 0.6f, 0.6f, 1.0f)
    );

    ImGui::SameLine();

    if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "RotateBuildingLeft").c_str()))
    {
        --t_selectedBauGfx.rotation;
    }
}

void mdcii::EditorGui::CurrentSelectedMapTileGui(const map::MapTile& t_mapTile) const
{
    if (!t_mapTile.HasBuilding())
    {
        return;
    }

    t_mapTile.RenderImGui(false);
}

void mdcii::EditorGui::SaveGameGui()
{
    const auto fileName{ Game::RESOURCES_REL_PATH + Game::INI.Get<std::string>("content", "save_map") };

    //static char str[128] = "";
    //ImGui::InputTextWithHint("input filename", "enter filename here", str, IM_ARRAYSIZE(str));

    if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "SaveGame").c_str()))
    {
        Log::MDCII_LOG_DEBUG("[EditorGui::SaveGameGui()] Start saving the game...");

        nlohmann::json j;
        nlohmann::json t = nlohmann::json::object();
        nlohmann::json b = nlohmann::json::object();
        std::ofstream file{ fileName };
        if (!file)
        {
            throw MDCII_EXCEPTION("[EditorGui::SaveGameGui()] Error while opening file " + fileName + ".");
        }

        j["width"] = m_map->mapContent->width;
        j["height"] = m_map->mapContent->height;
        j["layers"] = nlohmann::json::array();
        t["terrain"] = m_map->mapContent->GetLayer(map::LayerType::TERRAIN).mapTiles;
        b["buildings"] = m_map->mapContent->GetLayer(map::LayerType::BUILDINGS).mapTiles;
        j["layers"].push_back(t);
        j["layers"].push_back(b);

        file << j;

        m_info = true;

        Log::MDCII_LOG_DEBUG("[EditorGui::SaveGameGui()] The game has been successfully saved.");
    }

    if (m_info)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(0, 255, 0)));

        auto success{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "SaveGameSuccess") };
        success.append(" %s.");

        ImGui::Text(success.c_str(), fileName.c_str());
        ImGui::PopStyleColor();
    }
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::EditorGui::ToggleButton(const char* t_str, bool* t_v)
{
    const auto p{ ImGui::GetCursorScreenPos() };
    auto* drawList{ ImGui::GetWindowDrawList() };

    const auto height{ ImGui::GetFrameHeight() };
    const auto width{ height * 2.0f };
    const auto radius{ height * 0.5f };

    if (ImGui::InvisibleButton(t_str, ImVec2(width, height)))
    {
        *t_v = !*t_v;
    }

    constexpr auto offset{ 20 };

    // enable
    constexpr auto r{ 153 };
    constexpr auto g{ 61 };
    constexpr auto b{ 61 };

    // disable
    constexpr auto rd{ 145 };
    constexpr auto gd{ 211 };
    constexpr auto bd{ 68 };

    ImU32 colBg;
    if (ImGui::IsItemHovered())
    {
        colBg = *t_v ? IM_COL32(r + offset, g, b + offset, 255) : IM_COL32(rd - offset, gd - offset, bd - offset, 255);
    }
    else
    {
        colBg = *t_v ? IM_COL32(r, g, b, 255) : IM_COL32(rd, gd, bd, 255);
    }

    drawList->AddRectFilled(p, ImVec2(p.x + width, p.y + height), colBg, height * 0.5f);
    drawList->AddCircleFilled(ImVec2(*t_v ? (p.x + width - radius) : (p.x + radius), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
}
