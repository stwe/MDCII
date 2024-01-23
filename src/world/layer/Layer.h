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

#include "vendor/nlohmann/json.hpp"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::world
{
    /**
     * @brief Forward declaration class World.
     */
    class World;
}

namespace mdcii::world::layer
{
    //-------------------------------------------------
    // LayerType
    //-------------------------------------------------

    /**
     * @brief Representing different types of layers.
     */
    enum class LayerType
    {
        COAST, TERRAIN, BUILDINGS, FIGURES, MIXED, DEEP_WATER, NONE
    };

    //-------------------------------------------------
    // Layer
    //-------------------------------------------------

    /**
     * @brief Base class of all layers used in the game.
     */
    class Layer
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief The type the layer.
         */
        LayerType layerType{ LayerType::NONE };

        /**
         * @brief The width of the layer.
         */
        const int width;

        /**
         * @brief The height of the layer.
         */
        const int height;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Layer() = delete;

        /**
         * @brief Constructs a new Layer object.
         *
         * @param t_world Pointer to the parent World object.
         * @param t_layerType The type of the layer.
         * @param t_width The width of the layer.
         * @param t_height The height of the layer.
         */
        Layer(const World* t_world, LayerType t_layerType, int t_width, int t_height);

        Layer(const Layer& t_other) = delete;
        Layer(Layer&& t_other) noexcept = delete;
        Layer& operator=(const Layer& t_other) = delete;
        Layer& operator=(Layer&& t_other) noexcept = delete;

        virtual ~Layer() noexcept;

        //-------------------------------------------------
        // Create Tiles
        //-------------------------------------------------

        virtual void CreateLayerTiles(const nlohmann::json& t_json) = 0;

    protected:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief Pointer to the parent World object.
         */
        const World* m_world{ nullptr };

    private:

    };
}
