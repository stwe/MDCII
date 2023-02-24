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

#include <random>
#include <imgui.h>
#include "IslandGenerator.h"
#include "FastNoiseLite.h"
#include "MdciiAssert.h"
#include "MdciiUtils.h"
#include "Game.h"
#include "data/Text.h"
#include "layer/TerrainLayer.h"
#include "data/BuildingIds.h"

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
    static int32_t seed{ 4412 };
    static float frequency{ 0.167f };

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

    ImGui::Separator();
    ImGui::Checkbox("Show/hide Map types", &m_render_map_types);
    ImGui::Checkbox("Show/hide bitmask values", &m_render_bitmask_values);
    if (ImGui::Button("Noise"))
    {
        CalcMapTypes(seed, frequency, m_width, m_height);
        CalcBitmaskValues(m_grassToBankMap, m_bitmaskGrassToBank);

        CalcBankToWater();
        CalcBitmaskValues(m_bankToWaterMap, m_bitmaskBankToWater);
    }

    if (m_render_map_types)
    {
        RenderMapTypesImGui(m_grassToBankMap, "Grass2Bank Map types");
        RenderMapTypesImGui(m_bankToWaterMap, "Bank2Water Map types");
    }

    if (m_render_bitmask_values)
    {
        RenderBitmaskValuesImGui(m_bitmaskGrassToBank, "Grass2Bank Bitmask values");
        RenderBitmaskValuesImGui(m_bitmaskBankToWater, "Bank2Water Bitmask values");
    }

    RenderBitmaskValuesAsCharsImGui();
    SaveIslandImGui();

    ImGui::Separator();
    ImFontAtlas* atlas = ImGui::GetIO().Fonts;
    ImGui::PushFont(atlas->Fonts[1]);

    for (const auto [k, v] : m_tileTypeBitmasks)
    {
        const auto id{ std::string(m_tileTypeChars.at(k)).append(" ").append(magic_enum::enum_name(k)).append("##").append(std::to_string(v)) };
        if (ImGui::RadioButton(id.c_str(), m_bitmask_radio_button == v))
        {
            m_bitmask_radio_button = v;
        }
    }
    ImGui::Separator();

    ImGui::PopFont();
}

//-------------------------------------------------
// Noise
//-------------------------------------------------

void mdcii::world::IslandGenerator::CalcMapTypes(int32_t t_seed, float t_frequency, int32_t t_width, int32_t t_height)
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

            auto e{ fn.GetNoise(static_cast<double>(x), static_cast<double>(y)) / 2.0 + 0.5 };

            const auto lerp = [](const auto t_x, const auto t_y, const auto t_t) {
                return t_x * (1.0 - t_t) + t_y * t_t;
            };

            elevations.push_back(lerp(e, 1.0 - d, WATER_LEVEL));
        }
    }

    Log::MDCII_LOG_DEBUG("[IslandGenerator::CalcMapTypes()] Split elevation values into water and terrain.");

    for (const auto elevation : elevations)
    {
        elevation > WATER_LEVEL
            ? m_grassToBankMap.push_back(MapType::TERRAIN)
            : m_grassToBankMap.push_back(MapType::WATER);
    }
}

void mdcii::world::IslandGenerator::CalcBankToWater()
{
    for (const auto bitmask : m_bitmaskGrassToBank)
    {
        bitmask == 511
            ? m_bankToWaterMap.push_back(MapType::WATER)
            : m_bankToWaterMap.push_back(MapType::TERRAIN);
    }
}

void mdcii::world::IslandGenerator::CalcBitmaskValues(const std::vector<MapType>& t_map, std::vector<int32_t>& t_bitmasks)
{
    t_bitmasks.resize(m_width * m_height);
    std::fill(t_bitmasks.begin(), t_bitmasks.end(), 0);

    for (auto y{ 0 }; y < m_height; ++y)
    {
        for (auto x{ 0 }; x < m_width; ++x)
        {
            const auto idx{ GetIndex(x, y, m_width) };
            auto result{ static_cast<int>(GRASS_FLAG) };

            if (t_map.at(idx) == MapType::WATER)
            {
                result = static_cast<int>(WATER_FLAG);
            }

            result += GetNorthWestValue(t_map, x, y);
            result += GetNorthValue(t_map, x, y);
            result += GetNorthEastValue(t_map, x, y);

            result += GetWestValue(t_map, x, y);
            result += GetEastValue(t_map, x, y);

            result += GetSouthWestValue(t_map, x, y);
            result += GetSouthValue(t_map, x, y);
            result += GetSouthEastValue(t_map, x, y);

            t_bitmasks.at(idx) = result;
        }
    }
}

