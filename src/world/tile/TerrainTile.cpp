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
#include "MdciiAssert.h"
#include "resource/Buildings.h"
#include "world/Rotation.h"
#include "vendor/imgui/imgui.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::tile::TerrainTile::TerrainTile()
{
}

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
    CalculateGfx();
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

void mdcii::world::tile::TerrainTile::RenderImGui() const
{
    if (HasBuilding())
    {
        ImGui::Text("Building Id: %d", building->id);
        ImGui::Text("Building Gfx: %d", building->gfx);
    }
    ImGui::Text("Building x: %d", x);
    ImGui::Text("Building y: %d", y);
}

//-------------------------------------------------
// Gfx
//-------------------------------------------------

void mdcii::world::tile::TerrainTile::CalculateGfx()
{
    MDCII_ASSERT(HasBuilding(), "[TerrainTile::CalculateGfx()] Null pointer.")

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

bool mdcii::world::tile::TerrainTile::DetermineNeighborType()
{
    uint8_t neighbors{ 0 };

    if (n && n->type == TileType::TRAFFIC)
    {
        neighbors = NORTH;
    }

    if (e && e->type == TileType::TRAFFIC)
    {
        neighbors |= EAST;
    }

    if (s && s->type == TileType::TRAFFIC)
    {
        neighbors |= SOUTH;
    }

    if (w && w->type == TileType::TRAFFIC)
    {
        neighbors |= WEST;
    }

    NeighborType newNeighborType;
    switch (neighbors)
    {
        using enum NeighborType;
        case 0: newNeighborType = NEIGHBOR_NONE; // keine Nachbarn
            break;
        case 1: newNeighborType = NEIGHBOR_V;    // Norden
            break;
        case 2: newNeighborType = NEIGHBOR_H;    // Osten
            break;
        case 3: newNeighborType = NEIGHBOR_C3;   // Norden - Osten
            break;
        case 4:                                  // Sueden
        case 5: newNeighborType = NEIGHBOR_V;    // Sueden - Norden
            break;
        case 6: newNeighborType = NEIGHBOR_C1;   // Sueden - Osten
            break;
        case 7: newNeighborType = NEIGHBOR_T2;   // Norden - Osten - Sueden
            break;
        case 8: newNeighborType = NEIGHBOR_H;    // Westen
            break;
        case 9: newNeighborType = NEIGHBOR_C4;   // Westen - Norden
            break;
        case 10: newNeighborType = NEIGHBOR_H;   // Westen - Osten
            break;
        case 11: newNeighborType = NEIGHBOR_T4;  // Westen - Osten - Norden
            break;
        case 12: newNeighborType = NEIGHBOR_C2;  // Westen - Sueden
            break;
        case 13: newNeighborType = NEIGHBOR_T3;  // Westen - Sueden - Norden
            break;
        case 14: newNeighborType = NEIGHBOR_T1;  // Westen - Sueden - Osten
            break;
        case 15: newNeighborType = NEIGHBOR_X;
            break;
        default: newNeighborType = NEIGHBOR_NONE;
    }

    const auto oldNeighborType{ neighborType };
    neighborType = newNeighborType;

    return oldNeighborType != newNeighborType;
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
