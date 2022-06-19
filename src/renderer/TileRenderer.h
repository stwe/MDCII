#pragma once

#include "ogl/Window.h"
#include "camera/Camera.h"

//-------------------------------------------------
// TileRenderer
//-------------------------------------------------

namespace mdcii::renderer
{
    /**
     * Represents a tile renderer.
     */
    class TileRenderer
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        TileRenderer();

        TileRenderer(const TileRenderer& t_other) = delete;
        TileRenderer(TileRenderer&& t_other) noexcept = delete;
        TileRenderer& operator=(const TileRenderer& t_other) = delete;
        TileRenderer& operator=(TileRenderer&& t_other) noexcept = delete;

        ~TileRenderer() noexcept;

        //-------------------------------------------------
        // Render
        //-------------------------------------------------

        /**
         * Renders a single textured tile.
         *
         * @param t_modelMatrix The model matrix to transform the position into world space.
         * @param t_textureId The OpenGL texture handle.
         * @param t_window The Window object to get the orthographic projection matrix.
         * @param t_camera The Camera object to get the view matrix.
         * @param t_selected True if the tile is selected.
         */
        void RenderTile(
            const glm::mat4& t_modelMatrix,
            uint32_t t_textureId,
            const ogl::Window& t_window,
            const camera::Camera& t_camera,
            bool t_selected = false
        ) const;

        /**
         * Renders a single textured tile for mouse picking.
         *
         * @param t_modelMatrix The model matrix to transform the position into world space.
         * @param t_textureId The OpenGL texture handle.
         * @param t_color The Id color.
         * @param t_window The Window object to get the orthographic projection matrix.
         * @param t_camera The Camera object to get the view matrix.
         */
        void RenderTileForMousePicking(
            const glm::mat4& t_modelMatrix,
            uint32_t t_textureId,
            const glm::vec3& t_color,
            const ogl::Window& t_window,
            const camera::Camera& t_camera
        ) const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The OpenGL Vertex Array Object handle.
         */
        uint32_t m_vao{ 0 };

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Creates the VAO which consists a VBO to store the vertices and sets the buffer layout.
         */
        void Init();
    };
}
