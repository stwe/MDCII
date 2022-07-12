#pragma once

#include <memory>
#include "ini/ini.h"
#include "ecs/entt.hpp"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::ogl
{
    class Window;
}

namespace mdcii::camera
{
    class Camera;
}

namespace mdcii::data
{
    class Buildings;
}

namespace mdcii::state
{
    class StateStack;
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
        // Member
        //-------------------------------------------------

        inline static entt::registry ecs;

        inline static const inih::INIReader INI{ "./config.ini" }; // NOLINT(cert-err58-cpp)

#if defined(_WIN64)
        inline static const std::string RESOURCES_PATH{ INI.Get<std::string>("win64", "resources_path") }; // NOLINT(cert-err58-cpp)
#else
        inline static const std::string RESOURCES_PATH{ INI.Get<std::string>("linux", "resources_path") }; // NOLINT(cert-err58-cpp)
#endif

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
        std::shared_ptr<camera::Camera> m_camera;
        std::shared_ptr<data::Buildings> m_buildings;
        std::unique_ptr<state::StateStack> m_stateStack;

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

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        void CreateSharedObjects();
        void Start();
    };
}
