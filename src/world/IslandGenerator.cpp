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

    if (ImGui::Button("Noise"))
    {
        CalcMapTypes(seed, frequency, m_width, m_height);
    }

    if (!m_terrainValues.empty() && ImGui::Button("Calc bitmask values"))
    {
        CalcBitmaskValues();
    }

    RenderMapTypesImGui();
    RenderBitmaskValuesImGui();
    RenderBitmaskValuesAsCharsImGui();

    SaveIslandImGui();

    ImFontAtlas* atlas = ImGui::GetIO().Fonts;
    ImGui::PushFont(atlas->Fonts[1]);

    if (!m_invalid.empty())
    {
        ImGui::Separator();
        ImGui::Text("Invalid positions found.");

        static auto mode{ m_tileTypeBitmasks.at(AbstractTileType::GRASS) };
        for (const auto [k, v] : m_tileTypeBitmasks)
        {
            const auto id{ std::string(m_tileTypeChars.at(k)).append(" ").append(magic_enum::enum_name(k)).append("##").append(std::to_string(v)) };
            if (ImGui::RadioButton(id.c_str(), mode == v))
            {
                mode = v;
            }
        }

        for (auto idx : m_invalid)
        {
            if (auto id{ "Change index " + std::to_string(idx).append("##").append(std::to_string(idx)) }; ImGui::Button(id.c_str()))
            {
                m_bitmaskValues.at(idx) = mode;
            }
        }

        ImGui::Separator();
    }

    ImGui::PopFont();
}

//-------------------------------------------------
// Noise
//-------------------------------------------------

void mdcii::world::IslandGenerator::CalcMapTypes(int32_t t_seed, float t_frequency, int32_t t_width, int32_t t_height)
{
    Log::MDCII_LOG_DEBUG("[IslandGenerator::CalcTerrainValues()] Create elevations for a 2D map using Perlin Noise.");

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

    Log::MDCII_LOG_DEBUG("[IslandGenerator::CalcTerrainValues()] Split elevation values into water and terrain.");

    for (const auto elevation : elevations)
    {
        elevation > WATER_LEVEL
            ? m_terrainValues.push_back(MapType::TERRAIN)
            : m_terrainValues.push_back(MapType::WATER);
    }
}

void mdcii::world::IslandGenerator::CalcBitmaskValues()
{
    m_bitmaskValues.resize(m_width * m_height);
    std::fill(m_bitmaskValues.begin(), m_bitmaskValues.end(), 0);

    for (auto y{ 0 }; y < m_height; ++y)
    {
        for (auto x{ 0 }; x < m_width; ++x)
        {
            const auto idx{ GetIndex(x, y, m_width) };
            auto result{ static_cast<int>(GRASS_FLAG) };

            if (m_terrainValues.at(idx) == MapType::WATER)
            {
                result = static_cast<int>(WATER_FLAG);
            }

            result += GetNorthWestValue(x, y);
            result += GetNorthValue(x, y);
            result += GetNorthEastValue(x, y);

            result += GetWestValue(x, y);
            result += GetEastValue(x, y);

            result += GetSouthWestValue(x, y);
            result += GetSouthValue(x, y);
            result += GetSouthEastValue(x, y);

            m_bitmaskValues.at(idx) = result;
        }
    }
}

//-------------------------------------------------
// Bitmasking
//-------------------------------------------------

