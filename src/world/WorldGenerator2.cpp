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
#include "WorldGenerator2.h"
#include "World.h"
#include "MdciiAssert.h"
#include "Game.h"
#include "layer/TerrainLayer.h"
#include "data/BuildingIds.h"

// todo
#define CREATE_FILE

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::WorldGenerator2::WorldGenerator2()
{
    Log::MDCII_LOG_DEBUG("[WorldGenerator2::WorldGenerator2()] Create WorldGenerator2.");

    Init();
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
    static int32_t islandWidth{ 20 };
    static int32_t islandHeight{ 20 };
    static int32_t seed{ 2504 };        // 2504
    static float frequency{ 0.027f };   // 0.027

    static int32_t worldWidth{ World::WORLD_MIN_WIDTH };
    static int32_t worldHeight{ World::WORLD_MIN_HEIGHT };

    ImGui::Separator();
    ImGui::SliderInt("World width ", &worldWidth, World::WORLD_MIN_WIDTH, World::WORLD_MAX_WIDTH);
    ImGui::SliderInt("World height", &worldHeight, World::WORLD_MIN_HEIGHT, World::WORLD_MAX_HEIGHT);
    ImGui::Separator();
    ImGui::Checkbox("South", &m_south);
    ImGui::SliderInt("Island width", &islandWidth, 20, 60);
    ImGui::SliderInt("Island height", &islandHeight, 20, 60);
    ImGui::SliderInt("Seed", &seed, 100, 9000);
    ImGui::SliderFloat("Frequency", &frequency, 0.01f, 0.3f);
    ImGui::Separator();

    RenderLegendImGui();

    if (ImGui::Button("Create new map"))
    {
        CreateElevations(seed, frequency, islandWidth, islandHeight);
        StoreNeighbors();
        SplitElevationsInWaterAndTerrain();
        AddEmbankment();
        AlignEmbankment();
    }

#ifdef CREATE_FILE
    if (ImGui::Button("Save map"))
    {
        nlohmann::json j;

        AddWorldValues(j, worldWidth, worldHeight);
        AddIslandValues(j, 1, 1);

        m_file << j;
    }
#endif

    RenderElevationsImGui();
    RenderMapImGui();
}

//-------------------------------------------------
// Create
//-------------------------------------------------

void mdcii::world::WorldGenerator2::CreateElevations(const int32_t t_seed, const float t_frequency, const int32_t t_width, const int32_t t_height)
{
    m_fastNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    m_fastNoise.SetSeed(t_seed);
    m_fastNoise.SetFrequency(t_frequency);

    // update width and height
    m_map.width = t_width;
    m_map.height = t_height;

    // map positions should be empty
    if (!m_map.positions.empty())
    {
        std::vector<Position>().swap(m_map.positions);
    }
    MDCII_ASSERT(m_map.positions.empty(), "[WorldGenerator2::CreateElevations()] Empty array expected.")

    Log::MDCII_LOG_DEBUG("[WorldGenerator2::CreateElevations()] Create new elevations for a ({}, {}) map.", t_width, t_height);

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

            auto e{ m_fastNoise.GetNoise(static_cast<double>(x), static_cast<double>(y)) / 2.0 + 0.5 };

            const auto lerp = [](const auto t_x, const auto t_y, const auto t_t) {
                return t_x * (1.0 - t_t) + t_y * t_t;
            };
            e = lerp(e, 1.0 - d, Position::WATER_LEVEL);

            Position position{ x, y, e, {}, { -1, -1, -1, -1, -1, -1, -1, -1 }};
            m_map.positions.push_back(position);
        }
    }
}

