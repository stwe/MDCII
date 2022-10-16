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

#include "WorldLayer.h"
#include "data/Buildings.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::state
{
    /**
     * Forward declaration struct Context.
     */
    struct Context;
}

namespace mdcii::map
{
    /**
     * Forward declaration class TileAtlas.
     */
    class TileAtlas;
}

namespace mdcii::renderer
{
    /**
     * Forward declaration class WorldRenderer.
     */
    class WorldRenderer;
}

//-------------------------------------------------
// World
//-------------------------------------------------

namespace mdcii::world
{
    //-------------------------------------------------
    // Forward declarations
    //-------------------------------------------------

    /**
     * Forward declaration class MousePicker.
     */
    class MousePicker;

    /**
     * Forward declaration class WorldGui.
     */
    class WorldGui;

    //-------------------------------------------------
    // World
    //-------------------------------------------------

    /**
     * Loads and shows the world map.
     */
    class World
    {
    public:
        //-------------------------------------------------
        // Actions
        //-------------------------------------------------

        /**
         * The possible world actions.
         */
        enum class Action
        {
            BUILD,   // Change the building or the terrain.
            STATUS,  // Get information about the tiles.
            OPTIONS  // Change game settings.
        };

        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * The (untranslated) labels of the action buttons.
         */
        static constexpr std::array<std::string_view, magic_enum::enum_count<Action>()> ACTION_NAMES{ "Build", "Status", "Options" };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The world width.
         */
        int width{ -1 };

        /**
         * The world height.
         */
        int height{ -1 };

        /**
         * The world rotation.
         */
        map::Rotation rotation{ map::Rotation::DEG0 };

        /**
         * The world zoom.
         */
        map::Zoom zoom{ map::Zoom::GFX };

        /**
         * The world layers.
         */
        std::vector<std::unique_ptr<WorldLayer>> layers;

        /**
         * To have access to the shared objects (Window, Camera, original assets).
         */
        std::shared_ptr<state::Context> context;

        /**
         * An OpenGL texture array for each zoom level.
         */
        std::unique_ptr<map::TileAtlas> tileAtlas;

        /**
         * The renderer to show the world.
         */
        std::unique_ptr<renderer::WorldRenderer> worldRenderer;

        /**
         * A MousePicker object to select tiles.
         */
        std::unique_ptr<MousePicker> mousePicker;

        /**
         * Indicates which action button is currently active.
         */
        std::array<bool, magic_enum::enum_count<Action>()> actionButtons{ true, false, false };

        /**
         * The current action.
         */
        Action currentAction{ Action::BUILD };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        World() = delete;

        /**
         * Constructs a new World object.
         *
         * @param t_mapFilePath The path to the Json map file.
         * @param t_context Access to shared objects.
         */
        World(std::string t_mapFilePath, std::shared_ptr<state::Context> t_context);

        World(const World& t_other) = delete;
        World(World&& t_other) noexcept = delete;
        World& operator=(const World& t_other) = delete;
        World& operator=(World&& t_other) noexcept = delete;

        ~World() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * Get WorldLayer object by type.
         *
         * @param t_layerType The type of the layer.
         */
        [[nodiscard]] const WorldLayer& GetLayer(WorldLayerType t_layerType) const;

        /**
         * Get WorldLayer object by type.
         *
         * @param t_layerType The type of the layer.
         */
        WorldLayer& GetLayer(WorldLayerType t_layerType);

        //-------------------------------------------------
        // Render
        //-------------------------------------------------

        /**
         * Renders the world.
         */
        void Render() const;

        /**
         * Renders ImGui menus.
         */
        void RenderImGui();

        //-------------------------------------------------
        // Rotate && Zoom
        //-------------------------------------------------

        /**
         * Changes the rotation of the world.
         *
         * @param t_changeRotation Rotate left or right.
         */
        void Rotate(map::ChangeRotation t_changeRotation);

        /**
         * Changes the zoom of the world.
         *
         * @param t_changeZoom Zoom in or out.
         */
        void Zoom(map::ChangeZoom t_changeZoom);

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Checks whether a given position is in the world.
         *
         * @param t_x The x position to check.
         * @param t_x The y position to check.
         *
         * @return True or false.
         */
        [[nodiscard]] bool IsPositionInWorld(int t_x, int t_y) const;

        /**
         * 2D/1D - mapping.
         *
         * @param t_x The x position in the world.
         * @param t_y The y position in the world.
         *
         * @return The 1D index.
         */
        [[nodiscard]] int GetMapIndex(int t_x, int t_y) const;

