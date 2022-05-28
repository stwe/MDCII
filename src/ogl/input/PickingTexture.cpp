#include "PickingTexture.h"
#include "MdciiException.h"
#include "MdciiAssert.h"
#include "ogl/OpenGL.h"
#include "ogl/resource/Texture.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::ogl::input::PickingTexture::PickingTexture(const int t_width, const int t_height)
    : m_width{ t_width }
    , m_height{ t_height }
{
    MDCII_ASSERT(m_width, "[PickingTexture::PickingTexture()] Invalid width.")
    MDCII_ASSERT(m_height, "[PickingTexture::PickingTexture()] Invalid  height.")

    Log::MDCII_LOG_DEBUG("[PickingTexture::PickingTexture()] Create PickingTexture.");

    Init();
}

mdcii::ogl::input::PickingTexture::~PickingTexture() noexcept
{
    Log::MDCII_LOG_DEBUG("[PickingTexture::~PickingTexture()] Destruct PickingTexture.");

    CleanUp();
}

//-------------------------------------------------
// Write / render to texture
//-------------------------------------------------

void mdcii::ogl::input::PickingTexture::EnableWriting() const
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fboId);
}

void mdcii::ogl::input::PickingTexture::DisableWriting()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

//-------------------------------------------------
// Read from texture
//-------------------------------------------------

void mdcii::ogl::input::PickingTexture::EnableReading() const
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fboId);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
}

void mdcii::ogl::input::PickingTexture::DisableReading()
{
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

int mdcii::ogl::input::PickingTexture::ReadMapIndex(const int t_x, const int t_y) const
{
    EnableReading();

    unsigned char rgbaData[4];
    glReadPixels(t_x, m_height - t_y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, rgbaData);

    DisableReading();

    // convert the color back to an Id/index
    return rgbaData[0] +
           rgbaData[1] * 256 +
           rgbaData[2] * 256 * 256;
}

glm::vec3 mdcii::ogl::input::PickingTexture::CreateIdColor(const int t_id)
{
    // convert Id/index into an RGB color
    const int g{ (t_id & 0x0000FF00) >> 8 };
    const int r{ (t_id & 0x000000FF) >> 0 };
    const int b{ (t_id & 0x00FF0000) >> 16 };

    return { static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f };
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::ogl::input::PickingTexture::Init()
{
    // create Fbo handle
    CreateId();

    // bind Fbo
    Bind();

    // create the texture object for the primitive information buffer
    glGenTextures(1, &m_pickingTextureId);
    glBindTexture(GL_TEXTURE_2D, m_pickingTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pickingTextureId, 0);

    // disable reading to avoid problems with older GPUs
    glReadBuffer(GL_NONE);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    // verify that the Fbo is correct
    const auto status{ glCheckFramebufferStatus(GL_FRAMEBUFFER) };
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        throw MDCII_EXCEPTION("[PickingTexture::Init()] Error while creating Fbo attachments.");
    }

    // restore the default framebuffer
    resource::Texture::Unbind();
    Unbind();
}

//-------------------------------------------------
// Create
//-------------------------------------------------

void mdcii::ogl::input::PickingTexture::CreateId()
{
    glGenFramebuffers(1, &m_fboId);
    MDCII_ASSERT(m_fboId, "[PickingTexture::CreateId()] Error while creating a new Fbo.")

    Log::MDCII_LOG_DEBUG("[PickingTexture::CreateId()] A new Fbo was created. The Id is {}.", m_fboId);
}

//-------------------------------------------------
// Bind / unbind
//-------------------------------------------------

void mdcii::ogl::input::PickingTexture::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);
}

void mdcii::ogl::input::PickingTexture::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::ogl::input::PickingTexture::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[PickingTexture::CleanUp()] Clean up Fbo Id {}.", m_fboId);

    Unbind();

    if (m_fboId)
    {
        glDeleteFramebuffers(1, &m_fboId);
        Log::MDCII_LOG_DEBUG("[PickingTexture::CleanUp()] Fbo Id {} was deleted.", m_fboId);
    }

    if (m_pickingTextureId)
    {
        glDeleteTextures(1, &m_pickingTextureId);
        Log::MDCII_LOG_DEBUG("[PickingTexture::CleanUp()] Picking texture Id {} was deleted.", m_pickingTextureId);
    }
}
