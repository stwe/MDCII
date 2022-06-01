#pragma once

#include "ogl/Window.h"
#include "camera/Camera.h"

namespace mdcii::renderer
{
    class TileRenderer;
}

namespace mdcii::map
{
    class MousePicker
    {
    public:
        MousePicker();

        ~MousePicker() noexcept;

        void Render(const ogl::Window& t_window, const camera::Camera& t_camera);
        void RenderImGui();

    protected:

    private:
        glm::ivec2 m_mouse;
        glm::ivec2 m_cell;
        glm::ivec2 m_offsetIntoCell;
        glm::ivec2 m_selected;

        std::unique_ptr<renderer::TileRenderer> m_renderer;

        void Init();
    };
}

