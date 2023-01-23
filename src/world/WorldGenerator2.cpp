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
#include <filesystem>
#include "WorldGenerator2.h"
#include "FastNoiseLite.h"
#include "World.h"
#include "MdciiAssert.h"
#include "Game.h"
#include "layer/TerrainLayer.h"
#include "data/BuildingIds.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::WorldGenerator2::WorldGenerator2()
{
    Log::MDCII_LOG_DEBUG("[WorldGenerator2::WorldGenerator2()] Create WorldGenerator2.");
}

mdcii::world::WorldGenerator2::~WorldGenerator2() noexcept
{
    Log::MDCII_LOG_DEBUG("[WorldGenerator2::~WorldGenerator2()] Destruct WorldGenerator2.");
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::world::WorldGenerator2::RenderImGui()
{
    static int32_t worldWidth{ World::WORLD_MIN_WIDTH };
    static int32_t worldHeight{ World::WORLD_MIN_HEIGHT };

    static bool south{ false };

    static int32_t islandWidth{ 20 };
    static int32_t islandHeight{ 20 };
    static int32_t seed{ 4181 };
    static float frequency{ 0.086f };

    ImGui::Separator();
    ImGui::SliderInt("World width ", &worldWidth, World::WORLD_MIN_WIDTH, World::WORLD_MAX_WIDTH);
    ImGui::SliderInt("World height", &worldHeight, World::WORLD_MIN_HEIGHT, World::WORLD_MAX_HEIGHT);
    ImGui::Separator();
    ImGui::Checkbox("South", &south);
    ImGui::SliderInt("Island width", &islandWidth, 20, 60);
    ImGui::SliderInt("Island height", &islandHeight, 20, 60);
    ImGui::SliderInt("Seed", &seed, 100, 9000);
    ImGui::SliderFloat("Frequency", &frequency, 0.01f, 0.3f);
    ImGui::Separator();

    // current Position objects updated when "Create new map" button pressed
    static std::vector<Position> currentPositions;

    // create new Position objects
    if (ImGui::Button("Create new map"))
    {
        auto createdPositions{ CreateElevations(seed, frequency, islandWidth, islandHeight) };
        StoreNeighbors(createdPositions, islandWidth, islandHeight);
        SplitElevationsInWaterAndTerrain(createdPositions);

        AddDefaultEmbankment(createdPositions);
        CreateEmbankmentNeighbors(createdPositions);
        ValidateEmbankment(createdPositions);

        AddDefaultBeach(createdPositions);

        SetFinalEmbankment(createdPositions);

        // swap
        currentPositions = std::move(createdPositions);
    }

    if (ImGui::Button("Reset"))
    {
        std::vector<Position>().swap(currentPositions);
    }

    RenderLegendImGui();
    RenderElevationValuesImGui(currentPositions, islandWidth, islandHeight);
    RenderMapValuesImGui(currentPositions, islandWidth, islandHeight);
    RenderMapNeighborValuesImGui(currentPositions, islandWidth, islandHeight);

    // save button

    struct TextFilters
    {
        static int FilterAZ(ImGuiInputTextCallbackData* data)
        {
            if (auto c{ data->EventChar }; (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
            {
                return 0;
            }

            return 1;
        }
    };

    static char f[64] = "";
    static bool printError{ false };

    if (!currentPositions.empty())
    {
        ImGui::InputText("enter filename", f, 64, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CallbackCharFilter, TextFilters::FilterAZ);
        if (ImGui::Button("Save map") && f[0] != 0)
        {
            if (CreateFile(f))
            {
                nlohmann::json j;

                AddWorldValues(j, worldWidth, worldHeight);
                AddIslandValues(j, currentPositions, islandWidth, islandHeight, 1, 1, south);
                WriteJsonToFile(j);

                printError = false;
            }
            else
            {
                printError = true;
            }
        }
    }

    if (printError)
    {
        ImGui::Text("Error while creating file.");
    }
}

//-------------------------------------------------
// Create
//-------------------------------------------------

std::vector<mdcii::world::WorldGenerator2::Position> mdcii::world::WorldGenerator2::CreateElevations(
    const int32_t t_seed,
    const float t_frequency,
    const int32_t t_width,
    const int32_t t_height
)
{
    Log::MDCII_LOG_DEBUG("[WorldGenerator2::CreateElevations()] Create elevations for a 2D map using Perlin Noise.");

    FastNoiseLite fn;
    fn.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    fn.SetSeed(t_seed);
    fn.SetFrequency(t_frequency);

    std::vector<Position> positions;
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
            e = lerp(e, 1.0 - d, Position::WATER_LEVEL);

            positions.emplace_back(x, y, e);
        }
    }

    return positions;
}

void mdcii::world::WorldGenerator2::StoreNeighbors(std::vector<Position>& t_positions, const int32_t t_width, const int32_t t_height)
{
    if (t_positions.empty())
    {
        return;
    }

    Log::MDCII_LOG_DEBUG("[WorldGenerator2::StoreNeighbors()] Stores the neighbor indices of each Position object.");

    for (auto y{ 0 }; y < t_height; ++y)
    {
        for (auto x{ 0 }; x < t_width; ++x)
        {
            const auto idx{ GetIndex(x, y, t_width) };

            // regular grid
            if (y > 0)
            {
                t_positions.at(idx).neighborIndices.at(magic_enum::enum_integer(Direction::N)) = (y - 1) * t_width + x;
            }

            if (y < t_height - 1)
            {
                t_positions.at(idx).neighborIndices.at(magic_enum::enum_integer(Direction::S)) = (y + 1) * t_width + x;
            }

            if (x > 0)
            {
                t_positions.at(idx).neighborIndices.at(magic_enum::enum_integer(Direction::W)) = y * t_width + (x - 1);
            }

            if (x < t_width - 1)
            {
                t_positions.at(idx).neighborIndices.at(magic_enum::enum_integer(Direction::E)) = y * t_width + (x + 1);
            }

            // connect diagonally
            if (y > 0 && x < t_width - 1)
            {
                t_positions.at(idx).neighborIndices.at(magic_enum::enum_integer(Direction::NE)) = (y - 1) * t_width + (x + 1);
            }

            if (y > 0 && x > 0)
            {
                t_positions.at(idx).neighborIndices.at(magic_enum::enum_integer(Direction::NW)) = (y - 1) * t_width + (x - 1);
            }

            if (y < t_height - 1 && x > 0)
            {
                t_positions.at(idx).neighborIndices.at(magic_enum::enum_integer(Direction::SW)) = (y + 1) * t_width + (x - 1);
            }

            if (y < t_height - 1 && x < t_width - 1)
            {
                t_positions.at(idx).neighborIndices.at(magic_enum::enum_integer(Direction::SE)) = (y + 1) * t_width + (x + 1);
            }
        }
    }
}

void mdcii::world::WorldGenerator2::SplitElevationsInWaterAndTerrain(std::vector<Position>& t_positions)
{
    if (t_positions.empty())
    {
        return;
    }

    Log::MDCII_LOG_DEBUG("[WorldGenerator2::SplitElevationsInWaterAndTerrain()] Split elevation values into water and terrain.");

    for (auto& position : t_positions)
    {
        position.IsElevationAboveWaterLevel() ? position.mapValue.value = MAP_TERRAIN : position.mapValue.value = MAP_WATER;
    }
}

void mdcii::world::WorldGenerator2::AddDefaultEmbankment(std::vector<Position>& t_positions)
{
    if (t_positions.empty())
    {
        return;
    }

    Log::MDCII_LOG_DEBUG("[WorldGenerator2::AddEmbankment()] Create embankment default values.");

    for (auto& position : t_positions)
    {
        if (position.mapValue.value == MAP_TERRAIN && IsMapTerrainPositionOnSeaSide(t_positions, position))
        {
            position.mapValue.value = MAP_BANK;
        }
    }
}

bool mdcii::world::WorldGenerator2::IsMapTerrainPositionOnSeaSide(const std::vector<Position>& t_positions, const Position& t_position)
{
    if (t_position.mapValue.value != MAP_TERRAIN)
    {
        return false;
    }

    return std::any_of(t_position.neighborIndices.begin(), t_position.neighborIndices.end(), [&t_positions](const int32_t t_idx) {
        return t_idx != -1 && t_positions.at(t_idx).mapValue.value == MAP_WATER;
    });
}

void mdcii::world::WorldGenerator2::AddDefaultBeach(std::vector<Position>& t_positions)
{
    if (t_positions.empty())
    {
        return;
    }

    Log::MDCII_LOG_DEBUG("[WorldGenerator2::AddEmbankment()] Create beach default values.");

    for (auto& position : t_positions)
    {
        if (position.mapValue.value == MAP_WATER && IsMapWaterPositionOnBankSide(t_positions, position))
        {
            position.mapValue.value = MAP_BEACH;
        }
    }
}

bool mdcii::world::WorldGenerator2::IsMapWaterPositionOnBankSide(const std::vector<Position>& t_positions, const Position& t_position)
{
    if (t_position.mapValue.value != MAP_WATER)
    {
        return false;
    }

    return std::any_of(t_position.neighborIndices.begin(), t_position.neighborIndices.end(), [&t_positions](const int32_t t_idx) {
        return t_idx != -1 && t_positions.at(t_idx).mapValue.value == MAP_BANK;
    });
}

void mdcii::world::WorldGenerator2::CreateEmbankmentNeighbors(std::vector<Position>& t_positions)
{
    if (t_positions.empty())
    {
        return;
    }

    Log::MDCII_LOG_DEBUG("[WorldGenerator2::CreateEmbankmentNeighbors()] Create embankment neighbor values.");

    // reset
    for (auto& position : t_positions)
    {
        position.mapValue.neighborFlag = 0;
    }

    // set neighbors
    for (auto& position : t_positions)
    {
        if (position.mapValue.value == MAP_BANK)
        {
            // north
            if (const auto n{ position.neighborIndices.at(magic_enum::enum_integer(Direction::N)) }; n != -1 && t_positions.at(n).mapValue.value == MAP_BANK)
            {
                position.mapValue.neighborFlag = NeighborFlag::NORTH;
            }

            // east
            if (const auto e{ position.neighborIndices.at(magic_enum::enum_integer(Direction::E)) }; e != -1 && t_positions.at(e).mapValue.value == MAP_BANK)
            {
                position.mapValue.neighborFlag |= NeighborFlag::EAST;
            }

            // south
            if (const auto s{ position.neighborIndices.at(magic_enum::enum_integer(Direction::S)) }; s != -1 && t_positions.at(s).mapValue.value == MAP_BANK)
            {
                position.mapValue.neighborFlag |= NeighborFlag::SOUTH;
            }

            // west
            if (const auto w{ position.neighborIndices.at(magic_enum::enum_integer(Direction::W)) }; w != -1 && t_positions.at(w).mapValue.value == MAP_BANK)
            {
                position.mapValue.neighborFlag |= NeighborFlag::WEST;
            }
        }
    }
}

void mdcii::world::WorldGenerator2::ValidateEmbankment(std::vector<Position>& t_positions)
{
    Log::MDCII_LOG_DEBUG("[WorldGenerator2::ValidateEmbankment()] Check for invalid embankment positions.");

    // remove invalid embankment positions
    while (RemoveInvalidEmbankment(t_positions))
    {
        AddDefaultEmbankment(t_positions);
        CreateEmbankmentNeighbors(t_positions);
    }

    // remove embankment positions with no neighbors
    for (auto& position : t_positions)
    {
        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 0)
        {
            position.mapValue.value = MAP_WATER;
        }
    }
}