int32_t mdcii::world::IslandGenerator::GetNorthValue(const int32_t t_x, const int32_t t_y)
{
    if (t_y - 1 < 0)
    {
        return static_cast<int>(NORTH_FLAG);
    }

    if (m_terrainValues.at(GetIndex(t_x, t_y - 1, m_width)) == MapType::WATER)
    {
        return static_cast<int>(NORTH_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetEastValue(const int32_t t_x, const int32_t t_y)
{
    if (t_x + 1 >= m_width)
    {
        return static_cast<int>(EAST_FLAG);
    }

    if (m_terrainValues.at(GetIndex(t_x + 1, t_y, m_width)) == MapType::WATER)
    {
        return static_cast<int>(EAST_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetSouthValue(const int32_t t_x, const int32_t t_y)
{
    if (t_y + 1 >= m_height)
    {
        return static_cast<int>(SOUTH_FLAG);
    }

    if (m_terrainValues.at(GetIndex(t_x, t_y + 1, m_width)) == MapType::WATER)
    {
        return static_cast<int>(SOUTH_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetWestValue(const int32_t t_x, const int32_t t_y)
{
    if (t_x - 1 < 0)
    {
        return static_cast<int>(WEST_FLAG);
    }

    if (m_terrainValues.at(GetIndex(t_x - 1, t_y, m_width)) == MapType::WATER)
    {
        return static_cast<int>(WEST_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetNorthWestValue(const int32_t t_x, const int32_t t_y)
{
    if (t_x - 1 < 0 || t_y - 1 < 0)
    {
        return static_cast<int>(NORTH_WEST_FLAG);
    }

    if (m_terrainValues.at(GetIndex(t_x - 1, t_y - 1, m_width)) == MapType::WATER)
    {
        return static_cast<int>(NORTH_WEST_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetNorthEastValue(const int32_t t_x, const int32_t t_y)
{
    if (t_x + 1 >= m_width || t_y - 1 < 0)
    {
        return static_cast<int>(NORTH_EAST_FLAG);
    }

    if (m_terrainValues.at(GetIndex(t_x + 1, t_y - 1, m_width)) == MapType::WATER)
    {
        return static_cast<int>(NORTH_EAST_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetSouthWestValue(const int32_t t_x, const int32_t t_y)
{
    if (t_x - 1 < 0 || t_y + 1 >= m_height)
    {
        return static_cast<int>(SOUTH_WEST_FLAG);
    }

    if (m_terrainValues.at(GetIndex(t_x - 1, t_y + 1, m_width)) == MapType::WATER)
    {
        return static_cast<int>(SOUTH_WEST_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetSouthEastValue(const int32_t t_x, const int32_t t_y)
{
    if (t_x + 1 >= m_width || t_y + 1 >= m_height)
    {
        return static_cast<int>(SOUTH_EAST_FLAG);
    }

    if (m_terrainValues.at(GetIndex(t_x + 1, t_y + 1, m_width)) == MapType::WATER)
    {
        return static_cast<int>(SOUTH_EAST_FLAG);
    }

    return 0;
}

//-------------------------------------------------
// ImGui
//-------------------------------------------------

void mdcii::world::IslandGenerator::RenderMapTypesImGui()
{
    ImGui::Begin("Map types (Terrain/Water)");
    if (m_terrainValues.empty() || !m_render)
    {
        ImGui::Text("No values available.");
    }
    else
    {
        for (auto y{ 0 }; y < m_height; ++y)
        {
            for (auto x{ 0 }; x < m_width; ++x)
            {
                const auto& mapType{ m_terrainValues.at(GetIndex(x, y, m_width)) };
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

void mdcii::world::IslandGenerator::RenderBitmaskValuesImGui()
{
    ImGui::Begin("Bitmask values");
    if (m_bitmaskValues.empty() || m_terrainValues.empty() || !m_render)
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
    if (m_bitmaskValues.empty() || m_terrainValues.empty() || !m_render)
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

                if (m_bitmaskTileTypes.count(bitmask))
                {
                    ImGui::TextUnformatted(m_tileTypeChars.at(m_bitmaskTileTypes.at(bitmask)));
                }
                else
                {
                    if (bitmask < 256) // terrain before embankment
                    {
                        ImGui::TextUnformatted(m_tileTypeChars.at(AbstractTileType::GRASS));
                    }
                    else // invalid
                    {
                        if (m_invalid.count(idx) == 0)
                        {
                            m_invalid.insert(idx);
                        }

                        ImGui::PopStyleColor();
                        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
                        ImGui::TextUnformatted(m_tileTypeChars.at(AbstractTileType::INVALID));
                    }
                }
                ImGui::SameLine();
                ImGui::PopStyleColor();
            }

            ImGui::Text("");
        }
    }

    ImGui::PopFont();
    ImGui::End();
}

void mdcii::world::IslandGenerator::SaveIslandImGui()
{
    if (m_bitmaskValues.empty() || m_terrainValues.empty() || !m_render)
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
            fileName.append(".map");

            std::ofstream file;
            if (create_file(Game::RESOURCES_REL_PATH + "map/" + fileName, file))
            {
                nlohmann::json json;

                // todo: save island data only / new fileformat
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
    i["x"] = 2;              // todo: island start x position in the world
    i["y"] = 2;              // todo: island start y position in the world
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
    std::uniform_int_distribution randInRange(0, 10);

    for (auto y{ 0 }; y < m_height; ++y)
    {
        for (auto x{ 0 }; x < m_width; ++x)
        {
            const auto idx{ GetIndex(x, y, m_width) };
            const auto bitmask{ m_bitmaskValues.at(idx) };

            switch (m_terrainValues.at(idx))
            {
            case MapType::TERRAIN:
                // create tree or grass
                if (m_south)
                {
                    if (const auto r{ randInRange(gen) }; r >= 7)
                    {
                        t_terrainTiles.at(idx) = CreateTile(data::SOUTH_TREES.at(randInRange(gen)), x, y, Rotation::DEG0);
                    }
                    else
                    {
                        t_terrainTiles.at(idx) = CreateTile(data::GRASS_BUILDING_ID, x, y, Rotation::DEG0);
                    }
                }
                else
                {
                    if (const auto r{ randInRange(gen) }; r >= 7)
                    {
                        t_terrainTiles.at(idx) = CreateTile(data::NORTH_TREES.at(randInRange(gen)), x, y, Rotation::DEG0);
                    }
                    else
                    {
                        t_terrainTiles.at(idx) = CreateTile(data::GRASS_BUILDING_ID, x, y, Rotation::DEG0);
                    }
                }

                break;
            case MapType::WATER:
                if (bitmask == 511)
                {
                    // todo: deep water
                }
                else
                {
                    /*
                    if (m_txtTiles.count(bitmask))
                    {
                        // todo corners
                    }
                    else
                    {
                        // todo: invalid
                    }
                    */
                }
                break;
            default:;
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
            const auto bitmask{ m_bitmaskValues.at(idx) };

            switch (m_terrainValues.at(idx))
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

    std::vector<MapType>().swap(m_terrainValues);
    std::vector<int32_t>().swap(m_bitmaskValues);
    std::unordered_set<int32_t>().swap(m_invalid);

    m_render = true;
}
