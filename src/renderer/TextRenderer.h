#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include "ogl/Window.h"
#include "camera/Camera.h"

//-------------------------------------------------
// TextRenderer
//-------------------------------------------------

namespace mdcii::renderer
{
    //-------------------------------------------------
    // TextRenderer
    //-------------------------------------------------

    /**
     * FreeType text rendering.
     */
    class TextRenderer
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        TextRenderer() = delete;

        /**
         * Constructs a new TextRenderer object.
         *
         * @param t_fontPath The path to the TrueType font file.
         */
        explicit TextRenderer(std::string t_fontPath);

        TextRenderer(const TextRenderer& t_other) = delete;
        TextRenderer(TextRenderer&& t_other) noexcept = delete;
        TextRenderer& operator=(const TextRenderer& t_other) = delete;
        TextRenderer& operator=(TextRenderer&& t_other) noexcept = delete;

        ~TextRenderer() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        /**
         * Renders a string of characters.
         *
         * @param t_text The text to render.
         * @param t_xPos The x position in world space.
         * @param t_yPos The y position in world space.
         * @param t_scale The scaling of the text.
         * @param t_color The text color.
         * @param t_window The Window object to get the orthographic projection matrix.
         * @param t_camera The Camera object to get the view matrix.
         */
        void RenderText(
            const std::string& t_text,
            float t_xPos,
            float t_yPos,
            float t_scale,
            const glm::vec3& t_color,
            const ogl::Window& t_window,
            const camera::Camera& t_camera
        );

    protected:

    private:
        //-------------------------------------------------
        // Types
        //-------------------------------------------------

        /**
         * Represents a character glyph.
         */
        struct Character
        {
            uint32_t textureId;   // Id handle of the glyph texture.
            glm::ivec2 size;      // Size of glyph.
            glm::ivec2 bearing;   // Offset from baseline to left/top of glyph.
            signed long advance;  // Horizontal offset to advance to next glyph.
        };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * All character glyphs.
         */
        std::map<char, Character> m_characters;

        /**
         * The path to the TrueType font file.
         */
        std::string m_fontPath;

        /**
         * The texture handle.
         */
        uint32_t m_textureId{ 0 };

        /**
         * The Vao handle.
         */
        uint32_t m_vaoId{ 0 };

        /**
         * The Vbo handle.
         */
        uint32_t m_vboId{ 0 };

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initializes the FreeType library.
         */
        void Init();

        /**
         * Creates a Vbo and Vao for rendering quads.
         */
        void CreateMesh();
    };
}
