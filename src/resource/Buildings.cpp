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

#include "Buildings.h"
#include "Log.h"
#include "cod/CodParser.h"
#include "vendor/enum/magic_enum.hpp"

//-------------------------------------------------
// Building
//-------------------------------------------------

std::vector<std::pair<int, int>> mdcii::resource::Building::CreateBuildingStartOffsets() const
{
    return {
        { 0, 0 },
        { size.w - 1, 0 },
        { 0, size.h - 1 }
    };
}

//-------------------------------------------------
// Buildings
//-------------------------------------------------

mdcii::resource::Buildings::Buildings(const std::string& t_codFilePath)
{
    MDCII_LOG_DEBUG("[Buildings::Buildings()] Create Buildings.");

    GenerateBuildings(t_codFilePath);
}

mdcii::resource::Buildings::~Buildings() noexcept
{
    MDCII_LOG_DEBUG("[Buildings::~Buildings()] Destruct Buildings.");
}

void mdcii::resource::Buildings::GenerateBuildings(const std::string& t_codFilePath)
{
    MDCII_LOG_DEBUG("[Buildings::GenerateBuildings()] Generate buildings from haeuser.cod ...");

    const auto cod{ cod::CodParser(t_codFilePath, true) };
    for (auto i{ 0 }; i < cod.objects.object_size(); ++i)
    {
        if (const auto& obj{ cod.objects.object(i) }; obj.name() == "HAUS")
        {
            for (auto j{ 0 }; j < obj.objects_size(); ++j)
            {
                const auto building{ GenerateBuilding(&obj.objects(j)) };
                buildingsMap.try_emplace(building.id, building);
            }
        }
    }

    MDCII_LOG_DEBUG("[Buildings::GenerateBuildings()] The buildings were generated successfully from the haeuser.cod.");
}

