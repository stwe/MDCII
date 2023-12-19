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

#include <optional>
#include "physics/Aabb.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::resource
{
    /**
     * @brief Forward declaration class MdciiFile.
     */
    class MdciiFile;
}

namespace mdcii::world
{
    /**
     * @brief Forward declaration class World.
     */
    class World;

    /**
     * @brief Forward declaration struct Tile.
     */
    struct Tile;

    /**
     * @brief Forward declaration class Layer.
     */
    class Layer;

    /**
     * @brief Forward declaration enum class LayerType.
     */
    enum class LayerType;

    //-------------------------------------------------
    // ClimateZone
    //-------------------------------------------------

    /**
     * @brief Available climate zones.
     */
    enum class ClimateZone
    {
        NORTH, SOUTH
    };

    //-------------------------------------------------
    // Island
    //-------------------------------------------------

    /**
     * @brief Represents an Island.
     */
    class Island
    {
        //-------------------------------------------------
        // Friends
        //-------------------------------------------------

        /**
         * @brief So MdciiFile has access to the private Init method.
         */
        friend class resource::MdciiFile;
    public:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * @brief Number of layers (coast, terrain, buildings, mixed).
         */
        static constexpr auto NR_OF_LAYERS{ 4 };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief The width of the island.
         */
        const int width;

        /**
         * @brief The height of the island.
         */
        const int height;

        /**
         * @brief The X-coordinate of the island's initial position in the world.
         */
        const int startX;

        /**
         * @brief The Y-coordinate of the island's initial position in the world.
         */
        const int startY;

        /**
         * @brief The tile layers (coast, terrain, buildings, mixed) of the island.
         */
        std::array<std::unique_ptr<Layer>, NR_OF_LAYERS> layers;

        /**
         * @brief The climate zone of the island.
         */
        ClimateZone climateZone{ ClimateZone::NORTH };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Island() = delete;

        /**
         * @brief Constructs a new Island object.
         *
         * @param t_world Pointer to the parent World object.
         * @param t_width The width of the island.
         * @param t_height The height of the island.
         * @param t_x The X-coordinate of the island's initial position in the world.
         * @param t_y The Y-coordinate of the island's initial position in the world.
         */
        Island(World* t_world, int t_width, int t_height, int t_x, int t_y);

        Island(const Island& t_other) = delete;
        Island(Island&& t_other) noexcept = delete;
        Island& operator=(const Island& t_other) = delete;
        Island& operator=(Island&& t_other) noexcept = delete;

        ~Island() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * @brief Getter function to retrieve a pointer to the specified layer.
         *
         * @param t_layerType The type of layer to retrieve.
         *
         * @return A pointer to the requested layer.
         */
        [[nodiscard]] Layer* GetLayer(LayerType t_layerType);

        /**
         * @brief Const getter function to retrieve a constant pointer to the specified layer.
         *
         * @param t_layerType The type of layer to retrieve.
         *
         * @return A constant pointer to the requested layer.
         */
        [[nodiscard]] const Layer* GetLayer(LayerType t_layerType) const;

        //-------------------------------------------------
        // Position
        //-------------------------------------------------

        /**
         * @brief Checks whether the given world position is over the island.
         *
         * @param t_position The position in the world.
         *
         * @return True if the position is over the island. Otherwise, it returns false.
         */
        [[nodiscard]] bool IsWorldPositionOverIsland(const olc::vi2d& t_position) const;

        /**
         * @brief Checks whether the mouse is over the island.
         *
         * @return std::optional containing the mouse position relative to the island, std::nullopt otherwise.
         */
        [[nodiscard]] std::optional<olc::vi2d> IsMouseOverIsland() const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief Pointer to the parent World object.
         */
        World* m_world{ nullptr };

        /**
         * @brief An Aabb (axis-aligned bounding box) object for collision detection.
         *
         * The Aabb is constructed from startX and startY variables, along with width and height.
         * These define the location of the island in the world, indicating where it starts and ends.
         */
        const physics::Aabb m_aabb;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * @brief Initializes the island.
         */
        void Init();

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * @brief Checks whether the given world position is inside the Aabb.
         *
         * @param t_position The position in the world.
         *
         * @return True if the position is inside the Aabb. Otherwise, it returns false.
         */
        [[nodiscard]] bool IsWorldPositionInAabb(const olc::vi2d& t_position) const;

        //-------------------------------------------------
        // Layer
        //-------------------------------------------------

        /**
         * @brief Initialises the COAST, TERRAIN and BUILDINGS layer of the island.
         */
        void InitLayers();

        /**
         * @brief Initialises the mixed layer of the island, which is created from all other layers.
         */
        void InitMixedLayer();

        /**
         * @brief Populates the mixed layer with data from other layers.
         *
         * @param t_coastLayer Coast layer of the island.
         * @param t_terrainLayer Terrain layer of the island.
         * @param t_buildingsLayer Buildings layer of the island.
         * @param t_mixedLayer The mixed layer to be populated.
         * @param t_x The x position
         * @param t_y The y position.
         */
        void PopulateMixedLayer(const Layer* t_coastLayer, const Layer* t_terrainLayer, const Layer* t_buildingsLayer, Layer* t_mixedLayer, int t_x, int t_y) const;

        //-------------------------------------------------
        // Layer tiles
        //-------------------------------------------------

        /**
         * @brief Checks if a tile should be replaced in the mixed layer.
         *
         * @param t_layer Layer to check the tile from.
         * @param t_buildingsLayer Buildings layer of the island.
         * @param t_index Index of the tile to check.
         *
         * @return True if the tile should be replaced; false otherwise.
         */
        static bool ShouldReplaceTile(const Layer* t_layer, const Layer* t_buildingsLayer, int t_index);

        /**
         * @brief Initializes all layer tiles.
         *
         * @param t_layer Pointer to the Layer.
         */
        void InitLayerTiles(Layer* t_layer) const;

        /**
         * @brief Initializes the details of a single tile.
         *
         * @param t_layer Current layer being initialized.
         * @param t_x The x coordinate of the tile.
         * @param t_y The y coordinate of the tile.
         */
        void InitTileDetails(Layer* t_layer, int t_x, int t_y) const;
    };
}
