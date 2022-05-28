#include <imgui.h>
#include "TileRenderer.h"
#include "MdciiAssert.h"
#include "ogl/OpenGL.h"
#include "ogl/resource/ResourceManager.h"
#include "ogl/resource/TextureUtils.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::renderer::TileRenderer::TileRenderer()
{
    Log::MDCII_LOG_DEBUG("[TileRenderer::TileRenderer()] Create TileRenderer.");

    Init();
}

mdcii::renderer::TileRenderer::~TileRenderer() noexcept
{
    Log::MDCII_LOG_DEBUG("[TileRenderer::~TileRenderer()] Destruct TileRenderer.");
}

//-------------------------------------------------
// Render
//-------------------------------------------------

void mdcii::renderer::TileRenderer::RenderTile(
    const glm::mat4& t_modelMatrix,
    const uint32_t t_bshTextureId,
    const ogl::Window& t_window,
    const camera::Camera& t_camera
) const
{
    ogl::OpenGL::EnableAlphaBlending();

    const auto& shaderProgram{ ogl::resource::ResourceManager::LoadShaderProgram("resources/shader/mesh") };
    shaderProgram.Bind();

    shaderProgram.SetUniform("model", t_modelMatrix);
    shaderProgram.SetUniform("view", t_camera.GetViewMatrix());
    shaderProgram.SetUniform("projection", t_window.GetOrthographicProjectionMatrix());
    shaderProgram.SetUniform("diffuseMap", 0);

    glBindVertexArray(m_vao);
    ogl::resource::TextureUtils::BindForReading(t_bshTextureId, GL_TEXTURE0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    ogl::OpenGL::DisableBlending();
}

void mdcii::renderer::TileRenderer::RenderTileGfxImGui(
    const data::TileAssetProperties& t_tileAssetProperties,
    const file::BshFile& t_bshFile
)
{
    if (const auto gfx{ t_tileAssetProperties.gfx }; gfx >= 0)
    {
        MDCII_ASSERT(static_cast<size_t>(gfx) < t_bshFile.bshTextures.size(), "[TileRenderer::RenderTileGfxImGui()] Invalid arguments given.")

        ImGui::Text("Gfx: %d", gfx);

        const auto textureWidth{ t_bshFile.bshTextures.at(gfx)->width };
        MDCII_ASSERT(textureWidth, "[TileRenderer::RenderTileGfxImGui()] Invalid texture width.")

        const auto textureHeight{ t_bshFile.bshTextures.at(gfx)->height };
        MDCII_ASSERT(textureHeight, "[TileRenderer::RenderTileGfxImGui()] Invalid texture height.")

        const auto textureId{ reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(t_bshFile.bshTextures.at(gfx)->textureId)) };

        ImGui::Image(
            textureId,
            ImVec2(static_cast<float>(textureWidth), static_cast<float>(textureHeight)),
            ImVec2(0.0f, 0.0f),
            ImVec2(1.0f, 1.0f),
            ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
            ImVec4(0.6f, 0.6f, 0.6f, 1.0f)
        );
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
        ImGui::Text("Invalid Gfx: %d", gfx);
        ImGui::PopStyleColor();
    }
}

void mdcii::renderer::TileRenderer::RenderTileBauGfxImGui(const data::TileAssetProperties& t_tileAssetProperties, const file::BshFile& t_bshFile)
{
    if (const auto gfx{ t_tileAssetProperties.gfx }; gfx >= 0)
    {
        if (const auto baugfx{ t_tileAssetProperties.baugfx }; baugfx >= 0)
        {
            MDCII_ASSERT(static_cast<size_t>(baugfx) < t_bshFile.bshTextures.size(), "[TileRenderer::RenderTileBauGfxImGui()] Invalid arguments given.")

            ImGui::Text("BauGfx: %d", baugfx);

            const auto textureWidth{ t_bshFile.bshTextures.at(baugfx)->width };
            MDCII_ASSERT(textureWidth, "[TileRenderer::RenderTileBauGfxImGui()] Invalid texture width.")

            const auto textureHeight{ t_bshFile.bshTextures.at(baugfx)->height };
            MDCII_ASSERT(textureHeight, "[TileRenderer::RenderTileBauGfxImGui()] Invalid texture height.")

            const auto textureId{ reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(t_bshFile.bshTextures.at(baugfx)->textureId)) };

            ImGui::Image(
                textureId,
                ImVec2(static_cast<float>(textureWidth), static_cast<float>(textureHeight)),
                ImVec2(0.0f, 0.0f),
                ImVec2(1.0f, 1.0f),
                ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
                ImVec4(0.6f, 0.6f, 0.6f, 1.0f)
            );
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
            ImGui::Text("Invalid BauGfx: %d", baugfx);
            ImGui::PopStyleColor();
        }
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
        ImGui::Text("Invalid Gfx: %d", gfx);
        ImGui::PopStyleColor();
    }
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::renderer::TileRenderer::Init()
{
    constexpr float vertices[]
    {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &m_vao);

    uint32_t vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(m_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
