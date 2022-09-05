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

#include <glm/gtc/matrix_transform.hpp>
#include "TextRenderer.h"
#include "Log.h"
#include "MdciiException.h"
#include "ogl/OpenGL.h"
#include "ogl/resource/ResourceManager.h"
#include "ogl/resource/TextureUtils.h"
#include "ogl/buffer/Vao.h"
#include "ogl/buffer/Vbo.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::renderer::TextRenderer::TextRenderer(std::string t_fontPath)
    : m_fontPath{ std::move(t_fontPath) }
{
    Log::MDCII_LOG_DEBUG("[TextRenderer::TextRenderer()] Create TextRenderer.");

    Init();
    CreateMesh();
}

mdcii::renderer::TextRenderer::~TextRenderer() noexcept
{
    Log::MDCII_LOG_DEBUG("[TextRenderer::~TextRenderer()] Destruct TextRenderer.");
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::renderer::TextRenderer::RenderText(
    const std::string& t_text,
    float t_xPos,
    const float t_yPos,
    const float t_scale,
    const glm::vec3& t_color,
    const ogl::Window& t_window,
    const camera::Camera& t_camera
)
{
    ogl::OpenGL::EnableAlphaBlending();

    // bind shader program && update uniforms
    const auto& shaderProgram{ ogl::resource::ResourceManager::LoadShaderProgram("shader/text") };
    shaderProgram.Bind();
    shaderProgram.SetUniform("projection", t_window.GetOrthographicProjectionMatrix());
    shaderProgram.SetUniform("view", t_camera.GetViewMatrix());
    shaderProgram.SetUniform("textColor", t_color);

    // bind Vao
    m_vao->Bind();

    // iterate through all characters
    for (auto c = t_text.begin(); c != t_text.end(); ++c)
    {
        const auto ch{ m_characters[*c] };
        const auto xpos{ t_xPos + ch.bearing.x * t_scale };
        const auto ypos{ t_yPos + (m_characters['H'].bearing.y - ch.bearing.y) * t_scale };

        const auto w{ ch.size.x * t_scale };
        const auto h{ ch.size.y * t_scale };

        // update Vbo for each character
        const float vertices[6][4] = {
            { xpos, ypos + h, 0.0f, 1.0f },
            { xpos + w, ypos, 1.0f, 0.0f },
            { xpos, ypos, 0.0f, 0.0f },

            { xpos, ypos + h, 0.0f, 1.0f },
            { xpos + w, ypos + h, 1.0f, 1.0f },
            { xpos + w, ypos, 1.0f, 0.0f }
        };

        // bind a texture
        ogl::resource::TextureUtils::BindForReading(ch.textureId, GL_TEXTURE0);
        shaderProgram.SetUniform("textTexture", 0);

        // update content of Vbo memory
        m_vao->vbos.at(0)->Bind();
        ogl::buffer::Vbo::StoreData(0, sizeof(vertices), vertices);
        ogl::buffer::Vbo::Unbind();

        // render
        m_vao->DrawPrimitives();

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        t_xPos += static_cast<float>(ch.advance >> 6) * t_scale;
    }

    // unbind Vao
    ogl::buffer::Vao::Unbind();

    ogl::resource::TextureUtils::Unbind();
    ogl::resource::ShaderProgram::Unbind();

    ogl::OpenGL::DisableBlending();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::renderer::TextRenderer::Init()
{
    FT_Library ft;

    // All functions return a value different than 0 whenever an error occurred.
    if (FT_Init_FreeType(&ft))
    {
        throw MDCII_EXCEPTION("[TextRenderer::Init()] Could not init FreeType Library.");
    }

    // Load font as face.
    FT_Face face;
    if (FT_New_Face(ft, m_fontPath.c_str(), 0, &face))
    {
        throw MDCII_EXCEPTION("[TextRenderer::Init()] Failed to load font.");
    }

    // Set size to load glyphs as.
    FT_Set_Pixel_Sizes(face, 0, 48);

    // Disable byte-alignment restriction.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load first 128 characters of ASCII set.
    for (uint8_t c{ 0 }; c < 128; ++c)
    {
        // Load character glyph.
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            throw MDCII_EXCEPTION("[TextRenderer::Init()] Failed to load Glyph.");
        }

        // Generate texture.
        glGenTextures(1, &m_textureId);
        glBindTexture(GL_TEXTURE_2D, m_textureId);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            static_cast<int32_t>(face->glyph->bitmap.width),
            static_cast<int32_t>(face->glyph->bitmap.rows),
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        // Set texture options.
        ogl::resource::TextureUtils::UseClampToEdgeWrapping();
        ogl::resource::TextureUtils::UseBilinearFilter();

        // Now store character for later use.
        Character character = {
            m_textureId,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };

        m_characters.insert(std::pair<char, Character>(c, character));
    }

    // Destroy FreeType once we're finished.
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void mdcii::renderer::TextRenderer::CreateMesh()
{
    // create && bind vao
    m_vao = std::make_unique<ogl::buffer::Vao>();
    m_vao->Bind();

    // create && bind Vbo
    auto vbo{ std::make_unique<ogl::buffer::Vbo>() };
    vbo->Bind();

    // reserve enough memory
    ogl::buffer::Vbo::ReserveMemory(sizeof(float) * 6 * 4);

    // set buffer layout
    ogl::buffer::Vbo::AddFloatAttribute(0, 4, 4, 0);

    // set draw count
    m_vao->drawCount = 6;

    // unbind Vbo && Vao
    ogl::buffer::Vbo::Unbind();
    ogl::buffer::Vao::Unbind();

    // save Vbo in the Vao
    m_vao->vbos.emplace_back(std::move(vbo));
}
