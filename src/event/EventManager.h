#pragma once

#include "Event.h"
#include "eventpp/eventdispatcher.h"
#include "ogl/OpenGL.h"

//-------------------------------------------------
// EventManager
//-------------------------------------------------

namespace mdcii::event
{
    /**
     * Static event handling.
     */
    class EventManager
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        inline static eventpp::EventDispatcher<MdciiEventType, void(const MdciiEvent&)> eventDispatcher;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        EventManager(const EventManager& t_other) = delete;
        EventManager(EventManager&& t_other) noexcept = delete;
        EventManager& operator=(const EventManager& t_other) = delete;
        EventManager& operator=(EventManager&& t_other) noexcept = delete;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        static void SetKeyboardGlfwCallbacks(GLFWwindow* t_windowHandle);
        static void SetMouseGlfwCallbacks(GLFWwindow* t_windowHandle);

    protected:

    private:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        EventManager() = default;
        ~EventManager() noexcept = default;
    };
}
