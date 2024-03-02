// This file is part of the MDCII project.
//
// Copyright (c) 2024. stwe <https://github.com/stwe/MDCII>
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#pragma once

#include "MdciiAssert.h"
#include "Game.h"
#include "world/tile/TerrainTile.h"
#include "world/tile/FigureTile.h"
#include "world/Rotation.h"
#include "world/World.h"
#include "state/State.h"
#include "resource/OriginalResourcesManager.h"
#include "camera/Camera.h"

namespace mdcii::world::layer
{
    //-------------------------------------------------
    // Forward declarations
    //-------------------------------------------------

    /**
     * @brief Forward declaration class TerrainLayer.
     */
    template <typename T>
    class TerrainLayer;

    /**
     * @brief Forward declaration class FiguresLayer.
     */
    template <typename T>
    class FiguresLayer;

    //-------------------------------------------------
    // LayerType
    //-------------------------------------------------

    /**
     * @brief Representing different types of layers.
     */
    enum class LayerType
    {
        COAST, TERRAIN, BUILDINGS, FIGURES, MIXED, DEEP_WATER, NONE
    };

    //-------------------------------------------------
    // Layer
    //-------------------------------------------------

    /**
     * @brief Base class of all layers used in the game.
     *
     * @tparam T The tile type.
     */
    template <typename T>
    class Layer
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief The type the layer.
         */
        LayerType layerType{ LayerType::NONE };

        /**
         * @brief The width of the layer.
         */
        const int width;

        /**
         * @brief The height of the layer.
         */
        const int height;

        /**
         * @brief Contains all tiles of the layer, sorted as in rotation DEG0.
         */
        std::vector<T> tiles;

        /**
         * @brief Contains the tiles for each rotation in the correct order for rendering.
         */
        std::array<std::vector<T>, magic_enum::enum_count<Rotation>()> sortedTiles;

        /**
         * @brief To get the correct position of a tile in `sortedTiles` from rotation and render index.
         */
        std::array<std::map<int, int>, magic_enum::enum_count<Rotation>()> sortedIndices;

        /**
         * @brief The tiles that are currently being rendered.
         */
        std::vector<T> currentTiles;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Layer() = delete;

        /**
         * @brief Constructs a new Layer object.
         *
         * @param t_world Pointer to the parent World object.
         * @param t_layerType The type of the layer.
         * @param t_width The width of the layer.
         * @param t_height The height of the layer.
         */
        Layer(const World* t_world, const LayerType t_layerType, const int t_width, const int t_height)
            : layerType{ t_layerType }
            , width{ t_width }
            , height{ t_height }
            , m_world{ t_world }
        {
            MDCII_ASSERT(width > 0, "[Layer::Layer()] Invalid width given.")
            MDCII_ASSERT(height > 0, "[Layer::Layer()] Invalid height given.")
            MDCII_ASSERT(m_world, "[Layer::Layer()] Null pointer.")
        }

        Layer(const Layer& t_other) = delete;
        Layer(Layer&& t_other) noexcept = delete;
        Layer& operator=(const Layer& t_other) = delete;
        Layer& operator=(Layer&& t_other) noexcept = delete;

        virtual ~Layer() noexcept = default;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        [[nodiscard]] int GetSortedIndex(const int t_x, const int t_y, const Rotation t_rotation)
        {
            return sortedIndices.at(magic_enum::enum_integer(t_rotation)).at(tile::Tile::GetRenderIndex(t_x, t_y, width, height, t_rotation));
        }

        /**
         * @brief Retrieves a reference to a tile at the specified position and rotation from `sortedTiles`.
         *
         * @param t_x The x position on the layer.
         * @param t_y The y position on the layer.
         * @param t_rotation The rotation for the search.
         *
         * @return A reference to the tile.
         */
        [[nodiscard]] T& GetSortedTile(const int t_x, const int t_y, const Rotation t_rotation)
        {
            return sortedTiles.at(magic_enum::enum_integer(t_rotation)).at(GetSortedIndex(t_x, t_y, t_rotation));
        }

