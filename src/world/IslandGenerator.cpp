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
#include "data/Text.h"
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
    static int32_t seed{ 5990 };
    static float frequency{ 0.116f };

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

    if (ImGui::Button("Noise"))
    {
        CalcMapTypes(seed, frequency, m_width, m_height);
        CalcBitmaskValues(m_map, m_bitmaskValues);
    }

    RenderMapTypesImGui(m_map, "Map types");
    RenderBitmaskValuesImGui(m_bitmaskValues, "Bitmask values");
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
            ? m_map.push_back(MapType::TERRAIN)
            : m_map.push_back(MapType::WATER);
    }
}


void mdcii::world::IslandGenerator::CalcBitmaskValues(const std::vector<MapType>& t_map, std::vector<int32_t>& t_bitmaskValues)
{
    t_bitmaskValues.resize(m_width * m_height);
    std::fill(t_bitmaskValues.begin(), t_bitmaskValues.end(), 0);

    for (auto y{ 0 }; y < m_height; ++y)
    {
        for (auto x{ 0 }; x < m_width; ++x)
        {
            const auto idx{ GetIndex(x, y, m_width) };
            auto result{ static_cast<int>(TERRAIN_FLAG) };

            if (t_map.at(idx) == MapType::WATER)
            {
                result = static_cast<int>(EMBANKMENT_FLAG);
            }

            result += GetNorthWestValue(t_map, x, y, MapType::WATER);
            result += GetNorthValue(t_map, x, y, MapType::WATER);
            result += GetNorthEastValue(t_map, x, y, MapType::WATER);

            result += GetWestValue(t_map, x, y, MapType::WATER);
            result += GetEastValue(t_map, x, y, MapType::WATER);

            result += GetSouthWestValue(t_map, x, y, MapType::WATER);
            result += GetSouthValue(t_map, x, y, MapType::WATER);
            result += GetSouthEastValue(t_map, x, y, MapType::WATER);

            t_bitmaskValues.at(idx) = result;
        }
    }

    auto i{ 0 };
    for (const auto bitmask : m_bitmaskValues)
    {
        if (const auto mapTypeCol{ Bitmask2MapTypeCol(bitmask) }; mapTypeCol.mapType == MapType::EMBANKMENT)
        {
            m_map.at(i) = MapType::EMBANKMENT;
        }

        i++;
    }

    /*
    std::fill(t_bitmaskValues.begin(), t_bitmaskValues.end(), 0);

    for (auto y{ 0 }; y < m_height; ++y)
    {
        for (auto x{ 0 }; x < m_width; ++x)
        {
            const auto idx{ GetIndex(x, y, m_width) };
            auto result{ static_cast<int>(GRASS_FLAG) };

            if (t_map.at(idx) == MapType::EMBANKMENT)
            {
                result = static_cast<int>(EMBANKMENT_FLAG);
            }

            if (t_map.at(idx) == MapType::WATER)
            {
                result = static_cast<int>(SHALLOW_WATER_FLAG);
            }

            result += GetNorthWestValue(t_map, x, y, MapType::WATER);
            result += GetNorthValue(t_map, x, y, MapType::WATER);
            result += GetNorthEastValue(t_map, x, y, MapType::WATER);

            result += GetWestValue(t_map, x, y, MapType::WATER);
            result += GetEastValue(t_map, x, y, MapType::WATER);

            result += GetSouthWestValue(t_map, x, y, MapType::WATER);
            result += GetSouthValue(t_map, x, y, MapType::WATER);
            result += GetSouthEastValue(t_map, x, y, MapType::WATER);

            t_bitmaskValues.at(idx) = result;
        }
    }

    i = 0;
    for (const auto bitmask : m_bitmaskValues)
    {
        const auto mapTypeCol{ Bitmask2MapTypeCol(bitmask) };
        if (mapTypeCol.mapType == MapType::SHALLOW_WATER)
        {
            m_map.at(i) = MapType::SHALLOW_WATER;
        }

        i++;
    }
    */
}

//-------------------------------------------------
// Bitmasking
//-------------------------------------------------

