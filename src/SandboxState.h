#pragma once

#include "state/State.h"
#include "entt/entt.hpp"
#include "map/Map.h"

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
// SandboxState
//-------------------------------------------------

namespace mdcii
{
    //-------------------------------------------------
    // Components
    //-------------------------------------------------

    struct PositionComponent
    {
        int x;
        int y;

        int idx0;
        int idx90;
        int idx180;
        int idx270;

        glm::vec2 s0;
        glm::vec2 s90;
        glm::vec2 s180;
        glm::vec2 s270;

        bool higher;
    };

    struct GfxComponent
    {
        int gfx0;
        int gfx90;
        int gfx180;
        int gfx270;
    };

    /**
     * State to try out an Entity Component System.
     */
    class SandboxState : public state::State
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        SandboxState() = delete;

        SandboxState(Id t_id, state::StateStack* t_stateStack, std::shared_ptr<Context> t_context);

        SandboxState(const SandboxState& t_other) = delete;
        SandboxState(SandboxState&& t_other) noexcept = delete;
        SandboxState& operator=(const SandboxState& t_other) = delete;
        SandboxState& operator=(SandboxState&& t_other) noexcept = delete;

        ~SandboxState() noexcept override;

        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        void Input() override;
        void Update() override;
        void PreRender() override {}
        void Render() override;
        void RenderImGui() override;

    protected:

    private:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        static constexpr auto TILE_WIDTH{ 64 };
        static constexpr auto TILE_HEIGHT{ 32 };

        static constexpr auto TILE_WIDTH_HALF{ 32 };
        static constexpr auto TILE_HEIGHT_HALF{ 16 };

        static constexpr auto WIDTH{ 4 };
        static constexpr auto HEIGHT{ 8 };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        entt::registry m_registry;

        const std::vector<int> m_map
        {
            1165, 1094, 1094, 1166,
            1093,    4, 0,    1095,
            1093,    0, 0,    1095,
            1093,    0, 0,    1095,
            1093,    0, 0,    1095,
            1093,    0, 0,    1095,
            1093, 1320, 0,    1095,
            1164, 1092, 1092, 1167,
        };

        map::Rotation m_rotation{map::Rotation::DEG0 };

        std::unique_ptr<data::HousesJsonFile> m_housesJsonFile;
        std::unique_ptr<file::PaletteFile> m_paletteFile;
        std::unique_ptr<file::BshFile> m_stdBshFile;
        std::unique_ptr<renderer::TileRenderer> m_renderer;
        std::unique_ptr<renderer::TextRenderer> m_textRenderer;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void Init();

        void CreateMapEntities();
        void SortEntities();
    };
}
