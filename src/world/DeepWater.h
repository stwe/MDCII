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

#pragma once

#include "world/layer/Layer.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::world
{
    //-------------------------------------------------
    // DeepWater
    //-------------------------------------------------

    /**
     * @brief Represents the deep water area.
     */
    class DeepWater
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief The deep water area tile layer.
         */
        std::unique_ptr<layer::TerrainLayer<tile::TerrainTile>> layer;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        DeepWater() = delete;

        /**
         * @brief Constructs a new DeepWater object.
         *
         * @param t_world Pointer to the parent World object.
         */
        explicit DeepWater(World* t_world);

        DeepWater(const DeepWater& t_other) = delete;
        DeepWater(DeepWater&& t_other) noexcept = delete;
        DeepWater& operator=(const DeepWater& t_other) = delete;
        DeepWater& operator=(DeepWater&& t_other) noexcept = delete;

        ~DeepWater() noexcept;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief Pointer to the parent World object.
         */
        World* m_world{ nullptr };

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * @brief Initializes the deep water area.
         */
        void Init();

        /**
         * @brief Initialize the deep water layer and its tiles.
         */
        void InitLayer();

        /**
         * @brief Set the world positions for each tile.
         */
        void SetWorldPositions();

        /**
         * @brief Remove tiles that do not belong to the deep water.
         */
        void RemoveIrrelevantTiles() const;

        /**
         * @brief Update the properties of each tile based on the buildingId it contains.
         */
        void UpdateTileProperties() const;
    };
}
