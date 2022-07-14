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
     * Creates all tile layers from a json file.
     */
    class MapContent
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The width of the map in tiles.
         */
        int width{ 0 };

        /**
         * The height of the map in tiles.
         */
        int height{ 0 };

        /**
         * The map layers.
         */
        std::vector<std::unique_ptr<MapLayer>> mapLayers;

        /**
         * The map rotation.
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

        //-------------------------------------------------
        // Sort
        //-------------------------------------------------

        /**
         * Sorts the entities of all layers.
         */
        void SortEntitiesOfAllLayers() const;

        //-------------------------------------------------
        // Rotate
        //-------------------------------------------------

        /**
         * Rotate map content left.
         */
        void RotateLeft();

        /**
         * Rotate map content right.
         */
        void RotateRight();

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Checks whether a position is in map.
         *
         * @param t_position The position to check.
         *
         * @return True or false depending on the position in the map.
         */
        [[nodiscard]] bool IsPositionInMap(const glm::ivec2& t_position) const;

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

    protected:

    private:
        //-------------------------------------------------
        // Read
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
         * Creates the entities of all layers.
         */
        void CreateEntitiesOfAllLayers() const;
    };
}
