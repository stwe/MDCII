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

#include <string>
#include <memory>
#include <glm/vec2.hpp>

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
        // Member
        //-------------------------------------------------

        /**
         * To have access to the shared objects (Window, Camera, Original-Assets).
         */
        std::shared_ptr<state::Context> context;

        /**
         * The world width.
         */
        int32_t worldWidth;

        /**
         * The world height.
         */
        int32_t worldHeight;

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
        std::shared_ptr<Terrain> terrain;

        /**
         * An OpenGL texture array for each zoom level.
         */
        std::shared_ptr<world::TileAtlas> tileAtlas;

        /**
         * A renderer to render all islands with their coasts.
         */
        std::unique_ptr<renderer::TerrainRenderer> terrainRenderer;

        /**
         * A Layer object containing only the deep water area of the world.
         */
        std::unique_ptr<layer::WorldLayer> worldLayer;

        /**
         * A Layer object to show a world grid.
         */
        std::unique_ptr<layer::WorldGridLayer> worldGridLayer;

        /**
         * A renderer to render grids.
         */
        std::unique_ptr<renderer::GridRenderer> gridRenderer;

        /**
         * The MousePicker object.
         */
        std::unique_ptr<MousePicker> mousePicker;

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

        [[nodiscard]] bool IsPositionInWorld(int32_t t_x, int32_t t_y) const;

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

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initialize class.
         */
        void Init();

        //-------------------------------------------------
        // Clean up
        //-------------------------------------------------

        /**
         * Clean up.
         */
        void CleanUp() const;
    };
}
