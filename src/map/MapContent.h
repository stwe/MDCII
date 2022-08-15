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

#include "MapLayer.h"
#include "data/Buildings.h"
#include "event/Event.h"
#include "Zoom.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::state
{
    struct Context;
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
         * Shared objects (Window, Camera, original assets) between all states.
         */
        std::shared_ptr<state::Context> context;

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
         * The zoom of each layer.
         */
        Zoom zoom{ Zoom::GFX };

        /**
         * The zoom of the building thumbnails.
         * In History Ed. only GFX is available.
         */
        Zoom bauhausZoom{ Zoom::GFX };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        MapContent() = delete;

        /**
         * Constructs a new MapContent object.
         *
         * @param t_mapFilePath The path to the json map file.
         * @param t_context Shared objects between all states.
         */
        MapContent(const std::string& t_mapFilePath, std::shared_ptr<state::Context> t_context);

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
        // Render
        //-------------------------------------------------

        /**
         * Renders ImGui menus.
         */
        void RenderImGui() const;

        //-------------------------------------------------
        // Sort
        //-------------------------------------------------

        /**
         * Sorts the entities of each layer for rendering.
         */
        void SortEntitiesOfAllLayers() const;

        //-------------------------------------------------
        // Rotate && Zoom
        //-------------------------------------------------

        /**
         * Rotate all layers left.
         */
        void RotateLeft();

        /**
         * Rotate all layers right.
         */
        void RotateRight();

        /**
         * Zoom all layers in.
         */
        void ZoomIn();

        /**
         * Zoom all layers out.
         */
        void ZoomOut();

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

        /**
         * Adds the BuildingsLayerTileComponent to the tile entity.
         *
         * @param t_mapX The start map x position of a building.
         * @param t_mapY The start map y position of a building.
         * @param t_selectedBauGfx Information about the building to be add.
         */
        void AddBuildingsLayerComponent(int t_mapX, int t_mapY, const event::SelectedBauGfx& t_selectedBauGfx);

        /**
         * Removes the BuildingsLayerTileComponent from the tile entity.
         *
         * @param t_mapX The start map x position of a building.
         * @param t_mapY The start map y position of a building.
         * @param t_selectedBauGfx Information about the building to be remove.
         */
        void RemoveBuildingsLayerComponent(int t_mapX, int t_mapY, const event::SelectedBauGfx& t_selectedBauGfx);

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

        /**
         * Initializes the zoom for all the bauhaus thumbnails in main menu.
         */
        void InitBauhausZoom();

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Checks if the building is outside the map.
         *
         * @param t_mapX The start map x position of the building.
         * @param t_mapY The start map y position of the building.
         * @param t_building The Building object to check.
         *
         * @return True or false depending on whether the building is outside.
         */
        [[nodiscard]] bool IsBuildingOutsideTheMap(int t_mapX, int t_mapY, const data::Building& t_building) const;

        /**
         * Checks whether there is already a building at the given position in the layer.
         *
         * @param t_mapX The start map x position of the building in the layer.
         * @param t_mapY The start map y position of the building in the layer.
         * @param t_building The Building object to check.
         * @param t_layerType The Layer object to check.
         *
         * @return True or false, depending on whether a building already exists.
         */
        [[nodiscard]] bool IsAlreadyBuildingOnLayer(int t_mapX, int t_mapY, const data::Building& t_building, LayerType t_layerType) const;

        /**
         * Checks if the building is over the coast.
         *
         * @param t_mapX The start map x position of the building in the layer.
         * @param t_mapY The start map y position of the building in the layer.
         * @param t_building The Building object to check.
         *
         * @return True or false, depending on whether the building is over the coast.
         */
        [[nodiscard]] bool IsBuildingOnWaterOrCoast(int t_mapX, int t_mapY, const data::Building& t_building) const;

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
