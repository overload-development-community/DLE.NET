#include <stddef.h>
#include <string.h>

#include "mine.h"
#include "dle-xp.h"
#include "glew.h"
#include "textures.h"
#include "RenderBuffers.h"
#include "FBO.h"

//------------------------------------------------------------------------------

int CFBO::Available (void)
{
if (!Handle ())
	return 0;
switch (m_info.nStatus = glCheckFramebufferStatusEXT (GL_FRAMEBUFFER_EXT)) {                                          
	case GL_FRAMEBUFFER_COMPLETE_EXT:                       
		return 1;
	case GL_FRAMEBUFFER_UNSUPPORTED_EXT:                    
		return -1;
	default:                                                
		return -1;
	}
}

//------------------------------------------------------------------------------

void CFBO::Init (void)
{
memset (&m_info, 0, sizeof (m_info));
m_info.nBuffer = 0x7FFFFFFF;
}

//------------------------------------------------------------------------------

int CFBO::CreateColorBuffers (int nBuffers)
{
if (!nBuffers)
	return 1;

	GLint	nMaxBuffers;

glGetIntegerv (GL_MAX_COLOR_ATTACHMENTS_EXT, &nMaxBuffers);
if (nMaxBuffers > MAX_COLOR_BUFFERS)
	nMaxBuffers = MAX_COLOR_BUFFERS;
else if (nBuffers > nMaxBuffers)
	nBuffers = nMaxBuffers;
m_info.nColorBuffers = nBuffers;
glGenTextures (nBuffers, m_info.hColorBuffers);
glEnable (GL_TEXTURE_2D);
glActiveTexture (GL_TEXTURE0);
for (int i = 0; i < nBuffers; i++) {
	glBindTexture (GL_TEXTURE_2D, m_info.hColorBuffers [i]);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); 
	glTexParameteri (GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, m_info.nWidth, m_info.nHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, null);
	m_info.bufferIds [i] = GL_COLOR_ATTACHMENT0_EXT + i;
	}
return glGetError () ? 0 : 1;
}

//------------------------------------------------------------------------------

int CFBO::CreateDepthBuffer (void)
{
// depth buffer
if (!(m_info.hDepthBuffer = CreateDepthTexture (m_info.nType, m_info.nWidth, m_info.nHeight)))
	return 0;
glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, m_info.hDepthBuffer, 0);
if (m_info.nType) // stencil buffer
	glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, m_info.hStencilBuffer = m_info.hDepthBuffer, 0);
return glGetError () ? 0 : 1;
}

//------------------------------------------------------------------------------

void CFBO::AttachBuffers (void)
{
for (int i = 0; i < m_info.nColorBuffers; i++)
	glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, m_info.bufferIds [i], GL_TEXTURE_2D, m_info.hColorBuffers [i], 0);
// depth + stencil buffer
glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, m_info.hDepthBuffer, 0);
if (m_info.nType)
	glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, m_info.hStencilBuffer = m_info.hDepthBuffer, 0);
}

//------------------------------------------------------------------------------

int CFBO::Create (int nWidth, int nHeight, int nType, int nColorBuffers)
{
Destroy ();
m_info.nWidth = Pow2ize (nWidth);
m_info.nHeight = Pow2ize (nHeight);

m_info.nType = nType;
m_info.hDepthBuffer = 0;
m_info.hStencilBuffer = 0;

glGenFramebuffersEXT (1, &m_info.hFBO);
glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, m_info.hFBO);

if (!(CreateColorBuffers (nColorBuffers) && CreateDepthBuffer ())) {
	Destroy ();
	return 0;
	}
AttachBuffers ();
glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
if  (!Available ()) {
	Destroy ();
	return 0;
	}
return 1;
}

//------------------------------------------------------------------------------

void CFBO::Destroy (void)
{
if (m_info.hFBO) {
	if (m_info.nColorBuffers) {
		glDeleteTextures (m_info.nColorBuffers, m_info.hColorBuffers);
		memset (m_info.hColorBuffers, 0, sizeof (m_info.hColorBuffers));
		m_info.nColorBuffers = 0;
		}
	if (m_info.hDepthBuffer) {
		glDeleteTextures (1, &m_info.hDepthBuffer);
		glDeleteRenderbuffersEXT (1, &m_info.hDepthBuffer);
		m_info.hDepthBuffer =
		m_info.hStencilBuffer = 0;
		}
	glDeleteFramebuffersEXT (1, &m_info.hFBO);
	m_info.hFBO = 0;
	}
}

//------------------------------------------------------------------------------

void CFBO::SelectColorBuffers (int nBuffer) 
{ 
if ((m_info.nColorBuffers == 1) || (nBuffer >= m_info.nColorBuffers)) 
	glDrawBuffer (m_info.bufferIds [nBuffer = 0]); // only one buffer or invalid buffer index
else if (nBuffer < 0)
	glDrawBuffers (m_info.nColorBuffers, m_info.bufferIds); // use all available color buffers
else
	glDrawBuffer (m_info.bufferIds [nBuffer]); // use the specified color buffer
m_info.nBuffer = nBuffer;
}

//------------------------------------------------------------------------------

int CFBO::Enable (int nColorBuffers)
{
if (m_info.bActive)
	return 1;
glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, m_info.hFBO);
SelectColorBuffers (nColorBuffers);
return m_info.bActive = 1;
}

//------------------------------------------------------------------------------

int CFBO::Disable (void)
{
if (!m_info.bActive)
	return 1;
m_info.nBuffer = 0x7FFFFFFF;
glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
glDrawBuffer (GL_BACK);
glReadBuffer (GL_BACK);
m_info.bActive = 0;
return 1;
}