bool mdcii::world::WorldGenerator2::RemoveInvalidEmbankment(std::vector<Position>& t_positions)
{
    for (auto& position : t_positions)
    {
        // one neighbor
        if (position.mapValue.neighborFlag == 1)
        {
            position.mapValue.value = MAP_WATER;
            return true;
        }

        if (position.mapValue.neighborFlag == 2)
        {
            position.mapValue.value = MAP_WATER;
            return true;
        }

        if (position.mapValue.neighborFlag == 4)
        {
            position.mapValue.value = MAP_WATER;
            return true;
        }

        if (position.mapValue.neighborFlag == 8)
        {
            position.mapValue.value = MAP_WATER;
            return true;
        }

        // some positions with three neighbors
        if (position.mapValue.neighborFlag == 7)
        {
            const auto e{ position.neighborIndices.at(magic_enum::enum_integer(Direction::E)) };
            if (e != -1 && t_positions.at(e).mapValue.neighborFlag != 12 && t_positions.at(e).mapValue.neighborFlag != 9)
            {
                position.mapValue.value = MAP_WATER;
                return true;
            }
        }

        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 11)
        {
            const auto n{ position.neighborIndices.at(magic_enum::enum_integer(Direction::N)) };
            if (n != -1 && t_positions.at(n).mapValue.neighborFlag != 12 && t_positions.at(n).mapValue.neighborFlag != 6)
            {
                position.mapValue.value = MAP_WATER;
                return true;
            }
        }

        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 13)
        {
            const auto w{ position.neighborIndices.at(magic_enum::enum_integer(Direction::W)) };
            if (w != -1 && t_positions.at(w).mapValue.neighborFlag != 6 && t_positions.at(w).mapValue.neighborFlag != 3)
            {
                position.mapValue.value = MAP_WATER;
                return true;
            }
        }

        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 14)
        {
            const auto s{ position.neighborIndices.at(magic_enum::enum_integer(Direction::S)) };
            if (s != -1 && t_positions.at(s).mapValue.neighborFlag != 9 && t_positions.at(s).mapValue.neighborFlag != 3)
            {
                position.mapValue.value = MAP_WATER;
                return true;
            }
        }

        // more than three neighbors
        if (position.mapValue.neighborFlag == 15)
        {
            position.mapValue.value = MAP_WATER;
            return true;
        }
    }

    return false;
}

