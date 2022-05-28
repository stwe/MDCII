#pragma once

#include <map>
#include "state/State.h"

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

namespace mdcii::camera
{
    class Camera;
}

namespace mdcii::renderer
{
    class TileRenderer;
}

//-------------------------------------------------
// EditorState
//-------------------------------------------------

namespace mdcii
{
    /**
     * Represents a map editor.
     */
    class EditorState : public state::State
    {
    public:

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        EditorState() = delete;

        /**
         * Constructs a new EditorState object.
         *
         * @param t_id The unique identifier.
         * @param t_stateStack The parent StateStack object.
         * @param t_context The holder of shared objects.
         */
        EditorState(Id t_id, state::StateStack* t_stateStack, std::shared_ptr<Context> t_context);

        EditorState(const EditorState& t_other) = delete;
        EditorState(EditorState&& t_other) noexcept = delete;
        EditorState& operator=(const EditorState& t_other) = delete;
        EditorState& operator=(EditorState&& t_other) noexcept = delete;

        ~EditorState() noexcept override;

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
        // Member
        //-------------------------------------------------

        std::unique_ptr<file::PaletteFile> m_paletteFile;
        std::unique_ptr<file::BshFile> m_stdBshFile;
        std::unique_ptr<file::BshFile> m_bauhausBshFile;
        std::unique_ptr<camera::Camera> m_camera;
        std::unique_ptr<renderer::TileRenderer> m_renderer;
        std::unique_ptr<data::HousesJsonFile> m_housesJsonFile;
        std::map<int, std::string> m_graphicsFileContent;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void Init();

        //-------------------------------------------------
        // ImGui
        //-------------------------------------------------

        void TileMenuById() const;
        void TileMenuByGroup() const;
    };
}
