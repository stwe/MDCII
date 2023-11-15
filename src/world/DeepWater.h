// This file is part of the MDCII project.
//
// Copyright (c) 2023. stwe <https://github.com/stwe/MDCII>
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

#include <memory>

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii
{
    /**
     * @brief Forward declaration class Game.
     */
    class Game;
}

namespace mdcii::world
{
    /**
     * @brief Forward declaration class Layer.
     */
    class Layer;

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
         * @brief The width of the deep water area.
         */
        int width{ -1 };

        /**
         * @brief The height of the deep water area.
         */
        int height{ -1 };

        /**
         * @brief The deep water tile layer.
         */
        std::unique_ptr<Layer> layer;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        DeepWater();

        DeepWater(const DeepWater& t_other) = delete;
        DeepWater(DeepWater&& t_other) noexcept = delete;
        DeepWater& operator=(const DeepWater& t_other) = delete;
        DeepWater& operator=(DeepWater&& t_other) noexcept = delete;

        ~DeepWater() noexcept;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * @brief Initializes the layer.
         *
         * @param t_game Pointer to the Game.
         */
        void Init(const Game* t_game);

    protected:

    private:

    };
}