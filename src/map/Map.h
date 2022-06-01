#pragma once

#include <vector>
#include "ogl/Window.h"
#include "camera/Camera.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::data
{
    class HousesJsonFile;
}

namespace mdcii::file
{
    class PaletteFile;
    class BshFile;
}

namespace mdcii::renderer
{
    class TileRenderer;
    class TextRenderer;
}

//-------------------------------------------------
// Map
//-------------------------------------------------

namespace mdcii::map
{
    /**
     * Represents the possible map rotations.
     */
    enum class Rotation
    {
        DEG0, DEG90, DEG180, DEG270
    };

    /**
     * Represents a map with one or many islands.
     */
    class Map
    {
    public:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * No Id selected.
         */
        static constexpr auto INVALID_GFX_ID{ -1 };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The currently selected gfx Id.
         */
        int selectedId{ INVALID_GFX_ID };

        /**
         * The map rotation.
         */
        Rotation rotation{ Rotation::DEG0 };

        /**
         * Content from the haeuser.cod file as json.
         */
        std::unique_ptr<data::HousesJsonFile> housesJsonFile;

        /**
         * The bsh graphics from the stadtfld.bsh file.
         */
        std::unique_ptr<file::BshFile> stdBshFile;

        /**
         * The bsh graphics from the bauhaus.bsh file.
         */
        std::unique_ptr<file::BshFile> bauhausBshFile;

        /**
         * A tile renderer to render bsh graphics as GPU texture.
         */
        std::unique_ptr<renderer::TileRenderer> renderer;

        /**
         * Lets OpenGL render a text string.
         */
        std::unique_ptr<renderer::TextRenderer> textRenderer;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Map();

        Map(const Map& t_other) = delete;
        Map(Map&& t_other) noexcept = delete;
        Map& operator=(const Map& t_other) = delete;
        Map& operator=(Map&& t_other) noexcept = delete;

        ~Map() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        /**
         * Renders the map with the correct rotation.
         *
         * @param t_window The Window object to get the orthographic projection matrix.
         * @param t_camera The Camera object to get the view matrix.
         */
        void Render(const ogl::Window& t_window, const camera::Camera& t_camera) const;

        //-------------------------------------------------
        // Rotate map
        //-------------------------------------------------

        /**
         * Rotate map clockwise.
         */
        [[deprecated]] void RotateMapCw();

        /**
         * Rotate map counterclockwise.
         */
        [[deprecated]] void RotateMapCcw();

        //-------------------------------------------------
        // Utils
        //-------------------------------------------------

        /**
         * Projects map coordinates in isometric view.
         *
         * @param t_mapX The map x position.
         * @param t_mapY The map y position.
         *
         * @return The isometric coordinates.
         */
        [[nodiscard]] static glm::vec2 MapToIso(int t_mapX, int t_mapY);

        /**
         * Projects map coordinates in isometric view.
         *
         * @param t_mapX The map x position.
         * @param t_mapY The map y position.
         * @param t_rotation The current map rotation.
         *
         * @return The isometric coordinates.
         */
        [[nodiscard]] glm::vec2 MapToIso(int t_mapX, int t_mapY, Rotation t_rotation) const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The map width.
         */
        int m_width{ 4 };

        /**
         * The map height.
         */
        int m_height{ 8 };

        /**
         * The map content (4x8).
         */
        std::vector<int> m_map
        {
            1165, 1094, 1094, 1166,
            1093,    4, 0,    1095,
            1093,    0, 0,    1095,
            1093,    0, 0,    1095,
            1093,    0, 0,    1095,
            1093,    0, 0,    1095,
            1093,    0, 0,    1095,
            1164, 1092, 1092, 1167,
        };

        /**
         * Pallet values from the from the stadtfld.col file.
         */
        std::unique_ptr<file::PaletteFile> m_paletteFile;

        //-------------------------------------------------
        // Render helper
        //-------------------------------------------------

        /**
         * Renders the map content.
         *
         * @param t_mapX The map x position.
         * @param t_mapY The map y position.
         * @param t_window The Window object to get the orthographic projection matrix.
         * @param t_camera The Camera object to get the view matrix.
         */
        void RenderMapContent(int t_mapX, int t_mapY, const ogl::Window& t_window, const camera::Camera& t_camera) const;

        /**
         * Renders a grid tile.
         *
         * @param t_mapX The map x position.
         * @param t_mapY The map y position.
         * @param t_window The Window object to get the orthographic projection matrix.
         * @param t_camera The Camera object to get the view matrix.
         */
        void RenderGridTile(int t_mapX, int t_mapY, const ogl::Window& t_window, const camera::Camera& t_camera) const;

        /**
         * Renders a building tile.
         *
         * @param t_mapX The map x position.
         * @param t_mapY The map y position.
         * @param t_window The Window object to get the orthographic projection matrix.
         * @param t_camera The Camera object to get the view matrix.
         */
        void RenderBuildingTile(int t_mapX, int t_mapY, const ogl::Window& t_window, const camera::Camera& t_camera) const;

        /**
         * Renders the text.
         *
         * @param t_mapX The map x position.
         * @param t_mapY The map y position.
         * @param t_window The Window object to get the orthographic projection matrix.
         * @param t_camera The Camera object to get the view matrix.
         */
        void RenderText(int t_mapX, int t_mapY, const ogl::Window& t_window, const camera::Camera& t_camera) const;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Create objects.
         */
        void Init();
    };

    //-------------------------------------------------
    // Operators
    //-------------------------------------------------

    inline Rotation& operator++(Rotation& t_rotation)
    {
        if (t_rotation == Rotation::DEG270)
        {
            t_rotation = Rotation::DEG0;
        }
        else
        {
            t_rotation = static_cast<Rotation>(static_cast<std::underlying_type_t<Rotation>>(t_rotation) + 1);
        }

        return t_rotation;
    }

    inline Rotation& operator--(Rotation& t_rotation)
    {
        if (t_rotation == Rotation::DEG0)
        {
            t_rotation = Rotation::DEG270;
        }
        else
        {
            t_rotation = static_cast<Rotation>(static_cast<std::underlying_type_t<Rotation>>(t_rotation) - 1);
        }

        return t_rotation;
    }
}
