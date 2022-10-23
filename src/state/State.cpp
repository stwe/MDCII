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

#include "State.h"
#include "StateStack.h"
#include "MdciiAssert.h"
#include "ogl/Window.h"
#include "ogl/OpenGL.h"
#include "file/OriginalResourcesManager.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::state::State::State(const StateId t_id, std::shared_ptr<Context> t_context)
    : context{ std::move(t_context) }
    , m_id{ t_id }
{
    Log::MDCII_LOG_DEBUG("[State::State()] Create State.");

    MDCII_ASSERT(context, "[State::State()] Null pointer")
}

mdcii::state::State::~State() noexcept
{
    Log::MDCII_LOG_DEBUG("[State::~State()] Destruct State.");
}

//-------------------------------------------------
// Frame
//-------------------------------------------------

void mdcii::state::State::StartFrame()
{
    ogl::OpenGL::SetClearColor(0.39f, 0.58f, 0.93f, 1.0f);
    ogl::OpenGL::Clear();
}

void mdcii::state::State::EndFrame() const
{
    context->window->SwapBuffersAndCallEvents();
}
