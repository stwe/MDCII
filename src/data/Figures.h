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
#include "cod/CodParser.h"

namespace mdcii::data
{
    struct Figure
    {
        int32_t id{ -1 };
        int32_t gfx{ -1 };
    };

    class Figures
    {
    public:
        Figures() = delete;

        /**
         * Constructs a new Figures object.
         *
         * @param t_codFilePath The path to the figuren.cod.
         */
        explicit Figures(const std::string& t_codFilePath);

        Figures(const Figures& t_other) = delete;
        Figures(Figures&& t_other) noexcept = delete;
        Figures& operator=(const Figures& t_other) = delete;
        Figures& operator=(Figures&& t_other) noexcept = delete;

        ~Figures() noexcept;

    protected:

    private:
        void GenerateFigures(const std::string& t_codFilePath);
        static Figure GenerateFigure(const cod_pb::Object* t_obj);
    };
}
