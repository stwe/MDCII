#pragma once

#include <memory>

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::ogl
{
    class Window;
}

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
// Game
//-------------------------------------------------

namespace mdcii
{
    class Game
    {
    public:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * 60 updates per second.
         */
        static constexpr auto FRAME_TIME{ 1.0 / 60.0 };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Game();

        Game(const Game& t_other) = delete;
        Game(Game&& t_other) noexcept = delete;
        Game& operator=(const Game& t_other) = delete;
        Game& operator=(Game&& t_other) noexcept = delete;

        ~Game() noexcept;

        //-------------------------------------------------
        // Run
        //-------------------------------------------------

        void Run();

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        std::shared_ptr<ogl::Window> m_window;
        std::unique_ptr<file::PaletteFile> m_paletteFile;
        std::unique_ptr<file::BshFile> m_bshFile;
        std::unique_ptr<camera::Camera> m_camera;
        std::unique_ptr<renderer::MeshRenderer> m_renderer;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void Init();
        void Input();
        void Update();
        void Render();

        //-------------------------------------------------
        // Game loop
        //-------------------------------------------------

        void GameLoop();
    };
}
