#pragma once

#include <memory>
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

namespace mdcii::renderer
{
    class TileRenderer;
}

//-------------------------------------------------
// MousePicker
//-------------------------------------------------

namespace mdcii::map
{
    /**
     * Forward declaration class Map.
     */
    class Map;

    /**
     * Represents a mouse picker to select tiles.
     */
    class MousePicker
    {
    public:
        //-------------------------------------------------
        // Types
        //-------------------------------------------------

        /**
         * The tile under the mouse.
         */
        struct Selected
        {
            /**
             * The current tile map position under the mouse.
             */
            glm::ivec2 currentPosition{ -1 };

            /**
             * The last tilemap position under the mouse after mouse button is pressed.
             */
            glm::ivec2 lastPosition{ -1 };

            /**
             * Flag for lastSelected.
             */
            bool lastChanged{ false };
        };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The selected tile.
         */
        Selected selected;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        /**
         * Constructs a new MousePicker object.
         *
         * @param t_map The parent Map object.
         * @param t_buildings Access to all building objects.
         */
        MousePicker(std::shared_ptr<Map> t_map, std::shared_ptr<data::Buildings> t_buildings);

        MousePicker(const MousePicker& t_other) = delete;
        MousePicker(MousePicker&& t_other) noexcept = delete;
        MousePicker& operator=(const MousePicker& t_other) = delete;
        MousePicker& operator=(MousePicker&& t_other) noexcept = delete;

        ~MousePicker() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void Render(
            const ogl::Window& t_window,
            const camera::Camera& t_camera,
            const event::SelectedBauGfx& t_selectedBauGfx
        );

        void RenderImGui() const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The parent Map object.
         */
        std::shared_ptr<Map> m_map;

        /**
         * Access to all building objects.
         */
        std::shared_ptr<data::Buildings> m_buildings;

        /**
         * The current mouse position.
         */
        glm::ivec2 m_mouse{ 0 };

        /**
         * The active cell on current mouse position.
         */
        glm::ivec2 m_cell{ 0 };

        /**
         * The mouse offset into cell.
         */
        glm::ivec2 m_offsetIntoCell{ 0 };

        /**
         * A renderer for highlighting tiles.
         */
        std::unique_ptr<renderer::TileRenderer> m_renderer;

        /**
         * The pixels of the corner (cheat) image.
         */
        unsigned char* m_cornerImage{ nullptr };

        /**
         * Indicates whether the mouse is in the window.
         */
        bool m_inWindow{ false };

        //-------------------------------------------------
        // Cursor
        //-------------------------------------------------

        void RenderCursor(
            const ogl::Window& t_window,
            const camera::Camera& t_camera,
            const event::SelectedBauGfx& t_selectedBauGfx
        ) const;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initializing class.
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
