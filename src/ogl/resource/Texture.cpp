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

#include "Texture.h"
#include "Game.h"
#include "MdciiAssert.h"
#include "MdciiException.h"
#include "ogl/OpenGL.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::ogl::resource::Texture::Texture(std::string t_path, const bool t_loadVerticalFlipped)
    : m_path{ std::move(t_path) }
    , m_loadVerticalFlipped{ t_loadVerticalFlipped }
{
    Log::MDCII_LOG_DEBUG("[Texture::Texture()] Create Texture.");

    m_path = Game::RESOURCES_PATH + m_path;

    CreateId();
    LoadFromFile();
}

mdcii::ogl::resource::Texture::Texture(std::string t_path)
    : Texture(std::move(t_path), false)
{}

mdcii::ogl::resource::Texture::~Texture() noexcept
{
    Log::MDCII_LOG_DEBUG("[Texture::~Texture()] Destruct Texture.");

    CleanUp();
}

//-------------------------------------------------
// Bind / unbind
//-------------------------------------------------

void mdcii::ogl::resource::Texture::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, id);
}

void mdcii::ogl::resource::Texture::Unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void mdcii::ogl::resource::Texture::BindForReading(const uint32_t t_textureUnit) const
{
    // make sure that the OpenGL constants are used here
    MDCII_ASSERT(t_textureUnit >= GL_TEXTURE0 && t_textureUnit <= GL_TEXTURE15, "[Texture::BindForReading()] Invalid texture unit value.")
    glActiveTexture(t_textureUnit);
    Bind();
}

//-------------------------------------------------
// Create
//-------------------------------------------------

void mdcii::ogl::resource::Texture::CreateId()
{
    glGenTextures(1, &id);
    MDCII_ASSERT(id, "[Texture::CreateId()] Error while creating a new texture handle.")

    Log::MDCII_LOG_DEBUG("[Texture::CreateId()] A new texture handle was created. The Id is {}.", id);
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::ogl::resource::Texture::LoadFromFile()
{
    stbi_set_flip_vertically_on_load(m_loadVerticalFlipped);

    if (auto* const image{ stbi_load(m_path.c_str(), &width, &height, &m_channels, 0) })
    {
        MDCII_ASSERT(width, "[Texture::LoadFromFile()] Invalid image format.")
        MDCII_ASSERT(height, "[Texture::LoadFromFile()] Invalid image format.")
        MDCII_ASSERT(m_channels, "[Texture::LoadFromFile()] Invalid image format.")

        if (m_channels == STBI_grey)
            m_format = GL_RED;
        else if (m_channels == STBI_rgb)
            m_format = GL_RGB;
        else if (m_channels == STBI_rgb_alpha)
            m_format = GL_RGBA;

        MDCII_ASSERT(m_format, "[Texture::LoadFromFile()] Invalid image format.")

        Bind();

        glTexImage2D(GL_TEXTURE_2D, 0, m_format, width, height, 0, m_format, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);

        Log::MDCII_LOG_DEBUG("[Texture::LoadFromFile()] Texture {} was successfully loaded.", m_path);
    }
    else
    {
        throw MDCII_EXCEPTION("[Texture::LoadFromFile()] Texture failed to load at path: " + m_path);
    }
}

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::ogl::resource::Texture::CleanUp() const
{
    if (id)
    {
        Log::MDCII_LOG_DEBUG("[Texture::CleanUp()] Clean up Texture Id {}.", id);
        glDeleteTextures(1, &id);
    }
}