void mdcii::world::WorldGenerator2::SetFinalEmbankment(std::vector<Position>& t_positions)
{
    if (t_positions.empty())
    {
        return;
    }

    Log::MDCII_LOG_DEBUG("[WorldGenerator2::AlignEmbankment()] Sets the final map value for each rotation of the embankment.");

    // set map Ids - brute force
    for (auto& position : t_positions)
    {
        // left - right - top - bottom
        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 5)
        {
            const auto e{ position.neighborIndices.at(magic_enum::enum_integer(Direction::E)) };
            const auto w{ position.neighborIndices.at(magic_enum::enum_integer(Direction::W)) };

            if (e != -1 && t_positions.at(e).mapValue.value == MAP_TERRAIN)
            {
                position.mapValue.value = MAP_BANK_ROT1;
            }
            if (w != -1 && t_positions.at(w).mapValue.value == MAP_TERRAIN)
            {
                position.mapValue.value = MAP_BANK_ROT3;
            }
        }

        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 10)
        {
            const auto n{ position.neighborIndices.at(magic_enum::enum_integer(Direction::N)) };
            const auto s{ position.neighborIndices.at(magic_enum::enum_integer(Direction::S)) };

            if (n != -1 && t_positions.at(n).mapValue.value == MAP_TERRAIN)
            {
                position.mapValue.value = MAP_BANK_ROT0;
            }
            if (s != -1 && t_positions.at(s).mapValue.value == MAP_TERRAIN)
            {
                position.mapValue.value = MAP_BANK_ROT2;
            }
        }

        // corners - inside
        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 6)
        {
            const auto w{ position.neighborIndices.at(magic_enum::enum_integer(Direction::W)) };

            if (w != -1 && t_positions.at(w).mapValue.value == MAP_TERRAIN)
            {
                position.mapValue.value = MAP_BANK_CORNER_INSIDE_ROT1;
            }
        }

        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 9)
        {
            const auto e{ position.neighborIndices.at(magic_enum::enum_integer(Direction::E)) };

            if (e != -1 && t_positions.at(e).mapValue.value == MAP_TERRAIN)
            {
                position.mapValue.value = MAP_BANK_CORNER_INSIDE_ROT3;
            }
        }

        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 3)
        {
            const auto w{ position.neighborIndices.at(magic_enum::enum_integer(Direction::W)) };

            if (w != -1 && t_positions.at(w).mapValue.value == MAP_TERRAIN)
            {
                position.mapValue.value = MAP_BANK_CORNER_INSIDE_ROT0;
            }
        }

        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 12)
        {
            const auto e{ position.neighborIndices.at(magic_enum::enum_integer(Direction::E)) };

            if (e != -1 && t_positions.at(e).mapValue.value == MAP_TERRAIN)
            {
                position.mapValue.value = MAP_BANK_CORNER_INSIDE_ROT2;
            }
        }

        // corners - inside / three neighbors
        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 7)
        {
            const auto e{ position.neighborIndices.at(magic_enum::enum_integer(Direction::E)) };

            /*
             * 7 12
             */
            if (e != -1 && t_positions.at(e).mapValue.neighborFlag == 12)
            {
                position.mapValue.value = MAP_BANK_CORNER_INSIDE_ROT0;
            }

            /*
             * 7 9
             */
            if (e != -1 && t_positions.at(e).mapValue.neighborFlag == 9)
            {
                position.mapValue.value = MAP_BANK_CORNER_INSIDE_ROT1;
            }
        }

        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 11)
        {
            const auto n{ position.neighborIndices.at(magic_enum::enum_integer(Direction::N)) };

            /*
             * 12
             * 11
             */
            if (n != -1 && t_positions.at(n).mapValue.neighborFlag == 12)
            {
                position.mapValue.value = MAP_BANK_CORNER_INSIDE_ROT0;
            }

            /*
             *  6
             * 11
             */
            if (n != -1 && t_positions.at(n).mapValue.neighborFlag == 6)
            {
                position.mapValue.value = MAP_BANK_CORNER_INSIDE_ROT3;
            }
        }

        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 13)
        {
            const auto w{ position.neighborIndices.at(magic_enum::enum_integer(Direction::W)) };

            /*
             * 6 13
             */
            if (w != -1 && t_positions.at(w).mapValue.neighborFlag == 6)
            {
                position.mapValue.value = MAP_BANK_CORNER_INSIDE_ROT3;
            }

            /*
             * 3 13
             */
            if (w != -1 && t_positions.at(w).mapValue.neighborFlag == 3)
            {
                position.mapValue.value = MAP_BANK_CORNER_INSIDE_ROT2;
            }
        }

        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 14)
        {
            const auto s{ position.neighborIndices.at(magic_enum::enum_integer(Direction::S)) };

            /*
             * 14
             *  9
             */
            if (s != -1 && t_positions.at(s).mapValue.neighborFlag == 9)
            {
                position.mapValue.value = MAP_BANK_CORNER_INSIDE_ROT1;
            }

            /*
             * 14
             *  3
             */
            if (s != -1 && t_positions.at(s).mapValue.neighborFlag == 3)
            {
                position.mapValue.value = MAP_BANK_CORNER_INSIDE_ROT2;
            }
        }

        // corners
        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 12)
        {
            const auto sw{ position.neighborIndices.at(magic_enum::enum_integer(Direction::SW)) };
            if (sw != -1 && t_positions.at(sw).mapValue.value == MAP_TERRAIN || t_positions.at(sw).mapValue.value == MAP_BANK)
            {
                position.mapValue.value = MAP_BANK_CORNER_ROT2;
            }
        }

        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 9)
        {
            const auto nw{ position.neighborIndices.at(magic_enum::enum_integer(Direction::NW)) };
            if (nw != -1 && t_positions.at(nw).mapValue.value == MAP_TERRAIN || t_positions.at(nw).mapValue.value == MAP_BANK)
            {
                position.mapValue.value = MAP_BANK_CORNER_ROT3;
            }
        }

        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 3)
        {
            const auto ne{ position.neighborIndices.at(magic_enum::enum_integer(Direction::NE)) };
            if (ne != -1 && t_positions.at(ne).mapValue.value == MAP_TERRAIN || t_positions.at(ne).mapValue.value == MAP_BANK)
            {
                position.mapValue.value = MAP_BANK_CORNER_ROT0;
            }
        }

        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 6)
        {
            const auto se{ position.neighborIndices.at(magic_enum::enum_integer(Direction::SE)) };
            if (se != -1 && t_positions.at(se).mapValue.value == MAP_TERRAIN || t_positions.at(se).mapValue.value == MAP_BANK)
            {
                position.mapValue.value = MAP_BANK_CORNER_ROT1;
            }
        }
    }
}

