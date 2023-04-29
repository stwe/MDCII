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

#include "SoundBuffer.h"
#include "MdciiAssert.h"
#include "MdciiException.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::sound::SoundBuffer::SoundBuffer()
{
    Log::MDCII_LOG_DEBUG("[SoundBuffer::SoundBuffer()] Create SoundBuffer.");
}

mdcii::sound::SoundBuffer::~SoundBuffer() noexcept
{
    Log::MDCII_LOG_DEBUG("[SoundBuffer::~SoundBuffer()] Destruct SoundBuffer.");

    CleanUp();
}

//-------------------------------------------------
// Add
//-------------------------------------------------

ALuint mdcii::sound::SoundBuffer::AddSoundEffect(const file::SoundFile& t_soundFile)
{
    Log::MDCII_LOG_DEBUG("[SoundBuffer::AddSoundEffect()] Creates and adds a sound buffer for file {}.", t_soundFile.filePath);

    std::vector<short> buffer(t_soundFile.sfInfo.frames * t_soundFile.sfInfo.channels, 0);
    if (const auto numFrames{ sf_readf_short(t_soundFile.sndFile, buffer.data(), t_soundFile.sfInfo.frames) }; numFrames < 1)
    {
        throw MDCII_EXCEPTION("[SoundBuffer::AddSoundEffect()] Failed to read sound data.");
    }

    ALuint bufferId;
    alGenBuffers(1, &bufferId);
    MDCII_ASSERT(bufferId, "[SoundBuffer::AddSoundEffect()] Error while creating a new sound buffer handle.")

    alBufferData(bufferId, t_soundFile.format, buffer.data(), static_cast<ALsizei>(buffer.size() * sizeof(short)), t_soundFile.sfInfo.samplerate);

    m_soundBuffers.push_back(bufferId);

    return bufferId;
}

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::sound::SoundBuffer::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[SoundBuffer::CleanUp()] CleanUp sound buffer.");

    if (!m_soundBuffers.empty())
    {
        alDeleteBuffers(static_cast<ALsizei>(m_soundBuffers.size()), m_soundBuffers.data());
    }
}
