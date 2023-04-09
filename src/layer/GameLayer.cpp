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
#include "state/State.h"
#include "ogl/buffer/Ssbo.h"
#include "world/World.h"
#include "vendor/eventpp/utilities/argumentadapter.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::layer::GameLayer::GameLayer(std::shared_ptr<state::Context> t_context, world::World* t_world)
    : m_context{ std::move(t_context) }
    , m_world{ t_world }
{
    Log::MDCII_LOG_DEBUG("[GameLayer::GameLayer()] Create GameLayer.");

    MDCII_ASSERT(m_context, "[GameLayer::GameLayer()] Null pointer.")
    MDCII_ASSERT(m_world, "[GameLayer::GameLayer()] Null pointer.")

    AddListeners();
}

mdcii::layer::GameLayer::~GameLayer() noexcept
{
    Log::MDCII_LOG_DEBUG("[GameLayer::~GameLayer()] Destruct GameLayer.");

    CleanUp();
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

const mdcii::layer::GameLayer::Model_Matrices_For_Each_Rotation& mdcii::layer::GameLayer::GetModelMatrices(const world::Zoom t_zoom) const
{
    MDCII_ASSERT(!modelMatrices.at(0).at(0).empty(), "[GameLayer::GetModelMatrices()] Invalid model matrices container.")

    return modelMatrices.at(magic_enum::enum_integer(t_zoom));
}

//-------------------------------------------------
// Map index
//-------------------------------------------------

int32_t mdcii::layer::GameLayer::GetMapIndex(const int32_t t_x, const int32_t t_y, const world::Rotation t_rotation) const
{
    const auto position{ rotate_position(t_x, t_y, width, height, t_rotation) };

    if (t_rotation == world::Rotation::DEG0 || t_rotation == world::Rotation::DEG180)
    {
        return position.y * width + position.x;
    }

    return position.y * height + position.x;
}

int32_t mdcii::layer::GameLayer::GetMapIndex(const glm::ivec2& t_position, const world::Rotation t_rotation) const
{
    return GetMapIndex(t_position.x, t_position.y, t_rotation);
}

//-------------------------------------------------
// Prepare rendering
//-------------------------------------------------

void mdcii::layer::GameLayer::PrepareCpuDataForRendering()
{
    CreateTiles();
    SortTiles();

    CreateModelMatricesContainer();
    CreateSelectedContainer();
    CreateGfxNumbersContainer();
    CreateBuildingIdsContainer();
}

void mdcii::layer::GameLayer::PrepareGpuDataForRendering()
{
    StoreModelMatricesInGpu();
    StoreSelectedContainerInGpu();
    StoreGfxNumbersInGpu();
    StoreBuildingIdsInGpu();
}

//-------------------------------------------------
// Interface
//-------------------------------------------------

void mdcii::layer::GameLayer::CreateSelectedContainer()
{
    Log::MDCII_LOG_DEBUG("[GameLayer::CreateSelectedContainer()] Create selected instances container.");

    MDCII_ASSERT(selectedInstances.empty(), "[GameLayer::CreateSelectedContainer()] Invalid selected instances container size.")
    MDCII_ASSERT(instancesToRender > 0, "[GameLayer::CreateSelectedContainer()] Invalid number of instances.")

    selectedInstances.resize(instancesToRender, glm::ivec4(0));
}

void mdcii::layer::GameLayer::StoreModelMatricesInGpu()
{
    Log::MDCII_LOG_DEBUG("[GameLayer::StoreModelMatricesInGpu()] Store model matrices container in Gpu memory.");

    MDCII_ASSERT(!modelMatrices.at(0).at(0).empty(), "[GameLayer::StoreModelMatricesInGpu()] Invalid model matrices container size.")
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

void mdcii::layer::GameLayer::StoreSelectedContainerInGpu()
{
    Log::MDCII_LOG_DEBUG("[GameLayer::StoreSelectedContainerInGpu()] Store selected instances container in Gpu memory.");

    MDCII_ASSERT(!selectedInstances.empty(), "[GameLayer::StoreSelectedContainerInGpu()] Invalid selected instances container size.")
    MDCII_ASSERT(!selectedInstancesSsbo, "[GameLayer::StoreSelectedContainerInGpu()] Invalid selected instances Ssbo pointer.")

    selectedInstancesSsbo = std::make_unique<ogl::buffer::Ssbo>("SelectedInstances_Ssbo");
    selectedInstancesSsbo->Bind();
    ogl::buffer::Ssbo::StoreData(static_cast<uint32_t>(selectedInstances.size()) * sizeof(glm::ivec4), selectedInstances.data());
    ogl::buffer::Ssbo::Unbind();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::layer::GameLayer::AddListeners()
{
    Log::MDCII_LOG_DEBUG("[GameLayer::AddListeners()] Add event listeners.");

    // OnLeftMouseButtonPressed
    m_mouseButtonPressed = event::EventManager::event_dispatcher.appendListener(
        event::MdciiEventType::MOUSE_BUTTON_PRESSED,
        eventpp::argumentAdapter<void(const event::MouseButtonPressedEvent&)>(
            [this](const event::MouseButtonPressedEvent& t_event) {
                if (t_event.button == 0)
                {
                    OnLeftMouseButtonPressed();
                }
            }
        )
    );
}

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::layer::GameLayer::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[GameLayer::CleanUp()] CleanUp GameLayer.");

    event::EventManager::event_dispatcher.removeListener(event::MdciiEventType::MOUSE_BUTTON_PRESSED, m_mouseButtonPressed);
}
