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

#include "Tile.h"

namespace mdcii::world
{
    //-------------------------------------------------
    // Forward declarations
    //-------------------------------------------------

    /**
     * @brief Forward declaration enum class Rotation.
     */
    enum class Rotation;

    /**
     * @brief Forward declaration class World.
     */
    class World;

    //-------------------------------------------------
    // LayerType
    //-------------------------------------------------

    /**
     * @brief Enum class representing different types of layers.
     */
    enum class LayerType
    {
        COAST, TERRAIN, BUILDINGS, MIXED, DEEP_WATER, NONE
    };

    //-------------------------------------------------
    // Layer
    //-------------------------------------------------

    /**
     * @brief Class representing a layer.
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
         * @brief A vector holding all tiles belonging to this layer.
         */
        std::vector<Tile> tiles;

        /**
         * @brief An array of std::vectors, each representing a rotation of the tiles.
         *
         * Tiles are sorted by index for each rotation.
         */
        std::array<std::vector<Tile>, Tile::NR_OF_ROTATIONS> sortedTiles;

        /**
         * @brief The tiles that are currently being rendered.
         */
        std::vector<Tile> currentTiles;

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

        ~Layer() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * @brief Retrieves a reference to the Tile at the specified position and rotation.
         *
         * @param t_x The x position on the layer.
         * @param t_y The x position on the layer.
         * @param t_rotation The current camera rotation.
         *
         * @return A reference to the Tile at the specified position and rotation.
         */
        [[nodiscard]] Tile& GetTile(int t_x, int t_y, Rotation t_rotation);

        /**
         * @brief Retrieves a const reference to the Tile at the specified position and rotation.
         *
         * @param t_x The x position on the layer.
         * @param t_y The x position on the layer.
         * @param t_rotation The current camera rotation.
         *
         * @return A const reference to the Tile at the specified position and rotation.
         */
        [[nodiscard]] const Tile& GetTile(int t_x, int t_y, Rotation t_rotation) const;

        //-------------------------------------------------
        // Create Tiles
        //-------------------------------------------------

        void CreateLayerTiles(const nlohmann::json& t_json);
        Tile CreateTile(const nlohmann::json& t_json);

        //-------------------------------------------------
        // Add
        //-------------------------------------------------

        /**
         * @brief Adds a building to the layer.
         *
         * @param t_buildingTiles The tiles of the building to be added.
         */
        void AddBuilding(std::vector<Tile>& t_buildingTiles);

        //-------------------------------------------------
        // Sort
        //-------------------------------------------------

        /**
         * @brief Sorts the tiles for correct rendering order.
         *
         * The sorted tiles are stored in `sortedTiles`.
         */
        void SortTilesForRendering();

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * @brief 2D/1D - mapping of a position.
         *
         * The index can be used, for example, to get a tile with the 2D position from the sortedTiles array.
         *
         * @param t_x The 2D x position.
         * @param t_y The 2D y position.
         * @param t_width The width of the 2D array.
         * @param t_height The height of the 2D array.
         * @param t_rotation The given position is previously rotated by the specified rotation.
         *
         * @return The 1D index.
         */
        [[nodiscard]] static int GetMapIndex(int t_x, int t_y, int t_width, int t_height, Rotation t_rotation);

        /**
         * @brief 2D/1D - mapping of a layer position.
         *
         * The index is used to get a tile from the layer.
         *
         * @param t_x The x position on the layer.
         * @param t_y The x position on the layer.
         * @param t_rotation The given position is previously rotated by the specified rotation.
         *
         * @return The 1D index.
         */
        [[nodiscard]] int GetMapIndex(int t_x, int t_y, Rotation t_rotation) const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief Pointer to the parent World object.
         */
        const World* m_world{ nullptr };

        /**
         * @brief The width of the layer.
         */
        const int m_width;

        /**
         * @brief The height of the layer.
         */
        const int m_height;
    };
}
