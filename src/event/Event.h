#pragma once

#include <string>

namespace mdcii::event
{
    //-------------------------------------------------
    // Types
    //-------------------------------------------------

    /**
     * Represents the BauGfx after a selection from
     * the menu or map.
     */
    struct SelectedBauGfx
    {
        /**
         * The bauGfx Id.
         */
        int32_t gfxId{ -1 };

        /**
         * The bauGfx orientation.
         */
        int32_t orientation{ 0 };

        /**
         * The translated label.
         */
        std::string name;

        /**
         * Checks whether a valid Id is present.
         *
         * @return True if a valid Id is present.
         */
        [[nodiscard]] bool IsValid() const { return gfxId >= 0; }
    };

    enum class MdciiEventType
    {
        // default
        NONE,

        // keyboard events
        KEY_PRESSED, KEY_RELEASED,

        // mouse events
        MOUSE_BUTTON_PRESSED, MOUSE_BUTTON_RELEASED, MOUSE_MOVED, MOUSE_SCROLLED, MOUSE_ENTER,

        // menu events
        BAUGFX_SELECTED,
    };

    //-------------------------------------------------
    // Event base class
    //-------------------------------------------------

    struct MdciiEvent
    {
        MdciiEvent() = default;
        virtual ~MdciiEvent() = default;

        MdciiEventType type{ MdciiEventType::NONE };
    };

    //-------------------------------------------------
    // Keyboard events
    //-------------------------------------------------

    struct KeyPressedEvent : MdciiEvent
    {
        int key;
        int repeatCount;

        explicit KeyPressedEvent(const int t_key, const int t_repeatCount = 0)
            : key{ t_key }
            , repeatCount{ t_repeatCount }
        {
            type = MdciiEventType::KEY_PRESSED;
        }
    };

    struct KeyReleasedEvent : MdciiEvent
    {
        int key;

        explicit KeyReleasedEvent(const int t_key)
            : key{ t_key }
        {
            type = MdciiEventType::KEY_RELEASED;
        }
    };

    //-------------------------------------------------
    // Mouse events
    //-------------------------------------------------

    struct MouseButtonPressedEvent : MdciiEvent
    {
        int button;

        explicit MouseButtonPressedEvent(const int t_button)
            : button{ t_button }
        {
            type = MdciiEventType::MOUSE_BUTTON_PRESSED;
        }
    };

    struct MouseButtonReleasedEvent : MdciiEvent
    {
        int button;

        explicit MouseButtonReleasedEvent(const int t_button)
            : button{ t_button }
        {
            type = MdciiEventType::MOUSE_BUTTON_RELEASED;
        }
    };

    struct MouseMovedEvent : MdciiEvent
    {
        float x;
        float y;

        MouseMovedEvent(const float t_x, const float t_y)
            : x{ t_x }
            , y{ t_y }
        {
            type = MdciiEventType::MOUSE_MOVED;
        }
    };

    struct MouseScrolledEvent : MdciiEvent
    {
        float xOffset;
        float yOffset;

        MouseScrolledEvent(const float t_xOffset, const float t_yOffset)
            : xOffset{ t_xOffset }
            , yOffset{ t_yOffset }
        {
            type = MdciiEventType::MOUSE_SCROLLED;
        }
    };

    struct MouseEnterEvent : MdciiEvent
    {
        bool enter;

        explicit MouseEnterEvent(const bool t_enter)
            : enter{ t_enter }
        {
            type = MdciiEventType::MOUSE_ENTER;
        }
    };

    //-------------------------------------------------
    // Menu events
    //-------------------------------------------------

    struct BauGfxSelectedEvent : MdciiEvent
    {
        SelectedBauGfx selectedBauGfx;

        explicit BauGfxSelectedEvent(SelectedBauGfx t_selectedBauGfx)
            : selectedBauGfx{ std::move(t_selectedBauGfx) }
        {
            type = MdciiEventType::BAUGFX_SELECTED;
        }
    };
}
