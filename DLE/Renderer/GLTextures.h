#pragma once

class GLTextureHelpers
{
public:
    static bool GLCreateTexture(CTexture* texture, bool bForce);
    static GLuint GLBindTexture(const CTexture* texture, GLuint nTMU, GLuint nMode);
    static void GLReleaseTexture(CTexture* texture);
    static void CreateGLTextures(int nVersion = -1);
};
