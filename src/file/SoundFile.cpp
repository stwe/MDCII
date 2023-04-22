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

#include <alext.h>
#include <filesystem>
#include "SoundFile.h"
#include "MdciiAssert.h"
#include "MdciiException.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::file::SoundFile::SoundFile(std::string t_filePath)
    : filePath{ std::move(t_filePath) }
{
    Log::MDCII_LOG_DEBUG("[SoundFile::SoundFile()] Create SoundFile.");

    OpenFile();
    ReadAudioDataFormat();
}

mdcii::file::SoundFile::~SoundFile() noexcept
{
    Log::MDCII_LOG_DEBUG("[SoundFile::~SoundFile()] Destruct SoundFile.");

    CleanUp();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::file::SoundFile::OpenFile()
{
    Log::MDCII_LOG_DEBUG("[SoundFile::OpenFile()] Open file {}.", filePath);

    if (std::filesystem::path path = filePath; path.extension() != ".wav" && path.extension() != ".mp3")
    {
        throw MDCII_EXCEPTION("[SoundFile::OpenFile()] Invalid file given.");
    }

    sndFile = sf_open(filePath.c_str(), SFM_READ, &sfInfo);
    if (!sndFile)
    {
        throw MDCII_EXCEPTION("[SoundFile::OpenFile()] Unable to open provided file " + filePath + ".");
    }
}

void mdcii::file::SoundFile::ReadAudioDataFormat()
{
    Log::MDCII_LOG_DEBUG("[SoundFile::ReadAudioDataFormat()] Get audio data format for file {}.", filePath);

    switch (sfInfo.channels)
    {
    case 1:
        format = AL_FORMAT_MONO16;
        Log::MDCII_LOG_DEBUG("[SoundFile::ReadAudioDataFormat()] Found format AL_FORMAT_MONO16 in file {}.", filePath);
        break;
    case 2:
        format = AL_FORMAT_STEREO16;
        Log::MDCII_LOG_DEBUG("[SoundFile::ReadAudioDataFormat()] Found format AL_FORMAT_STEREO16 in file {}.", filePath);
        break;
    case 3:
        if (sf_command(sndFile, SFC_WAVEX_GET_AMBISONIC, nullptr, 0) == SF_AMBISONIC_B_FORMAT)
        {
            format = AL_FORMAT_BFORMAT2D_16;
            Log::MDCII_LOG_DEBUG("[SoundFile::ReadAudioDataFormat()] Found format AL_FORMAT_BFORMAT2D_16 in file {}.", filePath);
        }
        break;
    case 4:
        if (sf_command(sndFile, SFC_WAVEX_GET_AMBISONIC, nullptr, 0) == SF_AMBISONIC_B_FORMAT)
        {
            format = AL_FORMAT_BFORMAT3D_16;
            Log::MDCII_LOG_DEBUG("[SoundFile::ReadAudioDataFormat()] Found format AL_FORMAT_BFORMAT3D_16 in file {}.", filePath);
        }
        break;
    default:
        sf_close(sndFile);
        throw MDCII_EXCEPTION("[SoundFile::ReadAudioDataFormat()] Unsupported channel count from file " + filePath + ".");
    }

    MDCII_ASSERT(format, "[SoundFile::ReadAudioDataFormat()] Invalid audio data format.")
}

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::file::SoundFile::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[SoundFile::CleanUp()] CleanUp sound file.");

    if (sndFile)
    {
        sf_close(sndFile);
    }
}
