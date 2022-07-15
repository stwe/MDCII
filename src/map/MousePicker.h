#pragma once

#include <memory>
#include "ogl/Window.h"
#include "camera/Camera.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

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
        // Member
        //-------------------------------------------------

        /**
         * The current tile map position under the mouse.
         */
        glm::ivec2 currentPosition{ -1 };

        /**
         * The last tile map position under the mouse.
         */
        glm::ivec2 lastPosition{ -1 };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        /**
         * Constructs a new MousePicker object.
         *
         * @param t_map The parent Map object.
         * @param t_window The Window object.
         * @param t_camera The Camera object.
         */
        MousePicker(Map* t_map, const ogl::Window& t_window, const camera::Camera& t_camera);

        MousePicker(const MousePicker& t_other) = delete;
        MousePicker(MousePicker&& t_other) noexcept = delete;
        MousePicker& operator=(const MousePicker& t_other) = delete;
        MousePicker& operator=(MousePicker&& t_other) noexcept = delete;

        ~MousePicker() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void Render(const ogl::Window& t_window, const camera::Camera& t_camera);
        void RenderImGui() const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The parent Map object.
         */
        Map* m_map{ nullptr };

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
        // Helper
        //-------------------------------------------------

        [[nodiscard]] glm::ivec2 GetMapPosition(const ogl::Window& t_window, const camera::Camera& t_camera) const;

        //-------------------------------------------------
        // Cursor
        //-------------------------------------------------

        void RenderMouseCursor(const ogl::Window& t_window, const camera::Camera& t_camera) const;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initialize class.
         */
        void Init();

        /**
         * Adds event listeners.
         *
         * @param t_window The Window object.
         * @param t_camera The Camera object.
         */
        void AddListeners(const ogl::Window& t_window, const camera::Camera& t_camera);

        //-------------------------------------------------
        // Clean up
        //-------------------------------------------------

        /**
         * Clean up.
         */
        void CleanUp() const;
    };
}
