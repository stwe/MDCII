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

#include "Tile.h"
#include "vendor/nlohmann/json.hpp"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::resource
{
    /**
     * @brief Forward declaration struct Figure.
     */
    struct Figure;

    /**
     * @brief Forward declaration enum class FigureId.
     */
    enum class FigureId;
}

namespace mdcii::world::tile
{
    //-------------------------------------------------
    // FigureTile
    //-------------------------------------------------

    /**
     * @brief Represents a FigureTile.
     *
     * These tiles are intended to represent all the figures like animals or soldiers.
     */
    struct FigureTile : public Tile
    {
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * @brief The number of possible animation times.
         */
        static constexpr auto NR_OF_ANIM_TIMES{ 9 };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief Pointer to the Figure object generated from the figuren.cod file.
         */
        const resource::Figure* figure{ nullptr };

        /**
         * @brief The index of the animation currently in use.
         */
        int currentAnimation{ 0 };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        FigureTile() = default;

        /**
         * @brief Constructs a new TerrainTile object.
         *
         * @param t_figure Pointer to the Figure object generated from the figuren.cod file.
         * @param t_currentAnimation The index of the animation currently in use.
         * @param t_rotation The rotation of the tile/building.
         * @param t_posX Represents the x position for DEG0 of a tile relative to its parent island.
         * @param t_posY Represents the y position for DEG0 of a tile relative to its parent island.
         */
        FigureTile(const resource::Figure* t_figure, int t_currentAnimation, int t_rotation, int t_posX, int t_posY);

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * @brief For better readability/convenience.
         *
         * @return True if a Figure object is present.
         */
        [[nodiscard]] bool HasFigure() const { return figure != nullptr; }

        /**
         * @brief For better readability/convenience.
         *
         * @param t_figureId The character's Id.
         *
         * @return True if a Figure object is present.
         */
        [[nodiscard]] bool HasFigure(resource::FigureId t_figureId) const;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        /**
         * @brief Set the current animation frame.
         *
         * @param t_frameValues The current frame values for all animation times.
         */
        void UpdateFrame(const std::array<int, NR_OF_ANIM_TIMES>& t_frameValues);

        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        /**
         * @brief The tile cannot be rendered if there is no figure.
         *
         * @return True if there is no figure; otherwise false.
         */
        [[nodiscard]] bool IsNotRenderable() const override { return !HasFigure(); }

        /**
         * @brief Calculates all gfx values to be stored in `gfxs`.
         */
        void CalculateGfxValues() override {}
    };

    //-------------------------------------------------
    // Serializing TerrainTile into Json
    //-------------------------------------------------

    void to_json(nlohmann::json& t_json, const FigureTile& t_tile);
}
