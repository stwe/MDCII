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

#define OLC_PGE_APPLICATION
#define OLC_PGEX_DEAR_IMGUI_IMPLEMENTATION
#define OLC_SOUNDWAVE

#include "Log.h"
#include "Game.h"
#include "MdciiException.h"

//-------------------------------------------------
// Main
//-------------------------------------------------

int main()
{
    mdcii::Log::Init();

    MDCII_LOG_DEBUG("[main()] Starting main.");
    MDCII_LOG_DEBUG("[main()] Logger was initialized.");

    try
    {
        if (mdcii::Game game; game.Construct(
                mdcii::Game::INI.Get<int>("window", "width"),
                mdcii::Game::INI.Get<int>("window", "height"),
                1, 1
            ))
        {
            game.Start();
        }

        return EXIT_SUCCESS;
    }
    catch (const mdcii::MdciiException& e)
    {
        MDCII_LOG_ERROR("MdciiException {}", e.what());
    }
    catch (const std::exception& e)
    {
        MDCII_LOG_ERROR("Standard Exception: {}", e.what());
    }
    catch (...)
    {
        MDCII_LOG_ERROR("Unknown Exception. No details available.");
    }

    return EXIT_FAILURE;
}
