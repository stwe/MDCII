#include "Map.h"
#include "Log.h"
#include "data/HousesJsonFile.h"
#include "file/BshFile.h"
#include "renderer/TileRenderer.h"
#include "renderer/TextRenderer.h"
#include "renderer/Utils.h"
#include "ogl/resource/ResourceManager.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::map::Map::Map()
{
    Log::MDCII_LOG_DEBUG("[Map::Map()] Create Map.");

    Init();
}

mdcii::map::Map::~Map() noexcept
{
    Log::MDCII_LOG_DEBUG("[Map::~Map()] Destruct Map.");
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::map::Map::Render(const ogl::Window& t_window, const camera::Camera& t_camera) const
{
    if (rotation == Rotation::DEG0)
    {
        for (int y = 0; y < m_height; ++y)
        {
            for (int x = 0; x < m_width; ++x)
            {
                RenderMapContent(x, y, t_window, t_camera);
            }
        }
    }

    if (rotation == Rotation::DEG90)
    {
        for (int x = 0; x < m_width; ++x)
        {
            for (int y = m_height - 1; y >= 0; --y)
            {
                RenderMapContent(x, y, t_window, t_camera);
            }
        }
    }

    if (rotation == Rotation::DEG180)
    {
        for (int y = m_height - 1; y >= 0; --y)
        {
            for (int x = m_width - 1; x >= 0; --x)
            {
                RenderMapContent(x, y, t_window, t_camera);
            }
        }
    }

    if (rotation == Rotation::DEG270)
    {
        for (int x = m_width - 1; x >= 0; --x)
        {
            for (int y = 0; y < m_height; ++y)
            {
                RenderMapContent(x, y, t_window, t_camera);
            }
        }
    }
}

//-------------------------------------------------
// Rotate map
//-------------------------------------------------

void mdcii::map::Map::RotateMapCw()
{
    switch (rotation)
    {
    case Rotation::DEG0:
        rotation = Rotation::DEG90;
        break;
    case Rotation::DEG90:
        rotation = Rotation::DEG180;
        break;
    case Rotation::DEG180:
        rotation = Rotation::DEG270;
        break;
    case Rotation::DEG270:
        rotation = Rotation::DEG0;
        break;
    }
}

void mdcii::map::Map::RotateMapCcw()
{
    switch (rotation)
    {
    case Rotation::DEG0:
        rotation = Rotation::DEG270;
        break;
    case Rotation::DEG270:
        rotation = Rotation::DEG180;
        break;
    case Rotation::DEG180:
        rotation = Rotation::DEG90;
        break;
    case Rotation::DEG90:
        rotation = Rotation::DEG0;
        break;
    }
}

//-------------------------------------------------
// Utils
//-------------------------------------------------

glm::vec2 mdcii::map::Map::MapToIso(const int t_mapX, const int t_mapY)
{
    return {
        (t_mapX - t_mapY) << 5, // 32
        (t_mapX + t_mapY) << 4  // 16
    };
}

glm::vec2 mdcii::map::Map::MapToIso(const int t_mapX, const int t_mapY, const Rotation t_rotation) const
{
    auto xr{ t_mapX };
    auto yr{ t_mapY };

    if (t_rotation == Rotation::DEG90)
    {
        xr = m_width - t_mapY - 1;
        yr = t_mapX;
    }

    if (t_rotation == Rotation::DEG180)
    {
        xr = m_width - t_mapX - 1;
        yr = m_height - t_mapY - 1;
    }

    if (t_rotation == Rotation::DEG270)
    {
        xr = t_mapY;
        yr = m_height - t_mapX - 1;
    }

    return MapToIso(xr, yr);
}

//-------------------------------------------------
// Render helper
//-------------------------------------------------

void mdcii::map::Map::RenderMapContent(const int t_mapX, const int t_mapY, const ogl::Window& t_window, const camera::Camera& t_camera) const
{
    if (renderGrid)
    {
        RenderGridTile(t_mapX, t_mapY, t_window, t_camera);
    }

    if (renderBuildings)
    {
        RenderBuildingTile(t_mapX, t_mapY, t_window, t_camera);
    }

    if (renderText)
    {
        RenderText(t_mapX, t_mapY, t_window, t_camera);
    }
}

void mdcii::map::Map::RenderGridTile(const int t_mapX, const int t_mapY, const ogl::Window& t_window, const camera::Camera& t_camera) const
{
    renderer->RenderTile(
        renderer::Utils::GetModelMatrix(
            MapToIso(t_mapX, t_mapY, rotation),
            glm::vec2(64.0f, 32.0f)
        ),
        ogl::resource::ResourceManager::LoadTexture("resources/textures/red.png").id,
        t_window, t_camera
    );
}

void mdcii::map::Map::RenderBuildingTile(const int t_mapX, const int t_mapY, const ogl::Window& t_window, const camera::Camera& t_camera) const
{
    auto gfx{ m_map.at(GetMapIndex(t_mapX, t_mapY)) };

    gfx += static_cast<int>(rotation);

    const auto w{ static_cast<float>(stdBshFile->bshTextures[gfx]->width) };
    const auto h{ static_cast<float>(stdBshFile->bshTextures[gfx]->height) };

    auto screenPosition{ MapToIso(t_mapX, t_mapY, rotation) };
    screenPosition.y -= (h - 32.0f);

    if (gfx >= 4 && gfx <= 7)
    {
        screenPosition.y -= 20.0f;
    }

    renderer->RenderTile(
        renderer::Utils::GetModelMatrix(screenPosition, glm::vec2(w, h)),
        stdBshFile->bshTextures[gfx]->textureId,
        t_window,
        t_camera
    );
}

void mdcii::map::Map::RenderText(const int t_mapX, const int t_mapY, const ogl::Window& t_window, const camera::Camera& t_camera) const
{
    const auto screenPosition{ MapToIso(t_mapX, t_mapY, rotation) };

    textRenderer->RenderText(
        std::to_string(t_mapX).append(",  ").append(std::to_string(t_mapY)),
        screenPosition.x + 16, screenPosition.y + 8,
        0.25f,
        glm::vec3(1.0f, 0.0f, 0.0f),
        t_window,
        t_camera
    );
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::map::Map::Init()
{
    Log::MDCII_LOG_DEBUG("[Map::Init()] Initializing map.");

    // load houses.json file content
    housesJsonFile = std::make_unique<data::HousesJsonFile>();

    // load palette from stadtfld.col
    m_paletteFile = std::make_unique<file::PaletteFile>("STADTFLD.COL");
    m_paletteFile->ReadDataFromChunks();

    // load bsh graphics from stadtfld.bsh
    stdBshFile = std::make_unique<file::BshFile>("STADTFLD.BSH", m_paletteFile->palette);
    stdBshFile->ReadDataFromChunks();

    // load bsh graphics from bauhaus.bsh
    bauhausBshFile = std::make_unique<file::BshFile>("BAUHAUS.BSH", m_paletteFile->palette);
    bauhausBshFile->ReadDataFromChunks();

    // create tile renderer
    renderer = std::make_unique<renderer::TileRenderer>();

    // create text renderer
    textRenderer = std::make_unique<renderer::TextRenderer>("E:/Dev/MDCII/resources/bitter/Bitter-Regular.otf");

    Log::MDCII_LOG_DEBUG("[Map::Init()] The map was successfully initialized.");
}