//------------------------------------------------------------------------------

void CFBO::Flush (CRect viewport)
{
	float uMax = float (viewport.Width ()) / float (m_info.nWidth);
	float vMax = float (viewport.Height ()) / float (m_info.nHeight);
	float texCoord [4][2] = {{0.0, 0.0}, {0.0, vMax}, {uMax, vMax}, {uMax, 0.0}};
	float vertices [4][2] = {{0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}};

glMatrixMode (GL_PROJECTION);
glLoadIdentity ();
glOrtho (0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
glMatrixMode (GL_MODELVIEW);
glLoadIdentity ();

glActiveTexture (GL_TEXTURE0);
glClientActiveTexture (GL_TEXTURE0);
glEnable (GL_TEXTURE_2D);
glEnableClientState (GL_TEXTURE_COORD_ARRAY);
glEnableClientState (GL_VERTEX_ARRAY);
glTexCoordPointer (2, GL_FLOAT, 0, texCoord);
glVertexPointer (2, GL_FLOAT, 0, vertices);
glBindTexture (GL_TEXTURE_2D, m_info.hColorBuffers [0]);
glColor3f (1.0f, 1.0f, 1.0f);
glDisable (GL_BLEND);
glDrawArrays (GL_QUADS, 0, 4);
glDisableClientState (GL_TEXTURE_COORD_ARRAY);
glDisableClientState (GL_VERTEX_ARRAY);
}

//------------------------------------------------------------------------------

bool CFBO::Setup (void)
{
#ifdef _DEBUG
if (!(glDrawBuffers = (PFNGLDRAWBUFFERSPROC) wglGetProcAddress ("glDrawBuffers")))
	return false; //PrintLog (0, "glDrawBuffers not supported by the OpenGL driver\n");
else if (!(glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC) wglGetProcAddress ("glBindRenderbufferEXT")))
	return false; //PrintLog (0, "glBindRenderbufferEXT not supported by the OpenGL driver\n");
else if (!(glIsRenderbufferEXT = (PFNGLISRENDERBUFFEREXTPROC) wglGetProcAddress ("glIsRenderbufferEXT")))
	return false; //PrintLog (0, "glIsRenderbufferEXT not supported by the OpenGL driver\n");
else if (!(glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC) wglGetProcAddress ("glDeleteRenderbuffersEXT")))
	return false; //PrintLog (0, "glDeleteRenderbuffersEXT not supported by the OpenGL driver\n");
else if (!(glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC) wglGetProcAddress ("glGenRenderbuffersEXT")))
	return false; //PrintLog (0, "glGenRenderbuffersEXT not supported by the OpenGL driver\n");
else if (!(glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC) wglGetProcAddress ("glRenderbufferStorageEXT")))
	return false; //PrintLog (0, "glRenderbufferStorageEXT not supported by the OpenGL driver\n");
else if (!(glGetRenderbufferParameterivEXT = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC) wglGetProcAddress ("glGetRenderbufferParameterivEXT")))
	return false; //PrintLog (0, "glGetRenderbufferParameterivEXT not supported by the OpenGL driver\n");
else if (!(glIsFramebufferEXT = (PFNGLISFRAMEBUFFEREXTPROC) wglGetProcAddress ("glIsFramebufferEXT")))
	return false; //PrintLog (0, "glIsFramebufferEXT not supported by the OpenGL driver\n");
else if (!(glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC) wglGetProcAddress ("glBindFramebufferEXT")))
	return false; //PrintLog (0, "glBindFramebufferEXT not supported by the OpenGL driver\n");
else if (!(glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC) wglGetProcAddress ("glDeleteFramebuffersEXT")))
	return false; //PrintLog (0, "glDeleteFramebuffersEXT not supported by the OpenGL driver\n");
else if (!(glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC) wglGetProcAddress ("glGenFramebuffersEXT")))
	return false; //PrintLog (0, "glGenFramebuffersEXT not supported by the OpenGL driver\n");
else if (!(glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) wglGetProcAddress ("glCheckFramebufferStatusEXT")))
	return false; //PrintLog (0, "glCheckFramebufferStatusEXT not supported by the OpenGL driver\n");
else if (!(glFramebufferTexture1DEXT = (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC) wglGetProcAddress ("glFramebufferTexture1DEXT")))
	return false; //PrintLog (0, "glFramebufferTexture1DEXT not supported by the OpenGL driver\n");
else if (!(glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) wglGetProcAddress ("glFramebufferTexture2DEXT")))
	return false; //PrintLog (0, "glFramebufferTexture2DEXT not supported by the OpenGL driver\n");
else if (!(glFramebufferTexture3DEXT = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC) wglGetProcAddress ("glFramebufferTexture3DEXT")))
	return false; //PrintLog (0, "glFramebufferTexture3DEXT not supported by the OpenGL driver\n");
else if (!(glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) wglGetProcAddress ("glFramebufferRenderbufferEXT")))
	return false; //PrintLog (0, "glFramebufferRenderbufferEXT not supported by the OpenGL driver\n");
else if (!(glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC) wglGetProcAddress ("glGetFramebufferAttachmentParameterivEXT")))
	return false; //PrintLog (0, "glGetFramebufferAttachmentParameterivEXT not supported by the OpenGL driver\n");
else if (!(glGenerateMipmapEXT = (PFNGLGENERATEMIPMAPEXTPROC) wglGetProcAddress ("glGenerateMipmapEXT")))
	return false; //PrintLog (0, "glGenerateMipmapEXT not supported by the OpenGL driver\n");
else
#endif
	return true; //PrintLog (-1);
}

//------------------------------------------------------------------------------