        //-------------------------------------------------
        // Interface
        //-------------------------------------------------

        /**
         * @brief Creates a new tile from a Json value.
         *
         * @param t_json The Json value.
         *
         * @return The newly created Tile of type T.
         */
        virtual T CreateTileFromJson(const nlohmann::json& t_json) = 0;

        /**
         * @brief Initializes a tile.
         *
         * @param t_x The x position of the tile.
         * @param t_y The y position of the tile.
         */
        virtual void InitTile(int t_x, int t_y) = 0;

        /**
         * Does stuff before the tile is add to the layer.
         *
         * @param t_tile The tile to add.
         */
        virtual void PreTileAdd(T& t_tile) {};

        /**
         * Does stuff after the tile was added to the layer.
         *
         * @param t_tile The tile that was added.
         */
        virtual void PostTileAdd(T& t_tile) {};

        //-------------------------------------------------
        // Create and init tiles from Json
        //-------------------------------------------------

        /**
         * @brief Creates new tiles from a Json value.
         *
         * @param t_json The Json value.
         */
        void CreateTiles(const nlohmann::json& t_json)
        {
            MDCII_LOG_DEBUG("[Layer::CreateTiles()] Start creating tiles for layer of type {} ...", magic_enum::enum_name(layerType));

            for (const auto& [k, tileJson] : t_json.items())
            {
                tiles.emplace_back(CreateTileFromJson(tileJson));
            }

            MDCII_ASSERT(tiles.size() == width * static_cast<size_t>(height), "[Layer::CreateTiles()] Invalid number of tiles.")

            MDCII_LOG_DEBUG("[Layer::CreateTiles()] The tiles were created successfully.");
        }

        /**
         * @brief Initializes all tiles.
         */
        void InitTiles()
        {
            MDCII_LOG_DEBUG("[Layer::InitTiles()] Start initializing tiles for layer of type {} ...", magic_enum::enum_name(layerType));

            MDCII_ASSERT(!tiles.empty(), "[Layer::InitTiles()] Invalid number of tiles.")

            for (auto h{ 0 }; h < height; ++h)
            {
                for (auto w{ 0 }; w < width; ++w)
                {
                    InitTile(w, h);
                }
            }

            AddTileNeighbors();
            SortTilesForRendering();

            MDCII_LOG_DEBUG("[Layer::InitTiles()] The tiles were initialized successfully.");
        };

        //-------------------------------------------------
        // Sort tiles
        //-------------------------------------------------

        /**
         * @brief Sorts the tiles for correct rendering order.
         *
         * The sorted tiles are stored in `sortedTiles`.
         */
        void SortTilesForRendering()
        {
            MDCII_LOG_DEBUG("[Layer::SortTilesForRendering()] Start sorting tiles for rendering ...");

            for (const auto rotation : magic_enum::enum_values<Rotation>())
            {
                const auto rotationInt{ magic_enum::enum_integer(rotation) };

                MDCII_LOG_DEBUG("[Layer::SortTilesForRendering()] Sort for rotation: {}", magic_enum::enum_name(rotation));

                // sort tiles by index
                std::ranges::sort(tiles, [&](const T& t_a, const T& t_b)
                {
                    return t_a.renderIndices[rotationInt] < t_b.renderIndices[rotationInt];
                });

                // copy sorted tiles
                sortedTiles.at(rotationInt) = tiles;

                // the render-index can be negative; saves where the new index (i) can be found in sortedTiles
                auto i{ 0 };
                for (const auto& t : sortedTiles.at(rotationInt))
                {
                    sortedIndices.at(rotationInt).emplace(t.renderIndices.at(rotationInt), i);
                    i++;
                }
            }

            // revert tiles sorting = sortedTiles DEG0
            tiles = sortedTiles.at(magic_enum::enum_integer(Rotation::DEG0));

            MDCII_LOG_DEBUG("[Layer::SortTilesForRendering()] The tiles were sorted successfully.");
        }

