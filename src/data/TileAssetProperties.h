#pragma once

//-------------------------------------------------
// TileAssetProperties
//-------------------------------------------------

namespace mdcii::data
{
    /**
     * The hard-coded tile groups were read from the houses.json file.
     */
    enum class TileKind
    {
        UNUSED,
        BODEN,
        WALD,
        RUINE,
        STRANDRUINE,
        HANG,
        MEER,
        BRANDUNG,
        BRANDECK,
        HANGQUELL,
        HANGECK,
        MUENDUNG,
        STRAND,
        STRANDVARI,
        STRANDECKI,
        STRANDECKA,
        STRANDMUND,
        PIER,
        FELS,
        FLUSS,
        FLUSSECK,
        MAUER,
        MAUERSTRAND,
        TURM,
        TURMSTRAND,
        STRASSE,
        BRUECKE,
        PLATZ,
        TOR,
        STRANDHAUS,
        HQ,
        HAFEN,
        GEBAEUDE,
        WMUEHLE,
        MINE,
    };

    /**
     * Represents tile asset properties.
     * Values read from the houses.json file.
     */
    struct TileAssetProperties
    {
        static constexpr auto INVALID{ -1 };

        int id{ INVALID };
        int gfx{ INVALID };
        int blocknr{ INVALID };
        TileKind kind{ TileKind::UNUSED };
        int posoffs{ INVALID };
        int highflg{ INVALID };
        int einhoffs{ INVALID };
        int maxenergy{ INVALID };
        int maxbrand{ INVALID };
        int width{ INVALID };
        int height{ INVALID };
        int rotate{ INVALID };
        int randAnz{ INVALID };
        int randAdd{ INVALID };
        int animTime{ INVALID };
        int animFrame{ 0 };
        int animAdd{ 0 };
        int baugfx{ INVALID };
        int placeFlg{ INVALID };
        int animAnz{ 0 };

        // todo
        int randwachs{ INVALID };
        int strandoff{ INVALID };
        int kreuzBase{ INVALID };
        int noShotFlg{ INVALID };
        int strandflg{ INVALID };
        int ausbauflg{ INVALID };
        int tuerflg{ INVALID };
        int destroyflg{ INVALID };
    };
}