int32_t mdcii::world::IslandGenerator::GetNorthValue(const std::vector<MapType>& t_map, const int32_t t_x, const int32_t t_y, const MapType t_mapType)
{
    if (t_y - 1 < 0)
    {
        return static_cast<int>(NORTH_FLAG);
    }

    if (t_map.at(GetIndex(t_x, t_y - 1, m_width)) == t_mapType)
    {
        return static_cast<int>(NORTH_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetEastValue(const std::vector<MapType>& t_map, const int32_t t_x, const int32_t t_y, const MapType t_mapType)
{
    if (t_x + 1 >= m_width)
    {
        return static_cast<int>(EAST_FLAG);
    }

    if (t_map.at(GetIndex(t_x + 1, t_y, m_width)) == t_mapType)
    {
        return static_cast<int>(EAST_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetSouthValue(const std::vector<MapType>& t_map, const int32_t t_x, const int32_t t_y, const MapType t_mapType)
{
    if (t_y + 1 >= m_height)
    {
        return static_cast<int>(SOUTH_FLAG);
    }

    if (t_map.at(GetIndex(t_x, t_y + 1, m_width)) == t_mapType)
    {
        return static_cast<int>(SOUTH_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetWestValue(const std::vector<MapType>& t_map, const int32_t t_x, const int32_t t_y, const MapType t_mapType)
{
    if (t_x - 1 < 0)
    {
        return static_cast<int>(WEST_FLAG);
    }

    if (t_map.at(GetIndex(t_x - 1, t_y, m_width)) == t_mapType)
    {
        return static_cast<int>(WEST_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetNorthWestValue(const std::vector<MapType>& t_map, const int32_t t_x, const int32_t t_y, const MapType t_mapType)
{
    if (t_x - 1 < 0 || t_y - 1 < 0)
    {
        return static_cast<int>(NORTH_WEST_FLAG);
    }

    if (t_map.at(GetIndex(t_x - 1, t_y - 1, m_width)) == t_mapType)
    {
        return static_cast<int>(NORTH_WEST_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetNorthEastValue(const std::vector<MapType>& t_map, const int32_t t_x, const int32_t t_y, const MapType t_mapType)
{
    if (t_x + 1 >= m_width || t_y - 1 < 0)
    {
        return static_cast<int>(NORTH_EAST_FLAG);
    }

    if (t_map.at(GetIndex(t_x + 1, t_y - 1, m_width)) == t_mapType)
    {
        return static_cast<int>(NORTH_EAST_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetSouthWestValue(const std::vector<MapType>& t_map, const int32_t t_x, const int32_t t_y, const MapType t_mapType)
{
    if (t_x - 1 < 0 || t_y + 1 >= m_height)
    {
        return static_cast<int>(SOUTH_WEST_FLAG);
    }

    if (t_map.at(GetIndex(t_x - 1, t_y + 1, m_width)) == t_mapType)
    {
        return static_cast<int>(SOUTH_WEST_FLAG);
    }

    return 0;
}

int32_t mdcii::world::IslandGenerator::GetSouthEastValue(const std::vector<MapType>& t_map, const int32_t t_x, const int32_t t_y, const MapType t_mapType)
{
    if (t_x + 1 >= m_width || t_y + 1 >= m_height)
    {
        return static_cast<int>(SOUTH_EAST_FLAG);
    }

    if (t_map.at(GetIndex(t_x + 1, t_y + 1, m_width)) == t_mapType)
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
                    ImGui::PushStyleColor(ImGuiCol_Text, TERRAIN_COL);
                    ImGui::Text("%i", magic_enum::enum_integer(MapType::TERRAIN));
                    ImGui::SameLine();
                    ImGui::PopStyleColor();
                }
                else if (mapType == MapType::EMBANKMENT)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, EMBANKMENT_COL);
                    ImGui::Text("%i", magic_enum::enum_integer(MapType::EMBANKMENT));
                    ImGui::SameLine();
                    ImGui::PopStyleColor();
                }
                else if (mapType == MapType::COAST)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, COAST_COL);
                    ImGui::Text("%i", magic_enum::enum_integer(MapType::COAST));
                    ImGui::SameLine();
                    ImGui::PopStyleColor();
                }
                else if (mapType == MapType::SHALLOW_WATER)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, SHALLOW_WATER_COL);
                    ImGui::Text("%i", magic_enum::enum_integer(MapType::SHALLOW_WATER));
                    ImGui::SameLine();
                    ImGui::PopStyleColor();
                }
                else if (mapType == MapType::WATER)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, WATER_COL);
                    ImGui::Text("%i", magic_enum::enum_integer(MapType::WATER));
                    ImGui::SameLine();
                    ImGui::PopStyleColor();
                }
                else // MapType::INVALID
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, INVALID_COL);
                    ImGui::Text("%i", magic_enum::enum_integer(MapType::INVALID));
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

                const auto mapTypeCol{ Bitmask2MapTypeCol(bitmask) };
                ImGui::PushStyleColor(ImGuiCol_Text, mapTypeCol.color);

                ImGui::Text("%04i", bitmask);
                ImGui::SameLine();
                ImGui::PopStyleColor();
            }

            ImGui::Text("");
        }
    }

    ImGui::End();
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

    std::vector<MapType>().swap(m_map);
    std::vector<int32_t>().swap(m_bitmaskValues);

    m_render = true;
}

mdcii::world::IslandGenerator::MapTypeCol mdcii::world::IslandGenerator::Bitmask2MapTypeCol(const int32_t t_bitmask)
{
    if (t_bitmask == 0)
    {
        return { MapType::TERRAIN, TERRAIN_COL };
    }
    else if (t_bitmask > 0 && t_bitmask <= 255)
    {
        return { MapType::TERRAIN, TERRAIN_BORDER_COL };
    }
    else if (t_bitmask >= 256 && t_bitmask < 511)
    {
        return { MapType::EMBANKMENT, EMBANKMENT_COL };
    }
    if (t_bitmask == 511)
    {
        return { MapType::WATER, WATER_COL };
    }

    /*
    if (t_bitmask == 767)
    {
        return { MapType::WATER, WATER_COL };
    }
    else if (t_bitmask > 0 && t_bitmask <= 255)
    {
        return { MapType::TERRAIN, IM_COL32(22, 227, 15, 255) };
    }
    else if (t_bitmask >= 256 && t_bitmask < 511)
    {
        return { MapType::EMBANKMENT, EMBANKMENT_COL };
    }
    else if (t_bitmask >= 512 && t_bitmask <= 766)
    {
        return { MapType::SHALLOW_WATER, SHALLOW_WATER_COL };
    }
    */

    return { MapType::INVALID, INVALID_COL };
}
