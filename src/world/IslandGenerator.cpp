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

#include <imgui.h>
#include <random>
#include "IslandGenerator.h"
#include "MdciiAssert.h"
#include "Game.h"
#include "data/Text.h"
#include "data/BuildingIds.h"
#include "layer/TerrainLayer.h"
#include "file/IslandFile.h"
#include "vendor/fastnoise/FastNoiseLite.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::IslandGenerator::IslandGenerator()
{
    Log::MDCII_LOG_DEBUG("[IslandGenerator::IslandGenerator()] Create IslandGenerator.");
}

mdcii::world::IslandGenerator::~IslandGenerator() noexcept
{
    Log::MDCII_LOG_DEBUG("[IslandGenerator::~IslandGenerator()] Destruct IslandGenerator.");
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::world::IslandGenerator::RenderImGui()
{
    static auto seed{ 5990 };
    static auto frequency{ 0.116f };

    if (ImGui::Checkbox("South", &m_south))
    {
        Reset();
    }

    if (ImGui::SliderInt("Island width", &m_width, 20, 60))
    {
        Reset();
    }

    if (ImGui::SliderInt("Island height", &m_height, 20, 60))
    {
        Reset();
    }

    if (ImGui::SliderInt("Seed", &seed, 100, 9000))
    {
        Reset();
    }

    if (ImGui::SliderFloat("Frequency", &frequency, 0.01f, 0.3f))
    {
        Reset();
    }

    if (ImGui::SliderFloat("Water level", &m_water_level, 0.5f, 0.8f))
    {
        Reset();
    }

    ImGui::Separator();

    if (ImGui::Button("Noise"))
    {
        CalcMapTypes(seed, frequency, m_width, m_height);
        CalcBitmaskValues();
        m_noise = true;
    }

    if (m_noise)
    {
        ImGui::Separator();
        RenderEditMenuImGui();
        ImGui::Separator();
    }

    RenderMapTypesImGui();
    RenderBitmaskValuesImGui();
    RenderBitmaskValuesAsCharsImGui();
    SaveIslandImGui();
}

//-------------------------------------------------
// Noise
//-------------------------------------------------

void mdcii::world::IslandGenerator::CalcMapTypes(const int32_t t_seed, const float t_frequency, const int32_t t_width, const int32_t t_height)
{
    Log::MDCII_LOG_DEBUG("[IslandGenerator::CalcMapTypes()] Create elevations for a 2D map using Perlin Noise.");

    FastNoiseLite fn;
    fn.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    fn.SetSeed(t_seed);
    fn.SetFrequency(t_frequency);

    std::vector<double> elevations;
    for (auto y{ 0 }; y < t_height; ++y)
    {
        for (auto x{ 0 }; x < t_width; ++x)
        {
            const auto nx{ 2.0 * x / t_width - 1.0 };
            const auto ny{ 2.0 * y / t_height - 1.0 };

            auto d{ 1 - (1 - (nx * nx)) * (1 - (ny * ny)) };
            if (d < 0)
            {
                d = 0;
            }
            if (d > 1)
            {
                d = 1;
            }

            const auto e{ static_cast<double>(fn.GetNoise(static_cast<double>(x), static_cast<double>(y))) / 2.0 + 0.5 };

            const auto lerp = [](const auto t_x, const auto t_y, const auto t_t) {
                return t_x * (1.0 - t_t) + t_y * t_t;
            };

            elevations.push_back(lerp(e, 1.0 - d, static_cast<double>(m_water_level)));
        }
    }

    Log::MDCII_LOG_DEBUG("[IslandGenerator::CalcMapTypes()] Split elevation values into water and terrain.");

    for (const auto elevation : elevations)
    {
        elevation > static_cast<double>(m_water_level)
            ? m_map.push_back(MapType::TERRAIN)
            : m_map.push_back(MapType::WATER);
    }
}


void mdcii::world::IslandGenerator::CalcBitmaskValues()
{
    Log::MDCII_LOG_DEBUG("[IslandGenerator::CalcBitmaskValues()] Calc bitmask values.");

    m_bitmaskValues.resize(static_cast<size_t>(m_width) * m_height);
    std::fill(m_bitmaskValues.begin(), m_bitmaskValues.end(), 0);

    AddMapType(MapType::EMBANKMENT);
    AddMapType(MapType::COAST);
    AddMapType(MapType::SHALLOW_WATER);
    AddMapType(MapType::MEDIUM_WATER);
}

//-------------------------------------------------
// Bitmasking
//-------------------------------------------------

int32_t mdcii::world::IslandGenerator::GetNorthValue(const int32_t t_x, const int32_t t_y) const
{
    if (t_y - 1 < 0)
    {
        return static_cast<int>(NORTH_FLAG);
    }

    if (m_map.at(GetIndex(t_x, t_y - 1, m_width)) == MapType::WATER)
    {
        return static_cast<int>(NORTH_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetEastValue(const int32_t t_x, const int32_t t_y) const
{
    if (t_x + 1 >= m_width)
    {
        return static_cast<int>(EAST_FLAG);
    }

    if (m_map.at(GetIndex(t_x + 1, t_y, m_width)) == MapType::WATER)
    {
        return static_cast<int>(EAST_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetSouthValue(const int32_t t_x, const int32_t t_y) const
{
    if (t_y + 1 >= m_height)
    {
        return static_cast<int>(SOUTH_FLAG);
    }

    if (m_map.at(GetIndex(t_x, t_y + 1, m_width)) == MapType::WATER)
    {
        return static_cast<int>(SOUTH_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetWestValue(const int32_t t_x, const int32_t t_y) const
{
    if (t_x - 1 < 0)
    {
        return static_cast<int>(WEST_FLAG);
    }

    if (m_map.at(GetIndex(t_x - 1, t_y, m_width)) == MapType::WATER)
    {
        return static_cast<int>(WEST_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetNorthWestValue(const int32_t t_x, const int32_t t_y) const
{
    if (t_x - 1 < 0 || t_y - 1 < 0)
    {
        return static_cast<int>(NORTH_WEST_FLAG);
    }

    if (m_map.at(GetIndex(t_x - 1, t_y - 1, m_width)) == MapType::WATER)
    {
        return static_cast<int>(NORTH_WEST_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetNorthEastValue(const int32_t t_x, const int32_t t_y) const
{
    if (t_x + 1 >= m_width || t_y - 1 < 0)
    {
        return static_cast<int>(NORTH_EAST_FLAG);
    }

    if (m_map.at(GetIndex(t_x + 1, t_y - 1, m_width)) == MapType::WATER)
    {
        return static_cast<int>(NORTH_EAST_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetSouthWestValue(const int32_t t_x, const int32_t t_y) const
{
    if (t_x - 1 < 0 || t_y + 1 >= m_height)
    {
        return static_cast<int>(SOUTH_WEST_FLAG);
    }

    if (m_map.at(GetIndex(t_x - 1, t_y + 1, m_width)) == MapType::WATER)
    {
        return static_cast<int>(SOUTH_WEST_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetSouthEastValue(const int32_t t_x, const int32_t t_y) const
{
    if (t_x + 1 >= m_width || t_y + 1 >= m_height)
    {
        return static_cast<int>(SOUTH_EAST_FLAG);
    }

    if (m_map.at(GetIndex(t_x + 1, t_y + 1, m_width)) == MapType::WATER)
    {
        return static_cast<int>(SOUTH_EAST_FLAG);
    }

    return 0;
}

//-------------------------------------------------
// ImGui
//-------------------------------------------------

void mdcii::world::IslandGenerator::RenderMapTypesImGui() const
{
    ImGui::Begin("Map types");
    if (m_map.empty())
    {
        ImGui::Text("No values available.");
    }
    else
    {
        for (auto y{ 0 }; y < m_height; ++y)
        {
            for (auto x{ 0 }; x < m_width; ++x)
            {
                const auto& mapType{ m_map.at(GetIndex(x, y, m_width)) };
                switch (mapType)
                {
                case MapType::TERRAIN:
                    ImGui::PushStyleColor(ImGuiCol_Text, TERRAIN_COL);
                    break;
                case MapType::EMBANKMENT:
                    ImGui::PushStyleColor(ImGuiCol_Text, EMBANKMENT_COL);
                    break;
                case MapType::COAST:
                    ImGui::PushStyleColor(ImGuiCol_Text, COAST_COL);
                    break;
                case MapType::SHALLOW_WATER:
                    ImGui::PushStyleColor(ImGuiCol_Text, SHALLOW_WATER_COL);
                    break;
                case MapType::MEDIUM_WATER:
                    ImGui::PushStyleColor(ImGuiCol_Text, MEDIUM_WATER_COL);
                    break;
                case MapType::WATER:
                    ImGui::PushStyleColor(ImGuiCol_Text, WATER_COL);
                    break;
                default:
                    ImGui::PushStyleColor(ImGuiCol_Text, INVALID_COL);
                }

                ImGui::Text("%i", magic_enum::enum_integer(mapType));
                ImGui::SameLine();
                ImGui::PopStyleColor();
            }

            ImGui::Text("");
        }
    }

    ImGui::End();
}

void mdcii::world::IslandGenerator::RenderBitmaskValuesImGui() const
{
    ImGui::Begin("Bitmask values");
    if (m_bitmaskValues.empty())
    {
        ImGui::Text("No values available.");
    }
    else
    {
        for (auto y{ 0 }; y < m_height; ++y)
        {
            for (auto x{ 0 }; x < m_width; ++x)
            {
                const auto idx{ GetIndex(x, y, m_width) };
                const auto bitmask{ m_bitmaskValues.at(idx) };

                ImGui::PushStyleColor(ImGuiCol_Text, Bitmask2MapTypeCol(bitmask).color);
                ImGui::Text("%04i", bitmask);
                ImGui::SameLine();
                ImGui::PopStyleColor();
            }

            ImGui::Text("");
        }
    }

    ImGui::End();
}

void mdcii::world::IslandGenerator::RenderBitmaskValuesAsCharsImGui()
{
    ImFontAtlas* atlas = ImGui::GetIO().Fonts;
    ImGui::PushFont(atlas->Fonts[1]);

    ImGui::Begin("Resolved bitmask values");
    if (m_bitmaskValues.empty())
    {
        ImGui::Text("No values available.");
    }
    else
    {
        for (auto y{ 0 }; y < m_height; ++y)
        {
            for (auto x{ 0 }; x < m_width; ++x)
            {
                const auto idx{ GetIndex(x, y, m_width) };
                const auto bitmask{ m_bitmaskValues.at(idx) };

                switch (m_map.at(idx))
                {
                case MapType::TERRAIN:
                    RenderCharButtonImGui(MapType::TERRAIN, m_tileTypeChars.at(TileType::TERRAIN), idx, [this, idx]() {
                        UpdateBySelection(idx);
                    });
                    break;
                case MapType::EMBANKMENT:
                    if (IsTileTypeKnown(MapType::EMBANKMENT, bitmask))
                    {
                        RenderCharButtonImGui(MapType::EMBANKMENT, m_tileTypeChars.at(GetTileType(MapType::EMBANKMENT, bitmask)), idx, [this, idx]() {
                            UpdateBySelection(idx);
                        });
                    }
                    else
                    {
                        RenderCharButtonImGui(MapType::INVALID, "?", idx, [this, idx]() {
                            UpdateBySelection(idx);
                        });
                    }
                    break;
                case MapType::COAST:
                    if (IsTileTypeKnown(MapType::COAST, bitmask))
                    {
                        RenderCharButtonImGui(MapType::COAST, m_tileTypeChars.at(GetTileType(MapType::COAST, bitmask)), idx, [this, idx]() {
                            UpdateBySelection(idx);
                        });
                    }
                    else
                    {
                        RenderCharButtonImGui(MapType::INVALID, "?", idx, [this, idx]() {
                            UpdateBySelection(idx);
                        });
                    }
                    break;
                case MapType::SHALLOW_WATER:
                    if (IsTileTypeKnown(MapType::SHALLOW_WATER, bitmask))
                    {
                        RenderCharButtonImGui(MapType::SHALLOW_WATER, m_tileTypeChars.at(GetTileType(MapType::SHALLOW_WATER, bitmask)), idx, [this, idx]() {
                            UpdateBySelection(idx);
                        });
                    }
                    else
                    {
                        RenderCharButtonImGui(MapType::SHALLOW_WATER, m_tileTypeChars.at(TileType::SHALLOW), idx, [this, idx]() {
                            UpdateBySelection(idx);
                        });
                    }
                    break;
                case MapType::MEDIUM_WATER:
                    if (IsTileTypeKnown(MapType::MEDIUM_WATER, bitmask))
                    {
                        RenderCharButtonImGui(MapType::MEDIUM_WATER, m_tileTypeChars.at(GetTileType(MapType::MEDIUM_WATER, bitmask)), idx, [this, idx]() {
                            UpdateBySelection(idx);
                        });
                    }
                    else
                    {
                        RenderCharButtonImGui(MapType::MEDIUM_WATER, m_tileTypeChars.at(TileType::MEDIUM), idx, [this, idx]() {
                            UpdateBySelection(idx);
                        });
                    }
                    break;
                case MapType::WATER:
                    RenderCharButtonImGui(MapType::WATER, m_tileTypeChars.at(TileType::WATER), idx, [this, idx]() {
                        UpdateBySelection(idx);
                    });
                    break;
                default:
                    RenderCharButtonImGui(MapType::INVALID, "?", idx, [this, idx]() {
                        UpdateBySelection(idx);
                    });
                }
            }

            ImGui::Text("");
        }
    }

    ImGui::PopFont();
    ImGui::End();
}

void mdcii::world::IslandGenerator::RenderCharButtonImGui(const MapType t_mapType, std::string t_char, const int32_t t_index, const std::function<void()>& t_func)
{
    switch (t_mapType)
    {
    case MapType::TERRAIN:
        ImGui::PushStyleColor(ImGuiCol_Text, TERRAIN_COL);
        ImGui::PushStyleColor(ImGuiCol_Button, TERRAIN_BUTTON_COL);
        break;
    case MapType::EMBANKMENT:
        ImGui::PushStyleColor(ImGuiCol_Text, EMBANKMENT_COL);
        ImGui::PushStyleColor(ImGuiCol_Button, EMBANKMENT_BUTTON_COL);
        break;
    case MapType::COAST:
        ImGui::PushStyleColor(ImGuiCol_Text, COAST_COL);
        ImGui::PushStyleColor(ImGuiCol_Button, COAST_BUTTON_COL);
        break;
    case MapType::SHALLOW_WATER:
        ImGui::PushStyleColor(ImGuiCol_Text, SHALLOW_WATER_COL);
        ImGui::PushStyleColor(ImGuiCol_Button, SHALLOW_WATER_BUTTON_COL);
        break;
    case MapType::MEDIUM_WATER:
        ImGui::PushStyleColor(ImGuiCol_Text, MEDIUM_WATER_COL);
        ImGui::PushStyleColor(ImGuiCol_Button, MEDIUM_WATER_BUTTON_COL);
        break;
    case MapType::WATER:
        ImGui::PushStyleColor(ImGuiCol_Text, WATER_COL);
        ImGui::PushStyleColor(ImGuiCol_Button, WATER_BUTTON_COL);
        break;
    default:
        ImGui::PushStyleColor(ImGuiCol_Text, INVALID_COL);
        ImGui::PushStyleColor(ImGuiCol_Button, INVALID_BUTTON_COL);
    }

    if (auto id{ t_char.append("##").append(std::to_string(t_index)) }; ImGui::Button(id.c_str()))
    {
        t_func();
    }

    ImGui::SameLine();
    ImGui::PopStyleColor(2);
}

void mdcii::world::IslandGenerator::RenderEditMenuImGui()
{
    if (ImGui::BeginCombo("Set map type", MAP_TYPES_STRINGS.at(magic_enum::enum_integer(m_selMapType))))
    {
        for (auto i{ 0u }; i < MAP_TYPES_STRINGS.size(); ++i)
        {
            const auto mapTypeOpt{ magic_enum::enum_cast<MapType>(i) };
            auto mapType{ MapType::WATER };
            if (mapTypeOpt.has_value())
            {
                mapType = mapTypeOpt.value();
            }

            const bool isSelected = (m_selMapType == mapType);
            if (ImGui::Selectable(MAP_TYPES_STRINGS[i], isSelected))
            {
                m_selMapType = mapType;
            }

            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    ImFontAtlas* atlas = ImGui::GetIO().Fonts;
    ImGui::PushFont(atlas->Fonts[1]);

    for (const auto [tileType, tileChar] : m_tileTypeChars)
    {
        if (m_bitmaskTileTypes.count(m_selMapType))
        {
            auto b{ false };
            for (auto it{ m_bitmaskTileTypes.at(m_selMapType).begin() }; it != m_bitmaskTileTypes.at(m_selMapType).end(); ++it)
            {
                if (it->second == tileType)
                {
                    b = true;
                }
            }

            if (b)
            {
                const auto id{ std::string(tileChar).append(" ").append(magic_enum::enum_name(tileType)).append("##").append(magic_enum::enum_name(m_selMapType)) };
                if (ImGui::RadioButton(id.c_str(), m_sel_tile_type_button == tileType))
                {
                    m_sel_tile_type_button = tileType;
                }
            }
            else if (m_selMapType == MapType::MEDIUM_WATER && tileType == TileType::MEDIUM)
            {
                const auto id{ std::string(tileChar).append(" ").append(magic_enum::enum_name(TileType::MEDIUM)).append("##").append(magic_enum::enum_name(MapType::MEDIUM_WATER)) };
                if (ImGui::RadioButton(id.c_str(), m_sel_tile_type_button == TileType::MEDIUM))
                {
                    m_sel_tile_type_button = TileType::MEDIUM;
                }
            }
            else if (m_selMapType == MapType::SHALLOW_WATER && tileType == TileType::SHALLOW)
            {
                const auto id{ std::string(tileChar).append(" ").append(magic_enum::enum_name(TileType::SHALLOW)).append("##").append(magic_enum::enum_name(MapType::SHALLOW_WATER)) };
                if (ImGui::RadioButton(id.c_str(), m_sel_tile_type_button == TileType::SHALLOW))
                {
                    m_sel_tile_type_button = TileType::SHALLOW;
                }
            }
        }
        else if (m_selMapType == MapType::WATER && tileType == TileType::WATER)
        {
            m_sel_tile_type_button = TileType::WATER;
        }
        else if (m_selMapType == MapType::TERRAIN && tileType == TileType::TERRAIN)
        {
            m_sel_tile_type_button = TileType::TERRAIN;
        }
    }

    ImGui::PopFont();
}

void mdcii::world::IslandGenerator::SaveIslandImGui()
{
    if (m_map.empty() || m_bitmaskValues.empty())
    {
        return;
    }

    static bool error{ false };
    static bool saved{ false };
    static std::string fileName;
    save_file_button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "SaveGame").c_str(), &fileName);

    if (!fileName.empty())
    {
        if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Save").c_str()))
        {
            std::vector<std::shared_ptr<layer::Tile>> coastTiles;
            std::vector<std::shared_ptr<layer::Tile>> terrainTiles;
            CreateTerrainTiles(terrainTiles);
            CreateCoastTiles(coastTiles);

            file::IslandFile islandFile{ fileName };
            islandFile.SetData(m_width, m_height, terrainTiles, coastTiles);
            if (islandFile.SaveJsonToFile())
            {
                saved = true;
                fileName.clear();
            }
            else
            {
                error = true;
                saved = false;
                fileName.clear();
            }
        }
    }

    if (error)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
        ImGui::TextUnformatted(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "SaveError").c_str());
        ImGui::PopStyleColor();
    }

    if (saved)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
        ImGui::TextUnformatted(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "SaveSuccess").c_str());
        ImGui::PopStyleColor();
    }
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

int32_t mdcii::world::IslandGenerator::GetIndex(const int32_t t_x, const int32_t t_y, const int32_t t_width)
{
    MDCII_ASSERT(t_x >= 0, "[IslandGenerator::GetIndex()] Invalid x position.")
    MDCII_ASSERT(t_y >= 0, "[IslandGenerator::GetIndex()] Invalid y position.")
    MDCII_ASSERT(t_width >= 3, "[IslandGenerator::GetIndex()] Invalid width.")

    return t_y * t_width + t_x;
}

void mdcii::world::IslandGenerator::Reset()
{
    Log::MDCII_LOG_DEBUG("[IslandGenerator::Reset()] Reset all values.");

    m_noise = false;

    std::vector<MapType>().swap(m_map);
    std::vector<int32_t>().swap(m_bitmaskValues);
}

mdcii::world::IslandGenerator::MapTypeCol mdcii::world::IslandGenerator::Bitmask2MapTypeCol(const int32_t t_bitmask)
{
    if (t_bitmask == TERRAIN_FLAG)
    {
        return { MapType::TERRAIN, TERRAIN_COL };
    }
    else if (t_bitmask > TERRAIN_FLAG && t_bitmask < EMBANKMENT_FLAG)
    {
        return { MapType::TERRAIN, TERRAIN_BORDER_COL };
    }
    else if (t_bitmask >= EMBANKMENT_FLAG && t_bitmask < COAST_FLAG - 1)
    {
        return { MapType::EMBANKMENT, EMBANKMENT_COL };
    }
    else if (t_bitmask >= COAST_FLAG && t_bitmask < SHALLOW_FLAG - 1)
    {
        return { MapType::COAST, COAST_COL };
    }
    else if (t_bitmask >= SHALLOW_FLAG && t_bitmask < MEDIUM_FLAG - 1)
    {
        return { MapType::SHALLOW_WATER, SHALLOW_WATER_COL };
    }
    else if (t_bitmask >= MEDIUM_FLAG && t_bitmask < WATER_FLAG)
    {
        return { MapType::MEDIUM_WATER, MEDIUM_WATER_COL };
    }
    else if (t_bitmask == WATER_FLAG)
    {
        return { MapType::WATER, WATER_COL };
    }

    return { MapType::INVALID, INVALID_COL };
}

void mdcii::world::IslandGenerator::AddMapType(const MapType t_mapType)
{
    MDCII_ASSERT(t_mapType == MapType::MEDIUM_WATER || t_mapType == MapType::SHALLOW_WATER ||t_mapType == MapType::COAST || t_mapType == MapType::EMBANKMENT, "[IslandGenerator::AddMapType()] Invalid map type.")

    Log::MDCII_LOG_DEBUG("[IslandGenerator::AddMapType()] Add bitmask values for map type {}.", magic_enum::enum_name(t_mapType));

    for (auto y{ 0 }; y < m_height; ++y)
    {
        for (auto x{ 0 }; x < m_width; ++x)
        {
            const auto idx{ GetIndex(x, y, m_width) };
            auto result{ static_cast<int>(TERRAIN_FLAG) };

            if (m_map.at(idx) == MapType::WATER)
            {
                switch (t_mapType)
                {
                case MapType::MEDIUM_WATER:
                    result = static_cast<int>(MEDIUM_FLAG);
                    break;
                case MapType::SHALLOW_WATER:
                    result = static_cast<int>(SHALLOW_FLAG);
                    break;
                case MapType::COAST:
                    result = static_cast<int>(COAST_FLAG);
                    break;
                case MapType::EMBANKMENT:
                    result = static_cast<int>(EMBANKMENT_FLAG);
                    break;
                default:;
                }
            }

            if (m_map.at(idx) == MapType::EMBANKMENT)
            {
                result = static_cast<int>(EMBANKMENT_FLAG);
            }

            if (m_map.at(idx) == MapType::COAST)
            {
                result = static_cast<int>(COAST_FLAG);
            }

            if (m_map.at(idx) == MapType::SHALLOW_WATER)
            {
                result = static_cast<int>(SHALLOW_FLAG);
            }

            result += GetNorthWestValue(x, y);
            result += GetNorthValue(x, y);
            result += GetNorthEastValue(x, y);

            result += GetWestValue(x, y);
            result += GetEastValue(x, y);

            result += GetSouthWestValue(x, y);
            result += GetSouthValue(x, y);
            result += GetSouthEastValue(x, y);

            switch (t_mapType)
            {
            case MapType::EMBANKMENT:
                m_bitmaskValues.at(idx) = result;
                break;
            case MapType::COAST:
                if (m_map.at(idx) != MapType::TERRAIN && m_map.at(idx) != MapType::EMBANKMENT)
                {
                    m_bitmaskValues.at(idx) = result;
                }
                break;
            case MapType::SHALLOW_WATER:
                if (m_map.at(idx) != MapType::TERRAIN && m_map.at(idx) != MapType::EMBANKMENT && m_map.at(idx) != MapType::COAST)
                {
                    m_bitmaskValues.at(idx) = result;
                }
                break;
            case MapType::MEDIUM_WATER:
                if (m_map.at(idx) != MapType::TERRAIN && m_map.at(idx) != MapType::EMBANKMENT && m_map.at(idx) != MapType::COAST && m_map.at(idx) != MapType::SHALLOW_WATER)
                {
                    m_bitmaskValues.at(idx) = result;
                }
                break;
            default:;
            }
        }
    }

    auto i{ 0 };
    for (const auto bitmask : m_bitmaskValues)
    {
        if (const auto mapTypeCol{ Bitmask2MapTypeCol(bitmask) }; mapTypeCol.mapType == t_mapType)
        {
            m_map.at(i) = t_mapType;
        }

        i++;
    }
}

bool mdcii::world::IslandGenerator::IsTileTypeKnown(const MapType t_mapType, const int32_t t_bitmask) const
{
    if (const auto& tileTypes{ m_bitmaskTileTypes.at(t_mapType) }; tileTypes.count(t_bitmask))
    {
        return true;
    }

    return false;
}

mdcii::world::IslandGenerator::TileType mdcii::world::IslandGenerator::GetTileType(const MapType t_mapType, const int32_t t_bitmask) const
{
    MDCII_ASSERT(IsTileTypeKnown(t_mapType, t_bitmask), "[IslandGenerator::GetTileType()] Unknown tile type.")

    return m_bitmaskTileTypes.at(t_mapType).at(t_bitmask);
}

void mdcii::world::IslandGenerator::UpdateBySelection(const int32_t t_index)
{
    m_map.at(t_index) = m_selMapType;

    if (m_selMapType == MapType::WATER)
    {
        m_bitmaskValues.at(t_index) = WATER_FLAG;
    }
    else if (m_selMapType == MapType::TERRAIN)
    {
        m_bitmaskValues.at(t_index) = TERRAIN_FLAG;
    }
    else if (m_selMapType == MapType::MEDIUM_WATER)
    {
        m_bitmaskValues.at(t_index) = MEDIUM_FLAG;
    }
    else if (m_selMapType == MapType::SHALLOW_WATER)
    {
        m_bitmaskValues.at(t_index) = SHALLOW_FLAG;
    }
    else
    {
        for (auto it{ m_bitmaskTileTypes.at(m_selMapType).begin() }; it != m_bitmaskTileTypes.at(m_selMapType).end(); ++it)
        {
            if (it->second == m_sel_tile_type_button)
            {
                m_bitmaskValues.at(t_index) = it->first;
            }
        }
    }
}

//-------------------------------------------------
// Tiles
//-------------------------------------------------

void mdcii::world::IslandGenerator::CreateTerrainTiles(std::vector<std::shared_ptr<layer::Tile>>& t_terrainTiles) const
{
    Log::MDCII_LOG_DEBUG("[IslandGenerator::CreateTerrainTiles()] Create terrain tiles.");

    for (auto y{ 0 }; y < m_height; ++y)
    {
        for (auto x{ 0 }; x < m_width; ++x)
        {
            t_terrainTiles.emplace_back(std::make_unique<layer::Tile>());
        }
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution randForTrees(0, 10);
    std::uniform_int_distribution randForEmbankment(0, 9);
    std::uniform_int_distribution randForEmbankmentCorner(0, 2);
    std::uniform_int_distribution randForEmbankmentCornerInside(0, 3);

    for (auto y{ 0 }; y < m_height; ++y)
    {
        for (auto x{ 0 }; x < m_width; ++x)
        {
            const auto idx{ GetIndex(x, y, m_width) };
            const auto bitmask{ m_bitmaskValues.at(idx) };

            if (bitmask >= 0 && bitmask <= 255) // terrain
            {
                if (m_south)
                {
                    if (const auto r{ randForTrees(gen) }; r >= 7)
                    {
                        t_terrainTiles.at(idx) = CreateTile(data::SOUTH_TREES_BUILDING_IDS.at(randForTrees(gen)), x, y, Rotation::DEG0);
                    }
                    else
                    {
                        t_terrainTiles.at(idx) = CreateTile(data::GRASS_BUILDING_ID, x, y, Rotation::DEG0);
                    }
                }
                else
                {
                    if (const auto r{ randForTrees(gen) }; r >= 7)
                    {
                        t_terrainTiles.at(idx) = CreateTile(data::NORTH_TREES_BUILDING_IDS.at(randForTrees(gen)), x, y, Rotation::DEG0);
                    }
                    else
                    {
                        t_terrainTiles.at(idx) = CreateTile(data::GRASS_BUILDING_ID, x, y, Rotation::DEG0);
                    }
                }
            }
            else if (bitmask > 255 && bitmask <= 510) // embankment
            {
                if (m_bitmaskTileTypes.at(MapType::EMBANKMENT).count(bitmask))
                {
                    switch (m_bitmaskTileTypes.at(MapType::EMBANKMENT).at(bitmask))
                    {
                    case TileType::TOP:
                        t_terrainTiles.at(idx) = CreateTile(data::EMBANKMENT_BUILDING_IDS.at(randForEmbankment(gen)), x, y, Rotation::DEG180);
                        break;
                    case TileType::BOTTOM:
                        t_terrainTiles.at(idx) = CreateTile(data::EMBANKMENT_BUILDING_IDS.at(randForEmbankment(gen)), x, y, Rotation::DEG0);
                        break;
                    case TileType::LEFT:
                        t_terrainTiles.at(idx) = CreateTile(data::EMBANKMENT_BUILDING_IDS.at(randForEmbankment(gen)), x, y, Rotation::DEG90);
                        break;
                    case TileType::RIGHT:
                        t_terrainTiles.at(idx) = CreateTile(data::EMBANKMENT_BUILDING_IDS.at(randForEmbankment(gen)), x, y, Rotation::DEG270);
                        break;

                    case TileType::CORNER_OUT_TL:
                        t_terrainTiles.at(idx) = CreateTile(data::EMBANKMENT_CORNER_BUILDING_IDS.at(randForEmbankmentCorner(gen)), x, y, Rotation::DEG90);
                        break;
                    case TileType::CORNER_OUT_TR:
                        t_terrainTiles.at(idx) = CreateTile(data::EMBANKMENT_CORNER_BUILDING_IDS.at(randForEmbankmentCorner(gen)), x, y, Rotation::DEG180);
                        break;
                    case TileType::CORNER_OUT_BL:
                        t_terrainTiles.at(idx) = CreateTile(data::EMBANKMENT_CORNER_BUILDING_IDS.at(randForEmbankmentCorner(gen)), x, y, Rotation::DEG0);
                        break;
                    case TileType::CORNER_OUT_BR:
                        t_terrainTiles.at(idx) = CreateTile(data::EMBANKMENT_CORNER_BUILDING_IDS.at(randForEmbankmentCorner(gen)), x, y, Rotation::DEG270);
                        break;

                    case TileType::CORNER_IN_TL:
                        t_terrainTiles.at(idx) = CreateTile(data::EMBANKMENT_CORNER_INSIDE_BUILDING_IDS.at(randForEmbankmentCornerInside(gen)), x, y, Rotation::DEG270);
                        break;
                    case TileType::CORNER_IN_TR:
                        t_terrainTiles.at(idx) = CreateTile(data::EMBANKMENT_CORNER_INSIDE_BUILDING_IDS.at(randForEmbankmentCornerInside(gen)), x, y, Rotation::DEG0);
                        break;
                    case TileType::CORNER_IN_BL:
                        t_terrainTiles.at(idx) = CreateTile(data::EMBANKMENT_CORNER_INSIDE_BUILDING_IDS.at(randForEmbankmentCornerInside(gen)), x, y, Rotation::DEG180);
                        break;
                    case TileType::CORNER_IN_BR:
                        t_terrainTiles.at(idx) = CreateTile(data::EMBANKMENT_CORNER_INSIDE_BUILDING_IDS.at(randForEmbankmentCornerInside(gen)), x, y, Rotation::DEG90);
                        break;

                    default:
                        t_terrainTiles.at(idx) = CreateTile(data::GRASS_BUILDING_ID, x, y, Rotation::DEG0);
                    }
                }
            }
        }
    }
}

void mdcii::world::IslandGenerator::CreateCoastTiles(std::vector<std::shared_ptr<layer::Tile>>& t_coastTiles) const
{
    Log::MDCII_LOG_DEBUG("[IslandGenerator::CreateCoastTiles()] Create coast tiles.");

    for (auto y{ 0 }; y < m_height; ++y)
    {
        for (auto x{ 0 }; x < m_width; ++x)
        {
            t_coastTiles.emplace_back(std::make_unique<layer::Tile>());
        }
    }

    for (auto y{ 0 }; y < m_height; ++y)
    {
        for (auto x{ 0 }; x < m_width; ++x)
        {
            const auto idx{ GetIndex(x, y, m_width) };
            const auto bitmask{ m_bitmaskValues.at(idx) };

            if (m_map.at(idx) == MapType::COAST)
            {
                const auto& bitmaskTileTypes{ m_bitmaskTileTypes.at(MapType::COAST) };
                if (bitmaskTileTypes.count(bitmask))
                {
                    switch (bitmaskTileTypes.at(bitmask))
                    {
                    case TileType::TOP:
                        t_coastTiles.at(idx) = CreateTile(data::BEACH_BUILDING_ID, x, y, Rotation::DEG270);
                        break;
                    case TileType::BOTTOM:
                        t_coastTiles.at(idx) = CreateTile(data::BEACH_BUILDING_ID, x, y, Rotation::DEG90);
                        break;
                    case TileType::LEFT:
                        t_coastTiles.at(idx) = CreateTile(data::BEACH_BUILDING_ID, x, y, Rotation::DEG180);
                        break;
                    case TileType::RIGHT:
                        t_coastTiles.at(idx) = CreateTile(data::BEACH_BUILDING_ID, x, y, Rotation::DEG0);
                        break;

                    case TileType::CORNER_OUT_TL:
                        t_coastTiles.at(idx) = CreateTile(data::BEACH_CORNER_BUILDING_ID, x, y, Rotation::DEG90);
                        break;
                    case TileType::CORNER_OUT_TR:
                        t_coastTiles.at(idx) = CreateTile(data::BEACH_CORNER_BUILDING_ID, x, y, Rotation::DEG180);
                        break;
                    case TileType::CORNER_OUT_BL:
                        t_coastTiles.at(idx) = CreateTile(data::BEACH_CORNER_BUILDING_ID, x, y, Rotation::DEG0);
                        break;
                    case TileType::CORNER_OUT_BR:
                        t_coastTiles.at(idx) = CreateTile(data::BEACH_CORNER_BUILDING_ID, x, y, Rotation::DEG270);
                        break;

                    case TileType::CORNER_IN_TL:
                        t_coastTiles.at(idx) = CreateTile(data::BEACH_CORNER_INSIDE_BUILDING_ID, x, y, Rotation::DEG90);
                        break;
                    case TileType::CORNER_IN_TR:
                        t_coastTiles.at(idx) = CreateTile(data::BEACH_CORNER_INSIDE_BUILDING_ID, x, y, Rotation::DEG180);
                        break;
                    case TileType::CORNER_IN_BL:
                        t_coastTiles.at(idx) = CreateTile(data::BEACH_CORNER_INSIDE_BUILDING_ID, x, y, Rotation::DEG0);
                        break;
                    case TileType::CORNER_IN_BR:
                        t_coastTiles.at(idx) = CreateTile(data::BEACH_CORNER_INSIDE_BUILDING_ID, x, y, Rotation::DEG270);
                        break;
                    default:
                        t_coastTiles.at(idx) = CreateTile(data::BEACH_BUILDING_ID, x, y, Rotation::DEG0);
                    }
                }
            }
            else if (m_map.at(idx) == MapType::SHALLOW_WATER)
            {
                const auto& bitmaskTileTypes{ m_bitmaskTileTypes.at(MapType::SHALLOW_WATER) };
                if (bitmaskTileTypes.count(bitmask))
                {
                    switch (bitmaskTileTypes.at(bitmask))
                    {
                    case TileType::CORNER_OUT_TL:
                        t_coastTiles.at(idx) = CreateTile(data::SHALLOW_TO_MEDIUM_WATER_BUILDING_ID, x, y, Rotation::DEG270);
                        break;
                    case TileType::CORNER_OUT_TR:
                        t_coastTiles.at(idx) = CreateTile(data::SHALLOW_TO_MEDIUM_WATER_BUILDING_ID, x, y, Rotation::DEG0);
                        break;
                    case TileType::CORNER_OUT_BL:
                        t_coastTiles.at(idx) = CreateTile(data::SHALLOW_TO_MEDIUM_WATER_BUILDING_ID, x, y, Rotation::DEG180);
                        break;
                    case TileType::CORNER_OUT_BR:
                        t_coastTiles.at(idx) = CreateTile(data::SHALLOW_TO_MEDIUM_WATER_BUILDING_ID, x, y, Rotation::DEG90);
                        break;
                    default:;
                    }
                }
                else
                {
                    t_coastTiles.at(idx) = CreateTile(data::SHALLOW_WATER_BUILDING_ID, x, y, Rotation::DEG0);
                }
            }
            else if (m_map.at(idx) == MapType::MEDIUM_WATER)
            {
                const auto& bitmaskTileTypes{ m_bitmaskTileTypes.at(MapType::MEDIUM_WATER) };
                if (bitmaskTileTypes.count(bitmask))
                {
                    switch (bitmaskTileTypes.at(bitmask))
                    {
                    case TileType::CORNER_OUT_TL:
                        t_coastTiles.at(idx) = CreateTile(data::MEDIUM_TO_DEEP_WATER_BUILDING_ID, x, y, Rotation::DEG270);
                        break;
                    case TileType::CORNER_OUT_TR:
                        t_coastTiles.at(idx) = CreateTile(data::MEDIUM_TO_DEEP_WATER_BUILDING_ID, x, y, Rotation::DEG0);
                        break;
                    case TileType::CORNER_OUT_BL:
                        t_coastTiles.at(idx) = CreateTile(data::MEDIUM_TO_DEEP_WATER_BUILDING_ID, x, y, Rotation::DEG180);
                        break;
                    case TileType::CORNER_OUT_BR:
                        t_coastTiles.at(idx) = CreateTile(data::MEDIUM_TO_DEEP_WATER_BUILDING_ID, x, y, Rotation::DEG90);
                        break;
                    default:;
                    }
                }
                else
                {
                    t_coastTiles.at(idx) = CreateTile(data::MEDIUM_WATER_BUILDING_ID, x, y, Rotation::DEG0);
                }
            }
            else
            {
                t_coastTiles.at(idx) = CreateTile(data::DEEP_WATER_BUILDING_ID, x, y, Rotation::DEG0);
            }
        }
    }
}

std::unique_ptr<mdcii::layer::Tile> mdcii::world::IslandGenerator::CreateTile(const int32_t t_id, const int32_t t_worldX, const int32_t t_worldY, const Rotation t_rotation)
{
    auto tile{ std::make_unique<layer::Tile>() };
    tile->buildingId = t_id;
    tile->rotation = t_rotation;
    tile->x = 0;
    tile->y = 0;
    tile->worldXDeg0 = t_worldX;
    tile->worldYDeg0 = t_worldY;

    return tile;
}