//-------------------------------------------------
// Bitmasking
//-------------------------------------------------

int32_t mdcii::world::IslandGenerator::GetNorthValue(const std::vector<MapType>& t_map, const int32_t t_x, const int32_t t_y)
{
    if (t_y - 1 < 0)
    {
        return static_cast<int>(NORTH_FLAG);
    }

    if (t_map.at(GetIndex(t_x, t_y - 1, m_width)) == MapType::WATER)
    {
        return static_cast<int>(NORTH_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetEastValue(const std::vector<MapType>& t_map, const int32_t t_x, const int32_t t_y)
{
    if (t_x + 1 >= m_width)
    {
        return static_cast<int>(EAST_FLAG);
    }

    if (t_map.at(GetIndex(t_x + 1, t_y, m_width)) == MapType::WATER)
    {
        return static_cast<int>(EAST_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetSouthValue(const std::vector<MapType>& t_map, const int32_t t_x, const int32_t t_y)
{
    if (t_y + 1 >= m_height)
    {
        return static_cast<int>(SOUTH_FLAG);
    }

    if (t_map.at(GetIndex(t_x, t_y + 1, m_width)) == MapType::WATER)
    {
        return static_cast<int>(SOUTH_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetWestValue(const std::vector<MapType>& t_map, const int32_t t_x, const int32_t t_y)
{
    if (t_x - 1 < 0)
    {
        return static_cast<int>(WEST_FLAG);
    }

    if (t_map.at(GetIndex(t_x - 1, t_y, m_width)) == MapType::WATER)
    {
        return static_cast<int>(WEST_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetNorthWestValue(const std::vector<MapType>& t_map, const int32_t t_x, const int32_t t_y)
{
    if (t_x - 1 < 0 || t_y - 1 < 0)
    {
        return static_cast<int>(NORTH_WEST_FLAG);
    }

    if (t_map.at(GetIndex(t_x - 1, t_y - 1, m_width)) == MapType::WATER)
    {
        return static_cast<int>(NORTH_WEST_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetNorthEastValue(const std::vector<MapType>& t_map, const int32_t t_x, const int32_t t_y)
{
    if (t_x + 1 >= m_width || t_y - 1 < 0)
    {
        return static_cast<int>(NORTH_EAST_FLAG);
    }

    if (t_map.at(GetIndex(t_x + 1, t_y - 1, m_width)) == MapType::WATER)
    {
        return static_cast<int>(NORTH_EAST_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetSouthWestValue(const std::vector<MapType>& t_map, const int32_t t_x, const int32_t t_y)
{
    if (t_x - 1 < 0 || t_y + 1 >= m_height)
    {
        return static_cast<int>(SOUTH_WEST_FLAG);
    }

    if (t_map.at(GetIndex(t_x - 1, t_y + 1, m_width)) == MapType::WATER)
    {
        return static_cast<int>(SOUTH_WEST_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetSouthEastValue(const std::vector<MapType>& t_map, const int32_t t_x, const int32_t t_y)
{
    if (t_x + 1 >= m_width || t_y + 1 >= m_height)
    {
        return static_cast<int>(SOUTH_EAST_FLAG);
    }

    if (t_map.at(GetIndex(t_x + 1, t_y + 1, m_width)) == MapType::WATER)
    {
        return static_cast<int>(SOUTH_EAST_FLAG);
    }

    return 0;
}

//-------------------------------------------------
// ImGui
//-------------------------------------------------

void mdcii::world::IslandGenerator::RenderMapTypesImGui(const std::vector<MapType>& t_map, const std::string& t_title) const
{
    ImGui::Begin(t_title.c_str());
    if (t_map.empty() || !m_render)
    {
        ImGui::Text("No values available.");
    }
    else
    {
        for (auto y{ 0 }; y < m_height; ++y)
        {
            for (auto x{ 0 }; x < m_width; ++x)
            {
                const auto& mapType{ t_map.at(GetIndex(x, y, m_width)) };
                if (mapType == MapType::TERRAIN)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                    ImGui::Text("%i", magic_enum::enum_integer(MapType::TERRAIN));
                    ImGui::SameLine();
                    ImGui::PopStyleColor();
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
                    ImGui::Text("%i", magic_enum::enum_integer(MapType::WATER));
                    ImGui::SameLine();
                    ImGui::PopStyleColor();
                }
            }

            ImGui::Text("");
        }
    }

    ImGui::End();
}

void mdcii::world::IslandGenerator::RenderBitmaskValuesImGui(const std::vector<int32_t>& t_bitmasks, const std::string& t_title) const
{
    ImGui::Begin(t_title.c_str());
    if (t_bitmasks.empty() || !m_render)
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
                const auto bitmask{ t_bitmasks.at(idx) };

                if (bitmask == 0) // terrain
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                }
                else if (bitmask > 0 && bitmask <= 255) // terrain before embankment
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 175, 0, 255));
                }
                else if (bitmask > 255 && bitmask <= 510) // embankment
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255));
                }
                else if (bitmask == 511) // water
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
                }

                ImGui::Text("%03i", bitmask);
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
    if (m_bitmaskGrassToBank.empty() ||
        m_grassToBankMap.empty() ||
        m_bitmaskBankToWater.empty() ||
        m_bankToWaterMap.empty() ||
        !m_render
    )
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
                const auto bitmask{ m_bitmaskGrassToBank.at(idx) };

                if (bitmask == 0) // terrain
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 64, 0, 255));
                }
                else if (bitmask > 0 && bitmask <= 255) // terrain before embankment
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 175, 0, 255));
                    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 32, 0, 255));
                }
                else if (bitmask > 255 && bitmask <= 510) // embankment
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255));
                    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(64, 64, 0, 255));
                }
                else if (bitmask == 511) // water
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
                    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 64, 255));
                }

                if (m_bitmaskTileTypes.count(bitmask))
                {
                    if (auto id{ std::string(m_tileTypeChars.at(m_bitmaskTileTypes.at(bitmask))).append("##").append(std::to_string(idx)) }; ImGui::Button(id.c_str()))
                    {
                        m_bitmaskGrassToBank.at(idx) = m_bitmask_radio_button;
                    }
                }
                else
                {
                    if (bitmask < 256) // terrain before embankment
                    {
                        // use grass
                        if (auto id{ std::string(m_tileTypeChars.at(AbstractTileType::GRASS)).append("##").append(std::to_string(idx)) }; ImGui::Button(id.c_str()))
                        {
                            m_bitmaskGrassToBank.at(idx) = m_bitmask_radio_button;
                        }
                    }
                    else // invalid
                    {
                        if (m_invalid.count(idx) == 0)
                        {
                            m_invalid.insert(idx);
                        }

                        ImGui::PopStyleColor(2);
                        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
                        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(64, 0, 0, 255));
                        if (auto id{ std::string(m_tileTypeChars.at(AbstractTileType::INVALID)).append("##").append(std::to_string(idx)) }; ImGui::Button(id.c_str()))
                        {
                            m_bitmaskGrassToBank.at(idx) = m_bitmask_radio_button;
                        }
                    }
                }
                ImGui::SameLine();
                ImGui::PopStyleColor(2);
            }

            ImGui::Text("");
        }
    }

    ImGui::PopFont();
    ImGui::End();
}

