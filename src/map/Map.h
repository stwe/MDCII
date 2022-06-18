#pragma once

#include <memory>
#include <vector>
#include "MapTile.h"
#include "ogl/Window.h"
#include "camera/Camera.h"
#include "ecs/entt.hpp"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::data
{
    class Buildings;
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
     * Represents a map.
     */
    class Map
    {
    public:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * Something is invalid.
         */
        static constexpr auto INVALID{ -1 };

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
         * The MapTile objects.
         */
        std::vector<MapTile> mapTiles;

        /**
         * The width of the map in tiles.
         */
        int width{ 0 };

        /**
         * The height of the map in tiles.
         */
        int height{ 0 };

        /**
         * The currently selected gfx Id.
         */
        int selectedId{ INVALID };

        /**
         * The map rotation.
         */
        Rotation rotation{ Rotation::DEG0 };

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

        /**
         * Shows the isometric grid.
         */
        bool renderGrid{ true };

        /**
         * Shows all buildings.
         */
        bool renderBuildings{ false };

        /**
         * Shows the coordinates.
         */
        bool renderText{ true };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Map() = delete;

        /**
         * Constructs a new Map object.
         *
         * @param t_mapTiles The MapTile objects.
         * @param t_width The width of the map in tiles.
         * @param t_height The height of the map in tiles.
         * @param t_buildings The Buildings object.
         */
        Map(
            std::vector<MapTile> t_mapTiles,
            int t_width, int t_height,
            std::shared_ptr<data::Buildings> t_buildings
        );

        Map(const Map& t_other) = delete;
        Map(Map&& t_other) noexcept = delete;
        Map& operator=(const Map& t_other) = delete;
        Map& operator=(Map&& t_other) noexcept = delete;

        ~Map() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        /**
         * Renders the map.
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
        // Utils
        //-------------------------------------------------

        /**
         * Rotates a map position.
         *
         * @param t_mapX The x position of the map to rotate.
         * @param t_mapY The y position of the map to rotate.
         * @param t_rotation The rotation.
         *
         * @return The rotated map position.
         */
        [[nodiscard]] glm::ivec2 RotateMapPosition(int t_mapX, int t_mapY, Rotation t_rotation = Rotation::DEG0) const;

        /**
         * Projects map coordinates in isometric view.
         *
         * @param t_mapX The map x position.
         * @param t_mapY The map y position.
         * @param t_rotation The map rotation.
         *
         * @return The isometric coordinates.
         */
        [[nodiscard]] glm::vec2 MapToIso(int t_mapX, int t_mapY, Rotation t_rotation = Rotation::DEG0) const;

        /**
         * 2D/1D - mapping.
         *
         * @param t_mapX The map x position.
         * @param t_mapY The map y position.
         * @param t_rotation The map rotation.
         *
         * @return The map index.
         */
        [[nodiscard]] int GetMapIndex(int t_mapX, int t_mapY, Rotation t_rotation = Rotation::DEG0) const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The content from the haeuser.cod.
         */
        std::shared_ptr<data::Buildings> m_buildings;

        /**
         * Pallet values from the from the stadtfld.col file.
         */
        std::unique_ptr<file::PaletteFile> m_paletteFile;

        /**
         * The ECS.
         */
        entt::registry m_registry;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Create objects.
         */
        void Init();

        //-------------------------------------------------
        // Entities
        //-------------------------------------------------

        /**
         * Create entities for an isometric grid.
         */
        void CreateGridEntities();

        /**
         * Create all entities needed to represent the island.
         */
        void CreateBuildingEntities();

        /**
         * Renders the grid.
         */
        void RenderGridEntities(const ogl::Window& t_window, const camera::Camera& t_camera) const;

        /**
         * Renders all buildings.
         */
        void RenderBuildingEntities(const ogl::Window& t_window, const camera::Camera& t_camera) const;

        void SortEntities();
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
