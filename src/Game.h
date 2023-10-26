// This file is part of the MDCII project.
//
// Copyright (c) 2023. stwe <https://github.com/stwe/MDCII>
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

#include "world/Zoom.h"
#include "world/Rotation.h"
#include "vendor/ini/ini.h"
#include "vendor/imgui/imgui_impl_pge.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::world
{
    /**
     * @brief Forward declaration class Island.
     */
    class Island;
}

namespace mdcii::resource
{
    /**
     * @brief Forward declaration class OriginalResourcesManager.
     */
    class OriginalResourcesManager;

    /**
     * @brief Forward declaration class TileAtlas.
     */
    class TileAtlas;
}

namespace mdcii::renderer
{
    /**
     * @brief Forward declaration class Renderer.
     */
    class Renderer;
}

namespace mdcii::state
{
    /**
     * @brief Forward declaration class StateStack.
     */
    class StateStack;
}

namespace mdcii
{
    //-------------------------------------------------
    // Game
    //-------------------------------------------------

    /**
     * @brief The main game object.
     */
    class Game: public olc::PixelGameEngine
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Game();

        Game(const Game& t_other) = delete;
        Game(Game&& t_other) noexcept = delete;
        Game& operator=(const Game& t_other) = delete;
        Game& operator=(Game&& t_other) noexcept = delete;

        ~Game() override;

        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        inline static const inih::INIReader INI{ "./config.ini" };

#if defined(_WIN64)
        inline static const std::string ORIGINAL_RESOURCES_FULL_PATH{ INI.Get<std::string>("win64", "original_resources_full_path") };
        inline static const std::string RESOURCES_REL_PATH{ INI.Get<std::string>("win64", "resources_rel_path") };
#else
        inline static const std::string ORIGINAL_RESOURCES_FULL_PATH{ INI.Get<std::string>("linux", "original_resources_full_path") };
        inline static const std::string RESOURCES_REL_PATH{ INI.Get<std::string>("linux", "resources_rel_path") };
#endif

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        world::Zoom zoom{ world::Zoom::GFX };
        world::Rotation rotation{ world::Rotation::DEG0 };
        std::unique_ptr<resource::OriginalResourcesManager> origResMng;
        std::unique_ptr<resource::TileAtlas> tileAtlas;
        std::vector<std::unique_ptr<world::Island>> islands;
        uint32_t gameLayer;
        olc::imgui::PGE_ImGUI pgeImgui{ false };

        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        bool OnUserCreate() override;
        bool OnUserUpdate(float t_elapsedTime) override;
        void DrawImGui();

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        std::unique_ptr<renderer::Renderer> m_renderer;
        std::unique_ptr<state::StateStack> m_stateStack;
    };
}