void mdcii::world::IslandGenerator::SaveIslandImGui()
{
    if (m_bitmaskGrassToBank.empty() ||
        m_grassToBankMap.empty() ||
        m_bitmaskBankToWater.empty() ||
        m_bankToWaterMap.empty() ||
        !m_render
    )
    {
        return;
    }

    if (!m_invalid.empty())
    {
        ImGui::Separator();
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
        ImGui::Text("Invalid positions found.");
        ImGui::Text("Change the invalid values before save.");
        ImGui::PopStyleColor();
    }

    static bool error{ false };
    static bool saved{ false };
    static std::string fileName;
    save_file_button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "SaveGame").c_str(), &fileName);

    if (!fileName.empty())
    {
        if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Save").c_str()))
        {
            fileName.append(".map");

            std::ofstream file;
            if (create_file(Game::RESOURCES_REL_PATH + "map/" + fileName, file))
            {
                nlohmann::json json;

                // todo: save island data only for the new *.isl file format
                json["version"] = Game::VERSION;
                json["world"] = { { "width", WORLD_WIDTH }, { "height", WORLD_HEIGHT } };

                AddIslandJson(json);

                file << json;

                Log::MDCII_LOG_DEBUG("[IslandGenerator::SaveIslandImGui()] The island has been successfully saved in file {}.", fileName);

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
// Json
//-------------------------------------------------

void mdcii::world::IslandGenerator::AddIslandJson(nlohmann::json& t_json)
{
    Log::MDCII_LOG_DEBUG("[IslandGenerator::AddIslandJson()] Adds Json values for an new island.");

    std::vector<std::shared_ptr<layer::Tile>> coastTiles;
    std::vector<std::shared_ptr<layer::Tile>> terrainTiles;
    std::vector<std::shared_ptr<layer::Tile>> buildingsTiles;

    nlohmann::json c = nlohmann::json::object();
    nlohmann::json t = nlohmann::json::object();
    nlohmann::json b = nlohmann::json::object();
    nlohmann::json i = nlohmann::json::object();

    i["width"] = m_width;
    i["height"] = m_height;
    i["x"] = 2;              // todo: remove island start x position in the world
    i["y"] = 2;              // todo: remove island start y position in the world
    i["layers"] = nlohmann::json::array();

    CreateTerrainTiles(terrainTiles);
    CreateCoastTiles(coastTiles);
    CreateBuildingsTiles(buildingsTiles);
    c["coast"] = coastTiles;
    t["terrain"] = terrainTiles;
    b["buildings"] = buildingsTiles;

    i["layers"].push_back(c);
    i["layers"].push_back(t);
    i["layers"].push_back(b);

    t_json["islands"].push_back(i);
}

//-------------------------------------------------
// Tiles
//-------------------------------------------------

void mdcii::world::IslandGenerator::CreateTerrainTiles(std::vector<std::shared_ptr<layer::Tile>>& t_terrainTiles)
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
            const auto bitmask{ m_bitmaskGrassToBank.at(idx) };

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
                if (m_bitmaskTileTypes.count(bitmask))
                {
                    switch (m_bitmaskTileTypes.at(bitmask))
                    {
                    case AbstractTileType::TOP:
                        t_terrainTiles.at(idx) = CreateTile(data::EMBANKMENT_BUILDING_IDS.at(randForEmbankment(gen)), x, y, Rotation::DEG180);
                        break;
                    case AbstractTileType::BOTTOM:
                        t_terrainTiles.at(idx) = CreateTile(data::EMBANKMENT_BUILDING_IDS.at(randForEmbankment(gen)), x, y, Rotation::DEG0);
                        break;
                    case AbstractTileType::LEFT:
                        t_terrainTiles.at(idx) = CreateTile(data::EMBANKMENT_BUILDING_IDS.at(randForEmbankment(gen)), x, y, Rotation::DEG90);
                        break;
                    case AbstractTileType::RIGHT:
                        t_terrainTiles.at(idx) = CreateTile(data::EMBANKMENT_BUILDING_IDS.at(randForEmbankment(gen)), x, y, Rotation::DEG270);
                        break;

                    case AbstractTileType::CORNER_OUT_TL:
                        t_terrainTiles.at(idx) = CreateTile(data::EMBANKMENT_CORNER_BUILDING_IDS.at(randForEmbankmentCorner(gen)), x, y, Rotation::DEG90);
                        break;
                    case AbstractTileType::CORNER_OUT_TR:
                        t_terrainTiles.at(idx) = CreateTile(data::EMBANKMENT_CORNER_BUILDING_IDS.at(randForEmbankmentCorner(gen)), x, y, Rotation::DEG180);
                        break;
                    case AbstractTileType::CORNER_OUT_BL:
                        t_terrainTiles.at(idx) = CreateTile(data::EMBANKMENT_CORNER_BUILDING_IDS.at(randForEmbankmentCorner(gen)), x, y, Rotation::DEG0);
                        break;
                    case AbstractTileType::CORNER_OUT_BR:
                        t_terrainTiles.at(idx) = CreateTile(data::EMBANKMENT_CORNER_BUILDING_IDS.at(randForEmbankmentCorner(gen)), x, y, Rotation::DEG270);
                        break;

                    case AbstractTileType::CORNER_IN_TL:
                        t_terrainTiles.at(idx) = CreateTile(data::EMBANKMENT_CORNER_INSIDE_BUILDING_IDS.at(randForEmbankmentCornerInside(gen)), x, y, Rotation::DEG270);
                        break;
                    case AbstractTileType::CORNER_IN_TR:
                        t_terrainTiles.at(idx) = CreateTile(data::EMBANKMENT_CORNER_INSIDE_BUILDING_IDS.at(randForEmbankmentCornerInside(gen)), x, y, Rotation::DEG0);
                        break;
                    case AbstractTileType::CORNER_IN_BL:
                        t_terrainTiles.at(idx) = CreateTile(data::EMBANKMENT_CORNER_INSIDE_BUILDING_IDS.at(randForEmbankmentCornerInside(gen)), x, y, Rotation::DEG180);
                        break;
                    case AbstractTileType::CORNER_IN_BR:
                        t_terrainTiles.at(idx) = CreateTile(data::EMBANKMENT_CORNER_INSIDE_BUILDING_IDS.at(randForEmbankmentCornerInside(gen)), x, y, Rotation::DEG90);
                        break;

                    default:
                        t_terrainTiles.at(idx) = CreateTile(data::BEACH_BUILDING_ID, x, y, Rotation::DEG0);
                    }
                }
            }
        }
    }
}