        //-------------------------------------------------
        // Neighbors
        //-------------------------------------------------

        /**
         * @brief Stores a pointer to all neighbors for each tile.
         */
        void AddTileNeighbors()
        {
            MDCII_LOG_DEBUG("[Layer::AddTileNeighbors()] Save the neighbors for each tile ...");

            for (auto y{ 0 }; y < height; ++y)
            {
                for (auto x{ 0 }; x < width; ++x)
                {
                    const auto i{ y * width + x };

                    // regular grid
                    if (y > 0)
                    {
                        tiles.at(i).n = &tiles.at((y - 1) * width + x);
                        tiles.at(i).neighbors.push_back(tiles.at(i).n);
                    }

                    if (y < height - 1)
                    {
                        tiles.at(i).s = &tiles.at((y + 1) * width + x);
                        tiles.at(i).neighbors.push_back(tiles.at(i).s);
                    }

                    if (x > 0)
                    {
                        tiles.at(i).w = &tiles.at(y * width + (x - 1));
                        tiles.at(i).neighbors.push_back(tiles.at(i).w);
                    }

                    if (x < width - 1)
                    {
                        tiles.at(i).e = &tiles.at(y * width + (x + 1));
                        tiles.at(i).neighbors.push_back(tiles.at(i).e);
                    }

                    // connect diagonally
                    if (y > 0 && x < width - 1)
                    {
                        tiles.at(i).ne = &tiles[(y - 1) * width + (x + 1)];
                        tiles.at(i).neighbors.push_back(tiles.at(i).ne);
                    }

                    if (y > 0 && x > 0)
                    {
                        tiles.at(i).nw = &tiles[(y - 1) * width + (x - 1)];
                        tiles.at(i).neighbors.push_back(tiles.at(i).nw);
                    }

                    if (y < height - 1 && x > 0)
                    {
                        tiles.at(i).sw = &tiles[(y + 1) * width + (x - 1)];
                        tiles.at(i).neighbors.push_back(tiles.at(i).sw);
                    }

                    if (y < height - 1 && x < width - 1)
                    {
                        tiles.at(i).se = &tiles[(y + 1) * width + (x + 1)];
                        tiles.at(i).neighbors.push_back(tiles.at(i).se);
                    }
                }
            }

            MDCII_LOG_DEBUG("[Layer::AddTileNeighbors()] The neighbors have been saved successfully.");
        }

        //-------------------------------------------------
        // Current tiles to render
        //-------------------------------------------------

        /**
         * @brief Set `currentTiles` with the visible tiles to render.
         *
         * @param t_xOffset The x offset to check if the position is off screen.
         * @param t_yOffset The y offset to check if the position is off screen.
         *
         * @return True if visible tiles were found; false otherwise.
         */
        bool UpdateCurrentTiles(const int t_xOffset, const int t_yOffset)
        {
            std::vector<T>().swap(currentTiles);
            currentTiles = sortedTiles.at(magic_enum::enum_integer(m_world->camera->rotation));

            auto [begin, end]{ std::ranges::remove_if(currentTiles, [&](const T& t_tile) {
                return m_world->IsWorldPositionOutsideScreen(t_tile.posX + t_xOffset, t_tile.posY + t_yOffset) || t_tile.IsNotRenderable();
            } )};

            currentTiles.erase(begin, end);

            MDCII_LOG_DEBUG("[Layer::UpdateCurrentTiles()] Render {} current tiles for layer {}.",
                currentTiles.size(),
                magic_enum::enum_name(layerType)
            );

            return !currentTiles.empty();
        }

        //-------------------------------------------------
        // Add tiles
        //-------------------------------------------------

        /**
         * @brief Adds a tile to the layer.
         *
         * @param t_tile The tile to be added.
         */
        void AddTile(T& t_tile)
        {
            t_tile.CalcRenderPositions(width, height);

            PreTileAdd(t_tile);

            ReplaceTileInTilesArray(t_tile);
            IntoSortedTilesArray(t_tile);

            PostTileAdd(t_tile);
        }

