#include "Log.h"
#include "Game.h"
#include "MdciiException.h"

//-------------------------------------------------
// Main
//-------------------------------------------------

int main()
{
    mdcii::Log::Init();

    mdcii::Log::MDCII_LOG_DEBUG("[main()] Starting main.");
    mdcii::Log::MDCII_LOG_DEBUG("[main()] Logger was initialized.");

    try
    {
        mdcii::Game game;
        game.Run();

        return EXIT_SUCCESS;
    }
    catch (const mdcii::MdciiException& e)
    {
        mdcii::Log::MDCII_LOG_ERROR("MdciiException {}", e.what());
    }
    catch (const std::exception& e)
    {
        mdcii::Log::MDCII_LOG_ERROR("Standard Exception: {}", e.what());
    }
    catch (...)
    {
        mdcii::Log::MDCII_LOG_ERROR("Unknown Exception. No details available.");
    }

    return EXIT_FAILURE;
}
