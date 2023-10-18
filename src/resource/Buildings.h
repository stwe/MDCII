// This file is part of the MDCII project.
//
// Copyright (c) 2023. stwe <https://github.com/stwe/MDCII>
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

#include <map>
#include <vector>
#include <cstdint>
#include <string>

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace cod_pb
{
    class Object;
}

namespace mdcii::resource
{
    //-------------------------------------------------
    // Kind
    //-------------------------------------------------

    enum class KindType
    {
        UNSET = 0,
        WALD,
        TOR,
        RUINE,
        HQ,
        STRANDMUND,
        STRANDHAUS,
        BRANDUNG,
        STRANDRUINE,
        HANGECK,
        TURM,
        STRANDECKI,
        STRANDVARI,
        STRANDECKA,
        MEER,
        BODEN,
        HANGQUELL,
        MAUER,
        FELS,
        MUENDUNG,
        HANG,
        MINE,
        BRANDECK,
        TURMSTRAND,
        PIER,
        STRAND,
        BRUECKE,
        FLUSSECK,
        FLUSS,
        HAFEN,
        STRASSE,
        GEBAEUDE,
        MAUERSTRAND,
        PLATZ
    };

    //-------------------------------------------------
    // Ruinenr
    //-------------------------------------------------

    enum class RuinenrType
    {
        UNSET = 0,
        RUINE_KONTOR_N1,
        RUINE_KONTOR_N2,
        RUINE_HOLZ,
        RUINE_KONTOR_1,
        RUINE_FELD,
        RUINE_MARKT,
        RUINE_STEIN,
        RUINE_MINE,
        RUINE_ROAD_STEIN,
        RUINE_ROAD_FELD
    };

    //-------------------------------------------------
    // Bausample
    //-------------------------------------------------

    enum class BausampleType
    {
        UNSET = 0,
        WAV_TRIUMPH,
        WAV_SCHWERTBAU,
        WAV_PYRAMIDE,
        WAV_KATHETRALE,
        WAV_NIX,
        WAV_SCHLOSS,
        WAV_KAPELLE,
        WAV_WOHNHAUS,
        WAV_BRUNNEN,
        WAV_KONTOR,
        WAV_GIESSEREI,
        WAV_FLEISCHER,
        WAV_SCHULE,
        WAV_DENKMAL,
        WAV_BAECKER,
        WAV_MINE,
        WAV_STANDARD,
        WAV_BURG,
        WAV_RUMBRENNER,
        WAV_PLANTAGE,
        WAV_ARZT,
        WAV_MUEHLE,
        WAV_PIRATEN,
        WAV_JAGDHUETTE,
        WAV_KIRCHE,
        WAV_MARKT,
        WAV_BADEHAUS,
        WAV_WIRTSHAUS,
        WAV_THEATER
    };

    //-------------------------------------------------
    // Workstoff
    //-------------------------------------------------

    enum class WorkstoffType
    {
        UNSET = 0,
        HOLZ,
        STOFFE
    };

    //-------------------------------------------------
    // Erzbergnr
    //-------------------------------------------------

    enum class ErzbergnrType
    {
        UNSET = 0,
        ERZBERG_KLEIN,
        ERZBERG_GROSS
    };

    //-------------------------------------------------
    // HAUS_PRODTYP - Kind
    //-------------------------------------------------

    enum class ProdtypKindType
    {
        UNSET = 0,
        BRUNNEN,
        KONTOR,
        HOCHSCHULE,
        JAGDHAUS,
        STEINBRUCH,
        MARKT,
        PLANTAGE,
        DENKMAL,
        FISCHEREI,
        WEIDETIER,
        SCHLOSS,
        WIRT,
        HANDWERK,
        WACHTURM,
        KAPELLE,
        WERFT,
        THEATER,
        GALGEN,
        UNUSED,
        pMAUER,
        WOHNUNG,
        TRIUMPH,
        ROHSTOFF,
        KLINIK,
        BADEHAUS,
        ROHSTWACHS,
        SCHULE,
        ROHSTERZ,
        BERGWERK,
        PIRATWOHN,
        KIRCHE,
        MILITAR,
    };

    //-------------------------------------------------
    // Ware
    //-------------------------------------------------