//-------------------------------------------------
// ImGui
//-------------------------------------------------

void mdcii::world::WorldGenerator2::RenderLegendImGui()
{
    ImGui::Separator();
    ImGui::Text("Map values");
    ImGui::Separator();

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
    ImGui::Text("~ Shallow water");
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
    ImGui::Text("* Island");
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 0, 255));
    ImGui::Text("# Embankment");
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 200, 200, 255));
    ImGui::Text("= Beach");
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
    ImGui::Text("? Temp invalid");
    ImGui::PopStyleColor();

    ImGui::Separator();
    ImGui::Text("Map neighbor values");
    ImGui::Separator();

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 0, 255));
    ImGui::Text("0 ... 15 Embankment with neighbor flag");
    ImGui::Text("  0 - no neighbors");
    ImGui::Text("  1 - north");
    ImGui::Text("  2 - east");
    ImGui::Text("  3 - north && east");
    ImGui::Text("  4 - south");
    ImGui::Text("  5 - south && north");
    ImGui::Text("  6 - south && east");
    ImGui::Text("  7 - north && east && south");
    ImGui::Text("  8 - west");
    ImGui::Text("  9 - west && north");
    ImGui::Text("  10 - west && east");
    ImGui::Text("  11 - west && east && north");
    ImGui::Text("  12 - west && south");
    ImGui::Text("  13 - west && south && north");
    ImGui::Text("  14 - west && south && east");
    ImGui::Text("  15 - all cross");
    ImGui::PopStyleColor();

    ImGui::Separator();
}

