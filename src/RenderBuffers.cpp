#include <stddef.h>
#include <string.h>

#include "mine.h"
#include "dle-xp.h"
#include "glew.h"
#include "RenderBuffers.h"

// -----------------------------------------------------------------------------------

#define GL_DEPTH_STENCIL_EXT			0x84F9
#define GL_UNSIGNED_INT_24_8_EXT		0x84FA
#define GL_DEPTH24_STENCIL8_EXT		0x88F0
#define GL_TEXTURE_STENCIL_SIZE_EXT 0x88F1

GLuint CreateDepthTexture (int nType, int nWidth, int nHeight)
{
if (!(nWidth && nHeight))
	return 0;

GLuint	hDepthBuffer;

glEnable (GL_TEXTURE_2D);
glActiveTexture (GL_TEXTURE0);
glGenTextures (1, &hDepthBuffer);
if (glGetError ())
	return hDepthBuffer = 0;
glBindTexture (GL_TEXTURE_2D, hDepthBuffer);
glTexParameteri (GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
glTexParameteri (GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY); 	
if (nType == 1) // depth + stencil
	glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8_EXT, nWidth, nHeight, 0, GL_DEPTH_STENCIL_EXT, GL_UNSIGNED_INT_24_8_EXT, null);
else
	glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, nWidth, nHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, null);
GLenum nError;
if ((nError = glGetError ())) {
	glDeleteTextures (1, &hDepthBuffer);
	return hDepthBuffer = 0;
	}
glBindTexture (GL_TEXTURE_2D, 0);
return hDepthBuffer;
}

// -----------------------------------------------------------------------------------

GLuint CopyDepthTexture (GLuint hDestBuffer, int nType, int nWidth, int nHeight)
{
	static int nSamples = -1;
	static int nDuration = 0;

	GLenum nError = glGetError ();

glActiveTexture (GL_TEXTURE1);
glEnable (GL_TEXTURE_2D);
if (!(hDestBuffer || (hDestBuffer = CreateDepthTexture (nType, nWidth, nHeight))))
	return 0;
glBindTexture (GL_TEXTURE1, hDestBuffer);
glReadBuffer (GL_BACK);
glCopyTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, 0, 0, nWidth, nHeight);
if (glGetError ())
	return false;
#if 0
if (t > 0) {
	if (++nSamples > 0) {
		nDuration += SDL_GetTicks () - t;
		if ((nSamples >= 5) && (nDuration / nSamples > 10)) {
			PrintLog (0, "Disabling depth buffer reads (average read time: %d ms)\n", nDuration / nSamples);
			ogl.m_features.bDepthBlending = -1;
			DestroyDepthTexture (nId);
			hDepthBuffer = 0;
			}
		}
	}
#endif
return hDestBuffer;
}

// -----------------------------------------------------------------------------------

GLuint CreateColorTexture (int nWidth, int nHeight, bool bFBO)
{
if (!(nWidth && nHeight))
	return 0;

	GLuint	hColorBuffer;

if (GL_TEXTURE1 > GL_TEXTURE0)
	glActiveTexture (GL_TEXTURE1);
glEnable (GL_TEXTURE_2D);
glGenTextures (1, &hColorBuffer);
if (glGetError ())
	return hColorBuffer = 0;
glBindTexture (GL_TEXTURE1, hColorBuffer);
glTexParameteri (GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, nWidth, nHeight, 0, GL_RGB, GL_UNSIGNED_SHORT, null);
if (glGetError ()) {
	glDeleteTextures (1, &hColorBuffer);
	return hColorBuffer = 0;
	}
glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
if (!bFBO) {
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	glTexParameteri (GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
	}
if (glGetError ()) {
	glDeleteTextures (1, &hColorBuffer);
	return hColorBuffer = 0;
	}
return hColorBuffer;
}

// -----------------------------------------------------------------------------------

GLuint CopyColorTexture (GLuint hDestBuffer, int nWidth, int nHeight, bool bFBO)
{
	GLenum nError = glGetError ();

glActiveTexture (GL_TEXTURE1);
glEnable (GL_TEXTURE_2D);
if (!(hDestBuffer || (hDestBuffer = CreateColorTexture (nWidth, nHeight, bFBO))))
	return 0;
glBindTexture (GL_TEXTURE1, hDestBuffer);
glCopyTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, 0, 0, nWidth, nHeight);
return hDestBuffer;
}

// -----------------------------------------------------------------------------------

#if 0

GLuint COGL::CreateStencilTexture (int bFBO)
{
	GLuint	hBuffer;

if (GL_TEXTURE1 > 0)
	glActiveTexture (GL_TEXTURE1);
ogl.glEnable (GL_TEXTURE_2D);
GenTextures (1, &hBuffer);
if (glGetError ())
	return hDepthBuffer = 0;
ogl.BindTexture (hBuffer);
glTexParameteri (GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
glTexImage2D (GL_TEXTURE_2D, 0, GL_STENCIL_COMPONENT8, m_states.nCurWidth, m_states.nCurHeight,
				  0, GL_STENCIL_COMPONENT, GL_UNSIGNED_BYTE, null);
glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
if (glGetError ()) {
	DeleteTextures (1, &hBuffer);
	return hBuffer = 0;
	}
return hBuffer;
}

#endif

//------------------------------------------------------------------------------

