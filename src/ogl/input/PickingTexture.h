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

#include <glm/vec3.hpp>
#include <cstdint>

//-------------------------------------------------
// PickingTexture
//-------------------------------------------------

namespace mdcii::ogl::input
{
    /**
     * Represents a Framebuffer for mouse picking.
     */
    class PickingTexture
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        PickingTexture() = delete;

        /**
         * Constructs a new PickingTexture object.
         *
         * @param t_width The window width.
         * @param t_height The window height.
         */
        [[deprecated]] PickingTexture(int t_width, int t_height);

        PickingTexture(const PickingTexture& t_other) = delete;
        PickingTexture(PickingTexture&& t_other) noexcept = delete;
        PickingTexture& operator=(const PickingTexture& t_other) = delete;
        PickingTexture& operator=(PickingTexture&& t_other) noexcept = delete;

        ~PickingTexture() noexcept;

        //-------------------------------------------------
        // Write / render to texture
        //-------------------------------------------------

        /**
         * Enables the picking texture for writing so we can render into the texture.
         */
        void EnableWriting() const;

        /**
         * Finish rendering into the picking texture.
         */
        static void DisableWriting();

        //-------------------------------------------------
        // Read from texture
        //-------------------------------------------------

        /**
         * Bind the Fbo to the GL_READ_FRAMEBUFFER target so wie can read from it.
         */
        void EnableReading() const;

        /**
         * Finish reading from the picking texture.
         */
        static void DisableReading();

        /**
         * This method takes a screen position and gets the
         * corresponding pixel from the picking texture. The
         * pixel color is converted back to an Id.
         *
         * @param t_x The x position on the screen.
         * @param t_y The y position on the screen.
         *
         * @return The ID converted from the color.
         */
        [[nodiscard]] int ReadMapIndex(int t_x, int t_y) const;

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Converts an Id into an RGB color.
         *
         * @param t_id The Id to convert.
         *
         * @return The rgb values.
         */
        static glm::vec3 CreateIdColor(int t_id);

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The width of the window.
         */
        int m_width{ 0 };

        /**
         * The height of the window.
         */
        int m_height{ 0 };

        /**
         * The handle of the Fbo.
         */
        uint32_t m_fboId{ 0 };

        /**
         * The picking texture handle.
         */
        uint32_t m_pickingTextureId{ 0 };

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initializes the PickingTexture class.
         */
        void Init();

        //-------------------------------------------------
        // Create
        //-------------------------------------------------

        /**
         * Creates a new Fbo handle.
         */
        void CreateId();

        //-------------------------------------------------
        // Bind / unbind
        //-------------------------------------------------

        void Bind() const;
        static void Unbind();

        //-------------------------------------------------
        // Clean up
        //-------------------------------------------------

        /**
         * Delete Fbo and picking texture.
         */
        void CleanUp() const;
    };
}
