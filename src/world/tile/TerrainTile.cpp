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

#include "TerrainTile.h"
#include "world/layer/Layer.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::tile::TerrainTile::TerrainTile(
    const resource::Building* t_building,
    const int t_rotation,
    const int t_x,
    const int t_y,
    const int t_posX,
    const int t_posY
)
    : Tile(t_rotation, t_posX, t_posY)
    , building{ t_building }
    , x{ t_x }
    , y{ t_y }
{
    CalculateGfxs();
}

mdcii::world::tile::TerrainTile::TerrainTile(
    const resource::Building* t_building,
    const int t_rotation,
    const int t_x,
    const int t_y,
    const int t_posX,
    const int t_posY,
    const TileType t_tileType
)
    : Tile(t_rotation, t_posX, t_posY, t_tileType)
    , building{ t_building }
    , x{ t_x }
    , y{ t_y }
{
    CalculateGfxs();
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

bool mdcii::world::tile::TerrainTile::HasBuildingAboveWaterAndCoast() const
{
    return building != nullptr && building->posoffs > 0;
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::world::tile::TerrainTile::UpdateFrame(const std::array<int, NR_OF_ANIM_TIMES>& t_frameValues)
{
    switch (building->animTime)
    {
        case 90:
            frame = t_frameValues[0] % building->animAnz;
            break;
        case 130:
            frame = t_frameValues[1] % building->animAnz;
            break;
        case 150:
            frame = t_frameValues[2] % building->animAnz;
            break;
        case 180:
            frame = t_frameValues[3] % building->animAnz;
            break;
        case 220:
            frame = t_frameValues[4] % building->animAnz;
            break;
        default:
            frame = 0;
    }
}

void mdcii::world::tile::TerrainTile::RenderImGui(const void* t_layer) const
{
    // this

    RenderImGuiTileInfo(*this);

    // neighbors

    const auto* terrainLayer{ reinterpret_cast<const layer::TerrainLayer<TerrainTile>*>(t_layer) };

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
    ImGui::Separator();
    ImGui::Text("Traffic Neighbors: ");
    ImGui::Separator();
    ImGui::PopStyleColor();

    if (n >= 0 && terrainLayer->tiles.at(n).type == TileType::TRAFFIC)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
        ImGui::Text("North Traffic Tile");
        ImGui::PopStyleColor();

        RenderImGuiTileInfo(terrainLayer->tiles.at(n));
    }

    if (s >= 0 && terrainLayer->tiles.at(s).type == TileType::TRAFFIC)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
        ImGui::Text("South Traffic Tile");
        ImGui::PopStyleColor();

        RenderImGuiTileInfo(terrainLayer->tiles.at(s));
    }

    if (e >= 0 && terrainLayer->tiles.at(e).type == TileType::TRAFFIC)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
        ImGui::Text("East Traffic Tile");
        ImGui::PopStyleColor();

        RenderImGuiTileInfo(terrainLayer->tiles.at(e));
    }

    if (w >= 0 && terrainLayer->tiles.at(w).type == TileType::TRAFFIC)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
        ImGui::Text("West Traffic Tile");
        ImGui::PopStyleColor();

        RenderImGuiTileInfo(terrainLayer->tiles.at(w));
    }
}

void mdcii::world::tile::TerrainTile::RenderImGuiTileInfo(const TerrainTile& t_tile)
{
    if (t_tile.HasBuilding())
    {
        ImGui::Text("Building Id: %d with Gfx: %d", t_tile.building->id, t_tile.building->gfx);

        if (t_tile.building->IsBig())
        {
            ImGui::Text("Building xy: %d, %d", t_tile.x, t_tile.y);
        }
    }

    ImGui::Text("Building pos xy: %d, %d", t_tile.posX, t_tile.posY);
}

//-------------------------------------------------
// Gfx
//-------------------------------------------------

