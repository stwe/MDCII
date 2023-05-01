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

#include "Figures.h"
#include "Log.h"

mdcii::data::Figures::Figures(const std::string& t_codFilePath)
{
    GenerateFigures(t_codFilePath);
}

mdcii::data::Figures::~Figures() noexcept
{
}

void mdcii::data::Figures::GenerateFigures(const std::string& t_codFilePath)
{
    Log::MDCII_LOG_DEBUG("[Figures::GenerateFigures()] Generate figures...");

    const auto cod{ cod::CodParser(t_codFilePath) };
    for (auto i{ 0 }; i < cod.objects.object_size(); ++i)
    {
        if (const auto& obj{ cod.objects.object(i) }; obj.name() == "FIGUR")
        {
            for (auto j{ 0 }; j < obj.objects_size(); ++j)
            {
                //const auto building{ GenerateBuilding(&obj.objects(j)) };
                //buildingsMap.emplace(building.id, building);
            }
        }
    }

    Log::MDCII_LOG_DEBUG("[Figures::GenerateFigures()] The figures were generated successfully.");
}

mdcii::data::Figure mdcii::data::Figures::GenerateFigure(const cod_pb::Object* t_obj)
{
    return mdcii::data::Figure();
}