void mdcii::world::WorldGenerator2::StoreNeighbors()
{
    Log::MDCII_LOG_DEBUG("[WorldGenerator2::StoreNeighbors()] Stores the neighbor indices for each position.");

    for (auto y{ 0 }; y < m_map.height; ++y)
    {
        for (auto x{ 0 }; x < m_map.width; ++x)
        {
            const auto idx{ GetIndex(x, y, m_map.width) };

            // regular grid
            if (y > 0)
            {
                m_map.positions.at(idx).neighborIndices.at(magic_enum::enum_integer(Direction::N)) = (y - 1) * m_map.width + x;
            }

            if (y < m_map.height - 1)
            {
                m_map.positions.at(idx).neighborIndices.at(magic_enum::enum_integer(Direction::S)) = (y + 1) * m_map.width + x;
            }

            if (x > 0)
            {
                m_map.positions.at(idx).neighborIndices.at(magic_enum::enum_integer(Direction::W)) = y * m_map.width + (x - 1);
            }

            if (x < m_map.width - 1)
            {
                m_map.positions.at(idx).neighborIndices.at(magic_enum::enum_integer(Direction::E)) = y * m_map.width + (x + 1);
            }

            // connect diagonally
            if (y > 0 && x < m_map.width - 1)
            {
                m_map.positions.at(idx).neighborIndices.at(magic_enum::enum_integer(Direction::NE)) = (y - 1) * m_map.width + (x + 1);
            }

            if (y > 0 && x > 0)
            {
                m_map.positions.at(idx).neighborIndices.at(magic_enum::enum_integer(Direction::NW)) = (y - 1) * m_map.width + (x - 1);
            }

            if (y < m_map.height - 1 && x > 0)
            {
                m_map.positions.at(idx).neighborIndices.at(magic_enum::enum_integer(Direction::SW)) = (y + 1) * m_map.width + (x - 1);
            }

            if (y < m_map.height - 1 && x < m_map.width - 1)
            {
                m_map.positions.at(idx).neighborIndices.at(magic_enum::enum_integer(Direction::SE)) = (y + 1) * m_map.width + (x + 1);
            }
        }
    }
}

void mdcii::world::WorldGenerator2::RenderLegendImGui()
{
    ImGui::Text("Legend");
    ImGui::Separator();

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
    ImGui::Text("~ Shallow water");
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
    ImGui::Text("* Island");
    ImGui::PopStyleColor();

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

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
    ImGui::Text("? Temp invalid");
    ImGui::PopStyleColor();

    ImGui::Separator();
    ImGui::Text("1. Set values");
    ImGui::Text("2. Press Create new map");
    ImGui::Text("3. Press Save map");
    ImGui::Text("4. Load the map via Main Menu");
    ImGui::Text("   - Start a new game -");

    ImGui::Separator();
}

