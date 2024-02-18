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
#include <array>
#include <unordered_map>
#include "vendor/enum/magic_enum.hpp"

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

namespace mdcii::world
{
    /**
     * @brief Forward declaration enum class Zoom.
     */
    enum class Zoom;
}

namespace mdcii::resource
{
    //-------------------------------------------------
    // Asset
    //-------------------------------------------------

    /**
     * @brief Used to identify the different types of assets.
     */
    enum class Asset
    {
        CHEAT, RECT, GREEN_ISO, PURPLE_ISO, BLUE_ISO
    };

    /**
     * @brief The total number of unique assets.
     */
    static constexpr auto NR_OF_ASSETS{ magic_enum::enum_count<Asset>() };

    /**
     * @brief The file names of the assets.
     */
    static constexpr std::array<const char*, NR_OF_ASSETS> ASSET_FILE_NAMES
    {
        "corner", "frame", "green", "mouse", "blue"
    };

    //-------------------------------------------------
    // AssetManager
    //-------------------------------------------------

    /**
     * @brief Managing own assets, including loading them from files.
     */
    class AssetManager
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        AssetManager();

        AssetManager(const AssetManager& t_other) = delete;
        AssetManager(AssetManager&& t_other) noexcept = delete;
        AssetManager& operator=(const AssetManager& t_other) = delete;
        AssetManager& operator=(AssetManager&& t_other) noexcept = delete;

        ~AssetManager() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * @brief Retrieve the specified asset at a specific zoom level.
         * @param t_asset Type of the asset to retrieve.
         * @param t_zoom Zoom level of the asset.
         *
         * @return A pointer to the requested asset, or nullptr if the asset does not exist.
         */
        const olc::Renderable* GetAsset(Asset t_asset, world::Zoom t_zoom);

    protected:

    private:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * @brief The total number of zoom levels that each asset can have.
         *
         * For better readability.
         */
        static constexpr auto NR_OF_ZOOMS{ 3 };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief Map that holds arrays of unique pointers to each asset for each zoom level.
         */
        std::unordered_map<Asset, std::array<std::unique_ptr<const olc::Renderable>, NR_OF_ZOOMS>> m_assetSprites;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * @brief Initializes the AssetManager and loads all assets.
         */
        void Init();
    };
}
