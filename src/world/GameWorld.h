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
#include "World.h"
#include "event/EventManager.h"
#include "data/json.hpp"

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
// GameWorld
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
     * Forward declaration class TileAtlas.
     */
    class TileAtlas;

    /**
     * Forward declaration class WorldGui.
     */
    class WorldGui;

    //-------------------------------------------------
    // World
    //-------------------------------------------------

    /**
     * Represents a world including all islands and the ocean/deep water area.
     */
    class GameWorld : public World
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

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The Terrain object contains all islands with their coasts.
         */
        std::unique_ptr<Terrain> terrain;

        /**
         * An OpenGL texture array for each zoom level.
         */
        std::shared_ptr<world::TileAtlas> tileAtlas;

        /**
         * A renderer to render all islands with their coasts and the deep water.
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
         * Indicates which action button is currently active.
         */
        std::array<bool, magic_enum::enum_count<Action>()> actionButtons{ true, false, false, false };

        /**
         * The current action.
         */
        Action currentAction{ Action::BUILD };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        GameWorld() = delete;

        /**
         * Constructs a new GameWorld object.
         *
         * @param t_context Access to shared objects.
         * @param t_stateId The parent StateId.
         * @param t_mapFilePath The path to the Json map file.
         */
        GameWorld(std::shared_ptr<state::Context> t_context, state::StateId t_stateId, std::string t_mapFilePath);

        GameWorld(const GameWorld& t_other) = delete;
        GameWorld(GameWorld&& t_other) noexcept = delete;
        GameWorld& operator=(const GameWorld& t_other) = delete;
        GameWorld& operator=(GameWorld&& t_other) noexcept = delete;

        ~GameWorld() noexcept override;

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
         * The path to the Json map file.
         */
        std::string m_mapFilePath;

        /**
         * ImGui menus for the game.
         */
        std::unique_ptr<WorldGui> m_worldGui;

        /**
         * The Layer types to render.
         */
        layer::LayerType m_layerTypeToRender;

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

    //-------------------------------------------------
    // Json
    //-------------------------------------------------

    void to_json(nlohmann::json& t_json, const World& t_world);
}
