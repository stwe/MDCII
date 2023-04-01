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

#include <glm/vec2.hpp>
#include "data/json.hpp"
#include "event/EventManager.h"
#include "data/Buildings.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::layer
{
    /**
     * Forward declaration struct Tile.
     */
    struct Tile;
}

namespace mdcii::state
{
    /**
     * Forward declaration struct Context.
     */
    struct Context;
}

//-------------------------------------------------
// Terrain
//-------------------------------------------------

namespace mdcii::world
{
    //-------------------------------------------------
    // Forward declarations
    //-------------------------------------------------

    /**
     * Forward declaration class GameWorld.
     */
    class GameWorld;

    /**
     * Forward declaration class Island.
     */
    class Island;

    /**
     * Forward declaration enum class Island.
     */
    enum class Rotation;

    //-------------------------------------------------
    // Terrain
    //-------------------------------------------------

    /**
     * Represents the terrain of the world.
     */
    class Terrain
    {
    public:
        //-------------------------------------------------
        // Types
        //-------------------------------------------------

        /**
         * Used to add a building.
         */
        struct TilesToAdd
        {
            std::vector<std::unique_ptr<layer::Tile>> tiles;
            Island* island{ nullptr };
        };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The parent GameWorld object.
         */
        GameWorld* gameWorld{ nullptr };

        /**
         * The Island objects of the Terrain.
         */
        std::vector<std::unique_ptr<Island>> islands;

        /**
         * Pointer to the currently selected Island object.
         */
        Island* currentSelectedIsland{ nullptr };

        /**
         * Pointer to the currently Island object under the mouse.
         */
        Island* currentIslandUnderMouse{ nullptr };

        /**
         * Tiles to add a building.
         */
        TilesToAdd tilesToAdd;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Terrain() = delete;

        /**
         * Constructs a new Terrain object.
         *
         * @param t_context Access to shared objects.
         * @param t_gameWorld The parent GameWorld object.
         */
        Terrain(std::shared_ptr<state::Context> t_context, GameWorld* t_gameWorld);

        Terrain(const Terrain& t_other) = delete;
        Terrain(Terrain&& t_other) noexcept = delete;
        Terrain& operator=(const Terrain& t_other) = delete;
        Terrain& operator=(Terrain&& t_other) noexcept = delete;

        ~Terrain() noexcept;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Creates the Island objects from a Json value.
         *
         * @param t_json The Json value.
         */
        void CreateIslandsFromJson(const nlohmann::json& t_json);

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * Checks if a given world position is in deep water.
         *
         * @param t_x The world x position to be checked.
         * @param t_y The world y position to be checked.
         *
         * @return True or false.
         */
        [[nodiscard]] bool IsWorldPositionInDeepWater(int32_t t_x, int32_t t_y) const;

        /**
         * Checks whether the given building can be created at the specified world position on current island under mouse.
         *
         * @param t_startWorldPosition The world position to be checked.
         * @param t_building The building to add.
         * @param t_buildingRotation The rotation of the building.
         *
         * @return True or false.
         */
        [[nodiscard]] bool IsBuildableOnIslandUnderMouse(const glm::ivec2& t_startWorldPosition, const data::Building& t_building, Rotation t_buildingRotation) const;

        /**
         * Checks whether the building on the currently selected tile can be removed.
         *
         * @return True or false.
         */
        [[nodiscard]] bool IsCurrentSelectedTileRemovable() const;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        /**
         * Renders ImGui menus.
         */
        void RenderImGui() const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * To have access to the shared objects (Window, Camera, Original-Assets, Mdcii-Assets).
         */
        std::shared_ptr<state::Context> m_context;

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
        // Clean up
        //-------------------------------------------------

        /**
         * Clean up.
         */
        void CleanUp() const;
    };
}
