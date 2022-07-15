#pragma once

#include <memory>
#include "MapLayer.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::data
{
    class Buildings;
}

//-------------------------------------------------
// MapContent
//-------------------------------------------------

namespace mdcii::map
{
    /**
     * Creates and manages all layers.
     */
    class MapContent
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The width of each layer in tiles.
         */
        int width{ 0 };

        /**
         * The height of each layer in tiles.
         */
        int height{ 0 };

        /**
         * The layers of the map.
         */
        std::vector<std::unique_ptr<MapLayer>> mapLayers;

        /**
         * The rotation of each layer.
         */
        Rotation rotation{ Rotation::DEG0 };

        /**
         * The content from the haeuser.cod.
         */
        std::shared_ptr<data::Buildings> buildings;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        MapContent() = delete;

        /**
         * Constructs a new MapContent object.
         *
         * @param t_filePath The path to the json map file.
         * @param t_buildings Access to all building objects.
         */
        MapContent(const std::string& t_filePath, std::shared_ptr<data::Buildings> t_buildings);

        MapContent(const MapContent& t_other) = delete;
        MapContent(MapContent&& t_other) noexcept = delete;
        MapContent& operator=(const MapContent& t_other) = delete;
        MapContent& operator=(MapContent&& t_other) noexcept = delete;

        ~MapContent() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * For convenience: Get MapLayer object by type.
         *
         * @param t_layerType The type of the layer.
         */
        [[nodiscard]] const MapLayer& GetLayer(LayerType t_layerType) const;

        /**
         * For convenience: Get MapLayer object by type.
         *
         * @param t_layerType The type of the layer.
         */
        MapLayer& GetLayer(LayerType t_layerType);

        //-------------------------------------------------
        // Sort
        //-------------------------------------------------

        /**
         * Sorts the entities of each layer for rendering.
         */
        void SortEntitiesOfAllLayers() const;

        //-------------------------------------------------
        // Rotate
        //-------------------------------------------------

        /**
         * Rotate all layers left.
         */
        void RotateLeft();

        /**
         * Rotate all layers right.
         */
        void RotateRight();

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Checks whether a position is in map.
         *
         * @param t_mapX The x position of the map to check.
         * @param t_mapY The y position of the map to check.
         *
         * @return True or false depending on the position in the map.
         */
        [[nodiscard]] bool IsPositionInMap(int t_mapX, int t_mapY) const;

        /**
         * 2D/1D - mapping.
         *
         * @param t_mapX The x position of the map.
         * @param t_mapY The y position of the map.
         * @param t_rotation The position is previously rotated by the specified value.
         *
         * @return The index.
         */
        [[nodiscard]] int GetMapIndex(int t_mapX, int t_mapY, Rotation t_rotation = Rotation::DEG0) const;

        /**
         * Projects map coordinates into an isometric position
         * on the screen (world space).
         *
         * @param t_mapX The x position of the map.
         * @param t_mapY The y position of the map.
         * @param t_rotation The position is previously rotated by the specified value.
         *
         * @return The isometric coordinates on the screen.
         */
        [[nodiscard]] glm::vec2 MapToScreen(int t_mapX, int t_mapY, Rotation t_rotation = Rotation::DEG0) const;

        /**
         * Rotates a map position.
         *
         * @param t_mapX The x position of the map to rotate.
         * @param t_mapY The y position of the map to rotate.
         * @param t_rotation The rotation.
         *
         * @return The rotated position.
         */
        [[nodiscard]] glm::ivec2 RotatePosition(int t_mapX, int t_mapY, Rotation t_rotation = Rotation::DEG0) const;

        //-------------------------------------------------
        // Edit
        //-------------------------------------------------

        // todo

        void AddBuilding(int t_mapX, int t_mapY, int t_buildingId, int t_orientation, int t_x, int t_y);

    protected:

    private:
        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Creates the layers from the json data.
         *
         * @param t_filePath The path to the json map file.
         */
        void CreateContent(const std::string& t_filePath);

        /**
         * Read file and deserialize json.
         *
         * @param t_filePath The path to the json map file.
         */
        [[nodiscard]] static nlohmann::json ReadJsonFromFile(const std::string& t_filePath);

        //-------------------------------------------------
        // Ecs
        //-------------------------------------------------

        /**
         * Creates entities with components for each layer.
         */
        void CreateEntitiesOfAllLayers() const;

        //-------------------------------------------------
        // Pre-calculations
        //-------------------------------------------------

        /**
         * Adds some pre-calculations to each layer tile,
         * which are necessary for the display on the screen.
         */
        void PreCalcTiles() const;

        /**
         * Adds some pre-calculations to a tile object,
         * which are necessary for the display on the screen.
         *
         * @param t_mapTile The tile object.
         * @param t_mapX The x position of the map.
         * @param t_mapY The y position of the map.
         */
        void PreCalcTile(MapTile& t_mapTile, int t_mapX, int t_mapY) const;
    };
}
