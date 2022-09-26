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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#pragma once

#include <glm/mat4x4.hpp>
#include "Tile.h"
#include "data/json.hpp"

//-------------------------------------------------
// WorldLayer
//-------------------------------------------------

namespace mdcii::world
{
    //-------------------------------------------------
    // Forward declarations
    //-------------------------------------------------

    /**
     * Forward declaration class World.
     */
    class World;

    //-------------------------------------------------
    // Json
    //-------------------------------------------------

    void to_json(nlohmann::json& t_json, const Tile& t_tile);
    void from_json(const nlohmann::json& t_json, Tile& t_tile);

    //-------------------------------------------------
    // WorldLayerType
    //-------------------------------------------------

    /**
     * The possible layer types.
     */
    enum class WorldLayerType
    {
        TERRAIN,
        BUILDINGS,
        TERRAIN_AND_BUILDINGS,
        NONE
    };

    //-------------------------------------------------
    // WorldLayer
    //-------------------------------------------------

    /**
     * Represents a layer, which is simply a collection of Tile objects.
     */
    class WorldLayer
    {
    public:
        //-------------------------------------------------
        // Tile types
        //-------------------------------------------------

        /**
         * A Tile container.
         */
        using Tiles = std::vector<Tile>;

        /**
         * A Tile container for each of the four possible rotations.
         */
        using Tiles_For_Each_Rotation = std::array<Tiles, map::NR_OF_ROTATIONS>;

        //-------------------------------------------------
        // Model matrices types
        //-------------------------------------------------

        /**
         * A model matrices container.
         */
        using Model_Matrices = std::vector<glm::mat4>;

        /**
         * A model matrices container for each of the four possible rotations.
         */
        using Model_Matrices_For_Each_Rotation = std::array<Model_Matrices, map::NR_OF_ROTATIONS>;

        /**
         * Four model matrices containers for each of the three possible zoom levels.
         */
        using Model_Matrices_For_Each_Zoom = std::array<Model_Matrices_For_Each_Rotation, map::NR_OF_ZOOMS>;

        //-------------------------------------------------
        // Texture indices types
        //-------------------------------------------------

        /**
         * A texture atlas indices container.
         */
        using Texture_Atlas_Indices = std::vector<glm::ivec4>;

        /**
         * A texture atlas indices container for each of the three possible zoom levels.
         */
        using Texture_Atlas_Indices_For_Each_Zoom = std::array<Texture_Atlas_Indices, map::NR_OF_ZOOMS>;

        //-------------------------------------------------
        // Texture offsets types
        //-------------------------------------------------

        /**
         * A texture offsets container.
         */
        using Texture_Offsets = std::vector<glm::vec2>;

        /**
         * A texture offsets container for each of the four possible rotations.
         */
        using Texture_Offsets_For_Each_Rotation = std::array<Texture_Offsets, map::NR_OF_ROTATIONS>;

        /**
         * Four texture offsets containers for each of the three possible zoom levels.
         */
        using Texture_Offsets_For_Each_Zoom = std::array<Texture_Offsets_For_Each_Rotation, map::NR_OF_ZOOMS>;

        //-------------------------------------------------
        // Texture heights types
        //-------------------------------------------------

        /**
         * A texture heights container.
         */
        using Texture_Heights = std::vector<glm::vec4>;

        /**
         * A texture heights container for each of the three possible zoom levels.
         */
        using Texture_Heights_For_Each_Zoom = std::array<Texture_Heights, map::NR_OF_ZOOMS>;

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * Specifies the type of this layer.
         */
        WorldLayerType layerType{ WorldLayerType::NONE };

        /**
         * A Tile container.
         */
        Tiles tiles;

        /**
         * A Tile container for each of the four possible rotations.
         * Used to iterate over the tiles in the correct order for each rotation.
         */
        Tiles_For_Each_Rotation sortedTiles;

        /**
         * Four model matrices containers for each of the three possible zoom levels
         */
        Model_Matrices_For_Each_Zoom modelMatrices;