    enum class WareType
    {
        UNSET = 0,
        MEHL,
        GETREIDE,
        GOLD,
        KAKAO,
        WEINTRAUBEN,
        ALLWARE,
        TABAK,
        FISCHE,
        STOFFE,
        ALKOHOL,
        ERZE,
        ZIEGEL,
        GRAS,
        TABAKBAUM,
        EISENERZ,
        NOWARE,
        SCHMUCK,
        KLEIDUNG,
        SCHWERTER,
        HOLZ,
        BAUM,
        STEINE,
        KORN,
        KAKAOBAUM,
        EISEN,
        KANONEN,
        FLEISCH,
        GEWUERZE,
        TABAKWAREN,
        ZUCKER,
        NAHRUNG,
        WOLLE,
        BAUMWOLLE,
        WERKZEUG,
        GEWUERZBAUM,
        ZUCKERROHR,
        MUSKETEN,
    };

    //-------------------------------------------------
    // Rohstoff
    //-------------------------------------------------

    enum class RohstoffType
    {
        UNSET = 0,
        KORN,
        GETREIDE,
        GOLD,
        WEINTRAUBEN,
        ZUCKER,
        TABAK,
        FISCHE,
        STOFFE,
        GRAS,
        TABAKBAUM,
        MEHL,
        EISENERZ,
        NOWARE,
        HOLZ,
        BAUM,
        ALLWARE,
        KAKAOBAUM,
        EISEN,
        FLEISCH,
        STEINE,
        WOLLE,
        BAUMWOLLE,
        WILD,
        GEWUERZBAUM,
        ZUCKERROHR,
    };

    //-------------------------------------------------
    // Maxprodcnt
    //-------------------------------------------------

    enum class MaxprodcntType
    {
        UNSET = 0,
        MAXPRODCNT
    };

    //-------------------------------------------------
    // BAUINFRA
    //-------------------------------------------------

    enum class BauinfraType
    {
        UNSET = 0,
        INFRA_STUFE_3F,
        INFRA_STUFE_1A,
        INFRA_STUFE_3B,
        INFRA_STUFE_3C,
        INFRA_KATHETRALE,
        INFRA_THEATER,
        INFRA_KIRCHE,
        INFRA_SCHULE,
        INFRA_STUFE_5A,
        INFRA_GALGEN,
        INFRA_SCHLOSS,
        INFRA_TRIUMPH,
        INFRA_STUFE_2E,
        INFRA_STUFE_2D,
        INFRA_STUFE_2G,
        INFRA_STUFE_2F,
        INFRA_STUFE_2A,
        INFRA_STUFE_2C,
        INFRA_STUFE_2B,
        INFRA_ARZT,
        INFRA_STUFE_4A,
        INFRA_WIRT,
        INFRA_BADE,
        INFRA_DENKMAL,
        INFRA_HOCHSCHULE
    };

    //-------------------------------------------------
    // Figurnr
    //-------------------------------------------------

    enum class FigurnrType
    {
        UNSET = 0,
        TRAEGER2,
        HOLZFAELLER,
        KANONTURM,
        FISCHER,
        STEINKLOPFER,
        PFLUECKER,
        ARZT,
        JAEGER,
        MAEHER,
        PIRATTURM,
        PFLUECKER2,
        FLEISCHER,
        KARREN,
        LOESCH,
        RIND,
        TRAEGER,
        SCHAF,
        PACKESEL,
        SPEER1
    };

    //-------------------------------------------------
    // Rauchfignr
    //-------------------------------------------------

    enum class RauchfignrType
    {
        UNSET = 0,
        FAHNETURM3,
        RAUCHWERKZEUG,
        RAUCHKANONEN,
        RAUCHGOLD,
        FAHNEKONTOR,
        RAUCHBAECK,
        FAHNEKONTOR3,
        FAHNEKONTOR2,
        FAHNEKONTOR1,
        RAUCHSCHWERT,
        RAUCHSCHMELZ,
        FAHNEKONTOR4,
        FAHNEMARKT,
        RAUCHBRENNER,
        FAHNETURM2,
        RAUCHMUSKET,
        FAHNETURM1
    };

    //-------------------------------------------------
    // Building
    //-------------------------------------------------

    /**
     * @brief Represents a graphic object on the isometric map.
     */
    struct Building
    {
        int32_t id{ -1 };
        int32_t gfx{ -1 };
        int32_t blocknr{ -1 };
        int32_t posoffs{ -1 };
        int32_t highflg{ -1 };
        int32_t einhoffs{ -1 };
        int32_t maxenergy{ -1 };
        int32_t maxbrand{ -1 };
        int32_t rotate{ -1 };
        int32_t randAnz{ -1 };
        int32_t animAnz{ 0 };
        int32_t animTime{ -1 };
        int32_t animFrame{ 0 };
        int32_t animAdd{ 0 };
        int32_t baugfx{ -1 };
        int32_t kreuzBase{ -1 };
        int32_t randwachs{ -1 };
        int32_t randAdd{ -1 };
        int32_t strandoff{ -1 };
        int32_t placeFlg{ -1 };
        int32_t noShotFlg{ -1 };
        int32_t strandflg{ -1 };
        int32_t ausbauflg{ -1 };
        int32_t tuerflg{ -1 };
        int32_t destroyflg{ -1 };

