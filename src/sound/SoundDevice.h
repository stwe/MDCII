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

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

/**
 * Forward declaration struct ALCdevice.
 */
struct ALCdevice;

/**
 * Forward declaration struct ALCcontext.
 */
struct ALCcontext;

//-------------------------------------------------
// SoundDevice
//-------------------------------------------------

namespace mdcii::sound
{
    /**
     * Open a handle to the default sound device.
     * After the device is successfully opened, create and activate a context.
     */
    class SoundDevice
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        SoundDevice();

        SoundDevice(const SoundDevice& t_other) = delete;
        SoundDevice(SoundDevice&& t_other) noexcept = delete;
        SoundDevice& operator=(const SoundDevice& t_other) = delete;
        SoundDevice& operator=(SoundDevice&& t_other) noexcept = delete;

        ~SoundDevice() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * For better readability. Checks if the sound device is available.
         *
         * @return True if the default sound device is available.
         */
        [[nodiscard]] bool IsSoundDeviceAvailable() const { return m_device != nullptr; }

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The default sound device handle.
         */
        ALCdevice* m_device{ nullptr };

        /**
         * The current context.
         */
        ALCcontext* m_context{ nullptr };

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initializes the default sound device and create a context.
         */
        void Init();

        //-------------------------------------------------
        // Clean up
        //-------------------------------------------------

        /**
         * Clean up.
         */
        void CleanUp() const;
    };
}
