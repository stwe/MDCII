#pragma once

#include <memory>
#include <array>
#include <string>

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::ogl
{
    class Window;
}

//-------------------------------------------------
// State
//-------------------------------------------------

namespace mdcii::state
{
    /**
     * Forward declaration class StateStack.
     */
    class StateStack;

    /**
     * The base class for all states.
     */
    class State
    {
    public:
        //-------------------------------------------------
        // Types
        //-------------------------------------------------

        /**
         * The unique identifiers of the states.
         */
        enum class Id
        {
            MAIN_MENU,  // MainMenuState
            START,      // StartState
            WORLD,      // WorldState
            EDITOR,     // EditorState
            ALL         // is used when all states are meant
        };

        /**
         * The unique identifiers of the states as string.
         */
        static constexpr std::array<std::string_view, 5> STATE_IDS
        {
            "MAIN_MENU", "START", "WORLD", "EDITOR", "ALL"
        };

        /**
         * Shared objects between all states.
         */
        struct Context
        {
            explicit Context(std::shared_ptr<ogl::Window> t_window)
                : window{ std::move(t_window) }
            {}

            std::shared_ptr<ogl::Window> window;
        };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * Works as a holder of shared objects between all states.
         */
        std::shared_ptr<Context> context;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        State() = delete;

        /**
         * Constructs a new State object.
         *
         * @param t_id The unique identifier.
         * @param t_stateStack The StateStack object.
         * @param t_context The holder of shared objects.
         */
        State(Id t_id, StateStack* t_stateStack, std::shared_ptr<Context> t_context);

        State(const State& t_other) = delete;
        State(State&& t_other) noexcept = delete;
        State& operator=(const State& t_other) = delete;
        State& operator=(State&& t_other) noexcept = delete;

        virtual ~State() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        [[nodiscard]] Id GetId() const { return m_id; }

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        virtual void Input() = 0;
        virtual void Update() = 0;
        virtual void PreRender() {}
        virtual void Render() = 0;
        virtual void RenderImGui() = 0;

        //-------------------------------------------------
        // Frame
        //-------------------------------------------------

        virtual void StartFrame();
        virtual void EndFrame() const;

    protected:
        //-------------------------------------------------
        // Stack operations
        //-------------------------------------------------

        void RequestStackPush(Id t_id) const;
        void RequestStackPop() const;
        void RequestStateClear() const;

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The unique identifier of this state.
         */
        Id m_id;

        /**
         * Pointer to the parent state stack.
         */
        StateStack* m_stateStack{ nullptr };
    };
}
