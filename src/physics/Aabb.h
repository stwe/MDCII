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

#include "vendor/olc/olcPixelGameEngine.h"

namespace mdcii::physics
{
    /**
     * Represents an Aabb (axis-aligned bounding box) for collision detection.
     */
    struct Aabb
    {
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief The position.
         */
        olc::vi2d position{ -1, -1 };

        /**
         * @brief The size.
         */
        olc::vi2d size{-1, -1 };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Aabb() = default;

        /**
         * @brief Constructs a new Aabb object.
         *
         * @param t_position The position.
         * @param t_size The size.
         */
        Aabb(const olc::vi2d& t_position, const olc::vi2d& t_size)
            : position{ t_position }
            , size{ t_size }
        {}

        //-------------------------------------------------
        // Collision detection
        //-------------------------------------------------

        /**
         * @brief Point vs Aabb.
         *
         * @param t_point A point.
         * @param t_aabb An Aabb.
         *
         * @return True if there is a collision.
         */
        static bool PointVsAabb(const olc::vi2d& t_point, const Aabb& t_aabb)
        {
            return
                t_point.x >= t_aabb.position.x &&
                t_point.y >= t_aabb.position.y &&
                t_point.x < t_aabb.position.x + t_aabb.size.x &&
                t_point.y < t_aabb.position.y + t_aabb.size.y;
        }

        /**
         * @brief Aabb vs Aabb.
         *
         * @param t_a An Aabb.
         * @param t_b Another Aabb.
         *
         * @return True if there is a collision.
         */
        static bool AabbVsAabb(const Aabb& t_a, const Aabb& t_b)
        {
            return
                t_a.position.x < t_b.position.x + t_b.size.x &&
                t_a.position.x + t_a.size.x > t_b.position.x &&
                t_a.position.y < t_b.position.y + t_b.size.y &&
                t_a.position.y + t_a.size.y > t_b.position.y;
        }
    };
}