void mdcii::world::tile::TerrainTile::AdjustGfxForBigBuildings(int& t_gfx) const
{
    MDCII_ASSERT(HasBuilding(), "[TerrainTile::AdjustGfxForBigBuildings()] Null pointer.")

    // default: orientation 0
    auto rp{ olc::vi2d(x, y) };

    using enum Rotation;

    if (rotation == magic_enum::enum_integer(DEG270))
    {
        rp = rotate_position(
            x, y,
            building->size.w, building->size.h,
            DEG90
        );
    }

    if (rotation == magic_enum::enum_integer(DEG180))
    {
        rp = rotate_position(
            x, y,
            building->size.w, building->size.h,
            DEG180
        );
    }

    if (rotation == magic_enum::enum_integer(DEG90))
    {
        rp = rotate_position(
            x, y,
            building->size.w, building->size.h,
            DEG270
        );
    }

    const auto offset{ rp.y * building->size.w + rp.x };
    t_gfx += offset;
}

bool mdcii::world::tile::TerrainTile::DetermineTrafficGfx(const uint8_t t_neighborFlag)
{
    MDCII_ASSERT(HasBuilding(), "[TerrainTile::DetermineTrafficGfx()] Null pointer.")

    resource::RoadGfx roadGfx;
    switch (t_neighborFlag)
    {
        using enum resource::RoadGfx;
        case 0: roadGfx = ROAD_WE;
            break;
        case 1: roadGfx = ROAD_NS;
            break;
        case 2: roadGfx = ROAD_WE;
            break;
        case 3: roadGfx = ROAD_C_NE;
            break;
        case 4:
        case 5: roadGfx = ROAD_NS;
            break;
        case 6: roadGfx = ROAD_C_ES;
            break;
        case 7: roadGfx = ROAD_T_NES;
            break;
        case 8: roadGfx = ROAD_WE;
            break;
        case 9: roadGfx = ROAD_C_WN;
            break;
        case 10: roadGfx = ROAD_WE;
            break;
        case 11: roadGfx = ROAD_T_WNE;
            break;
        case 12: roadGfx = ROAD_C_SW;
            break;
        case 13: roadGfx = ROAD_T_NSW;
            break;
        case 14: roadGfx = ROAD_T_WES;
            break;
        case 15: roadGfx = ROAD_X;
            break;
        default: roadGfx = ROAD_WE;
    }

    auto newGfxInt{ magic_enum::enum_integer(roadGfx) };

    if (building->id == resource::DIRT_ROAD_ID)
    {
        newGfxInt += 20;
    }

    MDCII_ASSERT(!gfxs.empty(), "[TerrainTile::DetermineTrafficGfx()] Invalid gfx values.")

    const auto oldGfx{ gfxs[0] };
    gfxs[0] = newGfxInt;

    return oldGfx != newGfxInt;
}

//-------------------------------------------------
// Override
//-------------------------------------------------

void mdcii::world::tile::TerrainTile::CalculateGfxValues()
{
    MDCII_ASSERT(HasBuilding(), "[TerrainTile::CalculateGfxValues()] Null pointer.")
    MDCII_ASSERT(gfxs.empty(), "[TerrainTile::CalculateGfxValues()] Invalid gfx values.")

    const auto gfx0{ building->gfx };

    gfxs.push_back(gfx0);
    if (building->IsRotatable())
    {
        gfxs.push_back(gfx0 + (1 * building->rotate));
        gfxs.push_back(gfx0 + (2 * building->rotate));
        gfxs.push_back(gfx0 + (3 * building->rotate));
    }

    if (building->IsBig())
    {
        for (auto& gfx : gfxs)
        {
            AdjustGfxForBigBuildings(gfx);
        }
    }
}

//-------------------------------------------------
// Serializing TerrainTile into Json
//-------------------------------------------------

void mdcii::world::tile::to_json(nlohmann::json& t_json, const TerrainTile& t_tile)
{
    if (t_tile.HasBuilding())
    {
        t_json = nlohmann::json{
            { "id", t_tile.building->id },
            { "rotation", t_tile.rotation },
            { "x", t_tile.x },
            { "y", t_tile.y },
        };
    }
    else
    {
        t_json = nlohmann::json::object();
    }
}
