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

#include <memory>
#include "StateId.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::ogl
{
    /**
     * Forward declaration class Window.
     */
    class Window;
}

namespace mdcii::camera
{
    /**
     * Forward declaration class Camera.
     */
    class Camera;
}

namespace mdcii::file
{
    /**
     * Forward declaration class OriginalResourcesManager.
     */
    class OriginalResourcesManager;
}

//-------------------------------------------------
// State
//-------------------------------------------------

namespace mdcii::state
{
    //-------------------------------------------------
    // Forward declarations
    //-------------------------------------------------

    /**
     * Forward declaration class StateStack.
     */
    class StateStack;

    //-------------------------------------------------
    // Context
    //-------------------------------------------------

    /**
     * Shared objects between all states.
     */
    struct Context
    {
        Context() = delete;

        Context(
            std::shared_ptr<ogl::Window> t_window,
            std::shared_ptr<camera::Camera> t_camera,
            std::shared_ptr<file::OriginalResourcesManager> t_originalResourcesManager
        )
            : window{ std::move(t_window) }
            , camera{ std::move(t_camera) }
            , originalResourcesManager{ std::move(t_originalResourcesManager) }
        {}

        std::shared_ptr<ogl::Window> window;
        std::shared_ptr<camera::Camera> camera;
        std::shared_ptr<file::OriginalResourcesManager> originalResourcesManager;

        StateStack* stateStack{ nullptr };
    };

    //-------------------------------------------------
    // State
    //-------------------------------------------------

    /**
     * The base class for all States.
     */
    class State
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * Works as a holder of shared objects between all States.
         */
        std::shared_ptr<Context> context;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        State() = delete;

        /**
         * Constructs a new State object.
         *
         * @param t_id The unique identifier.
         * @param t_context The holder of shared objects.
         */
        State(StateId t_id, std::shared_ptr<Context> t_context);

        State(const State& t_other) = delete;
        State(State&& t_other) noexcept = delete;
        State& operator=(const State& t_other) = delete;
        State& operator=(State&& t_other) noexcept = delete;

        virtual ~State() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        [[nodiscard]] StateId GetStateId() const { return m_id; }

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        virtual void Input() = 0;
        virtual void Update() = 0;
        virtual void Render() = 0;
        virtual void RenderImGui() = 0;

        //-------------------------------------------------
        // Frame
        //-------------------------------------------------

        virtual void StartFrame();
        virtual void EndFrame() const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The unique identifier of this State.
         */
        StateId m_id;
    };
}
