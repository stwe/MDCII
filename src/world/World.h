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

#include <unordered_set>
#include "vendor/olc/olcPixelGameEngine.h"
#include "vendor/nlohmann/json.hpp"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::state
{
    /**
     * @brief Forward declaration class State.
     */
    class State;
}

namespace mdcii::camera
{
    /**
     * @brief Forward declaration class Camera.
     */
    class Camera;
}

namespace mdcii::resource
{
    /**
     * @brief Forward declaration class TileAtlas.
     */
    class TileAtlas;

    /**
     * @brief Forward declaration class AnimalsTileAtlas.
     */
    class AnimalsTileAtlas;
}

namespace mdcii::world::layer
{
    /**
     * @brief Forward declaration enum class LayerType.
     */
    enum class LayerType;
}

namespace mdcii::world::tile
{
    /**
     * @brief Forward declaration struct TerrainTile.
     */
    struct TerrainTile;
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

    /**
     * @brief Forward declaration class MousePicker.
     */
    class MousePicker;

    //-------------------------------------------------
    // Render layer options
    //-------------------------------------------------

    /**
     * @brief Options for which layers are rendered.
     */
    enum RenderLayer
    {
        RENDER_COAST_LAYER = 1,
        RENDER_TERRAIN_LAYER = 2,
        RENDER_BUILDINGS_LAYER = 4,
        RENDER_FIGURES_LAYER = 8,
        RENDER_MIXED_LAYER = 16,
        RENDER_DEEP_WATER_LAYER = 32,
        RENDER_ALL = 64
    };

    //-------------------------------------------------
    // Current
    //-------------------------------------------------

    /**
     * @brief Encapsulates the current island and the current position of the mouse on this island.
     */
    struct CurrentIsland
    {
        /**
         * @brief Pointer to the current Island object under mouse.
         */
        Island* island{ nullptr };

        /**
         * @brief The mouse position relative to the current island.
         */
        olc::vi2d position{ -1, -1 };
    };

    /**
     * @brief Encapsulates the current tiles under the mouse.
     */
    struct CurrentTiles
    {
        /**
         * @brief Pointer to the current terrain layer tile.
         */
        tile::TerrainTile* terrainTile{ nullptr };

        /**
         * @brief Pointer to the current building layer tile.
         */
        tile::TerrainTile* buildingTile{ nullptr };
    };

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
        // Member
        //-------------------------------------------------

        /**
         * @brief Pointer to the parent State object.
         */
        state::State* state{ nullptr };

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
        std::unordered_set<Island*> currentIslands;

        /**
         * @brief Pointer to the DeepWater object.
         */
        std::unique_ptr<DeepWater> deepWater;

        /**
         * @brief A camera to move around.
         */
        std::unique_ptr<camera::Camera> camera;

        /**
         * @brief Pointer to the MousePicker object.
         */
        std::unique_ptr<world::MousePicker> mousePicker;

        /**
         * @brief Pointer to the TileAtlas object.
         */
        std::unique_ptr<resource::TileAtlas> tileAtlas;

        /**
         * @brief Pointer to the AnimalsTileAtlas object.
         */
        std::unique_ptr<resource::AnimalsTileAtlas> animalsTileAtlas;

        /**
         * @brief Renders a grid on the deep water.
         */
        bool renderDeepWaterGrid{ false };

        /**
         * @brief Renders a grid on all the islands terrain.
         */
        bool renderIslandsGrid{ false };

        /**
         * @brief Representing the currently selected layers for rendering in the game world.
         */
        int renderLayer{ RENDER_ALL };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        World() = delete;

        /**
         * @brief Constructs a new World object.
         *
         * @param t_state Pointer to the parent State object.
         * @param t_width The width of the world in tiles.
         * @param t_height The height of the world in tiles.
         */
        World(state::State* t_state, int t_width, int t_height);

        /**
         * @brief Constructs a new World object.
         *
         * @param t_state Pointer to the parent State object.
         * @param t_json A Json value representing a World.
         */
        World(state::State* t_state, const nlohmann::json& t_json);

        World(const World& t_other) = delete;
        World(World&& t_other) noexcept = delete;
        World& operator=(const World& t_other) = delete;
        World& operator=(World&& t_other) noexcept = delete;

        ~World() noexcept;

        //-------------------------------------------------
        // Create core objects
        //-------------------------------------------------

        /**
         * @brief Creates all objects that depend on setting the size of the world.
         */
        void CreateCoreObjects();

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void OnUserUpdate(float t_elapsedTime);

        //-------------------------------------------------
        // ImGui
        //-------------------------------------------------

        void RenderImGui();

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
         * @brief Checks whether the given world position falls on any island.
         *
         * @param t_x The x position.
         * @param t_y The y position.
         *
         * @return True if the position falls on an island. Otherwise, it returns false.
         */
        [[nodiscard]] bool IsWorldPositionOverAnyIsland(int t_x, int t_y) const;

        /**
         * @brief Determines the current island under the mouse.
         */
        void SetCurrentIslandUnderMouse();

        /**
         * @brief Determines the current tiles under the mouse.
         */
        void SetCurrentTilesUnderMouse();

        /**
         * @brief Checks whether the given world position is outside the screen.
         *
         * @param t_x The x position.
         * @param t_y The y position.
         *
         * @return True if the position is outside the screen. Otherwise, it returns false.
         */
        [[nodiscard]] bool IsWorldPositionOutsideScreen(int t_x, int t_y) const;

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * @brief A helper function to check whether a given render layer option is active.
         *
         * @param t_option The render layer option to check.
         *
         * @return True if the option is active, false otherwise.
         */
        [[nodiscard]] bool HasRenderLayerOption(RenderLayer t_option) const { return (renderLayer & t_option) != 0; }

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief Initial access flag.
         */
        bool m_flag{ true };

        /**
         * @brief The current island under the mouse.
         */
        CurrentIsland m_currentIslandUnderMouse;

        /**
         * @brief The current terrain and building layer tile under the mouse.
         */
        CurrentTiles m_currentTilesUnderMouse;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * @brief Initializes the game world.
         *
         * @param t_json A Json value representing a World.
         */
        void Init(const nlohmann::json& t_json);

        //-------------------------------------------------
        // Render helper
        //-------------------------------------------------

        /**
         * @brief Adds the given island to the render list.
         *
         * @param t_island The island to be added to the render list.
         */
        void AddIslandToRenderList(Island* t_island);

        /**
         * @brief Identifies the islands visible on the screen for rendering.
         */
        void FindVisibleIslands();

        /**
         * @brief Identifies the deep water tiles visible on the screen for rendering.
         */
        void FindVisibleDeepWaterTiles() const;

        //-------------------------------------------------
        //  Render layer bits
        //-------------------------------------------------

        /**
         * @brief Toggles the given render layer option.
         *
         * @param t_renderLayer The render layer option to be toggled.
         */
        void ToggleRenderLayer(RenderLayer t_renderLayer);

        /**
         * @brief Disables the given render layer option.
         *
         * @param t_renderLayer The render layer option to be disabled.
         * @param t_toFalse A reference boolean that will be set to false.
         */
        void DisableRenderLayer(RenderLayer t_renderLayer, bool& t_toFalse);
    };

    //-------------------------------------------------
    // Serializing World into Json
    //-------------------------------------------------

    void to_json(nlohmann::json& t_json, const World& t_world);
}