void mdcii::world::WorldGenerator2::RenderElevationsImGui() const
{
    ImGui::Begin("Elevations");
    if (!m_map.positions.empty())
    {
        for (auto y{ 0 }; y < m_map.height; ++y)
        {
            for (auto x{ 0 }; x < m_map.width; ++x)
            {
                const auto position{ m_map.positions.at(GetIndex(x, y, m_map.width)) };
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

void mdcii::world::WorldGenerator2::SplitElevationsInWaterAndTerrain()
{
    MDCII_ASSERT(!m_map.positions.empty(), "[WorldGenerator2::SplitElevationsInWaterAndTerrain()] Missing elevation values.")

    Log::MDCII_LOG_DEBUG("[WorldGenerator2::SplitElevationsInWaterAndTerrain()] Split elevation values into water and terrain.");

    for (auto& position : m_map.positions)
    {
        position.IsElevationAboveWaterLevel() ? position.mapValue.value = MAP_TERRAIN : position.mapValue.value = MAP_WATER;
    }
}

void mdcii::world::WorldGenerator2::AddEmbankment()
{
    MDCII_ASSERT(!m_map.positions.empty(), "[WorldGenerator2::AddEmbankment()] Missing elevation values.")

    Log::MDCII_LOG_DEBUG("[WorldGenerator2::AddEmbankment()] Create embankment default values.");

    for (auto& position : m_map.positions)
    {
        // if a position is on land and a neighbor is water, set default embankment for the position
        if (position.mapValue.value == MAP_TERRAIN && IsMapTerrainPositionOnSeaSide(position))
        {
            position.mapValue.value = MAP_BANK;
        }
    }
}

void mdcii::world::WorldGenerator2::AlignEmbankment()
{
    MDCII_ASSERT(!m_map.positions.empty(), "[WorldGenerator2::RotateEmbankment()] Missing elevation values.")

    Log::MDCII_LOG_DEBUG("[WorldGenerator2::RotateEmbankment()] Sets a different map value for each rotation of the embankment.");

    // set neighbors, so we have a "snake" of embankment positions
    for (auto& position : m_map.positions)
    {
        if (position.mapValue.value == MAP_BANK)
        {
            // north
            const auto n{ position.neighborIndices.at(magic_enum::enum_integer(Direction::N)) };
            if (n != -1 && m_map.positions.at(n).mapValue.value == MAP_BANK)
            {
                position.mapValue.neighborFlag = NeighborFlag::NORTH;
            }

            // east
            const auto e{ position.neighborIndices.at(magic_enum::enum_integer(Direction::E)) };
            if (e != -1 && m_map.positions.at(e).mapValue.value == MAP_BANK)
            {
                position.mapValue.neighborFlag |= NeighborFlag::EAST;
            }

            // south
            const auto s{ position.neighborIndices.at(magic_enum::enum_integer(Direction::S)) };
            if (s != -1 && m_map.positions.at(s).mapValue.value == MAP_BANK)
            {
                position.mapValue.neighborFlag |= NeighborFlag::SOUTH;
            }

            // west
            const auto w{ position.neighborIndices.at(magic_enum::enum_integer(Direction::W)) };
            if (w != -1 && m_map.positions.at(w).mapValue.value == MAP_BANK)
            {
                position.mapValue.neighborFlag |= NeighborFlag::WEST;
            }
        }
    }

    // set map Ids - brute force
    for (auto& position : m_map.positions)
    {
        // left - right - top - bottom
        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 5)
        {
            const auto e{ position.neighborIndices.at(magic_enum::enum_integer(Direction::E)) };
            const auto w{ position.neighborIndices.at(magic_enum::enum_integer(Direction::W)) };

            if (e != -1 && m_map.positions.at(e).mapValue.value == MAP_TERRAIN)
            {
                position.mapValue.value = MAP_BANK_ROT1;
            }
            if (w != -1 && m_map.positions.at(w).mapValue.value == MAP_TERRAIN)
            {
                position.mapValue.value = MAP_BANK_ROT3;
            }
        }

        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 10)
        {
            const auto n{ position.neighborIndices.at(magic_enum::enum_integer(Direction::N)) };
            const auto s{ position.neighborIndices.at(magic_enum::enum_integer(Direction::S)) };

            if (n != -1 && m_map.positions.at(n).mapValue.value == MAP_TERRAIN)
            {
                position.mapValue.value = MAP_BANK_ROT0;
            }
            if (s != -1 && m_map.positions.at(s).mapValue.value == MAP_TERRAIN)
            {
                position.mapValue.value = MAP_BANK_ROT2;
            }
        }

        // corners - inside
        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 6)
        {
            const auto w{ position.neighborIndices.at(magic_enum::enum_integer(Direction::W)) };

            if (w != -1 && m_map.positions.at(w).mapValue.value == MAP_TERRAIN)
            {
                position.mapValue.value = MAP_BANK_CORNER_INSIDE_ROT1;
            }
        }

        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 9)
        {
            const auto e{ position.neighborIndices.at(magic_enum::enum_integer(Direction::E)) };

            if (e != -1 && m_map.positions.at(e).mapValue.value == MAP_TERRAIN)
            {
                position.mapValue.value = MAP_BANK_CORNER_INSIDE_ROT3;
            }
        }

        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 3)
        {
            const auto w{ position.neighborIndices.at(magic_enum::enum_integer(Direction::W)) };

            if (w != -1 && m_map.positions.at(w).mapValue.value == MAP_TERRAIN)
            {
                position.mapValue.value = MAP_BANK_CORNER_INSIDE_ROT0;
            }
        }

        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 12)
        {
            const auto e{ position.neighborIndices.at(magic_enum::enum_integer(Direction::E)) };

            if (e != -1 && m_map.positions.at(e).mapValue.value == MAP_TERRAIN)
            {
                position.mapValue.value = MAP_BANK_CORNER_INSIDE_ROT2;
            }
        }

        // corners
        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 12)
        {
            const auto e{ position.neighborIndices.at(magic_enum::enum_integer(Direction::E)) };
            if (e != -1 && m_map.positions.at(e).mapValue.value == MAP_WATER)
            {
                position.mapValue.value = MAP_BANK_CORNER_ROT2;
            }
        }

        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 9)
        {
            const auto e{ position.neighborIndices.at(magic_enum::enum_integer(Direction::E)) };
            if (e != -1 && m_map.positions.at(e).mapValue.value == MAP_WATER)
            {
                position.mapValue.value = MAP_BANK_CORNER_ROT3;
            }
        }

        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 3)
        {
            const auto w{ position.neighborIndices.at(magic_enum::enum_integer(Direction::W)) };
            if (w != -1 && m_map.positions.at(w).mapValue.value == MAP_WATER)
            {
                position.mapValue.value = MAP_BANK_CORNER_ROT0;
            }
        }

        if (position.mapValue.value == MAP_BANK && position.mapValue.neighborFlag == 6)
        {
            const auto w{ position.neighborIndices.at(magic_enum::enum_integer(Direction::W)) };
            if (w != -1 && m_map.positions.at(w).mapValue.value == MAP_WATER)
            {
                position.mapValue.value = MAP_BANK_CORNER_ROT1;
            }
        }
    }
}

