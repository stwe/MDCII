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

#include "GameLayer.h"
#include "Tile.h"

//-------------------------------------------------
// WorldGridLayer
//-------------------------------------------------

namespace mdcii::layer
{
    /**
     * The WorldGridLayer contains all the data to render a grid over the world.
     */
    class WorldGridLayer: public GameLayer
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * Contains all Tile pointers in the order DEG0.
         */
        std::vector<std::shared_ptr<Tile>> tiles;

        /**
         * A vector of Tile pointers for each rotation.
         * The Tile pointers are in the correct order for rendering.
         */
        std::array<std::vector<std::shared_ptr<Tile>>, world::NR_OF_ROTATIONS> sortedTiles;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        WorldGridLayer() = delete;

        /**
         * Constructs a new WorldGridLayer object.
         *
         * @param t_context Access to shared objects.
         * @param t_world The World object.
         */
        WorldGridLayer(std::shared_ptr<state::Context> t_context, world::World* t_world);

        WorldGridLayer(const WorldGridLayer& t_other) = delete;
        WorldGridLayer(WorldGridLayer&& t_other) noexcept = delete;
        WorldGridLayer& operator=(const WorldGridLayer& t_other) = delete;
        WorldGridLayer& operator=(WorldGridLayer&& t_other) noexcept = delete;

        ~WorldGridLayer() noexcept override;

    protected:

    private:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * Each model matrix is based on this gfx number.
         */
        static constexpr auto GRASS_GFX{ 4 };

        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        void CreateTiles() override;
        void SortTiles() override;
        void CreateModelMatricesContainer() override;

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Adds some pre-calculations to every Tile object of the Layer,
         * which are necessary to render the Tile on the screen.
         *
         * @param t_tile The Tile object.
         * @param t_x The x position for Deg0 in the world.
         * @param t_y The y position for Deg0 in the world.
         */
        void PreCalcTile(Tile& t_tile, int32_t t_x, int32_t t_y) const;

        /**
         * Creates a model matrix for a given Tile object.
         *
         * @param t_tile The Tile object.
         * @param t_zoom The zoom for which to create the model matrix.
         * @param t_rotation The rotation for which to create the model matrix.
         *
         * @return The model matrix.
         */
        [[nodiscard]] glm::mat4 CreateModelMatrix(const Tile& t_tile, world::Zoom t_zoom, world::Rotation t_rotation) const;
    };
}
