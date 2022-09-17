// This file is part of the MDCII project.
//
// Copyright (c) 2022. stwe <https://github.com/stwe/MDCII>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#pragma once

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

namespace mdcii::file
{
    class OriginalResourcesManager;
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
        inline static const std::string ORIGINAL_RESOURCES_FULL_PATH{ INI.Get<std::string>("win64", "original_resources_full_path") }; // NOLINT(cert-err58-cpp)
        inline static const std::string RESOURCES_REL_PATH{ INI.Get<std::string>("win64", "resources_rel_path") };                     // NOLINT(cert-err58-cpp)
#else
        inline static const std::string ORIGINAL_RESOURCES_FULL_PATH{ INI.Get<std::string>("linux", "original_resources_full_path") }; // NOLINT(cert-err58-cpp)
        inline static const std::string RESOURCES_REL_PATH{ INI.Get<std::string>("linux", "resources_rel_path") };                     // NOLINT(cert-err58-cpp)
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

        /**
         * A window with OpenGL context.
         */
        std::shared_ptr<ogl::Window> m_window;

        /**
         * An orthographic camera.
         */
        std::shared_ptr<camera::Camera> m_camera;

        /**
         * Resources of the original game needed here.
         */
        std::shared_ptr<file::OriginalResourcesManager> m_originalResourcesManager;

        /**
         * This game is organized with states.
         */
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
