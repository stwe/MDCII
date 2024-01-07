// This file is part of the MDCII project.
//
// Copyright (c) 2024. stwe <https://github.com/stwe/MDCII>
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

#include "vendor/ini/ini.h"
#include "vendor/imgui/imgui_impl_pge.h"
#include "vendor/olc/olcSoundWaveEngine.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::resource
{
    /**
     * @brief Forward declaration class OriginalResourcesManager.
     */
    class OriginalResourcesManager;

    /**
     * @brief Forward declaration class MdciiResourcesManager.
     */
    class MdciiResourcesManager;

    /**
     * @brief Forward declaration class TileAtlas.
     */
    class TileAtlas;

    /**
     * @brief Forward declaration class AnimalsTileAtlas.
     */
    class AnimalsTileAtlas;

    /**
     * @brief Forward declaration class AssetManager.
     */
    class AssetManager;
}

namespace mdcii::state
{
    /**
     * @brief Forward declaration class StateSystem.
     */
    class StateSystem;
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

        /**
         * @brief The name of this game.
         */
        static constexpr std::string_view GAME_NAME{ "MDCII" };

        /**
         * @brief The current version of this game.
         */
        static constexpr std::string_view VERSION{ "0.2-DEV" };

        /**
         * @brief The name of the Ini-File to read.
         */
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

        std::unique_ptr<resource::OriginalResourcesManager> originalResourcesManager;
        std::unique_ptr<resource::MdciiResourcesManager> mdciiResourcesManager;
        std::unique_ptr<resource::TileAtlas> stadtfldTileAtlas;
        std::unique_ptr<resource::AnimalsTileAtlas> animalsTileAtlas;
        std::unique_ptr<resource::AssetManager> assetManager;
        std::unique_ptr<state::StateSystem> stateSystem;

        /**
         * @brief The ImGui plugin.
         */
        olc::imgui::PGE_ImGUI pgeImGui{ Game::RESOURCES_REL_PATH + "font/DejaVuSansMono.ttf", false };

        /**
         * @brief The layer for the game.
         */
        int gameLayer{ -1 };

        // todo
        /*
        olc::sound::WaveEngine engine;
        olc::sound::Wave wave;
        */

        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        bool OnUserCreate() override;
        bool OnUserUpdate(float t_elapsedTime) override;

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * @brief Sets a new GameLayer for the game. The default layer 0 is used for ImGui.
         */
        void SetGameLayer();

    protected:

    private:
        //-------------------------------------------------
        // ImGui
        //-------------------------------------------------

        /**
         * @brief Renders the ImGui UI to the screen.
         */
        void RenderImGui();
    };
}
