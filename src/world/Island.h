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

#include "physics/Aabb.h"
#include "world/layer/Layer.h"

namespace mdcii::world
{
    //-------------------------------------------------
    // EnumClassHash
    //-------------------------------------------------

    /**
     * @brief Functor object to calculate hash of an Enum Class.
     */
    struct EnumClassHash
    {
        template <typename T>
        std::size_t operator()(T t_arg) const
        {
            return static_cast<std::size_t>(t_arg);
        }
    };

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
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief Pointer to the parent World object.
         */
        const World* world{ nullptr };

        /**
         * @brief The width of the island.
         */
        int width{ -1 };

        /**
         * @brief The height of the island.
         */
        int height{ -1 };

        /**
         * @brief The X-coordinate of the island's initial position in the world.
         */
        int startX{ -1 };

        /**
         * @brief The Y-coordinate of the island's initial position in the world.
         */
        int startY{ -1 };

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
         * @param t_json A Json value representing an Island.
         */
        Island(const World* t_world, const nlohmann::json& t_json);

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
         * @param t_layerType The type of island terrain layer to retrieve.
         *
         * @return A pointer to the requested island terrain layer.
         */
        [[nodiscard]] layer::TerrainLayer<tile::TerrainTile>* GetTerrainLayer(layer::LayerType t_layerType);

        /**
         * @brief Const getter function to retrieve a constant pointer to the specified layer.
         *
         * @param t_layerType The type of island terrain layer to retrieve.
         *
         * @return A constant pointer to the requested island terrain layer.
         */
        [[nodiscard]] const layer::TerrainLayer<tile::TerrainTile>* GetTerrainLayer(layer::LayerType t_layerType) const;

        /**
         * @brief Getter function to retrieve a pointer to the figures layer.
         *
         * @return A pointer to the island figures layer.
         */
        [[nodiscard]] layer::FiguresLayer<tile::FigureTile>* GetFiguresLayer();

        /**
         * @brief Const getter function to retrieve a constant pointer to the figures layer.
         *
         * @return A constant pointer to the island figures layer.
         */
        [[nodiscard]] const layer::FiguresLayer<tile::FigureTile>* GetFiguresLayer() const;

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

        //-------------------------------------------------
        // New building
        //-------------------------------------------------

        /**
         * @brief Creates `m_newBuildingTiles` representing a new building.
         *
         * @param t_building Pointer to a Building object.
         * @param t_rotation The building rotation.
         * @param t_position The building position.
         *
         * @return True if building tiles can be added; otherwise false.
         */
        [[nodiscard]] bool CreateNewBuildingTiles(const resource::Building* t_building, Rotation t_rotation, const olc::vi2d& t_position);

        /**
         * @brief Shows the content of `m_newBuildingTiles`.
         */
        void PreviewNewBuildingTiles() const;

        /**
         * @brief Adds the content of `m_newBuildingTiles` to the layers.
         */
        void AddNewBuildingTiles();

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief An Aabb (axis-aligned bounding box) object for collision detection.
         *
         * The Aabb is constructed from startX and startY variables, along with width and height.
         * These define the location of the island in the world, indicating where it starts and ends.
         */
        physics::Aabb m_aabb;

        /**
         * @brief The island terrain tile layers (COAST, TERRAIN, BUILDINGS, MIXED).
         */
        std::unordered_map<layer::LayerType, std::unique_ptr<layer::TerrainLayer<tile::TerrainTile>>, EnumClassHash> m_terrainLayers;

        /**
         * @brief The island figures layer.
         */
        std::unique_ptr<layer::FiguresLayer<tile::FigureTile>> m_figuresLayer;

        /**
         * @brief Tiles to create a new building.
         */
        std::optional<std::vector<tile::TerrainTile>> m_newBuildingTiles;

        //-------------------------------------------------
        // Set layer data from Json
        //-------------------------------------------------

        /**
         * @brief Sets the size, position and climate zone of the island from a Json value.
         *
         * @param t_json The Json value containing the data to set.
         */
        void SetIslandData(const nlohmann::json& t_json);

        /**
         * @brief Sets the island's tile data of all layers using a Json value.
         *
         * @param t_json The Json value containing the tile data to set.
         */
        void SetLayerData(const nlohmann::json& t_json);

        /**
         * @brief Sets the tile data of the given island layer type using a Json value.
         *
         * @param t_json The Json value containing the tile data to set.
         * @param t_layerType The type of island layer.
         */
        void SetLayerDataByType(const nlohmann::json& t_json, layer::LayerType t_layerType);

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
        // Init layer data
        //-------------------------------------------------

        /**
         * @brief Initialises the tile data of the COAST, TERRAIN and BUILDINGS island layer.
         */
        void InitLayerData();

        /**
         * @brief Initialises the mixed layer of the island, which is created from all other layers.
         */
        void InitMixedLayerData();

        /**
         * @brief Populates the mixed layer with data from other layers.
         *
         * @param t_x The tile x position.
         * @param t_y The tile y position.
         */
        void PopulateMixedLayer(int t_x, int t_y);

        /**
         * @brief Checks if a tile should be replaced in the mixed layer.
         *
         * @param t_layer Layer to check the tile from.
         * @param t_index Index of the tile to check.
         *
         * @return True if the tile should be replaced; false otherwise.
         */
        bool ShouldReplaceTile(const layer::TerrainLayer<tile::TerrainTile>* t_layer, int t_index);
    };

    //-------------------------------------------------
    // Serializing Island into Json
    //-------------------------------------------------

    void to_json(nlohmann::json& t_json, const Island& t_island);
}