void mdcii::world::WorldGenerator2::RenderMapImGui() const
{
    ImGui::Begin("Map");
    if (!m_map.positions.empty())
    {
        for (auto y{ 0 }; y < m_map.height; ++y)
        {
            for (auto x{ 0 }; x < m_map.width; ++x)
            {
                switch (m_map.positions.at(GetIndex(x, y, m_map.width)).mapValue.value)
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
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 0, 255));
                    m_map.positions.at(GetIndex(x, y, m_map.width)).mapValue.neighborFlag  < 10 ?
                        ImGui::Text(" %d", m_map.positions.at(GetIndex(x, y, m_map.width)).mapValue.neighborFlag) :
                        ImGui::Text("%d", m_map.positions.at(GetIndex(x, y, m_map.width)).mapValue.neighborFlag);
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

void mdcii::world::WorldGenerator2::AddIslandValues(nlohmann::json& t_j, const int32_t t_worldX, const int32_t t_worldY)
{
    Log::MDCII_LOG_DEBUG("[WorldGenerator2::AddIslandValues()] Adds Json values for an new island.");

    std::vector<std::shared_ptr<layer::Tile>> coastTiles;
    std::vector<std::shared_ptr<layer::Tile>> terrainTiles;
    std::vector<std::shared_ptr<layer::Tile>> buildingsTiles;

    nlohmann::json c = nlohmann::json::object();
    nlohmann::json t = nlohmann::json::object();
    nlohmann::json b = nlohmann::json::object();
    nlohmann::json i = nlohmann::json::object();

    i["width"] = m_map.width;
    i["height"] = m_map.height;
    i["x"] = t_worldX;
    i["y"] = t_worldY;
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

    t_j["islands"].push_back(i);
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::world::WorldGenerator2::Init()
{
    Log::MDCII_LOG_DEBUG("[WorldGenerator2::Init()] Initializing world generator...");

#ifdef CREATE_FILE
    const auto fileName{ Game::RESOURCES_REL_PATH + "data/NoiseWorld.json" };
    if (std::filesystem::exists(fileName))
    {
        Log::MDCII_LOG_WARN("[WorldGenerator2::Init()] The {} file already exists.", fileName);

        return;
    }

    m_file.open(fileName);
    if (!m_file.is_open())
    {
        throw MDCII_EXCEPTION("[WorldGenerator2::Init()] Error while opening file " + fileName + ".");
    }
#endif

    Log::MDCII_LOG_DEBUG("[WorldGenerator2::Init()] The world generator was successfully initialized.");
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

int32_t mdcii::world::WorldGenerator2::GetIndex(const int32_t t_x, const int32_t t_y, const int32_t t_width)
{
    return t_y * t_width + t_x;
}

bool mdcii::world::WorldGenerator2::IsMapTerrainPositionOnSeaSide(const WorldGenerator2::Position& t_position)
{
    if (t_position.mapValue.value != MAP_TERRAIN)
    {
        return false;
    }

    return std::any_of(t_position.neighborIndices.begin(), t_position.neighborIndices.end(), [this](const int32_t t_idx){
        return t_idx != -1 && m_map.positions.at(t_idx).mapValue.value == MAP_WATER;
    });
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

void mdcii::world::WorldGenerator2::CreateTerrainTiles(std::vector<std::shared_ptr<layer::Tile>>& t_terrainTiles) const
{
    MDCII_ASSERT(!m_map.positions.empty(), "[WorldGenerator2::CreateTerrainTiles()] Missing map values.")

    for (auto y{ 0 }; y < m_map.height; ++y)
    {
        for (auto x{ 0 }; x < m_map.width; ++x)
        {
            t_terrainTiles.emplace_back(std::make_unique<layer::Tile>());
        }
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution randInRange(0, 10);

    for (auto y{ 0 }; y < m_map.height; ++y)
    {
        for (auto x{ 0 }; x < m_map.width; ++x)
        {
            const auto idx{ GetIndex(x, y, m_map.width) };
            const auto val{ m_map.positions.at(idx).mapValue.value };

            if (val == MAP_TERRAIN)
            {
                auto id{ data::GRASS_BUILDING_ID };
                if (m_south)
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
        }
    }
}

void mdcii::world::WorldGenerator2::CreateCoastTiles(std::vector<std::shared_ptr<layer::Tile>>& t_coastTiles) const
{
    MDCII_ASSERT(!m_map.positions.empty(), "[WorldGenerator2::CreateCoastTiles()] Missing map values.")

    for (auto y{ 0 }; y < m_map.height; ++y)
    {
        for (auto x{ 0 }; x < m_map.width; ++x)
        {
            t_coastTiles.emplace_back(std::make_unique<layer::Tile>());
        }
    }

    for (auto y{ 0 }; y < m_map.height; ++y)
    {
        for (auto x{ 0 }; x < m_map.width; ++x)
        {
            const auto idx{ GetIndex(x, y, m_map.width) };
            const auto val{ m_map.positions.at(idx).mapValue.value };

            if (val == MAP_WATER)
            {
                t_coastTiles.at(idx) = CreateTile(data::SHALLOW_WATER_BUILDING_ID, x, y, Rotation::DEG0);
            }
        }
    }
}

void mdcii::world::WorldGenerator2::CreateBuildingsTiles(std::vector<std::shared_ptr<layer::Tile>>& t_buildingsTiles) const
{
    MDCII_ASSERT(!m_map.positions.empty(), "[WorldGenerator2::CreateBuildingsTiles()] Missing map values.")

    for (auto y{ 0 }; y < m_map.height; ++y)
    {
        for (auto x{ 0 }; x < m_map.width; ++x)
        {
            t_buildingsTiles.emplace_back(std::make_unique<layer::Tile>());
        }
    }
}
