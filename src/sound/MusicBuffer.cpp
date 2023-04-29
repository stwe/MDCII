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
#include "MusicBuffer.h"
#include "MdciiAssert.h"
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

    alGetError();
    alGetSourcei(m_sourceId, AL_SOURCE_STATE, &state);
    if (const auto error{ alGetError() }; error != AL_NO_ERROR)
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
    alGetError();

    // Rewind the source position and clear the buffer queue.
    alSourceRewind(m_sourceId);
    alSourcei(m_sourceId, AL_BUFFER, 0);

    // Fill the buffer queue.
    for (auto i{ 0 }; i < NUM_BUFFERS; ++i)
    {
        // Get some data to give it to the buffer.
        auto nrFrames{ sf_readf_short(m_soundFile->sndFile, m_chunkBuffer.data(), BUFFER_SAMPLES) };
        if (nrFrames < 1)
        {
            break;
        }

        nrFrames *= m_soundFile->sfInfo.channels * static_cast<sf_count_t>(sizeof(short));
        alBufferData(m_buffersIds[i], m_soundFile->format, m_chunkBuffer.data(), static_cast<ALsizei>(nrFrames), m_soundFile->sfInfo.samplerate);
    }

    if (const auto error{ alGetError() }; error != AL_NO_ERROR)
    {
        throw MDCII_EXCEPTION("[MusicBuffer::Play()] Error buffering for playback. " + std::string(alGetString(error)));
    }

    // Now queue and start playback.
    alSourceQueueBuffers(m_sourceId, NUM_BUFFERS, m_buffersIds.data());
    alSourcePlay(m_sourceId);
    if (const auto error{ alGetError() }; error != AL_NO_ERROR)
    {
        throw MDCII_EXCEPTION("[MusicBuffer::Play()] Error starting playback. " + std::string(alGetString(error)));
    }
}

void mdcii::sound::MusicBuffer::Pause() const
{
    alSourcePause(m_sourceId);
}

void mdcii::sound::MusicBuffer::Stop() const
{
    alSourceStop(m_sourceId);
}

void mdcii::sound::MusicBuffer::Resume() const
{
    alSourcePlay(m_sourceId);
}

void mdcii::sound::MusicBuffer::UpdateBufferStream()
{
    alGetError();

    // Get relevant source info
    ALint processed;
    ALint state;
    alGetSourcei(m_sourceId, AL_SOURCE_STATE, &state);
    alGetSourcei(m_sourceId, AL_BUFFERS_PROCESSED, &processed);
    if (const auto error{ alGetError() }; error != AL_NO_ERROR)
    {
        throw MDCII_EXCEPTION("[MusicBuffer::UpdateBufferStream()] Error checking music source state. " + std::string(alGetString(error)));
    }

    // Unqueue and handle each processed buffer.
    while (processed > 0)
    {
        ALuint bufId;
        alSourceUnqueueBuffers(m_sourceId, 1, &bufId);
        processed--;

        // Read the next chunk of data, refill the buffer, and queue it back on the source.
        if (auto nrFrames{ sf_readf_short(m_soundFile->sndFile, m_chunkBuffer.data(), BUFFER_SAMPLES) }; nrFrames > 0)
        {
            nrFrames *= m_soundFile->sfInfo.channels * static_cast<sf_count_t>(sizeof(short));
            alBufferData(bufId, m_soundFile->format, m_chunkBuffer.data(), static_cast<ALsizei>(nrFrames), m_soundFile->sfInfo.samplerate);
            alSourceQueueBuffers(m_sourceId, 1, &bufId);
        }

        if (const auto error{ alGetError() }; error != AL_NO_ERROR)
        {
            throw MDCII_EXCEPTION("[MusicBuffer::UpdateBufferStream()] Error buffering music data. " + std::string(alGetString(error)));
        }
    }

    // Make sure the source hasn't underrun.
    if (state != AL_PLAYING && state != AL_PAUSED)
    {
        // If no buffers are queued, playback is finished.
        ALint queued;
        alGetSourcei(m_sourceId, AL_BUFFERS_QUEUED, &queued);
        if (queued == 0)
        {
            return;
        }

        alSourcePlay(m_sourceId);
        if (const auto error{ alGetError() }; error != AL_NO_ERROR)
        {
            throw MDCII_EXCEPTION("[MusicBuffer::UpdateBufferStream()] Error restarting music playback. " + std::string(alGetString(error)));
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

    const auto frameSize{ static_cast<size_t>(BUFFER_SAMPLES) * static_cast<size_t>(m_soundFile->sfInfo.channels) };
    m_chunkBuffer.resize(frameSize, 0);
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::sound::MusicBuffer::CreateAudioSource()
{
    Log::MDCII_LOG_DEBUG("[MusicBuffer::CreateAudioSource()] Create audio source.");

    alGetError();
    alGenSources(1, &m_sourceId);
    MDCII_ASSERT(m_sourceId, "[MusicBuffer::CreateAudioSource()] Error while creating a new sound source handle.")

    if (const auto error{ alGetError() }; error != AL_NO_ERROR)
    {
        throw MDCII_EXCEPTION("[MusicBuffer::CreateAudioSource()] Unable to create audio source. " + std::string(alGetString(error)));
    }
}

void mdcii::sound::MusicBuffer::CreateBuffer()
{
    Log::MDCII_LOG_DEBUG("[MusicBuffer::CreateBuffer()] Create music buffer.");

    alGetError();
    alGenBuffers(NUM_BUFFERS, m_buffersIds.data());
    for (const auto id : m_buffersIds)
    {
        MDCII_ASSERT(id, "[MusicBuffer::CreateBuffer()] Error while creating a new sound buffer handle.")
    }

    if (const auto error{ alGetError() }; error != AL_NO_ERROR)
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

    alDeleteSources(1, &m_sourceId);
    alDeleteBuffers(NUM_BUFFERS, m_buffersIds.data());
}
