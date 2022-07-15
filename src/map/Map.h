#pragma once

#include <memory>
#include "ecs/Components.h"
#include "ogl/Window.h"
#include "camera/Camera.h"

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
        bool renderGrid{ false };

        /**
         * Shows the coordinates.
         */
        bool renderText{ false };

        /**
         * Shows the terrain layer.
         */
        bool renderTerrainLayer{ true };

        /**
         * Shows the buildings layer.
         */
        bool renderBuildingsLayer{ true };

        /**
         * Indicates whether a bauGfx is selected to build.
         */
        bool bauGfxSelected{ false };

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
         * @return the rotation as string
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
    };
}
