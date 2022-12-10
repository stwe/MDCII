// This file is part of the MDCII project.
//
// Copyright (c) 2022. stwe <https://github.com/stwe/MDCII>
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

#include "GridRenderer.h"
#include "RenderUtils.h"
#include "state/State.h"
#include "file/OriginalResourcesManager.h"
#include "camera/Camera.h"
#include "ogl/Window.h"
#include "ogl/resource/ResourceManager.h"
#include "ogl/resource/TextureUtils.h"
#include "ogl/buffer/Ssbo.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::renderer::GridRenderer::GridRenderer(std::shared_ptr<state::Context> t_context)
    : m_context{ std::move(t_context) }
{
    Log::MDCII_LOG_DEBUG("[GridRenderer::GridRenderer()] Create GridRenderer.");

    MDCII_ASSERT(m_context, "[GridRenderer::GridRenderer()] Null pointer.")

    Init();
}

mdcii::renderer::GridRenderer::~GridRenderer() noexcept
{
    Log::MDCII_LOG_DEBUG("[GridRenderer::~GridRenderer()] Destruct GridRenderer.");
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::renderer::GridRenderer::Render(const layer::GridLayer& t_gridLayer, const world::Zoom t_zoom, const world::Rotation t_rotation) const
{
    const auto zoomInt{ magic_enum::enum_integer(t_zoom) };
    const auto rotationInt{ magic_enum::enum_integer(t_rotation) };

    ogl::OpenGL::EnableAlphaBlending();

    const auto& shaderProgram{ ogl::resource::ResourceManager::LoadShaderProgram("shader/grid") };
    shaderProgram.Bind();

    shaderProgram.SetUniform("view", m_context->camera->GetViewMatrix());
    shaderProgram.SetUniform("projection", m_context->window->GetOrthographicProjectionMatrix());
    shaderProgram.SetUniform("diffuseMap", 0);
    shaderProgram.SetUniform("selected", false);

    m_vaos.at(zoomInt)->Bind();

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        MODEL_MATRICES_BINDING,
        t_gridLayer.modelMatricesSsbos.at(zoomInt).at(rotationInt)->id
    );

    const auto& textureId{ ogl::resource::ResourceManager::LoadTexture(m_gridFileNames.at(zoomInt)).id };
    ogl::resource::TextureUtils::BindForReading(textureId, GL_TEXTURE0);

    m_vaos.at(zoomInt)->DrawInstanced(t_gridLayer.instancesToRender);

    ogl::buffer::Vao::Unbind();

    ogl::OpenGL::DisableBlending();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::renderer::GridRenderer::Init()
{
    Log::MDCII_LOG_DEBUG("[GridRenderer::Init()] Starts initializing GridRenderer...");

    CreateVaos();

    magic_enum::enum_for_each<world::Zoom>([this](const world::Zoom t_zoom) {
        const auto zoomStr{ to_lower_case(std::string(magic_enum::enum_name<world::Zoom>(t_zoom))) };
        const auto fileName{ "textures/" + zoomStr + "/red_" + zoomStr + ".png" };
        m_gridFileNames.at(magic_enum::enum_integer(t_zoom)) = fileName;
    });

    Log::MDCII_LOG_DEBUG("[GridRenderer::Init()] The GridRenderer was initialized successfully.");
}

//-------------------------------------------------
// Create buffers
//-------------------------------------------------

void mdcii::renderer::GridRenderer::CreateVaos()
{
    Log::MDCII_LOG_DEBUG("[GridRenderer::CreateVaos()] Creates all Vaos.");

    magic_enum::enum_for_each<world::Zoom>([this](const world::Zoom t_zoom) {
        m_vaos.at(magic_enum::enum_integer(t_zoom)) = RenderUtils::CreateRectangleVao();
    });
}
