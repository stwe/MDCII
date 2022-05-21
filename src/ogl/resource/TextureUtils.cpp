#include "TextureUtils.h"
#include "MdciiAssert.h"
#include "ogl/OpenGL.h"

//-------------------------------------------------
// Generate
//-------------------------------------------------

uint32_t mdcii::ogl::resource::TextureUtils::GenerateNewTextureId()
{
    uint32_t textureId{ 0 };

    glGenTextures(1, &textureId);
    MDCII_ASSERT(textureId, "[TextureUtils::GenerateNewTextureId()] Invalid texture Id.")

    return textureId;
}

//-------------------------------------------------
// Filter
//-------------------------------------------------

void mdcii::ogl::resource::TextureUtils::UseNoFilter(const uint32_t t_target)
{
    glTexParameteri(t_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(t_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void mdcii::ogl::resource::TextureUtils::UseNoFilter()
{
    UseNoFilter(GL_TEXTURE_2D);
}

void mdcii::ogl::resource::TextureUtils::UseBilinearFilter(const uint32_t t_target)
{
    glTexParameteri(t_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(t_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void mdcii::ogl::resource::TextureUtils::UseBilinearFilter()
{
    UseBilinearFilter(GL_TEXTURE_2D);
}

void mdcii::ogl::resource::TextureUtils::UseBilinearMipmapFilter(const uint32_t t_target)
{
    glTexParameteri(t_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(t_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

void mdcii::ogl::resource::TextureUtils::UseBilinearMipmapFilter()
{
    UseBilinearMipmapFilter(GL_TEXTURE_2D);
}

//-------------------------------------------------
// Wrapping
//-------------------------------------------------

void mdcii::ogl::resource::TextureUtils::UseRepeatWrapping(const uint32_t t_target)
{
    glTexParameteri(t_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(t_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void mdcii::ogl::resource::TextureUtils::UseRepeatWrapping()
{
    UseRepeatWrapping(GL_TEXTURE_2D);
}

void mdcii::ogl::resource::TextureUtils::UseClampToEdgeWrapping(const uint32_t t_target)
{
    glTexParameteri(t_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(t_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(t_target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void mdcii::ogl::resource::TextureUtils::UseClampToEdgeWrapping()
{
    UseClampToEdgeWrapping(GL_TEXTURE_2D);
}

//-------------------------------------------------
// Bind / Unbind
//-------------------------------------------------

void mdcii::ogl::resource::TextureUtils::Bind(const uint32_t t_textureId, const uint32_t t_target)
{
    MDCII_ASSERT(t_textureId, "[TextureUtils::Bind()] Invalid texture Id.")
    glBindTexture(t_target, t_textureId);
}

void mdcii::ogl::resource::TextureUtils::Bind(const uint32_t t_textureId)
{
    Bind(t_textureId, GL_TEXTURE_2D);
}

void mdcii::ogl::resource::TextureUtils::Unbind(const uint32_t t_target)
{
    glBindTexture(t_target, 0);
}

void mdcii::ogl::resource::TextureUtils::Unbind()
{
    Unbind(GL_TEXTURE_2D);
}

void mdcii::ogl::resource::TextureUtils::BindForReading(const uint32_t t_textureId, const uint32_t t_textureUnit, const uint32_t t_target)
{
    // make sure that the OpenGL constants are used here
    MDCII_ASSERT(t_textureUnit >= GL_TEXTURE0 && t_textureUnit <= GL_TEXTURE15, "[TextureUtils::BindForReading()] Invalid texture unit value.")

    glActiveTexture(t_textureUnit);
    Bind(t_textureId, t_target);
}

void mdcii::ogl::resource::TextureUtils::BindForReading(const uint32_t t_textureId, const uint32_t t_textureUnit)
{
    BindForReading(t_textureId, t_textureUnit, GL_TEXTURE_2D);
}
