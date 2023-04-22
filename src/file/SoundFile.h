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
#include <sndfile.h>
#include <al.h>

//-------------------------------------------------
// SoundFile
//-------------------------------------------------

namespace mdcii::file
{
    /**
     * Represents a wav/mp3 file.
     */
    class SoundFile
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The path to the file to load.
         */
        std::string filePath;

        /**
         * Pointer to the loaded file content.
         */
        SNDFILE* sndFile{ nullptr };

        /**
         * Information about the file.
         */
        SF_INFO sfInfo{};

        /**
         * Format of the audio data.
         */
        ALenum format{ AL_NONE };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        SoundFile() = delete;

        /**
         * Constructs a new SoundFile object from a given path.
         *
         * @param t_filePath The path to the file to load.
         */
        explicit SoundFile(std::string t_filePath);

        SoundFile(const SoundFile& t_other) = delete;
        SoundFile(SoundFile&& t_other) noexcept = delete;
        SoundFile& operator=(const SoundFile& t_other) = delete;
        SoundFile& operator=(SoundFile&& t_other) noexcept = delete;

        ~SoundFile() noexcept;

    protected:

    private:
        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Open a wav/mp3 file.
         */
        void OpenFile();

        /**
         * Get audio data format of the wav/mp3 file.
         */
        void ReadAudioDataFormat();

        //-------------------------------------------------
        // Clean up
        //-------------------------------------------------

        /**
         * Clean up.
         */
        void CleanUp() const;
    };
}
