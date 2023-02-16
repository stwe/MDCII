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

#pragma once

#include <vector>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>

//-------------------------------------------------
// IslandGenerator
//-------------------------------------------------

namespace mdcii::world
{
    class IslandGenerator
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        IslandGenerator();

        IslandGenerator(const IslandGenerator& t_other) = delete;
        IslandGenerator(IslandGenerator&& t_other) noexcept = delete;
        IslandGenerator& operator=(const IslandGenerator& t_other) = delete;
        IslandGenerator& operator=(IslandGenerator&& t_other) noexcept = delete;

        ~IslandGenerator() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void RenderImGui();

    protected:

    private:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        static constexpr auto WATER_LEVEL{ 0.5 };
        static constexpr auto TERRAIN{ 1 };
        static constexpr auto WATER{ 0 };

        const std::unordered_map<int32_t, const char*> m_tiles = {
            { 383, "eaol" }, { 479, "eaor" }, { 507, "eaul" }, { 510, "eaur" },
            { 278, "eior" }, { 279, "eior" }, { 406, "eior" }, { 407, "eior" },
            { 360, "eiul" }, { 361, "eiul" }, { 489, "eiul" }, { 488, "eiul" },
            { 267, "eiol" }, { 299, "eiol" }, { 303, "eiol" }, { 271, "eiol" },
            { 496, "eiur" }, { 468, "eiur" }, { 464, "eiur" }, { 500, "eiur" },
            { 319, "g_o_" }, { 287, "g_o_" }, { 415, "g_o_" }, { 447, "g_o_" },
            { 505, "g_u_" }, { 504, "g_u_" }, { 508, "g_u_" }, { 509, "g_u_" },
            { 367, "g_l_" }, { 363, "g_l_" }, { 491, "g_l_" }, { 495, "g_l_" },
            { 471, "g_r_" }, { 470, "g_r_" }, { 502, "g_r_" }, { 503, "g_r_" },
        };

        //-------------------------------------------------
        // Types
        //-------------------------------------------------

        enum NeighborFlag
        {
            GRASS_FLAG = 0,

            NORTH_WEST_FLAG = 1,
            NORTH_FLAG = 2,
            NORTH_EAST_FLAG = 4,

            WEST_FLAG = 8,
            EAST_FLAG = 16,

            SOUTH_WEST_FLAG = 32,
            SOUTH_FLAG = 64,
            SOUTH_EAST_FLAG = 128,

            WATER_FLAG = 256
        };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        inline static int32_t m_width{ 20 };
        inline static int32_t m_height{ 20 };

        std::vector<int32_t> m_terrainValues;
        std::vector<int32_t> m_bitmaskValues;
        std::unordered_set<int32_t> m_invalid;

        //-------------------------------------------------
        // Noise
        //-------------------------------------------------

        void CalcTerrainValues(int32_t t_seed, float t_frequency, int32_t t_width, int32_t t_height);
        void CalcBitmaskValues();

        //-------------------------------------------------
        // Bitmasking
        //-------------------------------------------------

        int32_t GetNorthValue(int32_t t_x, int32_t t_y);
        int32_t GetEastValue(int32_t t_x, int32_t t_y);
        int32_t GetSouthValue(int32_t t_x, int32_t t_y);
        int32_t GetWestValue(int32_t t_x, int32_t t_y);

        int32_t GetNorthWestValue(int32_t t_x, int32_t t_y);
        int32_t GetNorthEastValue(int32_t t_x, int32_t t_y);
        int32_t GetSouthWestValue(int32_t t_x, int32_t t_y);
        int32_t GetSouthEastValue(int32_t t_x, int32_t t_y);

        //-------------------------------------------------
        // ImGui
        //-------------------------------------------------

        void RenderTerrainValuesImGui();
        void RenderBitmaskValuesImGui();
        void RenderTilesImGui();

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        static int32_t GetIndex(int32_t t_x, int32_t t_y, int32_t t_width);
    };
}