        /**
         * @brief Adds tiles to the layer.
         *
         * @param t_tiles The tiles to be added.
         */
        void AddTiles(std::vector<T>& t_tiles)
        {
            MDCII_ASSERT(!t_tiles.empty(), "[Layer::AddTiles()] Invalid number of tiles.")

            MDCII_LOG_DEBUG("[Layer::AddTiles()] Add {} tiles to the {} layer.", t_tiles.size(), magic_enum::enum_name(layerType));

            for (auto& tile : t_tiles)
            {
                AddTile(tile);
            }
        }

    protected:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief Pointer to the parent World object.
         */
        const World* m_world{ nullptr };

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * @brief Replaces a tile in the `tiles` array with the content of a given tile.
         *
         * @param t_tile The tile with the new content.
         */
        void ReplaceTileInTilesArray(T& t_tile)
        {
            tiles.at(t_tile.renderIndices[0]) = t_tile;
        }

        /**
         * @brief Copies a tile from `tiles`array into the `sortedTiles` array.
         *
         * @param t_tile The tile to copy.
         */
        void IntoSortedTilesArray(T& t_tile)
        {
            using enum Rotation;

            auto idx0{ sortedIndices.at(magic_enum::enum_integer(DEG0)).at(t_tile.renderIndices[0]) };
            auto idx90{ sortedIndices.at(magic_enum::enum_integer(DEG90)).at(t_tile.renderIndices[1]) };
            auto idx180{ sortedIndices.at(magic_enum::enum_integer(DEG180)).at(t_tile.renderIndices[2]) };
            auto idx270{ sortedIndices.at(magic_enum::enum_integer(DEG270)).at(t_tile.renderIndices[3]) };

            sortedTiles.at(magic_enum::enum_integer(DEG0)).at(idx0) = t_tile;
            sortedTiles.at(magic_enum::enum_integer(DEG90)).at(idx90) = t_tile;
            sortedTiles.at(magic_enum::enum_integer(DEG180)).at(idx180) = t_tile;
            sortedTiles.at(magic_enum::enum_integer(DEG270)).at(idx270) = t_tile;
        }

