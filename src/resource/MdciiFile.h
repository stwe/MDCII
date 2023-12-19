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

    /**
     * @brief Forward declaration class Island.
     */
    class Island;

    /**
     * @brief Forward declaration class Layer.
     */
    class Layer;

    /**
     * @brief Forward declaration enum class LayerType.
     */
    enum class LayerType;

    /**
     * @brief Forward declaration enum class ClimateZone.
     */
    enum class ClimateZone;

    /**
     * @brief Forward declaration struct Tile.
     */
    struct Tile;
}

namespace mdcii::resource
{
    //-------------------------------------------------
    // MdciiFile
    //-------------------------------------------------

    /**
     * @brief The class is for all MDCII Json files.
     */
    class MdciiFile
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        MdciiFile() = delete;

        /**
         * @brief Constructs a new MdciiFile object.
         *
         * @param t_fileName The name of the file.
         */
        explicit MdciiFile(std::string t_fileName);

        MdciiFile(const MdciiFile& t_other) = delete;
        MdciiFile(MdciiFile&& t_other) noexcept = delete;
        MdciiFile& operator=(const MdciiFile& t_other) = delete;
        MdciiFile& operator=(MdciiFile&& t_other) noexcept = delete;

        ~MdciiFile() noexcept;

        //-------------------------------------------------
        // Content to Json value
        //-------------------------------------------------

        /**
         * @brief Loads the Json value from a file.
         *
         * @return True if successful, false if an error occurs while loading.
         */
        [[nodiscard]] bool LoadJsonFromFile();

        void SetIslandJson(
            int t_width, int t_height,
            world::ClimateZone t_climateZone,
            const std::vector<world::Tile>& t_terrainTiles,
            const std::vector<world::Tile>& t_coastTiles
        );

        //-------------------------------------------------
        // Content from Json value
        //-------------------------------------------------

        /**
         * @brief Create world content (world width/height and islands) from Json value.
         *
         * @param t_world Pointer to the World object to set the content.
         */
        void CreateWorldContentFromJson(world::World* t_world) const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief The name of the file.
         */
        std::string m_fileName;

        /**
         * @brief The current Json value.
         */
        nlohmann::json m_json;

        //-------------------------------------------------
        // Layer && Tiles
        //-------------------------------------------------

        /**
         * @brief Init Layer by type.
         *
         * @param t_island Pointer to the Island object.
         * @param t_vars JSON variables related to the layer details.
         * @param t_layerType The type of layer.
         */
        static void InitLayerByType(world::Island* t_island, const nlohmann::json& t_vars, world::LayerType t_layerType);

        /**
         * @brief Extract tile data from JSON and save it to the Tile object.
         *
         * @param t_world Pointer to the World object.
         * @param t_source Source JSON data.
         * @param t_tileTarget Pointer to the target Tile object.
         */
        static void ExtractTileData(const world::World* t_world, nlohmann::json const& t_source, world::Tile* t_tileTarget);

        /**
         * @brief Create layer tiles according to the provided JSON data.
         *
         * @param t_world Pointer to the World object.
         * @param t_layer Pointer to the target Layer object.
         * @param t_layerTilesJson Source JSON data.
         */
        static void CreateLayerTiles(const world::World* t_world, world::Layer* t_layer, const nlohmann::json& t_layerTilesJson);
    };
}
