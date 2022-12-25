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

#include <magic_enum.hpp>
#include <glm/vec2.hpp>
#include "event/EventManager.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::layer
{
    /**
     * Forward declaration enum class LayerType.
     */
    enum class LayerType;

    /**
     * Forward declaration class WorldLayer.
     */
    class WorldLayer;

    /**
     * Forward declaration class WorldGridLayer.
     */
    class WorldGridLayer;
}

namespace mdcii::state
{
    /**
     * Forward declaration struct Context.
     */
    struct Context;

    /**
     * Forward declaration enum class StateId.
     */
    enum class StateId;
}

namespace mdcii::renderer
{
    /**
     * Forward declaration class TerrainRenderer.
     */
    class TerrainRenderer;

    /**
     * Forward declaration class GridRenderer.
     */
    class GridRenderer;
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
     * Forward declaration class Terrain.
     */
    class Terrain;

    /**
     * Forward declaration enum class Rotation.
     */
    enum class Rotation;

    /**
     * Forward declaration enum class ChangeRotation.
     */
    enum class ChangeRotation;

    /**
     * Forward declaration enum class Zoom.
     */
    enum class Zoom;

    /**
     * Forward declaration enum class ChangeZoom.
     */
    enum class ChangeZoom;

    /**
     * Forward declaration class TileAtlas.
     */
    class TileAtlas;

    /**
     * Forward declaration class WorldGui.
     */
    class WorldGui;

    /**
     * Forward declaration class MousePicker.
     */
    class MousePicker;

    //-------------------------------------------------
    // World
    //-------------------------------------------------

    /**
     * Represents the world including all islands and the ocean/deep water area.
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
            BUILD,    // Create a building.
            DEMOLISH, // Demolish a building.
            STATUS,   // Get information about a tile.
            OPTIONS,  // Change game settings.
        };

        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * The (untranslated) labels of the action buttons.
         */
        static constexpr std::array<std::string_view, magic_enum::enum_count<Action>()> ACTION_NAMES{ "Build", "Demolish", "Status", "Options" };

        /**
         * The min width of the world.
         */
        static constexpr auto WORLD_MIN_WIDTH{ 50 };

        /**
         * The min height of the world.
         */
        static constexpr auto WORLD_MIN_HEIGHT{ 35 };

        /**
         * The max width of the world.
         */
        static constexpr auto WORLD_MAX_WIDTH{ 500 };

        /**
         * The max height of the world.
         */
        static constexpr auto WORLD_MAX_HEIGHT{ 350 };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * To have access to the shared objects (Window, Camera, Original-Assets).
         */
        std::shared_ptr<state::Context> context;

        /**
         * The world width.
         */
        int32_t width{ WORLD_MIN_WIDTH };

        /**
         * The world height.
         */
        int32_t height{ WORLD_MIN_HEIGHT };

        /**
         * The world rotation.
         */
        Rotation rotation;

        /**
         * The world zoom.
         */
        Zoom zoom;

        /**
         * The Terrain object contains all islands with their coasts.
         */
        std::unique_ptr<Terrain> terrain;

        /**
         * An OpenGL texture array for each zoom level.
         */
        std::shared_ptr<world::TileAtlas> tileAtlas;

        /**
         * A renderer to render all islands with their coasts.
         */
        std::unique_ptr<renderer::TerrainRenderer> terrainRenderer;

        /**
         * A renderer showing grids over the islands.
         */
        std::unique_ptr<renderer::GridRenderer> gridRenderer;

        /**
         * A Layer object containing only the deep water area of the world.
         */
        std::unique_ptr<layer::WorldLayer> worldLayer;

        /**
         * A Layer object to show a world grid.
         */
        std::unique_ptr<layer::WorldGridLayer> worldGridLayer;

        /**
         * The MousePicker object.
         */
        std::unique_ptr<MousePicker> mousePicker;

        /**
         * Indicates which action button is currently active.
         */
        std::array<bool, magic_enum::enum_count<Action>()> actionButtons{ false, false, true, false };

        /**
         * The current action.
         */
        Action currentAction{ Action::STATUS };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        World() = delete;

        /**
         * Constructs a new World object.
         *
         * @param t_mapFilePath The path to the Json map file.
         * @param t_context Access to shared objects.
         * @param t_stateId The parent StateId.
         */
        World(std::string t_mapFilePath, std::shared_ptr<state::Context> t_context, state::StateId t_stateId);

        World(const World& t_other) = delete;
        World(World&& t_other) noexcept = delete;
        World& operator=(const World& t_other) = delete;
        World& operator=(World&& t_other) noexcept = delete;

        ~World() noexcept;

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Checks whether a given position is in the world.
         *
         * @param t_x The x position to check.
         * @param t_y The y position to check.
         *
         * @return True or false.
         */
        [[nodiscard]] bool IsPositionInWorld(int32_t t_x, int32_t t_y) const;

        /**
         * Checks whether a given position is in the world.
         *
         * @param t_position The position to check.
         *
         * @return True or false.
         */
        [[nodiscard]] bool IsPositionInWorld(const glm::ivec2& t_position) const;

        /**
         * Projects a position into an isometric position on the screen.
         *
         * @param t_x The x position.
         * @param t_y The y position.
         * @param t_zoom The zoom to get the tile sizes.
         * @param t_rotation The position is previously rotated by the specified value.
         *
         * @return The isometric coordinates on the screen.
         */
        [[nodiscard]] glm::vec2 WorldToScreen(int32_t t_x, int32_t t_y, Zoom t_zoom, Rotation t_rotation) const;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        /**
         * Updates the world.
         */
        void Update() const;

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
        void RotateWorld(ChangeRotation t_changeRotation);

        /**
         * Changes the zoom of the world.
         *
         * @param t_changeZoom Zoom in or out.
         */
        void ZoomWorld(ChangeZoom t_changeZoom);

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The parent StateId.
         */
        state::StateId m_stateId;

        /**
         * The path to the Json map file.
         */
        std::string m_mapFilePath;

        /**
         * ImGui menus for the game.
         */
        std::unique_ptr<WorldGui> m_worldGui;

        /**
         * To change the current Layer.
         */
        layer::LayerType m_layerType;

        /**
         * Toggles world deep water rendering on and off.
         */
        bool m_renderWorldLayer{ true };

        /**
         * Toggles world grid rendering on and off.
         */
        bool m_renderWorldGridLayer{ false };

        /**
         * Toggles island grid rendering on and off.
         */
        bool m_renderIslandGridLayers{ false };

        /**
         * Toggles animations on and off.
         */
        bool m_runAnimations{ true };

        /**
         * To demolish a building.
         */
        int m_demolishTileIndex{ -1 };

        /**
         * The mouse button pressed listener handle.
         */
        decltype(event::EventManager::event_dispatcher)::Handle m_mouseButtonPressed;

        /**
         * The mouse moved listener handle.
         */
        decltype(event::EventManager::event_dispatcher)::Handle m_mouseMoved;

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

        //-------------------------------------------------
        // Clean up
        //-------------------------------------------------

        /**
         * Clean up.
         */
        void CleanUp() const;
    };
}
