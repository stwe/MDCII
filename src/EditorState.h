#pragma once

#include "state/State.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::data
{
    class Text;
}

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
         * Language value from the config.ini.
         */
        std::string m_lang;

        /**
         * Texts in different languages.
         */
        std::unique_ptr<data::Text> m_text;

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

        //-------------------------------------------------
        // ImGui
        //-------------------------------------------------

        /**
         * Creates an menu to edit the map content.
         */
        void EditMenu() const;
    };
}
