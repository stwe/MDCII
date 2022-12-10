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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#include "GameLayer.h"
#include "MdciiAssert.h"
#include "Game.h"
#include "state/State.h"
#include "ogl/buffer/Ssbo.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::layer::GameLayer::GameLayer(std::shared_ptr<state::Context> t_context)
    : m_context{ std::move(t_context) }
{
    Log::MDCII_LOG_DEBUG("[GameLayer::GameLayer()] Create GameLayer.");

    MDCII_ASSERT(m_context, "[GameLayer::GameLayer()] Null pointer.")

    worldWidth = Game::INI.Get<int32_t>("game", "world_width");
    worldHeight = Game::INI.Get<int32_t>("game", "world_height");

    MDCII_ASSERT(worldWidth > 0, "[GameLayer::GameLayer()] Invalid width.")
    MDCII_ASSERT(worldHeight > 0, "[GameLayer::GameLayer()] Invalid height.")
}

mdcii::layer::GameLayer::~GameLayer() noexcept
{
    Log::MDCII_LOG_DEBUG("[GameLayer::~GameLayer()] Destruct GameLayer.");
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

const mdcii::layer::GameLayer::Model_Matrices_For_Each_Rotation& mdcii::layer::GameLayer::GetModelMatrices(const world::Zoom t_zoom) const
{
    return modelMatrices.at(magic_enum::enum_integer(t_zoom));
}

//-------------------------------------------------
// Tiles
//-------------------------------------------------

bool mdcii::layer::GameLayer::IsPositionInLayer(const int32_t t_x, const int32_t t_y) const
{
    if (t_x >= 0 && t_x < width &&
        t_y >= 0 && t_y < height)
    {
        return true;
    }

    return false;
}

glm::ivec2 mdcii::layer::GameLayer::RotatePosition(const int32_t t_x, const int32_t t_y, const world::Rotation t_rotation) const
{
    return rotate_position(t_x, t_y, width, height, t_rotation);
}

int32_t mdcii::layer::GameLayer::GetMapIndex(const int32_t t_x, const int32_t t_y) const
{
    MDCII_ASSERT(IsPositionInLayer(t_x, t_y), "[GameLayer::GetMapIndex()] Invalid position given.")

    return t_y * width + t_x;
}

int32_t mdcii::layer::GameLayer::GetMapIndex(const int32_t t_x, const int32_t t_y, const world::Rotation t_rotation) const
{
    MDCII_ASSERT(IsPositionInLayer(t_x, t_y), "[GameLayer::GetMapIndex()] Invalid position given.")

    const auto position{ RotatePosition(t_x, t_y, t_rotation) };

    if (t_rotation == world::Rotation::DEG0 || t_rotation == world::Rotation::DEG180)
    {
        return position.y * width + position.x;
    }

    return position.y * height + position.x;
}

glm::vec2 mdcii::layer::GameLayer::WorldToScreen(const int32_t t_x, const int32_t t_y, const world::Zoom t_zoom, const world::Rotation t_rotation) const
{
    const auto position{ rotate_position(t_x, t_y, worldWidth, worldHeight, t_rotation) };
    return {
        (position.x - position.y) * get_tile_width_half(t_zoom),
        (position.x + position.y) * get_tile_height_half(t_zoom)
    };
}

//-------------------------------------------------
// Interface
//-------------------------------------------------

void mdcii::layer::GameLayer::PrepareGpuDataForRendering()
{
    StoreModelMatricesInGpu();
}

//-------------------------------------------------
// Gpu data
//-------------------------------------------------

void mdcii::layer::GameLayer::StoreModelMatricesInGpu()
{
    Log::MDCII_LOG_DEBUG("[GameLayer::StoreModelMatricesInGpu()] Store model matrices container in Ssbos.");

    MDCII_ASSERT(!modelMatrices.at(0).at(0).empty(), "[GameLayer::StoreModelMatricesInGpu()] Invalid model matrices container.")
    MDCII_ASSERT(!modelMatricesSsbos.at(0).at(0), "[GameLayer::StoreModelMatricesInGpu()] Invalid model matrices Ssbo.")

    magic_enum::enum_for_each<world::Zoom>([this](const world::Zoom t_zoom) {
        magic_enum::enum_for_each<world::Rotation>([this, &t_zoom](const world::Rotation t_rotation) {
            const auto zoomInt{ magic_enum::enum_integer(t_zoom) };
            const auto rotationInt{ magic_enum::enum_integer(t_rotation) };

            const auto& matrices{ GetModelMatrices(t_zoom).at(rotationInt) };

            auto ssbo{ std::make_unique<ogl::buffer::Ssbo>(
                std::string("ModelMatrices_Ssbo_") +
                magic_enum::enum_name(t_zoom).data() +
                "_" + magic_enum::enum_name(t_rotation).data()
            ) };
            ssbo->Bind();
            ogl::buffer::Ssbo::StoreData(static_cast<uint32_t>(matrices.size()) * sizeof(glm::mat4), matrices.data());
            ogl::buffer::Ssbo::Unbind();

            modelMatricesSsbos.at(zoomInt).at(rotationInt) = std::move(ssbo);
        });
    });
}