        /**
         * A texture atlas indices container for each of the three possible zoom levels.
         * Specifies from which atlas image the texture for each tile is to be taken.
         * x = number of atlas image for rot0
         * y = number of atlas image for rot90
         * z = number of atlas image for rot180
         * w = number of atlas image for rot270
         */
        Texture_Atlas_Indices_For_Each_Zoom textureAtlasIndices;

        /**
         * Four texture offsets containers for each of the three possible zoom levels.
         */
        Texture_Offsets_For_Each_Zoom offsets;

        /**
         * A texture heights container for each of the three possible zoom levels.
         * The height of each texture.
         * x = height for rot0
         * y = height for rot90
         * z = height for rot180
         * w = height for rot270
         */
        Texture_Heights_For_Each_Zoom heights;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        WorldLayer() = delete;

        /**
         * Constructs a new WorldLayer object.
         *
         * @param t_world The parent World object.
         */
        explicit WorldLayer(World* t_world);

        WorldLayer(const WorldLayer& t_other) = delete;
        WorldLayer(WorldLayer&& t_other) noexcept = delete;
        WorldLayer& operator=(const WorldLayer& t_other) = delete;
        WorldLayer& operator=(WorldLayer&& t_other) noexcept = delete;

        ~WorldLayer() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * Returns a Tile.
         *
         * @param t_x The x position in the world.
         * @param t_y The y position in the world.
         *
         * @return The Tile object.
         */
        [[nodiscard]] const Tile& GetTile(int t_x, int t_y) const;

        /**
         * Returns a Tile.
         *
         * @param t_x The x position in the world.
         * @param t_y The y position in the world.
         *
         * @return The Tile object.
         */
        Tile& GetTile(int t_x, int t_y);

        /**
         * Returns the number of instances to render.
         *
         * @return The number of instances.
         */
        [[nodiscard]] int32_t GetInstances() const { return static_cast<int32_t>(tiles.size()); }

        /**
         * There are four arrays with the model matrices for each zoom level.
         * The function returns the arrays.
         *
         * @param t_zoom The requested zoom level.
         *
         * @return The model matrices of the zoom.
         */
        [[nodiscard]] const Model_Matrices_For_Each_Rotation& GetModelMatrices(map::Zoom t_zoom) const;

        //-------------------------------------------------
        // Layer type
        //-------------------------------------------------

        /**
         * Sets the type of this layer by a given string.
         *
         * @param t_layerType The layer type as string.
         */
        void SetLayerTypeByString(const std::string& t_layerType);

        //-------------------------------------------------
        // Add/replace tile
        //-------------------------------------------------

        /**
         * Adds a Tile object from a given Json value.
         *
         * @param t_json The Json value.
         */
        void AddTileFromJson(const nlohmann::json& t_json);

        //-------------------------------------------------
        // Prepare rendering
        //-------------------------------------------------

        /**
         * Prepare data for rendering.
         */
        void PrepareRendering();

    protected:

    private:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * If no data is available, the instance is created with this Id.
         */
        static constexpr auto GRASS_BUILDING_ID{ 101 };

        /**
         * If no data is available, the instance is created with this Gfx.
         */
        static constexpr auto GRASS_GFX{ 4 };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The parent World object.
         */
        World* m_world{ nullptr };

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Sort Tile objects for rendering.
         */
        void SortTiles();

        /**
         * Create model matrices for rendering.
         */
        void CreateModelMatrices();

        /**
         * Stores information about which texture is used for each instance to render.
         */
        void CreateTextureInfo();

        /**
         * Calculates the gfx considering the rotation of the Tile object and the World object.
         *
         * @param t_tile The Tile object.
         * @param t_rotation The world rotation.
         *
         * @return The gfx to use for rendering.
         */
        [[nodiscard]] int32_t CalcGfx(const Tile& t_tile, map::Rotation t_rotation) const;
    };
}
