#include <imgui.h>
#include <magic_enum.hpp>
#include "Map.h"
#include "Game.h"
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
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                RenderMapContent(x, y, t_window, t_camera);
            }
        }
    }

    if (rotation == Rotation::DEG90)
    {
        for (int x = 0; x < width; ++x)
        {
            for (int y = height - 1; y >= 0; --y)
            {
                RenderMapContent(x, y, t_window, t_camera);
            }
        }
    }

    if (rotation == Rotation::DEG180)
    {
        for (int y = height - 1; y >= 0; --y)
        {
            for (int x = width - 1; x >= 0; --x)
            {
                RenderMapContent(x, y, t_window, t_camera);
            }
        }
    }

    if (rotation == Rotation::DEG270)
    {
        for (int x = width - 1; x >= 0; --x)
        {
            for (int y = 0; y < height; ++y)
            {
                RenderMapContent(x, y, t_window, t_camera);
            }
        }
    }
}

void mdcii::map::Map::RenderImGui()
{
    ImGui::Begin("Map", nullptr, 0);

    // render options

    ImGui::Checkbox("Render grid", &renderGrid);
    ImGui::Checkbox("Render buildings", &renderBuildings);
    ImGui::Checkbox("Render text", &renderText);

    ImGui::Separator();

    // rotation

    ImGui::Text("Current rotation: %s", magic_enum::enum_name(rotation).data());
    if (ImGui::Button("Rotate right"))
    {
        ++rotation;
    }
    if (ImGui::Button("Rotate left"))
    {
        --rotation;
    }

    ImGui::Separator();

    // selected

    if (selectedId == INVALID_GFX_ID)
    {
        ImGui::Text("Current selected Id: nothing selected");
    }
    else
    {
        ImGui::Text("Current selected Id: %d", selectedId);
    }

    ImGui::End();
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

glm::vec2 mdcii::map::Map::MapToIso(const int t_mapX, const int t_mapY) const
{
    auto xr{ t_mapX };
    auto yr{ t_mapY };

    if (rotation == Rotation::DEG90)
    {
        xr = width - t_mapY - 1;
        yr = t_mapX;
    }

    if (rotation == Rotation::DEG180)
    {
        xr = width - t_mapX - 1;
        yr = height - t_mapY - 1;
    }

    if (rotation == Rotation::DEG270)
    {
        xr = t_mapY;
        yr = height - t_mapX - 1;
    }

    return {
        (xr - yr) * TILE_WIDTH_HALF,
        (xr + yr) * TILE_HEIGHT_HALF
    };
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
            MapToIso(t_mapX, t_mapY),
            glm::vec2(TILE_WIDTH, TILE_HEIGHT)
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

    auto screenPosition{ MapToIso(t_mapX, t_mapY) };
    screenPosition.y -= (h - TILE_HEIGHT);

    // todo: temp hardcoded
    if (gfx >= 4 && gfx <= 7)
    {
        screenPosition.y -= ELEVATION;
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
    const auto screenPosition{ MapToIso(t_mapX, t_mapY) };

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
    textRenderer = std::make_unique<renderer::TextRenderer>(Game::RESOURCES_PATH + "bitter/Bitter-Regular.otf");

    Log::MDCII_LOG_DEBUG("[Map::Init()] The map was successfully initialized.");
}
