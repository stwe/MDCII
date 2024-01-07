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

#include <memory>
#include <vector>
#include "world/Zoom.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace olc
{
    /**
     * @brief Forward declaration class Renderable.
     */
    class Renderable;
}

namespace mdcii::resource
{
    //-------------------------------------------------
    // AnimalsTileAtlas
    //-------------------------------------------------

    /**
     * @brief Provides a Tile Atlas for all the animal graphics for each zoom.
     */
    class AnimalsTileAtlas
    {
    public:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * @brief Width and height of the largest image.
         */
        static constexpr std::array<std::pair<world::Zoom, std::pair<float, float>>, world::NR_OF_ZOOMS> LARGEST_SIZE{
            { std::make_pair<world::Zoom, std::pair<float, float>>(world::Zoom::SGFX, std::make_pair<float, float>(17.0f, 16.0f)),
              std::make_pair<world::Zoom, std::pair<float, float>>(world::Zoom::MGFX, std::make_pair<float, float>(35.0f, 32.0f)),
              std::make_pair<world::Zoom, std::pair<float, float>>(world::Zoom::GFX, std::make_pair<float, float>(71.0f, 65.0f)) }
        };

        /**
         * @brief Number of rows in each Animals Tile Atlas.
         */
        static constexpr auto NR_OF_ROWS{ 16 };

        /**
         * @brief Number of SGFX Animals Tile Atlas images.
         */
        static constexpr auto NR_OF_SGFX_ATLAS_IMAGES{ 3 };

        /**
         * @brief Number of MGFX Animals Tile Atlas images.
         */
        static constexpr auto NR_OF_MGFX_ATLAS_IMAGES{ 3 };

        /**
         * @brief Number of GFX Animals Tile Atlas images.
         */
        static constexpr auto NR_OF_GFX_ATLAS_IMAGES{ 3 };

        /**
         * @brief The name of the Animals Tile Atlas.
         */
        static constexpr std::string_view TILE_ATLAS_NAME{ "animals" };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
          * @brief Contains the AnimalsTileAtlas images for each zoom as a renderable object.
          */
        std::array<std::vector<std::unique_ptr<const olc::Renderable>>, world::NR_OF_ZOOMS> atlas;

        /**
         * @brief Array of vectors containing the real heights of each image.
         */
        std::array<std::vector<int>, world::NR_OF_ZOOMS> heights;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        AnimalsTileAtlas();

        AnimalsTileAtlas(const AnimalsTileAtlas& t_other) = delete;
        AnimalsTileAtlas(AnimalsTileAtlas&& t_other) noexcept = delete;
        AnimalsTileAtlas& operator=(const AnimalsTileAtlas& t_other) = delete;
        AnimalsTileAtlas& operator=(AnimalsTileAtlas&& t_other) noexcept = delete;

        ~AnimalsTileAtlas() noexcept;

    protected:

    private:
        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * @brief Initialize the AnimalsTileAtlas.
         */
        void Init();

        //-------------------------------------------------
        // Load
        //-------------------------------------------------

        /**
         * @brief Load AnimalsTileAtlas images.
         */
        void LoadAtlasImages();

        /**
         * @brief Method to load all AnimalsTileAtlas images heights by given Zoom.
         *
         * @param t_zoom The zoom.
         */
        void LoadHeightsByZoom(world::Zoom t_zoom);
    };
}