void mdcii::world::WorldGenerator2::RenderElevationValuesImGui(const std::vector<Position>& t_positions, const int32_t t_width, const int32_t t_height)
{
    ImGui::Begin("Elevation values");
    if (t_positions.empty())
    {
        ImGui::Text("No elevation values available");
    }
    else
    {
        for (auto y{ 0 }; y < t_height; ++y)
        {
            for (auto x{ 0 }; x < t_width; ++x)
            {
                const auto& position{ t_positions.at(GetIndex(x, y, t_width)) };
                if (position.IsElevationAboveWaterLevel())
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                    ImGui::Text("%.2f", position.elevation);
                    ImGui::SameLine();
                    ImGui::PopStyleColor();
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
                    ImGui::Text("%.2f", position.elevation);
                    ImGui::SameLine();
                    ImGui::PopStyleColor();
                }
            }

            ImGui::Text("");
        }
    }

    ImGui::End();
}

void mdcii::world::WorldGenerator2::RenderMapValuesImGui(const std::vector<Position>& t_positions, const int32_t t_width, const int32_t t_height)
{
    ImGui::Begin("Map values");
    if (t_positions.empty())
    {
        ImGui::Text("No map values available");
    }
    else
    {
        for (auto y{ 0 }; y < t_height; ++y)
        {
            for (auto x{ 0 }; x < t_width; ++x)
            {
                switch (t_positions.at(GetIndex(x, y, t_width)).mapValue.value)
                {
                case MAP_WATER:
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
                    ImGui::Text("~");
                    ImGui::SameLine();
                    ImGui::PopStyleColor();
                    break;
                case MAP_TERRAIN:
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                    ImGui::Text("*");
                    ImGui::SameLine();
                    ImGui::PopStyleColor();
                    break;
                case MAP_BANK:
                case MAP_BANK_ROT0:
                case MAP_BANK_ROT1:
                case MAP_BANK_ROT2:
                case MAP_BANK_ROT3:
                case MAP_BANK_CORNER_INSIDE_ROT0:
                case MAP_BANK_CORNER_INSIDE_ROT1:
                case MAP_BANK_CORNER_INSIDE_ROT2:
                case MAP_BANK_CORNER_INSIDE_ROT3:
                case MAP_BANK_CORNER_ROT0:
                case MAP_BANK_CORNER_ROT1:
                case MAP_BANK_CORNER_ROT2:
                case MAP_BANK_CORNER_ROT3:
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 0, 255));
                    ImGui::Text("#");
                    ImGui::SameLine();
                    ImGui::PopStyleColor();
                    break;
                case MAP_BEACH:
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 200, 200, 255));
                    ImGui::Text("=");
                    ImGui::SameLine();
                    ImGui::PopStyleColor();
                    break;

                default:
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
                    ImGui::Text("?");
                    ImGui::SameLine();
                    ImGui::PopStyleColor();
                    break;
                }
            }

            ImGui::Text("");
        }
    }

    ImGui::End();
}

