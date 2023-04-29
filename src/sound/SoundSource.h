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

#include <AL/al.h>
#include <glm/vec3.hpp>

//-------------------------------------------------
// SoundSource
//-------------------------------------------------

namespace mdcii::sound
{
    /**
     * Plays a sound buffer as effect with a position and velocity.
     */
    class SoundSource
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        SoundSource() = delete;

        /**
         * Constructs a new SoundSource object.
         *
         * @param t_bufferId The buffer handle of the sound data.
         */
        explicit SoundSource(ALint t_bufferId);

        SoundSource(const SoundSource& t_other) = delete;
        SoundSource(SoundSource&& t_other) noexcept = delete;
        SoundSource& operator=(const SoundSource& t_other) = delete;
        SoundSource& operator=(SoundSource&& t_other) noexcept = delete;

        ~SoundSource() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        /**
         * Plays the sound effect.
         */
        void Play() const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The handle of the sound source.
         */
        ALuint m_sourceId{ 0 };

        /**
         * No pitch.
         */
        float m_pitch{ 1.0f };

        /**
         * No gain.
         */
        float m_gain{ 1.0f };

        /**
         * Position of the sound source.
         */
        glm::vec3 m_position{ 0.0f };

        /**
         * Velocity of the sound source.
         */
        glm::vec3 m_velocity{ 0.0f };

        /**
         * No looping.
         */
        bool m_loopSound{ false };

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initializes the sound source.
         *
         * @param t_bufferId The buffer handle of the sound data.
         */
        void Init(ALint t_bufferId);

        //-------------------------------------------------
        // Clean up
        //-------------------------------------------------

        /**
         * Clean up.
         */
        void CleanUp() const;
    };
}
