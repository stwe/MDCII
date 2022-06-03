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

namespace mdcii::map
{
    //-------------------------------------------------
    // Rotation
    //-------------------------------------------------

    /**
     * Represents the possible map rotations.
     */
    enum class Rotation
    {
        DEG0, DEG90, DEG180, DEG270
    };

    //-------------------------------------------------
    // Map
    //-------------------------------------------------

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

        /**
         * The width of a tile.
         */
        static constexpr auto TILE_WIDTH{ 64 };

        /**
         * The half width of a tile.
         */
        static constexpr auto TILE_WIDTH_HALF{ 32 };

        /**
         * The height of a tile.
         */
        static constexpr auto TILE_HEIGHT{ 32 };

        /**
         * The half height of a tile.
         */
        static constexpr auto TILE_HEIGHT_HALF{ 16 };

        /**
         * The height of the island.
         */
        static constexpr auto ELEVATION{ 20 };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The currently selected gfx Id.
         */
        int selectedId{ INVALID_GFX_ID };

        /**
         * The width of the map in tiles.
         */
        int width{ 16 };

        /**
         * The height of the map in tiles.
         */
        int height{ 24 };

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
         * A tile renderer to render bsh graphics as Gpu texture.
         */
        std::unique_ptr<renderer::TileRenderer> renderer;

        /**
         * Lets OpenGL render a text string.
         */
        std::unique_ptr<renderer::TextRenderer> textRenderer;

        // render options which can enable or disable

        bool renderGrid{ false };
        bool renderBuildings{ true };
        bool renderText{ false };

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

        /**
         * Renders ImGui menus.
         */
        void RenderImGui();

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
        [[nodiscard]] glm::vec2 MapToIso(int t_mapX, int t_mapY) const;

        /**
         * 2D/1D - mapping.
         *
         * @param t_mapX The map x position.
         * @param t_mapY The map y position.
         *
         * @return The map index.
         */
        [[nodiscard]] int GetMapIndex(const int t_mapX, const int t_mapY) const { return t_mapY * width + t_mapX; }

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /*
        std::vector<int> m_map
        {
            1165, 0, 0, 0,
               0, 4, 0, 0,
               0, 0, 0, 0,
               0, 0, 0, 0,
               0, 0, 0, 0,
               0, 0, 0, 0,
               0, 0, 0, 0,
               0, 0, 0, 0,
        };

        std::vector<int> m_map
        {
            1165, 1094, 1094, 1166,
            1093,    4, 0,    1095,
            1093,    0, 0,    1095,
            1093,    0, 0,    1095,
            1093,    4, 4,    1095,
            1093,    4, 4,    1095,
            1093,    4, 4,    1095,
            1164, 1092, 1092, 1167,
        };
        */

        //int m_width{ 16 };
        //int m_height{ 24 };
        std::vector<int> m_map
        {
            //(x)
            // 1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16  // (y)
            1165, 1094, 1094, 1094, 1094, 1094, 1094, 1094, 1094, 1094, 1094, 1094, 1094, 1094, 1094, 1166, //  1
            1093,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4, 1095, //  2
            1093,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4, 1095, //  3
            1093,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4, 1095, //  4
            1093,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4, 1095, //  5
            1093,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4, 1095, //  6
            1093,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4, 1095, //  7
            1093,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4, 1095, //  8
            1093,    4,    4,    4,    4, 3760, 3761,    4,    4,    4,    4,    4,    4,    4,    4, 1095, //  9
            1093,    4,    4,    4,    4, 3762, 3763,    4,    4,    4,    4, 3771, 3770,    4,    4, 1095, // 10
            1093,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4, 3769, 3768,    4,    4, 1095, // 11
            1093,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4, 1095, // 12
            1093,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4, 1095, // 13
            1093,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4, 1095, // 14
            1093,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4, 1095, // 15
            1093,    4,    4,    4,    4,    4,    4, 3766, 3764,    4,    4,    4,    4,    4,    4, 1095, // 16
            1093,    4,    4,    4,    4,    4,    4, 3767, 3765,    4,    4, 3773, 3775,    4,    4, 1095, // 17
            1093,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4, 3772, 3774,    4,    4, 1095, // 18
            1093,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4, 1095, // 19
            1093,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4, 1095, // 20
            1093,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4, 1095, // 21
            1093,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4, 1095, // 22
            1093,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4, 1095, // 23
            1164, 1092, 1092, 1092, 1092, 1092, 1092, 1092, 1092, 1092, 1092, 1092, 1092, 1092, 1092, 1167, // 24
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
