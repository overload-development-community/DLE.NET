#ifndef __RENDERBUFFERS_H
#define __RENDERBUFFERS_H

#include "glew.h"

GLuint CreateDepthTexture (int nType, int nWidth, int hHeight);
void DestroyDepthTexture (void);
GLuint CopyDepthTexture (GLuint hDestBuffer, int nType, int nWidth, int nHeight);
GLuint CreateColorTexture (int nWidth, int nHeight, bool bFBO = false);
void DestroyColorTexture (void);
GLuint CopyColorTexture (GLuint hDestBuffer, int nWidth, int nHeight, bool bFBO);

#endif //__RENDERBUFFERS_H
