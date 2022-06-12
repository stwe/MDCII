#include <magic_enum.hpp>
#include "Buildings.h"
#include "cod/CodParser.h"

mdcii::data::Buildings::Buildings(std::shared_ptr<cod::CodParser> t_cod)
    : m_cod{ std::move(t_cod) }
{
    GenerateBuildings();
}

void mdcii::data::Buildings::GenerateBuildings()
{
    for (int o = 0; o < m_cod->objects.object_size(); ++o)
    {
        auto obj{ m_cod->objects.object(o) };
        if (obj.name() == "HAUS")
        {
            for (int i = 0; i < obj.objects_size(); ++i)
            {
                auto haus{ GenerateBuilding(&obj.objects(i)) };
                buildingsMap.emplace(haus.id, haus);
            }
        }
    }
}

mdcii::data::Building mdcii::data::Buildings::GenerateBuilding(const cod_pb::Object* t_obj) const
{
    Building h;
    if (t_obj->has_variables() == true)
    {
        for (int i = 0; i < t_obj->variables().variable_size(); ++i)
        {
            auto var = t_obj->variables().variable(i);
            if (var.name() == "Id")
            {
                if (var.value_int() == 0)
                {
                    h.id = 0;
                }
                else
                {
                    h.id = var.value_int() - 20000;
                }
            }
            else if (var.name() == "Gfx")
            {
                h.gfx = var.value_int();
            }
            else if (var.name() == "Blocknr")
            {
                h.blocknr = var.value_int();
            }
            else if (var.name() == "Kind")
            {
                auto kind{ magic_enum::enum_cast<ObjectKindType>(var.value_string()) };
                if (kind.has_value())
                {
                    h.kind = kind.value();
                }
            }
            else if (var.name() == "Posoffs")
            {
                h.posoffs = var.value_int();
            }
            else if (var.name() == "Wegspeed")
            {
                for (int v = 0; v < var.value_array().value_size(); v++)
                {
                    h.wegspeed.push_back(var.value_array().value(v).value_int());
                }
            }
            else if (var.name() == "Highflg")
            {
                h.highflg = var.value_int();
            }
            else if (var.name() == "Einhoffs")
            {
                h.einhoffs = var.value_int();
            }
            else if (var.name() == "Bausample")
            {
                auto bausample{ magic_enum::enum_cast<BausampleType>(var.value_string()) };
                if (bausample.has_value())
                {
                    h.bausample = bausample.value();
                }
            }
            else if (var.name() == "Ruinenr")
            {
                auto ruinenr{ magic_enum::enum_cast<RuinenrType>(var.value_string()) };
                if (ruinenr.has_value())
                {
                    h.ruinenr = ruinenr.value();
                }
            }
            else if (var.name() == "Maxenergy")
            {
                h.maxenergy = var.value_int();
            }
            else if (var.name() == "Maxbrand")
            {
                h.maxbrand = var.value_int();
            }
            else if (var.name() == "Size")
            {
                h.size.w = var.value_array().value(0).value_int();
                h.size.h = var.value_array().value(1).value_int();
            }
            else if (var.name() == "Rotate")
            {
                h.rotate = var.value_int();
            }
            else if (var.name() == "RandAnz")
            {
                h.randAnz = var.value_int();
            }
            else if (var.name() == "AnimTime")
            {
                if (var.value_string() == "TIMENEVER")
                {
                    h.animTime = -1;
                }
                else
                {
                    h.animTime = var.value_int();
                }
            }
            else if (var.name() == "AnimFrame")
            {
                h.animFrame = var.value_int();
            }
            else if (var.name() == "AnimAdd")
            {
                h.animAdd = var.value_int();
            }
            else if (var.name() == "Baugfx")
            {
                h.baugfx = var.value_int();
            }
            else if (var.name() == "PlaceFlg")
            {
                h.placeFlg = var.value_int();
            }
            else if (var.name() == "AnimAnz")
            {
                h.animAnz = var.value_int();
            }
            else if (var.name() == "KreuzBase")
            {
                h.kreuzBase = var.value_int();
            }
            else if (var.name() == "NoShotFlg")
            {
                h.noShotFlg = var.value_int();
            }
            else if (var.name() == "Strandflg")
            {
                h.strandflg = var.value_int();
            }
            else if (var.name() == "Ausbauflg")
            {
                h.ausbauflg = var.value_int();
            }
            else if (var.name() == "Tuerflg")
            {
                h.tuerflg = var.value_int();
            }
            else if (var.name() == "Randwachs")
            {
                h.randwachs = var.value_int();
            }
            else if (var.name() == "RandAdd")
            {
                h.randAdd = var.value_int();
            }
            else if (var.name() == "Strandoff")
            {
                h.strandoff = var.value_int();
            }
            else if (var.name() == "Destroyflg")
            {
                h.destroyflg = var.value_int();
            }
        }
    }

    if (t_obj->objects_size() > 0)
    {
        for (int o = 0; o < t_obj->objects_size(); ++o)
        {
            auto nestedObj = t_obj->objects(o);
            if (nestedObj.name() == "HAUS_PRODTYP")
            {
                for (int i = 0; i < nestedObj.variables().variable_size(); i++)
                {
                    auto var = nestedObj.variables().variable(i);
                    if (var.name() == "Kind")
                    {
                        auto kind{ magic_enum::enum_cast<ProdtypKindType>(var.value_string()) };
                        if (kind.has_value())
                        {
                            h.houseProductionType.kind = kind.value();
                        }
                    }
                    else if (var.name() == "Ware")
                    {
                        auto ware{ magic_enum::enum_cast<WareType>(var.value_string()) };
                        if (ware.has_value())
                        {
                            h.houseProductionType.ware = ware.value();
                        }
                    }
                    else if (var.name() == "Workstoff")
                    {
                        auto workstoff{ magic_enum::enum_cast<WorkstoffType>(var.value_string()) };
                        if (workstoff.has_value())
                        {
                            h.houseProductionType.workstoff = workstoff.value();
                        }
                    }
                    else if (var.name() == "Erzbergnr")
                    {
                        auto erzbergnr{ magic_enum::enum_cast<ErzbergnrType>(var.value_string()) };
                        if (erzbergnr.has_value())
                        {
                            h.houseProductionType.erzbergnr = erzbergnr.value();
                        }
                    }
                    else if (var.name() == "Rohstoff")
                    {
                        auto rohstoff{ magic_enum::enum_cast<RohstoffType>(var.value_string()) };
                        if (rohstoff.has_value())
                        {
                            h.houseProductionType.rohstoff = rohstoff.value();
                        }
                    }
                    else if (var.name() == "MAXPRODCNT")
                    {
                        auto maxprodcnt{ magic_enum::enum_cast<MaxprodcntType>(var.value_string()) };
                        if (maxprodcnt.has_value())
                        {
                            h.houseProductionType.maxprodcnt = maxprodcnt.value();
                        }
                    }
                    else if (var.name() == "Bauinfra")
                    {
                        auto bauinfra{ magic_enum::enum_cast<BauinfraType>(var.value_string()) };
                        if (bauinfra.has_value())
                        {
                            h.houseProductionType.bauinfra = bauinfra.value();
                        }
                    }
                    else if (var.name() == "Figurnr")
                    {
                        auto figurnr{ magic_enum::enum_cast<FigurnrType>(var.value_string()) };
                        if (figurnr.has_value())
                        {
                            h.houseProductionType.figurnr = figurnr.value();
                        }
                    }
                    else if (var.name() == "Rauchfignr")
                    {
                        auto rauchfignr{ magic_enum::enum_cast<RauchfignrType>(var.value_string()) };
                        if (rauchfignr.has_value())
                        {
                            h.houseProductionType.rauchfignr = rauchfignr.value();
                        }
                    }
                    else if (var.name() == "Maxware")
                    {
                        for (int v = 0; v < var.value_array().value_size(); v++)
                        {
                            h.houseProductionType.maxware.push_back(var.value_array().value(v).value_int());
                        }
                    }
                    else if (var.name() == "Kosten")
                    {
                        for (int v = 0; v < var.value_array().value_size(); v++)
                        {
                            h.houseProductionType.kosten.push_back(var.value_array().value(v).value_int());
                        }
                    }
                    else if (var.name() == "BGruppe")
                    {
                        h.houseProductionType.bGruppe = var.value_int();
                    }
                    else if (var.name() == "LagAniFlg")
                    {
                        h.houseProductionType.lagAniFlg = var.value_int();
                    }
                    else if (var.name() == "NoMoreWork")
                    {
                        h.houseProductionType.noMoreWork = var.value_int();
                    }
                    else if (var.name() == "Workmenge")
                    {
                        h.houseProductionType.workmenge = var.value_int();
                    }
                    else if (var.name() == "Doerrflg")
                    {
                        h.houseProductionType.doerrflg = var.value_int();
                    }
                    else if (var.name() == "Anicontflg")
                    {
                        h.houseProductionType.anicontflg = var.value_int();
                    }
                    else if (var.name() == "MakLagFlg")
                    {
                        h.houseProductionType.makLagFlg = var.value_int();
                    }
                    else if (var.name() == "Nativflg")
                    {
                        h.houseProductionType.nativflg = var.value_int();
                    }
                    else if (var.name() == "NoLagVoll")
                    {
                        h.houseProductionType.noLagVoll = var.value_int();
                    }
                    else if (var.name() == "Radius")
                    {
                        h.houseProductionType.radius = var.value_int();
                    }
                    else if (var.name() == "Rohmenge")
                    {
                        h.houseProductionType.rohmenge = var.value_int();
                    }
                    else if (var.name() == "Prodmenge")
                    {
                        h.houseProductionType.prodmenge = var.value_int();
                    }
                    else if (var.name() == "Randwachs")
                    {
                        h.houseProductionType.randwachs = var.value_int();
                    }
                    else if (var.name() == "Maxlager")
                    {
                        h.houseProductionType.maxlager = var.value_int();
                    }
                    else if (var.name() == "Maxnorohst")
                    {
                        h.houseProductionType.maxnorohst = var.value_int();
                    }
                    else if (var.name() == "Arbeiter")
                    {
                        h.houseProductionType.arbeiter = var.value_int();
                    }
                    else if (var.name() == "Figuranz")
                    {
                        h.houseProductionType.figuranz = var.value_int();
                    }
                    else if (var.name() == "Interval")
                    {
                        h.houseProductionType.interval = var.value_int();
                    }
                }
            }
            else if (nestedObj.name() == "HAUS_BAUKOST")
            {
                for (int i = 0; i < nestedObj.variables().variable_size(); i++)
                {
                    auto var = nestedObj.variables().variable(i);
                    if (var.name() == "Money")
                    {
                        h.houseBuildCosts.money = var.value_int();
                    }
                    else if (var.name() == "Werkzeug")
                    {
                        h.houseBuildCosts.werkzeug = var.value_int();
                    }
                    else if (var.name() == "Holz")
                    {
                        h.houseBuildCosts.holz = var.value_int();
                    }
                    else if (var.name() == "Ziegel")
                    {
                        h.houseBuildCosts.ziegel = var.value_int();
                    }
                    else if (var.name() == "Kanon")
                    {
                        h.houseBuildCosts.kanon = var.value_int();
                    }
                }
            }
        }
    }

    return h;
}
