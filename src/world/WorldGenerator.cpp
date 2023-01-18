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

#include <filesystem>
#include <deque>
#include "WorldGenerator.h"
#include "world/World.h"
#include "Game.h"
#include "MdciiUtils.h"
#include "layer/TerrainLayer.h"
#include "data/BuildingIds.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::WorldGenerator::WorldGenerator()
{
    Log::MDCII_LOG_DEBUG("[WorldGenerator::WorldGenerator()] Create WorldGenerator.");

    Init();
}

mdcii::world::WorldGenerator::~WorldGenerator() noexcept
{
    Log::MDCII_LOG_DEBUG("[WorldGenerator::~WorldGenerator()] Destruct WorldGenerator.");
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::world::WorldGenerator::RenderImGui()
{
    nlohmann::json j;

    static int32_t width{ World::WORLD_MIN_WIDTH };
    static int32_t height{ World::WORLD_MIN_HEIGHT };

    ImGui::Separator();
    ImGui::SliderInt("World width ", &width, World::WORLD_MIN_WIDTH, World::WORLD_MAX_WIDTH);
    ImGui::SliderInt("World height", &height, World::WORLD_MIN_HEIGHT, World::WORLD_MAX_HEIGHT);
    ImGui::Separator();

    static int nrOfIslands{ 1 };
    ImGui::SliderInt("Islands to add", &nrOfIslands, 1, 4);

    static bool addIslands{ false };
    if (ImGui::Button("Add islands"))
    {
        addIslands = true;
    }

    if (addIslands)
    {
        static std::vector<int32_t> w(nrOfIslands, 16);
        static std::vector<int32_t> h(nrOfIslands, 16);
        static std::vector<int32_t> x(nrOfIslands, 0);
        static std::vector<int32_t> y(nrOfIslands, 0);
        static std::deque<bool> s(nrOfIslands, true);
        static std::vector<physics::Aabb> aabbs(nrOfIslands, physics::Aabb({ 0, 0 }, { 0, 0 }));

        if (ImGui::BeginTable("Islands", 5))
        {
            ImGui::TableSetupColumn("width");
            ImGui::TableSetupColumn("height");
            ImGui::TableSetupColumn("x");
            ImGui::TableSetupColumn("y");
            ImGui::TableSetupColumn("north/south");
            ImGui::TableHeadersRow();

            for (int row = 0; row < nrOfIslands; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 5; column++)
                {
                    ImGui::TableSetColumnIndex(column);

                    ImGui::PushID(row * 5 + column);
                    if (column == 0)
                    {
                        ImGui::InputInt("w", &w.at(row));
                    }
                    if (column == 1)
                    {
                        ImGui::InputInt("h", &h.at(row));
                    }
                    if (column == 2)
                    {
                        ImGui::InputInt("x", &x.at(row));
                    }
                    if (column == 3)
                    {
                        ImGui::InputInt("y", &y.at(row));
                    }
                    if (column == 4)
                    {
                        toggle_button("s", &s.at(row));
                    }
                    ImGui::PopID();
                }
            }
            ImGui::EndTable();
        }

        if (ImGui::Button("Create"))
        {
            for (auto v{ 0 }; v < nrOfIslands; ++v)
            {
                aabbs.at(v) = physics::Aabb(glm::ivec2(x.at(v), y.at(v)), glm::ivec2(w.at(v), h.at(v)));
            }

            if (Validate(aabbs))
            {
                AddWorldValues(j, width, height);

                for (auto i{ 0 }; i < nrOfIslands; ++i)
                {
                    AddIslandValues(j, w.at(i), h.at(i), x.at(i), y.at(i), s.at(i));
                }

                m_file << j;

                Log::MDCII_LOG_DEBUG("[WorldGenerator::RenderImGui()] The world was successfully created.");
            }
            else
            {
                Log::MDCII_LOG_WARN("[WorldGenerator::RenderImGui()] Invalid island positions.");
            }
        }
    }
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::world::WorldGenerator::Init()
{
    Log::MDCII_LOG_DEBUG("[WorldGenerator::Init()] Initializing world generator...");

    const auto fileName{ Game::RESOURCES_REL_PATH + "data/NewWorld.json" };
    if (std::filesystem::exists(fileName))
    {
        Log::MDCII_LOG_WARN("[WorldGenerator::Init()] The {} file already exists.", fileName);

        return;
    }

    m_file.open(fileName);
    if (!m_file.is_open())
    {
        throw MDCII_EXCEPTION("[WorldGenerator::Init()] Error while opening file " + fileName + ".");
    }

    Log::MDCII_LOG_DEBUG("[WorldGenerator::Init()] The world generator was successfully initialized.");
}

//-------------------------------------------------
// Create Json
//-------------------------------------------------

bool mdcii::world::WorldGenerator::Validate(const std::vector<physics::Aabb>& t_aabbs)
{
    Log::MDCII_LOG_DEBUG("[WorldGenerator::Validate()] Check for Aabbs collisions.");

    for (auto i{ 0 }; i < t_aabbs.size(); ++i)
    {
        for (auto j{ 0 }; j < t_aabbs.size(); ++j)
        {
            if (i != j)
            {
                if (physics::Aabb::AabbVsAabb(t_aabbs.at(i), t_aabbs.at(j)))
                {
                    return false;
                }
            }
        }
    }

    return true;
}

void mdcii::world::WorldGenerator::AddWorldValues(nlohmann::json& t_j, const int32_t t_width, const int32_t t_height)
{
    Log::MDCII_LOG_DEBUG("[WorldGenerator::AddWorldValues()] Adds world Json values.");

    t_j["world"] = { { "width", t_width }, { "height", t_height } };
}

void mdcii::world::WorldGenerator::AddIslandValues(
    nlohmann::json& t_j,
    const int32_t t_width,
    const int32_t t_height,
    const int32_t t_worldX,
    const int32_t t_worldY,
    const bool t_south
)
{
    Log::MDCII_LOG_DEBUG("[WorldGenerator::AddIslandValues()] Adds Json values for an new island.");

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

    CreateTerrain(t_width, t_height, terrainTiles, t_south);
    CreateCoast(t_width, t_height, coastTiles);
    CreateBuildings(t_width, t_height, buildingsTiles);
    c["coast"] = coastTiles;
    t["terrain"] = terrainTiles;
    b["buildings"] = buildingsTiles;

    i["layers"].push_back(c);
    i["layers"].push_back(t);
    i["layers"].push_back(b);

    t_j["islands"].push_back(i);
}

//-------------------------------------------------
// Create Layer
//-------------------------------------------------

void mdcii::world::WorldGenerator::CreateTerrain(
    const int32_t t_width,
    const int32_t t_height,
    std::vector<std::shared_ptr<layer::Tile>>& t_terrainTiles,
    const bool t_south
)
{
    // default
    for (auto y{ 0 }; y < t_height; ++y)
    {
        for (auto x{ 0 }; x < t_width; ++x)
        {
            t_terrainTiles.emplace_back(std::make_unique<layer::Tile>());
        }
    }

    auto mapIndex = [&](const int32_t t_x, const int32_t t_y) {
        return t_y * t_width + t_x;
    };

    srand(static_cast<uint32_t>(time(0)));

    // terrain
    for (auto y{ 2 }; y < t_height - 2; ++y)
    {
        for (auto x{ 2 }; x < t_width - 2; ++x)
        {
            auto id{ data::GRASS_BUILDING_ID };
            if (t_south)
            {
                if (auto r{ (rand() % 10) }; r >= 7)
                {
                    id = data::SOUTH_TREES.at(rand() % 11);
                }
            }
            else
            {
                if (auto r{ (rand() % 10) }; r >= 7)
                {
                    id = data::NORTH_TREES.at(rand() % 11);
                }
            }

            auto terrainTile{ std::make_unique<layer::Tile>() };
            terrainTile->buildingId = id;
            terrainTile->rotation = Rotation::DEG0;
            terrainTile->x = 0;
            terrainTile->y = 0;
            terrainTile->worldXDeg0 = x;
            terrainTile->worldYDeg0 = y;

            t_terrainTiles.at(mapIndex(x, y)) = std::move(terrainTile);
        }
    }

    // corners
    t_terrainTiles.at(mapIndex(2, 2))->buildingId = data::BANK_CORNER_BUILDING_ID; // top left
    t_terrainTiles.at(mapIndex(2, 2))->rotation = Rotation::DEG90;

    t_terrainTiles.at(mapIndex(t_width - 3, 2))->buildingId = data::BANK_CORNER_BUILDING_ID; // top right
    t_terrainTiles.at(mapIndex(t_width - 3, 2))->rotation = Rotation::DEG180;

    t_terrainTiles.at(mapIndex(t_width - 3, t_height - 3))->buildingId = data::BANK_CORNER_BUILDING_ID; // bottom right
    t_terrainTiles.at(mapIndex(t_width - 3, t_height - 3))->rotation = Rotation::DEG270;

    t_terrainTiles.at(mapIndex(2, t_height - 3))->buildingId = data::BANK_CORNER_BUILDING_ID; // bottom left
    t_terrainTiles.at(mapIndex(2, t_height - 3))->rotation = Rotation::DEG0;

    // borders
    for (auto x{ 3 }; x < t_width - 3; ++x)
    {
        t_terrainTiles.at(mapIndex(x, 2))->buildingId = data::BANK_BUILDING_ID;
        t_terrainTiles.at(mapIndex(x, 2))->rotation = Rotation::DEG180;

        t_terrainTiles.at(mapIndex(x, t_height - 3))->buildingId = data::BANK_BUILDING_ID;
        t_terrainTiles.at(mapIndex(x, t_height - 3))->rotation = Rotation::DEG0;
    }

    for (auto y{ 3 }; y < t_height - 3; ++y)
    {
        t_terrainTiles.at(mapIndex(2, y))->buildingId = data::BANK_BUILDING_ID;
        t_terrainTiles.at(mapIndex(2, y))->rotation = Rotation::DEG90;

        t_terrainTiles.at(mapIndex(t_width - 3, y))->buildingId = data::BANK_BUILDING_ID;
        t_terrainTiles.at(mapIndex(t_width - 3, y))->rotation = Rotation::DEG270;
    }
}

void mdcii::world::WorldGenerator::CreateCoast(
    const int32_t t_width,
    const int32_t t_height,
    std::vector<std::shared_ptr<layer::Tile>>& t_coastTiles
)
{
    // default
    for (auto y{ 0 }; y < t_height; ++y)
    {
        for (auto x{ 0 }; x < t_width; ++x)
        {
            t_coastTiles.emplace_back(std::make_unique<layer::Tile>());
        }
    }

    auto mapIndex = [&](const int32_t t_x, const int32_t t_y) {
        return t_y * t_width + t_x;
    };

    // coast
    for (auto y{ 0 }; y < t_height; ++y)
    {
        for (auto x{ 0 }; x < t_width; ++x)
        {
            if (x == 0 || x == 1 || x == t_width - 1 || x == t_width - 2 ||
                y == 0 || y == 1 || y == t_height - 1 || y == t_height - 2)
            {
                auto terrainTile{ std::make_unique<layer::Tile>() };
                terrainTile->buildingId = data::SHALLOW_WATER_BUILDING_ID;
                terrainTile->rotation = Rotation::DEG0;
                terrainTile->x = 0;
                terrainTile->y = 0;
                terrainTile->worldXDeg0 = x;
                terrainTile->worldYDeg0 = y;

                t_coastTiles.at(mapIndex(x, y)) = std::move(terrainTile);
            }
        }
    }

    // corners
    t_coastTiles.at(mapIndex(1, 1))->buildingId = data::BEACH_CORNER_BUILDING_ID; // top left
    t_coastTiles.at(mapIndex(1, 1))->rotation = Rotation::DEG90;

    t_coastTiles.at(mapIndex(t_width - 2, 1))->buildingId = data::BEACH_CORNER_BUILDING_ID; // top right
    t_coastTiles.at(mapIndex(t_width - 2, 1))->rotation = Rotation::DEG180;

    t_coastTiles.at(mapIndex(t_width - 2, t_height - 2))->buildingId = data::BEACH_CORNER_BUILDING_ID; // bottom right
    t_coastTiles.at(mapIndex(t_width - 2, t_height - 2))->rotation = Rotation::DEG270;

    t_coastTiles.at(mapIndex(1, t_height - 2))->buildingId = data::BEACH_CORNER_BUILDING_ID; // bottom left
    t_coastTiles.at(mapIndex(1, t_height - 2))->rotation = Rotation::DEG0;

    // borders
    for (auto x{ 2 }; x < t_width - 2; ++x)
    {
        t_coastTiles.at(mapIndex(x, 1))->buildingId = data::BEACH_BUILDING_ID;
        t_coastTiles.at(mapIndex(x, 1))->rotation = Rotation::DEG270;

        t_coastTiles.at(mapIndex(x, t_height - 2))->buildingId = data::BEACH_BUILDING_ID;
        t_coastTiles.at(mapIndex(x, t_height - 2))->rotation = Rotation::DEG90;
    }

    for (auto y{ 2 }; y < t_height - 2; ++y)
    {
        t_coastTiles.at(mapIndex(1, y))->buildingId = data::BEACH_BUILDING_ID;
        t_coastTiles.at(mapIndex(1, y))->rotation = Rotation::DEG180;

        t_coastTiles.at(mapIndex(t_width - 2, y))->buildingId = data::BEACH_BUILDING_ID;
        t_coastTiles.at(mapIndex(t_width - 2, y))->rotation = Rotation::DEG0;
    }
}

void mdcii::world::WorldGenerator::CreateBuildings(
    const int32_t t_width,
    const int32_t t_height,
    std::vector<std::shared_ptr<layer::Tile>>& t_buildingsTiles
)
{
    // default
    for (auto y{ 0 }; y < t_height; ++y)
    {
        for (auto x{ 0 }; x < t_width; ++x)
        {
            t_buildingsTiles.emplace_back(std::make_unique<layer::Tile>());
        }
    }
}
