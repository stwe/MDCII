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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace mdcii::ogl
{
    /**
     * Enable or disable OpenGL capabilities.
     */
    class OpenGL
    {
    public:
        //-------------------------------------------------
        // OpenGL states
        //-------------------------------------------------

        /**
         * Specify clear values for the color buffers.
         *
         * @param t_red red - range [0 ... 1].
         * @param t_green green - range [0 ... 1].
         * @param t_blue blue - range [0 ... 1].
         * @param t_alpha alpha - range [0 ... 1].
         */
        static void SetClearColor(const float t_red, const float t_green, const float t_blue, const float t_alpha)
        {
            glClearColor(t_red, t_green, t_blue, t_alpha);
        }

        /**
         * Clear buffers to preset values.
         */
        static void Clear()
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        }

        /**
         * Enable alpha blending.
         */
        static void EnableAlphaBlending()
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }

        /**
         * Disable blending.
         */
        static void DisableBlending()
        {
            glDisable(GL_BLEND);
        }

        /**
         * Enable wireframe mode.
         */
        static void EnableWireframeMode()
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        /**
         * Disable wireframe mode.
         */
        static void DisableWireframeMode()
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        /**
         * Enable face culling.
         */
        static void EnableFaceCulling()
        {
            // On a freshly created OpenGL Context, the default front face is GL_CCW.
            // All the faces that are not front-faces are discarded.
            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);
            glEnable(GL_CULL_FACE);
        }

        /**
         * Disable face culling.
         */
        static void DisableFaceCulling()
        {
            glDisable(GL_CULL_FACE);
        }

        /**
         * Enable depth and stencil buffer.
         */
        static void EnableDepthAndStencilTesting()
        {
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_STENCIL_TEST);
            glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
        }

        /**
         * Set depth function to default.
         */
        static void SetDefaultDepthFunction()
        {
            glDepthFunc(GL_LESS);
        }

        /**
         * Change depth function so depth test passes when values are equal to depth buffer's content.
         */
        static void SetEqualDepthFunction()
        {
            glDepthFunc(GL_LEQUAL);
        }

        /**
         * Enable depth testing.
         */
        static void EnableDepthTesting()
        {
            glEnable(GL_DEPTH_TEST);
        }

        /**
         * Disable depth testing.
         */
        static void DisableDepthTesting()
        {
            glDisable(GL_DEPTH_TEST);
        }

    protected:

    private:

    };
}
