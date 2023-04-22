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

#include <AL/alext.h>
#include "Log.h"
#include "MusicBuffer.h"
#include "MdciiException.h"
#include "file/SoundFile.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::sound::MusicBuffer::MusicBuffer(std::shared_ptr<file::SoundFile> t_soundFile)
    : m_soundFile{ std::move(t_soundFile) }
{
    Log::MDCII_LOG_DEBUG("[MusicBuffer::MusicBuffer()] Create MusicBuffer.");

    Init();
}

mdcii::sound::MusicBuffer::~MusicBuffer() noexcept
{
    Log::MDCII_LOG_DEBUG("[MusicBuffer::~MusicBuffer()] Destruct MusicBuffer.");

    CleanUp();
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

bool mdcii::sound::MusicBuffer::IsPlaying() const
{
    ALint state;
    ALenum error;
    alGetError();
    alGetSourcei(m_source, AL_SOURCE_STATE, &state);
    if ((error = alGetError()) != AL_NO_ERROR)
    {
        throw MDCII_EXCEPTION("[MusicBuffer::IsPlaying()] An error has occurred. " + std::string(alGetString(error)));
    }

    return (state == AL_PLAYING);
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::sound::MusicBuffer::Play()
{
    ALsizei i;

    alGetError();

    // Rewind the source position and clear the buffer queue
    alSourceRewind(m_source);
    alSourcei(m_source, AL_BUFFER, 0);

    // Fill the buffer queue
    for (i = 0; i < NUM_BUFFERS; i++)
    {
        // Get some data to give it to the buffer
        sf_count_t slen = sf_readf_short(m_soundFile->sndFile, m_memBuf, BUFFER_SAMPLES);
        if (slen < 1) break;

        slen *= m_soundFile->sfInfo.channels * (sf_count_t)sizeof(short);
        alBufferData(m_buffers[i], m_soundFile->format, m_memBuf, (ALsizei)slen, m_soundFile->sfInfo.samplerate);
    }

    if (alGetError() != AL_NO_ERROR)
    {
        throw("Error buffering for playback");
    }

    // Now queue and start playback!
    alSourceQueueBuffers(m_source, i, m_buffers);
    alSourcePlay(m_source);
    if (alGetError() != AL_NO_ERROR)
    {
        throw("Error starting playback");
    }
}

void mdcii::sound::MusicBuffer::Pause() const
{
    alSourcePause(m_source);
}

void mdcii::sound::MusicBuffer::Stop() const
{
    alSourceStop(m_source);
}

void mdcii::sound::MusicBuffer::Resume() const
{
    alSourcePlay(m_source);
}

void mdcii::sound::MusicBuffer::UpdateBufferStream()
{
    ALint processed, state;

    alGetError();

    // Get relevant source info
    alGetSourcei(m_source, AL_SOURCE_STATE, &state);
    alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processed);
    if (alGetError() != AL_NO_ERROR)
    {
        throw("error checking music source state");
    }

    // Unqueue and handle each processed buffer
    while (processed > 0)
    {
        ALuint bufid;
        sf_count_t slen;

        alSourceUnqueueBuffers(m_source, 1, &bufid);
        processed--;

        /* Read the next chunk of data, refill the buffer, and queue it
         * back on the source */
        slen = sf_readf_short(m_soundFile->sndFile, m_memBuf, BUFFER_SAMPLES);
        if (slen > 0)
        {
            slen *= m_soundFile->sfInfo.channels * (sf_count_t)sizeof(short);
            alBufferData(bufid, m_soundFile->format, m_memBuf, (ALsizei)slen, m_soundFile->sfInfo.samplerate);
            alSourceQueueBuffers(m_source, 1, &bufid);
        }
        if (alGetError() != AL_NO_ERROR)
        {
            throw("error buffering music data");
        }
    }

    // Make sure the source hasn't underrun
    if (state != AL_PLAYING && state != AL_PAUSED)
    {
        ALint queued;

        // If no buffers are queued, playback is finished
        alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queued);
        if (queued == 0)
            return;

        alSourcePlay(m_source);
        if (alGetError() != AL_NO_ERROR)
        {
            throw("error restarting music playback");
        }
    }
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::sound::MusicBuffer::Init()
{
    Log::MDCII_LOG_DEBUG("[MusicBuffer::Init()] Initializing music buffer.");

    CreateAudioSource();
    CreateBuffer();

    const auto frameSize{ (static_cast<size_t>(BUFFER_SAMPLES) * static_cast<size_t>(m_soundFile->sfInfo.channels)) * sizeof(short) };
    m_memBuf = static_cast<short*>(malloc(frameSize));
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::sound::MusicBuffer::CreateAudioSource()
{
    Log::MDCII_LOG_DEBUG("[MusicBuffer::CreateAudioSource()] Create audio source.");

    ALenum error;
    alGetError();
    alGenSources(1, &m_source);
    if ((error = alGetError()) != AL_NO_ERROR)
    {
        throw MDCII_EXCEPTION("[MusicBuffer::CreateAudioSource()] Unable to create audio source. " + std::string(alGetString(error)));
    }
}

void mdcii::sound::MusicBuffer::CreateBuffer()
{
    Log::MDCII_LOG_DEBUG("[MusicBuffer::CreateBuffer()] Create music buffer.");

    ALenum error;
    alGetError();
    alGenBuffers(NUM_BUFFERS, m_buffers);
    if ((error = alGetError()) != AL_NO_ERROR)
    {
        throw MDCII_EXCEPTION("[MusicBuffer::CreateBuffer()] Unable to create buffer. " + std::string(alGetString(error)));
    }
}

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::sound::MusicBuffer::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[MusicBuffer::CleanUp()] CleanUp music buffer.");

    alDeleteSources(1, &m_source);

    free(m_memBuf);

    alDeleteBuffers(NUM_BUFFERS, m_buffers);
}