mdcii::resource::Building mdcii::resource::Buildings::GenerateBuilding(const cod_pb::Object* t_obj)
{
    Building building;

    if (t_obj->has_variables())
    {
        for (auto i{ 0 }; i < t_obj->variables().variable_size(); ++i)
        {
            const auto var{ t_obj->variables().variable(i) };
            if (var.name() == "Id")
            {
                if (var.value_int() == 0)
                {
                    building.id = 0;
                }
                else
                {
                    building.id = var.value_int() - 20000;
                }
            }
            else if (var.name() == "Gfx")
            {
                building.gfx = var.value_int();
            }
            else if (var.name() == "Blocknr")
            {
                building.blocknr = var.value_int();
            }
            else if (var.name() == "Kind")
            {
                const auto kind{ magic_enum::enum_cast<KindType>(var.value_string()) };
                if (kind.has_value())
                {
                    building.kind = kind.value();
                }
            }
            else if (var.name() == "Posoffs")
            {
                building.posoffs = var.value_int();
            }
            else if (var.name() == "Wegspeed")
            {
                for (auto v{ 0 }; v < var.value_array().value_size(); ++v)
                {
                    building.wegspeed.push_back(var.value_array().value(v).value_int());
                }
            }
            else if (var.name() == "Highflg")
            {
                building.highflg = var.value_int();
            }
            else if (var.name() == "Einhoffs")
            {
                building.einhoffs = var.value_int();
            }
            else if (var.name() == "Bausample")
            {
                const auto bausample{ magic_enum::enum_cast<BausampleType>(var.value_string()) };
                if (bausample.has_value())
                {
                    building.bausample = bausample.value();
                }
            }
            else if (var.name() == "Ruinenr")
            {
                const auto ruinenr{ magic_enum::enum_cast<RuinenrType>(var.value_string()) };
                if (ruinenr.has_value())
                {
                    building.ruinenr = ruinenr.value();
                }
            }
            else if (var.name() == "Maxenergy")
            {
                building.maxenergy = var.value_int();
            }
            else if (var.name() == "Maxbrand")
            {
                building.maxbrand = var.value_int();
            }
            else if (var.name() == "Size")
            {
                building.size.w = var.value_array().value(0).value_int();
                building.size.h = var.value_array().value(1).value_int();
            }
            else if (var.name() == "Rotate")
            {
                building.rotate = var.value_int();
            }
            else if (var.name() == "RandAnz")
            {
                building.randAnz = var.value_int();
            }
            else if (var.name() == "AnimTime")
            {
                if (var.value_string() == "TIMENEVER")
                {
                    building.animTime = -1;
                }
                else
                {
                    building.animTime = var.value_int();
                }
            }
            else if (var.name() == "AnimFrame")
            {
                building.animFrame = var.value_int();
            }
            else if (var.name() == "AnimAdd")
            {
                building.animAdd = var.value_int();
            }
            else if (var.name() == "Baugfx")
            {
                building.baugfx = var.value_int();
            }
            else if (var.name() == "PlaceFlg")
            {
                building.placeFlg = var.value_int();
            }
            else if (var.name() == "AnimAnz")
            {
                building.animAnz = var.value_int();
            }
            else if (var.name() == "KreuzBase")
            {
                building.kreuzBase = var.value_int();
            }
            else if (var.name() == "NoShotFlg")
            {
                building.noShotFlg = var.value_int();
            }
            else if (var.name() == "Strandflg")
            {
                building.strandflg = var.value_int();
            }
            else if (var.name() == "Ausbauflg")
            {
                building.ausbauflg = var.value_int();
            }
            else if (var.name() == "Tuerflg")
            {
                building.tuerflg = var.value_int();
            }
            else if (var.name() == "Randwachs")
            {
                building.randwachs = var.value_int();
            }
            else if (var.name() == "RandAdd")
            {
                building.randAdd = var.value_int();
            }
            else if (var.name() == "Strandoff")
            {
                building.strandoff = var.value_int();
            }
            else if (var.name() == "Destroyflg")
            {
                building.destroyflg = var.value_int();
            }
        }
    }

    if (t_obj->objects_size() > 0)
    {
        for (auto i{ 0 }; i < t_obj->objects_size(); ++i)
        {
            const auto& nestedObj{ t_obj->objects(i) };
            if (nestedObj.name() == "HAUS_PRODTYP")
            {
                for (auto j{ 0 }; j < nestedObj.variables().variable_size(); ++j)
                {
                    const auto var{ nestedObj.variables().variable(j) };
                    if (var.name() == "Kind")
                    {
                        const auto kind{ magic_enum::enum_cast<ProdtypKindType>(var.value_string()) };
                        if (kind.has_value())
                        {
                            building.houseProductionType.kind = kind.value();
                        }
                    }
                    else if (var.name() == "Ware")
                    {
                        const auto ware{ magic_enum::enum_cast<WareType>(var.value_string()) };
                        if (ware.has_value())
                        {
                            building.houseProductionType.ware = ware.value();
                        }
                    }
                    else if (var.name() == "Workstoff")
                    {
                        const auto workstoff{ magic_enum::enum_cast<WorkstoffType>(var.value_string()) };
                        if (workstoff.has_value())
                        {
                            building.houseProductionType.workstoff = workstoff.value();
                        }
                    }
                    else if (var.name() == "Erzbergnr")
                    {
                        const auto erzbergnr{ magic_enum::enum_cast<ErzbergnrType>(var.value_string()) };
                        if (erzbergnr.has_value())
                        {
                            building.houseProductionType.erzbergnr = erzbergnr.value();
                        }
                    }
                    else if (var.name() == "Rohstoff")
                    {
                        const auto rohstoff{ magic_enum::enum_cast<RohstoffType>(var.value_string()) };
                        if (rohstoff.has_value())
                        {
                            building.houseProductionType.rohstoff = rohstoff.value();
                        }
                    }
                    else if (var.name() == "MAXPRODCNT")
                    {
                        const auto maxprodcnt{ magic_enum::enum_cast<MaxprodcntType>(var.value_string()) };
                        if (maxprodcnt.has_value())
                        {
                            building.houseProductionType.maxprodcnt = maxprodcnt.value();
                        }
                    }
                    else if (var.name() == "Bauinfra")
                    {
                        const auto bauinfra{ magic_enum::enum_cast<BauinfraType>(var.value_string()) };
                        if (bauinfra.has_value())
                        {
                            building.houseProductionType.bauinfra = bauinfra.value();
                        }
                    }
                    else if (var.name() == "Figurnr")
                    {
                        const auto figurnr{ magic_enum::enum_cast<FigurnrType>(var.value_string()) };
                        if (figurnr.has_value())
                        {
                            building.houseProductionType.figurnr = figurnr.value();
                        }
                    }
                    else if (var.name() == "Rauchfignr")
                    {
                        const auto rauchfignr{ magic_enum::enum_cast<RauchfignrType>(var.value_string()) };
                        if (rauchfignr.has_value())
                        {
                            building.houseProductionType.rauchfignr = rauchfignr.value();
                        }
                    }
                    else if (var.name() == "Maxware")
                    {
                        for (auto v{ 0 }; v < var.value_array().value_size(); ++v)
                        {
                            building.houseProductionType.maxware.push_back(var.value_array().value(v).value_int());
                        }
                    }
                    else if (var.name() == "Kosten")
                    {
                        for (auto v{ 0 }; v < var.value_array().value_size(); ++v)
                        {
                            building.houseProductionType.kosten.push_back(var.value_array().value(v).value_int());
                        }
                    }
                    else if (var.name() == "BGruppe")
                    {
                        building.houseProductionType.bGruppe = var.value_int();
                    }
                    else if (var.name() == "LagAniFlg")
                    {
                        building.houseProductionType.lagAniFlg = var.value_int();
                    }
                    else if (var.name() == "NoMoreWork")
                    {
                        building.houseProductionType.noMoreWork = var.value_int();
                    }
                    else if (var.name() == "Workmenge")
                    {
                        building.houseProductionType.workmenge = var.value_int();
                    }
                    else if (var.name() == "Doerrflg")
                    {
                        building.houseProductionType.doerrflg = var.value_int();
                    }
                    else if (var.name() == "Anicontflg")
                    {
                        building.houseProductionType.anicontflg = var.value_int();
                    }
                    else if (var.name() == "MakLagFlg")
                    {
                        building.houseProductionType.makLagFlg = var.value_int();
                    }
                    else if (var.name() == "Nativflg")
                    {
                        building.houseProductionType.nativflg = var.value_int();
                    }
                    else if (var.name() == "NoLagVoll")
                    {
                        building.houseProductionType.noLagVoll = var.value_int();
                    }
                    else if (var.name() == "Radius")
                    {
                        building.houseProductionType.radius = var.value_int();
                    }
                    else if (var.name() == "Rohmenge")
                    {
                        building.houseProductionType.rohmenge = var.value_int();
                    }
                    else if (var.name() == "Prodmenge")
                    {
                        building.houseProductionType.prodmenge = var.value_int();
                    }
                    else if (var.name() == "Randwachs")
                    {
                        building.houseProductionType.randwachs = var.value_int();
                    }
                    else if (var.name() == "Maxlager")
                    {
                        building.houseProductionType.maxlager = var.value_int();
                    }
                    else if (var.name() == "Maxnorohst")
                    {
                        building.houseProductionType.maxnorohst = var.value_int();
                    }
                    else if (var.name() == "Arbeiter")
                    {
                        building.houseProductionType.arbeiter = var.value_int();
                    }
                    else if (var.name() == "Figuranz")
                    {
                        building.houseProductionType.figuranz = var.value_int();
                    }
                    else if (var.name() == "Interval")
                    {
                        building.houseProductionType.interval = var.value_int();
                    }
                }
            }
            else if (nestedObj.name() == "HAUS_BAUKOST")
            {
                for (auto j{ 0 }; j < nestedObj.variables().variable_size(); ++j)
                {
                    const auto var{ nestedObj.variables().variable(j) };
                    if (var.name() == "Money")
                    {
                        building.houseBuildCosts.money = var.value_int();
                    }
                    else if (var.name() == "Werkzeug")
                    {
                        building.houseBuildCosts.werkzeug = var.value_int();
                    }
                    else if (var.name() == "Holz")
                    {
                        building.houseBuildCosts.holz = var.value_int();
                    }
                    else if (var.name() == "Ziegel")
                    {
                        building.houseBuildCosts.ziegel = var.value_int();
                    }
                    else if (var.name() == "Kanon")
                    {
                        building.houseBuildCosts.kanon = var.value_int();
                    }
                }
            }
        }
    }

    return building;
}

