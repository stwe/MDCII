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

#include "ecs/Components.h"
#include "ogl/Window.h"
#include "camera/Camera.h"
#include "event/Event.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::data
{
    class Buildings;
}

namespace mdcii::file
{
    class PaletteFile;
    class BshFile;
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

    //-------------------------------------------------
    // Rotation
    //-------------------------------------------------

    /**
     * Rotate left or right.
     */
    enum class ChangeRotation
    {
        LEFT, RIGHT
    };

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
        // Constants
        //-------------------------------------------------

        /**
         * The height of the island.
         */
        static constexpr auto ELEVATION{ 20 };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * A MousePicker object to select tiles.
         */
        std::unique_ptr<MousePicker> mousePicker;

        /**
         * The map content (layers with tiles) read from a json file.
         */
        std::unique_ptr<MapContent> mapContent;

        /**
         * The bsh graphics from the stadtfld.bsh file.
         */
        std::unique_ptr<file::BshFile> stdBshFile;

        /**
         * The bsh graphics from the bauhaus.bsh file.
         */
        std::unique_ptr<file::BshFile> bauhausBshFile;

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
        bool renderGrid{ true };

        /**
         * Shows the isometric grid coordinates.
         */
        bool renderText{ true };

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
         * Current selected building.
         */
        event::SelectedBauGfx selectedBauGfx;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Map() = delete;

        /**
         * Constructs a new Map object.
         *
         * @param t_filePath The path to the json map file.
         * @param t_buildings Access to all building objects.
         * @param t_window The Window object.
         * @param t_camera The Camera object.
         */
        Map(
            const std::string& t_filePath,
            std::shared_ptr<data::Buildings> t_buildings,
            const ogl::Window& t_window,
            const camera::Camera& t_camera
        );

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
         *
         * @param t_window The Window object to get the orthographic projection matrix.
         * @param t_camera The Camera object to get the view matrix.
         */
        void Render(const ogl::Window& t_window, const camera::Camera& t_camera) const;

        /**
         * Renders ImGui menus.
         */
        void RenderImGui();

        /**
         * Renders a gfx from the stadtfld.bsh file.
         *
         * @param t_window The Window object to get the orthographic projection matrix.
         * @param t_camera The Camera object to get the view matrix.
         * @param t_gfx The gfx to render.
         * @param t_screenPosition The isometric coordinates on the screen.
         * @param t_elevation Either 0 or 20 - equals the posoffs value of the building.
         * @param t_selected Determines if the building is selected and rendered a little darker.
         */
        void RenderBuilding(
            const ogl::Window& t_window,
            const camera::Camera& t_camera,
            int t_gfx,
            glm::vec2 t_screenPosition,
            float t_elevation,
            bool t_selected
        ) const;

        //-------------------------------------------------
        // Rotate
        //-------------------------------------------------

        /**
         * Changes the rotation of the map left or right.
         *
         * @param t_changeRotation left or right
         */
        void Rotate(ChangeRotation t_changeRotation) const;

        /**
         * Get the current map rotation as string.
         *
         * @return The rotation as string.
         */
        [[nodiscard]] const char* ShowCurrentRotation() const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The content from the haeuser.cod.
         */
        std::shared_ptr<data::Buildings> m_buildings;

        /**
         * Pallet values from the from the stadtfld.col file.
         */
        std::unique_ptr<file::PaletteFile> m_paletteFile;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initialize objects.
         *
         * @param t_filePath The path to the json map file.
         * @param t_window The Window object.
         * @param t_camera The Camera object.
         */
        void Init(const std::string& t_filePath, const ogl::Window& t_window, const camera::Camera& t_camera);

        //-------------------------------------------------
        // Render Entities
        //-------------------------------------------------

        /**
         * Renders the isometric grid.
         *
         * @param t_window The Window object to get the orthographic projection matrix.
         * @param t_camera The Camera object to get the view matrix.
         */
        void RenderGridEntities(const ogl::Window& t_window, const camera::Camera& t_camera) const;

        /**
         * Renders the entities from all layers.
         *
         * @param t_window The Window object to get the orthographic projection matrix.
         * @param t_camera The Camera object to get the view matrix.
         */
        void RenderEntities(const ogl::Window& t_window, const camera::Camera& t_camera) const;

        /**
         * Renders an entity.
         *
         * @param t_window The Window object to get the orthographic projection matrix.
         * @param t_camera The Camera object to get the view matrix.
         * @param t_mapTile A MapTile object.
         * @param t_building A Building object.
         * @param t_selected Determines if the building is selected and rendered a little darker.
         */
        void RenderEntity(
            const ogl::Window& t_window,
            const camera::Camera& t_camera,
            const MapTile& t_mapTile,
            const data::Building& t_building,
            bool t_selected = false
        ) const;



        // todo

        void RenderE(
            const ogl::Window& t_window,
            const camera::Camera& t_camera,
            const MapTile& t_mapTile,
            const data::Building& t_building
        ) const;

        void RenderTerrainLayerEntities(const ogl::Window& t_window, const camera::Camera& t_camera) const;
        void RenderBuildingsLayerEntities(const ogl::Window& t_window, const camera::Camera& t_camera) const;
        void RenderTerrainOrBuildingsEntities(const ogl::Window& t_window, const camera::Camera& t_camera) const;
    };
}
