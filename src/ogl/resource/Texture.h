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

#include <string>

//-------------------------------------------------
// Texture
//-------------------------------------------------

namespace mdcii::ogl::resource
{
    /**
     * Loads an image from a file and creates a texture.
     */
    class Texture
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The texture handle.
         */
        uint32_t id{ 0 };

        /**
         * The texture width.
         */
        int width{ 0 };

        /**
         * The texture height.
         */
        int height{ 0 };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Texture() = delete;

        /**
         * Constructs a new Texture object.
         *
         * @param t_path The path to the texture file to load.
         * @param t_loadVerticalFlipped Flip the image vertically.
         */
        Texture(std::string t_path, bool t_loadVerticalFlipped);

        /**
         * Constructs a new Texture object.
         *
         * @param t_path The path to the texture file to load.
         */
        explicit Texture(std::string t_path);

        Texture(const Texture& t_other) = delete;
        Texture(Texture&& t_other) noexcept = delete;
        Texture& operator=(const Texture& t_other) = delete;
        Texture& operator=(Texture&& t_other) noexcept = delete;

        ~Texture() noexcept;

        //-------------------------------------------------
        // Bind / unbind
        //-------------------------------------------------

        /**
         * Binds a texture.
         */
        void Bind() const;

        /**
         * Unbinds a texture.
         */
        static void Unbind();

        /**
         * Activates a texture unit.
         */
        void BindForReading(uint32_t t_textureUnit) const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The path to the texture file to load.
         */
        std::string m_path;

        /**
         * Flip the image vertically.
         */
        bool m_loadVerticalFlipped{ false };

        /**
         * The internal OpenGL texture format.
         */
        int m_format{ 0 };

        /**
         * The number of color channels.
         */
        int m_channels{ 0 };

        //-------------------------------------------------
        // Create
        //-------------------------------------------------

        /**
         * Creates a new texture handle.
         */
        void CreateId();

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Loads an image from a file.
         */
        void LoadFromFile();

        //-------------------------------------------------
        // Clean up
        //-------------------------------------------------

        /**
         * Clean up / delete texture.
         */
        void CleanUp() const;
    };
}
