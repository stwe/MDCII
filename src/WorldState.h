#pragma once

#include <vector>
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
        // Constants
        //-------------------------------------------------

        /**
         * The width of the world in tiles.
         */
        static constexpr auto WORLD_WIDTH = 500;

        /**
         * The height of the world in tiles.
         */
        static constexpr auto WORLD_HEIGHT = 350;

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
        std::unique_ptr<file::BshFile> m_stdBshFile;
        std::unique_ptr<file::BshFile> m_traegerBshFile;
        std::unique_ptr<camera::Camera> m_camera;
        std::unique_ptr<renderer::MeshRenderer> m_renderer;

        // todo temp code
        // -------------------
        int m_frame{ 0 };
        bool m_rotate90{ false };

        std::vector<int> m_map
        {
            1336, 0, 0, 0,
               0, 0, 0, 0,
               0, 0, 0, 0,
               0, 0, 0, 1340
        };
        // -------------------

        void RenderBuilding(int t_id, int t_mapX, int t_mapY) const;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void Init();
    };
}