    private:

    };

    //-------------------------------------------------
    // TerrainLayer
    //-------------------------------------------------

    /**
     * @brief A class for all terrain layers used in the game.
     */
    template <>
    class TerrainLayer<tile::TerrainTile> : public Layer<tile::TerrainTile>
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        TerrainLayer() = delete;

        /**
         * @brief Constructs a new TerrainLayer object.
         *
         * @param t_world Pointer to the parent World object.
         * @param t_layerType The type of the layer.
         * @param t_width The width of the layer.
         * @param t_height The height of the layer.
         */
        TerrainLayer(const World* t_world, LayerType t_layerType, const int t_width, const int t_height)
            : Layer(t_world, t_layerType, t_width, t_height)
        {
            MDCII_LOG_DEBUG("[TerrainLayer::TerrainLayer()] Create TerrainLayer of type {}.", magic_enum::enum_name(layerType));
        }

        TerrainLayer(const TerrainLayer& t_other) = delete;
        TerrainLayer(TerrainLayer&& t_other) noexcept = delete;
        TerrainLayer& operator=(const TerrainLayer& t_other) = delete;
        TerrainLayer& operator=(TerrainLayer&& t_other) noexcept = delete;

        ~TerrainLayer() noexcept override
        {
            MDCII_LOG_DEBUG("[TerrainLayer::~TerrainLayer()] Destruct TerrainLayer.");
        }

        //-------------------------------------------------
        // Interface
        //-------------------------------------------------

        /**
         * @brief Creates a new terrain tile from a Json value.
         *
         * @param t_json The Json value.
         *
         * @return The newly created Tile of type TerrainTile.
         */
        [[nodiscard]] tile::TerrainTile CreateTileFromJson(const nlohmann::json& t_json) override
        {
            tile::TerrainTile tile;

            if (t_json.count("id") && t_json.at("id") >= 0)
            {
                if (m_world)
                {
                    const auto& building{ m_world->state->game->originalResourcesManager->GetBuildingById(t_json.at("id")) };
                    tile.building = &building;
                }
                else
                {
                    throw MDCII_EXCEPTION("[TerrainLayer::CreateTileFromJson()] Null pointer.");
                }
            }

            if (t_json.count("rotation"))
            {
                tile.rotation = t_json.at("rotation");
                if (tile.HasBuilding() && tile.building->IsRotatable())
                {
                    if (tile.rotation < 0 || tile.rotation > 3)
                    {
                        throw MDCII_EXCEPTION("[TerrainLayer::CreateTileFromJson()] Invalid building rotation.");
                    }
                }
            }

            if (t_json.count("x"))
            {
                tile.x = t_json.at("x");
            }

            if (t_json.count("y"))
            {
                tile.y = t_json.at("y");
            }

            return tile;
        }

        /**
         * @brief Initializes a terrain tile.
         *
         * @param t_x The x position of the tile.
         * @param t_y The y position of the tile.
         */
        void InitTile(const int t_x, const int t_y) override
        {
            auto& tile{ tiles.at(t_y * width + t_x) };
            tile.posX = t_x;
            tile.posY = t_y;

            if (tile.HasBuilding())
            {
                tile.CalculateGfxValues();
            }

            tile.CalcRenderPositions(width, height);
        }

        /**
         * Does stuff before the tile is add to the layer.
         *
         * @param t_tile The tile to add.
         */
        void PreTileAdd(tile::TerrainTile& t_tile) override
        {
            if (t_tile.type == tile::Tile::TileType::TRAFFIC)
            {
                t_tile.neighbors = tiles.at(t_tile.renderIndices[0]).neighbors;

                t_tile.n = tiles.at(t_tile.renderIndices[0]).n;
                t_tile.s = tiles.at(t_tile.renderIndices[0]).s;
                t_tile.e = tiles.at(t_tile.renderIndices[0]).e;
                t_tile.w = tiles.at(t_tile.renderIndices[0]).w;

                t_tile.nw = tiles.at(t_tile.renderIndices[0]).nw;
                t_tile.ne = tiles.at(t_tile.renderIndices[0]).ne;
                t_tile.sw = tiles.at(t_tile.renderIndices[0]).sw;
                t_tile.se = tiles.at(t_tile.renderIndices[0]).se;

                t_tile.DetermineTrafficGfx();
            }
        }

        /**
         * Does stuff after the tile was added to the layer.
         *
         * @param t_tile The tile that was added.
         */
        void PostTileAdd(tile::TerrainTile& t_tile) override
        {
            if (t_tile.type == tile::Tile::TileType::TRAFFIC)
            {
                HandleTrafficNeighbors(t_tile);
            }
        }

    protected:

    private:
        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Fixes the Gfx of all neighbors of a traffic tile.
         *
         * @param t_tile The tile from which the neighbors are to be corrected.
         */
        void HandleTrafficNeighbors(const tile::TerrainTile& t_tile)
        {
            for (auto* neighborTile : t_tile.neighbors)
            {
                if (neighborTile)
                {
                    auto* neighborTerrainTile{ dynamic_cast<tile::TerrainTile*>(neighborTile) };
                    if (neighborTerrainTile->HasBuilding() && neighborTerrainTile->type == tile::Tile::TileType::TRAFFIC)
                    {
                        if (neighborTerrainTile->DetermineTrafficGfx())
                        {
                            ReplaceTileInTilesArray(*neighborTerrainTile);
                            IntoSortedTilesArray(*neighborTerrainTile);
                        }
                    }
                }
            }
        }
    };

    //-------------------------------------------------
    // FiguresLayer
    //-------------------------------------------------

    /**
     * @brief A class for all figures layers used in the game.
     */
    template <>
    class FiguresLayer<tile::FigureTile> : public Layer<tile::FigureTile>
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        FiguresLayer() = delete;

        /**
         * @brief Constructs a new FiguresLayer object.
         *
         * @param t_world Pointer to the parent World object.
         * @param t_width The width of the layer.
         * @param t_height The height of the layer.
         */
        FiguresLayer(const World* t_world, const int t_width, const int t_height)
            : Layer(t_world, LayerType::FIGURES, t_width, t_height)
        {
            MDCII_LOG_DEBUG("[FiguresLayer::FiguresLayer()] Create FiguresLayer.");
        }

        FiguresLayer(const FiguresLayer& t_other) = delete;
        FiguresLayer(FiguresLayer&& t_other) noexcept = delete;
        FiguresLayer& operator=(const FiguresLayer& t_other) = delete;
        FiguresLayer& operator=(FiguresLayer&& t_other) noexcept = delete;

        ~FiguresLayer() noexcept override
        {
            MDCII_LOG_DEBUG("[FiguresLayer::~FiguresLayer()] Destruct FiguresLayer.");
        }

        //-------------------------------------------------
        // Interface
        //-------------------------------------------------

        /**
         * @brief Creates a new figure tile from a Json value.
         *
         * @param t_json The Json value.
         *
         * @return The newly created Tile of type FigureTile.
         */
        [[nodiscard]] tile::FigureTile CreateTileFromJson(const nlohmann::json& t_json) override
        {
            tile::FigureTile tile;

            if (t_json.count("id") && t_json.at("id") > 0) // 0 = UNSET
            {
                const auto figureIdOpt{ magic_enum::enum_cast<resource::FigureId>(t_json.at("id")) };
                if (m_world && figureIdOpt.has_value())
                {
                    const auto& figure{ m_world->state->game->originalResourcesManager->GetFigureById(figureIdOpt.value()) };
                    tile.figure = &figure;
                }
                else
                {
                    throw MDCII_EXCEPTION("[FiguresLayer::CreateTileFromJson()] Null pointer or invalid figure Id.");
                }
            }

            if (t_json.count("rotation"))
            {
                tile.rotation = t_json.at("rotation");
                if (tile.HasFigure() && tile.figure->IsRotatable())
                {
                    if (tile.rotation < 0 || tile.rotation >= tile.figure->rotate)
                    {
                        throw MDCII_EXCEPTION("[FiguresLayer::CreateTileFromJson()] Invalid figure rotation.");
                    }
                }
            }

            if (t_json.count("animation"))
            {
                tile.currentAnimation = t_json.at("animation");
                if (tile.HasFigure() && tile.figure->IsRotatable())
                {
                    if (tile.currentAnimation < 0 || static_cast<size_t>(tile.currentAnimation) > tile.figure->animations.size() - 1)
                    {
                        throw MDCII_EXCEPTION("[FiguresLayer::CreateTileFromJson()] Invalid figure animation.");
                    }
                }
            }

            return tile;
        }

        /**
         * @brief Initializes a figure tile.
         *
         * @param t_x The x position of the tile.
         * @param t_y The y position of the tile.
         */
        void InitTile(const int t_x, const int t_y) override
        {
            auto& tile{ tiles.at(t_y * width + t_x) };
            tile.posX = t_x;
            tile.posY = t_y;

            tile.CalcRenderPositions(width, height);
        }

        //-------------------------------------------------
        // Count
        //-------------------------------------------------

        /**
         * @brief Counts the number of figures of a given FigureId.
         *
         * @return The number of figure tiles.
         */
        [[nodiscard]] int CountFigures(const resource::FigureId t_figureId)
        {
            const auto count{ std::count_if(tiles.begin(), tiles.end(), [t_figureId](const tile::FigureTile& t_tile) {
                return t_tile.HasFigure(t_figureId);
            }) };

            return static_cast<int>(count);
        }

    protected:

    private:

    };
}
