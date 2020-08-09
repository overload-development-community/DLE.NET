#include "stdafx.h"
#include "GLTextures.h"

bool GLTextureHelpers::GLCreateTexture(CTexture* texture, bool bForce)
{
    if (texture->m_glHandle)
        if (!bForce)
            return true;
        else
            GLReleaseTexture(texture);

    glGenTextures(1, &texture->m_glHandle);
    if (!texture->m_glHandle)
        return false;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture->m_glHandle);
    // don't use height: Hires animations have all frames vertically stacked in one texture!
    glTexImage2D(GL_TEXTURE_2D, 0, 4, texture->RenderWidth(), texture->RenderHeight(), 0, GL_BGRA, GL_UNSIGNED_BYTE, texture->RenderBuffer());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glDisable(GL_TEXTURE_2D);
    return true;
}

GLuint GLTextureHelpers::GLBindTexture(const CTexture* texture, GLuint nTMU, GLuint nMode)
{
    glActiveTexture(nTMU);
    glClientActiveTexture(nTMU);
    glEnable(GL_TEXTURE_2D);
    if (texture->m_glHandle)
        glBindTexture(GL_TEXTURE_2D, texture->m_glHandle);
    else
        return 0;
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, nMode);
    return texture->m_glHandle;
}

void GLTextureHelpers::GLReleaseTexture(CTexture* texture)
{
    if (texture->m_glHandle) {
        glDeleteTextures(1, &texture->m_glHandle);
        texture->m_glHandle = 0;
    }
}

void GLTextureHelpers::CreateGLTextures(int nVersion)
{
    int nVersionResolved = (nVersion < 0) ? textureManager.Version() : nVersion;

    if (!textureManager.Available(nVersionResolved))
        return;

    for (int i = 0; i < textureManager.GlobalTextureCount(); i++)
        GLCreateTexture(textureManager.TextureByIndex(i, nVersion), false);
    GLCreateTexture(&textureManager.Arrow(), false);
    for (int i = 0; i < ICON_COUNT; i++)
        GLCreateTexture(&textureManager.Icon(i), false);
}
