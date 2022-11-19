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
#include <unordered_map>
#include "Tile.h"
#include "data/json.hpp"
#include "data/Buildings.h"

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

    void to_json(nlohmann::json& t_json, const std::shared_ptr<Tile>& t_tile);
    void from_json(const nlohmann::json& t_json, Tile& t_tile);

    //-------------------------------------------------
    // WorldLayerType
    //-------------------------------------------------

    /**
     * The possible Layer types.
     */
    enum class WorldLayerType
    {
        TERRAIN,
        BUILDINGS,
        TERRAIN_AND_BUILDINGS,
        GRID,
        NONE
    };

    //-------------------------------------------------
    // WorldLayer
    //-------------------------------------------------

    /**
     * Represents a Layer, which is simply a collection of Tile objects.
     */
    class WorldLayer
    {
    public:
        //-------------------------------------------------
        // Tile types
        //-------------------------------------------------

        /**
         * A vector of Tile pointers.
         */
        using Tiles = std::vector<std::shared_ptr<Tile>>;

        /**
         * A vector of Tile pointers for each rotation.
         */
        using Tiles_For_Each_Rotation = std::array<Tiles, NR_OF_ROTATIONS>;

        //-------------------------------------------------
        // Model matrices types
        //-------------------------------------------------

        /**
         * A vector of model matrices.
         */
        using Model_Matrices = std::vector<glm::mat4>;

        /**
         * A vector of model matrices for each rotation.
         */
        using Model_Matrices_For_Each_Rotation = std::array<Model_Matrices, NR_OF_ROTATIONS>;

        /**
         * Four model matrices containers for each of the three possible zoom levels.
         */
        using Model_Matrices_For_Each_Zoom = std::array<Model_Matrices_For_Each_Rotation, NR_OF_ZOOMS>;

        //-------------------------------------------------
        // Texture heights types
        //-------------------------------------------------

        /**
         * A vector of texture heights.
         */
        using Texture_Heights = std::vector<glm::vec4>;

        /**
         * A vector of texture heights for each zoom level.
         */
        using Texture_Heights_For_Each_Zoom = std::array<Texture_Heights, NR_OF_ZOOMS>;

        //-------------------------------------------------
        // Gfx info
        //-------------------------------------------------

        /**
         * A vector of gfx info.
         */
        using Gfx_Info = std::vector<glm::ivec4>;

        /**
         * A vector of gfx info for each zoom level.
         */
        using Gfx_Info_For_Each_Zoom = std::array<Gfx_Info, NR_OF_ZOOMS>;

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * Specifies the type of this Layer object.
         */
        WorldLayerType layerType{ WorldLayerType::NONE };

        /**
         * Contains all Tile pointers in the order DEG0.
         */
        Tiles tiles;

        /**
         * A vector of Tile pointers for each rotation.
         * Used to iterate over the Tile pointers in the correct order for each rotation.
         */
        Tiles_For_Each_Rotation sortedTiles;

        /**
         * With this, the instance Id can be determined with a rotated world position.
         * x = worldX
         * y = worldY
         * z = rotationInt
         * result = instance Id
         */
        std::unordered_map<glm::ivec3, int32_t> instanceIds;

        /**
         * Four model matrices containers for each of the three possible zoom levels
         */
        Model_Matrices_For_Each_Zoom modelMatrices;

        /**
         * A vector of texture heights for each zoom level.
         * The height of each texture.
         * x = height for rot0
         * y = height for rot90
         * z = height for rot180
         * w = height for rot270
         */
        Texture_Heights_For_Each_Zoom heights;

        /**
         * A vector of gfx numbers for each zoom level.
         * A gfx number for each tile.
         * x = gfx for rot0
         * y = gfx for rot90
         * z = gfx for rot180
         * w = gfx for rot270
         */
        Gfx_Info_For_Each_Zoom gfxNumbers;

        /**
         * The number of instances to render.
         */
        int32_t instancesToRender{ -1 };

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
         * There are four arrays with the model matrices for each zoom level.
         * The function returns the arrays.
         *
         * @param t_zoom The requested zoom level.
         *
         * @return The model matrices of the zoom.
         */
        [[nodiscard]] const Model_Matrices_For_Each_Rotation& GetModelMatrices(Zoom t_zoom) const;

        /**
         * Returns a Tile object.
         *
         * @param t_x The DEG0 x position.
         * @param t_y The DEG0 y position.
         *
         * @return The Tile object.
         */
        [[nodiscard]] const Tile& GetTile(int t_x, int t_y) const;

        /**
         * Checks whether there is already a building at the given position in this Layer.
         *
         * @param t_mapX The start x position of the building.
         * @param t_mapY The start y position of the building.
         * @param t_building The Building object to check.
         * @param t_buildingRotation The rotation of the building.
         *
         * @return True or false, depending on whether a building already exists.
         */
        [[nodiscard]] bool IsAlreadyBuildingOnPosition(int t_x, int t_y, const data::Building& t_building, Rotation t_buildingRotation) const;

        //-------------------------------------------------
        // Setter
        //-------------------------------------------------

        /**
         * Sets the type of this Layer object by a given string.
         *
         * @param t_layerType The Layer type as string.
         */
        void SetLayerTypeByString(const std::string& t_layerType);

        //-------------------------------------------------
        // Tiles
        //-------------------------------------------------

        /**
         * Adds a Tile pointer from a given Json value.
         *
         * @param t_json The Json value.
         */
        void AddTileFromJson(const nlohmann::json& t_json);

        /**
         * Resets Tile pointer at given instances in <i>tiles</i> and <i>sortedTiles</i>.
         *
         * @param t_instanceIds Instances at which the pointer should be deleted.
         */
        void ResetTilePointersAt(const std::array<int32_t, NR_OF_ROTATIONS>& t_instanceIds);

        /**
         * Stores a new Tile in the Layer.
         *
         * @param t_tile The Tile to be stored.
         */
        void StoreTile(std::unique_ptr<Tile> t_tile);

        //-------------------------------------------------
        // Prepare rendering
        //-------------------------------------------------

        /**
         * Prepare data for rendering.
         */
        void PrepareRendering();

        /**
         * Creates a model matrix for a given Tile object.
         *
         * @param t_tile The Tile object.
         * @param t_zoom The zoom for which to create the model matrix.
         * @param t_rotation The rotation for which to create the model matrix.
         *
         * @return The model matrix.
         */
        [[nodiscard]] glm::mat4 GetModelMatrix(const Tile& t_tile, Zoom t_zoom, Rotation t_rotation) const;

        /**
         * Determines the texture atlas number of a given Tile object.
         *
         * @param t_tile The Tile object.
         * @param t_zoom The zoom for which to determine the atlas number.
         * @param t_rotation The rotation for which to create the atlas number.
         *
         * @return The texture atlas number.
         */
        [[maybe_unused]] [[nodiscard]] int GetTextureAtlasNr(const Tile& t_tile, Zoom t_zoom, Rotation t_rotation) const;

        /**
         * Determines the texture offset of a given Tile object.
         *
         * @param t_tile The Tile object.
         * @param t_zoom The zoom for which to determine the offset.
         * @param t_rotation The rotation for which to create the offset.
         *
         * @return The offset.
         */
        [[maybe_unused]] [[nodiscard]] glm::vec2 GetTextureOffset(const Tile& t_tile, Zoom t_zoom, Rotation t_rotation) const;

        /**
         * Determines the texture height of a given Tile object.
         *
         * @param t_tile The Tile object.
         * @param t_zoom The zoom for which to determine the height.
         * @param t_rotation The rotation for which to create the height.
         *
         * @return The texture height.
         */
        [[nodiscard]] float GetTextureHeight(const Tile& t_tile, Zoom t_zoom, Rotation t_rotation) const;

        /**
         * Calculates the gfx considering the rotation of the Tile object and the World object.
         *
         * @param t_tile The Tile object.
         * @param t_rotation The world rotation.
         *
         * @return The gfx to use for rendering.
         */
        [[nodiscard]] int32_t CalcGfx(const Tile& t_tile, Rotation t_rotation) const;

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
         * Creates model matrices for rendering.
         */
        void CreateModelMatrices();

        /**
         * Creates information about which gfx is used for each instance to render.
         */
        void CreateGfxInfo();
    };
}
