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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#pragma once

#include <glm/mat4x4.hpp>
#include "MapTile.h"
#include "LayerType.h"
#include "data/json.hpp"

namespace mdcii::map
{
    //-------------------------------------------------
    // Forward declarations
    //-------------------------------------------------

    /**
     * Forward declaration class MapContent.
     */
    class MapContent;

    //-------------------------------------------------
    // Json
    //-------------------------------------------------

    void to_json(nlohmann::json& t_json, const MapTile& t_mapTile);
    void from_json(const nlohmann::json& t_json, MapTile& t_mapTile);

    //-------------------------------------------------
    // MapLayer
    //-------------------------------------------------

    /**
     * Represents a layer, which is simply a collection of MapTile objects.
     */
    class MapLayer
    {
    public:
        //-------------------------------------------------
        // Types
        //-------------------------------------------------

        /**
         * A MapTile container.
         */
        using Map_Tiles = std::vector<MapTile>;

        /**
         * A MapTile container for each of the four possible rotations.
         */
        using Map_Tiles_For_Each_Rotation = std::array<Map_Tiles, NR_OF_ROTATIONS>;

        /**
         * A model matrices container.
         */
        using Model_Matrices = std::vector<glm::mat4>;

        /**
         * A model matrices container for each of the four possible rotations.
         */
        using Model_Matrices_For_Each_Rotation = std::array<Model_Matrices, NR_OF_ROTATIONS>;

        /**
         * Four model matrices containers for each of the three possible zoom levels.
         */
        using Model_Matrices_For_Each_Zoom = std::array<Model_Matrices_For_Each_Rotation, NR_OF_ZOOMS>;

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * Specifies the type of this layer.
         */
        LayerType layerType{ LayerType::NONE };

        /**
         * A MapTile container.
         */
        Map_Tiles mapTiles;

        /**
         * A MapTile container for each of the four possible rotations.
         */
        Map_Tiles_For_Each_Rotation sortedMapTiles;

        /**
         * Four model matrices containers for each of the three possible zoom levels
         */
        Model_Matrices_For_Each_Zoom modelMatrices;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        MapLayer() = delete;

        /**
         * Constructs a new MapLayer object.
         *
         * @param t_mapContent The parent MapContent object.
         */
        explicit MapLayer(MapContent* t_mapContent);

        MapLayer(const MapLayer& t_other) = delete;
        MapLayer(MapLayer&& t_other) noexcept = delete;
        MapLayer& operator=(const MapLayer& t_other) = delete;
        MapLayer& operator=(MapLayer&& t_other) noexcept = delete;

        ~MapLayer() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * There are four arrays with the model matrices for each zoom level.
         * The function returns the arrays.
         *
         * @param t_zoom The requested zoom level.
         *
         * @return The model matrices of the zoom.
         */
        [[nodiscard]] const Model_Matrices_For_Each_Rotation& GetModelMatrices(Zoom t_zoom) const;

        /**
         * Returns a MapTile.
         *
         * @param t_mapX The x position of the layer.
         * @param t_mapY The y position of the layer.
         *
         * @return The MapTile object.
         */
        [[nodiscard]] const MapTile& GetTile(int t_mapX, int t_mapY) const;

        /**
         * Returns a MapTile.
         *
         * @param t_mapX The x position of the layer.
         * @param t_mapY The y position of the layer.
         *
         * @return The MapTile object.
         */
        MapTile& GetTile(int t_mapX, int t_mapY);

        /**
         * Returns the number of instances to render.
         *
         * @return The number of instances.
         */
        [[nodiscard]] int32_t GetInstances() const { return static_cast<int32_t>(mapTiles.size()); }

        //-------------------------------------------------
        // Setter
        //-------------------------------------------------

        /**
         * Sets the type of this layer by a given string.
         *
         * @param t_layerType The layer type to set.
         */
        void SetLayerTypeByString(const std::string& t_layerType);

        //-------------------------------------------------
        // Instanced rendering
        //-------------------------------------------------

        /**
         * Sort MapTile objects for instanced rendering.
         */
        void SortMapTiles();

        /**
         * Create model matrices for instanced rendering.
         */
        void CreateModelMatrices();

        //-------------------------------------------------
        // Add/replace tile
        //-------------------------------------------------

        /**
         * Creates and saves a MapTile object from json in the layer.
         *
         * @param t_json The json entry to convert.
         */
        void AddTileFromJson(const nlohmann::json& t_json);

        /**
         * Replaces a MapTile object.
         *
         * @param t_mapTile The new MapTile object.
         */
        void ReplaceTile(const MapTile& t_mapTile);

        //-------------------------------------------------
        // Ecs
        //-------------------------------------------------

        /**
         * Creates a Grid-Entity from the MapTile object at the
         * specified location.
         *
         * @param t_mapX The x position of the layer.
         * @param t_mapY The y position of the layer.
         */
        void CreateGridEntity(int t_mapX, int t_mapY);

        /**
         * Creates an Entity from the MapTile object at the
         * specified location to show a terrain tile.
         *
         * @param t_mapX The x position of the layer.
         * @param t_mapY The y position of the layer.
         */
        void CreateTerrainLayerEntity(int t_mapX, int t_mapY);

        /**
         * A BuildingsLayerTileComponent is added to the entity
         * in case the terrain layer was modified by a building.
         *
         * @param t_mapX The x position of the layer.
         * @param t_mapY The y position of the layer.
         */
        void AddBuildingsLayerComponent(int t_mapX, int t_mapY);

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The parent MapContent object.
         */
        MapContent* m_mapContent{ nullptr };

        /**
         * Each zoom level has a different grid texture.
         */
        std::array<std::string, NR_OF_ZOOMS> m_gridFileNames{};

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initializes the class.
         */
        void Init();
    };
}
