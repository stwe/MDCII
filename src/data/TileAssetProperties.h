#pragma once

//-------------------------------------------------
// TileAssetProperties
//-------------------------------------------------

namespace mdcii::data
{
    /**
     * Represents tile asset properties.
     */
    struct TileAssetProperties
    {
        int id{ -1 };
        int gfx{ -1 };
        int blocknr{ -1 };
        std::string_view kind{};
        int posoffs{ -1 };
        int highflg{ -1 };
        int einhoffs{ -1 };
        int maxenergy{ -1 };
        int maxbrand{ -1 };
        int width{ -1 };
        int height{ -1 };
        int rotate{ -1 };
        int randAnz{ -1 };
        int randAdd{ -1 };
        int animTime{ -1 };
        int animFrame{ 0 };
        int animAdd{ 0 };
        int baugfx{ -1 };
        int placeFlg{ -1 };
        int animAnz{ 0 };

        // todo
        int randwachs{ -1 };
        int strandoff{ -1 };
        int kreuzBase{ -1 };
        int noShotFlg{ -1 };
        int strandflg{ -1 };
        int ausbauflg{ -1 };
        int tuerflg{ -1 };
        int destroyflg{ -1 };
    };
}
