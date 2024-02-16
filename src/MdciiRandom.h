// This file is part of the MDCII project.
//
// Copyright (c) 2024. stwe <https://github.com/stwe/MDCII>
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

#include <random>

static std::mt19937 mdcii_mersenne_engine(std::random_device{}());

template <typename IntType>
IntType uniform_random_int(IntType t_a, IntType t_b)
{
    std::uniform_int_distribution<IntType> dist(t_a, t_b);
    return dist(mdcii_mersenne_engine);
}

template <typename RealType>
RealType uniform_random_real(RealType t_a, RealType t_b)
{
    std::uniform_real_distribution<RealType> dist(t_a, t_b);
    return dist(mdcii_mersenne_engine);
}
