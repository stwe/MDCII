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

#include "SoundSource.h"
#include "MdciiAssert.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::sound::SoundSource::SoundSource(const ALint t_bufferId)
{
    Log::MDCII_LOG_DEBUG("[SoundSource::SoundSource()] Create SoundSource.");

    MDCII_ASSERT(t_bufferId, "[SoundSource::SoundSource()] Invalid buffer handle.")

    Init(t_bufferId);
}

mdcii::sound::SoundSource::~SoundSource() noexcept
{
    Log::MDCII_LOG_DEBUG("[SoundSource::~SoundSource()] Destruct SoundSource.");

    CleanUp();
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::sound::SoundSource::Play() const
{
    alSourcePlay(m_sourceId);
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::sound::SoundSource::Init(const ALint t_bufferId)
{
    Log::MDCII_LOG_DEBUG("[SoundSource::Init()] Initializing sound source for buffer handle {}.", t_bufferId);

    alGenSources(1, &m_sourceId);
    MDCII_ASSERT(m_sourceId, "[SoundSource::Init()] Error while creating a new Sound Source handle.")

    Log::MDCII_LOG_DEBUG("[SoundSource::Init()] A new Sound Source handle was created. The Id is {}.", m_sourceId);

    alSourcef(m_sourceId, AL_PITCH, m_pitch);
    alSourcef(m_sourceId, AL_GAIN, m_gain);
    alSource3f(m_sourceId, AL_POSITION, m_position.x, m_position.y, m_position.z);
    alSource3f(m_sourceId, AL_VELOCITY, m_velocity.x, m_velocity.y, m_velocity.z);
    alSourcei(m_sourceId, AL_LOOPING, m_loopSound);
    alSourcei(m_sourceId, AL_BUFFER, t_bufferId);
}

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::sound::SoundSource::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[SoundSource::CleanUp()] CleanUp sound source.");

    if (m_sourceId)
    {
        alDeleteSources(1, &m_sourceId);
    }
}
