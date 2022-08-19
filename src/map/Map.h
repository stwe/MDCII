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

#include "MapTile.h"
#include "data/Buildings.h"
#include "event/Event.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::state
{
    struct Context;
}

namespace mdcii::renderer
{
    class TileRenderer;
    class TextRenderer;
}

namespace mdcii::map
{
    //-------------------------------------------------
    // Forward declarations
    //-------------------------------------------------

    /**
     * Forward declaration class MousePicker.
     */
    class MousePicker;

    /**
     * Forward declaration class MapContent.
     */
    class MapContent;

    /**
     * Forward declaration enum class ChangeZoom.
     */
    enum class ChangeZoom;

    //-------------------------------------------------
    // Map
    //-------------------------------------------------

    /**
     * Represents a map.
     */
    class Map
    {
    public:
        //-------------------------------------------------
        // Types
        //-------------------------------------------------

        /**
         * The possible map actions.
         */
        enum class Action
        {
            BUILD,
            STATUS,
            OPTIONS
        };

        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * The (untranslated) labels of the map action buttons.
         */
        static constexpr std::array<std::string_view, magic_enum::enum_count<Action>()> ACTION_NAMES{ "Build", "Status", "Options" };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * Shared objects (Window, Camera, original assets) between all states.
         */
        std::shared_ptr<state::Context> context;

        /**
         * A MousePicker object to select tiles.
         */
        std::unique_ptr<MousePicker> mousePicker;

        /**
         * The map content (layers with tiles) read from a json file.
         */
        std::unique_ptr<MapContent> mapContent;

        /**
         * A tile renderer to render bsh graphics as Gpu texture.
         */
        std::unique_ptr<renderer::TileRenderer> renderer;

        /**
         * Lets OpenGL render a text string.
         */
        std::unique_ptr<renderer::TextRenderer> textRenderer;

        /**
         * Shows the isometric grid.
         */
        bool renderGrid{ false };

        /**
         * Shows the isometric grid coordinates.
         */
        bool renderText{ false };

        /**
         * Shows the terrain layer.
         */
        bool renderTerrainLayer{ true };

        /**
         * Skip buildings layer objects when rendering the terrain layer.
         */
        bool skipBuildingsLayerTiles{ false };

        /**
         * Shows the buildings layer.
         */
        bool renderBuildingsLayer{ true };

        /**
         * Indicates which action button is currently active.
         */
        std::array<bool, magic_enum::enum_count<Action>()> actionButtons{ false, true, false };

        /**
         * The current action.
         */
        Action currentAction{ Action::STATUS };

        /**
         * The current selected map tile.
         * todo: pointer
         */
        MapTile currentSelectedMapTile;

        /**
         * The building selected to build.
         */
        event::SelectedBauGfx currentSelectedBauGfx;

        /**
         * Enable/disable demolition mode.
         */
        bool demolitionMode{ false };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Map() = delete;

        /**
         * Constructs a new Map object.
         *
         * @param t_mapFilePath The path to the json map file.
         * @param t_context Shared objects between all states.
         */
        Map(const std::string& t_mapFilePath, std::shared_ptr<state::Context> t_context);

        Map(const Map& t_other) = delete;
        Map(Map&& t_other) noexcept = delete;
        Map& operator=(const Map& t_other) = delete;
        Map& operator=(Map&& t_other) noexcept = delete;

        ~Map() noexcept;

        //-------------------------------------------------
        // Render
        //-------------------------------------------------

        /**
         * Renders the map.
         */
        void Render() const;

        /**
         * Renders ImGui menus.
         */
        void RenderImGui();

        /**
         * Renders a gfx from the stadtfld.bsh file.
         *
         * @param t_gfx The gfx to render.
         * @param t_screenPosition The isometric coordinates on the screen.
         * @param t_elevation Either 0 or 20 - equals the posoffs value of the building.
         * @param t_selected Determines if the building is selected and rendered a little darker.
         */
        void RenderBuilding(
            int t_gfx,
            glm::vec2 t_screenPosition,
            float t_elevation,
            bool t_selected
        ) const;

        //-------------------------------------------------
        // Rotate && Zoom
        //-------------------------------------------------

        /**
         * Changes the rotation of the map left or right.
         *
         * @param t_changeRotation left or right
         */
        void Rotate(ChangeRotation t_changeRotation) const;

        /**
         * Changes the zoom of the map.
         *
         * @param t_changeZoom Zoom in or out.
         */
        void Zoom(ChangeZoom t_changeZoom) const;

    protected:

    private:
        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initialize objects.
         *
         * @param t_filePath The path to the json map file.
         */
        void Init(const std::string& t_filePath);

        //-------------------------------------------------
        // Render Entities
        //-------------------------------------------------

        /**
         * Renders the isometric grid.
         */
        void RenderGridEntities() const;

        /**
         * Renders the entities from all layers.
         */
        void RenderEntities() const;

        /**
         * Renders an entity.
         *
         * @param t_mapTile A MapTile object.
         * @param t_building A Building object.
         * @param t_selected Determines if the building is selected and rendered a little darker.
         */
        void RenderEntity(
            const MapTile& t_mapTile,
            const data::Building& t_building,
            bool t_selected = false
        ) const;

        /**
         * Iterates over all entities and renders tiles of the TerrainLayer.
         */
        void RenderTerrainLayerEntities() const;

        /**
         * Iterates over all entities and renders tiles of the BuildingsLayer.
         */
        void RenderBuildingsLayerEntities() const;

        /**
         * Iterates over all entities and renders tiles of the TerrainLayer or the BuildingsLayer.
         */
        void RenderTerrainOrBuildingsEntities() const;
    };
}
