#pragma once

#include <vector>
#include "MapTile.h"
#include "data/json.hpp"

namespace mdcii::map
{
    //-------------------------------------------------
    // Types conversions
    //-------------------------------------------------

    inline void to_json(nlohmann::json& t_json, const MapTile& t_mapTile)
    {
        t_json = nlohmann::json{
            { "id", t_mapTile.buildingId },
            { "orientation", t_mapTile.orientation },
            { "width", t_mapTile.width },
            { "height", t_mapTile.height },
            { "x", t_mapTile.x },
            { "y", t_mapTile.y }
        };
    }

    inline void from_json(const nlohmann::json& t_json, MapTile& t_mapTile)
    {
        t_json.at("id").get_to(t_mapTile.buildingId);
        t_json.at("orientation").get_to(t_mapTile.orientation);
        t_json.at("width").get_to(t_mapTile.width);
        t_json.at("height").get_to(t_mapTile.height);
        t_json.at("x").get_to(t_mapTile.x);
        t_json.at("y").get_to(t_mapTile.y);
    }

    //-------------------------------------------------
    // MapContent
    //-------------------------------------------------

    /**
     * Loads all tiles from a json file.
     */
    class MapContent
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The MapTile objects.
         */
        std::vector<MapTile> mapTiles;

        /**
         * The width of the map in tiles.
         */
        int width{ 0 };

        /**
         * The height of the map in tiles.
         */
        int height{ 0 };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        MapContent() = delete;

        /**
         * Constructs a new MapContent object.
         *
         * @param t_filePath The path to the map file.
         */
        explicit MapContent(const std::string& t_filePath);

        MapContent(const MapContent& t_other) = delete;
        MapContent(MapContent&& t_other) noexcept = delete;
        MapContent& operator=(const MapContent& t_other) = delete;
        MapContent& operator=(MapContent&& t_other) noexcept = delete;

        ~MapContent() noexcept;

    protected:

    private:
        //-------------------------------------------------
        // Read
        //-------------------------------------------------

        /**
         * Create tiles from the json data.
         *
         * @param t_filePath The path to the json map file.
         */
        void CreateTiles(const std::string& t_filePath);

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Read file and deserialize json.
         *
         * @param t_filePath The path to the json map file.
         */
        [[nodiscard]] static nlohmann::json ReadJsonFromFile(const std::string& t_filePath);
    };
}
