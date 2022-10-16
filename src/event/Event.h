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

#pragma once

#include <string>
#include "world/Rotation.h"

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
         * The building Id.
         */
        int32_t buildingId{ -1 };

        /**
         * The bauGfx rotation.
         */
        world::Rotation rotation{ world::Rotation::DEG0 };

        /**
         * The translated label.
         */
        std::string name;

        /**
         * For better readability/convenience.
         *
         * @return True if a valid building Id is present.
         */
        [[nodiscard]] bool HasBuilding() const { return buildingId >= 0; }
    };

    enum class MdciiEventType
    {
        // default
        NONE,

        // keyboard events
        KEY_PRESSED,
        KEY_RELEASED,

        // mouse events
        MOUSE_BUTTON_PRESSED,
        MOUSE_BUTTON_RELEASED,
        MOUSE_MOVED,
        MOUSE_SCROLLED,
        MOUSE_ENTER,

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

    struct KeyPressedEvent: MdciiEvent
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

    struct KeyReleasedEvent: MdciiEvent
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

    struct MouseButtonPressedEvent: MdciiEvent
    {
        int button;

        explicit MouseButtonPressedEvent(const int t_button)
            : button{ t_button }
        {
            type = MdciiEventType::MOUSE_BUTTON_PRESSED;
        }
    };

    struct MouseButtonReleasedEvent: MdciiEvent
    {
        int button;

        explicit MouseButtonReleasedEvent(const int t_button)
            : button{ t_button }
        {
            type = MdciiEventType::MOUSE_BUTTON_RELEASED;
        }
    };

    struct MouseMovedEvent: MdciiEvent
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

    struct MouseScrolledEvent: MdciiEvent
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

    struct MouseEnterEvent: MdciiEvent
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

    struct BauGfxSelectedEvent: MdciiEvent
    {
        SelectedBauGfx selectedBauGfx;

        explicit BauGfxSelectedEvent(SelectedBauGfx t_selectedBauGfx)
            : selectedBauGfx{ std::move(t_selectedBauGfx) }
        {
            type = MdciiEventType::BAUGFX_SELECTED;
        }
    };
}