//-------------------------------------------------
// Figure
//-------------------------------------------------

mdcii::resource::Figures::Figures(const std::string& t_codFilePath)
{
    MDCII_LOG_DEBUG("[Figures::Figures()] Create Figures.");

    GenerateFigures(t_codFilePath);
}

mdcii::resource::Figures::~Figures() noexcept
{
    MDCII_LOG_DEBUG("[Figures::~Figures()] Destruct Figures.");
}

void mdcii::resource::Figures::GenerateFigures(const std::string& t_codFilePath)
{
    MDCII_LOG_DEBUG("[Figures::GenerateFigures()] Generate figures from figuren.cod ...");

    const auto cod{ cod::CodParser(t_codFilePath, true) };
    for (auto i{ 0 }; i < cod.objects.object_size(); ++i)
    {
        if (const auto& obj{ cod.objects.object(i) }; obj.name() == "FIGUR")
        {
            for (auto j{ 0 }; j < obj.objects_size(); ++j)
            {
                // todo: Animals only now
                if (FIGURE_STRING_TO_ID_MAP.contains(obj.objects(j).name()))
                {
                    const auto figureId{ FIGURE_STRING_TO_ID_MAP.at(obj.objects(j).name()) };
                    const auto figure{ GenerateFigure(figureId, &obj.objects(j)) };
                    figuresMap.try_emplace(figureId, figure);
                }
            }
        }
    }

    MDCII_LOG_DEBUG("[Figures::GenerateFigures()] The figures were generated successfully from the figuren.cod.");
}