void mdcii::world::WorldGenerator2::RenderMapNeighborValuesImGui(const std::vector<Position>& t_positions, const int32_t t_width, const int32_t t_height)
{
    ImGui::Begin("Map neighbor values");
    if (t_positions.empty())
    {
        ImGui::Text("No map neighbor values available");
    }
    else
    {
        for (auto y{ 0 }; y < t_height; ++y)
        {
            for (auto x{ 0 }; x < t_width; ++x)
            {
                switch (t_positions.at(GetIndex(x, y, t_width)).mapValue.value)
                {
                case MAP_WATER:
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
                    ImGui::Text(" ~");
                    ImGui::SameLine();
                    ImGui::PopStyleColor();
                    break;
                case MAP_TERRAIN:
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                    ImGui::Text(" *");
                    ImGui::SameLine();
                    ImGui::PopStyleColor();
                    break;
                case MAP_BANK:
                case MAP_BANK_ROT0:
                case MAP_BANK_ROT1:
                case MAP_BANK_ROT2:
                case MAP_BANK_ROT3:
                case MAP_BANK_CORNER_INSIDE_ROT0:
                case MAP_BANK_CORNER_INSIDE_ROT1:
                case MAP_BANK_CORNER_INSIDE_ROT2:
                case MAP_BANK_CORNER_INSIDE_ROT3:
                case MAP_BANK_CORNER_ROT0:
                case MAP_BANK_CORNER_ROT1:
                case MAP_BANK_CORNER_ROT2:
                case MAP_BANK_CORNER_ROT3:
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 0, 255));
                    t_positions.at(GetIndex(x, y, t_width)).mapValue.neighborFlag  < 10 ?
                        ImGui::Text(" %d", t_positions.at(GetIndex(x, y, t_width)).mapValue.neighborFlag) :
                        ImGui::Text("%d", t_positions.at(GetIndex(x, y, t_width)).mapValue.neighborFlag);
                    ImGui::SameLine();
                    ImGui::PopStyleColor();
                    break;
                default:
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
                    ImGui::Text(" ?");
                    ImGui::SameLine();
                    ImGui::PopStyleColor();
                    break;
                }
            }

            ImGui::Text("");
        }
    }

    ImGui::End();
}

//-------------------------------------------------
// Json
//-------------------------------------------------

void mdcii::world::WorldGenerator2::AddWorldValues(nlohmann::json& t_j, const int32_t t_worldWidth, const int32_t t_worldHeight)
{
    Log::MDCII_LOG_DEBUG("[WorldGenerator2::AddWorldValues()] Adds world Json values.");

    t_j["world"] = { { "width", t_worldWidth }, { "height", t_worldHeight } };
}

