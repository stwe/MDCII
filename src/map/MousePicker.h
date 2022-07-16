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

        /**
         * Render method for the mouse cursor.
         *
         * @param t_window The Window object.
         * @param t_camera The Camera object.
         */
        void Render(const ogl::Window& t_window, const camera::Camera& t_camera);

        /**
         * Renders ImGui stuff.
         */
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
         * The current tile map position under the mouse.
         */
        glm::ivec2 m_currentPosition{ -1 };

        /**
         * The last tile map position under the mouse.
         */
        glm::ivec2 m_lastPosition{ -1 };

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
        // Logic
        //-------------------------------------------------

        /**
         * Updates the tile map positions under the mouse.
         *
         * @param t_window The Window object.
         * @param t_camera The Camera object.
         */
        void UpdatePositions(const ogl::Window& t_window, const camera::Camera& t_camera);

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Get the tile map position under the mouse.
         *
         * @param t_window The Window object.
         * @param t_camera The Camera object.
         *
         * @return The map position of the mouse.
         */
        [[nodiscard]] glm::ivec2 GetMapPosition(const ogl::Window& t_window, const camera::Camera& t_camera) const;

        /**
         * Checks whether the current mouse position is in map.
         *
         * @return True or false depending on the position in the map.
         */
        [[nodiscard]] bool IsCurrentMouseInMap() const;

        /**
         * Checks whether the last mouse position is in map.
         *
         * @return True or false depending on the position in the map.
         */
        [[nodiscard]] bool IsLastMouseInMap() const;

        //-------------------------------------------------
        // Cursor
        //-------------------------------------------------

        /**
         * Renders a texture as a mouse cursor.
         *
         * @param t_window The Window object.
         * @param t_camera The Camera object.
         */
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
