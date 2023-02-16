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
#include "IslandGenerator.h"
#include "FastNoiseLite.h"
#include "MdciiAssert.h"

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
    static bool south{ false };
    static int32_t seed{ 2394 };
    static float frequency{ 0.043f };

    ImGui::Checkbox("South", &south);
    ImGui::SliderInt("Island width", &m_width, 20, 60);
    ImGui::SliderInt("Island height", &m_height, 20, 60);
    ImGui::SliderInt("Seed", &seed, 100, 9000);
    ImGui::SliderFloat("Frequency", &frequency, 0.01f, 0.3f);

    if (ImGui::Button("Noise"))
    {
        CalcTerrainValues(seed, frequency, m_width, m_height);
    }

    if (!m_terrainValues.empty() && ImGui::Button("Create results"))
    {
        CalcBitmaskValues();
    }

    for (auto value : m_invalid)
    {
        ImGui::Text("%i", value);
    }

    RenderTerrainValuesImGui();
    RenderBitmaskValuesImGui();
    RenderTilesImGui();
}

//-------------------------------------------------
// Noise
//-------------------------------------------------

void mdcii::world::IslandGenerator::CalcTerrainValues(int32_t t_seed, float t_frequency, int32_t t_width, int32_t t_height)
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
        elevation > WATER_LEVEL ? m_terrainValues.push_back(TERRAIN) : m_terrainValues.push_back(WATER);
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

            if (m_terrainValues.at(idx) == WATER)
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

    if (m_terrainValues.at(GetIndex(t_x, t_y - 1, m_width)) == WATER)
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

    if (m_terrainValues.at(GetIndex(t_x + 1, t_y, m_width)) == WATER)
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

    if (m_terrainValues.at(GetIndex(t_x, t_y + 1, m_width)) == WATER)
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

    if (m_terrainValues.at(GetIndex(t_x - 1, t_y, m_width)) == WATER)
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

    if (m_terrainValues.at(GetIndex(t_x - 1, t_y - 1, m_width)) == WATER)
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

    if (m_terrainValues.at(GetIndex(t_x + 1, t_y - 1, m_width)) == WATER)
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

    if (m_terrainValues.at(GetIndex(t_x - 1, t_y + 1, m_width)) == WATER)
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

    if (m_terrainValues.at(GetIndex(t_x + 1, t_y + 1, m_width)) == WATER)
    {
        return static_cast<int>(SOUTH_EAST_FLAG);
    }

    return 0;
}

//-------------------------------------------------
// ImGui
//-------------------------------------------------

void mdcii::world::IslandGenerator::RenderTerrainValuesImGui()
{
    ImGui::Begin("Terrain");
    if (m_terrainValues.empty())
    {
        ImGui::Text("No values available.");
    }
    else
    {
        for (auto y{ 0 }; y < m_height; ++y)
        {
            for (auto x{ 0 }; x < m_width; ++x)
            {
                const auto& position{ m_terrainValues.at(GetIndex(x, y, m_width)) };
                if (position > WATER_LEVEL)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                    ImGui::Text("%i", TERRAIN);
                    ImGui::SameLine();
                    ImGui::PopStyleColor();
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
                    ImGui::Text("%i", WATER);
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
    ImGui::Begin("Results");
    if (m_bitmaskValues.empty() || m_terrainValues.empty())
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
                const auto result{ m_bitmaskValues.at(idx) };

                switch (m_terrainValues.at(idx))
                {
                case TERRAIN:
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                    ImGui::Text("%03i", result);
                    ImGui::SameLine();
                    ImGui::PopStyleColor();
                    break;
                case WATER:
                    result < 511
                        ? ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255))
                        : ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));

                    ImGui::Text("%03i", result);
                    ImGui::SameLine();
                    ImGui::PopStyleColor();
                    break;
                default:;
                }
            }

            ImGui::Text("");
        }
    }

    ImGui::End();
}

void mdcii::world::IslandGenerator::RenderTilesImGui()
{
    ImGui::Begin("Coast");
    if (m_bitmaskValues.empty() || m_terrainValues.empty())
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
                const auto result{ m_bitmaskValues.at(idx) };

                switch (m_terrainValues.at(idx))
                {
                case TERRAIN:
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                    ImGui::Text("----");
                    ImGui::SameLine();
                    ImGui::PopStyleColor();
                    break;
                case WATER:
                    if (result == 511)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
                        ImGui::Text("~~~~");
                    }
                    else
                    {
                        if (m_tiles.count(result))
                        {
                            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255));
                            ImGui::Text("%s", m_tiles.at(result));
                        }
                        else
                        {
                            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
                            ImGui::Text("----");
                            if (m_invalid.count(result) == 0)
                            {
                                m_invalid.insert(result);
                            }
                        }
                    }

                    ImGui::SameLine();
                    ImGui::PopStyleColor();
                    break;
                default:;
                }
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