void mdcii::world::WorldGenerator2::AddIslandValues(
    nlohmann::json& t_j,
    const std::vector<Position>& t_positions,
    const int32_t t_width,
    const int32_t t_height,
    const int32_t t_worldX,
    const int32_t t_worldY,
    const bool t_south
)
{
    Log::MDCII_LOG_DEBUG("[WorldGenerator2::AddIslandValues()] Adds Json values for an new island.");

    std::vector<std::shared_ptr<layer::Tile>> coastTiles;
    std::vector<std::shared_ptr<layer::Tile>> terrainTiles;
    std::vector<std::shared_ptr<layer::Tile>> buildingsTiles;

    nlohmann::json c = nlohmann::json::object();
    nlohmann::json t = nlohmann::json::object();
    nlohmann::json b = nlohmann::json::object();
    nlohmann::json i = nlohmann::json::object();

    i["width"] = t_width;
    i["height"] = t_height;
    i["x"] = t_worldX;
    i["y"] = t_worldY;
    i["layers"] = nlohmann::json::array();

    CreateTerrainTiles(terrainTiles, t_positions, t_width, t_height, t_south);
    CreateCoastTiles(coastTiles, t_positions, t_width, t_height);
    CreateBuildingsTiles(buildingsTiles, t_positions, t_width, t_height);
    c["coast"] = coastTiles;
    t["terrain"] = terrainTiles;
    b["buildings"] = buildingsTiles;

    i["layers"].push_back(c);
    i["layers"].push_back(t);
    i["layers"].push_back(b);

    t_j["islands"].push_back(i);
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

int32_t mdcii::world::WorldGenerator2::GetIndex(const int32_t t_x, const int32_t t_y, const int32_t t_width)
{
    return t_y * t_width + t_x;
}

std::unique_ptr<mdcii::layer::Tile> mdcii::world::WorldGenerator2::CreateTile(const int32_t t_id, const int32_t t_worldX, const int32_t t_worldY, const Rotation t_rotation)
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
// Create Tiles
//-------------------------------------------------

void mdcii::world::WorldGenerator2::CreateTerrainTiles(
    std::vector<std::shared_ptr<layer::Tile>>& t_terrainTiles,
    const std::vector<Position>& t_positions,
    const int32_t t_width,
    const int32_t t_height,
    const bool t_south
)
{
    Log::MDCII_LOG_DEBUG("[WorldGenerator2::CreateTerrainTiles()] Create terrain tiles.");

    for (auto y{ 0 }; y < t_height; ++y)
    {
        for (auto x{ 0 }; x < t_width; ++x)
        {
            t_terrainTiles.emplace_back(std::make_unique<layer::Tile>());
        }
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution randInRange(0, 10);

    for (auto y{ 0 }; y < t_height; ++y)
    {
        for (auto x{ 0 }; x < t_width; ++x)
        {
            const auto idx{ GetIndex(x, y, t_width) };
            const auto val{ t_positions.at(idx).mapValue.value };

            if (val == MAP_TERRAIN)
            {
                auto id{ data::GRASS_BUILDING_ID };
                if (t_south)
                {
                    if (const auto r{ randInRange(gen) }; r >= 7)
                    {
                        id = data::SOUTH_TREES.at(randInRange(gen));
                    }
                }
                else
                {
                    if (const auto r{ randInRange(gen) }; r >= 7)
                    {
                        id = data::NORTH_TREES.at(randInRange(gen));
                    }
                }

                t_terrainTiles.at(idx) = CreateTile(id, x, y, Rotation::DEG0);
            }

            if (val == MAP_BANK_ROT0)
            {
                t_terrainTiles.at(idx) = CreateTile(data::BANK_BUILDING_ID, x, y, Rotation::DEG0);
            }

            if (val == MAP_BANK_ROT1)
            {
                t_terrainTiles.at(idx) = CreateTile(data::BANK_BUILDING_ID, x, y, Rotation::DEG90);
            }

            if (val == MAP_BANK_ROT2)
            {
                t_terrainTiles.at(idx) = CreateTile(data::BANK_BUILDING_ID, x, y, Rotation::DEG180);
            }

            if (val == MAP_BANK_ROT3)
            {
                t_terrainTiles.at(idx) = CreateTile(data::BANK_BUILDING_ID, x, y, Rotation::DEG270);
            }

            if (val == MAP_BANK_CORNER_INSIDE_ROT0)
            {
                t_terrainTiles.at(idx) = CreateTile(data::BANK_CORNER_INSIDE_BUILDING_ID, x, y, Rotation::DEG0);
            }

            if (val == MAP_BANK_CORNER_INSIDE_ROT1)
            {
                t_terrainTiles.at(idx) = CreateTile(data::BANK_CORNER_INSIDE_BUILDING_ID, x, y, Rotation::DEG90);
            }

            if (val == MAP_BANK_CORNER_INSIDE_ROT2)
            {
                t_terrainTiles.at(idx) = CreateTile(data::BANK_CORNER_INSIDE_BUILDING_ID, x, y, Rotation::DEG180);
            }

            if (val == MAP_BANK_CORNER_INSIDE_ROT3)
            {
                t_terrainTiles.at(idx) = CreateTile(data::BANK_CORNER_INSIDE_BUILDING_ID, x, y, Rotation::DEG270);
            }

            if (val == MAP_BANK_CORNER_ROT0)
            {
                t_terrainTiles.at(idx) = CreateTile(data::BANK_CORNER_BUILDING_ID, x, y, Rotation::DEG0);
            }

            if (val == MAP_BANK_CORNER_ROT1)
            {
                t_terrainTiles.at(idx) = CreateTile(data::BANK_CORNER_BUILDING_ID, x, y, Rotation::DEG90);
            }

            if (val == MAP_BANK_CORNER_ROT2)
            {
                t_terrainTiles.at(idx) = CreateTile(data::BANK_CORNER_BUILDING_ID, x, y, Rotation::DEG180);
            }

            if (val == MAP_BANK_CORNER_ROT3)
            {
                t_terrainTiles.at(idx) = CreateTile(data::BANK_CORNER_BUILDING_ID, x, y, Rotation::DEG270);
            }

            /*
            if (val == MAP_BEACH)
            {
                t_terrainTiles.at(idx) = CreateTile(1205, x, y, Rotation::DEG0);
            }
            */
        }
    }
}

void mdcii::world::WorldGenerator2::CreateCoastTiles(
    std::vector<std::shared_ptr<layer::Tile>>& t_coastTiles,
    const std::vector<Position>& t_positions,
    const int32_t t_width,
    const int32_t t_height
)
{
    Log::MDCII_LOG_DEBUG("[WorldGenerator2::CreateCoastTiles()] Create coast tiles.");

    for (auto y{ 0 }; y < t_height; ++y)
    {
        for (auto x{ 0 }; x < t_width; ++x)
        {
            t_coastTiles.emplace_back(std::make_unique<layer::Tile>());
        }
    }

    for (auto y{ 0 }; y < t_height; ++y)
    {
        for (auto x{ 0 }; x < t_width; ++x)
        {
            const auto idx{ GetIndex(x, y, t_width) };
            const auto val{ t_positions.at(idx).mapValue.value };

            if (val == MAP_WATER)
            {
                t_coastTiles.at(idx) = CreateTile(data::SHALLOW_WATER_BUILDING_ID, x, y, Rotation::DEG0);
            }
        }
    }
}

void mdcii::world::WorldGenerator2::CreateBuildingsTiles(
    std::vector<std::shared_ptr<layer::Tile>>& t_buildingsTiles,
    const std::vector<Position>& t_positions,
    const int32_t t_width,
    const int32_t t_height
)
{
    Log::MDCII_LOG_DEBUG("[WorldGenerator2::CreateBuildingsTiles()] Create building tiles.");

    for (auto y{ 0 }; y < t_height; ++y)
    {
        for (auto x{ 0 }; x < t_width; ++x)
        {
            t_buildingsTiles.emplace_back(std::make_unique<layer::Tile>());
        }
    }
}

//-------------------------------------------------
// File
//-------------------------------------------------

bool mdcii::world::WorldGenerator2::CreateFile(const std::string& t_fileName)
{
    const auto fileName{ Game::RESOURCES_REL_PATH + "map/" + t_fileName + ".map" };
    if (std::filesystem::exists(fileName))
    {
        Log::MDCII_LOG_WARN("[WorldGenerator2::CreateFile()] The {} file already exists.", fileName);

        return false;
    }

    Log::MDCII_LOG_DEBUG("[WorldGenerator2::CreateFile()] Create new file {}.", t_fileName);

    m_file.open(fileName);
    if (!m_file.is_open())
    {
        throw MDCII_EXCEPTION("[WorldGenerator2::CreateFile()] Error while opening file " + fileName + ".");
    }

    return true;
}

void mdcii::world::WorldGenerator2::WriteJsonToFile(const nlohmann::json& t_j)
{
    MDCII_ASSERT(m_file.is_open(), "[WorldGenerator2::WriteJsonToFile()] File open error.")

    m_file << t_j;

    Log::MDCII_LOG_DEBUG("[WorldGenerator2::WriteJsonToFile()] Json successfully written to file.");
}
