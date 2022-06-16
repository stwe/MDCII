#pragma once

#include "state/State.h"
#include "entt/entt.hpp"
#include "map/Map.h"
#include "data/Buildings.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

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

namespace mdcii
{
    //-------------------------------------------------
    // Types
    //-------------------------------------------------

    struct TileAsset
    {
        int buildingId{ -1 };
        int direction{ 0 };
    };

    //-------------------------------------------------
    // Components
    //-------------------------------------------------

    struct BuildingComponent
    {
        TileAsset tileAsset;
        data::Building building;
    };

    struct PositionComponent
    {
        int mapX{ 0 };
        int mapY{ 0 };

        std::vector<int> indices;
        std::vector<glm::vec2> screenPositions;
        std::vector<int> gfx;
    };

    //-------------------------------------------------
    // SandboxState
    //-------------------------------------------------

    /**
     * State to try out an Entity-Component-System (EnTT ECS).
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

        TileAsset m_c0{ 1051, 0 };
        TileAsset m_c1{ 1051, 1 };
        TileAsset m_c2{ 1051, 2 };
        TileAsset m_c3{ 1051, 3 };

        TileAsset m_s0{ 1011, 0 };
        TileAsset m_s1{ 1011, 1 };
        TileAsset m_s2{ 1011, 2 };
        TileAsset m_s3{ 1011, 3 };

        TileAsset m_b{ 101, 0 };
        TileAsset m_f{ 1075, 0 };

        const std::vector<TileAsset> m_map
        {
            m_c1, m_s2, m_s2, m_c2,
            m_s1,  m_b,  m_b, m_s3,
            m_s1,  m_b,  m_b, m_s3,
            m_s1,  m_b,  m_b, m_s3,
            m_s1,  m_b,  m_b, m_s3,
            m_s1,  m_b,  m_b, m_s3,
            m_s1,  m_f,  m_b, m_s3,
            m_c0, m_s0, m_s0, m_c3,
        };

        map::Rotation m_rotation{ map::Rotation::DEG0 };

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
