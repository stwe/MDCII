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

#include <AL/al.h>
#include <AL/alc.h>
#include "SoundDevice.h"
#include "Log.h"
#include "MdciiException.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::sound::SoundDevice::SoundDevice()
{
    Log::MDCII_LOG_DEBUG("[SoundDevice::SoundDevice()] Create SoundDevice.");

    Init();
}

mdcii::sound::SoundDevice::~SoundDevice() noexcept
{
    Log::MDCII_LOG_DEBUG("[SoundDevice::~SoundDevice()] Destruct SoundDevice.");

    CleanUp();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::sound::SoundDevice::Init()
{
    Log::MDCII_LOG_DEBUG("[SoundDevice::Init()] Initializing sound device.");

    m_device = alcOpenDevice(nullptr);
    if (!m_device)
    {
        Log::MDCII_LOG_WARN("[SoundDevice::Init()] Something went wrong setting up the audio device. No sound output.");

        Log::MDCII_LOG_DEBUG("[SoundDevice::Init()] Error while initializing sound device.");
        return;
    }

    m_context = alcCreateContext(m_device, nullptr);
    if (!m_context)
    {
        throw MDCII_EXCEPTION("[SoundDevice::Init()] Failed to set sound context.");
    }

    if (!alcMakeContextCurrent(m_context))
    {
        throw MDCII_EXCEPTION("[SoundDevice::Init()] Failed to make context current.");
    }

    const ALCchar* deviceName{ nullptr };
    if (alcIsExtensionPresent(m_device, "ALC_ENUMERATE_ALL_EXT"))
    {
        deviceName = alcGetString(m_device, ALC_ALL_DEVICES_SPECIFIER);
    }

    if (!deviceName || alcGetError(m_device) != AL_NO_ERROR)
    {
        deviceName = alcGetString(m_device, ALC_DEVICE_SPECIFIER);
    }

    Log::MDCII_LOG_INFO("Sound device: {}", deviceName);
    Log::MDCII_LOG_INFO("OpenAL vendor string: {}", alGetString(AL_VENDOR));
    Log::MDCII_LOG_INFO("OpenAL renderer string: {}", alGetString(AL_RENDERER));
    Log::MDCII_LOG_INFO("OpenAL version string: {}", alGetString(AL_VERSION));

    Log::MDCII_LOG_DEBUG("[SoundDevice::Init()] The sound device was successfully initialized.");
}

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::sound::SoundDevice::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[SoundDevice::CleanUp()] CleanUp sound device and context.");

    alcMakeContextCurrent(nullptr);
    alcDestroyContext(m_context);
    alcCloseDevice(m_device);
}
