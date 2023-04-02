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

#include <vector>
#include "World.h"
#include "event/EventManager.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::layer
{
    /**
     * Forward declaration class WorldLayer.
     */
    class WorldLayer;

    /**
     * Forward declaration class WorldGridLayer.
     */
    class WorldGridLayer;
}

namespace mdcii::renderer
{
    /**
     * Forward declaration class TerrainRenderer.
     */
    class TerrainRenderer;
}

//-------------------------------------------------
// GeneratorWorld
//-------------------------------------------------

namespace mdcii::world
{
    //-------------------------------------------------
    // Forward declarations
    //-------------------------------------------------

    /**
     * Forward declaration class TileAtlas.
     */
    class TileAtlas;

    /**
     * Forward declaration class WorldGui.
     */
    class WorldGui;

    //-------------------------------------------------
    // GeneratorWorld
    //-------------------------------------------------

    /**
     * Represents a world generator.
     */
    class GeneratorWorld : public World
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * An OpenGL texture array for each zoom level.
         */
        std::shared_ptr<world::TileAtlas> tileAtlas;

        /**
         * A renderer to render all islands with their coasts and the deep water.
         */
        std::unique_ptr<renderer::TerrainRenderer> terrainRenderer;

        /**
         * A Layer object containing the deep water area of the world.
         */
        std::unique_ptr<layer::WorldLayer> worldLayer;

        /**
         * A Layer object to show a world grid.
         */
        std::unique_ptr<layer::WorldGridLayer> worldGridLayer;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        GeneratorWorld() = delete;

        /**
         * Constructs a new GameWorld object.
         *
         * @param t_context Access to shared objects.
         * @param t_stateId The parent StateId.
         * @param t_width The world width.
         * @param t_height The world height.
         */
        GeneratorWorld(std::shared_ptr<state::Context> t_context, state::StateId t_stateId, int32_t t_width, int32_t t_height);

        GeneratorWorld(const GeneratorWorld& t_other) = delete;
        GeneratorWorld(GeneratorWorld&& t_other) noexcept = delete;
        GeneratorWorld& operator=(const GeneratorWorld& t_other) = delete;
        GeneratorWorld& operator=(GeneratorWorld&& t_other) noexcept = delete;

        ~GeneratorWorld() noexcept override;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        /**
         * Updates the world.
         */
        void Update() override;

        /**
         * Renders the world.
         */
        void Render() override;

        /**
         * Renders ImGui menus.
         */
        void RenderImGui() override;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * ImGui menus for the generator.
         */
        std::unique_ptr<WorldGui> m_worldGui;

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
         * Adds event listeners.
         */
        void AddListeners();

        //-------------------------------------------------
        // ImGui
        //-------------------------------------------------

        /**
         * Provides a simple mechanism to choose an island file.
         *
         * @param t_files A list of island files.
         */
        void RenderIslandFileChooser(std::vector<std::string>& t_files);

        //-------------------------------------------------
        // Clean up
        //-------------------------------------------------

        /**
         * Clean up.
         */
        void CleanUp() const;
    };
}