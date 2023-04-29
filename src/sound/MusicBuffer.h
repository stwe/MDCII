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

#include <memory>
#include <array>
#include <vector>
#include <AL/al.h>

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::file
{
    /**
     * Forward declaration class SoundFile.
     */
    class SoundFile;
}

//-------------------------------------------------
// MusicBuffer
//-------------------------------------------------

namespace mdcii::sound
{
    /**
     * Streaming a soundtrack.
     */
    class MusicBuffer
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        MusicBuffer() = delete;

        /**
         * Constructs a new MusicBuffer object.
         *
         * @param t_soundFile A SoundFile object.
         */
        explicit MusicBuffer(std::shared_ptr<file::SoundFile> t_soundFile);

        MusicBuffer(const MusicBuffer& t_other) = delete;
        MusicBuffer(MusicBuffer&& t_other) noexcept = delete;
        MusicBuffer& operator=(const MusicBuffer& t_other) = delete;
        MusicBuffer& operator=(MusicBuffer&& t_other) noexcept = delete;

        ~MusicBuffer() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        [[nodiscard]] bool IsPlaying() const;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void Play();
        void Pause() const;
        void Stop() const;
        void Resume() const;

        void UpdateBufferStream();

    protected:

    private:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        static constexpr auto BUFFER_SAMPLES{ 8192 };
        static constexpr auto NUM_BUFFERS{ 4 };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * A SoundFile object holding audio data.
         */
        std::shared_ptr<file::SoundFile> m_soundFile;

        /**
         * The audio source object handle.
         */
        ALuint m_sourceId{ 0 };

        /**
         * Holding the raw audio stream.
         */
        std::array<ALuint, NUM_BUFFERS> m_buffersIds{ 0, 0, 0, 0 };

        /**
         * Chunk of audio data.
         */
        std::vector<short> m_chunkBuffer;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initializes the music buffer.
         */
        void Init();

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        void CreateAudioSource();
        void CreateBuffer();

        //-------------------------------------------------
        // Clean up
        //-------------------------------------------------

        /**
         * Clean up.
         */
        void CleanUp() const;
    };
}
