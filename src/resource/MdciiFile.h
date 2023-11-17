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
        // Member
        //-------------------------------------------------

        /**
         * @brief The name of the file.
         */
        std::string fileName;

        /**
         * @brief The current Json value.
         */
        nlohmann::json json;

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
        // Save && load
        //-------------------------------------------------

        /**
         * @brief Loads the Json value from a file.
         *
         * @return True if successful, false if an error occurs while loading.
         */
        [[nodiscard]] bool LoadJsonFromFile();

        /**
         * @brief Create world content and return a vector of unique pointers to the islands.
         *
         * @param t_worldWidth The width of the world is passed by reference and modified.
         * @param t_worldHeight The height of the world is passed by reference and modified.
         *
         * @return A vector of unique pointers to the islands.
         */
        [[nodiscard]] std::vector<std::unique_ptr<world::Island>> CreateWorldContent(int& t_worldWidth, int& t_worldHeight) const;

    protected:

    private:
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
         * @param t_source Source JSON data.
         * @param t_tileTarget Pointer to the target Tile object.
         */
        static void ExtractTileData(nlohmann::json const& t_source, world::Tile* t_tileTarget);

        /**
         * @brief Create layer tiles according to the provided JSON data.
         *
         * @param t_layer Pointer to the target Layer object.
         * @param t_layerTilesJson Source JSON data.
         */
        static void CreateLayerTiles(world::Layer* t_layer, const nlohmann::json& t_layerTilesJson);
    };
}