void mdcii::world::IslandGenerator::CreateCoastTiles(std::vector<std::shared_ptr<layer::Tile>>& t_coastTiles)
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
            const auto bitmask{ m_bitmaskGrassToBank.at(idx) };

            switch (m_grassToBankMap.at(idx))
            {
            case MapType::TERRAIN:
                break;
            case MapType::WATER:
                if (bitmask == 511)
                {
                    t_coastTiles.at(idx) = CreateTile(data::SHALLOW_WATER_BUILDING_ID, x, y, Rotation::DEG0);
                }
                break;
            default:;
            }
        }
    }
}

void mdcii::world::IslandGenerator::CreateBuildingsTiles(std::vector<std::shared_ptr<layer::Tile>>& t_buildingsTiles)
{
    Log::MDCII_LOG_DEBUG("[IslandGenerator::CreateBuildingsTiles()] Create building tiles.");

    for (auto y{ 0 }; y < m_height; ++y)
    {
        for (auto x{ 0 }; x < m_width; ++x)
        {
            t_buildingsTiles.emplace_back(std::make_unique<layer::Tile>());
        }
    }
}

std::unique_ptr<mdcii::layer::Tile> mdcii::world::IslandGenerator::CreateTile(const int32_t t_id, const int32_t t_worldX, const int32_t t_worldY, const Rotation t_rotation)
{
    std::unique_ptr<mdcii::layer::Tile> tile{ std::make_unique<layer::Tile>() };
    tile->buildingId = t_id;
    tile->rotation = t_rotation;
    tile->x = 0;
    tile->y = 0;
    tile->worldXDeg0 = t_worldX;
    tile->worldYDeg0 = t_worldY;

    return tile;
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
    m_render = false;

    std::vector<MapType>().swap(m_grassToBankMap);
    std::vector<int32_t>().swap(m_bitmaskGrassToBank);

    std::vector<MapType>().swap(m_bankToWaterMap);
    std::vector<int32_t>().swap(m_bitmaskBankToWater);

    std::unordered_set<int32_t>().swap(m_invalid);

    m_render = true;
}
