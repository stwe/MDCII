#pragma once

#include <map>
#include <vector>

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace cod_pb
{
    class Object;
}

namespace mdcii::data
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
     * Represents a graphic object on the isometric map.
     */
    struct Building
    {
        int id{ -1 };
        int gfx{ -1 };
        int blocknr{ -1 };
        int posoffs{ -1 };
        int highflg{ -1 };
        int einhoffs{ -1 };
        int maxenergy{ -1 };
        int maxbrand{ -1 };
        int rotate{ -1 };
        int randAnz{ -1 };
        int animAnz{ 0 };
        int animTime{ -1 };
        int animFrame{ 0 };
        int animAdd{ 0 };
        int baugfx{ -1 };
        int kreuzBase{ -1 };
        int randwachs{ -1 };
        int randAdd{ -1 };
        int strandoff{ -1 };
        int placeFlg{ -1 };
        int noShotFlg{ -1 };
        int strandflg{ -1 };
        int ausbauflg{ -1 };
        int tuerflg{ -1 };
        int destroyflg{ -1 };

        KindType kind{ KindType::UNSET };                // WALD, RUINE, STRANDHAUS, MEER, FLUSS etc.
        BausampleType bausample{ BausampleType::UNSET }; // Wav samples
        RuinenrType ruinenr{ RuinenrType::UNSET };       // RUINE_KONTOR_N1, RUINE_MARKT, RUINE_ROAD_FELD etc.
        std::vector<int> wegspeed = {};

        struct
        {
            int w{ 0 };
            int h{ 0 };
        } size;

        struct
        {
            int bGruppe{ -1 };
            int anicontflg{ -1 };
            int makLagFlg{ -1 };
            int nativflg{ -1 };
            int lagAniFlg{ -1 };
            int doerrflg{ -1 };
            int noMoreWork{ -1 };
            int workmenge{ -1 };
            int noLagVoll{ -1 };
            int radius{ -1 };
            int rohmenge{ -1 };
            int prodmenge{ -1 };
            int randwachs{ -1 };
            int maxlager{ -1 };
            int maxnorohst{ -1 };
            int arbeiter{ -1 };
            int figuranz{ -1 };
            int interval{ -1 };
            WorkstoffType workstoff{ WorkstoffType::UNSET };    // or HOLZ, STOFFE
            ErzbergnrType erzbergnr{ ErzbergnrType::UNSET };    // or ERZBERG_KLEIN, ERZBERG_GROSS
            ProdtypKindType kind{ ProdtypKindType::UNSET };     // BRUNNEN, THEATER, KLINIK, BERGWERK etc.
            WareType ware{ WareType::UNSET };                   // MEHL, GOLD, ERZE, ZIEGEL, MUSKETEN etc.
            RohstoffType rohstoff{ RohstoffType::UNSET };       // MEHL, GOLD, WILD, STEINE etc.
            MaxprodcntType maxprodcnt{ MaxprodcntType::UNSET }; // or MAXPRODCNT
            BauinfraType bauinfra{ BauinfraType::UNSET };       // INFRA_STUFE_1A, INFRA_STUFE_2A, INFRA_STUFE_2B etc.
            FigurnrType figurnr{ FigurnrType::UNSET };          // HOLZFAELLER, PFLUECKER, ARZT, JAEGER etc.
            RauchfignrType rauchfignr{ RauchfignrType::UNSET }; // FAHNETURM3, RAUCHGOLD, RAUCHBAECK etc.
            std::vector<int> maxware = {};
            std::vector<int> kosten = {};
        } houseProductionType;

        struct
        {
            int money{ -1 };
            int werkzeug{ -1 };
            int holz{ -1 };
            int ziegel{ -1 };
            int kanon{ -1 };
        } houseBuildCosts;
    };

    //-------------------------------------------------
    // Buildings
    //-------------------------------------------------

    /**
     * Parses the haeuser.cod and creates building objects from it.
     */
    class Buildings
    {
    public:
        /**
         * The map with all building objects.
         * The building ID is the key for each building.
         */
        std::map<int, Building> buildingsMap;

        /**
         * Constructs a new Buildings object.
         */
        Buildings();

    protected:

    private:
        /**
         * Creates the Building objects.
         */
        void GenerateBuildings();

        /**
         * Creates a single Building object.
         *
         * @param t_obj A Cod object.
         *
         * @return a Building object.
         */
        Building GenerateBuilding(const cod_pb::Object* t_obj) const;
    };
}
