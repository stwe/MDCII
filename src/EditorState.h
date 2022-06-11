#pragma once

#include <map>
#include "state/State.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::map
{
    class Map;
    class MousePicker;
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
         * @param t_id The unique identifier of the State.
         * @param t_stateStack A pointer to the parent StateStack object.
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
        void PreRender() override;
        void Render() override;
        void RenderImGui() override;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * Text identifiers for gfx Ids.
         */
        std::map<int, std::string> m_graphicsFileContent;

        /**
         * The Map object to edit.
         */
        std::shared_ptr<map::Map> m_map;

        /**
         * A MousePicker object to select tiles.
         */
        std::unique_ptr<map::MousePicker> m_mousePicker;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initializes the state.
         */
        void Init();

        /**
         * Creates a houses.json from the haeuser.cod.
         */
        void CreateHousesJsonFile() const;

        //-------------------------------------------------
        // ImGui
        //-------------------------------------------------

        /**
         * An ImGui tree view with a node for each Gfx Id.
         */
        void TileMenuById() const;

        /**
         * An ImGui tree view. The entries are grouped by TileKind.
         */
        void TileMenuByGroup() const;
    };
}
