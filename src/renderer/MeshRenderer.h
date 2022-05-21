#pragma once

#include <glm/mat4x4.hpp>
#include "ogl/Window.h"
#include "camera/Camera.h"

namespace mdcii::renderer
{
    class MeshRenderer
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        MeshRenderer();

        MeshRenderer(const MeshRenderer& t_other) = delete;
        MeshRenderer(MeshRenderer&& t_other) noexcept = delete;
        MeshRenderer& operator=(const MeshRenderer& t_other) = delete;
        MeshRenderer& operator=(MeshRenderer&& t_other) noexcept = delete;

        ~MeshRenderer() noexcept = default;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void Render(
            const glm::mat4& t_modelMatrix,
            uint32_t t_bshTextureId,
            const ogl::Window& t_window,
            const camera::Camera& t_camera
        ) const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        uint32_t m_vao{ 0 };

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void Init();
    };
}
