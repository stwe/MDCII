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
     * Represents a layer. A layer consists of many tiles.
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
         * @param t_width The width of the layer in tiles.
         * @param t_height The height of the layer in tiles.
         * @param t_buildings Access to all building objects.
         */
        MapLayer(int t_width, int t_height, std::shared_ptr<data::Buildings> t_buildings);

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
         * Adds a MapTile object to the layer.
         *
         * @param t_mapTile The MapTile object to add.
         */
        void AddTile(const MapTile& t_mapTile);

        /**
         * Replaces a MapTile object at a specified position.
         *
         * @param t_mapX The x position of the layer.
         * @param t_mapY The y position of the layer.
         * @param t_mapTile The new MapTile object.
         */
        void ReplaceTile(int t_mapX, int t_mapY, const MapTile& t_mapTile);

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
         * Creates all entities.
         */
        void CreateEntities();

        /**
         * Sorts entities with TileComponent for rendering.
         *
         * @param t_rotation The rotation to sort for.
         */
        static void SortEntities(Rotation t_rotation);

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Projects layer coordinates into an isometric position
         * on the screen (world space).
         *
         * @param t_mapX The x position of the layer.
         * @param t_mapY The y position of the layer.
         * @param t_rotation The position is previously rotated by the specified value.
         *
         * @return The isometric coordinates on the screen.
         */
        [[nodiscard]] glm::vec2 LayerToScreen(int t_mapX, int t_mapY, Rotation t_rotation = Rotation::DEG0) const;

        /**
         * Checks whether a layer position is in layer.
         *
         * @param t_position The position to check.
         *
         * @return True or false depending on the position in the layer.
         */
        [[nodiscard]] bool IsPositionInLayer(const glm::ivec2& t_position) const;

        /**
         * Rotates a layer position.
         *
         * @param t_mapX The x position of the layer to rotate.
         * @param t_mapY The y position of the layer to rotate.
         * @param t_rotation The rotation.
         *
         * @return The rotated position.
         */
        [[nodiscard]] glm::ivec2 RotatePosition(int t_mapX, int t_mapY, Rotation t_rotation = Rotation::DEG0) const;

        /**
         * 2D/1D - mapping.
         *
         * @param t_mapX The x position of the layer.
         * @param t_mapY The y position of the layer.
         * @param t_rotation The position is previously rotated by the specified value.
         *
         * @return The index.
         */
        [[nodiscard]] int GetMapIndex(int t_mapX, int t_mapY, Rotation t_rotation = Rotation::DEG0) const;

        //-------------------------------------------------
        // Precalculations
        //-------------------------------------------------

        /**
         * Adds some pre-calculations to the tile objects,
         * which are necessary for the display on the screen.
         */
        void PreCalcTiles();

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The width of the layer in tiles.
         */
        int m_width{ 0 };

        /**
         * The height of the layer in tiles.
         */
        int m_height{ 0 };

        /**
         * The content from the haeuser.cod.
         */
        std::shared_ptr<data::Buildings> m_buildings;

        //-------------------------------------------------
        // Ecs
        //-------------------------------------------------

        /**
         * Creates a new entity.
         *
         * @return The created EnTT entity.
         */
        static entt::entity CreatePlainEntity();

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
         * specified location.
         *
         * @param t_mapX The x position of the layer.
         * @param t_mapY The y position of the layer.
         */
        void CreateEntity(int t_mapX, int t_mapY);

        //-------------------------------------------------
        // Precalculations
        //-------------------------------------------------

        /**
         * Adds some pre-calculations to the tile object,
         * which are necessary for the display on the screen.
         *
         * @param t_mapTile The tile object.
         * @param t_mapX The x position of the layer.
         * @param t_mapY The y position of the layer.
         */
        void PreCalcTile(MapTile& t_mapTile, int t_mapX, int t_mapY) const;
    };
}