        KindType kind{ KindType::UNSET };                // WALD, RUINE, STRANDHAUS, MEER, FLUSS etc.
        BausampleType bausample{ BausampleType::UNSET }; // Wav samples
        RuinenrType ruinenr{ RuinenrType::UNSET };       // RUINE_KONTOR_N1, RUINE_MARKT, RUINE_ROAD_FELD etc.
        std::vector<int32_t> wegspeed = {};

        struct
        {
            int32_t w{ 0 };
            int32_t h{ 0 };
        } size;

        struct
        {
            int32_t bGruppe{ -1 };
            int32_t anicontflg{ -1 };
            int32_t makLagFlg{ -1 };
            int32_t nativflg{ -1 };
            int32_t lagAniFlg{ -1 };
            int32_t doerrflg{ -1 };
            int32_t noMoreWork{ -1 };
            int32_t workmenge{ -1 };
            int32_t noLagVoll{ -1 };
            int32_t radius{ -1 };
            int32_t rohmenge{ -1 };
            int32_t prodmenge{ -1 };
            int32_t randwachs{ -1 };
            int32_t maxlager{ -1 };
            int32_t maxnorohst{ -1 };
            int32_t arbeiter{ -1 };
            int32_t figuranz{ -1 };
            int32_t interval{ -1 };
            WorkstoffType workstoff{ WorkstoffType::UNSET };    // or HOLZ, STOFFE
            ErzbergnrType erzbergnr{ ErzbergnrType::UNSET };    // or ERZBERG_KLEIN, ERZBERG_GROSS
            ProdtypKindType kind{ ProdtypKindType::UNSET };     // BRUNNEN, THEATER, KLINIK, BERGWERK etc.
            WareType ware{ WareType::UNSET };                   // MEHL, GOLD, ERZE, ZIEGEL, MUSKETEN etc.
            RohstoffType rohstoff{ RohstoffType::UNSET };       // MEHL, GOLD, WILD, STEINE etc.
            MaxprodcntType maxprodcnt{ MaxprodcntType::UNSET }; // or MAXPRODCNT
            BauinfraType bauinfra{ BauinfraType::UNSET };       // INFRA_STUFE_1A, INFRA_STUFE_2A, INFRA_STUFE_2B etc.
            FigurnrType figurnr{ FigurnrType::UNSET };          // HOLZFAELLER, PFLUECKER, ARZT, JAEGER etc.
            RauchfignrType rauchfignr{ RauchfignrType::UNSET }; // FAHNETURM3, RAUCHGOLD, RAUCHBAECK etc.
            std::vector<int32_t> maxware = {};
            std::vector<int32_t> kosten = {};
        } houseProductionType;

        struct
        {
            int32_t money{ -1 };
            int32_t werkzeug{ -1 };
            int32_t holz{ -1 };
            int32_t ziegel{ -1 };
            int32_t kanon{ -1 };
        } houseBuildCosts;
    };

    //-------------------------------------------------
    // Buildings
    //-------------------------------------------------

    /**
     * @brief The decrypted content of the haeuser.cod.
     */
    class Buildings
    {
    public:
        /**
         * @brief The map with all building objects.
         *
         * The building ID is the key for each building.
         */
        std::map<int32_t, Building> buildingsMap;

        Buildings() = delete;

        /**
         * @brief Constructs a new Buildings object.
         *
         * @param t_codFilePath The path to the haeuser.cod.
         */
        explicit Buildings(const std::string& t_codFilePath);

        Buildings(const Buildings& t_other) = delete;
        Buildings(Buildings&& t_other) noexcept = delete;
        Buildings& operator=(const Buildings& t_other) = delete;
        Buildings& operator=(Buildings&& t_other) noexcept = delete;

        ~Buildings() noexcept;

    protected:

    private:
        /**
         * @brief Creates the Building objects.
         *
         * @param t_codFilePath The path to the haeuser.cod.
         */
        void GenerateBuildings(const std::string& t_codFilePath);

        /**
         * @brief Creates a single Building object.
         *
         * @param t_obj A Cod object.
         *
         * @return a Building object.
         */
        static Building GenerateBuilding(const cod_pb::Object* t_obj) ;
    };
}
