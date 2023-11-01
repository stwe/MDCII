// This file is part of the MDCII project.
//
// Copyright (c) 2023. stwe <https://github.com/stwe/MDCII>
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

#include "AssetManager.h"
#include "MdciiAssert.h"
#include "MdciiUtils.h"
#include "MdciiException.h"
#include "Game.h"
#include "world/Zoom.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::resource::AssetManager::AssetManager()
{
    MDCII_LOG_DEBUG("[AssetManager::AssetManager()] Create AssetManager.");

    Init();
}

mdcii::resource::AssetManager::~AssetManager() noexcept
{
    MDCII_LOG_DEBUG("[AssetManager::~AssetManager()] Destruct AssetManager.");
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

const olc::Renderable* mdcii::resource::AssetManager::GetAsset(const Asset t_asset, const world::Zoom t_zoom)
{
    return m_assetSprites.at(t_asset).at(magic_enum::enum_integer(t_zoom)).get();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::resource::AssetManager::Init()
{
    MDCII_LOG_DEBUG("[AssetManager::Init()] Start load assets from {}textures ...", Game::RESOURCES_REL_PATH);

    for (const auto asset : magic_enum::enum_values<Asset>())
    {
        MDCII_LOG_DEBUG("[AssetManager::Init()] Load Asset {}.", magic_enum::enum_name(asset));

        const auto& fileName{ ASSET_FILE_NAMES.at(magic_enum::enum_integer(asset)) };

        std::array<std::unique_ptr<const olc::Renderable>, 3> sprites;
        for (const auto zoom : magic_enum::enum_values<world::Zoom>())
        {
            const auto zoomInt{ magic_enum::enum_integer(zoom) };
            const auto zoomStr{ to_lower_case(std::string(magic_enum::enum_name(zoom))) };

            auto sprite{ std::make_unique<olc::Renderable>() };
            if (sprite->Load(fmt::format("{}textures/{}/{}_{}.png", Game::RESOURCES_REL_PATH, zoomStr, fileName, zoomStr)) != olc::OK)
            {
                throw MDCII_EXCEPTION("[AssetManager::Init()] Error while loading file.");
            }

            MDCII_ASSERT(sprite->Sprite(), "[AssetManager::Init()] Null pointer.")
            MDCII_ASSERT(sprite->Decal(), "[AssetManager::Init()] Null pointer.")

            sprites.at(zoomInt) = std::move(sprite);
        }

        m_assetSprites.try_emplace(asset, std::move(sprites));
    }

    MDCII_LOG_DEBUG("[AssetManager::Init()] All assets were load successfully.");
}
