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

#include "vendor/olc/olcPixelGameEngine.h"
#include "vendor/ini/ini.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::world
{
    class Island;
}

namespace mdcii::resource
{
    class OriginalResourcesManager;
    class TileAtlas;
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
        // Override
        //-------------------------------------------------

        bool OnUserCreate() override;
        bool OnUserUpdate(float t_elapsedTime) override;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        std::vector<std::unique_ptr<world::Island>> m_islands;
        std::unique_ptr<resource::OriginalResourcesManager> m_origResMng;
        std::unique_ptr<resource::TileAtlas> m_tileAtlas;
    };
}