mdcii::resource::Figure mdcii::resource::Figures::GenerateFigure(const FigureId t_figureId, const cod_pb::Object* t_obj)
{
    Figure figure;
    figure.id = t_figureId;

    if (t_obj->has_variables())
    {
        for (auto i{ 0 }; i < t_obj->variables().variable_size(); ++i)
        {
            const auto var{ t_obj->variables().variable(i) };
            if (var.name() == "Gfx")
            {
                figure.gfx = var.value_int();
            }
            else if (var.name() == "Rotate")
            {
                figure.rotate = var.value_int();
            }
        }
    }

    if (t_obj->objects_size() > 0)
    {
        for (auto i{ 0 }; i < t_obj->objects_size(); ++i)
        {
            const auto &nestedObj{ t_obj->objects(i) };
            if (nestedObj.name() == "ANIM")
            {
                if (nestedObj.objects_size() > 0)
                {
                    for (auto j{ 0 }; j < nestedObj.objects_size(); ++j)
                    {
                        if (nestedObj.objects(j).has_variables())
                        {
                            FigureAnimation figureAnimation;
                            for (auto k{ 0 }; k < nestedObj.objects(j).variables().variable_size(); ++k)
                            {
                                const auto var{ nestedObj.objects(j).variables().variable(k) };
                                if (var.name() == "Gfx")
                                {
                                    figureAnimation.gfx = figure.gfx; // todo: normally is 0
                                }
                                if (var.name() == "Kind")
                                {
                                    const auto kind{ magic_enum::enum_cast<AnimationKindType>(var.value_string()) };
                                    if (kind.has_value())
                                    {
                                        figureAnimation.animationKind = kind.value();
                                    }
                                }
                                else if (var.name() == "AnimOffs")
                                {
                                    figureAnimation.animOffs = var.value_int();
                                }
                                else if (var.name() == "AnimAdd")
                                {
                                    figureAnimation.animAdd = var.value_int();
                                }
                                else if (var.name() == "AnimAnz")
                                {
                                    figureAnimation.animAnz = var.value_int();
                                }
                                else if (var.name() == "AnimSpeed")
                                {
                                    figureAnimation.animSpeed = var.value_int();
                                }
                                else if (var.name() == "Rotate")
                                {
                                    figureAnimation.rotate = var.value_int();
                                }
                            }
                            figure.animations.push_back(figureAnimation);
                        }
                    }
                }
            }
        }
    }

    return figure;
}
