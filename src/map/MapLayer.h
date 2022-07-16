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

#include "MapTile.h"
#include "data/json.hpp"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::data
{
    class Buildings;
}

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
    // Types, Types conversions && Operators
    //-------------------------------------------------

    enum class LayerType
    {
        TERRAIN, BUILDINGS, NONE
    };

    enum class Rotation
    {
        DEG0, DEG90, DEG180, DEG270
    };

    inline Rotation& operator++(Rotation& t_rotation)
    {
        if (t_rotation == Rotation::DEG270)
        {
            t_rotation = Rotation::DEG0;
        }
        else
        {
            t_rotation = static_cast<Rotation>(static_cast<std::underlying_type_t<Rotation>>(t_rotation) + 1);
        }

        return t_rotation;
    }

    inline Rotation& operator--(Rotation& t_rotation)
    {
        if (t_rotation == Rotation::DEG0)
        {
            t_rotation = Rotation::DEG270;
        }
        else
        {
            t_rotation = static_cast<Rotation>(static_cast<std::underlying_type_t<Rotation>>(t_rotation) - 1);
        }

        return t_rotation;
    }

    void to_json(nlohmann::json& t_json, const MapTile& t_mapTile);
    void from_json(const nlohmann::json& t_json, MapTile& t_mapTile);

    //-------------------------------------------------
    // MapLayer
    //-------------------------------------------------

    /**
     * Represents a layer with many tiles.
     */
    class MapLayer
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * Specifies the type of layer.
         */
        LayerType layerType{ LayerType::NONE };

        /**
         * The MapTile objects.
         */
        std::vector<MapTile> mapTiles;

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
        // Layer type
        //-------------------------------------------------

        /**
         * Sets the layer type by a given string.
         *
         * @param t_layerType The layer type string.
         */
        void SetLayerTypeByString(const std::string& t_layerType);

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
        // Get tile
        //-------------------------------------------------

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

        /**
         * Sorts entities with TerrainLayerTileComponent for rendering.
         *
         * @param t_rotation The rotation to sort for.
         */
        static void SortEntities(Rotation t_rotation);

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The parent MapContent object.
         */
        MapContent* m_mapContent{ nullptr };

        //-------------------------------------------------
        // Ecs
        //-------------------------------------------------

        /**
         * Creates a new entity.
         *
         * @return The created EnTT entity.
         */
        static entt::entity CreatePlainEntity();
    };
}
