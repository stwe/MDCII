#pragma once

#include <map>
#include <string>
#include "TileAsset.h"

//-------------------------------------------------
// HousesJsonFile
//-------------------------------------------------

namespace mdcii::data
{
    /**
     * Reads the HAUS objects from the houses.json file.
     * The houses.json file contains the content from the
     * original game's haeuser.cod as json.
     */
    class HousesJsonFile
    {
    public:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * The path to the houses.json file.
         */
        inline static const std::string HOUSES_JSON_FILE_PATH{ "data/houses.json" };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The HAUS objects sorted by Id.
         */
        std::map<int, TileAsset> tileAssets;

        /**
         * The HAUS objects sorted by TileKind (e.g. BODEN, WALD, HANG, FELS etc).
         */
        std::multimap<TileKind, TileAsset> tileAssetsByKind;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        HousesJsonFile();

        HousesJsonFile(const HousesJsonFile& t_other) = delete;
        HousesJsonFile(HousesJsonFile&& t_other) noexcept = delete;
        HousesJsonFile& operator=(const HousesJsonFile& t_other) = delete;
        HousesJsonFile& operator=(HousesJsonFile&& t_other) noexcept = delete;

        ~HousesJsonFile() noexcept;

        //-------------------------------------------------
        // Read
        //-------------------------------------------------

        /**
         * Reads the houses.json file and creates TileAssetProperties objects.
         */
        void ReadFileData();

    protected:

    private:

    };
}