        /**
         * 2D/1D - mapping.
         *
         * @param t_x The x position in the world.
         * @param t_y The y position in the world.
         * @param t_rotation The position is previously rotated by the specified value.
         *
         * @return The 1D index.
         */
        [[nodiscard]] int GetMapIndex(int t_x, int t_y, map::Rotation t_rotation) const;

        /**
         * Projects world coordinates into an isometric position on the screen (world space).
         *
         * @param t_x The x position in the world.
         * @param t_y The y position in the world.
         * @param t_zoom The zoom to get the tile sizes.
         * @param t_rotation The position is previously rotated by the specified value.
         *
         * @return The isometric coordinates on the screen.
         */
        [[nodiscard]] glm::vec2 WorldToScreen(int t_x, int t_y, map::Zoom t_zoom, map::Rotation t_rotation = map::Rotation::DEG0) const;

        /**
         * Rotates a world position.
         *
         * @param t_x The x position in the world to rotate.
         * @param t_y The y position in the world to rotate.
         * @param t_rotation The rotation.
         *
         * @return The rotated position.
         */
        [[nodiscard]] glm::ivec2 RotatePosition(int t_x, int t_y, map::Rotation t_rotation = map::Rotation::DEG0) const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The path to the Json map file.
         */
        std::string m_mapFilePath;

        /**
         * The current layer type to render.
         */
        WorldLayerType m_renderLayerType{ WorldLayerType::TERRAIN_AND_BUILDINGS };

        /**
         * Toggles grid rendering on and off.
         */
        bool m_renderGridLayer{ false };

        /**
         * The index of the current selected Tile.
         */
        int m_currentTileIndex{ -1 };

        /**
         * Tiles used to create a building.
         */
        std::vector<std::unique_ptr<Tile>> m_tilesToAdd;

        /**
         * ImGui menus for the game.
         */
        std::unique_ptr<WorldGui> m_worldGui;

        //-------------------------------------------------
        // Event handler
        //-------------------------------------------------

        /**
         * Handles left mouse button pressed event.
         */
        void OnLeftMouseButtonPressed();

        /**
         * Handles mouse move.
         */
        void OnMouseMoved();

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initialize class.
         */
        void Init();

        /**
         * Adds event listeners.
         */
        void AddListeners();

        /**
         * Creates the Terrain- and Buildings Layer objects from Json value.
         */
        void CreateTerrainAndBuildingsLayers();

        /**
         * Prepares the Terrain- and Buildings Layer for rendering.
         * Calls PreCalcTile(), sorts tiles, creates model matrices and texture information.
         */
        void PrepareRendering();

        /**
         * Merge Terrain- with Buildings Layer to another new layer (TERRAIN_AND_BUILDINGS).
         */
        void MergeTerrainAndBuildingsLayers();

        /**
         * Creates the Grid Layer.
         */
        void CreateGridLayer();

        /**
         * Adds some pre-calculations to every Tile object of a Layer,
         * which are necessary for the display the Tile on the screen.
         *
         * @param t_tile The Tile object.
         * @param t_x The x position for Deg0 in the world.
         * @param t_y The y position for Deg0 in the world.
         */
        void PreCalcTile(Tile& t_tile, int t_x, int t_y) const;

        //-------------------------------------------------
        // Add building
        //-------------------------------------------------

        /**
         * Checks if the building is outside the world.
         *
         * @param t_mapX The start x position of the building.
         * @param t_mapY The start y position of the building.
         * @param t_building The Building object to check.
         *
         * @return True or false depending on whether the building is outside.
         */
        [[nodiscard]] bool IsBuildingOutsideTheWorld(int t_x, int t_y, const data::Building& t_building) const;

        /**
         * Checks whether there is already a building at the given position in the Building Layer.
         *
         * @param t_mapX The start x position of the building.
         * @param t_mapY The start y position of the building.
         * @param t_building The Building object to check.
         *
         * @return True or false, depending on whether a building already exists.
         */
        [[nodiscard]] bool IsAlreadyBuildingOnPosition(int t_x, int t_y, const data::Building& t_building) const;

        /**
         * Checks if the building is over the coast.
         *
         * @param t_mapX The start x position of the building.
         * @param t_mapY The start y position of the building.
         * @param t_building The Building object to check.
         *
         * @return True or false, depending on whether the building is over the coast.
         */
        [[nodiscard]] bool IsBuildingOnWaterOrCoast(int t_x, int t_y, const data::Building& t_building) const;
    };
}
