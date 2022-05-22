#pragma once

#include "state/State.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::file
{
    class PaletteFile;
    class BshFile;
}

namespace mdcii::camera
{
    class Camera;
}

namespace mdcii::renderer
{
    class MeshRenderer;
}

//-------------------------------------------------
// WorldState
//-------------------------------------------------

namespace mdcii
{
    class WorldState : public state::State
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        WorldState() = delete;

        /**
         * Constructs a new WorldState object.
         *
         * @param t_id The unique identifier.
         * @param t_stateStack The parent StateStack object.
         * @param t_context The holder of shared objects.
         */
        WorldState(Id t_id, state::StateStack* t_stateStack, std::shared_ptr<Context> t_context);

        WorldState(const WorldState& t_other) = delete;
        WorldState(WorldState&& t_other) noexcept = delete;
        WorldState& operator=(const WorldState& t_other) = delete;
        WorldState& operator=(WorldState&& t_other) noexcept = delete;

        ~WorldState() noexcept override;

        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        void Input() override;
        void Update() override;
        void PreRender() override {}
        void Render() override;
        void RenderImGui() override;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        std::unique_ptr<file::PaletteFile> m_paletteFile;
        std::unique_ptr<file::BshFile> m_bshFile;
        std::unique_ptr<camera::Camera> m_camera;
        std::unique_ptr<renderer::MeshRenderer> m_renderer;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void Init();
    };
}
