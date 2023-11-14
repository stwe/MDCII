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

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii
{
    /**
     * @brief Forward declaration class GameState.
     */
    class GameState;
}

namespace mdcii::renderer
{
    /**
     * @brief Forward declaration class Renderer.
     */
    class Renderer;
}

namespace mdcii::camera
{
    /**
     * @brief Forward declaration class Camera.
     */
    class Camera;
}

namespace mdcii::world
{
    /**
     * @brief Forward declaration class Island.
     */
    class Island;

    /**
     * @brief Forward declaration class DeepWater.
     */
    class DeepWater;

    //-------------------------------------------------
    // World
    //-------------------------------------------------

    /**
     * @brief Represents the game world.
     */
    class World
    {
    public:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * @brief The building Id used for the deep water area.
         */
        static constexpr auto DEEP_WATER_BUILDING_ID{ 1201 };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief Pointer to the parent GameState.
         */
        GameState* gameState{ nullptr };

        /**
         * @brief The width of the world in tiles.
         */
        int worldWidth{ -1 };

        /**
         * @brief The height of the world in tiles.
         */
        int worldHeight{ -1 };

        /**
         * @brief A collection of unique pointers to Island objects.
         */
        std::vector<std::unique_ptr<Island>> islands;

        /**
         * @brief Pointer to the Islands objects that are currently being rendered.
         */
        std::vector<Island*> currentIslands;

        /**
         * @brief Pointer to the DeepWater object.
         */
        std::unique_ptr<DeepWater> deepWater;

        /**
         * @brief For rendering this world.
         */
        std::unique_ptr<renderer::Renderer> renderer;

        /**
         * @brief A camera to move around.
         */
        std::unique_ptr<camera::Camera> camera;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        World() = delete;

        /**
         * @brief Constructs a new World object.
         *
         * @param t_gameState Pointer to the parent GameState.
         * @param t_fileName The name of the save-game file.
         */
        World(GameState* t_gameState, const std::string& t_fileName);

        World(const World& t_other) = delete;
        World(World&& t_other) noexcept = delete;
        World& operator=(const World& t_other) = delete;
        World& operator=(World&& t_other) noexcept = delete;

        ~World() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void OnUserUpdate(float t_elapsedTime);

        //-------------------------------------------------
        // World / screen positions
        //-------------------------------------------------

        /**
         * @brief Projects a world position into an isometric position on the screen.
         *
         * @param t_x The x position.
         * @param t_y The y position.
         *
         * @return The isometric coordinates on the screen.
         */
        [[nodiscard]] olc::vi2d ToScreen(int t_x, int t_y) const;

        /**
         * @brief Checks if the given world position falls on any island.
         *
         * @param t_x The x position.
         * @param t_y The y position.
         *
         * @return True if the position falls on an island. Otherwise, it returns false.
         */
        [[nodiscard]] bool IsWorldPositionOnAnyIsland(int t_x, int t_y) const;

        /**
         * @brief Checks if the given world position is outside the screen.
         *
         * @param t_x The x position.
         * @param t_y The y position.
         *
         * @return True if the position is outside the screen. Otherwise, it returns false.
         */
        [[nodiscard]] bool IsWorldPositionOutsideScreen(int t_x, int t_y) const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief Initial access flag.
         */
        bool m_flag{ true };

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * @brief Initializes the game world.
         *
         * @param t_fileName The name of the save-game file.
         */
        void Init(const std::string& t_fileName);

        /**
         * @brief Creates the deep water area.
         *
         * @return A unique pointer to the created DeepWater object.
         */
        [[nodiscard]] std::unique_ptr<DeepWater> CreateDeepWaterArea() const;

        //-------------------------------------------------
        // Render helper
        //-------------------------------------------------

        /**
         * @brief Identifies the visible islands and tiles for rendering.
         */
        void FindVisibleIslands();

        /**
         * @brief Identifies the visible deep water tiles for rendering.
         */
        void FindVisibleDeepWaterTiles() const;
    };
}
