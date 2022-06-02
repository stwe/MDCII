#pragma once

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

        explicit MousePicker(std::shared_ptr<Map> t_map);

        MousePicker(const MousePicker& t_other) = delete;
        MousePicker(MousePicker&& t_other) noexcept = delete;
        MousePicker& operator=(const MousePicker& t_other) = delete;
        MousePicker& operator=(MousePicker&& t_other) noexcept = delete;

        ~MousePicker() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void Render(const ogl::Window& t_window, const camera::Camera& t_camera);
        void RenderImGui();

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        std::shared_ptr<Map> m_map;

        glm::ivec2 m_mouse{ 0 };
        glm::ivec2 m_cell{ 0 };
        glm::ivec2 m_offsetIntoCell{ 0 };
        glm::ivec2 m_selected{ 0 };

        std::unique_ptr<renderer::TileRenderer> m_renderer;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void Init();
    };
}
