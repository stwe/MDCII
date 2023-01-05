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

#include <glm/mat4x4.hpp>
#include "world/Rotation.h"
#include "world/Zoom.h"
#include "event/EventManager.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::state
{
    /**
     * Forward declaration struct Context.
     */
    struct Context;
}

namespace mdcii::ogl::buffer
{
    /**
     * Forward declaration class Ssbo.
     */
    class Ssbo;
}

namespace mdcii::world
{
    /**
     * Forward declaration class World.
     */
    class World;
}

//-------------------------------------------------
// GameLayer
//-------------------------------------------------

namespace mdcii::layer
{
    /**
     * Layer types using for rendering islands.
     */
    enum class LayerType
    {
        COAST,      // renders the coast of an island
        TERRAIN,    // renders the terrain of an island
        BUILDINGS,  // renders the buildings of an island
        MIXED,      // renders the terrain && buildings of an island
        ALL,        // renders the coast and the mixed stuff of an island
        NOTHING     // doesn't render anything from an island
    };

    /**
     * The base class of all Layers.
     */
    class GameLayer
    {
    public:
        //-------------------------------------------------
        // Types
        //-------------------------------------------------

        /**
         * The model matrices for each of the four possible rotations.
         */
        using Model_Matrices_For_Each_Rotation = std::array<std::vector<glm::mat4>, world::NR_OF_ROTATIONS>;

        /**
         * The four containers with the model matrices for each zoom.
         */
        using Model_Matrices_For_Each_Zoom = std::array<Model_Matrices_For_Each_Rotation, world::NR_OF_ZOOMS>;

        /**
         * The four containers with the model matrices Ssbos for each zoom.
         */
        using Model_Matrices_Ssbos_For_Each_zoom = std::array<std::array<std::unique_ptr<ogl::buffer::Ssbo>, world::NR_OF_ROTATIONS>, world::NR_OF_ZOOMS>;

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The type of the Layer.
         */
        LayerType layerType{ LayerType::NOTHING };

        /**
         * The Layer width.
         */
        int32_t width{ -1 };

        /**
         * The Layer height.
         */
        int32_t height{ -1 };

        /**
         * The four containers with the model matrices for each zoom.
         * Access: modelMatrices[0/SGFX ... 2/GFX][0/DEG0 ... 3/DEG270][0 ... instances]
         */
        Model_Matrices_For_Each_Zoom modelMatrices;

        /**
         * The Ssbos to store all modelMatrices.
         * Access: modelMatricesSsbos[0/SGFX ... 2/GFX][0/DEG0 ... 3/DEG270]
         */
        Model_Matrices_Ssbos_For_Each_zoom modelMatricesSsbos;

        /**
         * The number of instances to render.
         */
        int32_t instancesToRender{ -1 };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        GameLayer() = delete;

        /**
         * Constructs a new GameLayer object.
         *
         * @param t_context Access to shared objects.
         * @param t_world The World object.
         */
        GameLayer(std::shared_ptr<state::Context> t_context, world::World* t_world);

        GameLayer(const GameLayer& t_other) = delete;
        GameLayer(GameLayer&& t_other) noexcept = delete;
        GameLayer& operator=(const GameLayer& t_other) = delete;
        GameLayer& operator=(GameLayer&& t_other) noexcept = delete;

        virtual ~GameLayer() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * There are four arrays for each zoom level holding model matrices.
         * The function returns these arrays.
         *
         * @param t_zoom The zoom level.
         *
         * @return The model matrices of the zoom.
         */
        [[nodiscard]] const Model_Matrices_For_Each_Rotation& GetModelMatrices(world::Zoom t_zoom) const;

        //-------------------------------------------------
        // Map index
        //-------------------------------------------------

        /**
         * 2D/1D - mapping of a Layer position.
         *
         * @param t_x The x position on this Layer.
         * @param t_y The y position on this Layer.
         * @param t_rotation The given position is previously rotated by the specified value.
         *
         * @return The 1D index.
         */
        [[nodiscard]] int32_t GetMapIndex(int32_t t_x, int32_t t_y, world::Rotation t_rotation) const;

        /**
         * 2D/1D - mapping of a Layer position.
         *
         * @param t_position The position on this Layer.
         * @param t_rotation The given position is previously rotated by the specified value.
         *
         * @return The 1D index.
         */
        [[nodiscard]] int32_t GetMapIndex(const glm::ivec2& t_position, world::Rotation t_rotation) const;

        //-------------------------------------------------
        // Prepare rendering
        //-------------------------------------------------

        /**
         * Prepare Cpu data for rendering.
         */
        void PrepareCpuDataForRendering();

        /**
         * Prepare Gpu data for rendering.
         */
        void PrepareGpuDataForRendering();

    protected:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * To have access to the shared objects (Window, Camera, Original-Assets).
         */
        std::shared_ptr<state::Context> m_context;

        /**
         * The World object.
         */
        world::World* m_world{ nullptr };

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The mouse button pressed listener handle.
         */
        decltype(event::EventManager::event_dispatcher)::Handle m_mouseButtonPressed;

        //-------------------------------------------------
        // Interface
        //-------------------------------------------------

        virtual void CreateTiles() {}
        virtual void SortTiles() {}

        virtual void CreateModelMatricesContainer() {}
        virtual void CreateGfxNumbersContainer() {}
        virtual void CreateBuildingIdsContainer() {}

        virtual void StoreModelMatricesInGpu();
        virtual void StoreGfxNumbersInGpu() {}
        virtual void StoreBuildingIdsInGpu() {}

        virtual void OnLeftMouseButtonPressed() {}

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Adds event listeners.
         */
        void AddListeners();

        //-------------------------------------------------
        // Clean up
        //-------------------------------------------------

        /**
         * Clean up.
         */
        void CleanUp() const;
    };
}
