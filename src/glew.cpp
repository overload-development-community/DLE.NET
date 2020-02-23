/*
** The OpenGL Extension Wrangler Library
** Copyright (C) 2002-2008, Milan Ikits <milan ikits[]ieee org>
** Copyright (C) 2002-2008, Marcelo E. Magallon <mmagallo[]debian org>
** Copyright (C) 2002, Lev Povalahev
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without 
** modification, are permitted provided that the following conditions are met:
** 
** * Redistributions of source code must retain the above copyright notice, 
**   this list of conditions and the following disclaimer.
** * Redistributions in binary form must reproduce the above copyright notice, 
**   this list of conditions and the following disclaimer in the documentation 
**   and/or other materials provided with the distribution.
** * The name of the author may be used to endorse or promote products 
**   derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
** THE POSSIBILITY OF SUCH DAMAGE.
*/

#if defined(_WIN32)
#	include "glew.h"
#  include "wglew.h"
#else
#	include "glew.h"
#	if !defined(__APPLE__) || defined(GLEW_APPLE_GLX)
#		include <GL/glxew.h>
#	endif
#endif

#ifndef null
#	define null NULL
#endif

/*
 * Define glewGetContext and related helper macros.
 */
#ifdef GLEW_MX
#  define glewGetContext() ctx
#  ifdef _WIN32
#    define GLEW_CONTEXT_ARG_DEF_INIT GLEWContext* ctx
#    define GLEW_CONTEXT_ARG_VAR_INIT ctx
#    define wglewGetContext() ctx
#    define WGLEW_CONTEXT_ARG_DEF_INIT WGLEWContext* ctx
#    define WGLEW_CONTEXT_ARG_DEF_LIST WGLEWContext* ctx
#  else /* _WIN32 */
#    define GLEW_CONTEXT_ARG_DEF_INIT void
#    define GLEW_CONTEXT_ARG_VAR_INIT
#    define glxewGetContext() ctx
#    define GLXEW_CONTEXT_ARG_DEF_INIT void
#    define GLXEW_CONTEXT_ARG_DEF_LIST GLXEWContext* ctx
#  endif /* _WIN32 */
#  define GLEW_CONTEXT_ARG_DEF_LIST GLEWContext* ctx
#else /* GLEW_MX */
#  define GLEW_CONTEXT_ARG_DEF_INIT void
#  define GLEW_CONTEXT_ARG_VAR_INIT
#  define GLEW_CONTEXT_ARG_DEF_LIST void
#  define WGLEW_CONTEXT_ARG_DEF_INIT void
#  define WGLEW_CONTEXT_ARG_DEF_LIST void
#  define GLXEW_CONTEXT_ARG_DEF_INIT void
#  define GLXEW_CONTEXT_ARG_DEF_LIST void
#endif /* GLEW_MX */

#if defined(__APPLE__)
#include <stdlib.h>
#include <string.h>
#include <AvailabilityMacros.h>

#ifdef MAC_OS_X_VERSION_10_3

#include <dlfcn.h>

void* NSGLGetProcAddress (const GLubyte *name)
{
  static void* image = null;
  if (null == image) 
  {
    image = dlopen("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", RTLD_LAZY);
  }
  return image ? dlsym(image, (const char*)name) : null;
}
#else

#include <mach-o/dyld.h>

void* NSGLGetProcAddress (const GLubyte *name)
{
  static const struct mach_header* image = null;
  NSSymbol symbol;
  char* symbolName;
  if (null == image)
  {
    image = NSAddImage("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", NSADDIMAGE_OPTION_RETURN_ON_ERROR);
  }
  /* prepend a '_' for the Unix C symbol mangling convention */
  symbolName = malloc(strlen((const char*)name) + 2);
  strcpy(symbolName+1, (const char*)name);
  symbolName[0] = '_';
  symbol = null;
  /* if (NSIsSymbolNameDefined(symbolName))
	 symbol = NSLookupAndBindSymbol(symbolName); */
  symbol = image ? NSLookupSymbolInImage(image, symbolName, NSLOOKUPSYMBOLINIMAGE_OPTION_BIND | NSLOOKUPSYMBOLINIMAGE_OPTION_RETURN_ON_ERROR) : null;
  free(symbolName);
  return symbol ? NSAddressOfSymbol(symbol) : null;
}
#endif /* MAC_OS_X_VERSION_10_3 */
#endif /* __APPLE__ */

#if defined(__sgi) || defined (__sun)
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

void* dlGetProcAddress (const GLubyte* name)
{
  static void* h = null;
  static void* gpa;

  if (h == null)
  {
    if ((h = dlopen(null, RTLD_LAZY | RTLD_LOCAL)) == null) return null;
    gpa = dlsym(h, "glXGetProcAddress");
  }

  if (gpa != null)
    return ((void*(*)(const GLubyte*))gpa)(name);
  else
    return dlsym(h, (const char*)name);
}
#endif /* __sgi || __sun */

/*
 * Define glewGetProcAddress.
 */
#if defined(_WIN32)
#  define glewGetProcAddress(name) wglGetProcAddress((LPCSTR)name)
#else
#  if defined(__APPLE__)
#    define glewGetProcAddress(name) NSGLGetProcAddress(name)
#  else
#    if defined(__sgi) || defined(__sun)
#      define glewGetProcAddress(name) dlGetProcAddress(name)
#    else /* __linux */
#      define glewGetProcAddress(name) (*glXGetProcAddressARB)(name)
#    endif
#  endif
#endif

/*
 * Define GLboolean const cast.
 */
#define CONST_CAST(x) (*(GLboolean*)&x)

/*
 * GLEW, just like OpenGL or GLU, does not rely on the standard C library.
 * These functions implement the functionality required in this file.
 */
static GLuint _glewStrLen (const GLubyte* s)
{
  GLuint i=0;
  if (s == null) return 0;
  while (s[i] != '\0') i++;
  return i;
}

static GLuint _glewStrCLen (const GLubyte* s, GLubyte c)
{
  GLuint i=0;
  if (s == null) return 0;
  while (s[i] != '\0' && s[i] != c) i++;
  return (s[i] == '\0' || s[i] == c) ? i : 0;
}

static GLboolean _glewStrSame (const GLubyte* a, const GLubyte* b, GLuint n)
{
  GLuint i=0;
  if(a == null || b == null)
    return (a == null && b == null && n == 0) ? GL_TRUE : GL_FALSE;
  while (i < n && a[i] != '\0' && b[i] != '\0' && a[i] == b[i]) i++;
  return i == n ? GL_TRUE : GL_FALSE;
}

static GLboolean _glewStrSame1 (GLubyte** a, GLuint* na, const GLubyte* b, GLuint nb)
{
  while (*na > 0 && (**a == ' ' || **a == '\n' || **a == '\r' || **a == '\t'))
  {
    (*a)++;
    (*na)--;
  }
  if(*na >= nb)
  {
    GLuint i=0;
    while (i < nb && (*a)+i != null && b+i != null && (*a)[i] == b[i]) i++;
	if(i == nb)
	{
		*a = *a + nb;
		*na = *na - nb;
		return GL_TRUE;
	}
  }
  return GL_FALSE;
}

static GLboolean _glewStrSame2 (GLubyte** a, GLuint* na, const GLubyte* b, GLuint nb)
{
  if(*na >= nb)
  {
    GLuint i=0;
    while (i < nb && (*a)+i != null && b+i != null && (*a)[i] == b[i]) i++;
	if(i == nb)
	{
		*a = *a + nb;
		*na = *na - nb;
		return GL_TRUE;
	}
  }
  return GL_FALSE;
}

static GLboolean _glewStrSame3 (GLubyte** a, GLuint* na, const GLubyte* b, GLuint nb)
{
  if(*na >= nb)
  {
    GLuint i=0;
    while (i < nb && (*a)+i != null && b+i != null && (*a)[i] == b[i]) i++;
    if (i == nb && (*na == nb || (*a)[i] == ' ' || (*a)[i] == '\n' || (*a)[i] == '\r' || (*a)[i] == '\t'))
    {
      *a = *a + nb;
      *na = *na - nb;
      return GL_TRUE;
    }
  }
  return GL_FALSE;
}

#if !defined(_WIN32) || !defined(GLEW_MX)

PFNGLCOPYTEXSUBIMAGE3DPROC __glewCopyTexSubImage3D = null;
PFNGLDRAWRANGEELEMENTSPROC __glewDrawRangeElements = null;
PFNGLTEXIMAGE3DPROC __glewTexImage3D = null;
PFNGLTEXSUBIMAGE3DPROC __glewTexSubImage3D = null;

PFNGLACTIVETEXTUREPROC __glewActiveTexture = null;
PFNGLCLIENTACTIVETEXTUREPROC __glewClientActiveTexture = null;
PFNGLCOMPRESSEDTEXIMAGE1DPROC __glewCompressedTexImage1D = null;
PFNGLCOMPRESSEDTEXIMAGE2DPROC __glewCompressedTexImage2D = null;
PFNGLCOMPRESSEDTEXIMAGE3DPROC __glewCompressedTexImage3D = null;
PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC __glewCompressedTexSubImage1D = null;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC __glewCompressedTexSubImage2D = null;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC __glewCompressedTexSubImage3D = null;
PFNGLGETCOMPRESSEDTEXIMAGEPROC __glewGetCompressedTexImage = null;
PFNGLLOADTRANSPOSEMATRIXDPROC __glewLoadTransposeMatrixd = null;
PFNGLLOADTRANSPOSEMATRIXFPROC __glewLoadTransposeMatrixf = null;
PFNGLMULTTRANSPOSEMATRIXDPROC __glewMultTransposeMatrixd = null;
PFNGLMULTTRANSPOSEMATRIXFPROC __glewMultTransposeMatrixf = null;
PFNGLMULTITEXCOORD1DPROC __glewMultiTexCoord1d = null;
PFNGLMULTITEXCOORD1DVPROC __glewMultiTexCoord1dv = null;
PFNGLMULTITEXCOORD1FPROC __glewMultiTexCoord1f = null;
PFNGLMULTITEXCOORD1FVPROC __glewMultiTexCoord1fv = null;
PFNGLMULTITEXCOORD1IPROC __glewMultiTexCoord1i = null;
PFNGLMULTITEXCOORD1IVPROC __glewMultiTexCoord1iv = null;
PFNGLMULTITEXCOORD1SPROC __glewMultiTexCoord1s = null;
PFNGLMULTITEXCOORD1SVPROC __glewMultiTexCoord1sv = null;
PFNGLMULTITEXCOORD2DPROC __glewMultiTexCoord2d = null;
PFNGLMULTITEXCOORD2DVPROC __glewMultiTexCoord2dv = null;
PFNGLMULTITEXCOORD2FPROC __glewMultiTexCoord2f = null;
PFNGLMULTITEXCOORD2FVPROC __glewMultiTexCoord2fv = null;
PFNGLMULTITEXCOORD2IPROC __glewMultiTexCoord2i = null;
PFNGLMULTITEXCOORD2IVPROC __glewMultiTexCoord2iv = null;
PFNGLMULTITEXCOORD2SPROC __glewMultiTexCoord2s = null;
PFNGLMULTITEXCOORD2SVPROC __glewMultiTexCoord2sv = null;
PFNGLMULTITEXCOORD3DPROC __glewMultiTexCoord3d = null;
PFNGLMULTITEXCOORD3DVPROC __glewMultiTexCoord3dv = null;
PFNGLMULTITEXCOORD3FPROC __glewMultiTexCoord3f = null;
PFNGLMULTITEXCOORD3FVPROC __glewMultiTexCoord3fv = null;
PFNGLMULTITEXCOORD3IPROC __glewMultiTexCoord3i = null;
PFNGLMULTITEXCOORD3IVPROC __glewMultiTexCoord3iv = null;
PFNGLMULTITEXCOORD3SPROC __glewMultiTexCoord3s = null;
PFNGLMULTITEXCOORD3SVPROC __glewMultiTexCoord3sv = null;
PFNGLMULTITEXCOORD4DPROC __glewMultiTexCoord4d = null;
PFNGLMULTITEXCOORD4DVPROC __glewMultiTexCoord4dv = null;
PFNGLMULTITEXCOORD4FPROC __glewMultiTexCoord4f = null;
PFNGLMULTITEXCOORD4FVPROC __glewMultiTexCoord4fv = null;
PFNGLMULTITEXCOORD4IPROC __glewMultiTexCoord4i = null;
PFNGLMULTITEXCOORD4IVPROC __glewMultiTexCoord4iv = null;
PFNGLMULTITEXCOORD4SPROC __glewMultiTexCoord4s = null;
PFNGLMULTITEXCOORD4SVPROC __glewMultiTexCoord4sv = null;
PFNGLSAMPLECOVERAGEPROC __glewSampleCoverage = null;

PFNGLBLENDCOLORPROC __glewBlendColor = null;
PFNGLBLENDEQUATIONPROC __glewBlendEquation = null;
PFNGLBLENDFUNCSEPARATEPROC __glewBlendFuncSeparate = null;
PFNGLFOGCOORDPOINTERPROC __glewFogCoordPointer = null;
PFNGLFOGCOORDDPROC __glewFogCoordd = null;
PFNGLFOGCOORDDVPROC __glewFogCoorddv = null;
PFNGLFOGCOORDFPROC __glewFogCoordf = null;
PFNGLFOGCOORDFVPROC __glewFogCoordfv = null;
PFNGLMULTIDRAWARRAYSPROC __glewMultiDrawArrays = null;
PFNGLMULTIDRAWELEMENTSPROC __glewMultiDrawElements = null;
PFNGLPOINTPARAMETERFPROC __glewPointParameterf = null;
PFNGLPOINTPARAMETERFVPROC __glewPointParameterfv = null;
PFNGLPOINTPARAMETERIPROC __glewPointParameteri = null;
PFNGLPOINTPARAMETERIVPROC __glewPointParameteriv = null;
PFNGLSECONDARYCOLOR3BPROC __glewSecondaryColor3b = null;
PFNGLSECONDARYCOLOR3BVPROC __glewSecondaryColor3bv = null;
PFNGLSECONDARYCOLOR3DPROC __glewSecondaryColor3d = null;
PFNGLSECONDARYCOLOR3DVPROC __glewSecondaryColor3dv = null;
PFNGLSECONDARYCOLOR3FPROC __glewSecondaryColor3f = null;
PFNGLSECONDARYCOLOR3FVPROC __glewSecondaryColor3fv = null;
PFNGLSECONDARYCOLOR3IPROC __glewSecondaryColor3i = null;
PFNGLSECONDARYCOLOR3IVPROC __glewSecondaryColor3iv = null;
PFNGLSECONDARYCOLOR3SPROC __glewSecondaryColor3s = null;
PFNGLSECONDARYCOLOR3SVPROC __glewSecondaryColor3sv = null;
PFNGLSECONDARYCOLOR3UBPROC __glewSecondaryColor3ub = null;
PFNGLSECONDARYCOLOR3UBVPROC __glewSecondaryColor3ubv = null;
PFNGLSECONDARYCOLOR3UIPROC __glewSecondaryColor3ui = null;
PFNGLSECONDARYCOLOR3UIVPROC __glewSecondaryColor3uiv = null;
PFNGLSECONDARYCOLOR3USPROC __glewSecondaryColor3us = null;
PFNGLSECONDARYCOLOR3USVPROC __glewSecondaryColor3usv = null;
PFNGLSECONDARYCOLORPOINTERPROC __glewSecondaryColorPointer = null;
PFNGLWINDOWPOS2DPROC __glewWindowPos2d = null;
PFNGLWINDOWPOS2DVPROC __glewWindowPos2dv = null;
PFNGLWINDOWPOS2FPROC __glewWindowPos2f = null;
PFNGLWINDOWPOS2FVPROC __glewWindowPos2fv = null;
PFNGLWINDOWPOS2IPROC __glewWindowPos2i = null;
PFNGLWINDOWPOS2IVPROC __glewWindowPos2iv = null;
PFNGLWINDOWPOS2SPROC __glewWindowPos2s = null;
PFNGLWINDOWPOS2SVPROC __glewWindowPos2sv = null;
PFNGLWINDOWPOS3DPROC __glewWindowPos3d = null;
PFNGLWINDOWPOS3DVPROC __glewWindowPos3dv = null;
PFNGLWINDOWPOS3FPROC __glewWindowPos3f = null;
PFNGLWINDOWPOS3FVPROC __glewWindowPos3fv = null;
PFNGLWINDOWPOS3IPROC __glewWindowPos3i = null;
PFNGLWINDOWPOS3IVPROC __glewWindowPos3iv = null;
PFNGLWINDOWPOS3SPROC __glewWindowPos3s = null;
PFNGLWINDOWPOS3SVPROC __glewWindowPos3sv = null;

PFNGLBEGINQUERYPROC __glewBeginQuery = null;
PFNGLBINDBUFFERPROC __glewBindBuffer = null;
PFNGLBUFFERDATAPROC __glewBufferData = null;
PFNGLBUFFERSUBDATAPROC __glewBufferSubData = null;
PFNGLDELETEBUFFERSPROC __glewDeleteBuffers = null;
PFNGLDELETEQUERIESPROC __glewDeleteQueries = null;
PFNGLENDQUERYPROC __glewEndQuery = null;
PFNGLGENBUFFERSPROC __glewGenBuffers = null;
PFNGLGENQUERIESPROC __glewGenQueries = null;
PFNGLGETBUFFERPARAMETERIVPROC __glewGetBufferParameteriv = null;
PFNGLGETBUFFERPOINTERVPROC __glewGetBufferPointerv = null;
PFNGLGETBUFFERSUBDATAPROC __glewGetBufferSubData = null;
PFNGLGETQUERYOBJECTIVPROC __glewGetQueryObjectiv = null;
PFNGLGETQUERYOBJECTUIVPROC __glewGetQueryObjectuiv = null;
PFNGLGETQUERYIVPROC __glewGetQueryiv = null;
PFNGLISBUFFERPROC __glewIsBuffer = null;
PFNGLISQUERYPROC __glewIsQuery = null;
PFNGLMAPBUFFERPROC __glewMapBuffer = null;
PFNGLUNMAPBUFFERPROC __glewUnmapBuffer = null;

PFNGLATTACHSHADERPROC __glewAttachShader = null;
PFNGLBINDATTRIBLOCATIONPROC __glewBindAttribLocation = null;
PFNGLBLENDEQUATIONSEPARATEPROC __glewBlendEquationSeparate = null;
PFNGLCOMPILESHADERPROC __glewCompileShader = null;
PFNGLCREATEPROGRAMPROC __glewCreateProgram = null;
PFNGLCREATESHADERPROC __glewCreateShader = null;
PFNGLDELETEPROGRAMPROC __glewDeleteProgram = null;
PFNGLDELETESHADERPROC __glewDeleteShader = null;
PFNGLDETACHSHADERPROC __glewDetachShader = null;
PFNGLDISABLEVERTEXATTRIBARRAYPROC __glewDisableVertexAttribArray = null;
PFNGLDRAWBUFFERSPROC __glewDrawBuffers = null;
PFNGLENABLEVERTEXATTRIBARRAYPROC __glewEnableVertexAttribArray = null;
PFNGLGETACTIVEATTRIBPROC __glewGetActiveAttrib = null;
PFNGLGETACTIVEUNIFORMPROC __glewGetActiveUniform = null;
PFNGLGETATTACHEDSHADERSPROC __glewGetAttachedShaders = null;
PFNGLGETATTRIBLOCATIONPROC __glewGetAttribLocation = null;
PFNGLGETPROGRAMINFOLOGPROC __glewGetProgramInfoLog = null;
PFNGLGETPROGRAMIVPROC __glewGetProgramiv = null;
PFNGLGETSHADERINFOLOGPROC __glewGetShaderInfoLog = null;
PFNGLGETSHADERSOURCEPROC __glewGetShaderSource = null;
PFNGLGETSHADERIVPROC __glewGetShaderiv = null;
PFNGLGETUNIFORMLOCATIONPROC __glewGetUniformLocation = null;
PFNGLGETUNIFORMFVPROC __glewGetUniformfv = null;
PFNGLGETUNIFORMIVPROC __glewGetUniformiv = null;
PFNGLGETVERTEXATTRIBPOINTERVPROC __glewGetVertexAttribPointerv = null;
PFNGLGETVERTEXATTRIBDVPROC __glewGetVertexAttribdv = null;
PFNGLGETVERTEXATTRIBFVPROC __glewGetVertexAttribfv = null;
PFNGLGETVERTEXATTRIBIVPROC __glewGetVertexAttribiv = null;
PFNGLISPROGRAMPROC __glewIsProgram = null;
PFNGLISSHADERPROC __glewIsShader = null;
PFNGLLINKPROGRAMPROC __glewLinkProgram = null;
PFNGLSHADERSOURCEPROC __glewShaderSource = null;
PFNGLSTENCILFUNCSEPARATEPROC __glewStencilFuncSeparate = null;
PFNGLSTENCILMASKSEPARATEPROC __glewStencilMaskSeparate = null;
PFNGLSTENCILOPSEPARATEPROC __glewStencilOpSeparate = null;
PFNGLUNIFORM1FPROC __glewUniform1f = null;
PFNGLUNIFORM1FVPROC __glewUniform1fv = null;
PFNGLUNIFORM1IPROC __glewUniform1i = null;
PFNGLUNIFORM1IVPROC __glewUniform1iv = null;
PFNGLUNIFORM2FPROC __glewUniform2f = null;
PFNGLUNIFORM2FVPROC __glewUniform2fv = null;
PFNGLUNIFORM2IPROC __glewUniform2i = null;
PFNGLUNIFORM2IVPROC __glewUniform2iv = null;
PFNGLUNIFORM3FPROC __glewUniform3f = null;
PFNGLUNIFORM3FVPROC __glewUniform3fv = null;
PFNGLUNIFORM3IPROC __glewUniform3i = null;
PFNGLUNIFORM3IVPROC __glewUniform3iv = null;
PFNGLUNIFORM4FPROC __glewUniform4f = null;
PFNGLUNIFORM4FVPROC __glewUniform4fv = null;
PFNGLUNIFORM4IPROC __glewUniform4i = null;
PFNGLUNIFORM4IVPROC __glewUniform4iv = null;
PFNGLUNIFORMMATRIX2FVPROC __glewUniformMatrix2fv = null;
PFNGLUNIFORMMATRIX3FVPROC __glewUniformMatrix3fv = null;
PFNGLUNIFORMMATRIX4FVPROC __glewUniformMatrix4fv = null;
PFNGLUSEPROGRAMPROC __glewUseProgram = null;
PFNGLVALIDATEPROGRAMPROC __glewValidateProgram = null;
PFNGLVERTEXATTRIB1DPROC __glewVertexAttrib1d = null;
PFNGLVERTEXATTRIB1DVPROC __glewVertexAttrib1dv = null;
PFNGLVERTEXATTRIB1FPROC __glewVertexAttrib1f = null;
PFNGLVERTEXATTRIB1FVPROC __glewVertexAttrib1fv = null;
PFNGLVERTEXATTRIB1SPROC __glewVertexAttrib1s = null;
PFNGLVERTEXATTRIB1SVPROC __glewVertexAttrib1sv = null;
PFNGLVERTEXATTRIB2DPROC __glewVertexAttrib2d = null;
PFNGLVERTEXATTRIB2DVPROC __glewVertexAttrib2dv = null;
PFNGLVERTEXATTRIB2FPROC __glewVertexAttrib2f = null;
PFNGLVERTEXATTRIB2FVPROC __glewVertexAttrib2fv = null;
PFNGLVERTEXATTRIB2SPROC __glewVertexAttrib2s = null;
PFNGLVERTEXATTRIB2SVPROC __glewVertexAttrib2sv = null;
PFNGLVERTEXATTRIB3DPROC __glewVertexAttrib3d = null;
PFNGLVERTEXATTRIB3DVPROC __glewVertexAttrib3dv = null;
PFNGLVERTEXATTRIB3FPROC __glewVertexAttrib3f = null;
PFNGLVERTEXATTRIB3FVPROC __glewVertexAttrib3fv = null;
PFNGLVERTEXATTRIB3SPROC __glewVertexAttrib3s = null;
PFNGLVERTEXATTRIB3SVPROC __glewVertexAttrib3sv = null;
PFNGLVERTEXATTRIB4NBVPROC __glewVertexAttrib4Nbv = null;
PFNGLVERTEXATTRIB4NIVPROC __glewVertexAttrib4Niv = null;
PFNGLVERTEXATTRIB4NSVPROC __glewVertexAttrib4Nsv = null;
PFNGLVERTEXATTRIB4NUBPROC __glewVertexAttrib4Nub = null;
PFNGLVERTEXATTRIB4NUBVPROC __glewVertexAttrib4Nubv = null;
PFNGLVERTEXATTRIB4NUIVPROC __glewVertexAttrib4Nuiv = null;
PFNGLVERTEXATTRIB4NUSVPROC __glewVertexAttrib4Nusv = null;
PFNGLVERTEXATTRIB4BVPROC __glewVertexAttrib4bv = null;
PFNGLVERTEXATTRIB4DPROC __glewVertexAttrib4d = null;
PFNGLVERTEXATTRIB4DVPROC __glewVertexAttrib4dv = null;
PFNGLVERTEXATTRIB4FPROC __glewVertexAttrib4f = null;
PFNGLVERTEXATTRIB4FVPROC __glewVertexAttrib4fv = null;
PFNGLVERTEXATTRIB4IVPROC __glewVertexAttrib4iv = null;
PFNGLVERTEXATTRIB4SPROC __glewVertexAttrib4s = null;
PFNGLVERTEXATTRIB4SVPROC __glewVertexAttrib4sv = null;
PFNGLVERTEXATTRIB4UBVPROC __glewVertexAttrib4ubv = null;
PFNGLVERTEXATTRIB4UIVPROC __glewVertexAttrib4uiv = null;
PFNGLVERTEXATTRIB4USVPROC __glewVertexAttrib4usv = null;
PFNGLVERTEXATTRIBPOINTERPROC __glewVertexAttribPointer = null;

PFNGLUNIFORMMATRIX2X3FVPROC __glewUniformMatrix2x3fv = null;
PFNGLUNIFORMMATRIX2X4FVPROC __glewUniformMatrix2x4fv = null;
PFNGLUNIFORMMATRIX3X2FVPROC __glewUniformMatrix3x2fv = null;
PFNGLUNIFORMMATRIX3X4FVPROC __glewUniformMatrix3x4fv = null;
PFNGLUNIFORMMATRIX4X2FVPROC __glewUniformMatrix4x2fv = null;
PFNGLUNIFORMMATRIX4X3FVPROC __glewUniformMatrix4x3fv = null;

PFNGLBEGINCONDITIONALRENDERPROC __glewBeginConditionalRender = null;
PFNGLBEGINTRANSFORMFEEDBACKPROC __glewBeginTransformFeedback = null;
PFNGLBINDFRAGDATALOCATIONPROC __glewBindFragDataLocation = null;
PFNGLCLAMPCOLORPROC __glewClampColor = null;
PFNGLCLEARBUFFERFIPROC __glewClearBufferfi = null;
PFNGLCLEARBUFFERFVPROC __glewClearBufferfv = null;
PFNGLCLEARBUFFERIVPROC __glewClearBufferiv = null;
PFNGLCLEARBUFFERUIVPROC __glewClearBufferuiv = null;
PFNGLCOLORMASKIPROC __glewColorMaski = null;
PFNGLDISABLEIPROC __glewDisablei = null;
PFNGLENABLEIPROC __glewEnablei = null;
PFNGLENDCONDITIONALRENDERPROC __glewEndConditionalRender = null;
PFNGLENDTRANSFORMFEEDBACKPROC __glewEndTransformFeedback = null;
PFNGLGETBOOLEANI_VPROC __glewGetBooleani_v = null;
PFNGLGETFRAGDATALOCATIONPROC __glewGetFragDataLocation = null;
PFNGLGETSTRINGIPROC __glewGetStringi = null;
PFNGLGETTEXPARAMETERIIVPROC __glewGetTexParameterIiv = null;
PFNGLGETTEXPARAMETERIUIVPROC __glewGetTexParameterIuiv = null;
PFNGLGETTRANSFORMFEEDBACKVARYINGPROC __glewGetTransformFeedbackVarying = null;
PFNGLGETUNIFORMUIVPROC __glewGetUniformuiv = null;
PFNGLGETVERTEXATTRIBIIVPROC __glewGetVertexAttribIiv = null;
PFNGLGETVERTEXATTRIBIUIVPROC __glewGetVertexAttribIuiv = null;
PFNGLISENABLEDIPROC __glewIsEnabledi = null;
PFNGLTEXPARAMETERIIVPROC __glewTexParameterIiv = null;
PFNGLTEXPARAMETERIUIVPROC __glewTexParameterIuiv = null;
PFNGLTRANSFORMFEEDBACKVARYINGSPROC __glewTransformFeedbackVaryings = null;
PFNGLUNIFORM1UIPROC __glewUniform1ui = null;
PFNGLUNIFORM1UIVPROC __glewUniform1uiv = null;
PFNGLUNIFORM2UIPROC __glewUniform2ui = null;
PFNGLUNIFORM2UIVPROC __glewUniform2uiv = null;
PFNGLUNIFORM3UIPROC __glewUniform3ui = null;
PFNGLUNIFORM3UIVPROC __glewUniform3uiv = null;
PFNGLUNIFORM4UIPROC __glewUniform4ui = null;
PFNGLUNIFORM4UIVPROC __glewUniform4uiv = null;
PFNGLVERTEXATTRIBI1IPROC __glewVertexAttribI1i = null;
PFNGLVERTEXATTRIBI1IVPROC __glewVertexAttribI1iv = null;
PFNGLVERTEXATTRIBI1UIPROC __glewVertexAttribI1ui = null;
PFNGLVERTEXATTRIBI1UIVPROC __glewVertexAttribI1uiv = null;
PFNGLVERTEXATTRIBI2IPROC __glewVertexAttribI2i = null;
PFNGLVERTEXATTRIBI2IVPROC __glewVertexAttribI2iv = null;
PFNGLVERTEXATTRIBI2UIPROC __glewVertexAttribI2ui = null;
PFNGLVERTEXATTRIBI2UIVPROC __glewVertexAttribI2uiv = null;
PFNGLVERTEXATTRIBI3IPROC __glewVertexAttribI3i = null;
PFNGLVERTEXATTRIBI3IVPROC __glewVertexAttribI3iv = null;
PFNGLVERTEXATTRIBI3UIPROC __glewVertexAttribI3ui = null;
PFNGLVERTEXATTRIBI3UIVPROC __glewVertexAttribI3uiv = null;
PFNGLVERTEXATTRIBI4BVPROC __glewVertexAttribI4bv = null;
PFNGLVERTEXATTRIBI4IPROC __glewVertexAttribI4i = null;
PFNGLVERTEXATTRIBI4IVPROC __glewVertexAttribI4iv = null;
PFNGLVERTEXATTRIBI4SVPROC __glewVertexAttribI4sv = null;
PFNGLVERTEXATTRIBI4UBVPROC __glewVertexAttribI4ubv = null;
PFNGLVERTEXATTRIBI4UIPROC __glewVertexAttribI4ui = null;
PFNGLVERTEXATTRIBI4UIVPROC __glewVertexAttribI4uiv = null;
PFNGLVERTEXATTRIBI4USVPROC __glewVertexAttribI4usv = null;
PFNGLVERTEXATTRIBIPOINTERPROC __glewVertexAttribIPointer = null;

PFNGLDRAWARRAYSINSTANCEDPROC __glewDrawArraysInstanced = null;
PFNGLDRAWELEMENTSINSTANCEDPROC __glewDrawElementsInstanced = null;
PFNGLPRIMITIVERESTARTINDEXPROC __glewPrimitiveRestartIndex = null;
PFNGLTEXBUFFERPROC __glewTexBuffer = null;

PFNGLFRAMEBUFFERTEXTUREPROC __glewFramebufferTexture = null;
PFNGLGETBUFFERPARAMETERI64VPROC __glewGetBufferParameteri64v = null;
PFNGLGETINTEGER64I_VPROC __glewGetInteger64i_v = null;

PFNGLTBUFFERMASK3DFXPROC __glewTbufferMask3DFX = null;

PFNGLBLENDEQUATIONINDEXEDAMDPROC __glewBlendEquationIndexedAMD = null;
PFNGLBLENDEQUATIONSEPARATEINDEXEDAMDPROC __glewBlendEquationSeparateIndexedAMD = null;
PFNGLBLENDFUNCINDEXEDAMDPROC __glewBlendFuncIndexedAMD = null;
PFNGLBLENDFUNCSEPARATEINDEXEDAMDPROC __glewBlendFuncSeparateIndexedAMD = null;

PFNGLBEGINPERFMONITORAMDPROC __glewBeginPerfMonitorAMD = null;
PFNGLDELETEPERFMONITORSAMDPROC __glewDeletePerfMonitorsAMD = null;
PFNGLENDPERFMONITORAMDPROC __glewEndPerfMonitorAMD = null;
PFNGLGENPERFMONITORSAMDPROC __glewGenPerfMonitorsAMD = null;
PFNGLGETPERFMONITORCOUNTERDATAAMDPROC __glewGetPerfMonitorCounterDataAMD = null;
PFNGLGETPERFMONITORCOUNTERINFOAMDPROC __glewGetPerfMonitorCounterInfoAMD = null;
PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC __glewGetPerfMonitorCounterStringAMD = null;
PFNGLGETPERFMONITORCOUNTERSAMDPROC __glewGetPerfMonitorCountersAMD = null;
PFNGLGETPERFMONITORGROUPSTRINGAMDPROC __glewGetPerfMonitorGroupStringAMD = null;
PFNGLGETPERFMONITORGROUPSAMDPROC __glewGetPerfMonitorGroupsAMD = null;
PFNGLSELECTPERFMONITORCOUNTERSAMDPROC __glewSelectPerfMonitorCountersAMD = null;

PFNGLTESSELLATIONFACTORAMDPROC __glewTessellationFactorAMD = null;
PFNGLTESSELLATIONMODEAMDPROC __glewTessellationModeAMD = null;

PFNGLDRAWELEMENTARRAYAPPLEPROC __glewDrawElementArrayAPPLE = null;
PFNGLDRAWRANGEELEMENTARRAYAPPLEPROC __glewDrawRangeElementArrayAPPLE = null;
PFNGLELEMENTPOINTERAPPLEPROC __glewElementPointerAPPLE = null;
PFNGLMULTIDRAWELEMENTARRAYAPPLEPROC __glewMultiDrawElementArrayAPPLE = null;
PFNGLMULTIDRAWRANGEELEMENTARRAYAPPLEPROC __glewMultiDrawRangeElementArrayAPPLE = null;

PFNGLDELETEFENCESAPPLEPROC __glewDeleteFencesAPPLE = null;
PFNGLFINISHFENCEAPPLEPROC __glewFinishFenceAPPLE = null;
PFNGLFINISHOBJECTAPPLEPROC __glewFinishObjectAPPLE = null;
PFNGLGENFENCESAPPLEPROC __glewGenFencesAPPLE = null;
PFNGLISFENCEAPPLEPROC __glewIsFenceAPPLE = null;
PFNGLSETFENCEAPPLEPROC __glewSetFenceAPPLE = null;
PFNGLTESTFENCEAPPLEPROC __glewTestFenceAPPLE = null;
PFNGLTESTOBJECTAPPLEPROC __glewTestObjectAPPLE = null;

PFNGLBUFFERPARAMETERIAPPLEPROC __glewBufferParameteriAPPLE = null;
PFNGLFLUSHMAPPEDBUFFERRANGEAPPLEPROC __glewFlushMappedBufferRangeAPPLE = null;

PFNGLGETOBJECTPARAMETERIVAPPLEPROC __glewGetObjectParameterivAPPLE = null;
PFNGLOBJECTPURGEABLEAPPLEPROC __glewObjectPurgeableAPPLE = null;
PFNGLOBJECTUNPURGEABLEAPPLEPROC __glewObjectUnpurgeableAPPLE = null;

PFNGLGETTEXPARAMETERPOINTERVAPPLEPROC __glewGetTexParameterPointervAPPLE = null;
PFNGLTEXTURERANGEAPPLEPROC __glewTextureRangeAPPLE = null;

PFNGLBINDVERTEXARRAYAPPLEPROC __glewBindVertexArrayAPPLE = null;
PFNGLDELETEVERTEXARRAYSAPPLEPROC __glewDeleteVertexArraysAPPLE = null;
PFNGLGENVERTEXARRAYSAPPLEPROC __glewGenVertexArraysAPPLE = null;
PFNGLISVERTEXARRAYAPPLEPROC __glewIsVertexArrayAPPLE = null;

PFNGLFLUSHVERTEXARRAYRANGEAPPLEPROC __glewFlushVertexArrayRangeAPPLE = null;
PFNGLVERTEXARRAYPARAMETERIAPPLEPROC __glewVertexArrayParameteriAPPLE = null;
PFNGLVERTEXARRAYRANGEAPPLEPROC __glewVertexArrayRangeAPPLE = null;

PFNGLDISABLEVERTEXATTRIBAPPLEPROC __glewDisableVertexAttribAPPLE = null;
PFNGLENABLEVERTEXATTRIBAPPLEPROC __glewEnableVertexAttribAPPLE = null;
PFNGLISVERTEXATTRIBENABLEDAPPLEPROC __glewIsVertexAttribEnabledAPPLE = null;
PFNGLMAPVERTEXATTRIB1DAPPLEPROC __glewMapVertexAttrib1dAPPLE = null;
PFNGLMAPVERTEXATTRIB1FAPPLEPROC __glewMapVertexAttrib1fAPPLE = null;
PFNGLMAPVERTEXATTRIB2DAPPLEPROC __glewMapVertexAttrib2dAPPLE = null;
PFNGLMAPVERTEXATTRIB2FAPPLEPROC __glewMapVertexAttrib2fAPPLE = null;

PFNGLCLAMPCOLORARBPROC __glewClampColorARB = null;

PFNGLCOPYBUFFERSUBDATAPROC __glewCopyBufferSubData = null;

PFNGLDRAWBUFFERSARBPROC __glewDrawBuffersARB = null;

PFNGLBLENDEQUATIONSEPARATEIARBPROC __glewBlendEquationSeparateiARB = null;
PFNGLBLENDEQUATIONIARBPROC __glewBlendEquationiARB = null;
PFNGLBLENDFUNCSEPARATEIARBPROC __glewBlendFuncSeparateiARB = null;
PFNGLBLENDFUNCIARBPROC __glewBlendFunciARB = null;

PFNGLDRAWELEMENTSBASEVERTEXPROC __glewDrawElementsBaseVertex = null;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC __glewDrawElementsInstancedBaseVertex = null;
PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC __glewDrawRangeElementsBaseVertex = null;
PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC __glewMultiDrawElementsBaseVertex = null;

PFNGLDRAWARRAYSINSTANCEDARBPROC __glewDrawArraysInstancedARB = null;
PFNGLDRAWELEMENTSINSTANCEDARBPROC __glewDrawElementsInstancedARB = null;

PFNGLBINDFRAMEBUFFERPROC __glewBindFramebuffer = null;
PFNGLBINDRENDERBUFFERPROC __glewBindRenderbuffer = null;
PFNGLBLITFRAMEBUFFERPROC __glewBlitFramebuffer = null;
PFNGLCHECKFRAMEBUFFERSTATUSPROC __glewCheckFramebufferStatus = null;
PFNGLDELETEFRAMEBUFFERSPROC __glewDeleteFramebuffers = null;
PFNGLDELETERENDERBUFFERSPROC __glewDeleteRenderbuffers = null;
PFNGLFRAMEBUFFERRENDERBUFFERPROC __glewFramebufferRenderbuffer = null;
PFNGLFRAMEBUFFERTEXTURE1DPROC __glewFramebufferTexture1D = null;
PFNGLFRAMEBUFFERTEXTURE2DPROC __glewFramebufferTexture2D = null;
PFNGLFRAMEBUFFERTEXTURE3DPROC __glewFramebufferTexture3D = null;
PFNGLFRAMEBUFFERTEXTURELAYERPROC __glewFramebufferTextureLayer = null;
PFNGLGENFRAMEBUFFERSPROC __glewGenFramebuffers = null;
PFNGLGENRENDERBUFFERSPROC __glewGenRenderbuffers = null;
PFNGLGENERATEMIPMAPPROC __glewGenerateMipmap = null;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC __glewGetFramebufferAttachmentParameteriv = null;
PFNGLGETRENDERBUFFERPARAMETERIVPROC __glewGetRenderbufferParameteriv = null;
PFNGLISFRAMEBUFFERPROC __glewIsFramebuffer = null;
PFNGLISRENDERBUFFERPROC __glewIsRenderbuffer = null;
PFNGLRENDERBUFFERSTORAGEPROC __glewRenderbufferStorage = null;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC __glewRenderbufferStorageMultisample = null;

PFNGLFRAMEBUFFERTEXTUREARBPROC __glewFramebufferTextureARB = null;
PFNGLFRAMEBUFFERTEXTUREFACEARBPROC __glewFramebufferTextureFaceARB = null;
PFNGLFRAMEBUFFERTEXTURELAYERARBPROC __glewFramebufferTextureLayerARB = null;
PFNGLPROGRAMPARAMETERIARBPROC __glewProgramParameteriARB = null;

PFNGLCOLORSUBTABLEPROC __glewColorSubTable = null;
PFNGLCOLORTABLEPROC __glewColorTable = null;
PFNGLCOLORTABLEPARAMETERFVPROC __glewColorTableParameterfv = null;
PFNGLCOLORTABLEPARAMETERIVPROC __glewColorTableParameteriv = null;
PFNGLCONVOLUTIONFILTER1DPROC __glewConvolutionFilter1D = null;
PFNGLCONVOLUTIONFILTER2DPROC __glewConvolutionFilter2D = null;
PFNGLCONVOLUTIONPARAMETERFPROC __glewConvolutionParameterf = null;
PFNGLCONVOLUTIONPARAMETERFVPROC __glewConvolutionParameterfv = null;
PFNGLCONVOLUTIONPARAMETERIPROC __glewConvolutionParameteri = null;
PFNGLCONVOLUTIONPARAMETERIVPROC __glewConvolutionParameteriv = null;
PFNGLCOPYCOLORSUBTABLEPROC __glewCopyColorSubTable = null;
PFNGLCOPYCOLORTABLEPROC __glewCopyColorTable = null;
PFNGLCOPYCONVOLUTIONFILTER1DPROC __glewCopyConvolutionFilter1D = null;
PFNGLCOPYCONVOLUTIONFILTER2DPROC __glewCopyConvolutionFilter2D = null;
PFNGLGETCOLORTABLEPROC __glewGetColorTable = null;
PFNGLGETCOLORTABLEPARAMETERFVPROC __glewGetColorTableParameterfv = null;
PFNGLGETCOLORTABLEPARAMETERIVPROC __glewGetColorTableParameteriv = null;
PFNGLGETCONVOLUTIONFILTERPROC __glewGetConvolutionFilter = null;
PFNGLGETCONVOLUTIONPARAMETERFVPROC __glewGetConvolutionParameterfv = null;
PFNGLGETCONVOLUTIONPARAMETERIVPROC __glewGetConvolutionParameteriv = null;
PFNGLGETHISTOGRAMPROC __glewGetHistogram = null;
PFNGLGETHISTOGRAMPARAMETERFVPROC __glewGetHistogramParameterfv = null;
PFNGLGETHISTOGRAMPARAMETERIVPROC __glewGetHistogramParameteriv = null;
PFNGLGETMINMAXPROC __glewGetMinmax = null;
PFNGLGETMINMAXPARAMETERFVPROC __glewGetMinmaxParameterfv = null;
PFNGLGETMINMAXPARAMETERIVPROC __glewGetMinmaxParameteriv = null;
PFNGLGETSEPARABLEFILTERPROC __glewGetSeparableFilter = null;
PFNGLHISTOGRAMPROC __glewHistogram = null;
PFNGLMINMAXPROC __glewMinmax = null;
PFNGLRESETHISTOGRAMPROC __glewResetHistogram = null;
PFNGLRESETMINMAXPROC __glewResetMinmax = null;
PFNGLSEPARABLEFILTER2DPROC __glewSeparableFilter2D = null;

PFNGLVERTEXATTRIBDIVISORARBPROC __glewVertexAttribDivisorARB = null;

PFNGLFLUSHMAPPEDBUFFERRANGEPROC __glewFlushMappedBufferRange = null;
PFNGLMAPBUFFERRANGEPROC __glewMapBufferRange = null;

PFNGLCURRENTPALETTEMATRIXARBPROC __glewCurrentPaletteMatrixARB = null;
PFNGLMATRIXINDEXPOINTERARBPROC __glewMatrixIndexPointerARB = null;
PFNGLMATRIXINDEXUBVARBPROC __glewMatrixIndexubvARB = null;
PFNGLMATRIXINDEXUIVARBPROC __glewMatrixIndexuivARB = null;
PFNGLMATRIXINDEXUSVARBPROC __glewMatrixIndexusvARB = null;

PFNGLSAMPLECOVERAGEARBPROC __glewSampleCoverageARB = null;

PFNGLACTIVETEXTUREARBPROC __glewActiveTextureARB = null;
PFNGLCLIENTACTIVETEXTUREARBPROC __glewClientActiveTextureARB = null;
PFNGLMULTITEXCOORD1DARBPROC __glewMultiTexCoord1dARB = null;
PFNGLMULTITEXCOORD1DVARBPROC __glewMultiTexCoord1dvARB = null;
PFNGLMULTITEXCOORD1FARBPROC __glewMultiTexCoord1fARB = null;
PFNGLMULTITEXCOORD1FVARBPROC __glewMultiTexCoord1fvARB = null;
PFNGLMULTITEXCOORD1IARBPROC __glewMultiTexCoord1iARB = null;
PFNGLMULTITEXCOORD1IVARBPROC __glewMultiTexCoord1ivARB = null;
PFNGLMULTITEXCOORD1SARBPROC __glewMultiTexCoord1sARB = null;
PFNGLMULTITEXCOORD1SVARBPROC __glewMultiTexCoord1svARB = null;
PFNGLMULTITEXCOORD2DARBPROC __glewMultiTexCoord2dARB = null;
PFNGLMULTITEXCOORD2DVARBPROC __glewMultiTexCoord2dvARB = null;
PFNGLMULTITEXCOORD2FARBPROC __glewMultiTexCoord2fARB = null;
PFNGLMULTITEXCOORD2FVARBPROC __glewMultiTexCoord2fvARB = null;
PFNGLMULTITEXCOORD2IARBPROC __glewMultiTexCoord2iARB = null;
PFNGLMULTITEXCOORD2IVARBPROC __glewMultiTexCoord2ivARB = null;
PFNGLMULTITEXCOORD2SARBPROC __glewMultiTexCoord2sARB = null;
PFNGLMULTITEXCOORD2SVARBPROC __glewMultiTexCoord2svARB = null;
PFNGLMULTITEXCOORD3DARBPROC __glewMultiTexCoord3dARB = null;
PFNGLMULTITEXCOORD3DVARBPROC __glewMultiTexCoord3dvARB = null;
PFNGLMULTITEXCOORD3FARBPROC __glewMultiTexCoord3fARB = null;
PFNGLMULTITEXCOORD3FVARBPROC __glewMultiTexCoord3fvARB = null;
PFNGLMULTITEXCOORD3IARBPROC __glewMultiTexCoord3iARB = null;
PFNGLMULTITEXCOORD3IVARBPROC __glewMultiTexCoord3ivARB = null;
PFNGLMULTITEXCOORD3SARBPROC __glewMultiTexCoord3sARB = null;
PFNGLMULTITEXCOORD3SVARBPROC __glewMultiTexCoord3svARB = null;
PFNGLMULTITEXCOORD4DARBPROC __glewMultiTexCoord4dARB = null;
PFNGLMULTITEXCOORD4DVARBPROC __glewMultiTexCoord4dvARB = null;
PFNGLMULTITEXCOORD4FARBPROC __glewMultiTexCoord4fARB = null;
PFNGLMULTITEXCOORD4FVARBPROC __glewMultiTexCoord4fvARB = null;
PFNGLMULTITEXCOORD4IARBPROC __glewMultiTexCoord4iARB = null;
PFNGLMULTITEXCOORD4IVARBPROC __glewMultiTexCoord4ivARB = null;
PFNGLMULTITEXCOORD4SARBPROC __glewMultiTexCoord4sARB = null;
PFNGLMULTITEXCOORD4SVARBPROC __glewMultiTexCoord4svARB = null;

PFNGLBEGINQUERYARBPROC __glewBeginQueryARB = null;
PFNGLDELETEQUERIESARBPROC __glewDeleteQueriesARB = null;
PFNGLENDQUERYARBPROC __glewEndQueryARB = null;
PFNGLGENQUERIESARBPROC __glewGenQueriesARB = null;
PFNGLGETQUERYOBJECTIVARBPROC __glewGetQueryObjectivARB = null;
PFNGLGETQUERYOBJECTUIVARBPROC __glewGetQueryObjectuivARB = null;
PFNGLGETQUERYIVARBPROC __glewGetQueryivARB = null;
PFNGLISQUERYARBPROC __glewIsQueryARB = null;

PFNGLPOINTPARAMETERFARBPROC __glewPointParameterfARB = null;
PFNGLPOINTPARAMETERFVARBPROC __glewPointParameterfvARB = null;

PFNGLPROVOKINGVERTEXPROC __glewProvokingVertex = null;

PFNGLMINSAMPLESHADINGARBPROC __glewMinSampleShadingARB = null;

PFNGLATTACHOBJECTARBPROC __glewAttachObjectARB = null;
PFNGLCOMPILESHADERARBPROC __glewCompileShaderARB = null;
PFNGLCREATEPROGRAMOBJECTARBPROC __glewCreateProgramObjectARB = null;
PFNGLCREATESHADEROBJECTARBPROC __glewCreateShaderObjectARB = null;
PFNGLDELETEOBJECTARBPROC __glewDeleteObjectARB = null;
PFNGLDETACHOBJECTARBPROC __glewDetachObjectARB = null;
PFNGLGETACTIVEUNIFORMARBPROC __glewGetActiveUniformARB = null;
PFNGLGETATTACHEDOBJECTSARBPROC __glewGetAttachedObjectsARB = null;
PFNGLGETHANDLEARBPROC __glewGetHandleARB = null;
PFNGLGETINFOLOGARBPROC __glewGetInfoLogARB = null;
PFNGLGETOBJECTPARAMETERFVARBPROC __glewGetObjectParameterfvARB = null;
PFNGLGETOBJECTPARAMETERIVARBPROC __glewGetObjectParameterivARB = null;
PFNGLGETSHADERSOURCEARBPROC __glewGetShaderSourceARB = null;
PFNGLGETUNIFORMLOCATIONARBPROC __glewGetUniformLocationARB = null;
PFNGLGETUNIFORMFVARBPROC __glewGetUniformfvARB = null;
PFNGLGETUNIFORMIVARBPROC __glewGetUniformivARB = null;
PFNGLLINKPROGRAMARBPROC __glewLinkProgramARB = null;
PFNGLSHADERSOURCEARBPROC __glewShaderSourceARB = null;
PFNGLUNIFORM1FARBPROC __glewUniform1fARB = null;
PFNGLUNIFORM1FVARBPROC __glewUniform1fvARB = null;
PFNGLUNIFORM1IARBPROC __glewUniform1iARB = null;
PFNGLUNIFORM1IVARBPROC __glewUniform1ivARB = null;
PFNGLUNIFORM2FARBPROC __glewUniform2fARB = null;
PFNGLUNIFORM2FVARBPROC __glewUniform2fvARB = null;
PFNGLUNIFORM2IARBPROC __glewUniform2iARB = null;
PFNGLUNIFORM2IVARBPROC __glewUniform2ivARB = null;
PFNGLUNIFORM3FARBPROC __glewUniform3fARB = null;
PFNGLUNIFORM3FVARBPROC __glewUniform3fvARB = null;
PFNGLUNIFORM3IARBPROC __glewUniform3iARB = null;
PFNGLUNIFORM3IVARBPROC __glewUniform3ivARB = null;
PFNGLUNIFORM4FARBPROC __glewUniform4fARB = null;
PFNGLUNIFORM4FVARBPROC __glewUniform4fvARB = null;
PFNGLUNIFORM4IARBPROC __glewUniform4iARB = null;
PFNGLUNIFORM4IVARBPROC __glewUniform4ivARB = null;
PFNGLUNIFORMMATRIX2FVARBPROC __glewUniformMatrix2fvARB = null;
PFNGLUNIFORMMATRIX3FVARBPROC __glewUniformMatrix3fvARB = null;
PFNGLUNIFORMMATRIX4FVARBPROC __glewUniformMatrix4fvARB = null;
PFNGLUSEPROGRAMOBJECTARBPROC __glewUseProgramObjectARB = null;
PFNGLVALIDATEPROGRAMARBPROC __glewValidateProgramARB = null;

PFNGLCLIENTWAITSYNCPROC __glewClientWaitSync = null;
PFNGLDELETESYNCPROC __glewDeleteSync = null;
PFNGLFENCESYNCPROC __glewFenceSync = null;
PFNGLGETINTEGER64VPROC __glewGetInteger64v = null;
PFNGLGETSYNCIVPROC __glewGetSynciv = null;
PFNGLISSYNCPROC __glewIsSync = null;
PFNGLWAITSYNCPROC __glewWaitSync = null;

PFNGLTEXBUFFERARBPROC __glewTexBufferARB = null;

PFNGLCOMPRESSEDTEXIMAGE1DARBPROC __glewCompressedTexImage1DARB = null;
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC __glewCompressedTexImage2DARB = null;
PFNGLCOMPRESSEDTEXIMAGE3DARBPROC __glewCompressedTexImage3DARB = null;
PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC __glewCompressedTexSubImage1DARB = null;
PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC __glewCompressedTexSubImage2DARB = null;
PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC __glewCompressedTexSubImage3DARB = null;
PFNGLGETCOMPRESSEDTEXIMAGEARBPROC __glewGetCompressedTexImageARB = null;

PFNGLGETMULTISAMPLEFVPROC __glewGetMultisamplefv = null;
PFNGLSAMPLEMASKIPROC __glewSampleMaski = null;
PFNGLTEXIMAGE2DMULTISAMPLEPROC __glewTexImage2DMultisample = null;
PFNGLTEXIMAGE3DMULTISAMPLEPROC __glewTexImage3DMultisample = null;

PFNGLLOADTRANSPOSEMATRIXDARBPROC __glewLoadTransposeMatrixdARB = null;
PFNGLLOADTRANSPOSEMATRIXFARBPROC __glewLoadTransposeMatrixfARB = null;
PFNGLMULTTRANSPOSEMATRIXDARBPROC __glewMultTransposeMatrixdARB = null;
PFNGLMULTTRANSPOSEMATRIXFARBPROC __glewMultTransposeMatrixfARB = null;

PFNGLBINDBUFFERBASEPROC __glewBindBufferBase = null;
PFNGLBINDBUFFERRANGEPROC __glewBindBufferRange = null;
PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC __glewGetActiveUniformBlockName = null;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC __glewGetActiveUniformBlockiv = null;
PFNGLGETACTIVEUNIFORMNAMEPROC __glewGetActiveUniformName = null;
PFNGLGETACTIVEUNIFORMSIVPROC __glewGetActiveUniformsiv = null;
PFNGLGETINTEGERI_VPROC __glewGetIntegeri_v = null;
PFNGLGETUNIFORMBLOCKINDEXPROC __glewGetUniformBlockIndex = null;
PFNGLGETUNIFORMINDICESPROC __glewGetUniformIndices = null;
PFNGLUNIFORMBLOCKBINDINGPROC __glewUniformBlockBinding = null;

PFNGLBINDVERTEXARRAYPROC __glewBindVertexArray = null;
PFNGLDELETEVERTEXARRAYSPROC __glewDeleteVertexArrays = null;
PFNGLGENVERTEXARRAYSPROC __glewGenVertexArrays = null;
PFNGLISVERTEXARRAYPROC __glewIsVertexArray = null;

PFNGLVERTEXBLENDARBPROC __glewVertexBlendARB = null;
PFNGLWEIGHTPOINTERARBPROC __glewWeightPointerARB = null;
PFNGLWEIGHTBVARBPROC __glewWeightbvARB = null;
PFNGLWEIGHTDVARBPROC __glewWeightdvARB = null;
PFNGLWEIGHTFVARBPROC __glewWeightfvARB = null;
PFNGLWEIGHTIVARBPROC __glewWeightivARB = null;
PFNGLWEIGHTSVARBPROC __glewWeightsvARB = null;
PFNGLWEIGHTUBVARBPROC __glewWeightubvARB = null;
PFNGLWEIGHTUIVARBPROC __glewWeightuivARB = null;
PFNGLWEIGHTUSVARBPROC __glewWeightusvARB = null;

PFNGLBINDBUFFERARBPROC __glewBindBufferARB = null;
PFNGLBUFFERDATAARBPROC __glewBufferDataARB = null;
PFNGLBUFFERSUBDATAARBPROC __glewBufferSubDataARB = null;
PFNGLDELETEBUFFERSARBPROC __glewDeleteBuffersARB = null;
PFNGLGENBUFFERSARBPROC __glewGenBuffersARB = null;
PFNGLGETBUFFERPARAMETERIVARBPROC __glewGetBufferParameterivARB = null;
PFNGLGETBUFFERPOINTERVARBPROC __glewGetBufferPointervARB = null;
PFNGLGETBUFFERSUBDATAARBPROC __glewGetBufferSubDataARB = null;
PFNGLISBUFFERARBPROC __glewIsBufferARB = null;
PFNGLMAPBUFFERARBPROC __glewMapBufferARB = null;
PFNGLUNMAPBUFFERARBPROC __glewUnmapBufferARB = null;

PFNGLBINDPROGRAMARBPROC __glewBindProgramARB = null;
PFNGLDELETEPROGRAMSARBPROC __glewDeleteProgramsARB = null;
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC __glewDisableVertexAttribArrayARB = null;
PFNGLENABLEVERTEXATTRIBARRAYARBPROC __glewEnableVertexAttribArrayARB = null;
PFNGLGENPROGRAMSARBPROC __glewGenProgramsARB = null;
PFNGLGETPROGRAMENVPARAMETERDVARBPROC __glewGetProgramEnvParameterdvARB = null;
PFNGLGETPROGRAMENVPARAMETERFVARBPROC __glewGetProgramEnvParameterfvARB = null;
PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC __glewGetProgramLocalParameterdvARB = null;
PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC __glewGetProgramLocalParameterfvARB = null;
PFNGLGETPROGRAMSTRINGARBPROC __glewGetProgramStringARB = null;
PFNGLGETPROGRAMIVARBPROC __glewGetProgramivARB = null;
PFNGLGETVERTEXATTRIBPOINTERVARBPROC __glewGetVertexAttribPointervARB = null;
PFNGLGETVERTEXATTRIBDVARBPROC __glewGetVertexAttribdvARB = null;
PFNGLGETVERTEXATTRIBFVARBPROC __glewGetVertexAttribfvARB = null;
PFNGLGETVERTEXATTRIBIVARBPROC __glewGetVertexAttribivARB = null;
PFNGLISPROGRAMARBPROC __glewIsProgramARB = null;
PFNGLPROGRAMENVPARAMETER4DARBPROC __glewProgramEnvParameter4dARB = null;
PFNGLPROGRAMENVPARAMETER4DVARBPROC __glewProgramEnvParameter4dvARB = null;
PFNGLPROGRAMENVPARAMETER4FARBPROC __glewProgramEnvParameter4fARB = null;
PFNGLPROGRAMENVPARAMETER4FVARBPROC __glewProgramEnvParameter4fvARB = null;
PFNGLPROGRAMLOCALPARAMETER4DARBPROC __glewProgramLocalParameter4dARB = null;
PFNGLPROGRAMLOCALPARAMETER4DVARBPROC __glewProgramLocalParameter4dvARB = null;
PFNGLPROGRAMLOCALPARAMETER4FARBPROC __glewProgramLocalParameter4fARB = null;
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC __glewProgramLocalParameter4fvARB = null;
PFNGLPROGRAMSTRINGARBPROC __glewProgramStringARB = null;
PFNGLVERTEXATTRIB1DARBPROC __glewVertexAttrib1dARB = null;
PFNGLVERTEXATTRIB1DVARBPROC __glewVertexAttrib1dvARB = null;
PFNGLVERTEXATTRIB1FARBPROC __glewVertexAttrib1fARB = null;
PFNGLVERTEXATTRIB1FVARBPROC __glewVertexAttrib1fvARB = null;
PFNGLVERTEXATTRIB1SARBPROC __glewVertexAttrib1sARB = null;
PFNGLVERTEXATTRIB1SVARBPROC __glewVertexAttrib1svARB = null;
PFNGLVERTEXATTRIB2DARBPROC __glewVertexAttrib2dARB = null;
PFNGLVERTEXATTRIB2DVARBPROC __glewVertexAttrib2dvARB = null;
PFNGLVERTEXATTRIB2FARBPROC __glewVertexAttrib2fARB = null;
PFNGLVERTEXATTRIB2FVARBPROC __glewVertexAttrib2fvARB = null;
PFNGLVERTEXATTRIB2SARBPROC __glewVertexAttrib2sARB = null;
PFNGLVERTEXATTRIB2SVARBPROC __glewVertexAttrib2svARB = null;
PFNGLVERTEXATTRIB3DARBPROC __glewVertexAttrib3dARB = null;
PFNGLVERTEXATTRIB3DVARBPROC __glewVertexAttrib3dvARB = null;
PFNGLVERTEXATTRIB3FARBPROC __glewVertexAttrib3fARB = null;
PFNGLVERTEXATTRIB3FVARBPROC __glewVertexAttrib3fvARB = null;
PFNGLVERTEXATTRIB3SARBPROC __glewVertexAttrib3sARB = null;
PFNGLVERTEXATTRIB3SVARBPROC __glewVertexAttrib3svARB = null;
PFNGLVERTEXATTRIB4NBVARBPROC __glewVertexAttrib4NbvARB = null;
PFNGLVERTEXATTRIB4NIVARBPROC __glewVertexAttrib4NivARB = null;
PFNGLVERTEXATTRIB4NSVARBPROC __glewVertexAttrib4NsvARB = null;
PFNGLVERTEXATTRIB4NUBARBPROC __glewVertexAttrib4NubARB = null;
PFNGLVERTEXATTRIB4NUBVARBPROC __glewVertexAttrib4NubvARB = null;
PFNGLVERTEXATTRIB4NUIVARBPROC __glewVertexAttrib4NuivARB = null;
PFNGLVERTEXATTRIB4NUSVARBPROC __glewVertexAttrib4NusvARB = null;
PFNGLVERTEXATTRIB4BVARBPROC __glewVertexAttrib4bvARB = null;
PFNGLVERTEXATTRIB4DARBPROC __glewVertexAttrib4dARB = null;
PFNGLVERTEXATTRIB4DVARBPROC __glewVertexAttrib4dvARB = null;
PFNGLVERTEXATTRIB4FARBPROC __glewVertexAttrib4fARB = null;
PFNGLVERTEXATTRIB4FVARBPROC __glewVertexAttrib4fvARB = null;
PFNGLVERTEXATTRIB4IVARBPROC __glewVertexAttrib4ivARB = null;
PFNGLVERTEXATTRIB4SARBPROC __glewVertexAttrib4sARB = null;
PFNGLVERTEXATTRIB4SVARBPROC __glewVertexAttrib4svARB = null;
PFNGLVERTEXATTRIB4UBVARBPROC __glewVertexAttrib4ubvARB = null;
PFNGLVERTEXATTRIB4UIVARBPROC __glewVertexAttrib4uivARB = null;
PFNGLVERTEXATTRIB4USVARBPROC __glewVertexAttrib4usvARB = null;
PFNGLVERTEXATTRIBPOINTERARBPROC __glewVertexAttribPointerARB = null;

PFNGLBINDATTRIBLOCATIONARBPROC __glewBindAttribLocationARB = null;
PFNGLGETACTIVEATTRIBARBPROC __glewGetActiveAttribARB = null;
PFNGLGETATTRIBLOCATIONARBPROC __glewGetAttribLocationARB = null;

PFNGLWINDOWPOS2DARBPROC __glewWindowPos2dARB = null;
PFNGLWINDOWPOS2DVARBPROC __glewWindowPos2dvARB = null;
PFNGLWINDOWPOS2FARBPROC __glewWindowPos2fARB = null;
PFNGLWINDOWPOS2FVARBPROC __glewWindowPos2fvARB = null;
PFNGLWINDOWPOS2IARBPROC __glewWindowPos2iARB = null;
PFNGLWINDOWPOS2IVARBPROC __glewWindowPos2ivARB = null;
PFNGLWINDOWPOS2SARBPROC __glewWindowPos2sARB = null;
PFNGLWINDOWPOS2SVARBPROC __glewWindowPos2svARB = null;
PFNGLWINDOWPOS3DARBPROC __glewWindowPos3dARB = null;
PFNGLWINDOWPOS3DVARBPROC __glewWindowPos3dvARB = null;
PFNGLWINDOWPOS3FARBPROC __glewWindowPos3fARB = null;
PFNGLWINDOWPOS3FVARBPROC __glewWindowPos3fvARB = null;
PFNGLWINDOWPOS3IARBPROC __glewWindowPos3iARB = null;
PFNGLWINDOWPOS3IVARBPROC __glewWindowPos3ivARB = null;
PFNGLWINDOWPOS3SARBPROC __glewWindowPos3sARB = null;
PFNGLWINDOWPOS3SVARBPROC __glewWindowPos3svARB = null;

PFNGLDRAWBUFFERSATIPROC __glewDrawBuffersATI = null;

PFNGLDRAWELEMENTARRAYATIPROC __glewDrawElementArrayATI = null;
PFNGLDRAWRANGEELEMENTARRAYATIPROC __glewDrawRangeElementArrayATI = null;
PFNGLELEMENTPOINTERATIPROC __glewElementPointerATI = null;

PFNGLGETTEXBUMPPARAMETERFVATIPROC __glewGetTexBumpParameterfvATI = null;
PFNGLGETTEXBUMPPARAMETERIVATIPROC __glewGetTexBumpParameterivATI = null;
PFNGLTEXBUMPPARAMETERFVATIPROC __glewTexBumpParameterfvATI = null;
PFNGLTEXBUMPPARAMETERIVATIPROC __glewTexBumpParameterivATI = null;

PFNGLALPHAFRAGMENTOP1ATIPROC __glewAlphaFragmentOp1ATI = null;
PFNGLALPHAFRAGMENTOP2ATIPROC __glewAlphaFragmentOp2ATI = null;
PFNGLALPHAFRAGMENTOP3ATIPROC __glewAlphaFragmentOp3ATI = null;
PFNGLBEGINFRAGMENTSHADERATIPROC __glewBeginFragmentShaderATI = null;
PFNGLBINDFRAGMENTSHADERATIPROC __glewBindFragmentShaderATI = null;
PFNGLCOLORFRAGMENTOP1ATIPROC __glewColorFragmentOp1ATI = null;
PFNGLCOLORFRAGMENTOP2ATIPROC __glewColorFragmentOp2ATI = null;
PFNGLCOLORFRAGMENTOP3ATIPROC __glewColorFragmentOp3ATI = null;
PFNGLDELETEFRAGMENTSHADERATIPROC __glewDeleteFragmentShaderATI = null;
PFNGLENDFRAGMENTSHADERATIPROC __glewEndFragmentShaderATI = null;
PFNGLGENFRAGMENTSHADERSATIPROC __glewGenFragmentShadersATI = null;
PFNGLPASSTEXCOORDATIPROC __glewPassTexCoordATI = null;
PFNGLSAMPLEMAPATIPROC __glewSampleMapATI = null;
PFNGLSETFRAGMENTSHADERCONSTANTATIPROC __glewSetFragmentShaderConstantATI = null;

PFNGLMAPOBJECTBUFFERATIPROC __glewMapObjectBufferATI = null;
PFNGLUNMAPOBJECTBUFFERATIPROC __glewUnmapObjectBufferATI = null;

PFNGLPNTRIANGLESFATIPROC __glPNTrianglewesfATI = null;
PFNGLPNTRIANGLESIATIPROC __glPNTrianglewesiATI = null;

PFNGLSTENCILFUNCSEPARATEATIPROC __glewStencilFuncSeparateATI = null;
PFNGLSTENCILOPSEPARATEATIPROC __glewStencilOpSeparateATI = null;

PFNGLARRAYOBJECTATIPROC __glewArrayObjectATI = null;
PFNGLFREEOBJECTBUFFERATIPROC __glewFreeObjectBufferATI = null;
PFNGLGETARRAYOBJECTFVATIPROC __glewGetArrayObjectfvATI = null;
PFNGLGETARRAYOBJECTIVATIPROC __glewGetArrayObjectivATI = null;
PFNGLGETOBJECTBUFFERFVATIPROC __glewGetObjectBufferfvATI = null;
PFNGLGETOBJECTBUFFERIVATIPROC __glewGetObjectBufferivATI = null;
PFNGLGETVARIANTARRAYOBJECTFVATIPROC __glewGetVariantArrayObjectfvATI = null;
PFNGLGETVARIANTARRAYOBJECTIVATIPROC __glewGetVariantArrayObjectivATI = null;
PFNGLISOBJECTBUFFERATIPROC __glewIsObjectBufferATI = null;
PFNGLNEWOBJECTBUFFERATIPROC __glewNewObjectBufferATI = null;
PFNGLUPDATEOBJECTBUFFERATIPROC __glewUpdateObjectBufferATI = null;
PFNGLVARIANTARRAYOBJECTATIPROC __glewVariantArrayObjectATI = null;

PFNGLGETVERTEXATTRIBARRAYOBJECTFVATIPROC __glewGetVertexAttribArrayObjectfvATI = null;
PFNGLGETVERTEXATTRIBARRAYOBJECTIVATIPROC __glewGetVertexAttribArrayObjectivATI = null;
PFNGLVERTEXATTRIBARRAYOBJECTATIPROC __glewVertexAttribArrayObjectATI = null;

PFNGLCLIENTACTIVEVERTEXSTREAMATIPROC __glewClientActiveVertexStreamATI = null;
PFNGLNORMALSTREAM3BATIPROC __glewNormalStream3bATI = null;
PFNGLNORMALSTREAM3BVATIPROC __glewNormalStream3bvATI = null;
PFNGLNORMALSTREAM3DATIPROC __glewNormalStream3dATI = null;
PFNGLNORMALSTREAM3DVATIPROC __glewNormalStream3dvATI = null;
PFNGLNORMALSTREAM3FATIPROC __glewNormalStream3fATI = null;
PFNGLNORMALSTREAM3FVATIPROC __glewNormalStream3fvATI = null;
PFNGLNORMALSTREAM3IATIPROC __glewNormalStream3iATI = null;
PFNGLNORMALSTREAM3IVATIPROC __glewNormalStream3ivATI = null;
PFNGLNORMALSTREAM3SATIPROC __glewNormalStream3sATI = null;
PFNGLNORMALSTREAM3SVATIPROC __glewNormalStream3svATI = null;
PFNGLVERTEXBLENDENVFATIPROC __glewVertexBlendEnvfATI = null;
PFNGLVERTEXBLENDENVIATIPROC __glewVertexBlendEnviATI = null;
PFNGLVERTEXSTREAM2DATIPROC __glewVertexStream2dATI = null;
PFNGLVERTEXSTREAM2DVATIPROC __glewVertexStream2dvATI = null;
PFNGLVERTEXSTREAM2FATIPROC __glewVertexStream2fATI = null;
PFNGLVERTEXSTREAM2FVATIPROC __glewVertexStream2fvATI = null;
PFNGLVERTEXSTREAM2IATIPROC __glewVertexStream2iATI = null;
PFNGLVERTEXSTREAM2IVATIPROC __glewVertexStream2ivATI = null;
PFNGLVERTEXSTREAM2SATIPROC __glewVertexStream2sATI = null;
PFNGLVERTEXSTREAM2SVATIPROC __glewVertexStream2svATI = null;
PFNGLVERTEXSTREAM3DATIPROC __glewVertexStream3dATI = null;
PFNGLVERTEXSTREAM3DVATIPROC __glewVertexStream3dvATI = null;
PFNGLVERTEXSTREAM3FATIPROC __glewVertexStream3fATI = null;
PFNGLVERTEXSTREAM3FVATIPROC __glewVertexStream3fvATI = null;
PFNGLVERTEXSTREAM3IATIPROC __glewVertexStream3iATI = null;
PFNGLVERTEXSTREAM3IVATIPROC __glewVertexStream3ivATI = null;
PFNGLVERTEXSTREAM3SATIPROC __glewVertexStream3sATI = null;
PFNGLVERTEXSTREAM3SVATIPROC __glewVertexStream3svATI = null;
PFNGLVERTEXSTREAM4DATIPROC __glewVertexStream4dATI = null;
PFNGLVERTEXSTREAM4DVATIPROC __glewVertexStream4dvATI = null;
PFNGLVERTEXSTREAM4FATIPROC __glewVertexStream4fATI = null;
PFNGLVERTEXSTREAM4FVATIPROC __glewVertexStream4fvATI = null;
PFNGLVERTEXSTREAM4IATIPROC __glewVertexStream4iATI = null;
PFNGLVERTEXSTREAM4IVATIPROC __glewVertexStream4ivATI = null;
PFNGLVERTEXSTREAM4SATIPROC __glewVertexStream4sATI = null;
PFNGLVERTEXSTREAM4SVATIPROC __glewVertexStream4svATI = null;

PFNGLGETUNIFORMBUFFERSIZEEXTPROC __glewGetUniformBufferSizeEXT = null;
PFNGLGETUNIFORMOFFSETEXTPROC __glewGetUniformOffsetEXT = null;
PFNGLUNIFORMBUFFEREXTPROC __glewUniformBufferEXT = null;

PFNGLBLENDCOLOREXTPROC __glewBlendColorEXT = null;

PFNGLBLENDEQUATIONSEPARATEEXTPROC __glewBlendEquationSeparateEXT = null;

PFNGLBLENDFUNCSEPARATEEXTPROC __glewBlendFuncSeparateEXT = null;

PFNGLBLENDEQUATIONEXTPROC __glewBlendEquationEXT = null;

PFNGLCOLORSUBTABLEEXTPROC __glewColorSubTableEXT = null;
PFNGLCOPYCOLORSUBTABLEEXTPROC __glewCopyColorSubTableEXT = null;

PFNGLLOCKARRAYSEXTPROC __glewLockArraysEXT = null;
PFNGLUNLOCKARRAYSEXTPROC __glewUnlockArraysEXT = null;

PFNGLCONVOLUTIONFILTER1DEXTPROC __glewConvolutionFilter1DEXT = null;
PFNGLCONVOLUTIONFILTER2DEXTPROC __glewConvolutionFilter2DEXT = null;
PFNGLCONVOLUTIONPARAMETERFEXTPROC __glewConvolutionParameterfEXT = null;
PFNGLCONVOLUTIONPARAMETERFVEXTPROC __glewConvolutionParameterfvEXT = null;
PFNGLCONVOLUTIONPARAMETERIEXTPROC __glewConvolutionParameteriEXT = null;
PFNGLCONVOLUTIONPARAMETERIVEXTPROC __glewConvolutionParameterivEXT = null;
PFNGLCOPYCONVOLUTIONFILTER1DEXTPROC __glewCopyConvolutionFilter1DEXT = null;
PFNGLCOPYCONVOLUTIONFILTER2DEXTPROC __glewCopyConvolutionFilter2DEXT = null;
PFNGLGETCONVOLUTIONFILTEREXTPROC __glewGetConvolutionFilterEXT = null;
PFNGLGETCONVOLUTIONPARAMETERFVEXTPROC __glewGetConvolutionParameterfvEXT = null;
PFNGLGETCONVOLUTIONPARAMETERIVEXTPROC __glewGetConvolutionParameterivEXT = null;
PFNGLGETSEPARABLEFILTEREXTPROC __glewGetSeparableFilterEXT = null;
PFNGLSEPARABLEFILTER2DEXTPROC __glewSeparableFilter2DEXT = null;

PFNGLBINORMALPOINTEREXTPROC __glewBinormalPointerEXT = null;
PFNGLTANGENTPOINTEREXTPROC __glewTangentPointerEXT = null;

PFNGLCOPYTEXIMAGE1DEXTPROC __glewCopyTexImage1DEXT = null;
PFNGLCOPYTEXIMAGE2DEXTPROC __glewCopyTexImage2DEXT = null;
PFNGLCOPYTEXSUBIMAGE1DEXTPROC __glewCopyTexSubImage1DEXT = null;
PFNGLCOPYTEXSUBIMAGE2DEXTPROC __glewCopyTexSubImage2DEXT = null;
PFNGLCOPYTEXSUBIMAGE3DEXTPROC __glewCopyTexSubImage3DEXT = null;

PFNGLCULLPARAMETERDVEXTPROC __glewCullParameterdvEXT = null;
PFNGLCULLPARAMETERFVEXTPROC __glewCullParameterfvEXT = null;

PFNGLDEPTHBOUNDSEXTPROC __glewDepthBoundsEXT = null;

PFNGLBINDMULTITEXTUREEXTPROC __glewBindMultiTextureEXT = null;
PFNGLCHECKNAMEDFRAMEBUFFERSTATUSEXTPROC __glewCheckNamedFramebufferStatusEXT = null;
PFNGLCLIENTATTRIBDEFAULTEXTPROC __glewClientAttribDefaultEXT = null;
PFNGLCOMPRESSEDMULTITEXIMAGE1DEXTPROC __glewCompressedMultiTexImage1DEXT = null;
PFNGLCOMPRESSEDMULTITEXIMAGE2DEXTPROC __glewCompressedMultiTexImage2DEXT = null;
PFNGLCOMPRESSEDMULTITEXIMAGE3DEXTPROC __glewCompressedMultiTexImage3DEXT = null;
PFNGLCOMPRESSEDMULTITEXSUBIMAGE1DEXTPROC __glewCompressedMultiTexSubImage1DEXT = null;
PFNGLCOMPRESSEDMULTITEXSUBIMAGE2DEXTPROC __glewCompressedMultiTexSubImage2DEXT = null;
PFNGLCOMPRESSEDMULTITEXSUBIMAGE3DEXTPROC __glewCompressedMultiTexSubImage3DEXT = null;
PFNGLCOMPRESSEDTEXTUREIMAGE1DEXTPROC __glewCompressedTextureImage1DEXT = null;
PFNGLCOMPRESSEDTEXTUREIMAGE2DEXTPROC __glewCompressedTextureImage2DEXT = null;
PFNGLCOMPRESSEDTEXTUREIMAGE3DEXTPROC __glewCompressedTextureImage3DEXT = null;
PFNGLCOMPRESSEDTEXTURESUBIMAGE1DEXTPROC __glewCompressedTextureSubImage1DEXT = null;
PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXTPROC __glewCompressedTextureSubImage2DEXT = null;
PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXTPROC __glewCompressedTextureSubImage3DEXT = null;
PFNGLCOPYMULTITEXIMAGE1DEXTPROC __glewCopyMultiTexImage1DEXT = null;
PFNGLCOPYMULTITEXIMAGE2DEXTPROC __glewCopyMultiTexImage2DEXT = null;
PFNGLCOPYMULTITEXSUBIMAGE1DEXTPROC __glewCopyMultiTexSubImage1DEXT = null;
PFNGLCOPYMULTITEXSUBIMAGE2DEXTPROC __glewCopyMultiTexSubImage2DEXT = null;
PFNGLCOPYMULTITEXSUBIMAGE3DEXTPROC __glewCopyMultiTexSubImage3DEXT = null;
PFNGLCOPYTEXTUREIMAGE1DEXTPROC __glewCopyTextureImage1DEXT = null;
PFNGLCOPYTEXTUREIMAGE2DEXTPROC __glewCopyTextureImage2DEXT = null;
PFNGLCOPYTEXTURESUBIMAGE1DEXTPROC __glewCopyTextureSubImage1DEXT = null;
PFNGLCOPYTEXTURESUBIMAGE2DEXTPROC __glewCopyTextureSubImage2DEXT = null;
PFNGLCOPYTEXTURESUBIMAGE3DEXTPROC __glewCopyTextureSubImage3DEXT = null;
PFNGLDISABLECLIENTSTATEINDEXEDEXTPROC __glewDisableClientStateIndexedEXT = null;
PFNGLDISABLECLIENTSTATEIEXTPROC __glewDisableClientStateiEXT = null;
PFNGLDISABLEVERTEXARRAYATTRIBEXTPROC __glewDisableVertexArrayAttribEXT = null;
PFNGLDISABLEVERTEXARRAYEXTPROC __glewDisableVertexArrayEXT = null;
PFNGLENABLECLIENTSTATEINDEXEDEXTPROC __glewEnableClientStateIndexedEXT = null;
PFNGLENABLECLIENTSTATEIEXTPROC __glewEnableClientStateiEXT = null;
PFNGLENABLEVERTEXARRAYATTRIBEXTPROC __glewEnableVertexArrayAttribEXT = null;
PFNGLENABLEVERTEXARRAYEXTPROC __glewEnableVertexArrayEXT = null;
PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEEXTPROC __glewFlushMappedNamedBufferRangeEXT = null;
PFNGLFRAMEBUFFERDRAWBUFFEREXTPROC __glewFramebufferDrawBufferEXT = null;
PFNGLFRAMEBUFFERDRAWBUFFERSEXTPROC __glewFramebufferDrawBuffersEXT = null;
PFNGLFRAMEBUFFERREADBUFFEREXTPROC __glewFramebufferReadBufferEXT = null;
PFNGLGENERATEMULTITEXMIPMAPEXTPROC __glewGenerateMultiTexMipmapEXT = null;
PFNGLGENERATETEXTUREMIPMAPEXTPROC __glewGenerateTextureMipmapEXT = null;
PFNGLGETCOMPRESSEDMULTITEXIMAGEEXTPROC __glewGetCompressedMultiTexImageEXT = null;
PFNGLGETCOMPRESSEDTEXTUREIMAGEEXTPROC __glewGetCompressedTextureImageEXT = null;
PFNGLGETDOUBLEINDEXEDVEXTPROC __glewGetDoubleIndexedvEXT = null;
PFNGLGETDOUBLEI_VEXTPROC __glewGetDoublei_vEXT = null;
PFNGLGETFLOATINDEXEDVEXTPROC __glewGetFloatIndexedvEXT = null;
PFNGLGETFLOATI_VEXTPROC __glewGetFloati_vEXT = null;
PFNGLGETFRAMEBUFFERPARAMETERIVEXTPROC __glewGetFramebufferParameterivEXT = null;
PFNGLGETMULTITEXENVFVEXTPROC __glewGetMultiTexEnvfvEXT = null;
PFNGLGETMULTITEXENVIVEXTPROC __glewGetMultiTexEnvivEXT = null;
PFNGLGETMULTITEXGENDVEXTPROC __glewGetMultiTexGendvEXT = null;
PFNGLGETMULTITEXGENFVEXTPROC __glewGetMultiTexGenfvEXT = null;
PFNGLGETMULTITEXGENIVEXTPROC __glewGetMultiTexGenivEXT = null;
PFNGLGETMULTITEXIMAGEEXTPROC __glewGetMultiTexImageEXT = null;
PFNGLGETMULTITEXLEVELPARAMETERFVEXTPROC __glewGetMultiTexLevelParameterfvEXT = null;
PFNGLGETMULTITEXLEVELPARAMETERIVEXTPROC __glewGetMultiTexLevelParameterivEXT = null;
PFNGLGETMULTITEXPARAMETERIIVEXTPROC __glewGetMultiTexParameterIivEXT = null;
PFNGLGETMULTITEXPARAMETERIUIVEXTPROC __glewGetMultiTexParameterIuivEXT = null;
PFNGLGETMULTITEXPARAMETERFVEXTPROC __glewGetMultiTexParameterfvEXT = null;
PFNGLGETMULTITEXPARAMETERIVEXTPROC __glewGetMultiTexParameterivEXT = null;
PFNGLGETNAMEDBUFFERPARAMETERIVEXTPROC __glewGetNamedBufferParameterivEXT = null;
PFNGLGETNAMEDBUFFERPOINTERVEXTPROC __glewGetNamedBufferPointervEXT = null;
PFNGLGETNAMEDBUFFERSUBDATAEXTPROC __glewGetNamedBufferSubDataEXT = null;
PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC __glewGetNamedFramebufferAttachmentParameterivEXT = null;
PFNGLGETNAMEDPROGRAMLOCALPARAMETERIIVEXTPROC __glewGetNamedProgramLocalParameterIivEXT = null;
PFNGLGETNAMEDPROGRAMLOCALPARAMETERIUIVEXTPROC __glewGetNamedProgramLocalParameterIuivEXT = null;
PFNGLGETNAMEDPROGRAMLOCALPARAMETERDVEXTPROC __glewGetNamedProgramLocalParameterdvEXT = null;
PFNGLGETNAMEDPROGRAMLOCALPARAMETERFVEXTPROC __glewGetNamedProgramLocalParameterfvEXT = null;
PFNGLGETNAMEDPROGRAMSTRINGEXTPROC __glewGetNamedProgramStringEXT = null;
PFNGLGETNAMEDPROGRAMIVEXTPROC __glewGetNamedProgramivEXT = null;
PFNGLGETNAMEDRENDERBUFFERPARAMETERIVEXTPROC __glewGetNamedRenderbufferParameterivEXT = null;
PFNGLGETPOINTERINDEXEDVEXTPROC __glewGetPointerIndexedvEXT = null;
PFNGLGETPOINTERI_VEXTPROC __glewGetPointeri_vEXT = null;
PFNGLGETTEXTUREIMAGEEXTPROC __glewGetTextureImageEXT = null;
PFNGLGETTEXTURELEVELPARAMETERFVEXTPROC __glewGetTextureLevelParameterfvEXT = null;
PFNGLGETTEXTURELEVELPARAMETERIVEXTPROC __glewGetTextureLevelParameterivEXT = null;
PFNGLGETTEXTUREPARAMETERIIVEXTPROC __glewGetTextureParameterIivEXT = null;
PFNGLGETTEXTUREPARAMETERIUIVEXTPROC __glewGetTextureParameterIuivEXT = null;
PFNGLGETTEXTUREPARAMETERFVEXTPROC __glewGetTextureParameterfvEXT = null;
PFNGLGETTEXTUREPARAMETERIVEXTPROC __glewGetTextureParameterivEXT = null;
PFNGLGETVERTEXARRAYINTEGERI_VEXTPROC __glewGetVertexArrayIntegeri_vEXT = null;
PFNGLGETVERTEXARRAYINTEGERVEXTPROC __glewGetVertexArrayIntegervEXT = null;
PFNGLGETVERTEXARRAYPOINTERI_VEXTPROC __glewGetVertexArrayPointeri_vEXT = null;
PFNGLGETVERTEXARRAYPOINTERVEXTPROC __glewGetVertexArrayPointervEXT = null;
PFNGLMAPNAMEDBUFFEREXTPROC __glewMapNamedBufferEXT = null;
PFNGLMAPNAMEDBUFFERRANGEEXTPROC __glewMapNamedBufferRangeEXT = null;
PFNGLMATRIXFRUSTUMEXTPROC __glewMatrixFrustumEXT = null;
PFNGLMATRIXLOADIDENTITYEXTPROC __glewMatrixLoadIdentityEXT = null;
PFNGLMATRIXLOADTRANSPOSEDEXTPROC __glewMatrixLoadTransposedEXT = null;
PFNGLMATRIXLOADTRANSPOSEFEXTPROC __glewMatrixLoadTransposefEXT = null;
PFNGLMATRIXLOADDEXTPROC __glewMatrixLoaddEXT = null;
PFNGLMATRIXLOADFEXTPROC __glewMatrixLoadfEXT = null;
PFNGLMATRIXMULTTRANSPOSEDEXTPROC __glewMatrixMultTransposedEXT = null;
PFNGLMATRIXMULTTRANSPOSEFEXTPROC __glewMatrixMultTransposefEXT = null;
PFNGLMATRIXMULTDEXTPROC __glewMatrixMultdEXT = null;
PFNGLMATRIXMULTFEXTPROC __glewMatrixMultfEXT = null;
PFNGLMATRIXORTHOEXTPROC __glewMatrixOrthoEXT = null;
PFNGLMATRIXPOPEXTPROC __glewMatrixPopEXT = null;
PFNGLMATRIXPUSHEXTPROC __glewMatrixPushEXT = null;
PFNGLMATRIXROTATEDEXTPROC __glewMatrixRotatedEXT = null;
PFNGLMATRIXROTATEFEXTPROC __glewMatrixRotatefEXT = null;
PFNGLMATRIXSCALEDEXTPROC __glewMatrixScaledEXT = null;
PFNGLMATRIXSCALEFEXTPROC __glewMatrixScalefEXT = null;
PFNGLMATRIXTRANSLATEDEXTPROC __glewMatrixTranslatedEXT = null;
PFNGLMATRIXTRANSLATEFEXTPROC __glewMatrixTranslatefEXT = null;
PFNGLMULTITEXBUFFEREXTPROC __glewMultiTexBufferEXT = null;
PFNGLMULTITEXCOORDPOINTEREXTPROC __glewMultiTexCoordPointerEXT = null;
PFNGLMULTITEXENVFEXTPROC __glewMultiTexEnvfEXT = null;
PFNGLMULTITEXENVFVEXTPROC __glewMultiTexEnvfvEXT = null;
PFNGLMULTITEXENVIEXTPROC __glewMultiTexEnviEXT = null;
PFNGLMULTITEXENVIVEXTPROC __glewMultiTexEnvivEXT = null;
PFNGLMULTITEXGENDEXTPROC __glewMultiTexGendEXT = null;
PFNGLMULTITEXGENDVEXTPROC __glewMultiTexGendvEXT = null;
PFNGLMULTITEXGENFEXTPROC __glewMultiTexGenfEXT = null;
PFNGLMULTITEXGENFVEXTPROC __glewMultiTexGenfvEXT = null;
PFNGLMULTITEXGENIEXTPROC __glewMultiTexGeniEXT = null;
PFNGLMULTITEXGENIVEXTPROC __glewMultiTexGenivEXT = null;
PFNGLMULTITEXIMAGE1DEXTPROC __glewMultiTexImage1DEXT = null;
PFNGLMULTITEXIMAGE2DEXTPROC __glewMultiTexImage2DEXT = null;
PFNGLMULTITEXIMAGE3DEXTPROC __glewMultiTexImage3DEXT = null;
PFNGLMULTITEXPARAMETERIIVEXTPROC __glewMultiTexParameterIivEXT = null;
PFNGLMULTITEXPARAMETERIUIVEXTPROC __glewMultiTexParameterIuivEXT = null;
PFNGLMULTITEXPARAMETERFEXTPROC __glewMultiTexParameterfEXT = null;
PFNGLMULTITEXPARAMETERFVEXTPROC __glewMultiTexParameterfvEXT = null;
PFNGLMULTITEXPARAMETERIEXTPROC __glewMultiTexParameteriEXT = null;
PFNGLMULTITEXPARAMETERIVEXTPROC __glewMultiTexParameterivEXT = null;
PFNGLMULTITEXRENDERBUFFEREXTPROC __glewMultiTexRenderbufferEXT = null;
PFNGLMULTITEXSUBIMAGE1DEXTPROC __glewMultiTexSubImage1DEXT = null;
PFNGLMULTITEXSUBIMAGE2DEXTPROC __glewMultiTexSubImage2DEXT = null;
PFNGLMULTITEXSUBIMAGE3DEXTPROC __glewMultiTexSubImage3DEXT = null;
PFNGLNAMEDBUFFERDATAEXTPROC __glewNamedBufferDataEXT = null;
PFNGLNAMEDBUFFERSUBDATAEXTPROC __glewNamedBufferSubDataEXT = null;
PFNGLNAMEDCOPYBUFFERSUBDATAEXTPROC __glewNamedCopyBufferSubDataEXT = null;
PFNGLNAMEDFRAMEBUFFERRENDERBUFFEREXTPROC __glewNamedFramebufferRenderbufferEXT = null;
PFNGLNAMEDFRAMEBUFFERTEXTURE1DEXTPROC __glewNamedFramebufferTexture1DEXT = null;
PFNGLNAMEDFRAMEBUFFERTEXTURE2DEXTPROC __glewNamedFramebufferTexture2DEXT = null;
PFNGLNAMEDFRAMEBUFFERTEXTURE3DEXTPROC __glewNamedFramebufferTexture3DEXT = null;
PFNGLNAMEDFRAMEBUFFERTEXTUREEXTPROC __glewNamedFramebufferTextureEXT = null;
PFNGLNAMEDFRAMEBUFFERTEXTUREFACEEXTPROC __glewNamedFramebufferTextureFaceEXT = null;
PFNGLNAMEDFRAMEBUFFERTEXTURELAYEREXTPROC __glewNamedFramebufferTextureLayerEXT = null;
PFNGLNAMEDPROGRAMLOCALPARAMETER4DEXTPROC __glewNamedProgramLocalParameter4dEXT = null;
PFNGLNAMEDPROGRAMLOCALPARAMETER4DVEXTPROC __glewNamedProgramLocalParameter4dvEXT = null;
PFNGLNAMEDPROGRAMLOCALPARAMETER4FEXTPROC __glewNamedProgramLocalParameter4fEXT = null;
PFNGLNAMEDPROGRAMLOCALPARAMETER4FVEXTPROC __glewNamedProgramLocalParameter4fvEXT = null;
PFNGLNAMEDPROGRAMLOCALPARAMETERI4IEXTPROC __glewNamedProgramLocalParameterI4iEXT = null;
PFNGLNAMEDPROGRAMLOCALPARAMETERI4IVEXTPROC __glewNamedProgramLocalParameterI4ivEXT = null;
PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIEXTPROC __glewNamedProgramLocalParameterI4uiEXT = null;
PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIVEXTPROC __glewNamedProgramLocalParameterI4uivEXT = null;
PFNGLNAMEDPROGRAMLOCALPARAMETERS4FVEXTPROC __glewNamedProgramLocalParameters4fvEXT = null;
PFNGLNAMEDPROGRAMLOCALPARAMETERSI4IVEXTPROC __glewNamedProgramLocalParametersI4ivEXT = null;
PFNGLNAMEDPROGRAMLOCALPARAMETERSI4UIVEXTPROC __glewNamedProgramLocalParametersI4uivEXT = null;
PFNGLNAMEDPROGRAMSTRINGEXTPROC __glewNamedProgramStringEXT = null;
PFNGLNAMEDRENDERBUFFERSTORAGEEXTPROC __glewNamedRenderbufferStorageEXT = null;
PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLECOVERAGEEXTPROC __glewNamedRenderbufferStorageMultisampleCoverageEXT = null;
PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC __glewNamedRenderbufferStorageMultisampleEXT = null;
PFNGLPROGRAMUNIFORM1FEXTPROC __glewProgramUniform1fEXT = null;
PFNGLPROGRAMUNIFORM1FVEXTPROC __glewProgramUniform1fvEXT = null;
PFNGLPROGRAMUNIFORM1IEXTPROC __glewProgramUniform1iEXT = null;
PFNGLPROGRAMUNIFORM1IVEXTPROC __glewProgramUniform1ivEXT = null;
PFNGLPROGRAMUNIFORM1UIEXTPROC __glewProgramUniform1uiEXT = null;
PFNGLPROGRAMUNIFORM1UIVEXTPROC __glewProgramUniform1uivEXT = null;
PFNGLPROGRAMUNIFORM2FEXTPROC __glewProgramUniform2fEXT = null;
PFNGLPROGRAMUNIFORM2FVEXTPROC __glewProgramUniform2fvEXT = null;
PFNGLPROGRAMUNIFORM2IEXTPROC __glewProgramUniform2iEXT = null;
PFNGLPROGRAMUNIFORM2IVEXTPROC __glewProgramUniform2ivEXT = null;
PFNGLPROGRAMUNIFORM2UIEXTPROC __glewProgramUniform2uiEXT = null;
PFNGLPROGRAMUNIFORM2UIVEXTPROC __glewProgramUniform2uivEXT = null;
PFNGLPROGRAMUNIFORM3FEXTPROC __glewProgramUniform3fEXT = null;
PFNGLPROGRAMUNIFORM3FVEXTPROC __glewProgramUniform3fvEXT = null;
PFNGLPROGRAMUNIFORM3IEXTPROC __glewProgramUniform3iEXT = null;
PFNGLPROGRAMUNIFORM3IVEXTPROC __glewProgramUniform3ivEXT = null;
PFNGLPROGRAMUNIFORM3UIEXTPROC __glewProgramUniform3uiEXT = null;
PFNGLPROGRAMUNIFORM3UIVEXTPROC __glewProgramUniform3uivEXT = null;
PFNGLPROGRAMUNIFORM4FEXTPROC __glewProgramUniform4fEXT = null;
PFNGLPROGRAMUNIFORM4FVEXTPROC __glewProgramUniform4fvEXT = null;
PFNGLPROGRAMUNIFORM4IEXTPROC __glewProgramUniform4iEXT = null;
PFNGLPROGRAMUNIFORM4IVEXTPROC __glewProgramUniform4ivEXT = null;
PFNGLPROGRAMUNIFORM4UIEXTPROC __glewProgramUniform4uiEXT = null;
PFNGLPROGRAMUNIFORM4UIVEXTPROC __glewProgramUniform4uivEXT = null;
PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC __glewProgramUniformMatrix2fvEXT = null;
PFNGLPROGRAMUNIFORMMATRIX2X3FVEXTPROC __glewProgramUniformMatrix2x3fvEXT = null;
PFNGLPROGRAMUNIFORMMATRIX2X4FVEXTPROC __glewProgramUniformMatrix2x4fvEXT = null;
PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC __glewProgramUniformMatrix3fvEXT = null;
PFNGLPROGRAMUNIFORMMATRIX3X2FVEXTPROC __glewProgramUniformMatrix3x2fvEXT = null;
PFNGLPROGRAMUNIFORMMATRIX3X4FVEXTPROC __glewProgramUniformMatrix3x4fvEXT = null;
PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC __glewProgramUniformMatrix4fvEXT = null;
PFNGLPROGRAMUNIFORMMATRIX4X2FVEXTPROC __glewProgramUniformMatrix4x2fvEXT = null;
PFNGLPROGRAMUNIFORMMATRIX4X3FVEXTPROC __glewProgramUniformMatrix4x3fvEXT = null;
PFNGLPUSHCLIENTATTRIBDEFAULTEXTPROC __glewPushClientAttribDefaultEXT = null;
PFNGLTEXTUREBUFFEREXTPROC __glewTextureBufferEXT = null;
PFNGLTEXTUREIMAGE1DEXTPROC __glewTextureImage1DEXT = null;
PFNGLTEXTUREIMAGE2DEXTPROC __glewTextureImage2DEXT = null;
PFNGLTEXTUREIMAGE3DEXTPROC __glewTextureImage3DEXT = null;
PFNGLTEXTUREPARAMETERIIVEXTPROC __glewTextureParameterIivEXT = null;
PFNGLTEXTUREPARAMETERIUIVEXTPROC __glewTextureParameterIuivEXT = null;
PFNGLTEXTUREPARAMETERFEXTPROC __glewTextureParameterfEXT = null;
PFNGLTEXTUREPARAMETERFVEXTPROC __glewTextureParameterfvEXT = null;
PFNGLTEXTUREPARAMETERIEXTPROC __glewTextureParameteriEXT = null;
PFNGLTEXTUREPARAMETERIVEXTPROC __glewTextureParameterivEXT = null;
PFNGLTEXTURERENDERBUFFEREXTPROC __glewTextureRenderbufferEXT = null;
PFNGLTEXTURESUBIMAGE1DEXTPROC __glewTextureSubImage1DEXT = null;
PFNGLTEXTURESUBIMAGE2DEXTPROC __glewTextureSubImage2DEXT = null;
PFNGLTEXTURESUBIMAGE3DEXTPROC __glewTextureSubImage3DEXT = null;
PFNGLUNMAPNAMEDBUFFEREXTPROC __glewUnmapNamedBufferEXT = null;
PFNGLVERTEXARRAYCOLOROFFSETEXTPROC __glewVertexArrayColorOffsetEXT = null;
PFNGLVERTEXARRAYEDGEFLAGOFFSETEXTPROC __glewVertexArrayEdgeFlagOffsetEXT = null;
PFNGLVERTEXARRAYFOGCOORDOFFSETEXTPROC __glewVertexArrayFogCoordOffsetEXT = null;
PFNGLVERTEXARRAYINDEXOFFSETEXTPROC __glewVertexArrayIndexOffsetEXT = null;
PFNGLVERTEXARRAYMULTITEXCOORDOFFSETEXTPROC __glewVertexArrayMultiTexCoordOffsetEXT = null;
PFNGLVERTEXARRAYNORMALOFFSETEXTPROC __glewVertexArrayNormalOffsetEXT = null;
PFNGLVERTEXARRAYSECONDARYCOLOROFFSETEXTPROC __glewVertexArraySecondaryColorOffsetEXT = null;
PFNGLVERTEXARRAYTEXCOORDOFFSETEXTPROC __glewVertexArrayTexCoordOffsetEXT = null;
PFNGLVERTEXARRAYVERTEXATTRIBIOFFSETEXTPROC __glewVertexArrayVertexAttribIOffsetEXT = null;
PFNGLVERTEXARRAYVERTEXATTRIBOFFSETEXTPROC __glewVertexArrayVertexAttribOffsetEXT = null;
PFNGLVERTEXARRAYVERTEXOFFSETEXTPROC __glewVertexArrayVertexOffsetEXT = null;

PFNGLCOLORMASKINDEXEDEXTPROC __glewColorMaskIndexedEXT = null;
PFNGLDISABLEINDEXEDEXTPROC __glewDisableIndexedEXT = null;
PFNGLENABLEINDEXEDEXTPROC __glewEnableIndexedEXT = null;
PFNGLGETBOOLEANINDEXEDVEXTPROC __glewGetBooleanIndexedvEXT = null;
PFNGLGETINTEGERINDEXEDVEXTPROC __glewGetIntegerIndexedvEXT = null;
PFNGLISENABLEDINDEXEDEXTPROC __glewIsEnabledIndexedEXT = null;

PFNGLDRAWARRAYSINSTANCEDEXTPROC __glewDrawArraysInstancedEXT = null;
PFNGLDRAWELEMENTSINSTANCEDEXTPROC __glewDrawElementsInstancedEXT = null;

PFNGLDRAWRANGEELEMENTSEXTPROC __glewDrawRangeElementsEXT = null;

PFNGLFOGCOORDPOINTEREXTPROC __glewFogCoordPointerEXT = null;
PFNGLFOGCOORDDEXTPROC __glewFogCoorddEXT = null;
PFNGLFOGCOORDDVEXTPROC __glewFogCoorddvEXT = null;
PFNGLFOGCOORDFEXTPROC __glewFogCoordfEXT = null;
PFNGLFOGCOORDFVEXTPROC __glewFogCoordfvEXT = null;

PFNGLFRAGMENTCOLORMATERIALEXTPROC __glewFragmentColorMaterialEXT = null;
PFNGLFRAGMENTLIGHTMODELFEXTPROC __glewFragmentLightModelfEXT = null;
PFNGLFRAGMENTLIGHTMODELFVEXTPROC __glewFragmentLightModelfvEXT = null;
PFNGLFRAGMENTLIGHTMODELIEXTPROC __glewFragmentLightModeliEXT = null;
PFNGLFRAGMENTLIGHTMODELIVEXTPROC __glewFragmentLightModelivEXT = null;
PFNGLFRAGMENTLIGHTFEXTPROC __glewFragmentLightfEXT = null;
PFNGLFRAGMENTLIGHTFVEXTPROC __glewFragmentLightfvEXT = null;
PFNGLFRAGMENTLIGHTIEXTPROC __glewFragmentLightiEXT = null;
PFNGLFRAGMENTLIGHTIVEXTPROC __glewFragmentLightivEXT = null;
PFNGLFRAGMENTMATERIALFEXTPROC __glewFragmentMaterialfEXT = null;
PFNGLFRAGMENTMATERIALFVEXTPROC __glewFragmentMaterialfvEXT = null;
PFNGLFRAGMENTMATERIALIEXTPROC __glewFragmentMaterialiEXT = null;
PFNGLFRAGMENTMATERIALIVEXTPROC __glewFragmentMaterialivEXT = null;
PFNGLGETFRAGMENTLIGHTFVEXTPROC __glewGetFragmentLightfvEXT = null;
PFNGLGETFRAGMENTLIGHTIVEXTPROC __glewGetFragmentLightivEXT = null;
PFNGLGETFRAGMENTMATERIALFVEXTPROC __glewGetFragmentMaterialfvEXT = null;
PFNGLGETFRAGMENTMATERIALIVEXTPROC __glewGetFragmentMaterialivEXT = null;
PFNGLLIGHTENVIEXTPROC __glewLightEnviEXT = null;

PFNGLBLITFRAMEBUFFEREXTPROC __glewBlitFramebufferEXT = null;

PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC __glewRenderbufferStorageMultisampleEXT = null;

PFNGLBINDFRAMEBUFFEREXTPROC __glewBindFramebufferEXT = null;
PFNGLBINDRENDERBUFFEREXTPROC __glewBindRenderbufferEXT = null;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC __glewCheckFramebufferStatusEXT = null;
PFNGLDELETEFRAMEBUFFERSEXTPROC __glewDeleteFramebuffersEXT = null;
PFNGLDELETERENDERBUFFERSEXTPROC __glewDeleteRenderbuffersEXT = null;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC __glewFramebufferRenderbufferEXT = null;
PFNGLFRAMEBUFFERTEXTURE1DEXTPROC __glewFramebufferTexture1DEXT = null;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC __glewFramebufferTexture2DEXT = null;
PFNGLFRAMEBUFFERTEXTURE3DEXTPROC __glewFramebufferTexture3DEXT = null;
PFNGLGENFRAMEBUFFERSEXTPROC __glewGenFramebuffersEXT = null;
PFNGLGENRENDERBUFFERSEXTPROC __glewGenRenderbuffersEXT = null;
PFNGLGENERATEMIPMAPEXTPROC __glewGenerateMipmapEXT = null;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC __glewGetFramebufferAttachmentParameterivEXT = null;
PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC __glewGetRenderbufferParameterivEXT = null;
PFNGLISFRAMEBUFFEREXTPROC __glewIsFramebufferEXT = null;
PFNGLISRENDERBUFFEREXTPROC __glewIsRenderbufferEXT = null;
PFNGLRENDERBUFFERSTORAGEEXTPROC __glewRenderbufferStorageEXT = null;

PFNGLFRAMEBUFFERTEXTUREEXTPROC __glewFramebufferTextureEXT = null;
PFNGLFRAMEBUFFERTEXTUREFACEEXTPROC __glewFramebufferTextureFaceEXT = null;
PFNGLFRAMEBUFFERTEXTURELAYEREXTPROC __glewFramebufferTextureLayerEXT = null;
PFNGLPROGRAMPARAMETERIEXTPROC __glewProgramParameteriEXT = null;

PFNGLPROGRAMENVPARAMETERS4FVEXTPROC __glewProgramEnvParameters4fvEXT = null;
PFNGLPROGRAMLOCALPARAMETERS4FVEXTPROC __glewProgramLocalParameters4fvEXT = null;

PFNGLBINDFRAGDATALOCATIONEXTPROC __glewBindFragDataLocationEXT = null;
PFNGLGETFRAGDATALOCATIONEXTPROC __glewGetFragDataLocationEXT = null;
PFNGLGETUNIFORMUIVEXTPROC __glewGetUniformuivEXT = null;
PFNGLGETVERTEXATTRIBIIVEXTPROC __glewGetVertexAttribIivEXT = null;
PFNGLGETVERTEXATTRIBIUIVEXTPROC __glewGetVertexAttribIuivEXT = null;
PFNGLUNIFORM1UIEXTPROC __glewUniform1uiEXT = null;
PFNGLUNIFORM1UIVEXTPROC __glewUniform1uivEXT = null;
PFNGLUNIFORM2UIEXTPROC __glewUniform2uiEXT = null;
PFNGLUNIFORM2UIVEXTPROC __glewUniform2uivEXT = null;
PFNGLUNIFORM3UIEXTPROC __glewUniform3uiEXT = null;
PFNGLUNIFORM3UIVEXTPROC __glewUniform3uivEXT = null;
PFNGLUNIFORM4UIEXTPROC __glewUniform4uiEXT = null;
PFNGLUNIFORM4UIVEXTPROC __glewUniform4uivEXT = null;
PFNGLVERTEXATTRIBI1IEXTPROC __glewVertexAttribI1iEXT = null;
PFNGLVERTEXATTRIBI1IVEXTPROC __glewVertexAttribI1ivEXT = null;
PFNGLVERTEXATTRIBI1UIEXTPROC __glewVertexAttribI1uiEXT = null;
PFNGLVERTEXATTRIBI1UIVEXTPROC __glewVertexAttribI1uivEXT = null;
PFNGLVERTEXATTRIBI2IEXTPROC __glewVertexAttribI2iEXT = null;
PFNGLVERTEXATTRIBI2IVEXTPROC __glewVertexAttribI2ivEXT = null;
PFNGLVERTEXATTRIBI2UIEXTPROC __glewVertexAttribI2uiEXT = null;
PFNGLVERTEXATTRIBI2UIVEXTPROC __glewVertexAttribI2uivEXT = null;
PFNGLVERTEXATTRIBI3IEXTPROC __glewVertexAttribI3iEXT = null;
PFNGLVERTEXATTRIBI3IVEXTPROC __glewVertexAttribI3ivEXT = null;
PFNGLVERTEXATTRIBI3UIEXTPROC __glewVertexAttribI3uiEXT = null;
PFNGLVERTEXATTRIBI3UIVEXTPROC __glewVertexAttribI3uivEXT = null;
PFNGLVERTEXATTRIBI4BVEXTPROC __glewVertexAttribI4bvEXT = null;
PFNGLVERTEXATTRIBI4IEXTPROC __glewVertexAttribI4iEXT = null;
PFNGLVERTEXATTRIBI4IVEXTPROC __glewVertexAttribI4ivEXT = null;
PFNGLVERTEXATTRIBI4SVEXTPROC __glewVertexAttribI4svEXT = null;
PFNGLVERTEXATTRIBI4UBVEXTPROC __glewVertexAttribI4ubvEXT = null;
PFNGLVERTEXATTRIBI4UIEXTPROC __glewVertexAttribI4uiEXT = null;
PFNGLVERTEXATTRIBI4UIVEXTPROC __glewVertexAttribI4uivEXT = null;
PFNGLVERTEXATTRIBI4USVEXTPROC __glewVertexAttribI4usvEXT = null;
PFNGLVERTEXATTRIBIPOINTEREXTPROC __glewVertexAttribIPointerEXT = null;

PFNGLGETHISTOGRAMEXTPROC __glewGetHistogramEXT = null;
PFNGLGETHISTOGRAMPARAMETERFVEXTPROC __glewGetHistogramParameterfvEXT = null;
PFNGLGETHISTOGRAMPARAMETERIVEXTPROC __glewGetHistogramParameterivEXT = null;
PFNGLGETMINMAXEXTPROC __glewGetMinmaxEXT = null;
PFNGLGETMINMAXPARAMETERFVEXTPROC __glewGetMinmaxParameterfvEXT = null;
PFNGLGETMINMAXPARAMETERIVEXTPROC __glewGetMinmaxParameterivEXT = null;
PFNGLHISTOGRAMEXTPROC __glewHistogramEXT = null;
PFNGLMINMAXEXTPROC __glewMinmaxEXT = null;
PFNGLRESETHISTOGRAMEXTPROC __glewResetHistogramEXT = null;
PFNGLRESETMINMAXEXTPROC __glewResetMinmaxEXT = null;

PFNGLINDEXFUNCEXTPROC __glewIndexFuncEXT = null;

PFNGLINDEXMATERIALEXTPROC __glewIndexMaterialEXT = null;

PFNGLAPPLYTEXTUREEXTPROC __glewApplyTextureEXT = null;
PFNGLTEXTURELIGHTEXTPROC __glewTextureLightEXT = null;
PFNGLTEXTUREMATERIALEXTPROC __glewTextureMaterialEXT = null;

PFNGLMULTIDRAWARRAYSEXTPROC __glewMultiDrawArraysEXT = null;
PFNGLMULTIDRAWELEMENTSEXTPROC __glewMultiDrawElementsEXT = null;

PFNGLSAMPLEMASKEXTPROC __glewSampleMaskEXT = null;
PFNGLSAMPLEPATTERNEXTPROC __glewSamplePatternEXT = null;

PFNGLCOLORTABLEEXTPROC __glewColorTableEXT = null;
PFNGLGETCOLORTABLEEXTPROC __glewGetColorTableEXT = null;
PFNGLGETCOLORTABLEPARAMETERFVEXTPROC __glewGetColorTableParameterfvEXT = null;
PFNGLGETCOLORTABLEPARAMETERIVEXTPROC __glewGetColorTableParameterivEXT = null;

PFNGLGETPIXELTRANSFORMPARAMETERFVEXTPROC __glewGetPixelTransformParameterfvEXT = null;
PFNGLGETPIXELTRANSFORMPARAMETERIVEXTPROC __glewGetPixelTransformParameterivEXT = null;
PFNGLPIXELTRANSFORMPARAMETERFEXTPROC __glewPixelTransformParameterfEXT = null;
PFNGLPIXELTRANSFORMPARAMETERFVEXTPROC __glewPixelTransformParameterfvEXT = null;
PFNGLPIXELTRANSFORMPARAMETERIEXTPROC __glewPixelTransformParameteriEXT = null;
PFNGLPIXELTRANSFORMPARAMETERIVEXTPROC __glewPixelTransformParameterivEXT = null;

PFNGLPOINTPARAMETERFEXTPROC __glewPointParameterfEXT = null;
PFNGLPOINTPARAMETERFVEXTPROC __glewPointParameterfvEXT = null;

PFNGLPOLYGONOFFSETEXTPROC __glewPolygonOffsetEXT = null;

PFNGLPROVOKINGVERTEXEXTPROC __glewProvokingVertexEXT = null;

PFNGLBEGINSCENEEXTPROC __glewBeginSceneEXT = null;
PFNGLENDSCENEEXTPROC __glewEndSceneEXT = null;

PFNGLSECONDARYCOLOR3BEXTPROC __glewSecondaryColor3bEXT = null;
PFNGLSECONDARYCOLOR3BVEXTPROC __glewSecondaryColor3bvEXT = null;
PFNGLSECONDARYCOLOR3DEXTPROC __glewSecondaryColor3dEXT = null;
PFNGLSECONDARYCOLOR3DVEXTPROC __glewSecondaryColor3dvEXT = null;
PFNGLSECONDARYCOLOR3FEXTPROC __glewSecondaryColor3fEXT = null;
PFNGLSECONDARYCOLOR3FVEXTPROC __glewSecondaryColor3fvEXT = null;
PFNGLSECONDARYCOLOR3IEXTPROC __glewSecondaryColor3iEXT = null;
PFNGLSECONDARYCOLOR3IVEXTPROC __glewSecondaryColor3ivEXT = null;
PFNGLSECONDARYCOLOR3SEXTPROC __glewSecondaryColor3sEXT = null;
PFNGLSECONDARYCOLOR3SVEXTPROC __glewSecondaryColor3svEXT = null;
PFNGLSECONDARYCOLOR3UBEXTPROC __glewSecondaryColor3ubEXT = null;
PFNGLSECONDARYCOLOR3UBVEXTPROC __glewSecondaryColor3ubvEXT = null;
PFNGLSECONDARYCOLOR3UIEXTPROC __glewSecondaryColor3uiEXT = null;
PFNGLSECONDARYCOLOR3UIVEXTPROC __glewSecondaryColor3uivEXT = null;
PFNGLSECONDARYCOLOR3USEXTPROC __glewSecondaryColor3usEXT = null;
PFNGLSECONDARYCOLOR3USVEXTPROC __glewSecondaryColor3usvEXT = null;
PFNGLSECONDARYCOLORPOINTEREXTPROC __glewSecondaryColorPointerEXT = null;

PFNGLACTIVEPROGRAMEXTPROC __glewActiveProgramEXT = null;
PFNGLCREATESHADERPROGRAMEXTPROC __glewCreateShaderProgramEXT = null;
PFNGLUSESHADERPROGRAMEXTPROC __glewUseShaderProgramEXT = null;

PFNGLACTIVESTENCILFACEEXTPROC __glewActiveStencilFaceEXT = null;

PFNGLTEXSUBIMAGE1DEXTPROC __glewTexSubImage1DEXT = null;
PFNGLTEXSUBIMAGE2DEXTPROC __glewTexSubImage2DEXT = null;
PFNGLTEXSUBIMAGE3DEXTPROC __glewTexSubImage3DEXT = null;

PFNGLTEXIMAGE3DEXTPROC __glewTexImage3DEXT = null;

PFNGLTEXBUFFEREXTPROC __glewTexBufferEXT = null;

PFNGLCLEARCOLORIIEXTPROC __glewClearColorIiEXT = null;
PFNGLCLEARCOLORIUIEXTPROC __glewClearColorIuiEXT = null;
PFNGLGETTEXPARAMETERIIVEXTPROC __glewGetTexParameterIivEXT = null;
PFNGLGETTEXPARAMETERIUIVEXTPROC __glewGetTexParameterIuivEXT = null;
PFNGLTEXPARAMETERIIVEXTPROC __glewTexParameterIivEXT = null;
PFNGLTEXPARAMETERIUIVEXTPROC __glewTexParameterIuivEXT = null;

PFNGLARETEXTURESRESIDENTEXTPROC __glewAreTexturesResidentEXT = null;
PFNGLBINDTEXTUREEXTPROC __glewBindTextureEXT = null;
PFNGLDELETETEXTURESEXTPROC __glewDeleteTexturesEXT = null;
PFNGLGENTEXTURESEXTPROC __glewGenTexturesEXT = null;
PFNGLISTEXTUREEXTPROC __glewIsTextureEXT = null;
PFNGLPRIORITIZETEXTURESEXTPROC __glewPrioritizeTexturesEXT = null;

PFNGLTEXTURENORMALEXTPROC __glewTextureNormalEXT = null;

PFNGLGETQUERYOBJECTI64VEXTPROC __glewGetQueryObjecti64vEXT = null;
PFNGLGETQUERYOBJECTUI64VEXTPROC __glewGetQueryObjectui64vEXT = null;

PFNGLBEGINTRANSFORMFEEDBACKEXTPROC __glewBeginTransformFeedbackEXT = null;
PFNGLBINDBUFFERBASEEXTPROC __glewBindBufferBaseEXT = null;
PFNGLBINDBUFFEROFFSETEXTPROC __glewBindBufferOffsetEXT = null;
PFNGLBINDBUFFERRANGEEXTPROC __glewBindBufferRangeEXT = null;
PFNGLENDTRANSFORMFEEDBACKEXTPROC __glewEndTransformFeedbackEXT = null;
PFNGLGETTRANSFORMFEEDBACKVARYINGEXTPROC __glewGetTransformFeedbackVaryingEXT = null;
PFNGLTRANSFORMFEEDBACKVARYINGSEXTPROC __glewTransformFeedbackVaryingsEXT = null;

PFNGLARRAYELEMENTEXTPROC __glewArrayElementEXT = null;
PFNGLCOLORPOINTEREXTPROC __glewColorPointerEXT = null;
PFNGLDRAWARRAYSEXTPROC __glewDrawArraysEXT = null;
PFNGLEDGEFLAGPOINTEREXTPROC __glewEdgeFlagPointerEXT = null;
PFNGLGETPOINTERVEXTPROC __glewGetPointervEXT = null;
PFNGLINDEXPOINTEREXTPROC __glewIndexPointerEXT = null;
PFNGLNORMALPOINTEREXTPROC __glewNormalPointerEXT = null;
PFNGLTEXCOORDPOINTEREXTPROC __glewTexCoordPointerEXT = null;
PFNGLVERTEXPOINTEREXTPROC __glewVertexPointerEXT = null;

PFNGLBEGINVERTEXSHADEREXTPROC __glewBeginVertexShaderEXT = null;
PFNGLBINDLIGHTPARAMETEREXTPROC __glewBindLightParameterEXT = null;
PFNGLBINDMATERIALPARAMETEREXTPROC __glewBindMaterialParameterEXT = null;
PFNGLBINDPARAMETEREXTPROC __glewBindParameterEXT = null;
PFNGLBINDTEXGENPARAMETEREXTPROC __glewBindTexGenParameterEXT = null;
PFNGLBINDTEXTUREUNITPARAMETEREXTPROC __glewBindTextureUnitParameterEXT = null;
PFNGLBINDVERTEXSHADEREXTPROC __glewBindVertexShaderEXT = null;
PFNGLDELETEVERTEXSHADEREXTPROC __glewDeleteVertexShaderEXT = null;
PFNGLDISABLEVARIANTCLIENTSTATEEXTPROC __glewDisableVariantClientStateEXT = null;
PFNGLENABLEVARIANTCLIENTSTATEEXTPROC __glewEnableVariantClientStateEXT = null;
PFNGLENDVERTEXSHADEREXTPROC __glewEndVertexShaderEXT = null;
PFNGLEXTRACTCOMPONENTEXTPROC __glewExtractComponentEXT = null;
PFNGLGENSYMBOLSEXTPROC __glewGenSymbolsEXT = null;
PFNGLGENVERTEXSHADERSEXTPROC __glewGenVertexShadersEXT = null;
PFNGLGETINVARIANTBOOLEANVEXTPROC __glewGetInvariantBooleanvEXT = null;
PFNGLGETINVARIANTFLOATVEXTPROC __glewGetInvariantFloatvEXT = null;
PFNGLGETINVARIANTINTEGERVEXTPROC __glewGetInvariantIntegervEXT = null;
PFNGLGETLOCALCONSTANTBOOLEANVEXTPROC __glewGetLocalConstantBooleanvEXT = null;
PFNGLGETLOCALCONSTANTFLOATVEXTPROC __glewGetLocalConstantFloatvEXT = null;
PFNGLGETLOCALCONSTANTINTEGERVEXTPROC __glewGetLocalConstantIntegervEXT = null;
PFNGLGETVARIANTBOOLEANVEXTPROC __glewGetVariantBooleanvEXT = null;
PFNGLGETVARIANTFLOATVEXTPROC __glewGetVariantFloatvEXT = null;
PFNGLGETVARIANTINTEGERVEXTPROC __glewGetVariantIntegervEXT = null;
PFNGLGETVARIANTPOINTERVEXTPROC __glewGetVariantPointervEXT = null;
PFNGLINSERTCOMPONENTEXTPROC __glewInsertComponentEXT = null;
PFNGLISVARIANTENABLEDEXTPROC __glewIsVariantEnabledEXT = null;
PFNGLSETINVARIANTEXTPROC __glewSetInvariantEXT = null;
PFNGLSETLOCALCONSTANTEXTPROC __glewSetLocalConstantEXT = null;
PFNGLSHADEROP1EXTPROC __glewShaderOp1EXT = null;
PFNGLSHADEROP2EXTPROC __glewShaderOp2EXT = null;
PFNGLSHADEROP3EXTPROC __glewShaderOp3EXT = null;
PFNGLSWIZZLEEXTPROC __glewSwizzleEXT = null;
PFNGLVARIANTPOINTEREXTPROC __glewVariantPointerEXT = null;
PFNGLVARIANTBVEXTPROC __glewVariantbvEXT = null;
PFNGLVARIANTDVEXTPROC __glewVariantdvEXT = null;
PFNGLVARIANTFVEXTPROC __glewVariantfvEXT = null;
PFNGLVARIANTIVEXTPROC __glewVariantivEXT = null;
PFNGLVARIANTSVEXTPROC __glewVariantsvEXT = null;
PFNGLVARIANTUBVEXTPROC __glewVariantubvEXT = null;
PFNGLVARIANTUIVEXTPROC __glewVariantuivEXT = null;
PFNGLVARIANTUSVEXTPROC __glewVariantusvEXT = null;
PFNGLWRITEMASKEXTPROC __glewWriteMaskEXT = null;

PFNGLVERTEXWEIGHTPOINTEREXTPROC __glewVertexWeightPointerEXT = null;
PFNGLVERTEXWEIGHTFEXTPROC __glewVertexWeightfEXT = null;
PFNGLVERTEXWEIGHTFVEXTPROC __glewVertexWeightfvEXT = null;

PFNGLFRAMETERMINATORGREMEDYPROC __glewFrameTerminatorGREMEDY = null;

PFNGLSTRINGMARKERGREMEDYPROC __glewStringMarkerGREMEDY = null;

PFNGLGETIMAGETRANSFORMPARAMETERFVHPPROC __glewGetImageTransformParameterfvHP = null;
PFNGLGETIMAGETRANSFORMPARAMETERIVHPPROC __glewGetImageTransformParameterivHP = null;
PFNGLIMAGETRANSFORMPARAMETERFHPPROC __glewImageTransformParameterfHP = null;
PFNGLIMAGETRANSFORMPARAMETERFVHPPROC __glewImageTransformParameterfvHP = null;
PFNGLIMAGETRANSFORMPARAMETERIHPPROC __glewImageTransformParameteriHP = null;
PFNGLIMAGETRANSFORMPARAMETERIVHPPROC __glewImageTransformParameterivHP = null;

PFNGLMULTIMODEDRAWARRAYSIBMPROC __glewMultiModeDrawArraysIBM = null;
PFNGLMULTIMODEDRAWELEMENTSIBMPROC __glewMultiModeDrawElementsIBM = null;

PFNGLCOLORPOINTERLISTIBMPROC __glewColorPointerListIBM = null;
PFNGLEDGEFLAGPOINTERLISTIBMPROC __glewEdgeFlagPointerListIBM = null;
PFNGLFOGCOORDPOINTERLISTIBMPROC __glewFogCoordPointerListIBM = null;
PFNGLINDEXPOINTERLISTIBMPROC __glewIndexPointerListIBM = null;
PFNGLNORMALPOINTERLISTIBMPROC __glewNormalPointerListIBM = null;
PFNGLSECONDARYCOLORPOINTERLISTIBMPROC __glewSecondaryColorPointerListIBM = null;
PFNGLTEXCOORDPOINTERLISTIBMPROC __glewTexCoordPointerListIBM = null;
PFNGLVERTEXPOINTERLISTIBMPROC __glewVertexPointerListIBM = null;

PFNGLCOLORPOINTERVINTELPROC __glewColorPointervINTEL = null;
PFNGLNORMALPOINTERVINTELPROC __glewNormalPointervINTEL = null;
PFNGLTEXCOORDPOINTERVINTELPROC __glewTexCoordPointervINTEL = null;
PFNGLVERTEXPOINTERVINTELPROC __glewVertexPointervINTEL = null;

PFNGLTEXSCISSORFUNCINTELPROC __glewTexScissorFuncINTEL = null;
PFNGLTEXSCISSORINTELPROC __glewTexScissorINTEL = null;

PFNGLBUFFERREGIONENABLEDEXTPROC __glewBufferRegionEnabledEXT = null;
PFNGLDELETEBUFFERREGIONEXTPROC __glewDeleteBufferRegionEXT = null;
PFNGLDRAWBUFFERREGIONEXTPROC __glewDrawBufferRegionEXT = null;
PFNGLNEWBUFFERREGIONEXTPROC __glewNewBufferRegionEXT = null;
PFNGLREADBUFFERREGIONEXTPROC __glewReadBufferRegionEXT = null;

PFNGLRESIZEBUFFERSMESAPROC __glewResizeBuffersMESA = null;

PFNGLWINDOWPOS2DMESAPROC __glewWindowPos2dMESA = null;
PFNGLWINDOWPOS2DVMESAPROC __glewWindowPos2dvMESA = null;
PFNGLWINDOWPOS2FMESAPROC __glewWindowPos2fMESA = null;
PFNGLWINDOWPOS2FVMESAPROC __glewWindowPos2fvMESA = null;
PFNGLWINDOWPOS2IMESAPROC __glewWindowPos2iMESA = null;
PFNGLWINDOWPOS2IVMESAPROC __glewWindowPos2ivMESA = null;
PFNGLWINDOWPOS2SMESAPROC __glewWindowPos2sMESA = null;
PFNGLWINDOWPOS2SVMESAPROC __glewWindowPos2svMESA = null;
PFNGLWINDOWPOS3DMESAPROC __glewWindowPos3dMESA = null;
PFNGLWINDOWPOS3DVMESAPROC __glewWindowPos3dvMESA = null;
PFNGLWINDOWPOS3FMESAPROC __glewWindowPos3fMESA = null;
PFNGLWINDOWPOS3FVMESAPROC __glewWindowPos3fvMESA = null;
PFNGLWINDOWPOS3IMESAPROC __glewWindowPos3iMESA = null;
PFNGLWINDOWPOS3IVMESAPROC __glewWindowPos3ivMESA = null;
PFNGLWINDOWPOS3SMESAPROC __glewWindowPos3sMESA = null;
PFNGLWINDOWPOS3SVMESAPROC __glewWindowPos3svMESA = null;
PFNGLWINDOWPOS4DMESAPROC __glewWindowPos4dMESA = null;
PFNGLWINDOWPOS4DVMESAPROC __glewWindowPos4dvMESA = null;
PFNGLWINDOWPOS4FMESAPROC __glewWindowPos4fMESA = null;
PFNGLWINDOWPOS4FVMESAPROC __glewWindowPos4fvMESA = null;
PFNGLWINDOWPOS4IMESAPROC __glewWindowPos4iMESA = null;
PFNGLWINDOWPOS4IVMESAPROC __glewWindowPos4ivMESA = null;
PFNGLWINDOWPOS4SMESAPROC __glewWindowPos4sMESA = null;
PFNGLWINDOWPOS4SVMESAPROC __glewWindowPos4svMESA = null;

PFNGLBEGINCONDITIONALRENDERNVPROC __glewBeginConditionalRenderNV = null;
PFNGLENDCONDITIONALRENDERNVPROC __glewEndConditionalRenderNV = null;

PFNGLCOPYIMAGESUBDATANVPROC __glewCopyImageSubDataNV = null;

PFNGLCLEARDEPTHDNVPROC __glewClearDepthdNV = null;
PFNGLDEPTHBOUNDSDNVPROC __glewDepthBoundsdNV = null;
PFNGLDEPTHRANGEDNVPROC __glewDepthRangedNV = null;

PFNGLEVALMAPSNVPROC __glewEvalMapsNV = null;
PFNGLGETMAPATTRIBPARAMETERFVNVPROC __glewGetMapAttribParameterfvNV = null;
PFNGLGETMAPATTRIBPARAMETERIVNVPROC __glewGetMapAttribParameterivNV = null;
PFNGLGETMAPCONTROLPOINTSNVPROC __glewGetMapControlPointsNV = null;
PFNGLGETMAPPARAMETERFVNVPROC __glewGetMapParameterfvNV = null;
PFNGLGETMAPPARAMETERIVNVPROC __glewGetMapParameterivNV = null;
PFNGLMAPCONTROLPOINTSNVPROC __glewMapControlPointsNV = null;
PFNGLMAPPARAMETERFVNVPROC __glewMapParameterfvNV = null;
PFNGLMAPPARAMETERIVNVPROC __glewMapParameterivNV = null;

PFNGLGETMULTISAMPLEFVNVPROC __glewGetMultisamplefvNV = null;
PFNGLSAMPLEMASKINDEXEDNVPROC __glewSampleMaskIndexedNV = null;
PFNGLTEXRENDERBUFFERNVPROC __glewTexRenderbufferNV = null;

PFNGLDELETEFENCESNVPROC __glewDeleteFencesNV = null;
PFNGLFINISHFENCENVPROC __glewFinishFenceNV = null;
PFNGLGENFENCESNVPROC __glewGenFencesNV = null;
PFNGLGETFENCEIVNVPROC __glewGetFenceivNV = null;
PFNGLISFENCENVPROC __glewIsFenceNV = null;
PFNGLSETFENCENVPROC __glewSetFenceNV = null;
PFNGLTESTFENCENVPROC __glewTestFenceNV = null;

PFNGLGETPROGRAMNAMEDPARAMETERDVNVPROC __glewGetProgramNamedParameterdvNV = null;
PFNGLGETPROGRAMNAMEDPARAMETERFVNVPROC __glewGetProgramNamedParameterfvNV = null;
PFNGLPROGRAMNAMEDPARAMETER4DNVPROC __glewProgramNamedParameter4dNV = null;
PFNGLPROGRAMNAMEDPARAMETER4DVNVPROC __glewProgramNamedParameter4dvNV = null;
PFNGLPROGRAMNAMEDPARAMETER4FNVPROC __glewProgramNamedParameter4fNV = null;
PFNGLPROGRAMNAMEDPARAMETER4FVNVPROC __glewProgramNamedParameter4fvNV = null;

PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC __glewRenderbufferStorageMultisampleCoverageNV = null;

PFNGLPROGRAMVERTEXLIMITNVPROC __glewProgramVertexLimitNV = null;

PFNGLPROGRAMENVPARAMETERI4INVPROC __glewProgramEnvParameterI4iNV = null;
PFNGLPROGRAMENVPARAMETERI4IVNVPROC __glewProgramEnvParameterI4ivNV = null;
PFNGLPROGRAMENVPARAMETERI4UINVPROC __glewProgramEnvParameterI4uiNV = null;
PFNGLPROGRAMENVPARAMETERI4UIVNVPROC __glewProgramEnvParameterI4uivNV = null;
PFNGLPROGRAMENVPARAMETERSI4IVNVPROC __glewProgramEnvParametersI4ivNV = null;
PFNGLPROGRAMENVPARAMETERSI4UIVNVPROC __glewProgramEnvParametersI4uivNV = null;
PFNGLPROGRAMLOCALPARAMETERI4INVPROC __glewProgramLocalParameterI4iNV = null;
PFNGLPROGRAMLOCALPARAMETERI4IVNVPROC __glewProgramLocalParameterI4ivNV = null;
PFNGLPROGRAMLOCALPARAMETERI4UINVPROC __glewProgramLocalParameterI4uiNV = null;
PFNGLPROGRAMLOCALPARAMETERI4UIVNVPROC __glewProgramLocalParameterI4uivNV = null;
PFNGLPROGRAMLOCALPARAMETERSI4IVNVPROC __glewProgramLocalParametersI4ivNV = null;
PFNGLPROGRAMLOCALPARAMETERSI4UIVNVPROC __glewProgramLocalParametersI4uivNV = null;

PFNGLCOLOR3HNVPROC __glewColor3hNV = null;
PFNGLCOLOR3HVNVPROC __glewColor3hvNV = null;
PFNGLCOLOR4HNVPROC __glewColor4hNV = null;
PFNGLCOLOR4HVNVPROC __glewColor4hvNV = null;
PFNGLFOGCOORDHNVPROC __glewFogCoordhNV = null;
PFNGLFOGCOORDHVNVPROC __glewFogCoordhvNV = null;
PFNGLMULTITEXCOORD1HNVPROC __glewMultiTexCoord1hNV = null;
PFNGLMULTITEXCOORD1HVNVPROC __glewMultiTexCoord1hvNV = null;
PFNGLMULTITEXCOORD2HNVPROC __glewMultiTexCoord2hNV = null;
PFNGLMULTITEXCOORD2HVNVPROC __glewMultiTexCoord2hvNV = null;
PFNGLMULTITEXCOORD3HNVPROC __glewMultiTexCoord3hNV = null;
PFNGLMULTITEXCOORD3HVNVPROC __glewMultiTexCoord3hvNV = null;
PFNGLMULTITEXCOORD4HNVPROC __glewMultiTexCoord4hNV = null;
PFNGLMULTITEXCOORD4HVNVPROC __glewMultiTexCoord4hvNV = null;
PFNGLNORMAL3HNVPROC __glewNormal3hNV = null;
PFNGLNORMAL3HVNVPROC __glewNormal3hvNV = null;
PFNGLSECONDARYCOLOR3HNVPROC __glewSecondaryColor3hNV = null;
PFNGLSECONDARYCOLOR3HVNVPROC __glewSecondaryColor3hvNV = null;
PFNGLTEXCOORD1HNVPROC __glewTexCoord1hNV = null;
PFNGLTEXCOORD1HVNVPROC __glewTexCoord1hvNV = null;
PFNGLTEXCOORD2HNVPROC __glewTexCoord2hNV = null;
PFNGLTEXCOORD2HVNVPROC __glewTexCoord2hvNV = null;
PFNGLTEXCOORD3HNVPROC __glewTexCoord3hNV = null;
PFNGLTEXCOORD3HVNVPROC __glewTexCoord3hvNV = null;
PFNGLTEXCOORD4HNVPROC __glewTexCoord4hNV = null;
PFNGLTEXCOORD4HVNVPROC __glewTexCoord4hvNV = null;
PFNGLVERTEX2HNVPROC __glewVertex2hNV = null;
PFNGLVERTEX2HVNVPROC __glewVertex2hvNV = null;
PFNGLVERTEX3HNVPROC __glewVertex3hNV = null;
PFNGLVERTEX3HVNVPROC __glewVertex3hvNV = null;
PFNGLVERTEX4HNVPROC __glewVertex4hNV = null;
PFNGLVERTEX4HVNVPROC __glewVertex4hvNV = null;
PFNGLVERTEXATTRIB1HNVPROC __glewVertexAttrib1hNV = null;
PFNGLVERTEXATTRIB1HVNVPROC __glewVertexAttrib1hvNV = null;
PFNGLVERTEXATTRIB2HNVPROC __glewVertexAttrib2hNV = null;
PFNGLVERTEXATTRIB2HVNVPROC __glewVertexAttrib2hvNV = null;
PFNGLVERTEXATTRIB3HNVPROC __glewVertexAttrib3hNV = null;
PFNGLVERTEXATTRIB3HVNVPROC __glewVertexAttrib3hvNV = null;
PFNGLVERTEXATTRIB4HNVPROC __glewVertexAttrib4hNV = null;
PFNGLVERTEXATTRIB4HVNVPROC __glewVertexAttrib4hvNV = null;
PFNGLVERTEXATTRIBS1HVNVPROC __glewVertexAttribs1hvNV = null;
PFNGLVERTEXATTRIBS2HVNVPROC __glewVertexAttribs2hvNV = null;
PFNGLVERTEXATTRIBS3HVNVPROC __glewVertexAttribs3hvNV = null;
PFNGLVERTEXATTRIBS4HVNVPROC __glewVertexAttribs4hvNV = null;
PFNGLVERTEXWEIGHTHNVPROC __glewVertexWeighthNV = null;
PFNGLVERTEXWEIGHTHVNVPROC __glewVertexWeighthvNV = null;

PFNGLBEGINOCCLUSIONQUERYNVPROC __glewBeginOcclusionQueryNV = null;
PFNGLDELETEOCCLUSIONQUERIESNVPROC __glewDeleteOcclusionQueriesNV = null;
PFNGLENDOCCLUSIONQUERYNVPROC __glewEndOcclusionQueryNV = null;
PFNGLGENOCCLUSIONQUERIESNVPROC __glewGenOcclusionQueriesNV = null;
PFNGLGETOCCLUSIONQUERYIVNVPROC __glewGetOcclusionQueryivNV = null;
PFNGLGETOCCLUSIONQUERYUIVNVPROC __glewGetOcclusionQueryuivNV = null;
PFNGLISOCCLUSIONQUERYNVPROC __glewIsOcclusionQueryNV = null;

PFNGLPROGRAMBUFFERPARAMETERSIIVNVPROC __glewProgramBufferParametersIivNV = null;
PFNGLPROGRAMBUFFERPARAMETERSIUIVNVPROC __glewProgramBufferParametersIuivNV = null;
PFNGLPROGRAMBUFFERPARAMETERSFVNVPROC __glewProgramBufferParametersfvNV = null;

PFNGLFLUSHPIXELDATARANGENVPROC __glewFlushPixelDataRangeNV = null;
PFNGLPIXELDATARANGENVPROC __glewPixelDataRangeNV = null;

PFNGLPOINTPARAMETERINVPROC __glewPointParameteriNV = null;
PFNGLPOINTPARAMETERIVNVPROC __glewPointParameterivNV = null;

PFNGLGETVIDEOI64VNVPROC __glewGetVideoi64vNV = null;
PFNGLGETVIDEOIVNVPROC __glewGetVideoivNV = null;
PFNGLGETVIDEOUI64VNVPROC __glewGetVideoui64vNV = null;
PFNGLGETVIDEOUIVNVPROC __glewGetVideouivNV = null;
PFNGLPRESENTFRAMEDUALFILLNVPROC __glewPresentFrameDualFillNV = null;
PFNGLPRESENTFRAMEKEYEDNVPROC __glewPresentFrameKeyedNV = null;

PFNGLPRIMITIVERESTARTINDEXNVPROC __glewPrimitiveRestartIndexNV = null;
PFNGLPRIMITIVERESTARTNVPROC __glewPrimitiveRestartNV = null;

PFNGLCOMBINERINPUTNVPROC __glewCombinerInputNV = null;
PFNGLCOMBINEROUTPUTNVPROC __glewCombinerOutputNV = null;
PFNGLCOMBINERPARAMETERFNVPROC __glewCombinerParameterfNV = null;
PFNGLCOMBINERPARAMETERFVNVPROC __glewCombinerParameterfvNV = null;
PFNGLCOMBINERPARAMETERINVPROC __glewCombinerParameteriNV = null;
PFNGLCOMBINERPARAMETERIVNVPROC __glewCombinerParameterivNV = null;
PFNGLFINALCOMBINERINPUTNVPROC __glewFinalCombinerInputNV = null;
PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC __glewGetCombinerInputParameterfvNV = null;
PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC __glewGetCombinerInputParameterivNV = null;
PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC __glewGetCombinerOutputParameterfvNV = null;
PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC __glewGetCombinerOutputParameterivNV = null;
PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC __glewGetFinalCombinerInputParameterfvNV = null;
PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC __glewGetFinalCombinerInputParameterivNV = null;

PFNGLCOMBINERSTAGEPARAMETERFVNVPROC __glewCombinerStageParameterfvNV = null;
PFNGLGETCOMBINERSTAGEPARAMETERFVNVPROC __glewGetCombinerStageParameterfvNV = null;

PFNGLGETBUFFERPARAMETERUI64VNVPROC __glewGetBufferParameterui64vNV = null;
PFNGLGETINTEGERUI64VNVPROC __glewGetIntegerui64vNV = null;
PFNGLGETNAMEDBUFFERPARAMETERUI64VNVPROC __glewGetNamedBufferParameterui64vNV = null;
PFNGLGETUNIFORMUI64VNVPROC __glewGetUniformui64vNV = null;
PFNGLISBUFFERRESIDENTNVPROC __glewIsBufferResidentNV = null;
PFNGLISNAMEDBUFFERRESIDENTNVPROC __glewIsNamedBufferResidentNV = null;
PFNGLMAKEBUFFERNONRESIDENTNVPROC __glewMakeBufferNonResidentNV = null;
PFNGLMAKEBUFFERRESIDENTNVPROC __glewMakeBufferResidentNV = null;
PFNGLMAKENAMEDBUFFERNONRESIDENTNVPROC __glewMakeNamedBufferNonResidentNV = null;
PFNGLMAKENAMEDBUFFERRESIDENTNVPROC __glewMakeNamedBufferResidentNV = null;
PFNGLPROGRAMUNIFORMUI64NVPROC __glewProgramUniformui64NV = null;
PFNGLPROGRAMUNIFORMUI64VNVPROC __glewProgramUniformui64vNV = null;
PFNGLUNIFORMUI64NVPROC __glewUniformui64NV = null;
PFNGLUNIFORMUI64VNVPROC __glewUniformui64vNV = null;

PFNGLTEXTUREBARRIERNVPROC __glewTextureBarrierNV = null;

PFNGLACTIVEVARYINGNVPROC __glewActiveVaryingNV = null;
PFNGLBEGINTRANSFORMFEEDBACKNVPROC __glewBeginTransformFeedbackNV = null;
PFNGLBINDBUFFERBASENVPROC __glewBindBufferBaseNV = null;
PFNGLBINDBUFFEROFFSETNVPROC __glewBindBufferOffsetNV = null;
PFNGLBINDBUFFERRANGENVPROC __glewBindBufferRangeNV = null;
PFNGLENDTRANSFORMFEEDBACKNVPROC __glewEndTransformFeedbackNV = null;
PFNGLGETACTIVEVARYINGNVPROC __glewGetActiveVaryingNV = null;
PFNGLGETTRANSFORMFEEDBACKVARYINGNVPROC __glewGetTransformFeedbackVaryingNV = null;
PFNGLGETVARYINGLOCATIONNVPROC __glewGetVaryingLocationNV = null;
PFNGLTRANSFORMFEEDBACKATTRIBSNVPROC __glewTransformFeedbackAttribsNV = null;
PFNGLTRANSFORMFEEDBACKVARYINGSNVPROC __glewTransformFeedbackVaryingsNV = null;

PFNGLBINDTRANSFORMFEEDBACKNVPROC __glewBindTransformFeedbackNV = null;
PFNGLDELETETRANSFORMFEEDBACKSNVPROC __glewDeleteTransformFeedbacksNV = null;
PFNGLDRAWTRANSFORMFEEDBACKNVPROC __glewDrawTransformFeedbackNV = null;
PFNGLGENTRANSFORMFEEDBACKSNVPROC __glewGenTransformFeedbacksNV = null;
PFNGLISTRANSFORMFEEDBACKNVPROC __glewIsTransformFeedbackNV = null;
PFNGLPAUSETRANSFORMFEEDBACKNVPROC __glewPauseTransformFeedbackNV = null;
PFNGLRESUMETRANSFORMFEEDBACKNVPROC __glewResumeTransformFeedbackNV = null;

PFNGLFLUSHVERTEXARRAYRANGENVPROC __glewFlushVertexArrayRangeNV = null;
PFNGLVERTEXARRAYRANGENVPROC __glewVertexArrayRangeNV = null;

PFNGLBUFFERADDRESSRANGENVPROC __glewBufferAddressRangeNV = null;
PFNGLCOLORFORMATNVPROC __glewColorFormatNV = null;
PFNGLEDGEFLAGFORMATNVPROC __glewEdgeFlagFormatNV = null;
PFNGLFOGCOORDFORMATNVPROC __glewFogCoordFormatNV = null;
PFNGLGETINTEGERUI64I_VNVPROC __glewGetIntegerui64i_vNV = null;
PFNGLINDEXFORMATNVPROC __glewIndexFormatNV = null;
PFNGLNORMALFORMATNVPROC __glewNormalFormatNV = null;
PFNGLSECONDARYCOLORFORMATNVPROC __glewSecondaryColorFormatNV = null;
PFNGLTEXCOORDFORMATNVPROC __glewTexCoordFormatNV = null;
PFNGLVERTEXATTRIBFORMATNVPROC __glewVertexAttribFormatNV = null;
PFNGLVERTEXATTRIBIFORMATNVPROC __glewVertexAttribIFormatNV = null;
PFNGLVERTEXFORMATNVPROC __glewVertexFormatNV = null;

PFNGLAREPROGRAMSRESIDENTNVPROC __glewAreProgramsResidentNV = null;
PFNGLBINDPROGRAMNVPROC __glewBindProgramNV = null;
PFNGLDELETEPROGRAMSNVPROC __glewDeleteProgramsNV = null;
PFNGLEXECUTEPROGRAMNVPROC __glewExecuteProgramNV = null;
PFNGLGENPROGRAMSNVPROC __glewGenProgramsNV = null;
PFNGLGETPROGRAMPARAMETERDVNVPROC __glewGetProgramParameterdvNV = null;
PFNGLGETPROGRAMPARAMETERFVNVPROC __glewGetProgramParameterfvNV = null;
PFNGLGETPROGRAMSTRINGNVPROC __glewGetProgramStringNV = null;
PFNGLGETPROGRAMIVNVPROC __glewGetProgramivNV = null;
PFNGLGETTRACKMATRIXIVNVPROC __glewGetTrackMatrixivNV = null;
PFNGLGETVERTEXATTRIBPOINTERVNVPROC __glewGetVertexAttribPointervNV = null;
PFNGLGETVERTEXATTRIBDVNVPROC __glewGetVertexAttribdvNV = null;
PFNGLGETVERTEXATTRIBFVNVPROC __glewGetVertexAttribfvNV = null;
PFNGLGETVERTEXATTRIBIVNVPROC __glewGetVertexAttribivNV = null;
PFNGLISPROGRAMNVPROC __glewIsProgramNV = null;
PFNGLLOADPROGRAMNVPROC __glewLoadProgramNV = null;
PFNGLPROGRAMPARAMETER4DNVPROC __glewProgramParameter4dNV = null;
PFNGLPROGRAMPARAMETER4DVNVPROC __glewProgramParameter4dvNV = null;
PFNGLPROGRAMPARAMETER4FNVPROC __glewProgramParameter4fNV = null;
PFNGLPROGRAMPARAMETER4FVNVPROC __glewProgramParameter4fvNV = null;
PFNGLPROGRAMPARAMETERS4DVNVPROC __glewProgramParameters4dvNV = null;
PFNGLPROGRAMPARAMETERS4FVNVPROC __glewProgramParameters4fvNV = null;
PFNGLREQUESTRESIDENTPROGRAMSNVPROC __glewRequestResidentProgramsNV = null;
PFNGLTRACKMATRIXNVPROC __glewTrackMatrixNV = null;
PFNGLVERTEXATTRIB1DNVPROC __glewVertexAttrib1dNV = null;
PFNGLVERTEXATTRIB1DVNVPROC __glewVertexAttrib1dvNV = null;
PFNGLVERTEXATTRIB1FNVPROC __glewVertexAttrib1fNV = null;
PFNGLVERTEXATTRIB1FVNVPROC __glewVertexAttrib1fvNV = null;
PFNGLVERTEXATTRIB1SNVPROC __glewVertexAttrib1sNV = null;
PFNGLVERTEXATTRIB1SVNVPROC __glewVertexAttrib1svNV = null;
PFNGLVERTEXATTRIB2DNVPROC __glewVertexAttrib2dNV = null;
PFNGLVERTEXATTRIB2DVNVPROC __glewVertexAttrib2dvNV = null;
PFNGLVERTEXATTRIB2FNVPROC __glewVertexAttrib2fNV = null;
PFNGLVERTEXATTRIB2FVNVPROC __glewVertexAttrib2fvNV = null;
PFNGLVERTEXATTRIB2SNVPROC __glewVertexAttrib2sNV = null;
PFNGLVERTEXATTRIB2SVNVPROC __glewVertexAttrib2svNV = null;
PFNGLVERTEXATTRIB3DNVPROC __glewVertexAttrib3dNV = null;
PFNGLVERTEXATTRIB3DVNVPROC __glewVertexAttrib3dvNV = null;
PFNGLVERTEXATTRIB3FNVPROC __glewVertexAttrib3fNV = null;
PFNGLVERTEXATTRIB3FVNVPROC __glewVertexAttrib3fvNV = null;
PFNGLVERTEXATTRIB3SNVPROC __glewVertexAttrib3sNV = null;
PFNGLVERTEXATTRIB3SVNVPROC __glewVertexAttrib3svNV = null;
PFNGLVERTEXATTRIB4DNVPROC __glewVertexAttrib4dNV = null;
PFNGLVERTEXATTRIB4DVNVPROC __glewVertexAttrib4dvNV = null;
PFNGLVERTEXATTRIB4FNVPROC __glewVertexAttrib4fNV = null;
PFNGLVERTEXATTRIB4FVNVPROC __glewVertexAttrib4fvNV = null;
PFNGLVERTEXATTRIB4SNVPROC __glewVertexAttrib4sNV = null;
PFNGLVERTEXATTRIB4SVNVPROC __glewVertexAttrib4svNV = null;
PFNGLVERTEXATTRIB4UBNVPROC __glewVertexAttrib4ubNV = null;
PFNGLVERTEXATTRIB4UBVNVPROC __glewVertexAttrib4ubvNV = null;
PFNGLVERTEXATTRIBPOINTERNVPROC __glewVertexAttribPointerNV = null;
PFNGLVERTEXATTRIBS1DVNVPROC __glewVertexAttribs1dvNV = null;
PFNGLVERTEXATTRIBS1FVNVPROC __glewVertexAttribs1fvNV = null;
PFNGLVERTEXATTRIBS1SVNVPROC __glewVertexAttribs1svNV = null;
PFNGLVERTEXATTRIBS2DVNVPROC __glewVertexAttribs2dvNV = null;
PFNGLVERTEXATTRIBS2FVNVPROC __glewVertexAttribs2fvNV = null;
PFNGLVERTEXATTRIBS2SVNVPROC __glewVertexAttribs2svNV = null;
PFNGLVERTEXATTRIBS3DVNVPROC __glewVertexAttribs3dvNV = null;
PFNGLVERTEXATTRIBS3FVNVPROC __glewVertexAttribs3fvNV = null;
PFNGLVERTEXATTRIBS3SVNVPROC __glewVertexAttribs3svNV = null;
PFNGLVERTEXATTRIBS4DVNVPROC __glewVertexAttribs4dvNV = null;
PFNGLVERTEXATTRIBS4FVNVPROC __glewVertexAttribs4fvNV = null;
PFNGLVERTEXATTRIBS4SVNVPROC __glewVertexAttribs4svNV = null;
PFNGLVERTEXATTRIBS4UBVNVPROC __glewVertexAttribs4ubvNV = null;

PFNGLCLEARDEPTHFOESPROC __glewClearDepthfOES = null;
PFNGLCLIPPLANEFOESPROC __glewClipPlanefOES = null;
PFNGLDEPTHRANGEFOESPROC __glewDepthRangefOES = null;
PFNGLFRUSTUMFOESPROC __glewFrustumfOES = null;
PFNGLGETCLIPPLANEFOESPROC __glewGetClipPlanefOES = null;
PFNGLORTHOFOESPROC __glewOrthofOES = null;

PFNGLDETAILTEXFUNCSGISPROC __glewDetailTexFuncSGIS = null;
PFNGLGETDETAILTEXFUNCSGISPROC __glewGetDetailTexFuncSGIS = null;

PFNGLFOGFUNCSGISPROC __glewFogFuncSGIS = null;
PFNGLGETFOGFUNCSGISPROC __glewGetFogFuncSGIS = null;

PFNGLSAMPLEMASKSGISPROC __glewSampleMaskSGIS = null;
PFNGLSAMPLEPATTERNSGISPROC __glewSamplePatternSGIS = null;

PFNGLGETSHARPENTEXFUNCSGISPROC __glewGetSharpenTexFuncSGIS = null;
PFNGLSHARPENTEXFUNCSGISPROC __glewSharpenTexFuncSGIS = null;

PFNGLTEXIMAGE4DSGISPROC __glewTexImage4DSGIS = null;
PFNGLTEXSUBIMAGE4DSGISPROC __glewTexSubImage4DSGIS = null;

PFNGLGETTEXFILTERFUNCSGISPROC __glewGetTexFilterFuncSGIS = null;
PFNGLTEXFILTERFUNCSGISPROC __glewTexFilterFuncSGIS = null;

PFNGLASYNCMARKERSGIXPROC __glewAsyncMarkerSGIX = null;
PFNGLDELETEASYNCMARKERSSGIXPROC __glewDeleteAsyncMarkersSGIX = null;
PFNGLFINISHASYNCSGIXPROC __glewFinishAsyncSGIX = null;
PFNGLGENASYNCMARKERSSGIXPROC __glewGenAsyncMarkersSGIX = null;
PFNGLISASYNCMARKERSGIXPROC __glewIsAsyncMarkerSGIX = null;
PFNGLPOLLASYNCSGIXPROC __glewPollAsyncSGIX = null;

PFNGLFLUSHRASTERSGIXPROC __glewFlushRasterSGIX = null;

PFNGLTEXTUREFOGSGIXPROC __glewTextureFogSGIX = null;

PFNGLFRAGMENTCOLORMATERIALSGIXPROC __glewFragmentColorMaterialSGIX = null;
PFNGLFRAGMENTLIGHTMODELFSGIXPROC __glewFragmentLightModelfSGIX = null;
PFNGLFRAGMENTLIGHTMODELFVSGIXPROC __glewFragmentLightModelfvSGIX = null;
PFNGLFRAGMENTLIGHTMODELISGIXPROC __glewFragmentLightModeliSGIX = null;
PFNGLFRAGMENTLIGHTMODELIVSGIXPROC __glewFragmentLightModelivSGIX = null;
PFNGLFRAGMENTLIGHTFSGIXPROC __glewFragmentLightfSGIX = null;
PFNGLFRAGMENTLIGHTFVSGIXPROC __glewFragmentLightfvSGIX = null;
PFNGLFRAGMENTLIGHTISGIXPROC __glewFragmentLightiSGIX = null;
PFNGLFRAGMENTLIGHTIVSGIXPROC __glewFragmentLightivSGIX = null;
PFNGLFRAGMENTMATERIALFSGIXPROC __glewFragmentMaterialfSGIX = null;
PFNGLFRAGMENTMATERIALFVSGIXPROC __glewFragmentMaterialfvSGIX = null;
PFNGLFRAGMENTMATERIALISGIXPROC __glewFragmentMaterialiSGIX = null;
PFNGLFRAGMENTMATERIALIVSGIXPROC __glewFragmentMaterialivSGIX = null;
PFNGLGETFRAGMENTLIGHTFVSGIXPROC __glewGetFragmentLightfvSGIX = null;
PFNGLGETFRAGMENTLIGHTIVSGIXPROC __glewGetFragmentLightivSGIX = null;
PFNGLGETFRAGMENTMATERIALFVSGIXPROC __glewGetFragmentMaterialfvSGIX = null;
PFNGLGETFRAGMENTMATERIALIVSGIXPROC __glewGetFragmentMaterialivSGIX = null;

PFNGLFRAMEZOOMSGIXPROC __glewFrameZoomSGIX = null;

PFNGLPIXELTEXGENSGIXPROC __glewPixelTexGenSGIX = null;

PFNGLREFERENCEPLANESGIXPROC __glewReferencePlaneSGIX = null;

PFNGLSPRITEPARAMETERFSGIXPROC __glewSpriteParameterfSGIX = null;
PFNGLSPRITEPARAMETERFVSGIXPROC __glewSpriteParameterfvSGIX = null;
PFNGLSPRITEPARAMETERISGIXPROC __glewSpriteParameteriSGIX = null;
PFNGLSPRITEPARAMETERIVSGIXPROC __glewSpriteParameterivSGIX = null;

PFNGLTAGSAMPLEBUFFERSGIXPROC __glewTagSampleBufferSGIX = null;

PFNGLCOLORTABLEPARAMETERFVSGIPROC __glewColorTableParameterfvSGI = null;
PFNGLCOLORTABLEPARAMETERIVSGIPROC __glewColorTableParameterivSGI = null;
PFNGLCOLORTABLESGIPROC __glewColorTableSGI = null;
PFNGLCOPYCOLORTABLESGIPROC __glewCopyColorTableSGI = null;
PFNGLGETCOLORTABLEPARAMETERFVSGIPROC __glewGetColorTableParameterfvSGI = null;
PFNGLGETCOLORTABLEPARAMETERIVSGIPROC __glewGetColorTableParameterivSGI = null;
PFNGLGETCOLORTABLESGIPROC __glewGetColorTableSGI = null;

PFNGLFINISHTEXTURESUNXPROC __glewFinishTextureSUNX = null;

PFNGLGLOBALALPHAFACTORBSUNPROC __glewGlobalAlphaFactorbSUN = null;
PFNGLGLOBALALPHAFACTORDSUNPROC __glewGlobalAlphaFactordSUN = null;
PFNGLGLOBALALPHAFACTORFSUNPROC __glewGlobalAlphaFactorfSUN = null;
PFNGLGLOBALALPHAFACTORISUNPROC __glewGlobalAlphaFactoriSUN = null;
PFNGLGLOBALALPHAFACTORSSUNPROC __glewGlobalAlphaFactorsSUN = null;
PFNGLGLOBALALPHAFACTORUBSUNPROC __glewGlobalAlphaFactorubSUN = null;
PFNGLGLOBALALPHAFACTORUISUNPROC __glewGlobalAlphaFactoruiSUN = null;
PFNGLGLOBALALPHAFACTORUSSUNPROC __glewGlobalAlphaFactorusSUN = null;

PFNGLREADVIDEOPIXELSSUNPROC __glewReadVideoPixelsSUN = null;

PFNGLREPLACEMENTCODEPOINTERSUNPROC __glewReplacementCodePointerSUN = null;
PFNGLREPLACEMENTCODEUBSUNPROC __glewReplacementCodeubSUN = null;
PFNGLREPLACEMENTCODEUBVSUNPROC __glewReplacementCodeubvSUN = null;
PFNGLREPLACEMENTCODEUISUNPROC __glewReplacementCodeuiSUN = null;
PFNGLREPLACEMENTCODEUIVSUNPROC __glewReplacementCodeuivSUN = null;
PFNGLREPLACEMENTCODEUSSUNPROC __glewReplacementCodeusSUN = null;
PFNGLREPLACEMENTCODEUSVSUNPROC __glewReplacementCodeusvSUN = null;

PFNGLCOLOR3FVERTEX3FSUNPROC __glewColor3fVertex3fSUN = null;
PFNGLCOLOR3FVERTEX3FVSUNPROC __glewColor3fVertex3fvSUN = null;
PFNGLCOLOR4FNORMAL3FVERTEX3FSUNPROC __glewColor4fNormal3fVertex3fSUN = null;
PFNGLCOLOR4FNORMAL3FVERTEX3FVSUNPROC __glewColor4fNormal3fVertex3fvSUN = null;
PFNGLCOLOR4UBVERTEX2FSUNPROC __glewColor4ubVertex2fSUN = null;
PFNGLCOLOR4UBVERTEX2FVSUNPROC __glewColor4ubVertex2fvSUN = null;
PFNGLCOLOR4UBVERTEX3FSUNPROC __glewColor4ubVertex3fSUN = null;
PFNGLCOLOR4UBVERTEX3FVSUNPROC __glewColor4ubVertex3fvSUN = null;
PFNGLNORMAL3FVERTEX3FSUNPROC __glewNormal3fVertex3fSUN = null;
PFNGLNORMAL3FVERTEX3FVSUNPROC __glewNormal3fVertex3fvSUN = null;
PFNGLREPLACEMENTCODEUICOLOR3FVERTEX3FSUNPROC __glewReplacementCodeuiColor3fVertex3fSUN = null;
PFNGLREPLACEMENTCODEUICOLOR3FVERTEX3FVSUNPROC __glewReplacementCodeuiColor3fVertex3fvSUN = null;
PFNGLREPLACEMENTCODEUICOLOR4FNORMAL3FVERTEX3FSUNPROC __glewReplacementCodeuiColor4fNormal3fVertex3fSUN = null;
PFNGLREPLACEMENTCODEUICOLOR4FNORMAL3FVERTEX3FVSUNPROC __glewReplacementCodeuiColor4fNormal3fVertex3fvSUN = null;
PFNGLREPLACEMENTCODEUICOLOR4UBVERTEX3FSUNPROC __glewReplacementCodeuiColor4ubVertex3fSUN = null;
PFNGLREPLACEMENTCODEUICOLOR4UBVERTEX3FVSUNPROC __glewReplacementCodeuiColor4ubVertex3fvSUN = null;
PFNGLREPLACEMENTCODEUINORMAL3FVERTEX3FSUNPROC __glewReplacementCodeuiNormal3fVertex3fSUN = null;
PFNGLREPLACEMENTCODEUINORMAL3FVERTEX3FVSUNPROC __glewReplacementCodeuiNormal3fVertex3fvSUN = null;
PFNGLREPLACEMENTCODEUITEXCOORD2FCOLOR4FNORMAL3FVERTEX3FSUNPROC __glewReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN = null;
PFNGLREPLACEMENTCODEUITEXCOORD2FCOLOR4FNORMAL3FVERTEX3FVSUNPROC __glewReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN = null;
PFNGLREPLACEMENTCODEUITEXCOORD2FNORMAL3FVERTEX3FSUNPROC __glewReplacementCodeuiTexCoord2fNormal3fVertex3fSUN = null;
PFNGLREPLACEMENTCODEUITEXCOORD2FNORMAL3FVERTEX3FVSUNPROC __glewReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN = null;
PFNGLREPLACEMENTCODEUITEXCOORD2FVERTEX3FSUNPROC __glewReplacementCodeuiTexCoord2fVertex3fSUN = null;
PFNGLREPLACEMENTCODEUITEXCOORD2FVERTEX3FVSUNPROC __glewReplacementCodeuiTexCoord2fVertex3fvSUN = null;
PFNGLREPLACEMENTCODEUIVERTEX3FSUNPROC __glewReplacementCodeuiVertex3fSUN = null;
PFNGLREPLACEMENTCODEUIVERTEX3FVSUNPROC __glewReplacementCodeuiVertex3fvSUN = null;
PFNGLTEXCOORD2FCOLOR3FVERTEX3FSUNPROC __glewTexCoord2fColor3fVertex3fSUN = null;
PFNGLTEXCOORD2FCOLOR3FVERTEX3FVSUNPROC __glewTexCoord2fColor3fVertex3fvSUN = null;
PFNGLTEXCOORD2FCOLOR4FNORMAL3FVERTEX3FSUNPROC __glewTexCoord2fColor4fNormal3fVertex3fSUN = null;
PFNGLTEXCOORD2FCOLOR4FNORMAL3FVERTEX3FVSUNPROC __glewTexCoord2fColor4fNormal3fVertex3fvSUN = null;
PFNGLTEXCOORD2FCOLOR4UBVERTEX3FSUNPROC __glewTexCoord2fColor4ubVertex3fSUN = null;
PFNGLTEXCOORD2FCOLOR4UBVERTEX3FVSUNPROC __glewTexCoord2fColor4ubVertex3fvSUN = null;
PFNGLTEXCOORD2FNORMAL3FVERTEX3FSUNPROC __glewTexCoord2fNormal3fVertex3fSUN = null;
PFNGLTEXCOORD2FNORMAL3FVERTEX3FVSUNPROC __glewTexCoord2fNormal3fVertex3fvSUN = null;
PFNGLTEXCOORD2FVERTEX3FSUNPROC __glewTexCoord2fVertex3fSUN = null;
PFNGLTEXCOORD2FVERTEX3FVSUNPROC __glewTexCoord2fVertex3fvSUN = null;
PFNGLTEXCOORD4FCOLOR4FNORMAL3FVERTEX4FSUNPROC __glewTexCoord4fColor4fNormal3fVertex4fSUN = null;
PFNGLTEXCOORD4FCOLOR4FNORMAL3FVERTEX4FVSUNPROC __glewTexCoord4fColor4fNormal3fVertex4fvSUN = null;
PFNGLTEXCOORD4FVERTEX4FSUNPROC __glewTexCoord4fVertex4fSUN = null;
PFNGLTEXCOORD4FVERTEX4FVSUNPROC __glewTexCoord4fVertex4fvSUN = null;

PFNGLADDSWAPHINTRECTWINPROC __glewAddSwapHintRectWIN = null;

#endif /* !WIN32 || !GLEW_MX */

#if !defined(GLEW_MX)

GLboolean __GLEW_VERSION_1_1 = GL_FALSE;
GLboolean __GLEW_VERSION_1_2 = GL_FALSE;
GLboolean __GLEW_VERSION_1_3 = GL_FALSE;
GLboolean __GLEW_VERSION_1_4 = GL_FALSE;
GLboolean __GLEW_VERSION_1_5 = GL_FALSE;
GLboolean __GLEW_VERSION_2_0 = GL_FALSE;
GLboolean __GLEW_VERSION_2_1 = GL_FALSE;
GLboolean __GLEW_VERSION_3_0 = GL_FALSE;
GLboolean __GLEW_VERSION_3_1 = GL_FALSE;
GLboolean __GLEW_VERSION_3_2 = GL_FALSE;
GLboolean __GLEW_3DFX_multisample = GL_FALSE;
GLboolean __GLEW_3DFX_tbuffer = GL_FALSE;
GLboolean __GLEW_3DFX_texture_compression_FXT1 = GL_FALSE;
GLboolean __GLEW_AMD_draw_buffers_blend = GL_FALSE;
GLboolean __GLEW_AMD_performance_monitor = GL_FALSE;
GLboolean __GLEW_AMD_seamless_cubemap_per_texture = GL_FALSE;
GLboolean __GLEW_AMD_shader_stencil_export = GL_FALSE;
GLboolean __GLEW_AMD_texture_texture4 = GL_FALSE;
GLboolean __GLEW_AMD_vertex_shader_tessellator = GL_FALSE;
GLboolean __GLEW_APPLE_aux_depth_stencil = GL_FALSE;
GLboolean __GLEW_APPLE_client_storage = GL_FALSE;
GLboolean __GLEW_APPLE_element_array = GL_FALSE;
GLboolean __GLEW_APPLE_fence = GL_FALSE;
GLboolean __GLEW_APPLE_float_pixels = GL_FALSE;
GLboolean __GLEW_APPLE_flush_buffer_range = GL_FALSE;
GLboolean __GLEW_APPLE_object_purgeable = GL_FALSE;
GLboolean __GLEW_APPLE_pixel_buffer = GL_FALSE;
GLboolean __GLEW_APPLE_rgb_422 = GL_FALSE;
GLboolean __GLEW_APPLE_row_bytes = GL_FALSE;
GLboolean __GLEW_APPLE_specular_vector = GL_FALSE;
GLboolean __GLEW_APPLE_texture_range = GL_FALSE;
GLboolean __GLEW_APPLE_transform_hint = GL_FALSE;
GLboolean __GLEW_APPLE_vertex_array_object = GL_FALSE;
GLboolean __GLEW_APPLE_vertex_array_range = GL_FALSE;
GLboolean __GLEW_APPLE_vertex_program_evaluators = GL_FALSE;
GLboolean __GLEW_APPLE_ycbcr_422 = GL_FALSE;
GLboolean __GLEW_ARB_color_buffer_float = GL_FALSE;
GLboolean __GLEW_ARB_compatibility = GL_FALSE;
GLboolean __GLEW_ARB_copy_buffer = GL_FALSE;
GLboolean __GLEW_ARB_depth_buffer_float = GL_FALSE;
GLboolean __GLEW_ARB_depth_clamp = GL_FALSE;
GLboolean __GLEW_ARB_depth_texture = GL_FALSE;
GLboolean __GLEW_ARB_draw_buffers = GL_FALSE;
GLboolean __GLEW_ARB_draw_buffers_blend = GL_FALSE;
GLboolean __GLEW_ARB_draw_elements_base_vertex = GL_FALSE;
GLboolean __GLEW_ARB_draw_instanced = GL_FALSE;
GLboolean __GLEW_ARB_fragment_coord_conventions = GL_FALSE;
GLboolean __GLEW_ARB_fragment_program = GL_FALSE;
GLboolean __GLEW_ARB_fragment_program_shadow = GL_FALSE;
GLboolean __GLEW_ARB_fragment_shader = GL_FALSE;
GLboolean __GLEW_ARB_framebuffer_object = GL_FALSE;
GLboolean __GLEW_ARB_framebuffer_sRGB = GL_FALSE;
GLboolean __GLEW_ARB_geometry_shader4 = GL_FALSE;
GLboolean __GLEW_ARB_half_float_pixel = GL_FALSE;
GLboolean __GLEW_ARB_half_float_vertex = GL_FALSE;
GLboolean __GLEW_ARB_imaging = GL_FALSE;
GLboolean __GLEW_ARB_instanced_arrays = GL_FALSE;
GLboolean __GLEW_ARB_map_buffer_range = GL_FALSE;
GLboolean __GLEW_ARB_matrix_palette = GL_FALSE;
GLboolean __GLEW_ARB_multisample = GL_FALSE;
GLboolean __GLEW_ARB_multitexture = GL_FALSE;
GLboolean __GLEW_ARB_occlusion_query = GL_FALSE;
GLboolean __GLEW_ARB_pixel_buffer_object = GL_FALSE;
GLboolean __GLEW_ARB_point_parameters = GL_FALSE;
GLboolean __GLEW_ARB_point_sprite = GL_FALSE;
GLboolean __GLEW_ARB_provoking_vertex = GL_FALSE;
GLboolean __GLEW_ARB_sample_shading = GL_FALSE;
GLboolean __GLEW_ARB_seamless_cube_map = GL_FALSE;
GLboolean __GLEW_ARB_shader_objects = GL_FALSE;
GLboolean __GLEW_ARB_shader_texture_lod = GL_FALSE;
GLboolean __GLEW_ARB_shading_language_100 = GL_FALSE;
GLboolean __GLEW_ARB_shadow = GL_FALSE;
GLboolean __GLEW_ARB_shadow_ambient = GL_FALSE;
GLboolean __GLEW_ARB_sync = GL_FALSE;
GLboolean __GLEW_ARB_texture_border_clamp = GL_FALSE;
GLboolean __GLEW_ARB_texture_buffer_object = GL_FALSE;
GLboolean __GLEW_ARB_texture_compression = GL_FALSE;
GLboolean __GLEW_ARB_texture_compression_rgtc = GL_FALSE;
GLboolean __GLEW_ARB_texture_cube_map = GL_FALSE;
GLboolean __GLEW_ARB_texture_cube_map_array = GL_FALSE;
GLboolean __GLEW_ARB_texture_env_add = GL_FALSE;
GLboolean __GLEW_ARB_texture_env_combine = GL_FALSE;
GLboolean __GLEW_ARB_texture_env_crossbar = GL_FALSE;
GLboolean __GLEW_ARB_texture_env_dot3 = GL_FALSE;
GLboolean __GLEW_ARB_texture_float = GL_FALSE;
GLboolean __GLEW_ARB_texture_gather = GL_FALSE;
GLboolean __GLEW_ARB_texture_mirrored_repeat = GL_FALSE;
GLboolean __GLEW_ARB_texture_multisample = GL_FALSE;
GLboolean __GLEW_ARB_texture_non_power_of_two = GL_FALSE;
GLboolean __GLEW_ARB_texture_query_lod = GL_FALSE;
GLboolean __GLEW_ARB_texture_rectangle = GL_FALSE;
GLboolean __GLEW_ARB_texture_rg = GL_FALSE;
GLboolean __GLEW_ARB_transpose_matrix = GL_FALSE;
GLboolean __GLEW_ARB_uniform_buffer_object = GL_FALSE;
GLboolean __GLEW_ARB_vertex_array_bgra = GL_FALSE;
GLboolean __GLEW_ARB_vertex_array_object = GL_FALSE;
GLboolean __GLEW_ARB_vertex_blend = GL_FALSE;
GLboolean __GLEW_ARB_vertex_buffer_object = GL_FALSE;
GLboolean __GLEW_ARB_vertex_program = GL_FALSE;
GLboolean __GLEW_ARB_vertex_shader = GL_FALSE;
GLboolean __GLEW_ARB_window_pos = GL_FALSE;
GLboolean __GLEW_ATIX_point_sprites = GL_FALSE;
GLboolean __GLEW_ATIX_texture_env_combine3 = GL_FALSE;
GLboolean __GLEW_ATIX_texture_env_route = GL_FALSE;
GLboolean __GLEW_ATIX_vertex_shader_output_point_size = GL_FALSE;
GLboolean __GLEW_ATI_draw_buffers = GL_FALSE;
GLboolean __GLEW_ATI_element_array = GL_FALSE;
GLboolean __GLEW_ATI_envmap_bumpmap = GL_FALSE;
GLboolean __GLEW_ATI_fragment_shader = GL_FALSE;
GLboolean __GLEW_ATI_map_object_buffer = GL_FALSE;
GLboolean __GLEW_ATI_meminfo = GL_FALSE;
GLboolean __GLEW_ATI_pn_triangles = GL_FALSE;
GLboolean __GLEW_ATI_separate_stencil = GL_FALSE;
GLboolean __GLEW_ATI_shader_texture_lod = GL_FALSE;
GLboolean __GLEW_ATI_text_fragment_shader = GL_FALSE;
GLboolean __GLEW_ATI_texture_compression_3dc = GL_FALSE;
GLboolean __GLEW_ATI_texture_env_combine3 = GL_FALSE;
GLboolean __GLEW_ATI_texture_float = GL_FALSE;
GLboolean __GLEW_ATI_texture_mirror_once = GL_FALSE;
GLboolean __GLEW_ATI_vertex_array_object = GL_FALSE;
GLboolean __GLEW_ATI_vertex_attrib_array_object = GL_FALSE;
GLboolean __GLEW_ATI_vertex_streams = GL_FALSE;
GLboolean __GLEW_EXT_422_pixels = GL_FALSE;
GLboolean __GLEW_EXT_Cg_shader = GL_FALSE;
GLboolean __GLEW_EXT_abgr = GL_FALSE;
GLboolean __GLEW_EXT_bgra = GL_FALSE;
GLboolean __GLEW_EXT_bindable_uniform = GL_FALSE;
GLboolean __GLEW_EXT_blend_color = GL_FALSE;
GLboolean __GLEW_EXT_blend_equation_separate = GL_FALSE;
GLboolean __GLEW_EXT_blend_func_separate = GL_FALSE;
GLboolean __GLEW_EXT_blend_logic_op = GL_FALSE;
GLboolean __GLEW_EXT_blend_minmax = GL_FALSE;
GLboolean __GLEW_EXT_blend_subtract = GL_FALSE;
GLboolean __GLEW_EXT_clip_volume_hint = GL_FALSE;
GLboolean __GLEW_EXT_cmyka = GL_FALSE;
GLboolean __GLEW_EXT_color_subtable = GL_FALSE;
GLboolean __GLEW_EXT_compiled_vertex_array = GL_FALSE;
GLboolean __GLEW_EXT_convolution = GL_FALSE;
GLboolean __GLEW_EXT_coordinate_frame = GL_FALSE;
GLboolean __GLEW_EXT_copy_texture = GL_FALSE;
GLboolean __GLEW_EXT_cull_vertex = GL_FALSE;
GLboolean __GLEW_EXT_depth_bounds_test = GL_FALSE;
GLboolean __GLEW_EXT_direct_state_access = GL_FALSE;
GLboolean __GLEW_EXT_draw_buffers2 = GL_FALSE;
GLboolean __GLEW_EXT_draw_instanced = GL_FALSE;
GLboolean __GLEW_EXT_draw_range_elements = GL_FALSE;
GLboolean __GLEW_EXT_fog_coord = GL_FALSE;
GLboolean __GLEW_EXT_fragment_lighting = GL_FALSE;
GLboolean __GLEW_EXT_framebuffer_blit = GL_FALSE;
GLboolean __GLEW_EXT_framebuffer_multisample = GL_FALSE;
GLboolean __GLEW_EXT_framebuffer_object = GL_FALSE;
GLboolean __GLEW_EXT_framebuffer_sRGB = GL_FALSE;
GLboolean __GLEW_EXT_geometry_shader4 = GL_FALSE;
GLboolean __GLEW_EXT_gpu_program_parameters = GL_FALSE;
GLboolean __GLEW_EXT_gpu_shader4 = GL_FALSE;
GLboolean __GLEW_EXT_histogram = GL_FALSE;
GLboolean __GLEW_EXT_index_array_formats = GL_FALSE;
GLboolean __GLEW_EXT_index_func = GL_FALSE;
GLboolean __GLEW_EXT_index_material = GL_FALSE;
GLboolean __GLEW_EXT_index_texture = GL_FALSE;
GLboolean __GLEW_EXT_light_texture = GL_FALSE;
GLboolean __GLEW_EXT_misc_attribute = GL_FALSE;
GLboolean __GLEW_EXT_multi_draw_arrays = GL_FALSE;
GLboolean __GLEW_EXT_multisample = GL_FALSE;
GLboolean __GLEW_EXT_packed_depth_stencil = GL_FALSE;
GLboolean __GLEW_EXT_packed_float = GL_FALSE;
GLboolean __GLEW_EXT_packed_pixels = GL_FALSE;
GLboolean __GLEW_EXT_paletted_texture = GL_FALSE;
GLboolean __GLEW_EXT_pixel_buffer_object = GL_FALSE;
GLboolean __GLEW_EXT_pixel_transform = GL_FALSE;
GLboolean __GLEW_EXT_pixel_transform_color_table = GL_FALSE;
GLboolean __GLEW_EXT_point_parameters = GL_FALSE;
GLboolean __GLEW_EXT_polygon_offset = GL_FALSE;
GLboolean __GLEW_EXT_provoking_vertex = GL_FALSE;
GLboolean __GLEW_EXT_rescale_normal = GL_FALSE;
GLboolean __GLEW_EXT_scene_marker = GL_FALSE;
GLboolean __GLEW_EXT_secondary_color = GL_FALSE;
GLboolean __GLEW_EXT_separate_shader_objects = GL_FALSE;
GLboolean __GLEW_EXT_separate_specular_color = GL_FALSE;
GLboolean __GLEW_EXT_shadow_funcs = GL_FALSE;
GLboolean __GLEW_EXT_shared_texture_palette = GL_FALSE;
GLboolean __GLEW_EXT_stencil_clear_tag = GL_FALSE;
GLboolean __GLEW_EXT_stencil_two_side = GL_FALSE;
GLboolean __GLEW_EXT_stencil_wrap = GL_FALSE;
GLboolean __GLEW_EXT_subtexture = GL_FALSE;
GLboolean __GLEW_EXT_texture = GL_FALSE;
GLboolean __GLEW_EXT_texture3D = GL_FALSE;
GLboolean __GLEW_EXT_texture_array = GL_FALSE;
GLboolean __GLEW_EXT_texture_buffer_object = GL_FALSE;
GLboolean __GLEW_EXT_texture_compression_dxt1 = GL_FALSE;
GLboolean __GLEW_EXT_texture_compression_latc = GL_FALSE;
GLboolean __GLEW_EXT_texture_compression_rgtc = GL_FALSE;
GLboolean __GLEW_EXT_texture_compression_s3tc = GL_FALSE;
GLboolean __GLEW_EXT_texture_cube_map = GL_FALSE;
GLboolean __GLEW_EXT_texture_edge_clamp = GL_FALSE;
GLboolean __GLEW_EXT_texture_env = GL_FALSE;
GLboolean __GLEW_EXT_texture_env_add = GL_FALSE;
GLboolean __GLEW_EXT_texture_env_combine = GL_FALSE;
GLboolean __GLEW_EXT_texture_env_dot3 = GL_FALSE;
GLboolean __GLEW_EXT_texture_filter_anisotropic = GL_FALSE;
GLboolean __GLEW_EXT_texture_integer = GL_FALSE;
GLboolean __GLEW_EXT_texture_lod_bias = GL_FALSE;
GLboolean __GLEW_EXT_texture_mirror_clamp = GL_FALSE;
GLboolean __GLEW_EXT_texture_object = GL_FALSE;
GLboolean __GLEW_EXT_texture_perturb_normal = GL_FALSE;
GLboolean __GLEW_EXT_texture_rectangle = GL_FALSE;
GLboolean __GLEW_EXT_texture_sRGB = GL_FALSE;
GLboolean __GLEW_EXT_texture_shared_exponent = GL_FALSE;
GLboolean __GLEW_EXT_texture_snorm = GL_FALSE;
GLboolean __GLEW_EXT_texture_swizzle = GL_FALSE;
GLboolean __GLEW_EXT_timer_query = GL_FALSE;
GLboolean __GLEW_EXT_transform_feedback = GL_FALSE;
GLboolean __GLEW_EXT_vertex_array = GL_FALSE;
GLboolean __GLEW_EXT_vertex_array_bgra = GL_FALSE;
GLboolean __GLEW_EXT_vertex_shader = GL_FALSE;
GLboolean __GLEW_EXT_vertex_weighting = GL_FALSE;
GLboolean __GLEW_GREMEDY_frame_terminator = GL_FALSE;
GLboolean __GLEW_GREMEDY_string_marker = GL_FALSE;
GLboolean __GLEW_HP_convolution_border_modes = GL_FALSE;
GLboolean __GLEW_HP_image_transform = GL_FALSE;
GLboolean __GLEW_HP_occlusion_test = GL_FALSE;
GLboolean __GLEW_HP_texture_lighting = GL_FALSE;
GLboolean __GLEW_IBM_cull_vertex = GL_FALSE;
GLboolean __GLEW_IBM_multimode_draw_arrays = GL_FALSE;
GLboolean __GLEW_IBM_rasterpos_clip = GL_FALSE;
GLboolean __GLEW_IBM_static_data = GL_FALSE;
GLboolean __GLEW_IBM_texture_mirrored_repeat = GL_FALSE;
GLboolean __GLEW_IBM_vertex_array_lists = GL_FALSE;
GLboolean __GLEW_INGR_color_clamp = GL_FALSE;
GLboolean __GLEW_INGR_interlace_read = GL_FALSE;
GLboolean __GLEW_INTEL_parallel_arrays = GL_FALSE;
GLboolean __GLEW_INTEL_texture_scissor = GL_FALSE;
GLboolean __GLEW_KTX_buffer_region = GL_FALSE;
GLboolean __GLEW_MESAX_texture_stack = GL_FALSE;
GLboolean __GLEW_MESA_pack_invert = GL_FALSE;
GLboolean __GLEW_MESA_resize_buffers = GL_FALSE;
GLboolean __GLEW_MESA_window_pos = GL_FALSE;
GLboolean __GLEW_MESA_ycbcr_texture = GL_FALSE;
GLboolean __GLEW_NV_blend_square = GL_FALSE;
GLboolean __GLEW_NV_conditional_render = GL_FALSE;
GLboolean __GLEW_NV_copy_depth_to_color = GL_FALSE;
GLboolean __GLEW_NV_copy_image = GL_FALSE;
GLboolean __GLEW_NV_depth_buffer_float = GL_FALSE;
GLboolean __GLEW_NV_depth_clamp = GL_FALSE;
GLboolean __GLEW_NV_depth_range_unclamped = GL_FALSE;
GLboolean __GLEW_NV_evaluators = GL_FALSE;
GLboolean __GLEW_NV_explicit_multisample = GL_FALSE;
GLboolean __GLEW_NV_fence = GL_FALSE;
GLboolean __GLEW_NV_float_buffer = GL_FALSE;
GLboolean __GLEW_NV_fog_distance = GL_FALSE;
GLboolean __GLEW_NV_fragment_program = GL_FALSE;
GLboolean __GLEW_NV_fragment_program2 = GL_FALSE;
GLboolean __GLEW_NV_fragment_program4 = GL_FALSE;
GLboolean __GLEW_NV_fragment_program_option = GL_FALSE;
GLboolean __GLEW_NV_framebuffer_multisample_coverage = GL_FALSE;
GLboolean __GLEW_NV_geometry_program4 = GL_FALSE;
GLboolean __GLEW_NV_geometry_shader4 = GL_FALSE;
GLboolean __GLEW_NV_gpu_program4 = GL_FALSE;
GLboolean __GLEW_NV_half_float = GL_FALSE;
GLboolean __GLEW_NV_light_max_exponent = GL_FALSE;
GLboolean __GLEW_NV_multisample_filter_hint = GL_FALSE;
GLboolean __GLEW_NV_occlusion_query = GL_FALSE;
GLboolean __GLEW_NV_packed_depth_stencil = GL_FALSE;
GLboolean __GLEW_NV_parameter_buffer_object = GL_FALSE;
GLboolean __GLEW_NV_parameter_buffer_object2 = GL_FALSE;
GLboolean __GLEW_NV_pixel_data_range = GL_FALSE;
GLboolean __GLEW_NV_point_sprite = GL_FALSE;
GLboolean __GLEW_NV_present_video = GL_FALSE;
GLboolean __GLEW_NV_primitive_restart = GL_FALSE;
GLboolean __GLEW_NV_register_combiners = GL_FALSE;
GLboolean __GLEW_NV_register_combiners2 = GL_FALSE;
GLboolean __GLEW_NV_shader_buffer_load = GL_FALSE;
GLboolean __GLEW_NV_texgen_emboss = GL_FALSE;
GLboolean __GLEW_NV_texgen_reflection = GL_FALSE;
GLboolean __GLEW_NV_texture_barrier = GL_FALSE;
GLboolean __GLEW_NV_texture_compression_vtc = GL_FALSE;
GLboolean __GLEW_NV_texture_env_combine4 = GL_FALSE;
GLboolean __GLEW_NV_texture_expand_normal = GL_FALSE;
GLboolean __GLEW_NV_texture_rectangle = GL_FALSE;
GLboolean __GLEW_NV_texture_shader = GL_FALSE;
GLboolean __GLEW_NV_texture_shader2 = GL_FALSE;
GLboolean __GLEW_NV_texture_shader3 = GL_FALSE;
GLboolean __GLEW_NV_transform_feedback = GL_FALSE;
GLboolean __GLEW_NV_transform_feedback2 = GL_FALSE;
GLboolean __GLEW_NV_vertex_array_range = GL_FALSE;
GLboolean __GLEW_NV_vertex_array_range2 = GL_FALSE;
GLboolean __GLEW_NV_vertex_buffer_unified_memory = GL_FALSE;
GLboolean __GLEW_NV_vertex_program = GL_FALSE;
GLboolean __GLEW_NV_vertex_program1_1 = GL_FALSE;
GLboolean __GLEW_NV_vertex_program2 = GL_FALSE;
GLboolean __GLEW_NV_vertex_program2_option = GL_FALSE;
GLboolean __GLEW_NV_vertex_program3 = GL_FALSE;
GLboolean __GLEW_NV_vertex_program4 = GL_FALSE;
GLboolean __GLEW_OES_byte_coordinates = GL_FALSE;
GLboolean __GLEW_OES_compressed_paletted_texture = GL_FALSE;
GLboolean __GLEW_OES_read_format = GL_FALSE;
GLboolean __GLEW_OES_single_precision = GL_FALSE;
GLboolean __GLEW_OML_interlace = GL_FALSE;
GLboolean __GLEW_OML_resample = GL_FALSE;
GLboolean __GLEW_OML_subsample = GL_FALSE;
GLboolean __GLEW_PGI_misc_hints = GL_FALSE;
GLboolean __GLEW_PGI_vertex_hints = GL_FALSE;
GLboolean __GLEW_REND_screen_coordinates = GL_FALSE;
GLboolean __GLEW_S3_s3tc = GL_FALSE;
GLboolean __GLEW_SGIS_color_range = GL_FALSE;
GLboolean __GLEW_SGIS_detail_texture = GL_FALSE;
GLboolean __GLEW_SGIS_fog_function = GL_FALSE;
GLboolean __GLEW_SGIS_generate_mipmap = GL_FALSE;
GLboolean __GLEW_SGIS_multisample = GL_FALSE;
GLboolean __GLEW_SGIS_pixel_texture = GL_FALSE;
GLboolean __GLEW_SGIS_point_line_texgen = GL_FALSE;
GLboolean __GLEW_SGIS_sharpen_texture = GL_FALSE;
GLboolean __GLEW_SGIS_texture4D = GL_FALSE;
GLboolean __GLEW_SGIS_texture_border_clamp = GL_FALSE;
GLboolean __GLEW_SGIS_texture_edge_clamp = GL_FALSE;
GLboolean __GLEW_SGIS_texture_filter4 = GL_FALSE;
GLboolean __GLEW_SGIS_texture_lod = GL_FALSE;
GLboolean __GLEW_SGIS_texture_select = GL_FALSE;
GLboolean __GLEW_SGIX_async = GL_FALSE;
GLboolean __GLEW_SGIX_async_histogram = GL_FALSE;
GLboolean __GLEW_SGIX_async_pixel = GL_FALSE;
GLboolean __GLEW_SGIX_blend_alpha_minmax = GL_FALSE;
GLboolean __GLEW_SGIX_clipmap = GL_FALSE;
GLboolean __GLEW_SGIX_convolution_accuracy = GL_FALSE;
GLboolean __GLEW_SGIX_depth_texture = GL_FALSE;
GLboolean __GLEW_SGIX_flush_raster = GL_FALSE;
GLboolean __GLEW_SGIX_fog_offset = GL_FALSE;
GLboolean __GLEW_SGIX_fog_texture = GL_FALSE;
GLboolean __GLEW_SGIX_fragment_specular_lighting = GL_FALSE;
GLboolean __GLEW_SGIX_framezoom = GL_FALSE;
GLboolean __GLEW_SGIX_interlace = GL_FALSE;
GLboolean __GLEW_SGIX_ir_instrument1 = GL_FALSE;
GLboolean __GLEW_SGIX_list_priority = GL_FALSE;
GLboolean __GLEW_SGIX_pixel_texture = GL_FALSE;
GLboolean __GLEW_SGIX_pixel_texture_bits = GL_FALSE;
GLboolean __GLEW_SGIX_reference_plane = GL_FALSE;
GLboolean __GLEW_SGIX_resample = GL_FALSE;
GLboolean __GLEW_SGIX_shadow = GL_FALSE;
GLboolean __GLEW_SGIX_shadow_ambient = GL_FALSE;
GLboolean __GLEW_SGIX_sprite = GL_FALSE;
GLboolean __GLEW_SGIX_tag_sample_buffer = GL_FALSE;
GLboolean __GLEW_SGIX_texture_add_env = GL_FALSE;
GLboolean __GLEW_SGIX_texture_coordinate_clamp = GL_FALSE;
GLboolean __GLEW_SGIX_texture_lod_bias = GL_FALSE;
GLboolean __GLEW_SGIX_texture_multi_buffer = GL_FALSE;
GLboolean __GLEW_SGIX_texture_range = GL_FALSE;
GLboolean __GLEW_SGIX_texture_scale_bias = GL_FALSE;
GLboolean __GLEW_SGIX_vertex_preclip = GL_FALSE;
GLboolean __GLEW_SGIX_vertex_preclip_hint = GL_FALSE;
GLboolean __GLEW_SGIX_ycrcb = GL_FALSE;
GLboolean __GLEW_SGI_color_matrix = GL_FALSE;
GLboolean __GLEW_SGI_color_table = GL_FALSE;
GLboolean __GLEW_SGI_texture_color_table = GL_FALSE;
GLboolean __GLEW_SUNX_constant_data = GL_FALSE;
GLboolean __GLEW_SUN_convolution_border_modes = GL_FALSE;
GLboolean __GLEW_SUN_global_alpha = GL_FALSE;
GLboolean __GLEW_SUN_mesh_array = GL_FALSE;
GLboolean __GLEW_SUN_read_video_pixels = GL_FALSE;
GLboolean __GLEW_SUN_slice_accum = GL_FALSE;
GLboolean __GLEW_SUN_triangle_list = GL_FALSE;
GLboolean __GLEW_SUN_vertex = GL_FALSE;
GLboolean __GLEW_WIN_phong_shading = GL_FALSE;
GLboolean __GLEW_WIN_specular_fog = GL_FALSE;
GLboolean __GLEW_WIN_swap_hint = GL_FALSE;

#endif /* !GLEW_MX */

#ifdef GL_VERSION_1_2

static GLboolean _glewInit_GL_VERSION_1_2 (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glCopyTexSubImage3D = (PFNGLCOPYTEXSUBIMAGE3DPROC)glewGetProcAddress((const GLubyte*)"glCopyTexSubImage3D")) == null) || r;
  r = ((glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC)glewGetProcAddress((const GLubyte*)"glDrawRangeElements")) == null) || r;
  r = ((glTexImage3D = (PFNGLTEXIMAGE3DPROC)glewGetProcAddress((const GLubyte*)"glTexImage3D")) == null) || r;
  r = ((glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC)glewGetProcAddress((const GLubyte*)"glTexSubImage3D")) == null) || r;

  return r;
}

#endif /* GL_VERSION_1_2 */

#ifdef GL_VERSION_1_3

static GLboolean _glewInit_GL_VERSION_1_3 (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glActiveTexture = (PFNGLACTIVETEXTUREPROC)glewGetProcAddress((const GLubyte*)"glActiveTexture")) == null) || r;
  r = ((glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC)glewGetProcAddress((const GLubyte*)"glClientActiveTexture")) == null) || r;
  r = ((glCompressedTexImage1D = (PFNGLCOMPRESSEDTEXIMAGE1DPROC)glewGetProcAddress((const GLubyte*)"glCompressedTexImage1D")) == null) || r;
  r = ((glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)glewGetProcAddress((const GLubyte*)"glCompressedTexImage2D")) == null) || r;
  r = ((glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC)glewGetProcAddress((const GLubyte*)"glCompressedTexImage3D")) == null) || r;
  r = ((glCompressedTexSubImage1D = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC)glewGetProcAddress((const GLubyte*)"glCompressedTexSubImage1D")) == null) || r;
  r = ((glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)glewGetProcAddress((const GLubyte*)"glCompressedTexSubImage2D")) == null) || r;
  r = ((glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)glewGetProcAddress((const GLubyte*)"glCompressedTexSubImage3D")) == null) || r;
  r = ((glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC)glewGetProcAddress((const GLubyte*)"glGetCompressedTexImage")) == null) || r;
  r = ((glLoadTransposeMatrixd = (PFNGLLOADTRANSPOSEMATRIXDPROC)glewGetProcAddress((const GLubyte*)"glLoadTransposeMatrixd")) == null) || r;
  r = ((glLoadTransposeMatrixf = (PFNGLLOADTRANSPOSEMATRIXFPROC)glewGetProcAddress((const GLubyte*)"glLoadTransposeMatrixf")) == null) || r;
  r = ((glMultTransposeMatrixd = (PFNGLMULTTRANSPOSEMATRIXDPROC)glewGetProcAddress((const GLubyte*)"glMultTransposeMatrixd")) == null) || r;
  r = ((glMultTransposeMatrixf = (PFNGLMULTTRANSPOSEMATRIXFPROC)glewGetProcAddress((const GLubyte*)"glMultTransposeMatrixf")) == null) || r;
  r = ((glMultiTexCoord1d = (PFNGLMULTITEXCOORD1DPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord1d")) == null) || r;
  r = ((glMultiTexCoord1dv = (PFNGLMULTITEXCOORD1DVPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord1dv")) == null) || r;
  r = ((glMultiTexCoord1f = (PFNGLMULTITEXCOORD1FPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord1f")) == null) || r;
  r = ((glMultiTexCoord1fv = (PFNGLMULTITEXCOORD1FVPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord1fv")) == null) || r;
  r = ((glMultiTexCoord1i = (PFNGLMULTITEXCOORD1IPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord1i")) == null) || r;
  r = ((glMultiTexCoord1iv = (PFNGLMULTITEXCOORD1IVPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord1iv")) == null) || r;
  r = ((glMultiTexCoord1s = (PFNGLMULTITEXCOORD1SPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord1s")) == null) || r;
  r = ((glMultiTexCoord1sv = (PFNGLMULTITEXCOORD1SVPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord1sv")) == null) || r;
  r = ((glMultiTexCoord2d = (PFNGLMULTITEXCOORD2DPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord2d")) == null) || r;
  r = ((glMultiTexCoord2dv = (PFNGLMULTITEXCOORD2DVPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord2dv")) == null) || r;
  r = ((glMultiTexCoord2f = (PFNGLMULTITEXCOORD2FPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord2f")) == null) || r;
  r = ((glMultiTexCoord2fv = (PFNGLMULTITEXCOORD2FVPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord2fv")) == null) || r;
  r = ((glMultiTexCoord2i = (PFNGLMULTITEXCOORD2IPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord2i")) == null) || r;
  r = ((glMultiTexCoord2iv = (PFNGLMULTITEXCOORD2IVPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord2iv")) == null) || r;
  r = ((glMultiTexCoord2s = (PFNGLMULTITEXCOORD2SPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord2s")) == null) || r;
  r = ((glMultiTexCoord2sv = (PFNGLMULTITEXCOORD2SVPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord2sv")) == null) || r;
  r = ((glMultiTexCoord3d = (PFNGLMULTITEXCOORD3DPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord3d")) == null) || r;
  r = ((glMultiTexCoord3dv = (PFNGLMULTITEXCOORD3DVPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord3dv")) == null) || r;
  r = ((glMultiTexCoord3f = (PFNGLMULTITEXCOORD3FPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord3f")) == null) || r;
  r = ((glMultiTexCoord3fv = (PFNGLMULTITEXCOORD3FVPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord3fv")) == null) || r;
  r = ((glMultiTexCoord3i = (PFNGLMULTITEXCOORD3IPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord3i")) == null) || r;
  r = ((glMultiTexCoord3iv = (PFNGLMULTITEXCOORD3IVPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord3iv")) == null) || r;
  r = ((glMultiTexCoord3s = (PFNGLMULTITEXCOORD3SPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord3s")) == null) || r;
  r = ((glMultiTexCoord3sv = (PFNGLMULTITEXCOORD3SVPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord3sv")) == null) || r;
  r = ((glMultiTexCoord4d = (PFNGLMULTITEXCOORD4DPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord4d")) == null) || r;
  r = ((glMultiTexCoord4dv = (PFNGLMULTITEXCOORD4DVPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord4dv")) == null) || r;
  r = ((glMultiTexCoord4f = (PFNGLMULTITEXCOORD4FPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord4f")) == null) || r;
  r = ((glMultiTexCoord4fv = (PFNGLMULTITEXCOORD4FVPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord4fv")) == null) || r;
  r = ((glMultiTexCoord4i = (PFNGLMULTITEXCOORD4IPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord4i")) == null) || r;
  r = ((glMultiTexCoord4iv = (PFNGLMULTITEXCOORD4IVPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord4iv")) == null) || r;
  r = ((glMultiTexCoord4s = (PFNGLMULTITEXCOORD4SPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord4s")) == null) || r;
  r = ((glMultiTexCoord4sv = (PFNGLMULTITEXCOORD4SVPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord4sv")) == null) || r;
  r = ((glSampleCoverage = (PFNGLSAMPLECOVERAGEPROC)glewGetProcAddress((const GLubyte*)"glSampleCoverage")) == null) || r;

  return r;
}

#endif /* GL_VERSION_1_3 */

#ifdef GL_VERSION_1_4

static GLboolean _glewInit_GL_VERSION_1_4 (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBlendColor = (PFNGLBLENDCOLORPROC)glewGetProcAddress((const GLubyte*)"glBlendColor")) == null) || r;
  r = ((glBlendEquation = (PFNGLBLENDEQUATIONPROC)glewGetProcAddress((const GLubyte*)"glBlendEquation")) == null) || r;
  r = ((glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC)glewGetProcAddress((const GLubyte*)"glBlendFuncSeparate")) == null) || r;
  r = ((glFogCoordPointer = (PFNGLFOGCOORDPOINTERPROC)glewGetProcAddress((const GLubyte*)"glFogCoordPointer")) == null) || r;
  r = ((glFogCoordd = (PFNGLFOGCOORDDPROC)glewGetProcAddress((const GLubyte*)"glFogCoordd")) == null) || r;
  r = ((glFogCoorddv = (PFNGLFOGCOORDDVPROC)glewGetProcAddress((const GLubyte*)"glFogCoorddv")) == null) || r;
  r = ((glFogCoordf = (PFNGLFOGCOORDFPROC)glewGetProcAddress((const GLubyte*)"glFogCoordf")) == null) || r;
  r = ((glFogCoordfv = (PFNGLFOGCOORDFVPROC)glewGetProcAddress((const GLubyte*)"glFogCoordfv")) == null) || r;
  r = ((glMultiDrawArrays = (PFNGLMULTIDRAWARRAYSPROC)glewGetProcAddress((const GLubyte*)"glMultiDrawArrays")) == null) || r;
  r = ((glMultiDrawElements = (PFNGLMULTIDRAWELEMENTSPROC)glewGetProcAddress((const GLubyte*)"glMultiDrawElements")) == null) || r;
  r = ((glPointParameterf = (PFNGLPOINTPARAMETERFPROC)glewGetProcAddress((const GLubyte*)"glPointParameterf")) == null) || r;
  r = ((glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC)glewGetProcAddress((const GLubyte*)"glPointParameterfv")) == null) || r;
  r = ((glPointParameteri = (PFNGLPOINTPARAMETERIPROC)glewGetProcAddress((const GLubyte*)"glPointParameteri")) == null) || r;
  r = ((glPointParameteriv = (PFNGLPOINTPARAMETERIVPROC)glewGetProcAddress((const GLubyte*)"glPointParameteriv")) == null) || r;
  r = ((glSecondaryColor3b = (PFNGLSECONDARYCOLOR3BPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3b")) == null) || r;
  r = ((glSecondaryColor3bv = (PFNGLSECONDARYCOLOR3BVPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3bv")) == null) || r;
  r = ((glSecondaryColor3d = (PFNGLSECONDARYCOLOR3DPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3d")) == null) || r;
  r = ((glSecondaryColor3dv = (PFNGLSECONDARYCOLOR3DVPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3dv")) == null) || r;
  r = ((glSecondaryColor3f = (PFNGLSECONDARYCOLOR3FPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3f")) == null) || r;
  r = ((glSecondaryColor3fv = (PFNGLSECONDARYCOLOR3FVPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3fv")) == null) || r;
  r = ((glSecondaryColor3i = (PFNGLSECONDARYCOLOR3IPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3i")) == null) || r;
  r = ((glSecondaryColor3iv = (PFNGLSECONDARYCOLOR3IVPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3iv")) == null) || r;
  r = ((glSecondaryColor3s = (PFNGLSECONDARYCOLOR3SPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3s")) == null) || r;
  r = ((glSecondaryColor3sv = (PFNGLSECONDARYCOLOR3SVPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3sv")) == null) || r;
  r = ((glSecondaryColor3ub = (PFNGLSECONDARYCOLOR3UBPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3ub")) == null) || r;
  r = ((glSecondaryColor3ubv = (PFNGLSECONDARYCOLOR3UBVPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3ubv")) == null) || r;
  r = ((glSecondaryColor3ui = (PFNGLSECONDARYCOLOR3UIPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3ui")) == null) || r;
  r = ((glSecondaryColor3uiv = (PFNGLSECONDARYCOLOR3UIVPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3uiv")) == null) || r;
  r = ((glSecondaryColor3us = (PFNGLSECONDARYCOLOR3USPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3us")) == null) || r;
  r = ((glSecondaryColor3usv = (PFNGLSECONDARYCOLOR3USVPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3usv")) == null) || r;
  r = ((glSecondaryColorPointer = (PFNGLSECONDARYCOLORPOINTERPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColorPointer")) == null) || r;
  r = ((glWindowPos2d = (PFNGLWINDOWPOS2DPROC)glewGetProcAddress((const GLubyte*)"glWindowPos2d")) == null) || r;
  r = ((glWindowPos2dv = (PFNGLWINDOWPOS2DVPROC)glewGetProcAddress((const GLubyte*)"glWindowPos2dv")) == null) || r;
  r = ((glWindowPos2f = (PFNGLWINDOWPOS2FPROC)glewGetProcAddress((const GLubyte*)"glWindowPos2f")) == null) || r;
  r = ((glWindowPos2fv = (PFNGLWINDOWPOS2FVPROC)glewGetProcAddress((const GLubyte*)"glWindowPos2fv")) == null) || r;
  r = ((glWindowPos2i = (PFNGLWINDOWPOS2IPROC)glewGetProcAddress((const GLubyte*)"glWindowPos2i")) == null) || r;
  r = ((glWindowPos2iv = (PFNGLWINDOWPOS2IVPROC)glewGetProcAddress((const GLubyte*)"glWindowPos2iv")) == null) || r;
  r = ((glWindowPos2s = (PFNGLWINDOWPOS2SPROC)glewGetProcAddress((const GLubyte*)"glWindowPos2s")) == null) || r;
  r = ((glWindowPos2sv = (PFNGLWINDOWPOS2SVPROC)glewGetProcAddress((const GLubyte*)"glWindowPos2sv")) == null) || r;
  r = ((glWindowPos3d = (PFNGLWINDOWPOS3DPROC)glewGetProcAddress((const GLubyte*)"glWindowPos3d")) == null) || r;
  r = ((glWindowPos3dv = (PFNGLWINDOWPOS3DVPROC)glewGetProcAddress((const GLubyte*)"glWindowPos3dv")) == null) || r;
  r = ((glWindowPos3f = (PFNGLWINDOWPOS3FPROC)glewGetProcAddress((const GLubyte*)"glWindowPos3f")) == null) || r;
  r = ((glWindowPos3fv = (PFNGLWINDOWPOS3FVPROC)glewGetProcAddress((const GLubyte*)"glWindowPos3fv")) == null) || r;
  r = ((glWindowPos3i = (PFNGLWINDOWPOS3IPROC)glewGetProcAddress((const GLubyte*)"glWindowPos3i")) == null) || r;
  r = ((glWindowPos3iv = (PFNGLWINDOWPOS3IVPROC)glewGetProcAddress((const GLubyte*)"glWindowPos3iv")) == null) || r;
  r = ((glWindowPos3s = (PFNGLWINDOWPOS3SPROC)glewGetProcAddress((const GLubyte*)"glWindowPos3s")) == null) || r;
  r = ((glWindowPos3sv = (PFNGLWINDOWPOS3SVPROC)glewGetProcAddress((const GLubyte*)"glWindowPos3sv")) == null) || r;

  return r;
}

#endif /* GL_VERSION_1_4 */

#ifdef GL_VERSION_1_5

static GLboolean _glewInit_GL_VERSION_1_5 (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBeginQuery = (PFNGLBEGINQUERYPROC)glewGetProcAddress((const GLubyte*)"glBeginQuery")) == null) || r;
  r = ((glBindBuffer = (PFNGLBINDBUFFERPROC)glewGetProcAddress((const GLubyte*)"glBindBuffer")) == null) || r;
  r = ((glBufferData = (PFNGLBUFFERDATAPROC)glewGetProcAddress((const GLubyte*)"glBufferData")) == null) || r;
  r = ((glBufferSubData = (PFNGLBUFFERSUBDATAPROC)glewGetProcAddress((const GLubyte*)"glBufferSubData")) == null) || r;
  r = ((glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)glewGetProcAddress((const GLubyte*)"glDeleteBuffers")) == null) || r;
  r = ((glDeleteQueries = (PFNGLDELETEQUERIESPROC)glewGetProcAddress((const GLubyte*)"glDeleteQueries")) == null) || r;
  r = ((glEndQuery = (PFNGLENDQUERYPROC)glewGetProcAddress((const GLubyte*)"glEndQuery")) == null) || r;
  r = ((glGenBuffers = (PFNGLGENBUFFERSPROC)glewGetProcAddress((const GLubyte*)"glGenBuffers")) == null) || r;
  r = ((glGenQueries = (PFNGLGENQUERIESPROC)glewGetProcAddress((const GLubyte*)"glGenQueries")) == null) || r;
  r = ((glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC)glewGetProcAddress((const GLubyte*)"glGetBufferParameteriv")) == null) || r;
  r = ((glGetBufferPointerv = (PFNGLGETBUFFERPOINTERVPROC)glewGetProcAddress((const GLubyte*)"glGetBufferPointerv")) == null) || r;
  r = ((glGetBufferSubData = (PFNGLGETBUFFERSUBDATAPROC)glewGetProcAddress((const GLubyte*)"glGetBufferSubData")) == null) || r;
  r = ((glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC)glewGetProcAddress((const GLubyte*)"glGetQueryObjectiv")) == null) || r;
  r = ((glGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC)glewGetProcAddress((const GLubyte*)"glGetQueryObjectuiv")) == null) || r;
  r = ((glGetQueryiv = (PFNGLGETQUERYIVPROC)glewGetProcAddress((const GLubyte*)"glGetQueryiv")) == null) || r;
  r = ((glIsBuffer = (PFNGLISBUFFERPROC)glewGetProcAddress((const GLubyte*)"glIsBuffer")) == null) || r;
  r = ((glIsQuery = (PFNGLISQUERYPROC)glewGetProcAddress((const GLubyte*)"glIsQuery")) == null) || r;
  r = ((glMapBuffer = (PFNGLMAPBUFFERPROC)glewGetProcAddress((const GLubyte*)"glMapBuffer")) == null) || r;
  r = ((glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)glewGetProcAddress((const GLubyte*)"glUnmapBuffer")) == null) || r;

  return r;
}

#endif /* GL_VERSION_1_5 */

#ifdef GL_VERSION_2_0

static GLboolean _glewInit_GL_VERSION_2_0 (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glAttachShader = (PFNGLATTACHSHADERPROC)glewGetProcAddress((const GLubyte*)"glAttachShader")) == null) || r;
  r = ((glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)glewGetProcAddress((const GLubyte*)"glBindAttribLocation")) == null) || r;
  r = ((glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC)glewGetProcAddress((const GLubyte*)"glBlendEquationSeparate")) == null) || r;
  r = ((glCompileShader = (PFNGLCOMPILESHADERPROC)glewGetProcAddress((const GLubyte*)"glCompileShader")) == null) || r;
  r = ((glCreateProgram = (PFNGLCREATEPROGRAMPROC)glewGetProcAddress((const GLubyte*)"glCreateProgram")) == null) || r;
  r = ((glCreateShader = (PFNGLCREATESHADERPROC)glewGetProcAddress((const GLubyte*)"glCreateShader")) == null) || r;
  r = ((glDeleteProgram = (PFNGLDELETEPROGRAMPROC)glewGetProcAddress((const GLubyte*)"glDeleteProgram")) == null) || r;
  r = ((glDeleteShader = (PFNGLDELETESHADERPROC)glewGetProcAddress((const GLubyte*)"glDeleteShader")) == null) || r;
  r = ((glDetachShader = (PFNGLDETACHSHADERPROC)glewGetProcAddress((const GLubyte*)"glDetachShader")) == null) || r;
  r = ((glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)glewGetProcAddress((const GLubyte*)"glDisableVertexAttribArray")) == null) || r;
  r = ((glDrawBuffers = (PFNGLDRAWBUFFERSPROC)glewGetProcAddress((const GLubyte*)"glDrawBuffers")) == null) || r;
  r = ((glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glewGetProcAddress((const GLubyte*)"glEnableVertexAttribArray")) == null) || r;
  r = ((glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC)glewGetProcAddress((const GLubyte*)"glGetActiveAttrib")) == null) || r;
  r = ((glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)glewGetProcAddress((const GLubyte*)"glGetActiveUniform")) == null) || r;
  r = ((glGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC)glewGetProcAddress((const GLubyte*)"glGetAttachedShaders")) == null) || r;
  r = ((glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)glewGetProcAddress((const GLubyte*)"glGetAttribLocation")) == null) || r;
  r = ((glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)glewGetProcAddress((const GLubyte*)"glGetProgramInfoLog")) == null) || r;
  r = ((glGetProgramiv = (PFNGLGETPROGRAMIVPROC)glewGetProcAddress((const GLubyte*)"glGetProgramiv")) == null) || r;
  r = ((glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)glewGetProcAddress((const GLubyte*)"glGetShaderInfoLog")) == null) || r;
  r = ((glGetShaderSource = (PFNGLGETSHADERSOURCEPROC)glewGetProcAddress((const GLubyte*)"glGetShaderSource")) == null) || r;
  r = ((glGetShaderiv = (PFNGLGETSHADERIVPROC)glewGetProcAddress((const GLubyte*)"glGetShaderiv")) == null) || r;
  r = ((glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)glewGetProcAddress((const GLubyte*)"glGetUniformLocation")) == null) || r;
  r = ((glGetUniformfv = (PFNGLGETUNIFORMFVPROC)glewGetProcAddress((const GLubyte*)"glGetUniformfv")) == null) || r;
  r = ((glGetUniformiv = (PFNGLGETUNIFORMIVPROC)glewGetProcAddress((const GLubyte*)"glGetUniformiv")) == null) || r;
  r = ((glGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC)glewGetProcAddress((const GLubyte*)"glGetVertexAttribPointerv")) == null) || r;
  r = ((glGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC)glewGetProcAddress((const GLubyte*)"glGetVertexAttribdv")) == null) || r;
  r = ((glGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC)glewGetProcAddress((const GLubyte*)"glGetVertexAttribfv")) == null) || r;
  r = ((glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC)glewGetProcAddress((const GLubyte*)"glGetVertexAttribiv")) == null) || r;
  r = ((glIsProgram = (PFNGLISPROGRAMPROC)glewGetProcAddress((const GLubyte*)"glIsProgram")) == null) || r;
  r = ((glIsShader = (PFNGLISSHADERPROC)glewGetProcAddress((const GLubyte*)"glIsShader")) == null) || r;
  r = ((glLinkProgram = (PFNGLLINKPROGRAMPROC)glewGetProcAddress((const GLubyte*)"glLinkProgram")) == null) || r;
  r = ((glShaderSource = (PFNGLSHADERSOURCEPROC)glewGetProcAddress((const GLubyte*)"glShaderSource")) == null) || r;
  r = ((glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC)glewGetProcAddress((const GLubyte*)"glStencilFuncSeparate")) == null) || r;
  r = ((glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC)glewGetProcAddress((const GLubyte*)"glStencilMaskSeparate")) == null) || r;
  r = ((glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC)glewGetProcAddress((const GLubyte*)"glStencilOpSeparate")) == null) || r;
  r = ((glUniform1f = (PFNGLUNIFORM1FPROC)glewGetProcAddress((const GLubyte*)"glUniform1f")) == null) || r;
  r = ((glUniform1fv = (PFNGLUNIFORM1FVPROC)glewGetProcAddress((const GLubyte*)"glUniform1fv")) == null) || r;
  r = ((glUniform1i = (PFNGLUNIFORM1IPROC)glewGetProcAddress((const GLubyte*)"glUniform1i")) == null) || r;
  r = ((glUniform1iv = (PFNGLUNIFORM1IVPROC)glewGetProcAddress((const GLubyte*)"glUniform1iv")) == null) || r;
  r = ((glUniform2f = (PFNGLUNIFORM2FPROC)glewGetProcAddress((const GLubyte*)"glUniform2f")) == null) || r;
  r = ((glUniform2fv = (PFNGLUNIFORM2FVPROC)glewGetProcAddress((const GLubyte*)"glUniform2fv")) == null) || r;
  r = ((glUniform2i = (PFNGLUNIFORM2IPROC)glewGetProcAddress((const GLubyte*)"glUniform2i")) == null) || r;
  r = ((glUniform2iv = (PFNGLUNIFORM2IVPROC)glewGetProcAddress((const GLubyte*)"glUniform2iv")) == null) || r;
  r = ((glUniform3f = (PFNGLUNIFORM3FPROC)glewGetProcAddress((const GLubyte*)"glUniform3f")) == null) || r;
  r = ((glUniform3fv = (PFNGLUNIFORM3FVPROC)glewGetProcAddress((const GLubyte*)"glUniform3fv")) == null) || r;
  r = ((glUniform3i = (PFNGLUNIFORM3IPROC)glewGetProcAddress((const GLubyte*)"glUniform3i")) == null) || r;
  r = ((glUniform3iv = (PFNGLUNIFORM3IVPROC)glewGetProcAddress((const GLubyte*)"glUniform3iv")) == null) || r;
  r = ((glUniform4f = (PFNGLUNIFORM4FPROC)glewGetProcAddress((const GLubyte*)"glUniform4f")) == null) || r;
  r = ((glUniform4fv = (PFNGLUNIFORM4FVPROC)glewGetProcAddress((const GLubyte*)"glUniform4fv")) == null) || r;
  r = ((glUniform4i = (PFNGLUNIFORM4IPROC)glewGetProcAddress((const GLubyte*)"glUniform4i")) == null) || r;
  r = ((glUniform4iv = (PFNGLUNIFORM4IVPROC)glewGetProcAddress((const GLubyte*)"glUniform4iv")) == null) || r;
  r = ((glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC)glewGetProcAddress((const GLubyte*)"glUniformMatrix2fv")) == null) || r;
  r = ((glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)glewGetProcAddress((const GLubyte*)"glUniformMatrix3fv")) == null) || r;
  r = ((glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)glewGetProcAddress((const GLubyte*)"glUniformMatrix4fv")) == null) || r;
  r = ((glUseProgram = (PFNGLUSEPROGRAMPROC)glewGetProcAddress((const GLubyte*)"glUseProgram")) == null) || r;
  r = ((glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)glewGetProcAddress((const GLubyte*)"glValidateProgram")) == null) || r;
  r = ((glVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib1d")) == null) || r;
  r = ((glVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib1dv")) == null) || r;
  r = ((glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib1f")) == null) || r;
  r = ((glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib1fv")) == null) || r;
  r = ((glVertexAttrib1s = (PFNGLVERTEXATTRIB1SPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib1s")) == null) || r;
  r = ((glVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib1sv")) == null) || r;
  r = ((glVertexAttrib2d = (PFNGLVERTEXATTRIB2DPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib2d")) == null) || r;
  r = ((glVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib2dv")) == null) || r;
  r = ((glVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib2f")) == null) || r;
  r = ((glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib2fv")) == null) || r;
  r = ((glVertexAttrib2s = (PFNGLVERTEXATTRIB2SPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib2s")) == null) || r;
  r = ((glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib2sv")) == null) || r;
  r = ((glVertexAttrib3d = (PFNGLVERTEXATTRIB3DPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib3d")) == null) || r;
  r = ((glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib3dv")) == null) || r;
  r = ((glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib3f")) == null) || r;
  r = ((glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib3fv")) == null) || r;
  r = ((glVertexAttrib3s = (PFNGLVERTEXATTRIB3SPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib3s")) == null) || r;
  r = ((glVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib3sv")) == null) || r;
  r = ((glVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4Nbv")) == null) || r;
  r = ((glVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4Niv")) == null) || r;
  r = ((glVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4Nsv")) == null) || r;
  r = ((glVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4Nub")) == null) || r;
  r = ((glVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4Nubv")) == null) || r;
  r = ((glVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4Nuiv")) == null) || r;
  r = ((glVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4Nusv")) == null) || r;
  r = ((glVertexAttrib4bv = (PFNGLVERTEXATTRIB4BVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4bv")) == null) || r;
  r = ((glVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4d")) == null) || r;
  r = ((glVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4dv")) == null) || r;
  r = ((glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4f")) == null) || r;
  r = ((glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4fv")) == null) || r;
  r = ((glVertexAttrib4iv = (PFNGLVERTEXATTRIB4IVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4iv")) == null) || r;
  r = ((glVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4s")) == null) || r;
  r = ((glVertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4sv")) == null) || r;
  r = ((glVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4ubv")) == null) || r;
  r = ((glVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4uiv")) == null) || r;
  r = ((glVertexAttrib4usv = (PFNGLVERTEXATTRIB4USVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4usv")) == null) || r;
  r = ((glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribPointer")) == null) || r;

  return r;
}

#endif /* GL_VERSION_2_0 */

#ifdef GL_VERSION_2_1

static GLboolean _glewInit_GL_VERSION_2_1 (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glUniformMatrix2x3fv = (PFNGLUNIFORMMATRIX2X3FVPROC)glewGetProcAddress((const GLubyte*)"glUniformMatrix2x3fv")) == null) || r;
  r = ((glUniformMatrix2x4fv = (PFNGLUNIFORMMATRIX2X4FVPROC)glewGetProcAddress((const GLubyte*)"glUniformMatrix2x4fv")) == null) || r;
  r = ((glUniformMatrix3x2fv = (PFNGLUNIFORMMATRIX3X2FVPROC)glewGetProcAddress((const GLubyte*)"glUniformMatrix3x2fv")) == null) || r;
  r = ((glUniformMatrix3x4fv = (PFNGLUNIFORMMATRIX3X4FVPROC)glewGetProcAddress((const GLubyte*)"glUniformMatrix3x4fv")) == null) || r;
  r = ((glUniformMatrix4x2fv = (PFNGLUNIFORMMATRIX4X2FVPROC)glewGetProcAddress((const GLubyte*)"glUniformMatrix4x2fv")) == null) || r;
  r = ((glUniformMatrix4x3fv = (PFNGLUNIFORMMATRIX4X3FVPROC)glewGetProcAddress((const GLubyte*)"glUniformMatrix4x3fv")) == null) || r;

  return r;
}

#endif /* GL_VERSION_2_1 */

#ifdef GL_VERSION_3_0

static GLboolean _glewInit_GL_VERSION_3_0 (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBeginConditionalRender = (PFNGLBEGINCONDITIONALRENDERPROC)glewGetProcAddress((const GLubyte*)"glBeginConditionalRender")) == null) || r;
  r = ((glBeginTransformFeedback = (PFNGLBEGINTRANSFORMFEEDBACKPROC)glewGetProcAddress((const GLubyte*)"glBeginTransformFeedback")) == null) || r;
  r = ((glBindFragDataLocation = (PFNGLBINDFRAGDATALOCATIONPROC)glewGetProcAddress((const GLubyte*)"glBindFragDataLocation")) == null) || r;
  r = ((glClampColor = (PFNGLCLAMPCOLORPROC)glewGetProcAddress((const GLubyte*)"glClampColor")) == null) || r;
  r = ((glClearBufferfi = (PFNGLCLEARBUFFERFIPROC)glewGetProcAddress((const GLubyte*)"glClearBufferfi")) == null) || r;
  r = ((glClearBufferfv = (PFNGLCLEARBUFFERFVPROC)glewGetProcAddress((const GLubyte*)"glClearBufferfv")) == null) || r;
  r = ((glClearBufferiv = (PFNGLCLEARBUFFERIVPROC)glewGetProcAddress((const GLubyte*)"glClearBufferiv")) == null) || r;
  r = ((glClearBufferuiv = (PFNGLCLEARBUFFERUIVPROC)glewGetProcAddress((const GLubyte*)"glClearBufferuiv")) == null) || r;
  r = ((glColorMaski = (PFNGLCOLORMASKIPROC)glewGetProcAddress((const GLubyte*)"glColorMaski")) == null) || r;
  r = ((glDisablei = (PFNGLDISABLEIPROC)glewGetProcAddress((const GLubyte*)"glDisablei")) == null) || r;
  r = ((glEnablei = (PFNGLENABLEIPROC)glewGetProcAddress((const GLubyte*)"glEnablei")) == null) || r;
  r = ((glEndConditionalRender = (PFNGLENDCONDITIONALRENDERPROC)glewGetProcAddress((const GLubyte*)"glEndConditionalRender")) == null) || r;
  r = ((glEndTransformFeedback = (PFNGLENDTRANSFORMFEEDBACKPROC)glewGetProcAddress((const GLubyte*)"glEndTransformFeedback")) == null) || r;
  r = ((glGetBooleani_v = (PFNGLGETBOOLEANI_VPROC)glewGetProcAddress((const GLubyte*)"glGetBooleani_v")) == null) || r;
  r = ((glGetFragDataLocation = (PFNGLGETFRAGDATALOCATIONPROC)glewGetProcAddress((const GLubyte*)"glGetFragDataLocation")) == null) || r;
  r = ((glGetStringi = (PFNGLGETSTRINGIPROC)glewGetProcAddress((const GLubyte*)"glGetStringi")) == null) || r;
  r = ((glGetTexParameterIiv = (PFNGLGETTEXPARAMETERIIVPROC)glewGetProcAddress((const GLubyte*)"glGetTexParameterIiv")) == null) || r;
  r = ((glGetTexParameterIuiv = (PFNGLGETTEXPARAMETERIUIVPROC)glewGetProcAddress((const GLubyte*)"glGetTexParameterIuiv")) == null) || r;
  r = ((glGetTransformFeedbackVarying = (PFNGLGETTRANSFORMFEEDBACKVARYINGPROC)glewGetProcAddress((const GLubyte*)"glGetTransformFeedbackVarying")) == null) || r;
  r = ((glGetUniformuiv = (PFNGLGETUNIFORMUIVPROC)glewGetProcAddress((const GLubyte*)"glGetUniformuiv")) == null) || r;
  r = ((glGetVertexAttribIiv = (PFNGLGETVERTEXATTRIBIIVPROC)glewGetProcAddress((const GLubyte*)"glGetVertexAttribIiv")) == null) || r;
  r = ((glGetVertexAttribIuiv = (PFNGLGETVERTEXATTRIBIUIVPROC)glewGetProcAddress((const GLubyte*)"glGetVertexAttribIuiv")) == null) || r;
  r = ((glIsEnabledi = (PFNGLISENABLEDIPROC)glewGetProcAddress((const GLubyte*)"glIsEnabledi")) == null) || r;
  r = ((glTexParameterIiv = (PFNGLTEXPARAMETERIIVPROC)glewGetProcAddress((const GLubyte*)"glTexParameterIiv")) == null) || r;
  r = ((glTexParameterIuiv = (PFNGLTEXPARAMETERIUIVPROC)glewGetProcAddress((const GLubyte*)"glTexParameterIuiv")) == null) || r;
  r = ((glTransformFeedbackVaryings = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC)glewGetProcAddress((const GLubyte*)"glTransformFeedbackVaryings")) == null) || r;
  r = ((glUniform1ui = (PFNGLUNIFORM1UIPROC)glewGetProcAddress((const GLubyte*)"glUniform1ui")) == null) || r;
  r = ((glUniform1uiv = (PFNGLUNIFORM1UIVPROC)glewGetProcAddress((const GLubyte*)"glUniform1uiv")) == null) || r;
  r = ((glUniform2ui = (PFNGLUNIFORM2UIPROC)glewGetProcAddress((const GLubyte*)"glUniform2ui")) == null) || r;
  r = ((glUniform2uiv = (PFNGLUNIFORM2UIVPROC)glewGetProcAddress((const GLubyte*)"glUniform2uiv")) == null) || r;
  r = ((glUniform3ui = (PFNGLUNIFORM3UIPROC)glewGetProcAddress((const GLubyte*)"glUniform3ui")) == null) || r;
  r = ((glUniform3uiv = (PFNGLUNIFORM3UIVPROC)glewGetProcAddress((const GLubyte*)"glUniform3uiv")) == null) || r;
  r = ((glUniform4ui = (PFNGLUNIFORM4UIPROC)glewGetProcAddress((const GLubyte*)"glUniform4ui")) == null) || r;
  r = ((glUniform4uiv = (PFNGLUNIFORM4UIVPROC)glewGetProcAddress((const GLubyte*)"glUniform4uiv")) == null) || r;
  r = ((glVertexAttribI1i = (PFNGLVERTEXATTRIBI1IPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI1i")) == null) || r;
  r = ((glVertexAttribI1iv = (PFNGLVERTEXATTRIBI1IVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI1iv")) == null) || r;
  r = ((glVertexAttribI1ui = (PFNGLVERTEXATTRIBI1UIPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI1ui")) == null) || r;
  r = ((glVertexAttribI1uiv = (PFNGLVERTEXATTRIBI1UIVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI1uiv")) == null) || r;
  r = ((glVertexAttribI2i = (PFNGLVERTEXATTRIBI2IPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI2i")) == null) || r;
  r = ((glVertexAttribI2iv = (PFNGLVERTEXATTRIBI2IVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI2iv")) == null) || r;
  r = ((glVertexAttribI2ui = (PFNGLVERTEXATTRIBI2UIPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI2ui")) == null) || r;
  r = ((glVertexAttribI2uiv = (PFNGLVERTEXATTRIBI2UIVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI2uiv")) == null) || r;
  r = ((glVertexAttribI3i = (PFNGLVERTEXATTRIBI3IPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI3i")) == null) || r;
  r = ((glVertexAttribI3iv = (PFNGLVERTEXATTRIBI3IVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI3iv")) == null) || r;
  r = ((glVertexAttribI3ui = (PFNGLVERTEXATTRIBI3UIPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI3ui")) == null) || r;
  r = ((glVertexAttribI3uiv = (PFNGLVERTEXATTRIBI3UIVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI3uiv")) == null) || r;
  r = ((glVertexAttribI4bv = (PFNGLVERTEXATTRIBI4BVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI4bv")) == null) || r;
  r = ((glVertexAttribI4i = (PFNGLVERTEXATTRIBI4IPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI4i")) == null) || r;
  r = ((glVertexAttribI4iv = (PFNGLVERTEXATTRIBI4IVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI4iv")) == null) || r;
  r = ((glVertexAttribI4sv = (PFNGLVERTEXATTRIBI4SVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI4sv")) == null) || r;
  r = ((glVertexAttribI4ubv = (PFNGLVERTEXATTRIBI4UBVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI4ubv")) == null) || r;
  r = ((glVertexAttribI4ui = (PFNGLVERTEXATTRIBI4UIPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI4ui")) == null) || r;
  r = ((glVertexAttribI4uiv = (PFNGLVERTEXATTRIBI4UIVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI4uiv")) == null) || r;
  r = ((glVertexAttribI4usv = (PFNGLVERTEXATTRIBI4USVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI4usv")) == null) || r;
  r = ((glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribIPointer")) == null) || r;

  return r;
}

#endif /* GL_VERSION_3_0 */

#ifdef GL_VERSION_3_1

static GLboolean _glewInit_GL_VERSION_3_1 (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC)glewGetProcAddress((const GLubyte*)"glDrawArraysInstanced")) == null) || r;
  r = ((glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC)glewGetProcAddress((const GLubyte*)"glDrawElementsInstanced")) == null) || r;
  r = ((glPrimitiveRestartIndex = (PFNGLPRIMITIVERESTARTINDEXPROC)glewGetProcAddress((const GLubyte*)"glPrimitiveRestartIndex")) == null) || r;
  r = ((glTexBuffer = (PFNGLTEXBUFFERPROC)glewGetProcAddress((const GLubyte*)"glTexBuffer")) == null) || r;

  return r;
}

#endif /* GL_VERSION_3_1 */

#ifdef GL_VERSION_3_2

static GLboolean _glewInit_GL_VERSION_3_2 (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)glewGetProcAddress((const GLubyte*)"glFramebufferTexture")) == null) || r;
  r = ((glGetBufferParameteri64v = (PFNGLGETBUFFERPARAMETERI64VPROC)glewGetProcAddress((const GLubyte*)"glGetBufferParameteri64v")) == null) || r;
  r = ((glGetInteger64i_v = (PFNGLGETINTEGER64I_VPROC)glewGetProcAddress((const GLubyte*)"glGetInteger64i_v")) == null) || r;

  return r;
}

#endif /* GL_VERSION_3_2 */

#ifdef GL_3DFX_multisample

#endif /* GL_3DFX_multisample */

#ifdef GL_3DFX_tbuffer

static GLboolean _glewInit_GL_3DFX_tbuffer (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glTbufferMask3DFX = (PFNGLTBUFFERMASK3DFXPROC)glewGetProcAddress((const GLubyte*)"glTbufferMask3DFX")) == null) || r;

  return r;
}

#endif /* GL_3DFX_tbuffer */

#ifdef GL_3DFX_texture_compression_FXT1

#endif /* GL_3DFX_texture_compression_FXT1 */

#ifdef GL_AMD_draw_buffers_blend

static GLboolean _glewInit_GL_AMD_draw_buffers_blend (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBlendEquationIndexedAMD = (PFNGLBLENDEQUATIONINDEXEDAMDPROC)glewGetProcAddress((const GLubyte*)"glBlendEquationIndexedAMD")) == null) || r;
  r = ((glBlendEquationSeparateIndexedAMD = (PFNGLBLENDEQUATIONSEPARATEINDEXEDAMDPROC)glewGetProcAddress((const GLubyte*)"glBlendEquationSeparateIndexedAMD")) == null) || r;
  r = ((glBlendFuncIndexedAMD = (PFNGLBLENDFUNCINDEXEDAMDPROC)glewGetProcAddress((const GLubyte*)"glBlendFuncIndexedAMD")) == null) || r;
  r = ((glBlendFuncSeparateIndexedAMD = (PFNGLBLENDFUNCSEPARATEINDEXEDAMDPROC)glewGetProcAddress((const GLubyte*)"glBlendFuncSeparateIndexedAMD")) == null) || r;

  return r;
}

#endif /* GL_AMD_draw_buffers_blend */

#ifdef GL_AMD_performance_monitor

static GLboolean _glewInit_GL_AMD_performance_monitor (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBeginPerfMonitorAMD = (PFNGLBEGINPERFMONITORAMDPROC)glewGetProcAddress((const GLubyte*)"glBeginPerfMonitorAMD")) == null) || r;
  r = ((glDeletePerfMonitorsAMD = (PFNGLDELETEPERFMONITORSAMDPROC)glewGetProcAddress((const GLubyte*)"glDeletePerfMonitorsAMD")) == null) || r;
  r = ((glEndPerfMonitorAMD = (PFNGLENDPERFMONITORAMDPROC)glewGetProcAddress((const GLubyte*)"glEndPerfMonitorAMD")) == null) || r;
  r = ((glGenPerfMonitorsAMD = (PFNGLGENPERFMONITORSAMDPROC)glewGetProcAddress((const GLubyte*)"glGenPerfMonitorsAMD")) == null) || r;
  r = ((glGetPerfMonitorCounterDataAMD = (PFNGLGETPERFMONITORCOUNTERDATAAMDPROC)glewGetProcAddress((const GLubyte*)"glGetPerfMonitorCounterDataAMD")) == null) || r;
  r = ((glGetPerfMonitorCounterInfoAMD = (PFNGLGETPERFMONITORCOUNTERINFOAMDPROC)glewGetProcAddress((const GLubyte*)"glGetPerfMonitorCounterInfoAMD")) == null) || r;
  r = ((glGetPerfMonitorCounterStringAMD = (PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC)glewGetProcAddress((const GLubyte*)"glGetPerfMonitorCounterStringAMD")) == null) || r;
  r = ((glGetPerfMonitorCountersAMD = (PFNGLGETPERFMONITORCOUNTERSAMDPROC)glewGetProcAddress((const GLubyte*)"glGetPerfMonitorCountersAMD")) == null) || r;
  r = ((glGetPerfMonitorGroupStringAMD = (PFNGLGETPERFMONITORGROUPSTRINGAMDPROC)glewGetProcAddress((const GLubyte*)"glGetPerfMonitorGroupStringAMD")) == null) || r;
  r = ((glGetPerfMonitorGroupsAMD = (PFNGLGETPERFMONITORGROUPSAMDPROC)glewGetProcAddress((const GLubyte*)"glGetPerfMonitorGroupsAMD")) == null) || r;
  r = ((glSelectPerfMonitorCountersAMD = (PFNGLSELECTPERFMONITORCOUNTERSAMDPROC)glewGetProcAddress((const GLubyte*)"glSelectPerfMonitorCountersAMD")) == null) || r;

  return r;
}

#endif /* GL_AMD_performance_monitor */

#ifdef GL_AMD_seamless_cubemap_per_texture

#endif /* GL_AMD_seamless_cubemap_per_texture */

#ifdef GL_AMD_shader_stencil_export

#endif /* GL_AMD_shader_stencil_export */

#ifdef GL_AMD_texture_texture4

#endif /* GL_AMD_texture_texture4 */

#ifdef GL_AMD_vertex_shader_tessellator

static GLboolean _glewInit_GL_AMD_vertex_shader_tessellator (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glTessellationFactorAMD = (PFNGLTESSELLATIONFACTORAMDPROC)glewGetProcAddress((const GLubyte*)"glTessellationFactorAMD")) == null) || r;
  r = ((glTessellationModeAMD = (PFNGLTESSELLATIONMODEAMDPROC)glewGetProcAddress((const GLubyte*)"glTessellationModeAMD")) == null) || r;

  return r;
}

#endif /* GL_AMD_vertex_shader_tessellator */

#ifdef GL_APPLE_aux_depth_stencil

#endif /* GL_APPLE_aux_depth_stencil */

#ifdef GL_APPLE_client_storage

#endif /* GL_APPLE_client_storage */

#ifdef GL_APPLE_element_array

static GLboolean _glewInit_GL_APPLE_element_array (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glDrawElementArrayAPPLE = (PFNGLDRAWELEMENTARRAYAPPLEPROC)glewGetProcAddress((const GLubyte*)"glDrawElementArrayAPPLE")) == null) || r;
  r = ((glDrawRangeElementArrayAPPLE = (PFNGLDRAWRANGEELEMENTARRAYAPPLEPROC)glewGetProcAddress((const GLubyte*)"glDrawRangeElementArrayAPPLE")) == null) || r;
  r = ((glElementPointerAPPLE = (PFNGLELEMENTPOINTERAPPLEPROC)glewGetProcAddress((const GLubyte*)"glElementPointerAPPLE")) == null) || r;
  r = ((glMultiDrawElementArrayAPPLE = (PFNGLMULTIDRAWELEMENTARRAYAPPLEPROC)glewGetProcAddress((const GLubyte*)"glMultiDrawElementArrayAPPLE")) == null) || r;
  r = ((glMultiDrawRangeElementArrayAPPLE = (PFNGLMULTIDRAWRANGEELEMENTARRAYAPPLEPROC)glewGetProcAddress((const GLubyte*)"glMultiDrawRangeElementArrayAPPLE")) == null) || r;

  return r;
}

#endif /* GL_APPLE_element_array */

#ifdef GL_APPLE_fence

static GLboolean _glewInit_GL_APPLE_fence (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glDeleteFencesAPPLE = (PFNGLDELETEFENCESAPPLEPROC)glewGetProcAddress((const GLubyte*)"glDeleteFencesAPPLE")) == null) || r;
  r = ((glFinishFenceAPPLE = (PFNGLFINISHFENCEAPPLEPROC)glewGetProcAddress((const GLubyte*)"glFinishFenceAPPLE")) == null) || r;
  r = ((glFinishObjectAPPLE = (PFNGLFINISHOBJECTAPPLEPROC)glewGetProcAddress((const GLubyte*)"glFinishObjectAPPLE")) == null) || r;
  r = ((glGenFencesAPPLE = (PFNGLGENFENCESAPPLEPROC)glewGetProcAddress((const GLubyte*)"glGenFencesAPPLE")) == null) || r;
  r = ((glIsFenceAPPLE = (PFNGLISFENCEAPPLEPROC)glewGetProcAddress((const GLubyte*)"glIsFenceAPPLE")) == null) || r;
  r = ((glSetFenceAPPLE = (PFNGLSETFENCEAPPLEPROC)glewGetProcAddress((const GLubyte*)"glSetFenceAPPLE")) == null) || r;
  r = ((glTestFenceAPPLE = (PFNGLTESTFENCEAPPLEPROC)glewGetProcAddress((const GLubyte*)"glTestFenceAPPLE")) == null) || r;
  r = ((glTestObjectAPPLE = (PFNGLTESTOBJECTAPPLEPROC)glewGetProcAddress((const GLubyte*)"glTestObjectAPPLE")) == null) || r;

  return r;
}

#endif /* GL_APPLE_fence */

#ifdef GL_APPLE_float_pixels

#endif /* GL_APPLE_float_pixels */

#ifdef GL_APPLE_flush_buffer_range

static GLboolean _glewInit_GL_APPLE_flush_buffer_range (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBufferParameteriAPPLE = (PFNGLBUFFERPARAMETERIAPPLEPROC)glewGetProcAddress((const GLubyte*)"glBufferParameteriAPPLE")) == null) || r;
  r = ((glFlushMappedBufferRangeAPPLE = (PFNGLFLUSHMAPPEDBUFFERRANGEAPPLEPROC)glewGetProcAddress((const GLubyte*)"glFlushMappedBufferRangeAPPLE")) == null) || r;

  return r;
}

#endif /* GL_APPLE_flush_buffer_range */

#ifdef GL_APPLE_object_purgeable

static GLboolean _glewInit_GL_APPLE_object_purgeable (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glGetObjectParameterivAPPLE = (PFNGLGETOBJECTPARAMETERIVAPPLEPROC)glewGetProcAddress((const GLubyte*)"glGetObjectParameterivAPPLE")) == null) || r;
  r = ((glObjectPurgeableAPPLE = (PFNGLOBJECTPURGEABLEAPPLEPROC)glewGetProcAddress((const GLubyte*)"glObjectPurgeableAPPLE")) == null) || r;
  r = ((glObjectUnpurgeableAPPLE = (PFNGLOBJECTUNPURGEABLEAPPLEPROC)glewGetProcAddress((const GLubyte*)"glObjectUnpurgeableAPPLE")) == null) || r;

  return r;
}

#endif /* GL_APPLE_object_purgeable */

#ifdef GL_APPLE_pixel_buffer

#endif /* GL_APPLE_pixel_buffer */

#ifdef GL_APPLE_rgb_422

#endif /* GL_APPLE_rgb_422 */

#ifdef GL_APPLE_row_bytes

#endif /* GL_APPLE_row_bytes */

#ifdef GL_APPLE_specular_vector

#endif /* GL_APPLE_specular_vector */

#ifdef GL_APPLE_texture_range

static GLboolean _glewInit_GL_APPLE_texture_range (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glGetTexParameterPointervAPPLE = (PFNGLGETTEXPARAMETERPOINTERVAPPLEPROC)glewGetProcAddress((const GLubyte*)"glGetTexParameterPointervAPPLE")) == null) || r;
  r = ((glTextureRangeAPPLE = (PFNGLTEXTURERANGEAPPLEPROC)glewGetProcAddress((const GLubyte*)"glTextureRangeAPPLE")) == null) || r;

  return r;
}

#endif /* GL_APPLE_texture_range */

#ifdef GL_APPLE_transform_hint

#endif /* GL_APPLE_transform_hint */

#ifdef GL_APPLE_vertex_array_object

static GLboolean _glewInit_GL_APPLE_vertex_array_object (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBindVertexArrayAPPLE = (PFNGLBINDVERTEXARRAYAPPLEPROC)glewGetProcAddress((const GLubyte*)"glBindVertexArrayAPPLE")) == null) || r;
  r = ((glDeleteVertexArraysAPPLE = (PFNGLDELETEVERTEXARRAYSAPPLEPROC)glewGetProcAddress((const GLubyte*)"glDeleteVertexArraysAPPLE")) == null) || r;
  r = ((glGenVertexArraysAPPLE = (PFNGLGENVERTEXARRAYSAPPLEPROC)glewGetProcAddress((const GLubyte*)"glGenVertexArraysAPPLE")) == null) || r;
  r = ((glIsVertexArrayAPPLE = (PFNGLISVERTEXARRAYAPPLEPROC)glewGetProcAddress((const GLubyte*)"glIsVertexArrayAPPLE")) == null) || r;

  return r;
}

#endif /* GL_APPLE_vertex_array_object */

#ifdef GL_APPLE_vertex_array_range

static GLboolean _glewInit_GL_APPLE_vertex_array_range (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glFlushVertexArrayRangeAPPLE = (PFNGLFLUSHVERTEXARRAYRANGEAPPLEPROC)glewGetProcAddress((const GLubyte*)"glFlushVertexArrayRangeAPPLE")) == null) || r;
  r = ((glVertexArrayParameteriAPPLE = (PFNGLVERTEXARRAYPARAMETERIAPPLEPROC)glewGetProcAddress((const GLubyte*)"glVertexArrayParameteriAPPLE")) == null) || r;
  r = ((glVertexArrayRangeAPPLE = (PFNGLVERTEXARRAYRANGEAPPLEPROC)glewGetProcAddress((const GLubyte*)"glVertexArrayRangeAPPLE")) == null) || r;

  return r;
}

#endif /* GL_APPLE_vertex_array_range */

#ifdef GL_APPLE_vertex_program_evaluators

static GLboolean _glewInit_GL_APPLE_vertex_program_evaluators (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glDisableVertexAttribAPPLE = (PFNGLDISABLEVERTEXATTRIBAPPLEPROC)glewGetProcAddress((const GLubyte*)"glDisableVertexAttribAPPLE")) == null) || r;
  r = ((glEnableVertexAttribAPPLE = (PFNGLENABLEVERTEXATTRIBAPPLEPROC)glewGetProcAddress((const GLubyte*)"glEnableVertexAttribAPPLE")) == null) || r;
  r = ((glIsVertexAttribEnabledAPPLE = (PFNGLISVERTEXATTRIBENABLEDAPPLEPROC)glewGetProcAddress((const GLubyte*)"glIsVertexAttribEnabledAPPLE")) == null) || r;
  r = ((glMapVertexAttrib1dAPPLE = (PFNGLMAPVERTEXATTRIB1DAPPLEPROC)glewGetProcAddress((const GLubyte*)"glMapVertexAttrib1dAPPLE")) == null) || r;
  r = ((glMapVertexAttrib1fAPPLE = (PFNGLMAPVERTEXATTRIB1FAPPLEPROC)glewGetProcAddress((const GLubyte*)"glMapVertexAttrib1fAPPLE")) == null) || r;
  r = ((glMapVertexAttrib2dAPPLE = (PFNGLMAPVERTEXATTRIB2DAPPLEPROC)glewGetProcAddress((const GLubyte*)"glMapVertexAttrib2dAPPLE")) == null) || r;
  r = ((glMapVertexAttrib2fAPPLE = (PFNGLMAPVERTEXATTRIB2FAPPLEPROC)glewGetProcAddress((const GLubyte*)"glMapVertexAttrib2fAPPLE")) == null) || r;

  return r;
}

#endif /* GL_APPLE_vertex_program_evaluators */

#ifdef GL_APPLE_ycbcr_422

#endif /* GL_APPLE_ycbcr_422 */

#ifdef GL_ARB_color_buffer_float

static GLboolean _glewInit_GL_ARB_color_buffer_float (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glClampColorARB = (PFNGLCLAMPCOLORARBPROC)glewGetProcAddress((const GLubyte*)"glClampColorARB")) == null) || r;

  return r;
}

#endif /* GL_ARB_color_buffer_float */

#ifdef GL_ARB_compatibility

#endif /* GL_ARB_compatibility */

#ifdef GL_ARB_copy_buffer

static GLboolean _glewInit_GL_ARB_copy_buffer (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glCopyBufferSubData = (PFNGLCOPYBUFFERSUBDATAPROC)glewGetProcAddress((const GLubyte*)"glCopyBufferSubData")) == null) || r;

  return r;
}

#endif /* GL_ARB_copy_buffer */

#ifdef GL_ARB_depth_buffer_float

#endif /* GL_ARB_depth_buffer_float */

#ifdef GL_ARB_depth_clamp

#endif /* GL_ARB_depth_clamp */

#ifdef GL_ARB_depth_texture

#endif /* GL_ARB_depth_texture */

#ifdef GL_ARB_draw_buffers

static GLboolean _glewInit_GL_ARB_draw_buffers (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glDrawBuffersARB = (PFNGLDRAWBUFFERSARBPROC)glewGetProcAddress((const GLubyte*)"glDrawBuffersARB")) == null) || r;

  return r;
}

#endif /* GL_ARB_draw_buffers */

#ifdef GL_ARB_draw_buffers_blend

static GLboolean _glewInit_GL_ARB_draw_buffers_blend (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBlendEquationSeparateiARB = (PFNGLBLENDEQUATIONSEPARATEIARBPROC)glewGetProcAddress((const GLubyte*)"glBlendEquationSeparateiARB")) == null) || r;
  r = ((glBlendEquationiARB = (PFNGLBLENDEQUATIONIARBPROC)glewGetProcAddress((const GLubyte*)"glBlendEquationiARB")) == null) || r;
  r = ((glBlendFuncSeparateiARB = (PFNGLBLENDFUNCSEPARATEIARBPROC)glewGetProcAddress((const GLubyte*)"glBlendFuncSeparateiARB")) == null) || r;
  r = ((glBlendFunciARB = (PFNGLBLENDFUNCIARBPROC)glewGetProcAddress((const GLubyte*)"glBlendFunciARB")) == null) || r;

  return r;
}

#endif /* GL_ARB_draw_buffers_blend */

#ifdef GL_ARB_draw_elements_base_vertex

static GLboolean _glewInit_GL_ARB_draw_elements_base_vertex (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glDrawElementsBaseVertex = (PFNGLDRAWELEMENTSBASEVERTEXPROC)glewGetProcAddress((const GLubyte*)"glDrawElementsBaseVertex")) == null) || r;
  r = ((glDrawElementsInstancedBaseVertex = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC)glewGetProcAddress((const GLubyte*)"glDrawElementsInstancedBaseVertex")) == null) || r;
  r = ((glDrawRangeElementsBaseVertex = (PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC)glewGetProcAddress((const GLubyte*)"glDrawRangeElementsBaseVertex")) == null) || r;
  r = ((glMultiDrawElementsBaseVertex = (PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC)glewGetProcAddress((const GLubyte*)"glMultiDrawElementsBaseVertex")) == null) || r;

  return r;
}

#endif /* GL_ARB_draw_elements_base_vertex */

#ifdef GL_ARB_draw_instanced

static GLboolean _glewInit_GL_ARB_draw_instanced (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glDrawArraysInstancedARB = (PFNGLDRAWARRAYSINSTANCEDARBPROC)glewGetProcAddress((const GLubyte*)"glDrawArraysInstancedARB")) == null) || r;
  r = ((glDrawElementsInstancedARB = (PFNGLDRAWELEMENTSINSTANCEDARBPROC)glewGetProcAddress((const GLubyte*)"glDrawElementsInstancedARB")) == null) || r;

  return r;
}

#endif /* GL_ARB_draw_instanced */

#ifdef GL_ARB_fragment_coord_conventions

#endif /* GL_ARB_fragment_coord_conventions */

#ifdef GL_ARB_fragment_program

#endif /* GL_ARB_fragment_program */

#ifdef GL_ARB_fragment_program_shadow

#endif /* GL_ARB_fragment_program_shadow */

#ifdef GL_ARB_fragment_shader

#endif /* GL_ARB_fragment_shader */

#ifdef GL_ARB_framebuffer_object

static GLboolean _glewInit_GL_ARB_framebuffer_object (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)glewGetProcAddress((const GLubyte*)"glBindFramebuffer")) == null) || r;
  r = ((glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)glewGetProcAddress((const GLubyte*)"glBindRenderbuffer")) == null) || r;
  r = ((glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)glewGetProcAddress((const GLubyte*)"glBlitFramebuffer")) == null) || r;
  r = ((glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)glewGetProcAddress((const GLubyte*)"glCheckFramebufferStatus")) == null) || r;
  r = ((glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)glewGetProcAddress((const GLubyte*)"glDeleteFramebuffers")) == null) || r;
  r = ((glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)glewGetProcAddress((const GLubyte*)"glDeleteRenderbuffers")) == null) || r;
  r = ((glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)glewGetProcAddress((const GLubyte*)"glFramebufferRenderbuffer")) == null) || r;
  r = ((glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)glewGetProcAddress((const GLubyte*)"glFramebufferTexture1D")) == null) || r;
  r = ((glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)glewGetProcAddress((const GLubyte*)"glFramebufferTexture2D")) == null) || r;
  r = ((glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)glewGetProcAddress((const GLubyte*)"glFramebufferTexture3D")) == null) || r;
  r = ((glFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC)glewGetProcAddress((const GLubyte*)"glFramebufferTextureLayer")) == null) || r;
  r = ((glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)glewGetProcAddress((const GLubyte*)"glGenFramebuffers")) == null) || r;
  r = ((glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)glewGetProcAddress((const GLubyte*)"glGenRenderbuffers")) == null) || r;
  r = ((glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)glewGetProcAddress((const GLubyte*)"glGenerateMipmap")) == null) || r;
  r = ((glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)glewGetProcAddress((const GLubyte*)"glGetFramebufferAttachmentParameteriv")) == null) || r;
  r = ((glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)glewGetProcAddress((const GLubyte*)"glGetRenderbufferParameteriv")) == null) || r;
  r = ((glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)glewGetProcAddress((const GLubyte*)"glIsFramebuffer")) == null) || r;
  r = ((glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)glewGetProcAddress((const GLubyte*)"glIsRenderbuffer")) == null) || r;
  r = ((glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)glewGetProcAddress((const GLubyte*)"glRenderbufferStorage")) == null) || r;
  r = ((glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)glewGetProcAddress((const GLubyte*)"glRenderbufferStorageMultisample")) == null) || r;

  return r;
}

#endif /* GL_ARB_framebuffer_object */

#ifdef GL_ARB_framebuffer_sRGB

#endif /* GL_ARB_framebuffer_sRGB */

#ifdef GL_ARB_geometry_shader4

static GLboolean _glewInit_GL_ARB_geometry_shader4 (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glFramebufferTextureARB = (PFNGLFRAMEBUFFERTEXTUREARBPROC)glewGetProcAddress((const GLubyte*)"glFramebufferTextureARB")) == null) || r;
  r = ((glFramebufferTextureFaceARB = (PFNGLFRAMEBUFFERTEXTUREFACEARBPROC)glewGetProcAddress((const GLubyte*)"glFramebufferTextureFaceARB")) == null) || r;
  r = ((glFramebufferTextureLayerARB = (PFNGLFRAMEBUFFERTEXTURELAYERARBPROC)glewGetProcAddress((const GLubyte*)"glFramebufferTextureLayerARB")) == null) || r;
  r = ((glProgramParameteriARB = (PFNGLPROGRAMPARAMETERIARBPROC)glewGetProcAddress((const GLubyte*)"glProgramParameteriARB")) == null) || r;

  return r;
}

#endif /* GL_ARB_geometry_shader4 */

#ifdef GL_ARB_half_float_pixel

#endif /* GL_ARB_half_float_pixel */

#ifdef GL_ARB_half_float_vertex

#endif /* GL_ARB_half_float_vertex */

#ifdef GL_ARB_imaging

static GLboolean _glewInit_GL_ARB_imaging (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBlendEquation = (PFNGLBLENDEQUATIONPROC)glewGetProcAddress((const GLubyte*)"glBlendEquation")) == null) || r;
  r = ((glColorSubTable = (PFNGLCOLORSUBTABLEPROC)glewGetProcAddress((const GLubyte*)"glColorSubTable")) == null) || r;
  r = ((glColorTable = (PFNGLCOLORTABLEPROC)glewGetProcAddress((const GLubyte*)"glColorTable")) == null) || r;
  r = ((glColorTableParameterfv = (PFNGLCOLORTABLEPARAMETERFVPROC)glewGetProcAddress((const GLubyte*)"glColorTableParameterfv")) == null) || r;
  r = ((glColorTableParameteriv = (PFNGLCOLORTABLEPARAMETERIVPROC)glewGetProcAddress((const GLubyte*)"glColorTableParameteriv")) == null) || r;
  r = ((glConvolutionFilter1D = (PFNGLCONVOLUTIONFILTER1DPROC)glewGetProcAddress((const GLubyte*)"glConvolutionFilter1D")) == null) || r;
  r = ((glConvolutionFilter2D = (PFNGLCONVOLUTIONFILTER2DPROC)glewGetProcAddress((const GLubyte*)"glConvolutionFilter2D")) == null) || r;
  r = ((glConvolutionParameterf = (PFNGLCONVOLUTIONPARAMETERFPROC)glewGetProcAddress((const GLubyte*)"glConvolutionParameterf")) == null) || r;
  r = ((glConvolutionParameterfv = (PFNGLCONVOLUTIONPARAMETERFVPROC)glewGetProcAddress((const GLubyte*)"glConvolutionParameterfv")) == null) || r;
  r = ((glConvolutionParameteri = (PFNGLCONVOLUTIONPARAMETERIPROC)glewGetProcAddress((const GLubyte*)"glConvolutionParameteri")) == null) || r;
  r = ((glConvolutionParameteriv = (PFNGLCONVOLUTIONPARAMETERIVPROC)glewGetProcAddress((const GLubyte*)"glConvolutionParameteriv")) == null) || r;
  r = ((glCopyColorSubTable = (PFNGLCOPYCOLORSUBTABLEPROC)glewGetProcAddress((const GLubyte*)"glCopyColorSubTable")) == null) || r;
  r = ((glCopyColorTable = (PFNGLCOPYCOLORTABLEPROC)glewGetProcAddress((const GLubyte*)"glCopyColorTable")) == null) || r;
  r = ((glCopyConvolutionFilter1D = (PFNGLCOPYCONVOLUTIONFILTER1DPROC)glewGetProcAddress((const GLubyte*)"glCopyConvolutionFilter1D")) == null) || r;
  r = ((glCopyConvolutionFilter2D = (PFNGLCOPYCONVOLUTIONFILTER2DPROC)glewGetProcAddress((const GLubyte*)"glCopyConvolutionFilter2D")) == null) || r;
  r = ((glGetColorTable = (PFNGLGETCOLORTABLEPROC)glewGetProcAddress((const GLubyte*)"glGetColorTable")) == null) || r;
  r = ((glGetColorTableParameterfv = (PFNGLGETCOLORTABLEPARAMETERFVPROC)glewGetProcAddress((const GLubyte*)"glGetColorTableParameterfv")) == null) || r;
  r = ((glGetColorTableParameteriv = (PFNGLGETCOLORTABLEPARAMETERIVPROC)glewGetProcAddress((const GLubyte*)"glGetColorTableParameteriv")) == null) || r;
  r = ((glGetConvolutionFilter = (PFNGLGETCONVOLUTIONFILTERPROC)glewGetProcAddress((const GLubyte*)"glGetConvolutionFilter")) == null) || r;
  r = ((glGetConvolutionParameterfv = (PFNGLGETCONVOLUTIONPARAMETERFVPROC)glewGetProcAddress((const GLubyte*)"glGetConvolutionParameterfv")) == null) || r;
  r = ((glGetConvolutionParameteriv = (PFNGLGETCONVOLUTIONPARAMETERIVPROC)glewGetProcAddress((const GLubyte*)"glGetConvolutionParameteriv")) == null) || r;
  r = ((glGetHistogram = (PFNGLGETHISTOGRAMPROC)glewGetProcAddress((const GLubyte*)"glGetHistogram")) == null) || r;
  r = ((glGetHistogramParameterfv = (PFNGLGETHISTOGRAMPARAMETERFVPROC)glewGetProcAddress((const GLubyte*)"glGetHistogramParameterfv")) == null) || r;
  r = ((glGetHistogramParameteriv = (PFNGLGETHISTOGRAMPARAMETERIVPROC)glewGetProcAddress((const GLubyte*)"glGetHistogramParameteriv")) == null) || r;
  r = ((glGetMinmax = (PFNGLGETMINMAXPROC)glewGetProcAddress((const GLubyte*)"glGetMinmax")) == null) || r;
  r = ((glGetMinmaxParameterfv = (PFNGLGETMINMAXPARAMETERFVPROC)glewGetProcAddress((const GLubyte*)"glGetMinmaxParameterfv")) == null) || r;
  r = ((glGetMinmaxParameteriv = (PFNGLGETMINMAXPARAMETERIVPROC)glewGetProcAddress((const GLubyte*)"glGetMinmaxParameteriv")) == null) || r;
  r = ((glGetSeparableFilter = (PFNGLGETSEPARABLEFILTERPROC)glewGetProcAddress((const GLubyte*)"glGetSeparableFilter")) == null) || r;
  r = ((glHistogram = (PFNGLHISTOGRAMPROC)glewGetProcAddress((const GLubyte*)"glHistogram")) == null) || r;
  r = ((glMinmax = (PFNGLMINMAXPROC)glewGetProcAddress((const GLubyte*)"glMinmax")) == null) || r;
  r = ((glResetHistogram = (PFNGLRESETHISTOGRAMPROC)glewGetProcAddress((const GLubyte*)"glResetHistogram")) == null) || r;
  r = ((glResetMinmax = (PFNGLRESETMINMAXPROC)glewGetProcAddress((const GLubyte*)"glResetMinmax")) == null) || r;
  r = ((glSeparableFilter2D = (PFNGLSEPARABLEFILTER2DPROC)glewGetProcAddress((const GLubyte*)"glSeparableFilter2D")) == null) || r;

  return r;
}

#endif /* GL_ARB_imaging */

#ifdef GL_ARB_instanced_arrays

static GLboolean _glewInit_GL_ARB_instanced_arrays (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glVertexAttribDivisorARB = (PFNGLVERTEXATTRIBDIVISORARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribDivisorARB")) == null) || r;

  return r;
}

#endif /* GL_ARB_instanced_arrays */

#ifdef GL_ARB_map_buffer_range

static GLboolean _glewInit_GL_ARB_map_buffer_range (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC)glewGetProcAddress((const GLubyte*)"glFlushMappedBufferRange")) == null) || r;
  r = ((glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC)glewGetProcAddress((const GLubyte*)"glMapBufferRange")) == null) || r;

  return r;
}

#endif /* GL_ARB_map_buffer_range */

#ifdef GL_ARB_matrix_palette

static GLboolean _glewInit_GL_ARB_matrix_palette (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glCurrentPaletteMatrixARB = (PFNGLCURRENTPALETTEMATRIXARBPROC)glewGetProcAddress((const GLubyte*)"glCurrentPaletteMatrixARB")) == null) || r;
  r = ((glMatrixIndexPointerARB = (PFNGLMATRIXINDEXPOINTERARBPROC)glewGetProcAddress((const GLubyte*)"glMatrixIndexPointerARB")) == null) || r;
  r = ((glMatrixIndexubvARB = (PFNGLMATRIXINDEXUBVARBPROC)glewGetProcAddress((const GLubyte*)"glMatrixIndexubvARB")) == null) || r;
  r = ((glMatrixIndexuivARB = (PFNGLMATRIXINDEXUIVARBPROC)glewGetProcAddress((const GLubyte*)"glMatrixIndexuivARB")) == null) || r;
  r = ((glMatrixIndexusvARB = (PFNGLMATRIXINDEXUSVARBPROC)glewGetProcAddress((const GLubyte*)"glMatrixIndexusvARB")) == null) || r;

  return r;
}

#endif /* GL_ARB_matrix_palette */

#ifdef GL_ARB_multisample

static GLboolean _glewInit_GL_ARB_multisample (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glSampleCoverageARB = (PFNGLSAMPLECOVERAGEARBPROC)glewGetProcAddress((const GLubyte*)"glSampleCoverageARB")) == null) || r;

  return r;
}

#endif /* GL_ARB_multisample */

#ifdef GL_ARB_multitexture

static GLboolean _glewInit_GL_ARB_multitexture (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)glewGetProcAddress((const GLubyte*)"glActiveTextureARB")) == null) || r;
  r = ((glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)glewGetProcAddress((const GLubyte*)"glClientActiveTextureARB")) == null) || r;
  r = ((glMultiTexCoord1dARB = (PFNGLMULTITEXCOORD1DARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord1dARB")) == null) || r;
  r = ((glMultiTexCoord1dvARB = (PFNGLMULTITEXCOORD1DVARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord1dvARB")) == null) || r;
  r = ((glMultiTexCoord1fARB = (PFNGLMULTITEXCOORD1FARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord1fARB")) == null) || r;
  r = ((glMultiTexCoord1fvARB = (PFNGLMULTITEXCOORD1FVARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord1fvARB")) == null) || r;
  r = ((glMultiTexCoord1iARB = (PFNGLMULTITEXCOORD1IARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord1iARB")) == null) || r;
  r = ((glMultiTexCoord1ivARB = (PFNGLMULTITEXCOORD1IVARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord1ivARB")) == null) || r;
  r = ((glMultiTexCoord1sARB = (PFNGLMULTITEXCOORD1SARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord1sARB")) == null) || r;
  r = ((glMultiTexCoord1svARB = (PFNGLMULTITEXCOORD1SVARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord1svARB")) == null) || r;
  r = ((glMultiTexCoord2dARB = (PFNGLMULTITEXCOORD2DARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord2dARB")) == null) || r;
  r = ((glMultiTexCoord2dvARB = (PFNGLMULTITEXCOORD2DVARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord2dvARB")) == null) || r;
  r = ((glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord2fARB")) == null) || r;
  r = ((glMultiTexCoord2fvARB = (PFNGLMULTITEXCOORD2FVARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord2fvARB")) == null) || r;
  r = ((glMultiTexCoord2iARB = (PFNGLMULTITEXCOORD2IARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord2iARB")) == null) || r;
  r = ((glMultiTexCoord2ivARB = (PFNGLMULTITEXCOORD2IVARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord2ivARB")) == null) || r;
  r = ((glMultiTexCoord2sARB = (PFNGLMULTITEXCOORD2SARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord2sARB")) == null) || r;
  r = ((glMultiTexCoord2svARB = (PFNGLMULTITEXCOORD2SVARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord2svARB")) == null) || r;
  r = ((glMultiTexCoord3dARB = (PFNGLMULTITEXCOORD3DARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord3dARB")) == null) || r;
  r = ((glMultiTexCoord3dvARB = (PFNGLMULTITEXCOORD3DVARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord3dvARB")) == null) || r;
  r = ((glMultiTexCoord3fARB = (PFNGLMULTITEXCOORD3FARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord3fARB")) == null) || r;
  r = ((glMultiTexCoord3fvARB = (PFNGLMULTITEXCOORD3FVARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord3fvARB")) == null) || r;
  r = ((glMultiTexCoord3iARB = (PFNGLMULTITEXCOORD3IARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord3iARB")) == null) || r;
  r = ((glMultiTexCoord3ivARB = (PFNGLMULTITEXCOORD3IVARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord3ivARB")) == null) || r;
  r = ((glMultiTexCoord3sARB = (PFNGLMULTITEXCOORD3SARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord3sARB")) == null) || r;
  r = ((glMultiTexCoord3svARB = (PFNGLMULTITEXCOORD3SVARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord3svARB")) == null) || r;
  r = ((glMultiTexCoord4dARB = (PFNGLMULTITEXCOORD4DARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord4dARB")) == null) || r;
  r = ((glMultiTexCoord4dvARB = (PFNGLMULTITEXCOORD4DVARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord4dvARB")) == null) || r;
  r = ((glMultiTexCoord4fARB = (PFNGLMULTITEXCOORD4FARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord4fARB")) == null) || r;
  r = ((glMultiTexCoord4fvARB = (PFNGLMULTITEXCOORD4FVARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord4fvARB")) == null) || r;
  r = ((glMultiTexCoord4iARB = (PFNGLMULTITEXCOORD4IARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord4iARB")) == null) || r;
  r = ((glMultiTexCoord4ivARB = (PFNGLMULTITEXCOORD4IVARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord4ivARB")) == null) || r;
  r = ((glMultiTexCoord4sARB = (PFNGLMULTITEXCOORD4SARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord4sARB")) == null) || r;
  r = ((glMultiTexCoord4svARB = (PFNGLMULTITEXCOORD4SVARBPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord4svARB")) == null) || r;

  return r;
}

#endif /* GL_ARB_multitexture */

#ifdef GL_ARB_occlusion_query

static GLboolean _glewInit_GL_ARB_occlusion_query (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBeginQueryARB = (PFNGLBEGINQUERYARBPROC)glewGetProcAddress((const GLubyte*)"glBeginQueryARB")) == null) || r;
  r = ((glDeleteQueriesARB = (PFNGLDELETEQUERIESARBPROC)glewGetProcAddress((const GLubyte*)"glDeleteQueriesARB")) == null) || r;
  r = ((glEndQueryARB = (PFNGLENDQUERYARBPROC)glewGetProcAddress((const GLubyte*)"glEndQueryARB")) == null) || r;
  r = ((glGenQueriesARB = (PFNGLGENQUERIESARBPROC)glewGetProcAddress((const GLubyte*)"glGenQueriesARB")) == null) || r;
  r = ((glGetQueryObjectivARB = (PFNGLGETQUERYOBJECTIVARBPROC)glewGetProcAddress((const GLubyte*)"glGetQueryObjectivARB")) == null) || r;
  r = ((glGetQueryObjectuivARB = (PFNGLGETQUERYOBJECTUIVARBPROC)glewGetProcAddress((const GLubyte*)"glGetQueryObjectuivARB")) == null) || r;
  r = ((glGetQueryivARB = (PFNGLGETQUERYIVARBPROC)glewGetProcAddress((const GLubyte*)"glGetQueryivARB")) == null) || r;
  r = ((glIsQueryARB = (PFNGLISQUERYARBPROC)glewGetProcAddress((const GLubyte*)"glIsQueryARB")) == null) || r;

  return r;
}

#endif /* GL_ARB_occlusion_query */

#ifdef GL_ARB_pixel_buffer_object

#endif /* GL_ARB_pixel_buffer_object */

#ifdef GL_ARB_point_parameters

static GLboolean _glewInit_GL_ARB_point_parameters (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glPointParameterfARB = (PFNGLPOINTPARAMETERFARBPROC)glewGetProcAddress((const GLubyte*)"glPointParameterfARB")) == null) || r;
  r = ((glPointParameterfvARB = (PFNGLPOINTPARAMETERFVARBPROC)glewGetProcAddress((const GLubyte*)"glPointParameterfvARB")) == null) || r;

  return r;
}

#endif /* GL_ARB_point_parameters */

#ifdef GL_ARB_point_sprite

#endif /* GL_ARB_point_sprite */

#ifdef GL_ARB_provoking_vertex

static GLboolean _glewInit_GL_ARB_provoking_vertex (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glProvokingVertex = (PFNGLPROVOKINGVERTEXPROC)glewGetProcAddress((const GLubyte*)"glProvokingVertex")) == null) || r;

  return r;
}

#endif /* GL_ARB_provoking_vertex */

#ifdef GL_ARB_sample_shading

static GLboolean _glewInit_GL_ARB_sample_shading (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glMinSampleShadingARB = (PFNGLMINSAMPLESHADINGARBPROC)glewGetProcAddress((const GLubyte*)"glMinSampleShadingARB")) == null) || r;

  return r;
}

#endif /* GL_ARB_sample_shading */

#ifdef GL_ARB_seamless_cube_map

#endif /* GL_ARB_seamless_cube_map */

#ifdef GL_ARB_shader_objects

static GLboolean _glewInit_GL_ARB_shader_objects (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)glewGetProcAddress((const GLubyte*)"glAttachObjectARB")) == null) || r;
  r = ((glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)glewGetProcAddress((const GLubyte*)"glCompileShaderARB")) == null) || r;
  r = ((glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)glewGetProcAddress((const GLubyte*)"glCreateProgramObjectARB")) == null) || r;
  r = ((glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)glewGetProcAddress((const GLubyte*)"glCreateShaderObjectARB")) == null) || r;
  r = ((glDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC)glewGetProcAddress((const GLubyte*)"glDeleteObjectARB")) == null) || r;
  r = ((glDetachObjectARB = (PFNGLDETACHOBJECTARBPROC)glewGetProcAddress((const GLubyte*)"glDetachObjectARB")) == null) || r;
  r = ((glGetActiveUniformARB = (PFNGLGETACTIVEUNIFORMARBPROC)glewGetProcAddress((const GLubyte*)"glGetActiveUniformARB")) == null) || r;
  r = ((glGetAttachedObjectsARB = (PFNGLGETATTACHEDOBJECTSARBPROC)glewGetProcAddress((const GLubyte*)"glGetAttachedObjectsARB")) == null) || r;
  r = ((glGetHandleARB = (PFNGLGETHANDLEARBPROC)glewGetProcAddress((const GLubyte*)"glGetHandleARB")) == null) || r;
  r = ((glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)glewGetProcAddress((const GLubyte*)"glGetInfoLogARB")) == null) || r;
  r = ((glGetObjectParameterfvARB = (PFNGLGETOBJECTPARAMETERFVARBPROC)glewGetProcAddress((const GLubyte*)"glGetObjectParameterfvARB")) == null) || r;
  r = ((glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)glewGetProcAddress((const GLubyte*)"glGetObjectParameterivARB")) == null) || r;
  r = ((glGetShaderSourceARB = (PFNGLGETSHADERSOURCEARBPROC)glewGetProcAddress((const GLubyte*)"glGetShaderSourceARB")) == null) || r;
  r = ((glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)glewGetProcAddress((const GLubyte*)"glGetUniformLocationARB")) == null) || r;
  r = ((glGetUniformfvARB = (PFNGLGETUNIFORMFVARBPROC)glewGetProcAddress((const GLubyte*)"glGetUniformfvARB")) == null) || r;
  r = ((glGetUniformivARB = (PFNGLGETUNIFORMIVARBPROC)glewGetProcAddress((const GLubyte*)"glGetUniformivARB")) == null) || r;
  r = ((glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)glewGetProcAddress((const GLubyte*)"glLinkProgramARB")) == null) || r;
  r = ((glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)glewGetProcAddress((const GLubyte*)"glShaderSourceARB")) == null) || r;
  r = ((glUniform1fARB = (PFNGLUNIFORM1FARBPROC)glewGetProcAddress((const GLubyte*)"glUniform1fARB")) == null) || r;
  r = ((glUniform1fvARB = (PFNGLUNIFORM1FVARBPROC)glewGetProcAddress((const GLubyte*)"glUniform1fvARB")) == null) || r;
  r = ((glUniform1iARB = (PFNGLUNIFORM1IARBPROC)glewGetProcAddress((const GLubyte*)"glUniform1iARB")) == null) || r;
  r = ((glUniform1ivARB = (PFNGLUNIFORM1IVARBPROC)glewGetProcAddress((const GLubyte*)"glUniform1ivARB")) == null) || r;
  r = ((glUniform2fARB = (PFNGLUNIFORM2FARBPROC)glewGetProcAddress((const GLubyte*)"glUniform2fARB")) == null) || r;
  r = ((glUniform2fvARB = (PFNGLUNIFORM2FVARBPROC)glewGetProcAddress((const GLubyte*)"glUniform2fvARB")) == null) || r;
  r = ((glUniform2iARB = (PFNGLUNIFORM2IARBPROC)glewGetProcAddress((const GLubyte*)"glUniform2iARB")) == null) || r;
  r = ((glUniform2ivARB = (PFNGLUNIFORM2IVARBPROC)glewGetProcAddress((const GLubyte*)"glUniform2ivARB")) == null) || r;
  r = ((glUniform3fARB = (PFNGLUNIFORM3FARBPROC)glewGetProcAddress((const GLubyte*)"glUniform3fARB")) == null) || r;
  r = ((glUniform3fvARB = (PFNGLUNIFORM3FVARBPROC)glewGetProcAddress((const GLubyte*)"glUniform3fvARB")) == null) || r;
  r = ((glUniform3iARB = (PFNGLUNIFORM3IARBPROC)glewGetProcAddress((const GLubyte*)"glUniform3iARB")) == null) || r;
  r = ((glUniform3ivARB = (PFNGLUNIFORM3IVARBPROC)glewGetProcAddress((const GLubyte*)"glUniform3ivARB")) == null) || r;
  r = ((glUniform4fARB = (PFNGLUNIFORM4FARBPROC)glewGetProcAddress((const GLubyte*)"glUniform4fARB")) == null) || r;
  r = ((glUniform4fvARB = (PFNGLUNIFORM4FVARBPROC)glewGetProcAddress((const GLubyte*)"glUniform4fvARB")) == null) || r;
  r = ((glUniform4iARB = (PFNGLUNIFORM4IARBPROC)glewGetProcAddress((const GLubyte*)"glUniform4iARB")) == null) || r;
  r = ((glUniform4ivARB = (PFNGLUNIFORM4IVARBPROC)glewGetProcAddress((const GLubyte*)"glUniform4ivARB")) == null) || r;
  r = ((glUniformMatrix2fvARB = (PFNGLUNIFORMMATRIX2FVARBPROC)glewGetProcAddress((const GLubyte*)"glUniformMatrix2fvARB")) == null) || r;
  r = ((glUniformMatrix3fvARB = (PFNGLUNIFORMMATRIX3FVARBPROC)glewGetProcAddress((const GLubyte*)"glUniformMatrix3fvARB")) == null) || r;
  r = ((glUniformMatrix4fvARB = (PFNGLUNIFORMMATRIX4FVARBPROC)glewGetProcAddress((const GLubyte*)"glUniformMatrix4fvARB")) == null) || r;
  r = ((glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)glewGetProcAddress((const GLubyte*)"glUseProgramObjectARB")) == null) || r;
  r = ((glValidateProgramARB = (PFNGLVALIDATEPROGRAMARBPROC)glewGetProcAddress((const GLubyte*)"glValidateProgramARB")) == null) || r;

  return r;
}

#endif /* GL_ARB_shader_objects */

#ifdef GL_ARB_shader_texture_lod

#endif /* GL_ARB_shader_texture_lod */

#ifdef GL_ARB_shading_language_100

#endif /* GL_ARB_shading_language_100 */

#ifdef GL_ARB_shadow

#endif /* GL_ARB_shadow */

#ifdef GL_ARB_shadow_ambient

#endif /* GL_ARB_shadow_ambient */

#ifdef GL_ARB_sync

static GLboolean _glewInit_GL_ARB_sync (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glClientWaitSync = (PFNGLCLIENTWAITSYNCPROC)glewGetProcAddress((const GLubyte*)"glClientWaitSync")) == null) || r;
  r = ((glDeleteSync = (PFNGLDELETESYNCPROC)glewGetProcAddress((const GLubyte*)"glDeleteSync")) == null) || r;
  r = ((glFenceSync = (PFNGLFENCESYNCPROC)glewGetProcAddress((const GLubyte*)"glFenceSync")) == null) || r;
  r = ((glGetInteger64v = (PFNGLGETINTEGER64VPROC)glewGetProcAddress((const GLubyte*)"glGetInteger64v")) == null) || r;
  r = ((glGetSynciv = (PFNGLGETSYNCIVPROC)glewGetProcAddress((const GLubyte*)"glGetSynciv")) == null) || r;
  r = ((glIsSync = (PFNGLISSYNCPROC)glewGetProcAddress((const GLubyte*)"glIsSync")) == null) || r;
  r = ((glWaitSync = (PFNGLWAITSYNCPROC)glewGetProcAddress((const GLubyte*)"glWaitSync")) == null) || r;

  return r;
}

#endif /* GL_ARB_sync */

#ifdef GL_ARB_texture_border_clamp

#endif /* GL_ARB_texture_border_clamp */

#ifdef GL_ARB_texture_buffer_object

static GLboolean _glewInit_GL_ARB_texture_buffer_object (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glTexBufferARB = (PFNGLTEXBUFFERARBPROC)glewGetProcAddress((const GLubyte*)"glTexBufferARB")) == null) || r;

  return r;
}

#endif /* GL_ARB_texture_buffer_object */

#ifdef GL_ARB_texture_compression

static GLboolean _glewInit_GL_ARB_texture_compression (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glCompressedTexImage1DARB = (PFNGLCOMPRESSEDTEXIMAGE1DARBPROC)glewGetProcAddress((const GLubyte*)"glCompressedTexImage1DARB")) == null) || r;
  r = ((glCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)glewGetProcAddress((const GLubyte*)"glCompressedTexImage2DARB")) == null) || r;
  r = ((glCompressedTexImage3DARB = (PFNGLCOMPRESSEDTEXIMAGE3DARBPROC)glewGetProcAddress((const GLubyte*)"glCompressedTexImage3DARB")) == null) || r;
  r = ((glCompressedTexSubImage1DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC)glewGetProcAddress((const GLubyte*)"glCompressedTexSubImage1DARB")) == null) || r;
  r = ((glCompressedTexSubImage2DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC)glewGetProcAddress((const GLubyte*)"glCompressedTexSubImage2DARB")) == null) || r;
  r = ((glCompressedTexSubImage3DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC)glewGetProcAddress((const GLubyte*)"glCompressedTexSubImage3DARB")) == null) || r;
  r = ((glGetCompressedTexImageARB = (PFNGLGETCOMPRESSEDTEXIMAGEARBPROC)glewGetProcAddress((const GLubyte*)"glGetCompressedTexImageARB")) == null) || r;

  return r;
}

#endif /* GL_ARB_texture_compression */

#ifdef GL_ARB_texture_compression_rgtc

#endif /* GL_ARB_texture_compression_rgtc */

#ifdef GL_ARB_texture_cube_map

#endif /* GL_ARB_texture_cube_map */

#ifdef GL_ARB_texture_cube_map_array

#endif /* GL_ARB_texture_cube_map_array */

#ifdef GL_ARB_texture_env_add

#endif /* GL_ARB_texture_env_add */

#ifdef GL_ARB_texture_env_combine

#endif /* GL_ARB_texture_env_combine */

#ifdef GL_ARB_texture_env_crossbar

#endif /* GL_ARB_texture_env_crossbar */

#ifdef GL_ARB_texture_env_dot3

#endif /* GL_ARB_texture_env_dot3 */

#ifdef GL_ARB_texture_float

#endif /* GL_ARB_texture_float */

#ifdef GL_ARB_texture_gather

#endif /* GL_ARB_texture_gather */

#ifdef GL_ARB_texture_mirrored_repeat

#endif /* GL_ARB_texture_mirrored_repeat */

#ifdef GL_ARB_texture_multisample

static GLboolean _glewInit_GL_ARB_texture_multisample (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glGetMultisamplefv = (PFNGLGETMULTISAMPLEFVPROC)glewGetProcAddress((const GLubyte*)"glGetMultisamplefv")) == null) || r;
  r = ((glSampleMaski = (PFNGLSAMPLEMASKIPROC)glewGetProcAddress((const GLubyte*)"glSampleMaski")) == null) || r;
  r = ((glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)glewGetProcAddress((const GLubyte*)"glTexImage2DMultisample")) == null) || r;
  r = ((glTexImage3DMultisample = (PFNGLTEXIMAGE3DMULTISAMPLEPROC)glewGetProcAddress((const GLubyte*)"glTexImage3DMultisample")) == null) || r;

  return r;
}

#endif /* GL_ARB_texture_multisample */

#ifdef GL_ARB_texture_non_power_of_two

#endif /* GL_ARB_texture_non_power_of_two */

#ifdef GL_ARB_texture_query_lod

#endif /* GL_ARB_texture_query_lod */

#ifdef GL_ARB_texture_rectangle

#endif /* GL_ARB_texture_rectangle */

#ifdef GL_ARB_texture_rg

#endif /* GL_ARB_texture_rg */

#ifdef GL_ARB_transpose_matrix

static GLboolean _glewInit_GL_ARB_transpose_matrix (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glLoadTransposeMatrixdARB = (PFNGLLOADTRANSPOSEMATRIXDARBPROC)glewGetProcAddress((const GLubyte*)"glLoadTransposeMatrixdARB")) == null) || r;
  r = ((glLoadTransposeMatrixfARB = (PFNGLLOADTRANSPOSEMATRIXFARBPROC)glewGetProcAddress((const GLubyte*)"glLoadTransposeMatrixfARB")) == null) || r;
  r = ((glMultTransposeMatrixdARB = (PFNGLMULTTRANSPOSEMATRIXDARBPROC)glewGetProcAddress((const GLubyte*)"glMultTransposeMatrixdARB")) == null) || r;
  r = ((glMultTransposeMatrixfARB = (PFNGLMULTTRANSPOSEMATRIXFARBPROC)glewGetProcAddress((const GLubyte*)"glMultTransposeMatrixfARB")) == null) || r;

  return r;
}

#endif /* GL_ARB_transpose_matrix */

#ifdef GL_ARB_uniform_buffer_object

static GLboolean _glewInit_GL_ARB_uniform_buffer_object (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)glewGetProcAddress((const GLubyte*)"glBindBufferBase")) == null) || r;
  r = ((glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)glewGetProcAddress((const GLubyte*)"glBindBufferRange")) == null) || r;
  r = ((glGetActiveUniformBlockName = (PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC)glewGetProcAddress((const GLubyte*)"glGetActiveUniformBlockName")) == null) || r;
  r = ((glGetActiveUniformBlockiv = (PFNGLGETACTIVEUNIFORMBLOCKIVPROC)glewGetProcAddress((const GLubyte*)"glGetActiveUniformBlockiv")) == null) || r;
  r = ((glGetActiveUniformName = (PFNGLGETACTIVEUNIFORMNAMEPROC)glewGetProcAddress((const GLubyte*)"glGetActiveUniformName")) == null) || r;
  r = ((glGetActiveUniformsiv = (PFNGLGETACTIVEUNIFORMSIVPROC)glewGetProcAddress((const GLubyte*)"glGetActiveUniformsiv")) == null) || r;
  r = ((glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC)glewGetProcAddress((const GLubyte*)"glGetIntegeri_v")) == null) || r;
  r = ((glGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC)glewGetProcAddress((const GLubyte*)"glGetUniformBlockIndex")) == null) || r;
  r = ((glGetUniformIndices = (PFNGLGETUNIFORMINDICESPROC)glewGetProcAddress((const GLubyte*)"glGetUniformIndices")) == null) || r;
  r = ((glUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC)glewGetProcAddress((const GLubyte*)"glUniformBlockBinding")) == null) || r;

  return r;
}

#endif /* GL_ARB_uniform_buffer_object */

#ifdef GL_ARB_vertex_array_bgra

#endif /* GL_ARB_vertex_array_bgra */

#ifdef GL_ARB_vertex_array_object

static GLboolean _glewInit_GL_ARB_vertex_array_object (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glewGetProcAddress((const GLubyte*)"glBindVertexArray")) == null) || r;
  r = ((glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)glewGetProcAddress((const GLubyte*)"glDeleteVertexArrays")) == null) || r;
  r = ((glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glewGetProcAddress((const GLubyte*)"glGenVertexArrays")) == null) || r;
  r = ((glIsVertexArray = (PFNGLISVERTEXARRAYPROC)glewGetProcAddress((const GLubyte*)"glIsVertexArray")) == null) || r;

  return r;
}

#endif /* GL_ARB_vertex_array_object */

#ifdef GL_ARB_vertex_blend

static GLboolean _glewInit_GL_ARB_vertex_blend (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glVertexBlendARB = (PFNGLVERTEXBLENDARBPROC)glewGetProcAddress((const GLubyte*)"glVertexBlendARB")) == null) || r;
  r = ((glWeightPointerARB = (PFNGLWEIGHTPOINTERARBPROC)glewGetProcAddress((const GLubyte*)"glWeightPointerARB")) == null) || r;
  r = ((glWeightbvARB = (PFNGLWEIGHTBVARBPROC)glewGetProcAddress((const GLubyte*)"glWeightbvARB")) == null) || r;
  r = ((glWeightdvARB = (PFNGLWEIGHTDVARBPROC)glewGetProcAddress((const GLubyte*)"glWeightdvARB")) == null) || r;
  r = ((glWeightfvARB = (PFNGLWEIGHTFVARBPROC)glewGetProcAddress((const GLubyte*)"glWeightfvARB")) == null) || r;
  r = ((glWeightivARB = (PFNGLWEIGHTIVARBPROC)glewGetProcAddress((const GLubyte*)"glWeightivARB")) == null) || r;
  r = ((glWeightsvARB = (PFNGLWEIGHTSVARBPROC)glewGetProcAddress((const GLubyte*)"glWeightsvARB")) == null) || r;
  r = ((glWeightubvARB = (PFNGLWEIGHTUBVARBPROC)glewGetProcAddress((const GLubyte*)"glWeightubvARB")) == null) || r;
  r = ((glWeightuivARB = (PFNGLWEIGHTUIVARBPROC)glewGetProcAddress((const GLubyte*)"glWeightuivARB")) == null) || r;
  r = ((glWeightusvARB = (PFNGLWEIGHTUSVARBPROC)glewGetProcAddress((const GLubyte*)"glWeightusvARB")) == null) || r;

  return r;
}

#endif /* GL_ARB_vertex_blend */

#ifdef GL_ARB_vertex_buffer_object

static GLboolean _glewInit_GL_ARB_vertex_buffer_object (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBindBufferARB = (PFNGLBINDBUFFERARBPROC)glewGetProcAddress((const GLubyte*)"glBindBufferARB")) == null) || r;
  r = ((glBufferDataARB = (PFNGLBUFFERDATAARBPROC)glewGetProcAddress((const GLubyte*)"glBufferDataARB")) == null) || r;
  r = ((glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)glewGetProcAddress((const GLubyte*)"glBufferSubDataARB")) == null) || r;
  r = ((glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)glewGetProcAddress((const GLubyte*)"glDeleteBuffersARB")) == null) || r;
  r = ((glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)glewGetProcAddress((const GLubyte*)"glGenBuffersARB")) == null) || r;
  r = ((glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)glewGetProcAddress((const GLubyte*)"glGetBufferParameterivARB")) == null) || r;
  r = ((glGetBufferPointervARB = (PFNGLGETBUFFERPOINTERVARBPROC)glewGetProcAddress((const GLubyte*)"glGetBufferPointervARB")) == null) || r;
  r = ((glGetBufferSubDataARB = (PFNGLGETBUFFERSUBDATAARBPROC)glewGetProcAddress((const GLubyte*)"glGetBufferSubDataARB")) == null) || r;
  r = ((glIsBufferARB = (PFNGLISBUFFERARBPROC)glewGetProcAddress((const GLubyte*)"glIsBufferARB")) == null) || r;
  r = ((glMapBufferARB = (PFNGLMAPBUFFERARBPROC)glewGetProcAddress((const GLubyte*)"glMapBufferARB")) == null) || r;
  r = ((glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)glewGetProcAddress((const GLubyte*)"glUnmapBufferARB")) == null) || r;

  return r;
}

#endif /* GL_ARB_vertex_buffer_object */

#ifdef GL_ARB_vertex_program

static GLboolean _glewInit_GL_ARB_vertex_program (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBindProgramARB = (PFNGLBINDPROGRAMARBPROC)glewGetProcAddress((const GLubyte*)"glBindProgramARB")) == null) || r;
  r = ((glDeleteProgramsARB = (PFNGLDELETEPROGRAMSARBPROC)glewGetProcAddress((const GLubyte*)"glDeleteProgramsARB")) == null) || r;
  r = ((glDisableVertexAttribArrayARB = (PFNGLDISABLEVERTEXATTRIBARRAYARBPROC)glewGetProcAddress((const GLubyte*)"glDisableVertexAttribArrayARB")) == null) || r;
  r = ((glEnableVertexAttribArrayARB = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC)glewGetProcAddress((const GLubyte*)"glEnableVertexAttribArrayARB")) == null) || r;
  r = ((glGenProgramsARB = (PFNGLGENPROGRAMSARBPROC)glewGetProcAddress((const GLubyte*)"glGenProgramsARB")) == null) || r;
  r = ((glGetProgramEnvParameterdvARB = (PFNGLGETPROGRAMENVPARAMETERDVARBPROC)glewGetProcAddress((const GLubyte*)"glGetProgramEnvParameterdvARB")) == null) || r;
  r = ((glGetProgramEnvParameterfvARB = (PFNGLGETPROGRAMENVPARAMETERFVARBPROC)glewGetProcAddress((const GLubyte*)"glGetProgramEnvParameterfvARB")) == null) || r;
  r = ((glGetProgramLocalParameterdvARB = (PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC)glewGetProcAddress((const GLubyte*)"glGetProgramLocalParameterdvARB")) == null) || r;
  r = ((glGetProgramLocalParameterfvARB = (PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC)glewGetProcAddress((const GLubyte*)"glGetProgramLocalParameterfvARB")) == null) || r;
  r = ((glGetProgramStringARB = (PFNGLGETPROGRAMSTRINGARBPROC)glewGetProcAddress((const GLubyte*)"glGetProgramStringARB")) == null) || r;
  r = ((glGetProgramivARB = (PFNGLGETPROGRAMIVARBPROC)glewGetProcAddress((const GLubyte*)"glGetProgramivARB")) == null) || r;
  r = ((glGetVertexAttribPointervARB = (PFNGLGETVERTEXATTRIBPOINTERVARBPROC)glewGetProcAddress((const GLubyte*)"glGetVertexAttribPointervARB")) == null) || r;
  r = ((glGetVertexAttribdvARB = (PFNGLGETVERTEXATTRIBDVARBPROC)glewGetProcAddress((const GLubyte*)"glGetVertexAttribdvARB")) == null) || r;
  r = ((glGetVertexAttribfvARB = (PFNGLGETVERTEXATTRIBFVARBPROC)glewGetProcAddress((const GLubyte*)"glGetVertexAttribfvARB")) == null) || r;
  r = ((glGetVertexAttribivARB = (PFNGLGETVERTEXATTRIBIVARBPROC)glewGetProcAddress((const GLubyte*)"glGetVertexAttribivARB")) == null) || r;
  r = ((glIsProgramARB = (PFNGLISPROGRAMARBPROC)glewGetProcAddress((const GLubyte*)"glIsProgramARB")) == null) || r;
  r = ((glProgramEnvParameter4dARB = (PFNGLPROGRAMENVPARAMETER4DARBPROC)glewGetProcAddress((const GLubyte*)"glProgramEnvParameter4dARB")) == null) || r;
  r = ((glProgramEnvParameter4dvARB = (PFNGLPROGRAMENVPARAMETER4DVARBPROC)glewGetProcAddress((const GLubyte*)"glProgramEnvParameter4dvARB")) == null) || r;
  r = ((glProgramEnvParameter4fARB = (PFNGLPROGRAMENVPARAMETER4FARBPROC)glewGetProcAddress((const GLubyte*)"glProgramEnvParameter4fARB")) == null) || r;
  r = ((glProgramEnvParameter4fvARB = (PFNGLPROGRAMENVPARAMETER4FVARBPROC)glewGetProcAddress((const GLubyte*)"glProgramEnvParameter4fvARB")) == null) || r;
  r = ((glProgramLocalParameter4dARB = (PFNGLPROGRAMLOCALPARAMETER4DARBPROC)glewGetProcAddress((const GLubyte*)"glProgramLocalParameter4dARB")) == null) || r;
  r = ((glProgramLocalParameter4dvARB = (PFNGLPROGRAMLOCALPARAMETER4DVARBPROC)glewGetProcAddress((const GLubyte*)"glProgramLocalParameter4dvARB")) == null) || r;
  r = ((glProgramLocalParameter4fARB = (PFNGLPROGRAMLOCALPARAMETER4FARBPROC)glewGetProcAddress((const GLubyte*)"glProgramLocalParameter4fARB")) == null) || r;
  r = ((glProgramLocalParameter4fvARB = (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)glewGetProcAddress((const GLubyte*)"glProgramLocalParameter4fvARB")) == null) || r;
  r = ((glProgramStringARB = (PFNGLPROGRAMSTRINGARBPROC)glewGetProcAddress((const GLubyte*)"glProgramStringARB")) == null) || r;
  r = ((glVertexAttrib1dARB = (PFNGLVERTEXATTRIB1DARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib1dARB")) == null) || r;
  r = ((glVertexAttrib1dvARB = (PFNGLVERTEXATTRIB1DVARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib1dvARB")) == null) || r;
  r = ((glVertexAttrib1fARB = (PFNGLVERTEXATTRIB1FARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib1fARB")) == null) || r;
  r = ((glVertexAttrib1fvARB = (PFNGLVERTEXATTRIB1FVARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib1fvARB")) == null) || r;
  r = ((glVertexAttrib1sARB = (PFNGLVERTEXATTRIB1SARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib1sARB")) == null) || r;
  r = ((glVertexAttrib1svARB = (PFNGLVERTEXATTRIB1SVARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib1svARB")) == null) || r;
  r = ((glVertexAttrib2dARB = (PFNGLVERTEXATTRIB2DARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib2dARB")) == null) || r;
  r = ((glVertexAttrib2dvARB = (PFNGLVERTEXATTRIB2DVARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib2dvARB")) == null) || r;
  r = ((glVertexAttrib2fARB = (PFNGLVERTEXATTRIB2FARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib2fARB")) == null) || r;
  r = ((glVertexAttrib2fvARB = (PFNGLVERTEXATTRIB2FVARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib2fvARB")) == null) || r;
  r = ((glVertexAttrib2sARB = (PFNGLVERTEXATTRIB2SARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib2sARB")) == null) || r;
  r = ((glVertexAttrib2svARB = (PFNGLVERTEXATTRIB2SVARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib2svARB")) == null) || r;
  r = ((glVertexAttrib3dARB = (PFNGLVERTEXATTRIB3DARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib3dARB")) == null) || r;
  r = ((glVertexAttrib3dvARB = (PFNGLVERTEXATTRIB3DVARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib3dvARB")) == null) || r;
  r = ((glVertexAttrib3fARB = (PFNGLVERTEXATTRIB3FARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib3fARB")) == null) || r;
  r = ((glVertexAttrib3fvARB = (PFNGLVERTEXATTRIB3FVARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib3fvARB")) == null) || r;
  r = ((glVertexAttrib3sARB = (PFNGLVERTEXATTRIB3SARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib3sARB")) == null) || r;
  r = ((glVertexAttrib3svARB = (PFNGLVERTEXATTRIB3SVARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib3svARB")) == null) || r;
  r = ((glVertexAttrib4NbvARB = (PFNGLVERTEXATTRIB4NBVARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4NbvARB")) == null) || r;
  r = ((glVertexAttrib4NivARB = (PFNGLVERTEXATTRIB4NIVARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4NivARB")) == null) || r;
  r = ((glVertexAttrib4NsvARB = (PFNGLVERTEXATTRIB4NSVARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4NsvARB")) == null) || r;
  r = ((glVertexAttrib4NubARB = (PFNGLVERTEXATTRIB4NUBARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4NubARB")) == null) || r;
  r = ((glVertexAttrib4NubvARB = (PFNGLVERTEXATTRIB4NUBVARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4NubvARB")) == null) || r;
  r = ((glVertexAttrib4NuivARB = (PFNGLVERTEXATTRIB4NUIVARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4NuivARB")) == null) || r;
  r = ((glVertexAttrib4NusvARB = (PFNGLVERTEXATTRIB4NUSVARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4NusvARB")) == null) || r;
  r = ((glVertexAttrib4bvARB = (PFNGLVERTEXATTRIB4BVARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4bvARB")) == null) || r;
  r = ((glVertexAttrib4dARB = (PFNGLVERTEXATTRIB4DARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4dARB")) == null) || r;
  r = ((glVertexAttrib4dvARB = (PFNGLVERTEXATTRIB4DVARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4dvARB")) == null) || r;
  r = ((glVertexAttrib4fARB = (PFNGLVERTEXATTRIB4FARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4fARB")) == null) || r;
  r = ((glVertexAttrib4fvARB = (PFNGLVERTEXATTRIB4FVARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4fvARB")) == null) || r;
  r = ((glVertexAttrib4ivARB = (PFNGLVERTEXATTRIB4IVARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4ivARB")) == null) || r;
  r = ((glVertexAttrib4sARB = (PFNGLVERTEXATTRIB4SARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4sARB")) == null) || r;
  r = ((glVertexAttrib4svARB = (PFNGLVERTEXATTRIB4SVARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4svARB")) == null) || r;
  r = ((glVertexAttrib4ubvARB = (PFNGLVERTEXATTRIB4UBVARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4ubvARB")) == null) || r;
  r = ((glVertexAttrib4uivARB = (PFNGLVERTEXATTRIB4UIVARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4uivARB")) == null) || r;
  r = ((glVertexAttrib4usvARB = (PFNGLVERTEXATTRIB4USVARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4usvARB")) == null) || r;
  r = ((glVertexAttribPointerARB = (PFNGLVERTEXATTRIBPOINTERARBPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribPointerARB")) == null) || r;

  return r;
}

#endif /* GL_ARB_vertex_program */

#ifdef GL_ARB_vertex_shader

static GLboolean _glewInit_GL_ARB_vertex_shader (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBindAttribLocationARB = (PFNGLBINDATTRIBLOCATIONARBPROC)glewGetProcAddress((const GLubyte*)"glBindAttribLocationARB")) == null) || r;
  r = ((glGetActiveAttribARB = (PFNGLGETACTIVEATTRIBARBPROC)glewGetProcAddress((const GLubyte*)"glGetActiveAttribARB")) == null) || r;
  r = ((glGetAttribLocationARB = (PFNGLGETATTRIBLOCATIONARBPROC)glewGetProcAddress((const GLubyte*)"glGetAttribLocationARB")) == null) || r;

  return r;
}

#endif /* GL_ARB_vertex_shader */

#ifdef GL_ARB_window_pos

static GLboolean _glewInit_GL_ARB_window_pos (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glWindowPos2dARB = (PFNGLWINDOWPOS2DARBPROC)glewGetProcAddress((const GLubyte*)"glWindowPos2dARB")) == null) || r;
  r = ((glWindowPos2dvARB = (PFNGLWINDOWPOS2DVARBPROC)glewGetProcAddress((const GLubyte*)"glWindowPos2dvARB")) == null) || r;
  r = ((glWindowPos2fARB = (PFNGLWINDOWPOS2FARBPROC)glewGetProcAddress((const GLubyte*)"glWindowPos2fARB")) == null) || r;
  r = ((glWindowPos2fvARB = (PFNGLWINDOWPOS2FVARBPROC)glewGetProcAddress((const GLubyte*)"glWindowPos2fvARB")) == null) || r;
  r = ((glWindowPos2iARB = (PFNGLWINDOWPOS2IARBPROC)glewGetProcAddress((const GLubyte*)"glWindowPos2iARB")) == null) || r;
  r = ((glWindowPos2ivARB = (PFNGLWINDOWPOS2IVARBPROC)glewGetProcAddress((const GLubyte*)"glWindowPos2ivARB")) == null) || r;
  r = ((glWindowPos2sARB = (PFNGLWINDOWPOS2SARBPROC)glewGetProcAddress((const GLubyte*)"glWindowPos2sARB")) == null) || r;
  r = ((glWindowPos2svARB = (PFNGLWINDOWPOS2SVARBPROC)glewGetProcAddress((const GLubyte*)"glWindowPos2svARB")) == null) || r;
  r = ((glWindowPos3dARB = (PFNGLWINDOWPOS3DARBPROC)glewGetProcAddress((const GLubyte*)"glWindowPos3dARB")) == null) || r;
  r = ((glWindowPos3dvARB = (PFNGLWINDOWPOS3DVARBPROC)glewGetProcAddress((const GLubyte*)"glWindowPos3dvARB")) == null) || r;
  r = ((glWindowPos3fARB = (PFNGLWINDOWPOS3FARBPROC)glewGetProcAddress((const GLubyte*)"glWindowPos3fARB")) == null) || r;
  r = ((glWindowPos3fvARB = (PFNGLWINDOWPOS3FVARBPROC)glewGetProcAddress((const GLubyte*)"glWindowPos3fvARB")) == null) || r;
  r = ((glWindowPos3iARB = (PFNGLWINDOWPOS3IARBPROC)glewGetProcAddress((const GLubyte*)"glWindowPos3iARB")) == null) || r;
  r = ((glWindowPos3ivARB = (PFNGLWINDOWPOS3IVARBPROC)glewGetProcAddress((const GLubyte*)"glWindowPos3ivARB")) == null) || r;
  r = ((glWindowPos3sARB = (PFNGLWINDOWPOS3SARBPROC)glewGetProcAddress((const GLubyte*)"glWindowPos3sARB")) == null) || r;
  r = ((glWindowPos3svARB = (PFNGLWINDOWPOS3SVARBPROC)glewGetProcAddress((const GLubyte*)"glWindowPos3svARB")) == null) || r;

  return r;
}

#endif /* GL_ARB_window_pos */

#ifdef GL_ATIX_point_sprites

#endif /* GL_ATIX_point_sprites */

#ifdef GL_ATIX_texture_env_combine3

#endif /* GL_ATIX_texture_env_combine3 */

#ifdef GL_ATIX_texture_env_route

#endif /* GL_ATIX_texture_env_route */

#ifdef GL_ATIX_vertex_shader_output_point_size

#endif /* GL_ATIX_vertex_shader_output_point_size */

#ifdef GL_ATI_draw_buffers

static GLboolean _glewInit_GL_ATI_draw_buffers (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glDrawBuffersATI = (PFNGLDRAWBUFFERSATIPROC)glewGetProcAddress((const GLubyte*)"glDrawBuffersATI")) == null) || r;

  return r;
}

#endif /* GL_ATI_draw_buffers */

#ifdef GL_ATI_element_array

static GLboolean _glewInit_GL_ATI_element_array (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glDrawElementArrayATI = (PFNGLDRAWELEMENTARRAYATIPROC)glewGetProcAddress((const GLubyte*)"glDrawElementArrayATI")) == null) || r;
  r = ((glDrawRangeElementArrayATI = (PFNGLDRAWRANGEELEMENTARRAYATIPROC)glewGetProcAddress((const GLubyte*)"glDrawRangeElementArrayATI")) == null) || r;
  r = ((glElementPointerATI = (PFNGLELEMENTPOINTERATIPROC)glewGetProcAddress((const GLubyte*)"glElementPointerATI")) == null) || r;

  return r;
}

#endif /* GL_ATI_element_array */

#ifdef GL_ATI_envmap_bumpmap

static GLboolean _glewInit_GL_ATI_envmap_bumpmap (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glGetTexBumpParameterfvATI = (PFNGLGETTEXBUMPPARAMETERFVATIPROC)glewGetProcAddress((const GLubyte*)"glGetTexBumpParameterfvATI")) == null) || r;
  r = ((glGetTexBumpParameterivATI = (PFNGLGETTEXBUMPPARAMETERIVATIPROC)glewGetProcAddress((const GLubyte*)"glGetTexBumpParameterivATI")) == null) || r;
  r = ((glTexBumpParameterfvATI = (PFNGLTEXBUMPPARAMETERFVATIPROC)glewGetProcAddress((const GLubyte*)"glTexBumpParameterfvATI")) == null) || r;
  r = ((glTexBumpParameterivATI = (PFNGLTEXBUMPPARAMETERIVATIPROC)glewGetProcAddress((const GLubyte*)"glTexBumpParameterivATI")) == null) || r;

  return r;
}

#endif /* GL_ATI_envmap_bumpmap */

#ifdef GL_ATI_fragment_shader

static GLboolean _glewInit_GL_ATI_fragment_shader (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glAlphaFragmentOp1ATI = (PFNGLALPHAFRAGMENTOP1ATIPROC)glewGetProcAddress((const GLubyte*)"glAlphaFragmentOp1ATI")) == null) || r;
  r = ((glAlphaFragmentOp2ATI = (PFNGLALPHAFRAGMENTOP2ATIPROC)glewGetProcAddress((const GLubyte*)"glAlphaFragmentOp2ATI")) == null) || r;
  r = ((glAlphaFragmentOp3ATI = (PFNGLALPHAFRAGMENTOP3ATIPROC)glewGetProcAddress((const GLubyte*)"glAlphaFragmentOp3ATI")) == null) || r;
  r = ((glBeginFragmentShaderATI = (PFNGLBEGINFRAGMENTSHADERATIPROC)glewGetProcAddress((const GLubyte*)"glBeginFragmentShaderATI")) == null) || r;
  r = ((glBindFragmentShaderATI = (PFNGLBINDFRAGMENTSHADERATIPROC)glewGetProcAddress((const GLubyte*)"glBindFragmentShaderATI")) == null) || r;
  r = ((glColorFragmentOp1ATI = (PFNGLCOLORFRAGMENTOP1ATIPROC)glewGetProcAddress((const GLubyte*)"glColorFragmentOp1ATI")) == null) || r;
  r = ((glColorFragmentOp2ATI = (PFNGLCOLORFRAGMENTOP2ATIPROC)glewGetProcAddress((const GLubyte*)"glColorFragmentOp2ATI")) == null) || r;
  r = ((glColorFragmentOp3ATI = (PFNGLCOLORFRAGMENTOP3ATIPROC)glewGetProcAddress((const GLubyte*)"glColorFragmentOp3ATI")) == null) || r;
  r = ((glDeleteFragmentShaderATI = (PFNGLDELETEFRAGMENTSHADERATIPROC)glewGetProcAddress((const GLubyte*)"glDeleteFragmentShaderATI")) == null) || r;
  r = ((glEndFragmentShaderATI = (PFNGLENDFRAGMENTSHADERATIPROC)glewGetProcAddress((const GLubyte*)"glEndFragmentShaderATI")) == null) || r;
  r = ((glGenFragmentShadersATI = (PFNGLGENFRAGMENTSHADERSATIPROC)glewGetProcAddress((const GLubyte*)"glGenFragmentShadersATI")) == null) || r;
  r = ((glPassTexCoordATI = (PFNGLPASSTEXCOORDATIPROC)glewGetProcAddress((const GLubyte*)"glPassTexCoordATI")) == null) || r;
  r = ((glSampleMapATI = (PFNGLSAMPLEMAPATIPROC)glewGetProcAddress((const GLubyte*)"glSampleMapATI")) == null) || r;
  r = ((glSetFragmentShaderConstantATI = (PFNGLSETFRAGMENTSHADERCONSTANTATIPROC)glewGetProcAddress((const GLubyte*)"glSetFragmentShaderConstantATI")) == null) || r;

  return r;
}

#endif /* GL_ATI_fragment_shader */

#ifdef GL_ATI_map_object_buffer

static GLboolean _glewInit_GL_ATI_map_object_buffer (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glMapObjectBufferATI = (PFNGLMAPOBJECTBUFFERATIPROC)glewGetProcAddress((const GLubyte*)"glMapObjectBufferATI")) == null) || r;
  r = ((glUnmapObjectBufferATI = (PFNGLUNMAPOBJECTBUFFERATIPROC)glewGetProcAddress((const GLubyte*)"glUnmapObjectBufferATI")) == null) || r;

  return r;
}

#endif /* GL_ATI_map_object_buffer */

#ifdef GL_ATI_meminfo

#endif /* GL_ATI_meminfo */

#ifdef GL_ATI_pn_triangles

static GLboolean _glewInit_GL_ATI_pn_triangles (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glPNTrianglesfATI = (PFNGLPNTRIANGLESFATIPROC)glewGetProcAddress((const GLubyte*)"glPNTrianglesfATI")) == null) || r;
  r = ((glPNTrianglesiATI = (PFNGLPNTRIANGLESIATIPROC)glewGetProcAddress((const GLubyte*)"glPNTrianglesiATI")) == null) || r;

  return r;
}

#endif /* GL_ATI_pn_triangles */

#ifdef GL_ATI_separate_stencil

static GLboolean _glewInit_GL_ATI_separate_stencil (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glStencilFuncSeparateATI = (PFNGLSTENCILFUNCSEPARATEATIPROC)glewGetProcAddress((const GLubyte*)"glStencilFuncSeparateATI")) == null) || r;
  r = ((glStencilOpSeparateATI = (PFNGLSTENCILOPSEPARATEATIPROC)glewGetProcAddress((const GLubyte*)"glStencilOpSeparateATI")) == null) || r;

  return r;
}

#endif /* GL_ATI_separate_stencil */

#ifdef GL_ATI_shader_texture_lod

#endif /* GL_ATI_shader_texture_lod */

#ifdef GL_ATI_text_fragment_shader

#endif /* GL_ATI_text_fragment_shader */

#ifdef GL_ATI_texture_compression_3dc

#endif /* GL_ATI_texture_compression_3dc */

#ifdef GL_ATI_texture_env_combine3

#endif /* GL_ATI_texture_env_combine3 */

#ifdef GL_ATI_texture_float

#endif /* GL_ATI_texture_float */

#ifdef GL_ATI_texture_mirror_once

#endif /* GL_ATI_texture_mirror_once */

#ifdef GL_ATI_vertex_array_object

static GLboolean _glewInit_GL_ATI_vertex_array_object (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glArrayObjectATI = (PFNGLARRAYOBJECTATIPROC)glewGetProcAddress((const GLubyte*)"glArrayObjectATI")) == null) || r;
  r = ((glFreeObjectBufferATI = (PFNGLFREEOBJECTBUFFERATIPROC)glewGetProcAddress((const GLubyte*)"glFreeObjectBufferATI")) == null) || r;
  r = ((glGetArrayObjectfvATI = (PFNGLGETARRAYOBJECTFVATIPROC)glewGetProcAddress((const GLubyte*)"glGetArrayObjectfvATI")) == null) || r;
  r = ((glGetArrayObjectivATI = (PFNGLGETARRAYOBJECTIVATIPROC)glewGetProcAddress((const GLubyte*)"glGetArrayObjectivATI")) == null) || r;
  r = ((glGetObjectBufferfvATI = (PFNGLGETOBJECTBUFFERFVATIPROC)glewGetProcAddress((const GLubyte*)"glGetObjectBufferfvATI")) == null) || r;
  r = ((glGetObjectBufferivATI = (PFNGLGETOBJECTBUFFERIVATIPROC)glewGetProcAddress((const GLubyte*)"glGetObjectBufferivATI")) == null) || r;
  r = ((glGetVariantArrayObjectfvATI = (PFNGLGETVARIANTARRAYOBJECTFVATIPROC)glewGetProcAddress((const GLubyte*)"glGetVariantArrayObjectfvATI")) == null) || r;
  r = ((glGetVariantArrayObjectivATI = (PFNGLGETVARIANTARRAYOBJECTIVATIPROC)glewGetProcAddress((const GLubyte*)"glGetVariantArrayObjectivATI")) == null) || r;
  r = ((glIsObjectBufferATI = (PFNGLISOBJECTBUFFERATIPROC)glewGetProcAddress((const GLubyte*)"glIsObjectBufferATI")) == null) || r;
  r = ((glNewObjectBufferATI = (PFNGLNEWOBJECTBUFFERATIPROC)glewGetProcAddress((const GLubyte*)"glNewObjectBufferATI")) == null) || r;
  r = ((glUpdateObjectBufferATI = (PFNGLUPDATEOBJECTBUFFERATIPROC)glewGetProcAddress((const GLubyte*)"glUpdateObjectBufferATI")) == null) || r;
  r = ((glVariantArrayObjectATI = (PFNGLVARIANTARRAYOBJECTATIPROC)glewGetProcAddress((const GLubyte*)"glVariantArrayObjectATI")) == null) || r;

  return r;
}

#endif /* GL_ATI_vertex_array_object */

#ifdef GL_ATI_vertex_attrib_array_object

static GLboolean _glewInit_GL_ATI_vertex_attrib_array_object (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glGetVertexAttribArrayObjectfvATI = (PFNGLGETVERTEXATTRIBARRAYOBJECTFVATIPROC)glewGetProcAddress((const GLubyte*)"glGetVertexAttribArrayObjectfvATI")) == null) || r;
  r = ((glGetVertexAttribArrayObjectivATI = (PFNGLGETVERTEXATTRIBARRAYOBJECTIVATIPROC)glewGetProcAddress((const GLubyte*)"glGetVertexAttribArrayObjectivATI")) == null) || r;
  r = ((glVertexAttribArrayObjectATI = (PFNGLVERTEXATTRIBARRAYOBJECTATIPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribArrayObjectATI")) == null) || r;

  return r;
}

#endif /* GL_ATI_vertex_attrib_array_object */

#ifdef GL_ATI_vertex_streams

static GLboolean _glewInit_GL_ATI_vertex_streams (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glClientActiveVertexStreamATI = (PFNGLCLIENTACTIVEVERTEXSTREAMATIPROC)glewGetProcAddress((const GLubyte*)"glClientActiveVertexStreamATI")) == null) || r;
  r = ((glNormalStream3bATI = (PFNGLNORMALSTREAM3BATIPROC)glewGetProcAddress((const GLubyte*)"glNormalStream3bATI")) == null) || r;
  r = ((glNormalStream3bvATI = (PFNGLNORMALSTREAM3BVATIPROC)glewGetProcAddress((const GLubyte*)"glNormalStream3bvATI")) == null) || r;
  r = ((glNormalStream3dATI = (PFNGLNORMALSTREAM3DATIPROC)glewGetProcAddress((const GLubyte*)"glNormalStream3dATI")) == null) || r;
  r = ((glNormalStream3dvATI = (PFNGLNORMALSTREAM3DVATIPROC)glewGetProcAddress((const GLubyte*)"glNormalStream3dvATI")) == null) || r;
  r = ((glNormalStream3fATI = (PFNGLNORMALSTREAM3FATIPROC)glewGetProcAddress((const GLubyte*)"glNormalStream3fATI")) == null) || r;
  r = ((glNormalStream3fvATI = (PFNGLNORMALSTREAM3FVATIPROC)glewGetProcAddress((const GLubyte*)"glNormalStream3fvATI")) == null) || r;
  r = ((glNormalStream3iATI = (PFNGLNORMALSTREAM3IATIPROC)glewGetProcAddress((const GLubyte*)"glNormalStream3iATI")) == null) || r;
  r = ((glNormalStream3ivATI = (PFNGLNORMALSTREAM3IVATIPROC)glewGetProcAddress((const GLubyte*)"glNormalStream3ivATI")) == null) || r;
  r = ((glNormalStream3sATI = (PFNGLNORMALSTREAM3SATIPROC)glewGetProcAddress((const GLubyte*)"glNormalStream3sATI")) == null) || r;
  r = ((glNormalStream3svATI = (PFNGLNORMALSTREAM3SVATIPROC)glewGetProcAddress((const GLubyte*)"glNormalStream3svATI")) == null) || r;
  r = ((glVertexBlendEnvfATI = (PFNGLVERTEXBLENDENVFATIPROC)glewGetProcAddress((const GLubyte*)"glVertexBlendEnvfATI")) == null) || r;
  r = ((glVertexBlendEnviATI = (PFNGLVERTEXBLENDENVIATIPROC)glewGetProcAddress((const GLubyte*)"glVertexBlendEnviATI")) == null) || r;
  r = ((glVertexStream2dATI = (PFNGLVERTEXSTREAM2DATIPROC)glewGetProcAddress((const GLubyte*)"glVertexStream2dATI")) == null) || r;
  r = ((glVertexStream2dvATI = (PFNGLVERTEXSTREAM2DVATIPROC)glewGetProcAddress((const GLubyte*)"glVertexStream2dvATI")) == null) || r;
  r = ((glVertexStream2fATI = (PFNGLVERTEXSTREAM2FATIPROC)glewGetProcAddress((const GLubyte*)"glVertexStream2fATI")) == null) || r;
  r = ((glVertexStream2fvATI = (PFNGLVERTEXSTREAM2FVATIPROC)glewGetProcAddress((const GLubyte*)"glVertexStream2fvATI")) == null) || r;
  r = ((glVertexStream2iATI = (PFNGLVERTEXSTREAM2IATIPROC)glewGetProcAddress((const GLubyte*)"glVertexStream2iATI")) == null) || r;
  r = ((glVertexStream2ivATI = (PFNGLVERTEXSTREAM2IVATIPROC)glewGetProcAddress((const GLubyte*)"glVertexStream2ivATI")) == null) || r;
  r = ((glVertexStream2sATI = (PFNGLVERTEXSTREAM2SATIPROC)glewGetProcAddress((const GLubyte*)"glVertexStream2sATI")) == null) || r;
  r = ((glVertexStream2svATI = (PFNGLVERTEXSTREAM2SVATIPROC)glewGetProcAddress((const GLubyte*)"glVertexStream2svATI")) == null) || r;
  r = ((glVertexStream3dATI = (PFNGLVERTEXSTREAM3DATIPROC)glewGetProcAddress((const GLubyte*)"glVertexStream3dATI")) == null) || r;
  r = ((glVertexStream3dvATI = (PFNGLVERTEXSTREAM3DVATIPROC)glewGetProcAddress((const GLubyte*)"glVertexStream3dvATI")) == null) || r;
  r = ((glVertexStream3fATI = (PFNGLVERTEXSTREAM3FATIPROC)glewGetProcAddress((const GLubyte*)"glVertexStream3fATI")) == null) || r;
  r = ((glVertexStream3fvATI = (PFNGLVERTEXSTREAM3FVATIPROC)glewGetProcAddress((const GLubyte*)"glVertexStream3fvATI")) == null) || r;
  r = ((glVertexStream3iATI = (PFNGLVERTEXSTREAM3IATIPROC)glewGetProcAddress((const GLubyte*)"glVertexStream3iATI")) == null) || r;
  r = ((glVertexStream3ivATI = (PFNGLVERTEXSTREAM3IVATIPROC)glewGetProcAddress((const GLubyte*)"glVertexStream3ivATI")) == null) || r;
  r = ((glVertexStream3sATI = (PFNGLVERTEXSTREAM3SATIPROC)glewGetProcAddress((const GLubyte*)"glVertexStream3sATI")) == null) || r;
  r = ((glVertexStream3svATI = (PFNGLVERTEXSTREAM3SVATIPROC)glewGetProcAddress((const GLubyte*)"glVertexStream3svATI")) == null) || r;
  r = ((glVertexStream4dATI = (PFNGLVERTEXSTREAM4DATIPROC)glewGetProcAddress((const GLubyte*)"glVertexStream4dATI")) == null) || r;
  r = ((glVertexStream4dvATI = (PFNGLVERTEXSTREAM4DVATIPROC)glewGetProcAddress((const GLubyte*)"glVertexStream4dvATI")) == null) || r;
  r = ((glVertexStream4fATI = (PFNGLVERTEXSTREAM4FATIPROC)glewGetProcAddress((const GLubyte*)"glVertexStream4fATI")) == null) || r;
  r = ((glVertexStream4fvATI = (PFNGLVERTEXSTREAM4FVATIPROC)glewGetProcAddress((const GLubyte*)"glVertexStream4fvATI")) == null) || r;
  r = ((glVertexStream4iATI = (PFNGLVERTEXSTREAM4IATIPROC)glewGetProcAddress((const GLubyte*)"glVertexStream4iATI")) == null) || r;
  r = ((glVertexStream4ivATI = (PFNGLVERTEXSTREAM4IVATIPROC)glewGetProcAddress((const GLubyte*)"glVertexStream4ivATI")) == null) || r;
  r = ((glVertexStream4sATI = (PFNGLVERTEXSTREAM4SATIPROC)glewGetProcAddress((const GLubyte*)"glVertexStream4sATI")) == null) || r;
  r = ((glVertexStream4svATI = (PFNGLVERTEXSTREAM4SVATIPROC)glewGetProcAddress((const GLubyte*)"glVertexStream4svATI")) == null) || r;

  return r;
}

#endif /* GL_ATI_vertex_streams */

#ifdef GL_EXT_422_pixels

#endif /* GL_EXT_422_pixels */

#ifdef GL_EXT_Cg_shader

#endif /* GL_EXT_Cg_shader */

#ifdef GL_EXT_abgr

#endif /* GL_EXT_abgr */

#ifdef GL_EXT_bgra

#endif /* GL_EXT_bgra */

#ifdef GL_EXT_bindable_uniform

static GLboolean _glewInit_GL_EXT_bindable_uniform (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glGetUniformBufferSizeEXT = (PFNGLGETUNIFORMBUFFERSIZEEXTPROC)glewGetProcAddress((const GLubyte*)"glGetUniformBufferSizeEXT")) == null) || r;
  r = ((glGetUniformOffsetEXT = (PFNGLGETUNIFORMOFFSETEXTPROC)glewGetProcAddress((const GLubyte*)"glGetUniformOffsetEXT")) == null) || r;
  r = ((glUniformBufferEXT = (PFNGLUNIFORMBUFFEREXTPROC)glewGetProcAddress((const GLubyte*)"glUniformBufferEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_bindable_uniform */

#ifdef GL_EXT_blend_color

static GLboolean _glewInit_GL_EXT_blend_color (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBlendColorEXT = (PFNGLBLENDCOLOREXTPROC)glewGetProcAddress((const GLubyte*)"glBlendColorEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_blend_color */

#ifdef GL_EXT_blend_equation_separate

static GLboolean _glewInit_GL_EXT_blend_equation_separate (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBlendEquationSeparateEXT = (PFNGLBLENDEQUATIONSEPARATEEXTPROC)glewGetProcAddress((const GLubyte*)"glBlendEquationSeparateEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_blend_equation_separate */

#ifdef GL_EXT_blend_func_separate

static GLboolean _glewInit_GL_EXT_blend_func_separate (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBlendFuncSeparateEXT = (PFNGLBLENDFUNCSEPARATEEXTPROC)glewGetProcAddress((const GLubyte*)"glBlendFuncSeparateEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_blend_func_separate */

#ifdef GL_EXT_blend_logic_op

#endif /* GL_EXT_blend_logic_op */

#ifdef GL_EXT_blend_minmax

static GLboolean _glewInit_GL_EXT_blend_minmax (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBlendEquationEXT = (PFNGLBLENDEQUATIONEXTPROC)glewGetProcAddress((const GLubyte*)"glBlendEquationEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_blend_minmax */

#ifdef GL_EXT_blend_subtract

#endif /* GL_EXT_blend_subtract */

#ifdef GL_EXT_clip_volume_hint

#endif /* GL_EXT_clip_volume_hint */

#ifdef GL_EXT_cmyka

#endif /* GL_EXT_cmyka */

#ifdef GL_EXT_color_subtable

static GLboolean _glewInit_GL_EXT_color_subtable (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glColorSubTableEXT = (PFNGLCOLORSUBTABLEEXTPROC)glewGetProcAddress((const GLubyte*)"glColorSubTableEXT")) == null) || r;
  r = ((glCopyColorSubTableEXT = (PFNGLCOPYCOLORSUBTABLEEXTPROC)glewGetProcAddress((const GLubyte*)"glCopyColorSubTableEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_color_subtable */

#ifdef GL_EXT_compiled_vertex_array

static GLboolean _glewInit_GL_EXT_compiled_vertex_array (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glLockArraysEXT = (PFNGLLOCKARRAYSEXTPROC)glewGetProcAddress((const GLubyte*)"glLockArraysEXT")) == null) || r;
  r = ((glUnlockArraysEXT = (PFNGLUNLOCKARRAYSEXTPROC)glewGetProcAddress((const GLubyte*)"glUnlockArraysEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_compiled_vertex_array */

#ifdef GL_EXT_convolution

static GLboolean _glewInit_GL_EXT_convolution (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glConvolutionFilter1DEXT = (PFNGLCONVOLUTIONFILTER1DEXTPROC)glewGetProcAddress((const GLubyte*)"glConvolutionFilter1DEXT")) == null) || r;
  r = ((glConvolutionFilter2DEXT = (PFNGLCONVOLUTIONFILTER2DEXTPROC)glewGetProcAddress((const GLubyte*)"glConvolutionFilter2DEXT")) == null) || r;
  r = ((glConvolutionParameterfEXT = (PFNGLCONVOLUTIONPARAMETERFEXTPROC)glewGetProcAddress((const GLubyte*)"glConvolutionParameterfEXT")) == null) || r;
  r = ((glConvolutionParameterfvEXT = (PFNGLCONVOLUTIONPARAMETERFVEXTPROC)glewGetProcAddress((const GLubyte*)"glConvolutionParameterfvEXT")) == null) || r;
  r = ((glConvolutionParameteriEXT = (PFNGLCONVOLUTIONPARAMETERIEXTPROC)glewGetProcAddress((const GLubyte*)"glConvolutionParameteriEXT")) == null) || r;
  r = ((glConvolutionParameterivEXT = (PFNGLCONVOLUTIONPARAMETERIVEXTPROC)glewGetProcAddress((const GLubyte*)"glConvolutionParameterivEXT")) == null) || r;
  r = ((glCopyConvolutionFilter1DEXT = (PFNGLCOPYCONVOLUTIONFILTER1DEXTPROC)glewGetProcAddress((const GLubyte*)"glCopyConvolutionFilter1DEXT")) == null) || r;
  r = ((glCopyConvolutionFilter2DEXT = (PFNGLCOPYCONVOLUTIONFILTER2DEXTPROC)glewGetProcAddress((const GLubyte*)"glCopyConvolutionFilter2DEXT")) == null) || r;
  r = ((glGetConvolutionFilterEXT = (PFNGLGETCONVOLUTIONFILTEREXTPROC)glewGetProcAddress((const GLubyte*)"glGetConvolutionFilterEXT")) == null) || r;
  r = ((glGetConvolutionParameterfvEXT = (PFNGLGETCONVOLUTIONPARAMETERFVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetConvolutionParameterfvEXT")) == null) || r;
  r = ((glGetConvolutionParameterivEXT = (PFNGLGETCONVOLUTIONPARAMETERIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetConvolutionParameterivEXT")) == null) || r;
  r = ((glGetSeparableFilterEXT = (PFNGLGETSEPARABLEFILTEREXTPROC)glewGetProcAddress((const GLubyte*)"glGetSeparableFilterEXT")) == null) || r;
  r = ((glSeparableFilter2DEXT = (PFNGLSEPARABLEFILTER2DEXTPROC)glewGetProcAddress((const GLubyte*)"glSeparableFilter2DEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_convolution */

#ifdef GL_EXT_coordinate_frame

static GLboolean _glewInit_GL_EXT_coordinate_frame (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBinormalPointerEXT = (PFNGLBINORMALPOINTEREXTPROC)glewGetProcAddress((const GLubyte*)"glBinormalPointerEXT")) == null) || r;
  r = ((glTangentPointerEXT = (PFNGLTANGENTPOINTEREXTPROC)glewGetProcAddress((const GLubyte*)"glTangentPointerEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_coordinate_frame */

#ifdef GL_EXT_copy_texture

static GLboolean _glewInit_GL_EXT_copy_texture (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glCopyTexImage1DEXT = (PFNGLCOPYTEXIMAGE1DEXTPROC)glewGetProcAddress((const GLubyte*)"glCopyTexImage1DEXT")) == null) || r;
  r = ((glCopyTexImage2DEXT = (PFNGLCOPYTEXIMAGE2DEXTPROC)glewGetProcAddress((const GLubyte*)"glCopyTexImage2DEXT")) == null) || r;
  r = ((glCopyTexSubImage1DEXT = (PFNGLCOPYTEXSUBIMAGE1DEXTPROC)glewGetProcAddress((const GLubyte*)"glCopyTexSubImage1DEXT")) == null) || r;
  r = ((glCopyTexSubImage2DEXT = (PFNGLCOPYTEXSUBIMAGE2DEXTPROC)glewGetProcAddress((const GLubyte*)"glCopyTexSubImage2DEXT")) == null) || r;
  r = ((glCopyTexSubImage3DEXT = (PFNGLCOPYTEXSUBIMAGE3DEXTPROC)glewGetProcAddress((const GLubyte*)"glCopyTexSubImage3DEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_copy_texture */

#ifdef GL_EXT_cull_vertex

static GLboolean _glewInit_GL_EXT_cull_vertex (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glCullParameterdvEXT = (PFNGLCULLPARAMETERDVEXTPROC)glewGetProcAddress((const GLubyte*)"glCullParameterdvEXT")) == null) || r;
  r = ((glCullParameterfvEXT = (PFNGLCULLPARAMETERFVEXTPROC)glewGetProcAddress((const GLubyte*)"glCullParameterfvEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_cull_vertex */

#ifdef GL_EXT_depth_bounds_test

static GLboolean _glewInit_GL_EXT_depth_bounds_test (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glDepthBoundsEXT = (PFNGLDEPTHBOUNDSEXTPROC)glewGetProcAddress((const GLubyte*)"glDepthBoundsEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_depth_bounds_test */

#ifdef GL_EXT_direct_state_access

static GLboolean _glewInit_GL_EXT_direct_state_access (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBindMultiTextureEXT = (PFNGLBINDMULTITEXTUREEXTPROC)glewGetProcAddress((const GLubyte*)"glBindMultiTextureEXT")) == null) || r;
  r = ((glCheckNamedFramebufferStatusEXT = (PFNGLCHECKNAMEDFRAMEBUFFERSTATUSEXTPROC)glewGetProcAddress((const GLubyte*)"glCheckNamedFramebufferStatusEXT")) == null) || r;
  r = ((glClientAttribDefaultEXT = (PFNGLCLIENTATTRIBDEFAULTEXTPROC)glewGetProcAddress((const GLubyte*)"glClientAttribDefaultEXT")) == null) || r;
  r = ((glCompressedMultiTexImage1DEXT = (PFNGLCOMPRESSEDMULTITEXIMAGE1DEXTPROC)glewGetProcAddress((const GLubyte*)"glCompressedMultiTexImage1DEXT")) == null) || r;
  r = ((glCompressedMultiTexImage2DEXT = (PFNGLCOMPRESSEDMULTITEXIMAGE2DEXTPROC)glewGetProcAddress((const GLubyte*)"glCompressedMultiTexImage2DEXT")) == null) || r;
  r = ((glCompressedMultiTexImage3DEXT = (PFNGLCOMPRESSEDMULTITEXIMAGE3DEXTPROC)glewGetProcAddress((const GLubyte*)"glCompressedMultiTexImage3DEXT")) == null) || r;
  r = ((glCompressedMultiTexSubImage1DEXT = (PFNGLCOMPRESSEDMULTITEXSUBIMAGE1DEXTPROC)glewGetProcAddress((const GLubyte*)"glCompressedMultiTexSubImage1DEXT")) == null) || r;
  r = ((glCompressedMultiTexSubImage2DEXT = (PFNGLCOMPRESSEDMULTITEXSUBIMAGE2DEXTPROC)glewGetProcAddress((const GLubyte*)"glCompressedMultiTexSubImage2DEXT")) == null) || r;
  r = ((glCompressedMultiTexSubImage3DEXT = (PFNGLCOMPRESSEDMULTITEXSUBIMAGE3DEXTPROC)glewGetProcAddress((const GLubyte*)"glCompressedMultiTexSubImage3DEXT")) == null) || r;
  r = ((glCompressedTextureImage1DEXT = (PFNGLCOMPRESSEDTEXTUREIMAGE1DEXTPROC)glewGetProcAddress((const GLubyte*)"glCompressedTextureImage1DEXT")) == null) || r;
  r = ((glCompressedTextureImage2DEXT = (PFNGLCOMPRESSEDTEXTUREIMAGE2DEXTPROC)glewGetProcAddress((const GLubyte*)"glCompressedTextureImage2DEXT")) == null) || r;
  r = ((glCompressedTextureImage3DEXT = (PFNGLCOMPRESSEDTEXTUREIMAGE3DEXTPROC)glewGetProcAddress((const GLubyte*)"glCompressedTextureImage3DEXT")) == null) || r;
  r = ((glCompressedTextureSubImage1DEXT = (PFNGLCOMPRESSEDTEXTURESUBIMAGE1DEXTPROC)glewGetProcAddress((const GLubyte*)"glCompressedTextureSubImage1DEXT")) == null) || r;
  r = ((glCompressedTextureSubImage2DEXT = (PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXTPROC)glewGetProcAddress((const GLubyte*)"glCompressedTextureSubImage2DEXT")) == null) || r;
  r = ((glCompressedTextureSubImage3DEXT = (PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXTPROC)glewGetProcAddress((const GLubyte*)"glCompressedTextureSubImage3DEXT")) == null) || r;
  r = ((glCopyMultiTexImage1DEXT = (PFNGLCOPYMULTITEXIMAGE1DEXTPROC)glewGetProcAddress((const GLubyte*)"glCopyMultiTexImage1DEXT")) == null) || r;
  r = ((glCopyMultiTexImage2DEXT = (PFNGLCOPYMULTITEXIMAGE2DEXTPROC)glewGetProcAddress((const GLubyte*)"glCopyMultiTexImage2DEXT")) == null) || r;
  r = ((glCopyMultiTexSubImage1DEXT = (PFNGLCOPYMULTITEXSUBIMAGE1DEXTPROC)glewGetProcAddress((const GLubyte*)"glCopyMultiTexSubImage1DEXT")) == null) || r;
  r = ((glCopyMultiTexSubImage2DEXT = (PFNGLCOPYMULTITEXSUBIMAGE2DEXTPROC)glewGetProcAddress((const GLubyte*)"glCopyMultiTexSubImage2DEXT")) == null) || r;
  r = ((glCopyMultiTexSubImage3DEXT = (PFNGLCOPYMULTITEXSUBIMAGE3DEXTPROC)glewGetProcAddress((const GLubyte*)"glCopyMultiTexSubImage3DEXT")) == null) || r;
  r = ((glCopyTextureImage1DEXT = (PFNGLCOPYTEXTUREIMAGE1DEXTPROC)glewGetProcAddress((const GLubyte*)"glCopyTextureImage1DEXT")) == null) || r;
  r = ((glCopyTextureImage2DEXT = (PFNGLCOPYTEXTUREIMAGE2DEXTPROC)glewGetProcAddress((const GLubyte*)"glCopyTextureImage2DEXT")) == null) || r;
  r = ((glCopyTextureSubImage1DEXT = (PFNGLCOPYTEXTURESUBIMAGE1DEXTPROC)glewGetProcAddress((const GLubyte*)"glCopyTextureSubImage1DEXT")) == null) || r;
  r = ((glCopyTextureSubImage2DEXT = (PFNGLCOPYTEXTURESUBIMAGE2DEXTPROC)glewGetProcAddress((const GLubyte*)"glCopyTextureSubImage2DEXT")) == null) || r;
  r = ((glCopyTextureSubImage3DEXT = (PFNGLCOPYTEXTURESUBIMAGE3DEXTPROC)glewGetProcAddress((const GLubyte*)"glCopyTextureSubImage3DEXT")) == null) || r;
  r = ((glDisableClientStateIndexedEXT = (PFNGLDISABLECLIENTSTATEINDEXEDEXTPROC)glewGetProcAddress((const GLubyte*)"glDisableClientStateIndexedEXT")) == null) || r;
  r = ((glDisableClientStateiEXT = (PFNGLDISABLECLIENTSTATEIEXTPROC)glewGetProcAddress((const GLubyte*)"glDisableClientStateiEXT")) == null) || r;
  r = ((glDisableVertexArrayAttribEXT = (PFNGLDISABLEVERTEXARRAYATTRIBEXTPROC)glewGetProcAddress((const GLubyte*)"glDisableVertexArrayAttribEXT")) == null) || r;
  r = ((glDisableVertexArrayEXT = (PFNGLDISABLEVERTEXARRAYEXTPROC)glewGetProcAddress((const GLubyte*)"glDisableVertexArrayEXT")) == null) || r;
  r = ((glEnableClientStateIndexedEXT = (PFNGLENABLECLIENTSTATEINDEXEDEXTPROC)glewGetProcAddress((const GLubyte*)"glEnableClientStateIndexedEXT")) == null) || r;
  r = ((glEnableClientStateiEXT = (PFNGLENABLECLIENTSTATEIEXTPROC)glewGetProcAddress((const GLubyte*)"glEnableClientStateiEXT")) == null) || r;
  r = ((glEnableVertexArrayAttribEXT = (PFNGLENABLEVERTEXARRAYATTRIBEXTPROC)glewGetProcAddress((const GLubyte*)"glEnableVertexArrayAttribEXT")) == null) || r;
  r = ((glEnableVertexArrayEXT = (PFNGLENABLEVERTEXARRAYEXTPROC)glewGetProcAddress((const GLubyte*)"glEnableVertexArrayEXT")) == null) || r;
  r = ((glFlushMappedNamedBufferRangeEXT = (PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEEXTPROC)glewGetProcAddress((const GLubyte*)"glFlushMappedNamedBufferRangeEXT")) == null) || r;
  r = ((glFramebufferDrawBufferEXT = (PFNGLFRAMEBUFFERDRAWBUFFEREXTPROC)glewGetProcAddress((const GLubyte*)"glFramebufferDrawBufferEXT")) == null) || r;
  r = ((glFramebufferDrawBuffersEXT = (PFNGLFRAMEBUFFERDRAWBUFFERSEXTPROC)glewGetProcAddress((const GLubyte*)"glFramebufferDrawBuffersEXT")) == null) || r;
  r = ((glFramebufferReadBufferEXT = (PFNGLFRAMEBUFFERREADBUFFEREXTPROC)glewGetProcAddress((const GLubyte*)"glFramebufferReadBufferEXT")) == null) || r;
  r = ((glGenerateMultiTexMipmapEXT = (PFNGLGENERATEMULTITEXMIPMAPEXTPROC)glewGetProcAddress((const GLubyte*)"glGenerateMultiTexMipmapEXT")) == null) || r;
  r = ((glGenerateTextureMipmapEXT = (PFNGLGENERATETEXTUREMIPMAPEXTPROC)glewGetProcAddress((const GLubyte*)"glGenerateTextureMipmapEXT")) == null) || r;
  r = ((glGetCompressedMultiTexImageEXT = (PFNGLGETCOMPRESSEDMULTITEXIMAGEEXTPROC)glewGetProcAddress((const GLubyte*)"glGetCompressedMultiTexImageEXT")) == null) || r;
  r = ((glGetCompressedTextureImageEXT = (PFNGLGETCOMPRESSEDTEXTUREIMAGEEXTPROC)glewGetProcAddress((const GLubyte*)"glGetCompressedTextureImageEXT")) == null) || r;
  r = ((glGetDoubleIndexedvEXT = (PFNGLGETDOUBLEINDEXEDVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetDoubleIndexedvEXT")) == null) || r;
  r = ((glGetDoublei_vEXT = (PFNGLGETDOUBLEI_VEXTPROC)glewGetProcAddress((const GLubyte*)"glGetDoublei_vEXT")) == null) || r;
  r = ((glGetFloatIndexedvEXT = (PFNGLGETFLOATINDEXEDVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetFloatIndexedvEXT")) == null) || r;
  r = ((glGetFloati_vEXT = (PFNGLGETFLOATI_VEXTPROC)glewGetProcAddress((const GLubyte*)"glGetFloati_vEXT")) == null) || r;
  r = ((glGetFramebufferParameterivEXT = (PFNGLGETFRAMEBUFFERPARAMETERIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetFramebufferParameterivEXT")) == null) || r;
  r = ((glGetMultiTexEnvfvEXT = (PFNGLGETMULTITEXENVFVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetMultiTexEnvfvEXT")) == null) || r;
  r = ((glGetMultiTexEnvivEXT = (PFNGLGETMULTITEXENVIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetMultiTexEnvivEXT")) == null) || r;
  r = ((glGetMultiTexGendvEXT = (PFNGLGETMULTITEXGENDVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetMultiTexGendvEXT")) == null) || r;
  r = ((glGetMultiTexGenfvEXT = (PFNGLGETMULTITEXGENFVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetMultiTexGenfvEXT")) == null) || r;
  r = ((glGetMultiTexGenivEXT = (PFNGLGETMULTITEXGENIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetMultiTexGenivEXT")) == null) || r;
  r = ((glGetMultiTexImageEXT = (PFNGLGETMULTITEXIMAGEEXTPROC)glewGetProcAddress((const GLubyte*)"glGetMultiTexImageEXT")) == null) || r;
  r = ((glGetMultiTexLevelParameterfvEXT = (PFNGLGETMULTITEXLEVELPARAMETERFVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetMultiTexLevelParameterfvEXT")) == null) || r;
  r = ((glGetMultiTexLevelParameterivEXT = (PFNGLGETMULTITEXLEVELPARAMETERIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetMultiTexLevelParameterivEXT")) == null) || r;
  r = ((glGetMultiTexParameterIivEXT = (PFNGLGETMULTITEXPARAMETERIIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetMultiTexParameterIivEXT")) == null) || r;
  r = ((glGetMultiTexParameterIuivEXT = (PFNGLGETMULTITEXPARAMETERIUIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetMultiTexParameterIuivEXT")) == null) || r;
  r = ((glGetMultiTexParameterfvEXT = (PFNGLGETMULTITEXPARAMETERFVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetMultiTexParameterfvEXT")) == null) || r;
  r = ((glGetMultiTexParameterivEXT = (PFNGLGETMULTITEXPARAMETERIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetMultiTexParameterivEXT")) == null) || r;
  r = ((glGetNamedBufferParameterivEXT = (PFNGLGETNAMEDBUFFERPARAMETERIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetNamedBufferParameterivEXT")) == null) || r;
  r = ((glGetNamedBufferPointervEXT = (PFNGLGETNAMEDBUFFERPOINTERVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetNamedBufferPointervEXT")) == null) || r;
  r = ((glGetNamedBufferSubDataEXT = (PFNGLGETNAMEDBUFFERSUBDATAEXTPROC)glewGetProcAddress((const GLubyte*)"glGetNamedBufferSubDataEXT")) == null) || r;
  r = ((glGetNamedFramebufferAttachmentParameterivEXT = (PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetNamedFramebufferAttachmentParameterivEXT")) == null) || r;
  r = ((glGetNamedProgramLocalParameterIivEXT = (PFNGLGETNAMEDPROGRAMLOCALPARAMETERIIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetNamedProgramLocalParameterIivEXT")) == null) || r;
  r = ((glGetNamedProgramLocalParameterIuivEXT = (PFNGLGETNAMEDPROGRAMLOCALPARAMETERIUIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetNamedProgramLocalParameterIuivEXT")) == null) || r;
  r = ((glGetNamedProgramLocalParameterdvEXT = (PFNGLGETNAMEDPROGRAMLOCALPARAMETERDVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetNamedProgramLocalParameterdvEXT")) == null) || r;
  r = ((glGetNamedProgramLocalParameterfvEXT = (PFNGLGETNAMEDPROGRAMLOCALPARAMETERFVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetNamedProgramLocalParameterfvEXT")) == null) || r;
  r = ((glGetNamedProgramStringEXT = (PFNGLGETNAMEDPROGRAMSTRINGEXTPROC)glewGetProcAddress((const GLubyte*)"glGetNamedProgramStringEXT")) == null) || r;
  r = ((glGetNamedProgramivEXT = (PFNGLGETNAMEDPROGRAMIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetNamedProgramivEXT")) == null) || r;
  r = ((glGetNamedRenderbufferParameterivEXT = (PFNGLGETNAMEDRENDERBUFFERPARAMETERIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetNamedRenderbufferParameterivEXT")) == null) || r;
  r = ((glGetPointerIndexedvEXT = (PFNGLGETPOINTERINDEXEDVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetPointerIndexedvEXT")) == null) || r;
  r = ((glGetPointeri_vEXT = (PFNGLGETPOINTERI_VEXTPROC)glewGetProcAddress((const GLubyte*)"glGetPointeri_vEXT")) == null) || r;
  r = ((glGetTextureImageEXT = (PFNGLGETTEXTUREIMAGEEXTPROC)glewGetProcAddress((const GLubyte*)"glGetTextureImageEXT")) == null) || r;
  r = ((glGetTextureLevelParameterfvEXT = (PFNGLGETTEXTURELEVELPARAMETERFVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetTextureLevelParameterfvEXT")) == null) || r;
  r = ((glGetTextureLevelParameterivEXT = (PFNGLGETTEXTURELEVELPARAMETERIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetTextureLevelParameterivEXT")) == null) || r;
  r = ((glGetTextureParameterIivEXT = (PFNGLGETTEXTUREPARAMETERIIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetTextureParameterIivEXT")) == null) || r;
  r = ((glGetTextureParameterIuivEXT = (PFNGLGETTEXTUREPARAMETERIUIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetTextureParameterIuivEXT")) == null) || r;
  r = ((glGetTextureParameterfvEXT = (PFNGLGETTEXTUREPARAMETERFVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetTextureParameterfvEXT")) == null) || r;
  r = ((glGetTextureParameterivEXT = (PFNGLGETTEXTUREPARAMETERIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetTextureParameterivEXT")) == null) || r;
  r = ((glGetVertexArrayIntegeri_vEXT = (PFNGLGETVERTEXARRAYINTEGERI_VEXTPROC)glewGetProcAddress((const GLubyte*)"glGetVertexArrayIntegeri_vEXT")) == null) || r;
  r = ((glGetVertexArrayIntegervEXT = (PFNGLGETVERTEXARRAYINTEGERVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetVertexArrayIntegervEXT")) == null) || r;
  r = ((glGetVertexArrayPointeri_vEXT = (PFNGLGETVERTEXARRAYPOINTERI_VEXTPROC)glewGetProcAddress((const GLubyte*)"glGetVertexArrayPointeri_vEXT")) == null) || r;
  r = ((glGetVertexArrayPointervEXT = (PFNGLGETVERTEXARRAYPOINTERVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetVertexArrayPointervEXT")) == null) || r;
  r = ((glMapNamedBufferEXT = (PFNGLMAPNAMEDBUFFEREXTPROC)glewGetProcAddress((const GLubyte*)"glMapNamedBufferEXT")) == null) || r;
  r = ((glMapNamedBufferRangeEXT = (PFNGLMAPNAMEDBUFFERRANGEEXTPROC)glewGetProcAddress((const GLubyte*)"glMapNamedBufferRangeEXT")) == null) || r;
  r = ((glMatrixFrustumEXT = (PFNGLMATRIXFRUSTUMEXTPROC)glewGetProcAddress((const GLubyte*)"glMatrixFrustumEXT")) == null) || r;
  r = ((glMatrixLoadIdentityEXT = (PFNGLMATRIXLOADIDENTITYEXTPROC)glewGetProcAddress((const GLubyte*)"glMatrixLoadIdentityEXT")) == null) || r;
  r = ((glMatrixLoadTransposedEXT = (PFNGLMATRIXLOADTRANSPOSEDEXTPROC)glewGetProcAddress((const GLubyte*)"glMatrixLoadTransposedEXT")) == null) || r;
  r = ((glMatrixLoadTransposefEXT = (PFNGLMATRIXLOADTRANSPOSEFEXTPROC)glewGetProcAddress((const GLubyte*)"glMatrixLoadTransposefEXT")) == null) || r;
  r = ((glMatrixLoaddEXT = (PFNGLMATRIXLOADDEXTPROC)glewGetProcAddress((const GLubyte*)"glMatrixLoaddEXT")) == null) || r;
  r = ((glMatrixLoadfEXT = (PFNGLMATRIXLOADFEXTPROC)glewGetProcAddress((const GLubyte*)"glMatrixLoadfEXT")) == null) || r;
  r = ((glMatrixMultTransposedEXT = (PFNGLMATRIXMULTTRANSPOSEDEXTPROC)glewGetProcAddress((const GLubyte*)"glMatrixMultTransposedEXT")) == null) || r;
  r = ((glMatrixMultTransposefEXT = (PFNGLMATRIXMULTTRANSPOSEFEXTPROC)glewGetProcAddress((const GLubyte*)"glMatrixMultTransposefEXT")) == null) || r;
  r = ((glMatrixMultdEXT = (PFNGLMATRIXMULTDEXTPROC)glewGetProcAddress((const GLubyte*)"glMatrixMultdEXT")) == null) || r;
  r = ((glMatrixMultfEXT = (PFNGLMATRIXMULTFEXTPROC)glewGetProcAddress((const GLubyte*)"glMatrixMultfEXT")) == null) || r;
  r = ((glMatrixOrthoEXT = (PFNGLMATRIXORTHOEXTPROC)glewGetProcAddress((const GLubyte*)"glMatrixOrthoEXT")) == null) || r;
  r = ((glMatrixPopEXT = (PFNGLMATRIXPOPEXTPROC)glewGetProcAddress((const GLubyte*)"glMatrixPopEXT")) == null) || r;
  r = ((glMatrixPushEXT = (PFNGLMATRIXPUSHEXTPROC)glewGetProcAddress((const GLubyte*)"glMatrixPushEXT")) == null) || r;
  r = ((glMatrixRotatedEXT = (PFNGLMATRIXROTATEDEXTPROC)glewGetProcAddress((const GLubyte*)"glMatrixRotatedEXT")) == null) || r;
  r = ((glMatrixRotatefEXT = (PFNGLMATRIXROTATEFEXTPROC)glewGetProcAddress((const GLubyte*)"glMatrixRotatefEXT")) == null) || r;
  r = ((glMatrixScaledEXT = (PFNGLMATRIXSCALEDEXTPROC)glewGetProcAddress((const GLubyte*)"glMatrixScaledEXT")) == null) || r;
  r = ((glMatrixScalefEXT = (PFNGLMATRIXSCALEFEXTPROC)glewGetProcAddress((const GLubyte*)"glMatrixScalefEXT")) == null) || r;
  r = ((glMatrixTranslatedEXT = (PFNGLMATRIXTRANSLATEDEXTPROC)glewGetProcAddress((const GLubyte*)"glMatrixTranslatedEXT")) == null) || r;
  r = ((glMatrixTranslatefEXT = (PFNGLMATRIXTRANSLATEFEXTPROC)glewGetProcAddress((const GLubyte*)"glMatrixTranslatefEXT")) == null) || r;
  r = ((glMultiTexBufferEXT = (PFNGLMULTITEXBUFFEREXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexBufferEXT")) == null) || r;
  r = ((glMultiTexCoordPointerEXT = (PFNGLMULTITEXCOORDPOINTEREXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoordPointerEXT")) == null) || r;
  r = ((glMultiTexEnvfEXT = (PFNGLMULTITEXENVFEXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexEnvfEXT")) == null) || r;
  r = ((glMultiTexEnvfvEXT = (PFNGLMULTITEXENVFVEXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexEnvfvEXT")) == null) || r;
  r = ((glMultiTexEnviEXT = (PFNGLMULTITEXENVIEXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexEnviEXT")) == null) || r;
  r = ((glMultiTexEnvivEXT = (PFNGLMULTITEXENVIVEXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexEnvivEXT")) == null) || r;
  r = ((glMultiTexGendEXT = (PFNGLMULTITEXGENDEXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexGendEXT")) == null) || r;
  r = ((glMultiTexGendvEXT = (PFNGLMULTITEXGENDVEXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexGendvEXT")) == null) || r;
  r = ((glMultiTexGenfEXT = (PFNGLMULTITEXGENFEXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexGenfEXT")) == null) || r;
  r = ((glMultiTexGenfvEXT = (PFNGLMULTITEXGENFVEXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexGenfvEXT")) == null) || r;
  r = ((glMultiTexGeniEXT = (PFNGLMULTITEXGENIEXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexGeniEXT")) == null) || r;
  r = ((glMultiTexGenivEXT = (PFNGLMULTITEXGENIVEXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexGenivEXT")) == null) || r;
  r = ((glMultiTexImage1DEXT = (PFNGLMULTITEXIMAGE1DEXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexImage1DEXT")) == null) || r;
  r = ((glMultiTexImage2DEXT = (PFNGLMULTITEXIMAGE2DEXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexImage2DEXT")) == null) || r;
  r = ((glMultiTexImage3DEXT = (PFNGLMULTITEXIMAGE3DEXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexImage3DEXT")) == null) || r;
  r = ((glMultiTexParameterIivEXT = (PFNGLMULTITEXPARAMETERIIVEXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexParameterIivEXT")) == null) || r;
  r = ((glMultiTexParameterIuivEXT = (PFNGLMULTITEXPARAMETERIUIVEXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexParameterIuivEXT")) == null) || r;
  r = ((glMultiTexParameterfEXT = (PFNGLMULTITEXPARAMETERFEXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexParameterfEXT")) == null) || r;
  r = ((glMultiTexParameterfvEXT = (PFNGLMULTITEXPARAMETERFVEXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexParameterfvEXT")) == null) || r;
  r = ((glMultiTexParameteriEXT = (PFNGLMULTITEXPARAMETERIEXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexParameteriEXT")) == null) || r;
  r = ((glMultiTexParameterivEXT = (PFNGLMULTITEXPARAMETERIVEXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexParameterivEXT")) == null) || r;
  r = ((glMultiTexRenderbufferEXT = (PFNGLMULTITEXRENDERBUFFEREXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexRenderbufferEXT")) == null) || r;
  r = ((glMultiTexSubImage1DEXT = (PFNGLMULTITEXSUBIMAGE1DEXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexSubImage1DEXT")) == null) || r;
  r = ((glMultiTexSubImage2DEXT = (PFNGLMULTITEXSUBIMAGE2DEXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexSubImage2DEXT")) == null) || r;
  r = ((glMultiTexSubImage3DEXT = (PFNGLMULTITEXSUBIMAGE3DEXTPROC)glewGetProcAddress((const GLubyte*)"glMultiTexSubImage3DEXT")) == null) || r;
  r = ((glNamedBufferDataEXT = (PFNGLNAMEDBUFFERDATAEXTPROC)glewGetProcAddress((const GLubyte*)"glNamedBufferDataEXT")) == null) || r;
  r = ((glNamedBufferSubDataEXT = (PFNGLNAMEDBUFFERSUBDATAEXTPROC)glewGetProcAddress((const GLubyte*)"glNamedBufferSubDataEXT")) == null) || r;
  r = ((glNamedCopyBufferSubDataEXT = (PFNGLNAMEDCOPYBUFFERSUBDATAEXTPROC)glewGetProcAddress((const GLubyte*)"glNamedCopyBufferSubDataEXT")) == null) || r;
  r = ((glNamedFramebufferRenderbufferEXT = (PFNGLNAMEDFRAMEBUFFERRENDERBUFFEREXTPROC)glewGetProcAddress((const GLubyte*)"glNamedFramebufferRenderbufferEXT")) == null) || r;
  r = ((glNamedFramebufferTexture1DEXT = (PFNGLNAMEDFRAMEBUFFERTEXTURE1DEXTPROC)glewGetProcAddress((const GLubyte*)"glNamedFramebufferTexture1DEXT")) == null) || r;
  r = ((glNamedFramebufferTexture2DEXT = (PFNGLNAMEDFRAMEBUFFERTEXTURE2DEXTPROC)glewGetProcAddress((const GLubyte*)"glNamedFramebufferTexture2DEXT")) == null) || r;
  r = ((glNamedFramebufferTexture3DEXT = (PFNGLNAMEDFRAMEBUFFERTEXTURE3DEXTPROC)glewGetProcAddress((const GLubyte*)"glNamedFramebufferTexture3DEXT")) == null) || r;
  r = ((glNamedFramebufferTextureEXT = (PFNGLNAMEDFRAMEBUFFERTEXTUREEXTPROC)glewGetProcAddress((const GLubyte*)"glNamedFramebufferTextureEXT")) == null) || r;
  r = ((glNamedFramebufferTextureFaceEXT = (PFNGLNAMEDFRAMEBUFFERTEXTUREFACEEXTPROC)glewGetProcAddress((const GLubyte*)"glNamedFramebufferTextureFaceEXT")) == null) || r;
  r = ((glNamedFramebufferTextureLayerEXT = (PFNGLNAMEDFRAMEBUFFERTEXTURELAYEREXTPROC)glewGetProcAddress((const GLubyte*)"glNamedFramebufferTextureLayerEXT")) == null) || r;
  r = ((glNamedProgramLocalParameter4dEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETER4DEXTPROC)glewGetProcAddress((const GLubyte*)"glNamedProgramLocalParameter4dEXT")) == null) || r;
  r = ((glNamedProgramLocalParameter4dvEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETER4DVEXTPROC)glewGetProcAddress((const GLubyte*)"glNamedProgramLocalParameter4dvEXT")) == null) || r;
  r = ((glNamedProgramLocalParameter4fEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETER4FEXTPROC)glewGetProcAddress((const GLubyte*)"glNamedProgramLocalParameter4fEXT")) == null) || r;
  r = ((glNamedProgramLocalParameter4fvEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETER4FVEXTPROC)glewGetProcAddress((const GLubyte*)"glNamedProgramLocalParameter4fvEXT")) == null) || r;
  r = ((glNamedProgramLocalParameterI4iEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERI4IEXTPROC)glewGetProcAddress((const GLubyte*)"glNamedProgramLocalParameterI4iEXT")) == null) || r;
  r = ((glNamedProgramLocalParameterI4ivEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERI4IVEXTPROC)glewGetProcAddress((const GLubyte*)"glNamedProgramLocalParameterI4ivEXT")) == null) || r;
  r = ((glNamedProgramLocalParameterI4uiEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIEXTPROC)glewGetProcAddress((const GLubyte*)"glNamedProgramLocalParameterI4uiEXT")) == null) || r;
  r = ((glNamedProgramLocalParameterI4uivEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIVEXTPROC)glewGetProcAddress((const GLubyte*)"glNamedProgramLocalParameterI4uivEXT")) == null) || r;
  r = ((glNamedProgramLocalParameters4fvEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERS4FVEXTPROC)glewGetProcAddress((const GLubyte*)"glNamedProgramLocalParameters4fvEXT")) == null) || r;
  r = ((glNamedProgramLocalParametersI4ivEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERSI4IVEXTPROC)glewGetProcAddress((const GLubyte*)"glNamedProgramLocalParametersI4ivEXT")) == null) || r;
  r = ((glNamedProgramLocalParametersI4uivEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERSI4UIVEXTPROC)glewGetProcAddress((const GLubyte*)"glNamedProgramLocalParametersI4uivEXT")) == null) || r;
  r = ((glNamedProgramStringEXT = (PFNGLNAMEDPROGRAMSTRINGEXTPROC)glewGetProcAddress((const GLubyte*)"glNamedProgramStringEXT")) == null) || r;
  r = ((glNamedRenderbufferStorageEXT = (PFNGLNAMEDRENDERBUFFERSTORAGEEXTPROC)glewGetProcAddress((const GLubyte*)"glNamedRenderbufferStorageEXT")) == null) || r;
  r = ((glNamedRenderbufferStorageMultisampleCoverageEXT = (PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLECOVERAGEEXTPROC)glewGetProcAddress((const GLubyte*)"glNamedRenderbufferStorageMultisampleCoverageEXT")) == null) || r;
  r = ((glNamedRenderbufferStorageMultisampleEXT = (PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)glewGetProcAddress((const GLubyte*)"glNamedRenderbufferStorageMultisampleEXT")) == null) || r;
  r = ((glProgramUniform1fEXT = (PFNGLPROGRAMUNIFORM1FEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniform1fEXT")) == null) || r;
  r = ((glProgramUniform1fvEXT = (PFNGLPROGRAMUNIFORM1FVEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniform1fvEXT")) == null) || r;
  r = ((glProgramUniform1iEXT = (PFNGLPROGRAMUNIFORM1IEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniform1iEXT")) == null) || r;
  r = ((glProgramUniform1ivEXT = (PFNGLPROGRAMUNIFORM1IVEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniform1ivEXT")) == null) || r;
  r = ((glProgramUniform1uiEXT = (PFNGLPROGRAMUNIFORM1UIEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniform1uiEXT")) == null) || r;
  r = ((glProgramUniform1uivEXT = (PFNGLPROGRAMUNIFORM1UIVEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniform1uivEXT")) == null) || r;
  r = ((glProgramUniform2fEXT = (PFNGLPROGRAMUNIFORM2FEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniform2fEXT")) == null) || r;
  r = ((glProgramUniform2fvEXT = (PFNGLPROGRAMUNIFORM2FVEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniform2fvEXT")) == null) || r;
  r = ((glProgramUniform2iEXT = (PFNGLPROGRAMUNIFORM2IEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniform2iEXT")) == null) || r;
  r = ((glProgramUniform2ivEXT = (PFNGLPROGRAMUNIFORM2IVEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniform2ivEXT")) == null) || r;
  r = ((glProgramUniform2uiEXT = (PFNGLPROGRAMUNIFORM2UIEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniform2uiEXT")) == null) || r;
  r = ((glProgramUniform2uivEXT = (PFNGLPROGRAMUNIFORM2UIVEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniform2uivEXT")) == null) || r;
  r = ((glProgramUniform3fEXT = (PFNGLPROGRAMUNIFORM3FEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniform3fEXT")) == null) || r;
  r = ((glProgramUniform3fvEXT = (PFNGLPROGRAMUNIFORM3FVEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniform3fvEXT")) == null) || r;
  r = ((glProgramUniform3iEXT = (PFNGLPROGRAMUNIFORM3IEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniform3iEXT")) == null) || r;
  r = ((glProgramUniform3ivEXT = (PFNGLPROGRAMUNIFORM3IVEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniform3ivEXT")) == null) || r;
  r = ((glProgramUniform3uiEXT = (PFNGLPROGRAMUNIFORM3UIEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniform3uiEXT")) == null) || r;
  r = ((glProgramUniform3uivEXT = (PFNGLPROGRAMUNIFORM3UIVEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniform3uivEXT")) == null) || r;
  r = ((glProgramUniform4fEXT = (PFNGLPROGRAMUNIFORM4FEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniform4fEXT")) == null) || r;
  r = ((glProgramUniform4fvEXT = (PFNGLPROGRAMUNIFORM4FVEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniform4fvEXT")) == null) || r;
  r = ((glProgramUniform4iEXT = (PFNGLPROGRAMUNIFORM4IEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniform4iEXT")) == null) || r;
  r = ((glProgramUniform4ivEXT = (PFNGLPROGRAMUNIFORM4IVEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniform4ivEXT")) == null) || r;
  r = ((glProgramUniform4uiEXT = (PFNGLPROGRAMUNIFORM4UIEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniform4uiEXT")) == null) || r;
  r = ((glProgramUniform4uivEXT = (PFNGLPROGRAMUNIFORM4UIVEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniform4uivEXT")) == null) || r;
  r = ((glProgramUniformMatrix2fvEXT = (PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniformMatrix2fvEXT")) == null) || r;
  r = ((glProgramUniformMatrix2x3fvEXT = (PFNGLPROGRAMUNIFORMMATRIX2X3FVEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniformMatrix2x3fvEXT")) == null) || r;
  r = ((glProgramUniformMatrix2x4fvEXT = (PFNGLPROGRAMUNIFORMMATRIX2X4FVEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniformMatrix2x4fvEXT")) == null) || r;
  r = ((glProgramUniformMatrix3fvEXT = (PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniformMatrix3fvEXT")) == null) || r;
  r = ((glProgramUniformMatrix3x2fvEXT = (PFNGLPROGRAMUNIFORMMATRIX3X2FVEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniformMatrix3x2fvEXT")) == null) || r;
  r = ((glProgramUniformMatrix3x4fvEXT = (PFNGLPROGRAMUNIFORMMATRIX3X4FVEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniformMatrix3x4fvEXT")) == null) || r;
  r = ((glProgramUniformMatrix4fvEXT = (PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniformMatrix4fvEXT")) == null) || r;
  r = ((glProgramUniformMatrix4x2fvEXT = (PFNGLPROGRAMUNIFORMMATRIX4X2FVEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniformMatrix4x2fvEXT")) == null) || r;
  r = ((glProgramUniformMatrix4x3fvEXT = (PFNGLPROGRAMUNIFORMMATRIX4X3FVEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramUniformMatrix4x3fvEXT")) == null) || r;
  r = ((glPushClientAttribDefaultEXT = (PFNGLPUSHCLIENTATTRIBDEFAULTEXTPROC)glewGetProcAddress((const GLubyte*)"glPushClientAttribDefaultEXT")) == null) || r;
  r = ((glTextureBufferEXT = (PFNGLTEXTUREBUFFEREXTPROC)glewGetProcAddress((const GLubyte*)"glTextureBufferEXT")) == null) || r;
  r = ((glTextureImage1DEXT = (PFNGLTEXTUREIMAGE1DEXTPROC)glewGetProcAddress((const GLubyte*)"glTextureImage1DEXT")) == null) || r;
  r = ((glTextureImage2DEXT = (PFNGLTEXTUREIMAGE2DEXTPROC)glewGetProcAddress((const GLubyte*)"glTextureImage2DEXT")) == null) || r;
  r = ((glTextureImage3DEXT = (PFNGLTEXTUREIMAGE3DEXTPROC)glewGetProcAddress((const GLubyte*)"glTextureImage3DEXT")) == null) || r;
  r = ((glTextureParameterIivEXT = (PFNGLTEXTUREPARAMETERIIVEXTPROC)glewGetProcAddress((const GLubyte*)"glTextureParameterIivEXT")) == null) || r;
  r = ((glTextureParameterIuivEXT = (PFNGLTEXTUREPARAMETERIUIVEXTPROC)glewGetProcAddress((const GLubyte*)"glTextureParameterIuivEXT")) == null) || r;
  r = ((glTextureParameterfEXT = (PFNGLTEXTUREPARAMETERFEXTPROC)glewGetProcAddress((const GLubyte*)"glTextureParameterfEXT")) == null) || r;
  r = ((glTextureParameterfvEXT = (PFNGLTEXTUREPARAMETERFVEXTPROC)glewGetProcAddress((const GLubyte*)"glTextureParameterfvEXT")) == null) || r;
  r = ((glTextureParameteriEXT = (PFNGLTEXTUREPARAMETERIEXTPROC)glewGetProcAddress((const GLubyte*)"glTextureParameteriEXT")) == null) || r;
  r = ((glTextureParameterivEXT = (PFNGLTEXTUREPARAMETERIVEXTPROC)glewGetProcAddress((const GLubyte*)"glTextureParameterivEXT")) == null) || r;
  r = ((glTextureRenderbufferEXT = (PFNGLTEXTURERENDERBUFFEREXTPROC)glewGetProcAddress((const GLubyte*)"glTextureRenderbufferEXT")) == null) || r;
  r = ((glTextureSubImage1DEXT = (PFNGLTEXTURESUBIMAGE1DEXTPROC)glewGetProcAddress((const GLubyte*)"glTextureSubImage1DEXT")) == null) || r;
  r = ((glTextureSubImage2DEXT = (PFNGLTEXTURESUBIMAGE2DEXTPROC)glewGetProcAddress((const GLubyte*)"glTextureSubImage2DEXT")) == null) || r;
  r = ((glTextureSubImage3DEXT = (PFNGLTEXTURESUBIMAGE3DEXTPROC)glewGetProcAddress((const GLubyte*)"glTextureSubImage3DEXT")) == null) || r;
  r = ((glUnmapNamedBufferEXT = (PFNGLUNMAPNAMEDBUFFEREXTPROC)glewGetProcAddress((const GLubyte*)"glUnmapNamedBufferEXT")) == null) || r;
  r = ((glVertexArrayColorOffsetEXT = (PFNGLVERTEXARRAYCOLOROFFSETEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexArrayColorOffsetEXT")) == null) || r;
  r = ((glVertexArrayEdgeFlagOffsetEXT = (PFNGLVERTEXARRAYEDGEFLAGOFFSETEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexArrayEdgeFlagOffsetEXT")) == null) || r;
  r = ((glVertexArrayFogCoordOffsetEXT = (PFNGLVERTEXARRAYFOGCOORDOFFSETEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexArrayFogCoordOffsetEXT")) == null) || r;
  r = ((glVertexArrayIndexOffsetEXT = (PFNGLVERTEXARRAYINDEXOFFSETEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexArrayIndexOffsetEXT")) == null) || r;
  r = ((glVertexArrayMultiTexCoordOffsetEXT = (PFNGLVERTEXARRAYMULTITEXCOORDOFFSETEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexArrayMultiTexCoordOffsetEXT")) == null) || r;
  r = ((glVertexArrayNormalOffsetEXT = (PFNGLVERTEXARRAYNORMALOFFSETEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexArrayNormalOffsetEXT")) == null) || r;
  r = ((glVertexArraySecondaryColorOffsetEXT = (PFNGLVERTEXARRAYSECONDARYCOLOROFFSETEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexArraySecondaryColorOffsetEXT")) == null) || r;
  r = ((glVertexArrayTexCoordOffsetEXT = (PFNGLVERTEXARRAYTEXCOORDOFFSETEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexArrayTexCoordOffsetEXT")) == null) || r;
  r = ((glVertexArrayVertexAttribIOffsetEXT = (PFNGLVERTEXARRAYVERTEXATTRIBIOFFSETEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexArrayVertexAttribIOffsetEXT")) == null) || r;
  r = ((glVertexArrayVertexAttribOffsetEXT = (PFNGLVERTEXARRAYVERTEXATTRIBOFFSETEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexArrayVertexAttribOffsetEXT")) == null) || r;
  r = ((glVertexArrayVertexOffsetEXT = (PFNGLVERTEXARRAYVERTEXOFFSETEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexArrayVertexOffsetEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_direct_state_access */

#ifdef GL_EXT_draw_buffers2

static GLboolean _glewInit_GL_EXT_draw_buffers2 (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glColorMaskIndexedEXT = (PFNGLCOLORMASKINDEXEDEXTPROC)glewGetProcAddress((const GLubyte*)"glColorMaskIndexedEXT")) == null) || r;
  r = ((glDisableIndexedEXT = (PFNGLDISABLEINDEXEDEXTPROC)glewGetProcAddress((const GLubyte*)"glDisableIndexedEXT")) == null) || r;
  r = ((glEnableIndexedEXT = (PFNGLENABLEINDEXEDEXTPROC)glewGetProcAddress((const GLubyte*)"glEnableIndexedEXT")) == null) || r;
  r = ((glGetBooleanIndexedvEXT = (PFNGLGETBOOLEANINDEXEDVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetBooleanIndexedvEXT")) == null) || r;
  r = ((glGetIntegerIndexedvEXT = (PFNGLGETINTEGERINDEXEDVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetIntegerIndexedvEXT")) == null) || r;
  r = ((glIsEnabledIndexedEXT = (PFNGLISENABLEDINDEXEDEXTPROC)glewGetProcAddress((const GLubyte*)"glIsEnabledIndexedEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_draw_buffers2 */

#ifdef GL_EXT_draw_instanced

static GLboolean _glewInit_GL_EXT_draw_instanced (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glDrawArraysInstancedEXT = (PFNGLDRAWARRAYSINSTANCEDEXTPROC)glewGetProcAddress((const GLubyte*)"glDrawArraysInstancedEXT")) == null) || r;
  r = ((glDrawElementsInstancedEXT = (PFNGLDRAWELEMENTSINSTANCEDEXTPROC)glewGetProcAddress((const GLubyte*)"glDrawElementsInstancedEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_draw_instanced */

#ifdef GL_EXT_draw_range_elements

static GLboolean _glewInit_GL_EXT_draw_range_elements (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glDrawRangeElementsEXT = (PFNGLDRAWRANGEELEMENTSEXTPROC)glewGetProcAddress((const GLubyte*)"glDrawRangeElementsEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_draw_range_elements */

#ifdef GL_EXT_fog_coord

static GLboolean _glewInit_GL_EXT_fog_coord (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glFogCoordPointerEXT = (PFNGLFOGCOORDPOINTEREXTPROC)glewGetProcAddress((const GLubyte*)"glFogCoordPointerEXT")) == null) || r;
  r = ((glFogCoorddEXT = (PFNGLFOGCOORDDEXTPROC)glewGetProcAddress((const GLubyte*)"glFogCoorddEXT")) == null) || r;
  r = ((glFogCoorddvEXT = (PFNGLFOGCOORDDVEXTPROC)glewGetProcAddress((const GLubyte*)"glFogCoorddvEXT")) == null) || r;
  r = ((glFogCoordfEXT = (PFNGLFOGCOORDFEXTPROC)glewGetProcAddress((const GLubyte*)"glFogCoordfEXT")) == null) || r;
  r = ((glFogCoordfvEXT = (PFNGLFOGCOORDFVEXTPROC)glewGetProcAddress((const GLubyte*)"glFogCoordfvEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_fog_coord */

#ifdef GL_EXT_fragment_lighting

static GLboolean _glewInit_GL_EXT_fragment_lighting (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glFragmentColorMaterialEXT = (PFNGLFRAGMENTCOLORMATERIALEXTPROC)glewGetProcAddress((const GLubyte*)"glFragmentColorMaterialEXT")) == null) || r;
  r = ((glFragmentLightModelfEXT = (PFNGLFRAGMENTLIGHTMODELFEXTPROC)glewGetProcAddress((const GLubyte*)"glFragmentLightModelfEXT")) == null) || r;
  r = ((glFragmentLightModelfvEXT = (PFNGLFRAGMENTLIGHTMODELFVEXTPROC)glewGetProcAddress((const GLubyte*)"glFragmentLightModelfvEXT")) == null) || r;
  r = ((glFragmentLightModeliEXT = (PFNGLFRAGMENTLIGHTMODELIEXTPROC)glewGetProcAddress((const GLubyte*)"glFragmentLightModeliEXT")) == null) || r;
  r = ((glFragmentLightModelivEXT = (PFNGLFRAGMENTLIGHTMODELIVEXTPROC)glewGetProcAddress((const GLubyte*)"glFragmentLightModelivEXT")) == null) || r;
  r = ((glFragmentLightfEXT = (PFNGLFRAGMENTLIGHTFEXTPROC)glewGetProcAddress((const GLubyte*)"glFragmentLightfEXT")) == null) || r;
  r = ((glFragmentLightfvEXT = (PFNGLFRAGMENTLIGHTFVEXTPROC)glewGetProcAddress((const GLubyte*)"glFragmentLightfvEXT")) == null) || r;
  r = ((glFragmentLightiEXT = (PFNGLFRAGMENTLIGHTIEXTPROC)glewGetProcAddress((const GLubyte*)"glFragmentLightiEXT")) == null) || r;
  r = ((glFragmentLightivEXT = (PFNGLFRAGMENTLIGHTIVEXTPROC)glewGetProcAddress((const GLubyte*)"glFragmentLightivEXT")) == null) || r;
  r = ((glFragmentMaterialfEXT = (PFNGLFRAGMENTMATERIALFEXTPROC)glewGetProcAddress((const GLubyte*)"glFragmentMaterialfEXT")) == null) || r;
  r = ((glFragmentMaterialfvEXT = (PFNGLFRAGMENTMATERIALFVEXTPROC)glewGetProcAddress((const GLubyte*)"glFragmentMaterialfvEXT")) == null) || r;
  r = ((glFragmentMaterialiEXT = (PFNGLFRAGMENTMATERIALIEXTPROC)glewGetProcAddress((const GLubyte*)"glFragmentMaterialiEXT")) == null) || r;
  r = ((glFragmentMaterialivEXT = (PFNGLFRAGMENTMATERIALIVEXTPROC)glewGetProcAddress((const GLubyte*)"glFragmentMaterialivEXT")) == null) || r;
  r = ((glGetFragmentLightfvEXT = (PFNGLGETFRAGMENTLIGHTFVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetFragmentLightfvEXT")) == null) || r;
  r = ((glGetFragmentLightivEXT = (PFNGLGETFRAGMENTLIGHTIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetFragmentLightivEXT")) == null) || r;
  r = ((glGetFragmentMaterialfvEXT = (PFNGLGETFRAGMENTMATERIALFVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetFragmentMaterialfvEXT")) == null) || r;
  r = ((glGetFragmentMaterialivEXT = (PFNGLGETFRAGMENTMATERIALIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetFragmentMaterialivEXT")) == null) || r;
  r = ((glLightEnviEXT = (PFNGLLIGHTENVIEXTPROC)glewGetProcAddress((const GLubyte*)"glLightEnviEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_fragment_lighting */

#ifdef GL_EXT_framebuffer_blit

static GLboolean _glewInit_GL_EXT_framebuffer_blit (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBlitFramebufferEXT = (PFNGLBLITFRAMEBUFFEREXTPROC)glewGetProcAddress((const GLubyte*)"glBlitFramebufferEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_framebuffer_blit */

#ifdef GL_EXT_framebuffer_multisample

static GLboolean _glewInit_GL_EXT_framebuffer_multisample (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glRenderbufferStorageMultisampleEXT = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)glewGetProcAddress((const GLubyte*)"glRenderbufferStorageMultisampleEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_framebuffer_multisample */

#ifdef GL_EXT_framebuffer_object

static GLboolean _glewInit_GL_EXT_framebuffer_object (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)glewGetProcAddress((const GLubyte*)"glBindFramebufferEXT")) == null) || r;
  r = ((glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC)glewGetProcAddress((const GLubyte*)"glBindRenderbufferEXT")) == null) || r;
  r = ((glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)glewGetProcAddress((const GLubyte*)"glCheckFramebufferStatusEXT")) == null) || r;
  r = ((glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC)glewGetProcAddress((const GLubyte*)"glDeleteFramebuffersEXT")) == null) || r;
  r = ((glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC)glewGetProcAddress((const GLubyte*)"glDeleteRenderbuffersEXT")) == null) || r;
  r = ((glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)glewGetProcAddress((const GLubyte*)"glFramebufferRenderbufferEXT")) == null) || r;
  r = ((glFramebufferTexture1DEXT = (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)glewGetProcAddress((const GLubyte*)"glFramebufferTexture1DEXT")) == null) || r;
  r = ((glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)glewGetProcAddress((const GLubyte*)"glFramebufferTexture2DEXT")) == null) || r;
  r = ((glFramebufferTexture3DEXT = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)glewGetProcAddress((const GLubyte*)"glFramebufferTexture3DEXT")) == null) || r;
  r = ((glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)glewGetProcAddress((const GLubyte*)"glGenFramebuffersEXT")) == null) || r;
  r = ((glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC)glewGetProcAddress((const GLubyte*)"glGenRenderbuffersEXT")) == null) || r;
  r = ((glGenerateMipmapEXT = (PFNGLGENERATEMIPMAPEXTPROC)glewGetProcAddress((const GLubyte*)"glGenerateMipmapEXT")) == null) || r;
  r = ((glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetFramebufferAttachmentParameterivEXT")) == null) || r;
  r = ((glGetRenderbufferParameterivEXT = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetRenderbufferParameterivEXT")) == null) || r;
  r = ((glIsFramebufferEXT = (PFNGLISFRAMEBUFFEREXTPROC)glewGetProcAddress((const GLubyte*)"glIsFramebufferEXT")) == null) || r;
  r = ((glIsRenderbufferEXT = (PFNGLISRENDERBUFFEREXTPROC)glewGetProcAddress((const GLubyte*)"glIsRenderbufferEXT")) == null) || r;
  r = ((glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)glewGetProcAddress((const GLubyte*)"glRenderbufferStorageEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_framebuffer_object */

#ifdef GL_EXT_framebuffer_sRGB

#endif /* GL_EXT_framebuffer_sRGB */

#ifdef GL_EXT_geometry_shader4

static GLboolean _glewInit_GL_EXT_geometry_shader4 (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glFramebufferTextureEXT = (PFNGLFRAMEBUFFERTEXTUREEXTPROC)glewGetProcAddress((const GLubyte*)"glFramebufferTextureEXT")) == null) || r;
  r = ((glFramebufferTextureFaceEXT = (PFNGLFRAMEBUFFERTEXTUREFACEEXTPROC)glewGetProcAddress((const GLubyte*)"glFramebufferTextureFaceEXT")) == null) || r;
  r = ((glFramebufferTextureLayerEXT = (PFNGLFRAMEBUFFERTEXTURELAYEREXTPROC)glewGetProcAddress((const GLubyte*)"glFramebufferTextureLayerEXT")) == null) || r;
  r = ((glProgramParameteriEXT = (PFNGLPROGRAMPARAMETERIEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramParameteriEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_geometry_shader4 */

#ifdef GL_EXT_gpu_program_parameters

static GLboolean _glewInit_GL_EXT_gpu_program_parameters (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glProgramEnvParameters4fvEXT = (PFNGLPROGRAMENVPARAMETERS4FVEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramEnvParameters4fvEXT")) == null) || r;
  r = ((glProgramLocalParameters4fvEXT = (PFNGLPROGRAMLOCALPARAMETERS4FVEXTPROC)glewGetProcAddress((const GLubyte*)"glProgramLocalParameters4fvEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_gpu_program_parameters */

#ifdef GL_EXT_gpu_shader4

static GLboolean _glewInit_GL_EXT_gpu_shader4 (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBindFragDataLocationEXT = (PFNGLBINDFRAGDATALOCATIONEXTPROC)glewGetProcAddress((const GLubyte*)"glBindFragDataLocationEXT")) == null) || r;
  r = ((glGetFragDataLocationEXT = (PFNGLGETFRAGDATALOCATIONEXTPROC)glewGetProcAddress((const GLubyte*)"glGetFragDataLocationEXT")) == null) || r;
  r = ((glGetUniformuivEXT = (PFNGLGETUNIFORMUIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetUniformuivEXT")) == null) || r;
  r = ((glGetVertexAttribIivEXT = (PFNGLGETVERTEXATTRIBIIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetVertexAttribIivEXT")) == null) || r;
  r = ((glGetVertexAttribIuivEXT = (PFNGLGETVERTEXATTRIBIUIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetVertexAttribIuivEXT")) == null) || r;
  r = ((glUniform1uiEXT = (PFNGLUNIFORM1UIEXTPROC)glewGetProcAddress((const GLubyte*)"glUniform1uiEXT")) == null) || r;
  r = ((glUniform1uivEXT = (PFNGLUNIFORM1UIVEXTPROC)glewGetProcAddress((const GLubyte*)"glUniform1uivEXT")) == null) || r;
  r = ((glUniform2uiEXT = (PFNGLUNIFORM2UIEXTPROC)glewGetProcAddress((const GLubyte*)"glUniform2uiEXT")) == null) || r;
  r = ((glUniform2uivEXT = (PFNGLUNIFORM2UIVEXTPROC)glewGetProcAddress((const GLubyte*)"glUniform2uivEXT")) == null) || r;
  r = ((glUniform3uiEXT = (PFNGLUNIFORM3UIEXTPROC)glewGetProcAddress((const GLubyte*)"glUniform3uiEXT")) == null) || r;
  r = ((glUniform3uivEXT = (PFNGLUNIFORM3UIVEXTPROC)glewGetProcAddress((const GLubyte*)"glUniform3uivEXT")) == null) || r;
  r = ((glUniform4uiEXT = (PFNGLUNIFORM4UIEXTPROC)glewGetProcAddress((const GLubyte*)"glUniform4uiEXT")) == null) || r;
  r = ((glUniform4uivEXT = (PFNGLUNIFORM4UIVEXTPROC)glewGetProcAddress((const GLubyte*)"glUniform4uivEXT")) == null) || r;
  r = ((glVertexAttribI1iEXT = (PFNGLVERTEXATTRIBI1IEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI1iEXT")) == null) || r;
  r = ((glVertexAttribI1ivEXT = (PFNGLVERTEXATTRIBI1IVEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI1ivEXT")) == null) || r;
  r = ((glVertexAttribI1uiEXT = (PFNGLVERTEXATTRIBI1UIEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI1uiEXT")) == null) || r;
  r = ((glVertexAttribI1uivEXT = (PFNGLVERTEXATTRIBI1UIVEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI1uivEXT")) == null) || r;
  r = ((glVertexAttribI2iEXT = (PFNGLVERTEXATTRIBI2IEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI2iEXT")) == null) || r;
  r = ((glVertexAttribI2ivEXT = (PFNGLVERTEXATTRIBI2IVEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI2ivEXT")) == null) || r;
  r = ((glVertexAttribI2uiEXT = (PFNGLVERTEXATTRIBI2UIEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI2uiEXT")) == null) || r;
  r = ((glVertexAttribI2uivEXT = (PFNGLVERTEXATTRIBI2UIVEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI2uivEXT")) == null) || r;
  r = ((glVertexAttribI3iEXT = (PFNGLVERTEXATTRIBI3IEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI3iEXT")) == null) || r;
  r = ((glVertexAttribI3ivEXT = (PFNGLVERTEXATTRIBI3IVEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI3ivEXT")) == null) || r;
  r = ((glVertexAttribI3uiEXT = (PFNGLVERTEXATTRIBI3UIEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI3uiEXT")) == null) || r;
  r = ((glVertexAttribI3uivEXT = (PFNGLVERTEXATTRIBI3UIVEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI3uivEXT")) == null) || r;
  r = ((glVertexAttribI4bvEXT = (PFNGLVERTEXATTRIBI4BVEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI4bvEXT")) == null) || r;
  r = ((glVertexAttribI4iEXT = (PFNGLVERTEXATTRIBI4IEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI4iEXT")) == null) || r;
  r = ((glVertexAttribI4ivEXT = (PFNGLVERTEXATTRIBI4IVEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI4ivEXT")) == null) || r;
  r = ((glVertexAttribI4svEXT = (PFNGLVERTEXATTRIBI4SVEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI4svEXT")) == null) || r;
  r = ((glVertexAttribI4ubvEXT = (PFNGLVERTEXATTRIBI4UBVEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI4ubvEXT")) == null) || r;
  r = ((glVertexAttribI4uiEXT = (PFNGLVERTEXATTRIBI4UIEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI4uiEXT")) == null) || r;
  r = ((glVertexAttribI4uivEXT = (PFNGLVERTEXATTRIBI4UIVEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI4uivEXT")) == null) || r;
  r = ((glVertexAttribI4usvEXT = (PFNGLVERTEXATTRIBI4USVEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribI4usvEXT")) == null) || r;
  r = ((glVertexAttribIPointerEXT = (PFNGLVERTEXATTRIBIPOINTEREXTPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribIPointerEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_gpu_shader4 */

#ifdef GL_EXT_histogram

static GLboolean _glewInit_GL_EXT_histogram (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glGetHistogramEXT = (PFNGLGETHISTOGRAMEXTPROC)glewGetProcAddress((const GLubyte*)"glGetHistogramEXT")) == null) || r;
  r = ((glGetHistogramParameterfvEXT = (PFNGLGETHISTOGRAMPARAMETERFVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetHistogramParameterfvEXT")) == null) || r;
  r = ((glGetHistogramParameterivEXT = (PFNGLGETHISTOGRAMPARAMETERIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetHistogramParameterivEXT")) == null) || r;
  r = ((glGetMinmaxEXT = (PFNGLGETMINMAXEXTPROC)glewGetProcAddress((const GLubyte*)"glGetMinmaxEXT")) == null) || r;
  r = ((glGetMinmaxParameterfvEXT = (PFNGLGETMINMAXPARAMETERFVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetMinmaxParameterfvEXT")) == null) || r;
  r = ((glGetMinmaxParameterivEXT = (PFNGLGETMINMAXPARAMETERIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetMinmaxParameterivEXT")) == null) || r;
  r = ((glHistogramEXT = (PFNGLHISTOGRAMEXTPROC)glewGetProcAddress((const GLubyte*)"glHistogramEXT")) == null) || r;
  r = ((glMinmaxEXT = (PFNGLMINMAXEXTPROC)glewGetProcAddress((const GLubyte*)"glMinmaxEXT")) == null) || r;
  r = ((glResetHistogramEXT = (PFNGLRESETHISTOGRAMEXTPROC)glewGetProcAddress((const GLubyte*)"glResetHistogramEXT")) == null) || r;
  r = ((glResetMinmaxEXT = (PFNGLRESETMINMAXEXTPROC)glewGetProcAddress((const GLubyte*)"glResetMinmaxEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_histogram */

#ifdef GL_EXT_index_array_formats

#endif /* GL_EXT_index_array_formats */

#ifdef GL_EXT_index_func

static GLboolean _glewInit_GL_EXT_index_func (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glIndexFuncEXT = (PFNGLINDEXFUNCEXTPROC)glewGetProcAddress((const GLubyte*)"glIndexFuncEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_index_func */

#ifdef GL_EXT_index_material

static GLboolean _glewInit_GL_EXT_index_material (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glIndexMaterialEXT = (PFNGLINDEXMATERIALEXTPROC)glewGetProcAddress((const GLubyte*)"glIndexMaterialEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_index_material */

#ifdef GL_EXT_index_texture

#endif /* GL_EXT_index_texture */

#ifdef GL_EXT_light_texture

static GLboolean _glewInit_GL_EXT_light_texture (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glApplyTextureEXT = (PFNGLAPPLYTEXTUREEXTPROC)glewGetProcAddress((const GLubyte*)"glApplyTextureEXT")) == null) || r;
  r = ((glTextureLightEXT = (PFNGLTEXTURELIGHTEXTPROC)glewGetProcAddress((const GLubyte*)"glTextureLightEXT")) == null) || r;
  r = ((glTextureMaterialEXT = (PFNGLTEXTUREMATERIALEXTPROC)glewGetProcAddress((const GLubyte*)"glTextureMaterialEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_light_texture */

#ifdef GL_EXT_misc_attribute

#endif /* GL_EXT_misc_attribute */

#ifdef GL_EXT_multi_draw_arrays

static GLboolean _glewInit_GL_EXT_multi_draw_arrays (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glMultiDrawArraysEXT = (PFNGLMULTIDRAWARRAYSEXTPROC)glewGetProcAddress((const GLubyte*)"glMultiDrawArraysEXT")) == null) || r;
  r = ((glMultiDrawElementsEXT = (PFNGLMULTIDRAWELEMENTSEXTPROC)glewGetProcAddress((const GLubyte*)"glMultiDrawElementsEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_multi_draw_arrays */

#ifdef GL_EXT_multisample

static GLboolean _glewInit_GL_EXT_multisample (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glSampleMaskEXT = (PFNGLSAMPLEMASKEXTPROC)glewGetProcAddress((const GLubyte*)"glSampleMaskEXT")) == null) || r;
  r = ((glSamplePatternEXT = (PFNGLSAMPLEPATTERNEXTPROC)glewGetProcAddress((const GLubyte*)"glSamplePatternEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_multisample */

#ifdef GL_EXT_packed_depth_stencil

#endif /* GL_EXT_packed_depth_stencil */

#ifdef GL_EXT_packed_float

#endif /* GL_EXT_packed_float */

#ifdef GL_EXT_packed_pixels

#endif /* GL_EXT_packed_pixels */

#ifdef GL_EXT_paletted_texture

static GLboolean _glewInit_GL_EXT_paletted_texture (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glColorTableEXT = (PFNGLCOLORTABLEEXTPROC)glewGetProcAddress((const GLubyte*)"glColorTableEXT")) == null) || r;
  r = ((glGetColorTableEXT = (PFNGLGETCOLORTABLEEXTPROC)glewGetProcAddress((const GLubyte*)"glGetColorTableEXT")) == null) || r;
  r = ((glGetColorTableParameterfvEXT = (PFNGLGETCOLORTABLEPARAMETERFVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetColorTableParameterfvEXT")) == null) || r;
  r = ((glGetColorTableParameterivEXT = (PFNGLGETCOLORTABLEPARAMETERIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetColorTableParameterivEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_paletted_texture */

#ifdef GL_EXT_pixel_buffer_object

#endif /* GL_EXT_pixel_buffer_object */

#ifdef GL_EXT_pixel_transform

static GLboolean _glewInit_GL_EXT_pixel_transform (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glGetPixelTransformParameterfvEXT = (PFNGLGETPIXELTRANSFORMPARAMETERFVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetPixelTransformParameterfvEXT")) == null) || r;
  r = ((glGetPixelTransformParameterivEXT = (PFNGLGETPIXELTRANSFORMPARAMETERIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetPixelTransformParameterivEXT")) == null) || r;
  r = ((glPixelTransformParameterfEXT = (PFNGLPIXELTRANSFORMPARAMETERFEXTPROC)glewGetProcAddress((const GLubyte*)"glPixelTransformParameterfEXT")) == null) || r;
  r = ((glPixelTransformParameterfvEXT = (PFNGLPIXELTRANSFORMPARAMETERFVEXTPROC)glewGetProcAddress((const GLubyte*)"glPixelTransformParameterfvEXT")) == null) || r;
  r = ((glPixelTransformParameteriEXT = (PFNGLPIXELTRANSFORMPARAMETERIEXTPROC)glewGetProcAddress((const GLubyte*)"glPixelTransformParameteriEXT")) == null) || r;
  r = ((glPixelTransformParameterivEXT = (PFNGLPIXELTRANSFORMPARAMETERIVEXTPROC)glewGetProcAddress((const GLubyte*)"glPixelTransformParameterivEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_pixel_transform */

#ifdef GL_EXT_pixel_transform_color_table

#endif /* GL_EXT_pixel_transform_color_table */

#ifdef GL_EXT_point_parameters

static GLboolean _glewInit_GL_EXT_point_parameters (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glPointParameterfEXT = (PFNGLPOINTPARAMETERFEXTPROC)glewGetProcAddress((const GLubyte*)"glPointParameterfEXT")) == null) || r;
  r = ((glPointParameterfvEXT = (PFNGLPOINTPARAMETERFVEXTPROC)glewGetProcAddress((const GLubyte*)"glPointParameterfvEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_point_parameters */

#ifdef GL_EXT_polygon_offset

static GLboolean _glewInit_GL_EXT_polygon_offset (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glPolygonOffsetEXT = (PFNGLPOLYGONOFFSETEXTPROC)glewGetProcAddress((const GLubyte*)"glPolygonOffsetEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_polygon_offset */

#ifdef GL_EXT_provoking_vertex

static GLboolean _glewInit_GL_EXT_provoking_vertex (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glProvokingVertexEXT = (PFNGLPROVOKINGVERTEXEXTPROC)glewGetProcAddress((const GLubyte*)"glProvokingVertexEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_provoking_vertex */

#ifdef GL_EXT_rescale_normal

#endif /* GL_EXT_rescale_normal */

#ifdef GL_EXT_scene_marker

static GLboolean _glewInit_GL_EXT_scene_marker (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBeginSceneEXT = (PFNGLBEGINSCENEEXTPROC)glewGetProcAddress((const GLubyte*)"glBeginSceneEXT")) == null) || r;
  r = ((glEndSceneEXT = (PFNGLENDSCENEEXTPROC)glewGetProcAddress((const GLubyte*)"glEndSceneEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_scene_marker */

#ifdef GL_EXT_secondary_color

static GLboolean _glewInit_GL_EXT_secondary_color (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glSecondaryColor3bEXT = (PFNGLSECONDARYCOLOR3BEXTPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3bEXT")) == null) || r;
  r = ((glSecondaryColor3bvEXT = (PFNGLSECONDARYCOLOR3BVEXTPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3bvEXT")) == null) || r;
  r = ((glSecondaryColor3dEXT = (PFNGLSECONDARYCOLOR3DEXTPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3dEXT")) == null) || r;
  r = ((glSecondaryColor3dvEXT = (PFNGLSECONDARYCOLOR3DVEXTPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3dvEXT")) == null) || r;
  r = ((glSecondaryColor3fEXT = (PFNGLSECONDARYCOLOR3FEXTPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3fEXT")) == null) || r;
  r = ((glSecondaryColor3fvEXT = (PFNGLSECONDARYCOLOR3FVEXTPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3fvEXT")) == null) || r;
  r = ((glSecondaryColor3iEXT = (PFNGLSECONDARYCOLOR3IEXTPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3iEXT")) == null) || r;
  r = ((glSecondaryColor3ivEXT = (PFNGLSECONDARYCOLOR3IVEXTPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3ivEXT")) == null) || r;
  r = ((glSecondaryColor3sEXT = (PFNGLSECONDARYCOLOR3SEXTPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3sEXT")) == null) || r;
  r = ((glSecondaryColor3svEXT = (PFNGLSECONDARYCOLOR3SVEXTPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3svEXT")) == null) || r;
  r = ((glSecondaryColor3ubEXT = (PFNGLSECONDARYCOLOR3UBEXTPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3ubEXT")) == null) || r;
  r = ((glSecondaryColor3ubvEXT = (PFNGLSECONDARYCOLOR3UBVEXTPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3ubvEXT")) == null) || r;
  r = ((glSecondaryColor3uiEXT = (PFNGLSECONDARYCOLOR3UIEXTPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3uiEXT")) == null) || r;
  r = ((glSecondaryColor3uivEXT = (PFNGLSECONDARYCOLOR3UIVEXTPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3uivEXT")) == null) || r;
  r = ((glSecondaryColor3usEXT = (PFNGLSECONDARYCOLOR3USEXTPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3usEXT")) == null) || r;
  r = ((glSecondaryColor3usvEXT = (PFNGLSECONDARYCOLOR3USVEXTPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3usvEXT")) == null) || r;
  r = ((glSecondaryColorPointerEXT = (PFNGLSECONDARYCOLORPOINTEREXTPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColorPointerEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_secondary_color */

#ifdef GL_EXT_separate_shader_objects

static GLboolean _glewInit_GL_EXT_separate_shader_objects (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glActiveProgramEXT = (PFNGLACTIVEPROGRAMEXTPROC)glewGetProcAddress((const GLubyte*)"glActiveProgramEXT")) == null) || r;
  r = ((glCreateShaderProgramEXT = (PFNGLCREATESHADERPROGRAMEXTPROC)glewGetProcAddress((const GLubyte*)"glCreateShaderProgramEXT")) == null) || r;
  r = ((glUseShaderProgramEXT = (PFNGLUSESHADERPROGRAMEXTPROC)glewGetProcAddress((const GLubyte*)"glUseShaderProgramEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_separate_shader_objects */

#ifdef GL_EXT_separate_specular_color

#endif /* GL_EXT_separate_specular_color */

#ifdef GL_EXT_shadow_funcs

#endif /* GL_EXT_shadow_funcs */

#ifdef GL_EXT_shared_texture_palette

#endif /* GL_EXT_shared_texture_palette */

#ifdef GL_EXT_stencil_clear_tag

#endif /* GL_EXT_stencil_clear_tag */

#ifdef GL_EXT_stencil_two_side

static GLboolean _glewInit_GL_EXT_stencil_two_side (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glActiveStencilFaceEXT = (PFNGLACTIVESTENCILFACEEXTPROC)glewGetProcAddress((const GLubyte*)"glActiveStencilFaceEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_stencil_two_side */

#ifdef GL_EXT_stencil_wrap

#endif /* GL_EXT_stencil_wrap */

#ifdef GL_EXT_subtexture

static GLboolean _glewInit_GL_EXT_subtexture (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glTexSubImage1DEXT = (PFNGLTEXSUBIMAGE1DEXTPROC)glewGetProcAddress((const GLubyte*)"glTexSubImage1DEXT")) == null) || r;
  r = ((glTexSubImage2DEXT = (PFNGLTEXSUBIMAGE2DEXTPROC)glewGetProcAddress((const GLubyte*)"glTexSubImage2DEXT")) == null) || r;
  r = ((glTexSubImage3DEXT = (PFNGLTEXSUBIMAGE3DEXTPROC)glewGetProcAddress((const GLubyte*)"glTexSubImage3DEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_subtexture */

#ifdef GL_EXT_texture

#endif /* GL_EXT_texture */

#ifdef GL_EXT_texture3D

static GLboolean _glewInit_GL_EXT_texture3D (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glTexImage3DEXT = (PFNGLTEXIMAGE3DEXTPROC)glewGetProcAddress((const GLubyte*)"glTexImage3DEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_texture3D */

#ifdef GL_EXT_texture_array

#endif /* GL_EXT_texture_array */

#ifdef GL_EXT_texture_buffer_object

static GLboolean _glewInit_GL_EXT_texture_buffer_object (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glTexBufferEXT = (PFNGLTEXBUFFEREXTPROC)glewGetProcAddress((const GLubyte*)"glTexBufferEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_texture_buffer_object */

#ifdef GL_EXT_texture_compression_dxt1

#endif /* GL_EXT_texture_compression_dxt1 */

#ifdef GL_EXT_texture_compression_latc

#endif /* GL_EXT_texture_compression_latc */

#ifdef GL_EXT_texture_compression_rgtc

#endif /* GL_EXT_texture_compression_rgtc */

#ifdef GL_EXT_texture_compression_s3tc

#endif /* GL_EXT_texture_compression_s3tc */

#ifdef GL_EXT_texture_cube_map

#endif /* GL_EXT_texture_cube_map */

#ifdef GL_EXT_texture_edge_clamp

#endif /* GL_EXT_texture_edge_clamp */

#ifdef GL_EXT_texture_env

#endif /* GL_EXT_texture_env */

#ifdef GL_EXT_texture_env_add

#endif /* GL_EXT_texture_env_add */

#ifdef GL_EXT_texture_env_combine

#endif /* GL_EXT_texture_env_combine */

#ifdef GL_EXT_texture_env_dot3

#endif /* GL_EXT_texture_env_dot3 */

#ifdef GL_EXT_texture_filter_anisotropic

#endif /* GL_EXT_texture_filter_anisotropic */

#ifdef GL_EXT_texture_integer

static GLboolean _glewInit_GL_EXT_texture_integer (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glClearColorIiEXT = (PFNGLCLEARCOLORIIEXTPROC)glewGetProcAddress((const GLubyte*)"glClearColorIiEXT")) == null) || r;
  r = ((glClearColorIuiEXT = (PFNGLCLEARCOLORIUIEXTPROC)glewGetProcAddress((const GLubyte*)"glClearColorIuiEXT")) == null) || r;
  r = ((glGetTexParameterIivEXT = (PFNGLGETTEXPARAMETERIIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetTexParameterIivEXT")) == null) || r;
  r = ((glGetTexParameterIuivEXT = (PFNGLGETTEXPARAMETERIUIVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetTexParameterIuivEXT")) == null) || r;
  r = ((glTexParameterIivEXT = (PFNGLTEXPARAMETERIIVEXTPROC)glewGetProcAddress((const GLubyte*)"glTexParameterIivEXT")) == null) || r;
  r = ((glTexParameterIuivEXT = (PFNGLTEXPARAMETERIUIVEXTPROC)glewGetProcAddress((const GLubyte*)"glTexParameterIuivEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_texture_integer */

#ifdef GL_EXT_texture_lod_bias

#endif /* GL_EXT_texture_lod_bias */

#ifdef GL_EXT_texture_mirror_clamp

#endif /* GL_EXT_texture_mirror_clamp */

#ifdef GL_EXT_texture_object

static GLboolean _glewInit_GL_EXT_texture_object (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glAreTexturesResidentEXT = (PFNGLARETEXTURESRESIDENTEXTPROC)glewGetProcAddress((const GLubyte*)"glAreTexturesResidentEXT")) == null) || r;
  r = ((glBindTextureEXT = (PFNGLBINDTEXTUREEXTPROC)glewGetProcAddress((const GLubyte*)"glBindTextureEXT")) == null) || r;
  r = ((glDeleteTexturesEXT = (PFNGLDELETETEXTURESEXTPROC)glewGetProcAddress((const GLubyte*)"glDeleteTexturesEXT")) == null) || r;
  r = ((glGenTexturesEXT = (PFNGLGENTEXTURESEXTPROC)glewGetProcAddress((const GLubyte*)"glGenTexturesEXT")) == null) || r;
  r = ((glIsTextureEXT = (PFNGLISTEXTUREEXTPROC)glewGetProcAddress((const GLubyte*)"glIsTextureEXT")) == null) || r;
  r = ((glPrioritizeTexturesEXT = (PFNGLPRIORITIZETEXTURESEXTPROC)glewGetProcAddress((const GLubyte*)"glPrioritizeTexturesEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_texture_object */

#ifdef GL_EXT_texture_perturb_normal

static GLboolean _glewInit_GL_EXT_texture_perturb_normal (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glTextureNormalEXT = (PFNGLTEXTURENORMALEXTPROC)glewGetProcAddress((const GLubyte*)"glTextureNormalEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_texture_perturb_normal */

#ifdef GL_EXT_texture_rectangle

#endif /* GL_EXT_texture_rectangle */

#ifdef GL_EXT_texture_sRGB

#endif /* GL_EXT_texture_sRGB */

#ifdef GL_EXT_texture_shared_exponent

#endif /* GL_EXT_texture_shared_exponent */

#ifdef GL_EXT_texture_snorm

#endif /* GL_EXT_texture_snorm */

#ifdef GL_EXT_texture_swizzle

#endif /* GL_EXT_texture_swizzle */

#ifdef GL_EXT_timer_query

static GLboolean _glewInit_GL_EXT_timer_query (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glGetQueryObjecti64vEXT = (PFNGLGETQUERYOBJECTI64VEXTPROC)glewGetProcAddress((const GLubyte*)"glGetQueryObjecti64vEXT")) == null) || r;
  r = ((glGetQueryObjectui64vEXT = (PFNGLGETQUERYOBJECTUI64VEXTPROC)glewGetProcAddress((const GLubyte*)"glGetQueryObjectui64vEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_timer_query */

#ifdef GL_EXT_transform_feedback

static GLboolean _glewInit_GL_EXT_transform_feedback (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBeginTransformFeedbackEXT = (PFNGLBEGINTRANSFORMFEEDBACKEXTPROC)glewGetProcAddress((const GLubyte*)"glBeginTransformFeedbackEXT")) == null) || r;
  r = ((glBindBufferBaseEXT = (PFNGLBINDBUFFERBASEEXTPROC)glewGetProcAddress((const GLubyte*)"glBindBufferBaseEXT")) == null) || r;
  r = ((glBindBufferOffsetEXT = (PFNGLBINDBUFFEROFFSETEXTPROC)glewGetProcAddress((const GLubyte*)"glBindBufferOffsetEXT")) == null) || r;
  r = ((glBindBufferRangeEXT = (PFNGLBINDBUFFERRANGEEXTPROC)glewGetProcAddress((const GLubyte*)"glBindBufferRangeEXT")) == null) || r;
  r = ((glEndTransformFeedbackEXT = (PFNGLENDTRANSFORMFEEDBACKEXTPROC)glewGetProcAddress((const GLubyte*)"glEndTransformFeedbackEXT")) == null) || r;
  r = ((glGetTransformFeedbackVaryingEXT = (PFNGLGETTRANSFORMFEEDBACKVARYINGEXTPROC)glewGetProcAddress((const GLubyte*)"glGetTransformFeedbackVaryingEXT")) == null) || r;
  r = ((glTransformFeedbackVaryingsEXT = (PFNGLTRANSFORMFEEDBACKVARYINGSEXTPROC)glewGetProcAddress((const GLubyte*)"glTransformFeedbackVaryingsEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_transform_feedback */

#ifdef GL_EXT_vertex_array

static GLboolean _glewInit_GL_EXT_vertex_array (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glArrayElementEXT = (PFNGLARRAYELEMENTEXTPROC)glewGetProcAddress((const GLubyte*)"glArrayElementEXT")) == null) || r;
  r = ((glColorPointerEXT = (PFNGLCOLORPOINTEREXTPROC)glewGetProcAddress((const GLubyte*)"glColorPointerEXT")) == null) || r;
  r = ((glDrawArraysEXT = (PFNGLDRAWARRAYSEXTPROC)glewGetProcAddress((const GLubyte*)"glDrawArraysEXT")) == null) || r;
  r = ((glEdgeFlagPointerEXT = (PFNGLEDGEFLAGPOINTEREXTPROC)glewGetProcAddress((const GLubyte*)"glEdgeFlagPointerEXT")) == null) || r;
  r = ((glGetPointervEXT = (PFNGLGETPOINTERVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetPointervEXT")) == null) || r;
  r = ((glIndexPointerEXT = (PFNGLINDEXPOINTEREXTPROC)glewGetProcAddress((const GLubyte*)"glIndexPointerEXT")) == null) || r;
  r = ((glNormalPointerEXT = (PFNGLNORMALPOINTEREXTPROC)glewGetProcAddress((const GLubyte*)"glNormalPointerEXT")) == null) || r;
  r = ((glTexCoordPointerEXT = (PFNGLTEXCOORDPOINTEREXTPROC)glewGetProcAddress((const GLubyte*)"glTexCoordPointerEXT")) == null) || r;
  r = ((glVertexPointerEXT = (PFNGLVERTEXPOINTEREXTPROC)glewGetProcAddress((const GLubyte*)"glVertexPointerEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_vertex_array */

#ifdef GL_EXT_vertex_array_bgra

#endif /* GL_EXT_vertex_array_bgra */

#ifdef GL_EXT_vertex_shader

static GLboolean _glewInit_GL_EXT_vertex_shader (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBeginVertexShaderEXT = (PFNGLBEGINVERTEXSHADEREXTPROC)glewGetProcAddress((const GLubyte*)"glBeginVertexShaderEXT")) == null) || r;
  r = ((glBindLightParameterEXT = (PFNGLBINDLIGHTPARAMETEREXTPROC)glewGetProcAddress((const GLubyte*)"glBindLightParameterEXT")) == null) || r;
  r = ((glBindMaterialParameterEXT = (PFNGLBINDMATERIALPARAMETEREXTPROC)glewGetProcAddress((const GLubyte*)"glBindMaterialParameterEXT")) == null) || r;
  r = ((glBindParameterEXT = (PFNGLBINDPARAMETEREXTPROC)glewGetProcAddress((const GLubyte*)"glBindParameterEXT")) == null) || r;
  r = ((glBindTexGenParameterEXT = (PFNGLBINDTEXGENPARAMETEREXTPROC)glewGetProcAddress((const GLubyte*)"glBindTexGenParameterEXT")) == null) || r;
  r = ((glBindTextureUnitParameterEXT = (PFNGLBINDTEXTUREUNITPARAMETEREXTPROC)glewGetProcAddress((const GLubyte*)"glBindTextureUnitParameterEXT")) == null) || r;
  r = ((glBindVertexShaderEXT = (PFNGLBINDVERTEXSHADEREXTPROC)glewGetProcAddress((const GLubyte*)"glBindVertexShaderEXT")) == null) || r;
  r = ((glDeleteVertexShaderEXT = (PFNGLDELETEVERTEXSHADEREXTPROC)glewGetProcAddress((const GLubyte*)"glDeleteVertexShaderEXT")) == null) || r;
  r = ((glDisableVariantClientStateEXT = (PFNGLDISABLEVARIANTCLIENTSTATEEXTPROC)glewGetProcAddress((const GLubyte*)"glDisableVariantClientStateEXT")) == null) || r;
  r = ((glEnableVariantClientStateEXT = (PFNGLENABLEVARIANTCLIENTSTATEEXTPROC)glewGetProcAddress((const GLubyte*)"glEnableVariantClientStateEXT")) == null) || r;
  r = ((glEndVertexShaderEXT = (PFNGLENDVERTEXSHADEREXTPROC)glewGetProcAddress((const GLubyte*)"glEndVertexShaderEXT")) == null) || r;
  r = ((glExtractComponentEXT = (PFNGLEXTRACTCOMPONENTEXTPROC)glewGetProcAddress((const GLubyte*)"glExtractComponentEXT")) == null) || r;
  r = ((glGenSymbolsEXT = (PFNGLGENSYMBOLSEXTPROC)glewGetProcAddress((const GLubyte*)"glGenSymbolsEXT")) == null) || r;
  r = ((glGenVertexShadersEXT = (PFNGLGENVERTEXSHADERSEXTPROC)glewGetProcAddress((const GLubyte*)"glGenVertexShadersEXT")) == null) || r;
  r = ((glGetInvariantBooleanvEXT = (PFNGLGETINVARIANTBOOLEANVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetInvariantBooleanvEXT")) == null) || r;
  r = ((glGetInvariantFloatvEXT = (PFNGLGETINVARIANTFLOATVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetInvariantFloatvEXT")) == null) || r;
  r = ((glGetInvariantIntegervEXT = (PFNGLGETINVARIANTINTEGERVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetInvariantIntegervEXT")) == null) || r;
  r = ((glGetLocalConstantBooleanvEXT = (PFNGLGETLOCALCONSTANTBOOLEANVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetLocalConstantBooleanvEXT")) == null) || r;
  r = ((glGetLocalConstantFloatvEXT = (PFNGLGETLOCALCONSTANTFLOATVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetLocalConstantFloatvEXT")) == null) || r;
  r = ((glGetLocalConstantIntegervEXT = (PFNGLGETLOCALCONSTANTINTEGERVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetLocalConstantIntegervEXT")) == null) || r;
  r = ((glGetVariantBooleanvEXT = (PFNGLGETVARIANTBOOLEANVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetVariantBooleanvEXT")) == null) || r;
  r = ((glGetVariantFloatvEXT = (PFNGLGETVARIANTFLOATVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetVariantFloatvEXT")) == null) || r;
  r = ((glGetVariantIntegervEXT = (PFNGLGETVARIANTINTEGERVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetVariantIntegervEXT")) == null) || r;
  r = ((glGetVariantPointervEXT = (PFNGLGETVARIANTPOINTERVEXTPROC)glewGetProcAddress((const GLubyte*)"glGetVariantPointervEXT")) == null) || r;
  r = ((glInsertComponentEXT = (PFNGLINSERTCOMPONENTEXTPROC)glewGetProcAddress((const GLubyte*)"glInsertComponentEXT")) == null) || r;
  r = ((glIsVariantEnabledEXT = (PFNGLISVARIANTENABLEDEXTPROC)glewGetProcAddress((const GLubyte*)"glIsVariantEnabledEXT")) == null) || r;
  r = ((glSetInvariantEXT = (PFNGLSETINVARIANTEXTPROC)glewGetProcAddress((const GLubyte*)"glSetInvariantEXT")) == null) || r;
  r = ((glSetLocalConstantEXT = (PFNGLSETLOCALCONSTANTEXTPROC)glewGetProcAddress((const GLubyte*)"glSetLocalConstantEXT")) == null) || r;
  r = ((glShaderOp1EXT = (PFNGLSHADEROP1EXTPROC)glewGetProcAddress((const GLubyte*)"glShaderOp1EXT")) == null) || r;
  r = ((glShaderOp2EXT = (PFNGLSHADEROP2EXTPROC)glewGetProcAddress((const GLubyte*)"glShaderOp2EXT")) == null) || r;
  r = ((glShaderOp3EXT = (PFNGLSHADEROP3EXTPROC)glewGetProcAddress((const GLubyte*)"glShaderOp3EXT")) == null) || r;
  r = ((glSwizzleEXT = (PFNGLSWIZZLEEXTPROC)glewGetProcAddress((const GLubyte*)"glSwizzleEXT")) == null) || r;
  r = ((glVariantPointerEXT = (PFNGLVARIANTPOINTEREXTPROC)glewGetProcAddress((const GLubyte*)"glVariantPointerEXT")) == null) || r;
  r = ((glVariantbvEXT = (PFNGLVARIANTBVEXTPROC)glewGetProcAddress((const GLubyte*)"glVariantbvEXT")) == null) || r;
  r = ((glVariantdvEXT = (PFNGLVARIANTDVEXTPROC)glewGetProcAddress((const GLubyte*)"glVariantdvEXT")) == null) || r;
  r = ((glVariantfvEXT = (PFNGLVARIANTFVEXTPROC)glewGetProcAddress((const GLubyte*)"glVariantfvEXT")) == null) || r;
  r = ((glVariantivEXT = (PFNGLVARIANTIVEXTPROC)glewGetProcAddress((const GLubyte*)"glVariantivEXT")) == null) || r;
  r = ((glVariantsvEXT = (PFNGLVARIANTSVEXTPROC)glewGetProcAddress((const GLubyte*)"glVariantsvEXT")) == null) || r;
  r = ((glVariantubvEXT = (PFNGLVARIANTUBVEXTPROC)glewGetProcAddress((const GLubyte*)"glVariantubvEXT")) == null) || r;
  r = ((glVariantuivEXT = (PFNGLVARIANTUIVEXTPROC)glewGetProcAddress((const GLubyte*)"glVariantuivEXT")) == null) || r;
  r = ((glVariantusvEXT = (PFNGLVARIANTUSVEXTPROC)glewGetProcAddress((const GLubyte*)"glVariantusvEXT")) == null) || r;
  r = ((glWriteMaskEXT = (PFNGLWRITEMASKEXTPROC)glewGetProcAddress((const GLubyte*)"glWriteMaskEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_vertex_shader */

#ifdef GL_EXT_vertex_weighting

static GLboolean _glewInit_GL_EXT_vertex_weighting (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glVertexWeightPointerEXT = (PFNGLVERTEXWEIGHTPOINTEREXTPROC)glewGetProcAddress((const GLubyte*)"glVertexWeightPointerEXT")) == null) || r;
  r = ((glVertexWeightfEXT = (PFNGLVERTEXWEIGHTFEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexWeightfEXT")) == null) || r;
  r = ((glVertexWeightfvEXT = (PFNGLVERTEXWEIGHTFVEXTPROC)glewGetProcAddress((const GLubyte*)"glVertexWeightfvEXT")) == null) || r;

  return r;
}

#endif /* GL_EXT_vertex_weighting */

#ifdef GL_GREMEDY_frame_terminator

static GLboolean _glewInit_GL_GREMEDY_frame_terminator (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glFrameTerminatorGREMEDY = (PFNGLFRAMETERMINATORGREMEDYPROC)glewGetProcAddress((const GLubyte*)"glFrameTerminatorGREMEDY")) == null) || r;

  return r;
}

#endif /* GL_GREMEDY_frame_terminator */

#ifdef GL_GREMEDY_string_marker

static GLboolean _glewInit_GL_GREMEDY_string_marker (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glStringMarkerGREMEDY = (PFNGLSTRINGMARKERGREMEDYPROC)glewGetProcAddress((const GLubyte*)"glStringMarkerGREMEDY")) == null) || r;

  return r;
}

#endif /* GL_GREMEDY_string_marker */

#ifdef GL_HP_convolution_border_modes

#endif /* GL_HP_convolution_border_modes */

#ifdef GL_HP_image_transform

static GLboolean _glewInit_GL_HP_image_transform (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glGetImageTransformParameterfvHP = (PFNGLGETIMAGETRANSFORMPARAMETERFVHPPROC)glewGetProcAddress((const GLubyte*)"glGetImageTransformParameterfvHP")) == null) || r;
  r = ((glGetImageTransformParameterivHP = (PFNGLGETIMAGETRANSFORMPARAMETERIVHPPROC)glewGetProcAddress((const GLubyte*)"glGetImageTransformParameterivHP")) == null) || r;
  r = ((glImageTransformParameterfHP = (PFNGLIMAGETRANSFORMPARAMETERFHPPROC)glewGetProcAddress((const GLubyte*)"glImageTransformParameterfHP")) == null) || r;
  r = ((glImageTransformParameterfvHP = (PFNGLIMAGETRANSFORMPARAMETERFVHPPROC)glewGetProcAddress((const GLubyte*)"glImageTransformParameterfvHP")) == null) || r;
  r = ((glImageTransformParameteriHP = (PFNGLIMAGETRANSFORMPARAMETERIHPPROC)glewGetProcAddress((const GLubyte*)"glImageTransformParameteriHP")) == null) || r;
  r = ((glImageTransformParameterivHP = (PFNGLIMAGETRANSFORMPARAMETERIVHPPROC)glewGetProcAddress((const GLubyte*)"glImageTransformParameterivHP")) == null) || r;

  return r;
}

#endif /* GL_HP_image_transform */

#ifdef GL_HP_occlusion_test

#endif /* GL_HP_occlusion_test */

#ifdef GL_HP_texture_lighting

#endif /* GL_HP_texture_lighting */

#ifdef GL_IBM_cull_vertex

#endif /* GL_IBM_cull_vertex */

#ifdef GL_IBM_multimode_draw_arrays

static GLboolean _glewInit_GL_IBM_multimode_draw_arrays (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glMultiModeDrawArraysIBM = (PFNGLMULTIMODEDRAWARRAYSIBMPROC)glewGetProcAddress((const GLubyte*)"glMultiModeDrawArraysIBM")) == null) || r;
  r = ((glMultiModeDrawElementsIBM = (PFNGLMULTIMODEDRAWELEMENTSIBMPROC)glewGetProcAddress((const GLubyte*)"glMultiModeDrawElementsIBM")) == null) || r;

  return r;
}

#endif /* GL_IBM_multimode_draw_arrays */

#ifdef GL_IBM_rasterpos_clip

#endif /* GL_IBM_rasterpos_clip */

#ifdef GL_IBM_static_data

#endif /* GL_IBM_static_data */

#ifdef GL_IBM_texture_mirrored_repeat

#endif /* GL_IBM_texture_mirrored_repeat */

#ifdef GL_IBM_vertex_array_lists

static GLboolean _glewInit_GL_IBM_vertex_array_lists (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glColorPointerListIBM = (PFNGLCOLORPOINTERLISTIBMPROC)glewGetProcAddress((const GLubyte*)"glColorPointerListIBM")) == null) || r;
  r = ((glEdgeFlagPointerListIBM = (PFNGLEDGEFLAGPOINTERLISTIBMPROC)glewGetProcAddress((const GLubyte*)"glEdgeFlagPointerListIBM")) == null) || r;
  r = ((glFogCoordPointerListIBM = (PFNGLFOGCOORDPOINTERLISTIBMPROC)glewGetProcAddress((const GLubyte*)"glFogCoordPointerListIBM")) == null) || r;
  r = ((glIndexPointerListIBM = (PFNGLINDEXPOINTERLISTIBMPROC)glewGetProcAddress((const GLubyte*)"glIndexPointerListIBM")) == null) || r;
  r = ((glNormalPointerListIBM = (PFNGLNORMALPOINTERLISTIBMPROC)glewGetProcAddress((const GLubyte*)"glNormalPointerListIBM")) == null) || r;
  r = ((glSecondaryColorPointerListIBM = (PFNGLSECONDARYCOLORPOINTERLISTIBMPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColorPointerListIBM")) == null) || r;
  r = ((glTexCoordPointerListIBM = (PFNGLTEXCOORDPOINTERLISTIBMPROC)glewGetProcAddress((const GLubyte*)"glTexCoordPointerListIBM")) == null) || r;
  r = ((glVertexPointerListIBM = (PFNGLVERTEXPOINTERLISTIBMPROC)glewGetProcAddress((const GLubyte*)"glVertexPointerListIBM")) == null) || r;

  return r;
}

#endif /* GL_IBM_vertex_array_lists */

#ifdef GL_INGR_color_clamp

#endif /* GL_INGR_color_clamp */

#ifdef GL_INGR_interlace_read

#endif /* GL_INGR_interlace_read */

#ifdef GL_INTEL_parallel_arrays

static GLboolean _glewInit_GL_INTEL_parallel_arrays (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glColorPointervINTEL = (PFNGLCOLORPOINTERVINTELPROC)glewGetProcAddress((const GLubyte*)"glColorPointervINTEL")) == null) || r;
  r = ((glNormalPointervINTEL = (PFNGLNORMALPOINTERVINTELPROC)glewGetProcAddress((const GLubyte*)"glNormalPointervINTEL")) == null) || r;
  r = ((glTexCoordPointervINTEL = (PFNGLTEXCOORDPOINTERVINTELPROC)glewGetProcAddress((const GLubyte*)"glTexCoordPointervINTEL")) == null) || r;
  r = ((glVertexPointervINTEL = (PFNGLVERTEXPOINTERVINTELPROC)glewGetProcAddress((const GLubyte*)"glVertexPointervINTEL")) == null) || r;

  return r;
}

#endif /* GL_INTEL_parallel_arrays */

#ifdef GL_INTEL_texture_scissor

static GLboolean _glewInit_GL_INTEL_texture_scissor (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glTexScissorFuncINTEL = (PFNGLTEXSCISSORFUNCINTELPROC)glewGetProcAddress((const GLubyte*)"glTexScissorFuncINTEL")) == null) || r;
  r = ((glTexScissorINTEL = (PFNGLTEXSCISSORINTELPROC)glewGetProcAddress((const GLubyte*)"glTexScissorINTEL")) == null) || r;

  return r;
}

#endif /* GL_INTEL_texture_scissor */

#ifdef GL_KTX_buffer_region

static GLboolean _glewInit_GL_KTX_buffer_region (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBufferRegionEnabledEXT = (PFNGLBUFFERREGIONENABLEDEXTPROC)glewGetProcAddress((const GLubyte*)"glBufferRegionEnabledEXT")) == null) || r;
  r = ((glDeleteBufferRegionEXT = (PFNGLDELETEBUFFERREGIONEXTPROC)glewGetProcAddress((const GLubyte*)"glDeleteBufferRegionEXT")) == null) || r;
  r = ((glDrawBufferRegionEXT = (PFNGLDRAWBUFFERREGIONEXTPROC)glewGetProcAddress((const GLubyte*)"glDrawBufferRegionEXT")) == null) || r;
  r = ((glNewBufferRegionEXT = (PFNGLNEWBUFFERREGIONEXTPROC)glewGetProcAddress((const GLubyte*)"glNewBufferRegionEXT")) == null) || r;
  r = ((glReadBufferRegionEXT = (PFNGLREADBUFFERREGIONEXTPROC)glewGetProcAddress((const GLubyte*)"glReadBufferRegionEXT")) == null) || r;

  return r;
}

#endif /* GL_KTX_buffer_region */

#ifdef GL_MESAX_texture_stack

#endif /* GL_MESAX_texture_stack */

#ifdef GL_MESA_pack_invert

#endif /* GL_MESA_pack_invert */

#ifdef GL_MESA_resize_buffers

static GLboolean _glewInit_GL_MESA_resize_buffers (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glResizeBuffersMESA = (PFNGLRESIZEBUFFERSMESAPROC)glewGetProcAddress((const GLubyte*)"glResizeBuffersMESA")) == null) || r;

  return r;
}

#endif /* GL_MESA_resize_buffers */

#ifdef GL_MESA_window_pos

static GLboolean _glewInit_GL_MESA_window_pos (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glWindowPos2dMESA = (PFNGLWINDOWPOS2DMESAPROC)glewGetProcAddress((const GLubyte*)"glWindowPos2dMESA")) == null) || r;
  r = ((glWindowPos2dvMESA = (PFNGLWINDOWPOS2DVMESAPROC)glewGetProcAddress((const GLubyte*)"glWindowPos2dvMESA")) == null) || r;
  r = ((glWindowPos2fMESA = (PFNGLWINDOWPOS2FMESAPROC)glewGetProcAddress((const GLubyte*)"glWindowPos2fMESA")) == null) || r;
  r = ((glWindowPos2fvMESA = (PFNGLWINDOWPOS2FVMESAPROC)glewGetProcAddress((const GLubyte*)"glWindowPos2fvMESA")) == null) || r;
  r = ((glWindowPos2iMESA = (PFNGLWINDOWPOS2IMESAPROC)glewGetProcAddress((const GLubyte*)"glWindowPos2iMESA")) == null) || r;
  r = ((glWindowPos2ivMESA = (PFNGLWINDOWPOS2IVMESAPROC)glewGetProcAddress((const GLubyte*)"glWindowPos2ivMESA")) == null) || r;
  r = ((glWindowPos2sMESA = (PFNGLWINDOWPOS2SMESAPROC)glewGetProcAddress((const GLubyte*)"glWindowPos2sMESA")) == null) || r;
  r = ((glWindowPos2svMESA = (PFNGLWINDOWPOS2SVMESAPROC)glewGetProcAddress((const GLubyte*)"glWindowPos2svMESA")) == null) || r;
  r = ((glWindowPos3dMESA = (PFNGLWINDOWPOS3DMESAPROC)glewGetProcAddress((const GLubyte*)"glWindowPos3dMESA")) == null) || r;
  r = ((glWindowPos3dvMESA = (PFNGLWINDOWPOS3DVMESAPROC)glewGetProcAddress((const GLubyte*)"glWindowPos3dvMESA")) == null) || r;
  r = ((glWindowPos3fMESA = (PFNGLWINDOWPOS3FMESAPROC)glewGetProcAddress((const GLubyte*)"glWindowPos3fMESA")) == null) || r;
  r = ((glWindowPos3fvMESA = (PFNGLWINDOWPOS3FVMESAPROC)glewGetProcAddress((const GLubyte*)"glWindowPos3fvMESA")) == null) || r;
  r = ((glWindowPos3iMESA = (PFNGLWINDOWPOS3IMESAPROC)glewGetProcAddress((const GLubyte*)"glWindowPos3iMESA")) == null) || r;
  r = ((glWindowPos3ivMESA = (PFNGLWINDOWPOS3IVMESAPROC)glewGetProcAddress((const GLubyte*)"glWindowPos3ivMESA")) == null) || r;
  r = ((glWindowPos3sMESA = (PFNGLWINDOWPOS3SMESAPROC)glewGetProcAddress((const GLubyte*)"glWindowPos3sMESA")) == null) || r;
  r = ((glWindowPos3svMESA = (PFNGLWINDOWPOS3SVMESAPROC)glewGetProcAddress((const GLubyte*)"glWindowPos3svMESA")) == null) || r;
  r = ((glWindowPos4dMESA = (PFNGLWINDOWPOS4DMESAPROC)glewGetProcAddress((const GLubyte*)"glWindowPos4dMESA")) == null) || r;
  r = ((glWindowPos4dvMESA = (PFNGLWINDOWPOS4DVMESAPROC)glewGetProcAddress((const GLubyte*)"glWindowPos4dvMESA")) == null) || r;
  r = ((glWindowPos4fMESA = (PFNGLWINDOWPOS4FMESAPROC)glewGetProcAddress((const GLubyte*)"glWindowPos4fMESA")) == null) || r;
  r = ((glWindowPos4fvMESA = (PFNGLWINDOWPOS4FVMESAPROC)glewGetProcAddress((const GLubyte*)"glWindowPos4fvMESA")) == null) || r;
  r = ((glWindowPos4iMESA = (PFNGLWINDOWPOS4IMESAPROC)glewGetProcAddress((const GLubyte*)"glWindowPos4iMESA")) == null) || r;
  r = ((glWindowPos4ivMESA = (PFNGLWINDOWPOS4IVMESAPROC)glewGetProcAddress((const GLubyte*)"glWindowPos4ivMESA")) == null) || r;
  r = ((glWindowPos4sMESA = (PFNGLWINDOWPOS4SMESAPROC)glewGetProcAddress((const GLubyte*)"glWindowPos4sMESA")) == null) || r;
  r = ((glWindowPos4svMESA = (PFNGLWINDOWPOS4SVMESAPROC)glewGetProcAddress((const GLubyte*)"glWindowPos4svMESA")) == null) || r;

  return r;
}

#endif /* GL_MESA_window_pos */

#ifdef GL_MESA_ycbcr_texture

#endif /* GL_MESA_ycbcr_texture */

#ifdef GL_NV_blend_square

#endif /* GL_NV_blend_square */

#ifdef GL_NV_conditional_render

static GLboolean _glewInit_GL_NV_conditional_render (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBeginConditionalRenderNV = (PFNGLBEGINCONDITIONALRENDERNVPROC)glewGetProcAddress((const GLubyte*)"glBeginConditionalRenderNV")) == null) || r;
  r = ((glEndConditionalRenderNV = (PFNGLENDCONDITIONALRENDERNVPROC)glewGetProcAddress((const GLubyte*)"glEndConditionalRenderNV")) == null) || r;

  return r;
}

#endif /* GL_NV_conditional_render */

#ifdef GL_NV_copy_depth_to_color

#endif /* GL_NV_copy_depth_to_color */

#ifdef GL_NV_copy_image

static GLboolean _glewInit_GL_NV_copy_image (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glCopyImageSubDataNV = (PFNGLCOPYIMAGESUBDATANVPROC)glewGetProcAddress((const GLubyte*)"glCopyImageSubDataNV")) == null) || r;

  return r;
}

#endif /* GL_NV_copy_image */

#ifdef GL_NV_depth_buffer_float

static GLboolean _glewInit_GL_NV_depth_buffer_float (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glClearDepthdNV = (PFNGLCLEARDEPTHDNVPROC)glewGetProcAddress((const GLubyte*)"glClearDepthdNV")) == null) || r;
  r = ((glDepthBoundsdNV = (PFNGLDEPTHBOUNDSDNVPROC)glewGetProcAddress((const GLubyte*)"glDepthBoundsdNV")) == null) || r;
  r = ((glDepthRangedNV = (PFNGLDEPTHRANGEDNVPROC)glewGetProcAddress((const GLubyte*)"glDepthRangedNV")) == null) || r;

  return r;
}

#endif /* GL_NV_depth_buffer_float */

#ifdef GL_NV_depth_clamp

#endif /* GL_NV_depth_clamp */

#ifdef GL_NV_depth_range_unclamped

#endif /* GL_NV_depth_range_unclamped */

#ifdef GL_NV_evaluators

static GLboolean _glewInit_GL_NV_evaluators (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glEvalMapsNV = (PFNGLEVALMAPSNVPROC)glewGetProcAddress((const GLubyte*)"glEvalMapsNV")) == null) || r;
  r = ((glGetMapAttribParameterfvNV = (PFNGLGETMAPATTRIBPARAMETERFVNVPROC)glewGetProcAddress((const GLubyte*)"glGetMapAttribParameterfvNV")) == null) || r;
  r = ((glGetMapAttribParameterivNV = (PFNGLGETMAPATTRIBPARAMETERIVNVPROC)glewGetProcAddress((const GLubyte*)"glGetMapAttribParameterivNV")) == null) || r;
  r = ((glGetMapControlPointsNV = (PFNGLGETMAPCONTROLPOINTSNVPROC)glewGetProcAddress((const GLubyte*)"glGetMapControlPointsNV")) == null) || r;
  r = ((glGetMapParameterfvNV = (PFNGLGETMAPPARAMETERFVNVPROC)glewGetProcAddress((const GLubyte*)"glGetMapParameterfvNV")) == null) || r;
  r = ((glGetMapParameterivNV = (PFNGLGETMAPPARAMETERIVNVPROC)glewGetProcAddress((const GLubyte*)"glGetMapParameterivNV")) == null) || r;
  r = ((glMapControlPointsNV = (PFNGLMAPCONTROLPOINTSNVPROC)glewGetProcAddress((const GLubyte*)"glMapControlPointsNV")) == null) || r;
  r = ((glMapParameterfvNV = (PFNGLMAPPARAMETERFVNVPROC)glewGetProcAddress((const GLubyte*)"glMapParameterfvNV")) == null) || r;
  r = ((glMapParameterivNV = (PFNGLMAPPARAMETERIVNVPROC)glewGetProcAddress((const GLubyte*)"glMapParameterivNV")) == null) || r;

  return r;
}

#endif /* GL_NV_evaluators */

#ifdef GL_NV_explicit_multisample

static GLboolean _glewInit_GL_NV_explicit_multisample (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glGetMultisamplefvNV = (PFNGLGETMULTISAMPLEFVNVPROC)glewGetProcAddress((const GLubyte*)"glGetMultisamplefvNV")) == null) || r;
  r = ((glSampleMaskIndexedNV = (PFNGLSAMPLEMASKINDEXEDNVPROC)glewGetProcAddress((const GLubyte*)"glSampleMaskIndexedNV")) == null) || r;
  r = ((glTexRenderbufferNV = (PFNGLTEXRENDERBUFFERNVPROC)glewGetProcAddress((const GLubyte*)"glTexRenderbufferNV")) == null) || r;

  return r;
}

#endif /* GL_NV_explicit_multisample */

#ifdef GL_NV_fence

static GLboolean _glewInit_GL_NV_fence (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glDeleteFencesNV = (PFNGLDELETEFENCESNVPROC)glewGetProcAddress((const GLubyte*)"glDeleteFencesNV")) == null) || r;
  r = ((glFinishFenceNV = (PFNGLFINISHFENCENVPROC)glewGetProcAddress((const GLubyte*)"glFinishFenceNV")) == null) || r;
  r = ((glGenFencesNV = (PFNGLGENFENCESNVPROC)glewGetProcAddress((const GLubyte*)"glGenFencesNV")) == null) || r;
  r = ((glGetFenceivNV = (PFNGLGETFENCEIVNVPROC)glewGetProcAddress((const GLubyte*)"glGetFenceivNV")) == null) || r;
  r = ((glIsFenceNV = (PFNGLISFENCENVPROC)glewGetProcAddress((const GLubyte*)"glIsFenceNV")) == null) || r;
  r = ((glSetFenceNV = (PFNGLSETFENCENVPROC)glewGetProcAddress((const GLubyte*)"glSetFenceNV")) == null) || r;
  r = ((glTestFenceNV = (PFNGLTESTFENCENVPROC)glewGetProcAddress((const GLubyte*)"glTestFenceNV")) == null) || r;

  return r;
}

#endif /* GL_NV_fence */

#ifdef GL_NV_float_buffer

#endif /* GL_NV_float_buffer */

#ifdef GL_NV_fog_distance

#endif /* GL_NV_fog_distance */

#ifdef GL_NV_fragment_program

static GLboolean _glewInit_GL_NV_fragment_program (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glGetProgramNamedParameterdvNV = (PFNGLGETPROGRAMNAMEDPARAMETERDVNVPROC)glewGetProcAddress((const GLubyte*)"glGetProgramNamedParameterdvNV")) == null) || r;
  r = ((glGetProgramNamedParameterfvNV = (PFNGLGETPROGRAMNAMEDPARAMETERFVNVPROC)glewGetProcAddress((const GLubyte*)"glGetProgramNamedParameterfvNV")) == null) || r;
  r = ((glProgramNamedParameter4dNV = (PFNGLPROGRAMNAMEDPARAMETER4DNVPROC)glewGetProcAddress((const GLubyte*)"glProgramNamedParameter4dNV")) == null) || r;
  r = ((glProgramNamedParameter4dvNV = (PFNGLPROGRAMNAMEDPARAMETER4DVNVPROC)glewGetProcAddress((const GLubyte*)"glProgramNamedParameter4dvNV")) == null) || r;
  r = ((glProgramNamedParameter4fNV = (PFNGLPROGRAMNAMEDPARAMETER4FNVPROC)glewGetProcAddress((const GLubyte*)"glProgramNamedParameter4fNV")) == null) || r;
  r = ((glProgramNamedParameter4fvNV = (PFNGLPROGRAMNAMEDPARAMETER4FVNVPROC)glewGetProcAddress((const GLubyte*)"glProgramNamedParameter4fvNV")) == null) || r;

  return r;
}

#endif /* GL_NV_fragment_program */

#ifdef GL_NV_fragment_program2

#endif /* GL_NV_fragment_program2 */

#ifdef GL_NV_fragment_program4

#endif /* GL_NV_fragment_program4 */

#ifdef GL_NV_fragment_program_option

#endif /* GL_NV_fragment_program_option */

#ifdef GL_NV_framebuffer_multisample_coverage

static GLboolean _glewInit_GL_NV_framebuffer_multisample_coverage (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glRenderbufferStorageMultisampleCoverageNV = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC)glewGetProcAddress((const GLubyte*)"glRenderbufferStorageMultisampleCoverageNV")) == null) || r;

  return r;
}

#endif /* GL_NV_framebuffer_multisample_coverage */

#ifdef GL_NV_geometry_program4

static GLboolean _glewInit_GL_NV_geometry_program4 (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glProgramVertexLimitNV = (PFNGLPROGRAMVERTEXLIMITNVPROC)glewGetProcAddress((const GLubyte*)"glProgramVertexLimitNV")) == null) || r;

  return r;
}

#endif /* GL_NV_geometry_program4 */

#ifdef GL_NV_geometry_shader4

#endif /* GL_NV_geometry_shader4 */

#ifdef GL_NV_gpu_program4

static GLboolean _glewInit_GL_NV_gpu_program4 (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glProgramEnvParameterI4iNV = (PFNGLPROGRAMENVPARAMETERI4INVPROC)glewGetProcAddress((const GLubyte*)"glProgramEnvParameterI4iNV")) == null) || r;
  r = ((glProgramEnvParameterI4ivNV = (PFNGLPROGRAMENVPARAMETERI4IVNVPROC)glewGetProcAddress((const GLubyte*)"glProgramEnvParameterI4ivNV")) == null) || r;
  r = ((glProgramEnvParameterI4uiNV = (PFNGLPROGRAMENVPARAMETERI4UINVPROC)glewGetProcAddress((const GLubyte*)"glProgramEnvParameterI4uiNV")) == null) || r;
  r = ((glProgramEnvParameterI4uivNV = (PFNGLPROGRAMENVPARAMETERI4UIVNVPROC)glewGetProcAddress((const GLubyte*)"glProgramEnvParameterI4uivNV")) == null) || r;
  r = ((glProgramEnvParametersI4ivNV = (PFNGLPROGRAMENVPARAMETERSI4IVNVPROC)glewGetProcAddress((const GLubyte*)"glProgramEnvParametersI4ivNV")) == null) || r;
  r = ((glProgramEnvParametersI4uivNV = (PFNGLPROGRAMENVPARAMETERSI4UIVNVPROC)glewGetProcAddress((const GLubyte*)"glProgramEnvParametersI4uivNV")) == null) || r;
  r = ((glProgramLocalParameterI4iNV = (PFNGLPROGRAMLOCALPARAMETERI4INVPROC)glewGetProcAddress((const GLubyte*)"glProgramLocalParameterI4iNV")) == null) || r;
  r = ((glProgramLocalParameterI4ivNV = (PFNGLPROGRAMLOCALPARAMETERI4IVNVPROC)glewGetProcAddress((const GLubyte*)"glProgramLocalParameterI4ivNV")) == null) || r;
  r = ((glProgramLocalParameterI4uiNV = (PFNGLPROGRAMLOCALPARAMETERI4UINVPROC)glewGetProcAddress((const GLubyte*)"glProgramLocalParameterI4uiNV")) == null) || r;
  r = ((glProgramLocalParameterI4uivNV = (PFNGLPROGRAMLOCALPARAMETERI4UIVNVPROC)glewGetProcAddress((const GLubyte*)"glProgramLocalParameterI4uivNV")) == null) || r;
  r = ((glProgramLocalParametersI4ivNV = (PFNGLPROGRAMLOCALPARAMETERSI4IVNVPROC)glewGetProcAddress((const GLubyte*)"glProgramLocalParametersI4ivNV")) == null) || r;
  r = ((glProgramLocalParametersI4uivNV = (PFNGLPROGRAMLOCALPARAMETERSI4UIVNVPROC)glewGetProcAddress((const GLubyte*)"glProgramLocalParametersI4uivNV")) == null) || r;

  return r;
}

#endif /* GL_NV_gpu_program4 */

#ifdef GL_NV_half_float

static GLboolean _glewInit_GL_NV_half_float (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glColor3hNV = (PFNGLCOLOR3HNVPROC)glewGetProcAddress((const GLubyte*)"glColor3hNV")) == null) || r;
  r = ((glColor3hvNV = (PFNGLCOLOR3HVNVPROC)glewGetProcAddress((const GLubyte*)"glColor3hvNV")) == null) || r;
  r = ((glColor4hNV = (PFNGLCOLOR4HNVPROC)glewGetProcAddress((const GLubyte*)"glColor4hNV")) == null) || r;
  r = ((glColor4hvNV = (PFNGLCOLOR4HVNVPROC)glewGetProcAddress((const GLubyte*)"glColor4hvNV")) == null) || r;
  r = ((glFogCoordhNV = (PFNGLFOGCOORDHNVPROC)glewGetProcAddress((const GLubyte*)"glFogCoordhNV")) == null) || r;
  r = ((glFogCoordhvNV = (PFNGLFOGCOORDHVNVPROC)glewGetProcAddress((const GLubyte*)"glFogCoordhvNV")) == null) || r;
  r = ((glMultiTexCoord1hNV = (PFNGLMULTITEXCOORD1HNVPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord1hNV")) == null) || r;
  r = ((glMultiTexCoord1hvNV = (PFNGLMULTITEXCOORD1HVNVPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord1hvNV")) == null) || r;
  r = ((glMultiTexCoord2hNV = (PFNGLMULTITEXCOORD2HNVPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord2hNV")) == null) || r;
  r = ((glMultiTexCoord2hvNV = (PFNGLMULTITEXCOORD2HVNVPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord2hvNV")) == null) || r;
  r = ((glMultiTexCoord3hNV = (PFNGLMULTITEXCOORD3HNVPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord3hNV")) == null) || r;
  r = ((glMultiTexCoord3hvNV = (PFNGLMULTITEXCOORD3HVNVPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord3hvNV")) == null) || r;
  r = ((glMultiTexCoord4hNV = (PFNGLMULTITEXCOORD4HNVPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord4hNV")) == null) || r;
  r = ((glMultiTexCoord4hvNV = (PFNGLMULTITEXCOORD4HVNVPROC)glewGetProcAddress((const GLubyte*)"glMultiTexCoord4hvNV")) == null) || r;
  r = ((glNormal3hNV = (PFNGLNORMAL3HNVPROC)glewGetProcAddress((const GLubyte*)"glNormal3hNV")) == null) || r;
  r = ((glNormal3hvNV = (PFNGLNORMAL3HVNVPROC)glewGetProcAddress((const GLubyte*)"glNormal3hvNV")) == null) || r;
  r = ((glSecondaryColor3hNV = (PFNGLSECONDARYCOLOR3HNVPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3hNV")) == null) || r;
  r = ((glSecondaryColor3hvNV = (PFNGLSECONDARYCOLOR3HVNVPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColor3hvNV")) == null) || r;
  r = ((glTexCoord1hNV = (PFNGLTEXCOORD1HNVPROC)glewGetProcAddress((const GLubyte*)"glTexCoord1hNV")) == null) || r;
  r = ((glTexCoord1hvNV = (PFNGLTEXCOORD1HVNVPROC)glewGetProcAddress((const GLubyte*)"glTexCoord1hvNV")) == null) || r;
  r = ((glTexCoord2hNV = (PFNGLTEXCOORD2HNVPROC)glewGetProcAddress((const GLubyte*)"glTexCoord2hNV")) == null) || r;
  r = ((glTexCoord2hvNV = (PFNGLTEXCOORD2HVNVPROC)glewGetProcAddress((const GLubyte*)"glTexCoord2hvNV")) == null) || r;
  r = ((glTexCoord3hNV = (PFNGLTEXCOORD3HNVPROC)glewGetProcAddress((const GLubyte*)"glTexCoord3hNV")) == null) || r;
  r = ((glTexCoord3hvNV = (PFNGLTEXCOORD3HVNVPROC)glewGetProcAddress((const GLubyte*)"glTexCoord3hvNV")) == null) || r;
  r = ((glTexCoord4hNV = (PFNGLTEXCOORD4HNVPROC)glewGetProcAddress((const GLubyte*)"glTexCoord4hNV")) == null) || r;
  r = ((glTexCoord4hvNV = (PFNGLTEXCOORD4HVNVPROC)glewGetProcAddress((const GLubyte*)"glTexCoord4hvNV")) == null) || r;
  r = ((glVertex2hNV = (PFNGLVERTEX2HNVPROC)glewGetProcAddress((const GLubyte*)"glVertex2hNV")) == null) || r;
  r = ((glVertex2hvNV = (PFNGLVERTEX2HVNVPROC)glewGetProcAddress((const GLubyte*)"glVertex2hvNV")) == null) || r;
  r = ((glVertex3hNV = (PFNGLVERTEX3HNVPROC)glewGetProcAddress((const GLubyte*)"glVertex3hNV")) == null) || r;
  r = ((glVertex3hvNV = (PFNGLVERTEX3HVNVPROC)glewGetProcAddress((const GLubyte*)"glVertex3hvNV")) == null) || r;
  r = ((glVertex4hNV = (PFNGLVERTEX4HNVPROC)glewGetProcAddress((const GLubyte*)"glVertex4hNV")) == null) || r;
  r = ((glVertex4hvNV = (PFNGLVERTEX4HVNVPROC)glewGetProcAddress((const GLubyte*)"glVertex4hvNV")) == null) || r;
  r = ((glVertexAttrib1hNV = (PFNGLVERTEXATTRIB1HNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib1hNV")) == null) || r;
  r = ((glVertexAttrib1hvNV = (PFNGLVERTEXATTRIB1HVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib1hvNV")) == null) || r;
  r = ((glVertexAttrib2hNV = (PFNGLVERTEXATTRIB2HNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib2hNV")) == null) || r;
  r = ((glVertexAttrib2hvNV = (PFNGLVERTEXATTRIB2HVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib2hvNV")) == null) || r;
  r = ((glVertexAttrib3hNV = (PFNGLVERTEXATTRIB3HNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib3hNV")) == null) || r;
  r = ((glVertexAttrib3hvNV = (PFNGLVERTEXATTRIB3HVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib3hvNV")) == null) || r;
  r = ((glVertexAttrib4hNV = (PFNGLVERTEXATTRIB4HNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4hNV")) == null) || r;
  r = ((glVertexAttrib4hvNV = (PFNGLVERTEXATTRIB4HVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4hvNV")) == null) || r;
  r = ((glVertexAttribs1hvNV = (PFNGLVERTEXATTRIBS1HVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribs1hvNV")) == null) || r;
  r = ((glVertexAttribs2hvNV = (PFNGLVERTEXATTRIBS2HVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribs2hvNV")) == null) || r;
  r = ((glVertexAttribs3hvNV = (PFNGLVERTEXATTRIBS3HVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribs3hvNV")) == null) || r;
  r = ((glVertexAttribs4hvNV = (PFNGLVERTEXATTRIBS4HVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribs4hvNV")) == null) || r;
  r = ((glVertexWeighthNV = (PFNGLVERTEXWEIGHTHNVPROC)glewGetProcAddress((const GLubyte*)"glVertexWeighthNV")) == null) || r;
  r = ((glVertexWeighthvNV = (PFNGLVERTEXWEIGHTHVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexWeighthvNV")) == null) || r;

  return r;
}

#endif /* GL_NV_half_float */

#ifdef GL_NV_light_max_exponent

#endif /* GL_NV_light_max_exponent */

#ifdef GL_NV_multisample_filter_hint

#endif /* GL_NV_multisample_filter_hint */

#ifdef GL_NV_occlusion_query

static GLboolean _glewInit_GL_NV_occlusion_query (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBeginOcclusionQueryNV = (PFNGLBEGINOCCLUSIONQUERYNVPROC)glewGetProcAddress((const GLubyte*)"glBeginOcclusionQueryNV")) == null) || r;
  r = ((glDeleteOcclusionQueriesNV = (PFNGLDELETEOCCLUSIONQUERIESNVPROC)glewGetProcAddress((const GLubyte*)"glDeleteOcclusionQueriesNV")) == null) || r;
  r = ((glEndOcclusionQueryNV = (PFNGLENDOCCLUSIONQUERYNVPROC)glewGetProcAddress((const GLubyte*)"glEndOcclusionQueryNV")) == null) || r;
  r = ((glGenOcclusionQueriesNV = (PFNGLGENOCCLUSIONQUERIESNVPROC)glewGetProcAddress((const GLubyte*)"glGenOcclusionQueriesNV")) == null) || r;
  r = ((glGetOcclusionQueryivNV = (PFNGLGETOCCLUSIONQUERYIVNVPROC)glewGetProcAddress((const GLubyte*)"glGetOcclusionQueryivNV")) == null) || r;
  r = ((glGetOcclusionQueryuivNV = (PFNGLGETOCCLUSIONQUERYUIVNVPROC)glewGetProcAddress((const GLubyte*)"glGetOcclusionQueryuivNV")) == null) || r;
  r = ((glIsOcclusionQueryNV = (PFNGLISOCCLUSIONQUERYNVPROC)glewGetProcAddress((const GLubyte*)"glIsOcclusionQueryNV")) == null) || r;

  return r;
}

#endif /* GL_NV_occlusion_query */

#ifdef GL_NV_packed_depth_stencil

#endif /* GL_NV_packed_depth_stencil */

#ifdef GL_NV_parameter_buffer_object

static GLboolean _glewInit_GL_NV_parameter_buffer_object (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glProgramBufferParametersIivNV = (PFNGLPROGRAMBUFFERPARAMETERSIIVNVPROC)glewGetProcAddress((const GLubyte*)"glProgramBufferParametersIivNV")) == null) || r;
  r = ((glProgramBufferParametersIuivNV = (PFNGLPROGRAMBUFFERPARAMETERSIUIVNVPROC)glewGetProcAddress((const GLubyte*)"glProgramBufferParametersIuivNV")) == null) || r;
  r = ((glProgramBufferParametersfvNV = (PFNGLPROGRAMBUFFERPARAMETERSFVNVPROC)glewGetProcAddress((const GLubyte*)"glProgramBufferParametersfvNV")) == null) || r;

  return r;
}

#endif /* GL_NV_parameter_buffer_object */

#ifdef GL_NV_parameter_buffer_object2

#endif /* GL_NV_parameter_buffer_object2 */

#ifdef GL_NV_pixel_data_range

static GLboolean _glewInit_GL_NV_pixel_data_range (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glFlushPixelDataRangeNV = (PFNGLFLUSHPIXELDATARANGENVPROC)glewGetProcAddress((const GLubyte*)"glFlushPixelDataRangeNV")) == null) || r;
  r = ((glPixelDataRangeNV = (PFNGLPIXELDATARANGENVPROC)glewGetProcAddress((const GLubyte*)"glPixelDataRangeNV")) == null) || r;

  return r;
}

#endif /* GL_NV_pixel_data_range */

#ifdef GL_NV_point_sprite

static GLboolean _glewInit_GL_NV_point_sprite (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glPointParameteriNV = (PFNGLPOINTPARAMETERINVPROC)glewGetProcAddress((const GLubyte*)"glPointParameteriNV")) == null) || r;
  r = ((glPointParameterivNV = (PFNGLPOINTPARAMETERIVNVPROC)glewGetProcAddress((const GLubyte*)"glPointParameterivNV")) == null) || r;

  return r;
}

#endif /* GL_NV_point_sprite */

#ifdef GL_NV_present_video

static GLboolean _glewInit_GL_NV_present_video (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glGetVideoi64vNV = (PFNGLGETVIDEOI64VNVPROC)glewGetProcAddress((const GLubyte*)"glGetVideoi64vNV")) == null) || r;
  r = ((glGetVideoivNV = (PFNGLGETVIDEOIVNVPROC)glewGetProcAddress((const GLubyte*)"glGetVideoivNV")) == null) || r;
  r = ((glGetVideoui64vNV = (PFNGLGETVIDEOUI64VNVPROC)glewGetProcAddress((const GLubyte*)"glGetVideoui64vNV")) == null) || r;
  r = ((glGetVideouivNV = (PFNGLGETVIDEOUIVNVPROC)glewGetProcAddress((const GLubyte*)"glGetVideouivNV")) == null) || r;
  r = ((glPresentFrameDualFillNV = (PFNGLPRESENTFRAMEDUALFILLNVPROC)glewGetProcAddress((const GLubyte*)"glPresentFrameDualFillNV")) == null) || r;
  r = ((glPresentFrameKeyedNV = (PFNGLPRESENTFRAMEKEYEDNVPROC)glewGetProcAddress((const GLubyte*)"glPresentFrameKeyedNV")) == null) || r;

  return r;
}

#endif /* GL_NV_present_video */

#ifdef GL_NV_primitive_restart

static GLboolean _glewInit_GL_NV_primitive_restart (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glPrimitiveRestartIndexNV = (PFNGLPRIMITIVERESTARTINDEXNVPROC)glewGetProcAddress((const GLubyte*)"glPrimitiveRestartIndexNV")) == null) || r;
  r = ((glPrimitiveRestartNV = (PFNGLPRIMITIVERESTARTNVPROC)glewGetProcAddress((const GLubyte*)"glPrimitiveRestartNV")) == null) || r;

  return r;
}

#endif /* GL_NV_primitive_restart */

#ifdef GL_NV_register_combiners

static GLboolean _glewInit_GL_NV_register_combiners (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glCombinerInputNV = (PFNGLCOMBINERINPUTNVPROC)glewGetProcAddress((const GLubyte*)"glCombinerInputNV")) == null) || r;
  r = ((glCombinerOutputNV = (PFNGLCOMBINEROUTPUTNVPROC)glewGetProcAddress((const GLubyte*)"glCombinerOutputNV")) == null) || r;
  r = ((glCombinerParameterfNV = (PFNGLCOMBINERPARAMETERFNVPROC)glewGetProcAddress((const GLubyte*)"glCombinerParameterfNV")) == null) || r;
  r = ((glCombinerParameterfvNV = (PFNGLCOMBINERPARAMETERFVNVPROC)glewGetProcAddress((const GLubyte*)"glCombinerParameterfvNV")) == null) || r;
  r = ((glCombinerParameteriNV = (PFNGLCOMBINERPARAMETERINVPROC)glewGetProcAddress((const GLubyte*)"glCombinerParameteriNV")) == null) || r;
  r = ((glCombinerParameterivNV = (PFNGLCOMBINERPARAMETERIVNVPROC)glewGetProcAddress((const GLubyte*)"glCombinerParameterivNV")) == null) || r;
  r = ((glFinalCombinerInputNV = (PFNGLFINALCOMBINERINPUTNVPROC)glewGetProcAddress((const GLubyte*)"glFinalCombinerInputNV")) == null) || r;
  r = ((glGetCombinerInputParameterfvNV = (PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC)glewGetProcAddress((const GLubyte*)"glGetCombinerInputParameterfvNV")) == null) || r;
  r = ((glGetCombinerInputParameterivNV = (PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC)glewGetProcAddress((const GLubyte*)"glGetCombinerInputParameterivNV")) == null) || r;
  r = ((glGetCombinerOutputParameterfvNV = (PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC)glewGetProcAddress((const GLubyte*)"glGetCombinerOutputParameterfvNV")) == null) || r;
  r = ((glGetCombinerOutputParameterivNV = (PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC)glewGetProcAddress((const GLubyte*)"glGetCombinerOutputParameterivNV")) == null) || r;
  r = ((glGetFinalCombinerInputParameterfvNV = (PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC)glewGetProcAddress((const GLubyte*)"glGetFinalCombinerInputParameterfvNV")) == null) || r;
  r = ((glGetFinalCombinerInputParameterivNV = (PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC)glewGetProcAddress((const GLubyte*)"glGetFinalCombinerInputParameterivNV")) == null) || r;

  return r;
}

#endif /* GL_NV_register_combiners */

#ifdef GL_NV_register_combiners2

static GLboolean _glewInit_GL_NV_register_combiners2 (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glCombinerStageParameterfvNV = (PFNGLCOMBINERSTAGEPARAMETERFVNVPROC)glewGetProcAddress((const GLubyte*)"glCombinerStageParameterfvNV")) == null) || r;
  r = ((glGetCombinerStageParameterfvNV = (PFNGLGETCOMBINERSTAGEPARAMETERFVNVPROC)glewGetProcAddress((const GLubyte*)"glGetCombinerStageParameterfvNV")) == null) || r;

  return r;
}

#endif /* GL_NV_register_combiners2 */

#ifdef GL_NV_shader_buffer_load

static GLboolean _glewInit_GL_NV_shader_buffer_load (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glGetBufferParameterui64vNV = (PFNGLGETBUFFERPARAMETERUI64VNVPROC)glewGetProcAddress((const GLubyte*)"glGetBufferParameterui64vNV")) == null) || r;
  r = ((glGetIntegerui64vNV = (PFNGLGETINTEGERUI64VNVPROC)glewGetProcAddress((const GLubyte*)"glGetIntegerui64vNV")) == null) || r;
  r = ((glGetNamedBufferParameterui64vNV = (PFNGLGETNAMEDBUFFERPARAMETERUI64VNVPROC)glewGetProcAddress((const GLubyte*)"glGetNamedBufferParameterui64vNV")) == null) || r;
  r = ((glGetUniformui64vNV = (PFNGLGETUNIFORMUI64VNVPROC)glewGetProcAddress((const GLubyte*)"glGetUniformui64vNV")) == null) || r;
  r = ((glIsBufferResidentNV = (PFNGLISBUFFERRESIDENTNVPROC)glewGetProcAddress((const GLubyte*)"glIsBufferResidentNV")) == null) || r;
  r = ((glIsNamedBufferResidentNV = (PFNGLISNAMEDBUFFERRESIDENTNVPROC)glewGetProcAddress((const GLubyte*)"glIsNamedBufferResidentNV")) == null) || r;
  r = ((glMakeBufferNonResidentNV = (PFNGLMAKEBUFFERNONRESIDENTNVPROC)glewGetProcAddress((const GLubyte*)"glMakeBufferNonResidentNV")) == null) || r;
  r = ((glMakeBufferResidentNV = (PFNGLMAKEBUFFERRESIDENTNVPROC)glewGetProcAddress((const GLubyte*)"glMakeBufferResidentNV")) == null) || r;
  r = ((glMakeNamedBufferNonResidentNV = (PFNGLMAKENAMEDBUFFERNONRESIDENTNVPROC)glewGetProcAddress((const GLubyte*)"glMakeNamedBufferNonResidentNV")) == null) || r;
  r = ((glMakeNamedBufferResidentNV = (PFNGLMAKENAMEDBUFFERRESIDENTNVPROC)glewGetProcAddress((const GLubyte*)"glMakeNamedBufferResidentNV")) == null) || r;
  r = ((glProgramUniformui64NV = (PFNGLPROGRAMUNIFORMUI64NVPROC)glewGetProcAddress((const GLubyte*)"glProgramUniformui64NV")) == null) || r;
  r = ((glProgramUniformui64vNV = (PFNGLPROGRAMUNIFORMUI64VNVPROC)glewGetProcAddress((const GLubyte*)"glProgramUniformui64vNV")) == null) || r;
  r = ((glUniformui64NV = (PFNGLUNIFORMUI64NVPROC)glewGetProcAddress((const GLubyte*)"glUniformui64NV")) == null) || r;
  r = ((glUniformui64vNV = (PFNGLUNIFORMUI64VNVPROC)glewGetProcAddress((const GLubyte*)"glUniformui64vNV")) == null) || r;

  return r;
}

#endif /* GL_NV_shader_buffer_load */

#ifdef GL_NV_texgen_emboss

#endif /* GL_NV_texgen_emboss */

#ifdef GL_NV_texgen_reflection

#endif /* GL_NV_texgen_reflection */

#ifdef GL_NV_texture_barrier

static GLboolean _glewInit_GL_NV_texture_barrier (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glTextureBarrierNV = (PFNGLTEXTUREBARRIERNVPROC)glewGetProcAddress((const GLubyte*)"glTextureBarrierNV")) == null) || r;

  return r;
}

#endif /* GL_NV_texture_barrier */

#ifdef GL_NV_texture_compression_vtc

#endif /* GL_NV_texture_compression_vtc */

#ifdef GL_NV_texture_env_combine4

#endif /* GL_NV_texture_env_combine4 */

#ifdef GL_NV_texture_expand_normal

#endif /* GL_NV_texture_expand_normal */

#ifdef GL_NV_texture_rectangle

#endif /* GL_NV_texture_rectangle */

#ifdef GL_NV_texture_shader

#endif /* GL_NV_texture_shader */

#ifdef GL_NV_texture_shader2

#endif /* GL_NV_texture_shader2 */

#ifdef GL_NV_texture_shader3

#endif /* GL_NV_texture_shader3 */

#ifdef GL_NV_transform_feedback

static GLboolean _glewInit_GL_NV_transform_feedback (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glActiveVaryingNV = (PFNGLACTIVEVARYINGNVPROC)glewGetProcAddress((const GLubyte*)"glActiveVaryingNV")) == null) || r;
  r = ((glBeginTransformFeedbackNV = (PFNGLBEGINTRANSFORMFEEDBACKNVPROC)glewGetProcAddress((const GLubyte*)"glBeginTransformFeedbackNV")) == null) || r;
  r = ((glBindBufferBaseNV = (PFNGLBINDBUFFERBASENVPROC)glewGetProcAddress((const GLubyte*)"glBindBufferBaseNV")) == null) || r;
  r = ((glBindBufferOffsetNV = (PFNGLBINDBUFFEROFFSETNVPROC)glewGetProcAddress((const GLubyte*)"glBindBufferOffsetNV")) == null) || r;
  r = ((glBindBufferRangeNV = (PFNGLBINDBUFFERRANGENVPROC)glewGetProcAddress((const GLubyte*)"glBindBufferRangeNV")) == null) || r;
  r = ((glEndTransformFeedbackNV = (PFNGLENDTRANSFORMFEEDBACKNVPROC)glewGetProcAddress((const GLubyte*)"glEndTransformFeedbackNV")) == null) || r;
  r = ((glGetActiveVaryingNV = (PFNGLGETACTIVEVARYINGNVPROC)glewGetProcAddress((const GLubyte*)"glGetActiveVaryingNV")) == null) || r;
  r = ((glGetTransformFeedbackVaryingNV = (PFNGLGETTRANSFORMFEEDBACKVARYINGNVPROC)glewGetProcAddress((const GLubyte*)"glGetTransformFeedbackVaryingNV")) == null) || r;
  r = ((glGetVaryingLocationNV = (PFNGLGETVARYINGLOCATIONNVPROC)glewGetProcAddress((const GLubyte*)"glGetVaryingLocationNV")) == null) || r;
  r = ((glTransformFeedbackAttribsNV = (PFNGLTRANSFORMFEEDBACKATTRIBSNVPROC)glewGetProcAddress((const GLubyte*)"glTransformFeedbackAttribsNV")) == null) || r;
  r = ((glTransformFeedbackVaryingsNV = (PFNGLTRANSFORMFEEDBACKVARYINGSNVPROC)glewGetProcAddress((const GLubyte*)"glTransformFeedbackVaryingsNV")) == null) || r;

  return r;
}

#endif /* GL_NV_transform_feedback */

#ifdef GL_NV_transform_feedback2

static GLboolean _glewInit_GL_NV_transform_feedback2 (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBindTransformFeedbackNV = (PFNGLBINDTRANSFORMFEEDBACKNVPROC)glewGetProcAddress((const GLubyte*)"glBindTransformFeedbackNV")) == null) || r;
  r = ((glDeleteTransformFeedbacksNV = (PFNGLDELETETRANSFORMFEEDBACKSNVPROC)glewGetProcAddress((const GLubyte*)"glDeleteTransformFeedbacksNV")) == null) || r;
  r = ((glDrawTransformFeedbackNV = (PFNGLDRAWTRANSFORMFEEDBACKNVPROC)glewGetProcAddress((const GLubyte*)"glDrawTransformFeedbackNV")) == null) || r;
  r = ((glGenTransformFeedbacksNV = (PFNGLGENTRANSFORMFEEDBACKSNVPROC)glewGetProcAddress((const GLubyte*)"glGenTransformFeedbacksNV")) == null) || r;
  r = ((glIsTransformFeedbackNV = (PFNGLISTRANSFORMFEEDBACKNVPROC)glewGetProcAddress((const GLubyte*)"glIsTransformFeedbackNV")) == null) || r;
  r = ((glPauseTransformFeedbackNV = (PFNGLPAUSETRANSFORMFEEDBACKNVPROC)glewGetProcAddress((const GLubyte*)"glPauseTransformFeedbackNV")) == null) || r;
  r = ((glResumeTransformFeedbackNV = (PFNGLRESUMETRANSFORMFEEDBACKNVPROC)glewGetProcAddress((const GLubyte*)"glResumeTransformFeedbackNV")) == null) || r;

  return r;
}

#endif /* GL_NV_transform_feedback2 */

#ifdef GL_NV_vertex_array_range

static GLboolean _glewInit_GL_NV_vertex_array_range (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glFlushVertexArrayRangeNV = (PFNGLFLUSHVERTEXARRAYRANGENVPROC)glewGetProcAddress((const GLubyte*)"glFlushVertexArrayRangeNV")) == null) || r;
  r = ((glVertexArrayRangeNV = (PFNGLVERTEXARRAYRANGENVPROC)glewGetProcAddress((const GLubyte*)"glVertexArrayRangeNV")) == null) || r;

  return r;
}

#endif /* GL_NV_vertex_array_range */

#ifdef GL_NV_vertex_array_range2

#endif /* GL_NV_vertex_array_range2 */

#ifdef GL_NV_vertex_buffer_unified_memory

static GLboolean _glewInit_GL_NV_vertex_buffer_unified_memory (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glBufferAddressRangeNV = (PFNGLBUFFERADDRESSRANGENVPROC)glewGetProcAddress((const GLubyte*)"glBufferAddressRangeNV")) == null) || r;
  r = ((glColorFormatNV = (PFNGLCOLORFORMATNVPROC)glewGetProcAddress((const GLubyte*)"glColorFormatNV")) == null) || r;
  r = ((glEdgeFlagFormatNV = (PFNGLEDGEFLAGFORMATNVPROC)glewGetProcAddress((const GLubyte*)"glEdgeFlagFormatNV")) == null) || r;
  r = ((glFogCoordFormatNV = (PFNGLFOGCOORDFORMATNVPROC)glewGetProcAddress((const GLubyte*)"glFogCoordFormatNV")) == null) || r;
  r = ((glGetIntegerui64i_vNV = (PFNGLGETINTEGERUI64I_VNVPROC)glewGetProcAddress((const GLubyte*)"glGetIntegerui64i_vNV")) == null) || r;
  r = ((glIndexFormatNV = (PFNGLINDEXFORMATNVPROC)glewGetProcAddress((const GLubyte*)"glIndexFormatNV")) == null) || r;
  r = ((glNormalFormatNV = (PFNGLNORMALFORMATNVPROC)glewGetProcAddress((const GLubyte*)"glNormalFormatNV")) == null) || r;
  r = ((glSecondaryColorFormatNV = (PFNGLSECONDARYCOLORFORMATNVPROC)glewGetProcAddress((const GLubyte*)"glSecondaryColorFormatNV")) == null) || r;
  r = ((glTexCoordFormatNV = (PFNGLTEXCOORDFORMATNVPROC)glewGetProcAddress((const GLubyte*)"glTexCoordFormatNV")) == null) || r;
  r = ((glVertexAttribFormatNV = (PFNGLVERTEXATTRIBFORMATNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribFormatNV")) == null) || r;
  r = ((glVertexAttribIFormatNV = (PFNGLVERTEXATTRIBIFORMATNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribIFormatNV")) == null) || r;
  r = ((glVertexFormatNV = (PFNGLVERTEXFORMATNVPROC)glewGetProcAddress((const GLubyte*)"glVertexFormatNV")) == null) || r;

  return r;
}

#endif /* GL_NV_vertex_buffer_unified_memory */

#ifdef GL_NV_vertex_program

static GLboolean _glewInit_GL_NV_vertex_program (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glAreProgramsResidentNV = (PFNGLAREPROGRAMSRESIDENTNVPROC)glewGetProcAddress((const GLubyte*)"glAreProgramsResidentNV")) == null) || r;
  r = ((glBindProgramNV = (PFNGLBINDPROGRAMNVPROC)glewGetProcAddress((const GLubyte*)"glBindProgramNV")) == null) || r;
  r = ((glDeleteProgramsNV = (PFNGLDELETEPROGRAMSNVPROC)glewGetProcAddress((const GLubyte*)"glDeleteProgramsNV")) == null) || r;
  r = ((glExecuteProgramNV = (PFNGLEXECUTEPROGRAMNVPROC)glewGetProcAddress((const GLubyte*)"glExecuteProgramNV")) == null) || r;
  r = ((glGenProgramsNV = (PFNGLGENPROGRAMSNVPROC)glewGetProcAddress((const GLubyte*)"glGenProgramsNV")) == null) || r;
  r = ((glGetProgramParameterdvNV = (PFNGLGETPROGRAMPARAMETERDVNVPROC)glewGetProcAddress((const GLubyte*)"glGetProgramParameterdvNV")) == null) || r;
  r = ((glGetProgramParameterfvNV = (PFNGLGETPROGRAMPARAMETERFVNVPROC)glewGetProcAddress((const GLubyte*)"glGetProgramParameterfvNV")) == null) || r;
  r = ((glGetProgramStringNV = (PFNGLGETPROGRAMSTRINGNVPROC)glewGetProcAddress((const GLubyte*)"glGetProgramStringNV")) == null) || r;
  r = ((glGetProgramivNV = (PFNGLGETPROGRAMIVNVPROC)glewGetProcAddress((const GLubyte*)"glGetProgramivNV")) == null) || r;
  r = ((glGetTrackMatrixivNV = (PFNGLGETTRACKMATRIXIVNVPROC)glewGetProcAddress((const GLubyte*)"glGetTrackMatrixivNV")) == null) || r;
  r = ((glGetVertexAttribPointervNV = (PFNGLGETVERTEXATTRIBPOINTERVNVPROC)glewGetProcAddress((const GLubyte*)"glGetVertexAttribPointervNV")) == null) || r;
  r = ((glGetVertexAttribdvNV = (PFNGLGETVERTEXATTRIBDVNVPROC)glewGetProcAddress((const GLubyte*)"glGetVertexAttribdvNV")) == null) || r;
  r = ((glGetVertexAttribfvNV = (PFNGLGETVERTEXATTRIBFVNVPROC)glewGetProcAddress((const GLubyte*)"glGetVertexAttribfvNV")) == null) || r;
  r = ((glGetVertexAttribivNV = (PFNGLGETVERTEXATTRIBIVNVPROC)glewGetProcAddress((const GLubyte*)"glGetVertexAttribivNV")) == null) || r;
  r = ((glIsProgramNV = (PFNGLISPROGRAMNVPROC)glewGetProcAddress((const GLubyte*)"glIsProgramNV")) == null) || r;
  r = ((glLoadProgramNV = (PFNGLLOADPROGRAMNVPROC)glewGetProcAddress((const GLubyte*)"glLoadProgramNV")) == null) || r;
  r = ((glProgramParameter4dNV = (PFNGLPROGRAMPARAMETER4DNVPROC)glewGetProcAddress((const GLubyte*)"glProgramParameter4dNV")) == null) || r;
  r = ((glProgramParameter4dvNV = (PFNGLPROGRAMPARAMETER4DVNVPROC)glewGetProcAddress((const GLubyte*)"glProgramParameter4dvNV")) == null) || r;
  r = ((glProgramParameter4fNV = (PFNGLPROGRAMPARAMETER4FNVPROC)glewGetProcAddress((const GLubyte*)"glProgramParameter4fNV")) == null) || r;
  r = ((glProgramParameter4fvNV = (PFNGLPROGRAMPARAMETER4FVNVPROC)glewGetProcAddress((const GLubyte*)"glProgramParameter4fvNV")) == null) || r;
  r = ((glProgramParameters4dvNV = (PFNGLPROGRAMPARAMETERS4DVNVPROC)glewGetProcAddress((const GLubyte*)"glProgramParameters4dvNV")) == null) || r;
  r = ((glProgramParameters4fvNV = (PFNGLPROGRAMPARAMETERS4FVNVPROC)glewGetProcAddress((const GLubyte*)"glProgramParameters4fvNV")) == null) || r;
  r = ((glRequestResidentProgramsNV = (PFNGLREQUESTRESIDENTPROGRAMSNVPROC)glewGetProcAddress((const GLubyte*)"glRequestResidentProgramsNV")) == null) || r;
  r = ((glTrackMatrixNV = (PFNGLTRACKMATRIXNVPROC)glewGetProcAddress((const GLubyte*)"glTrackMatrixNV")) == null) || r;
  r = ((glVertexAttrib1dNV = (PFNGLVERTEXATTRIB1DNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib1dNV")) == null) || r;
  r = ((glVertexAttrib1dvNV = (PFNGLVERTEXATTRIB1DVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib1dvNV")) == null) || r;
  r = ((glVertexAttrib1fNV = (PFNGLVERTEXATTRIB1FNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib1fNV")) == null) || r;
  r = ((glVertexAttrib1fvNV = (PFNGLVERTEXATTRIB1FVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib1fvNV")) == null) || r;
  r = ((glVertexAttrib1sNV = (PFNGLVERTEXATTRIB1SNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib1sNV")) == null) || r;
  r = ((glVertexAttrib1svNV = (PFNGLVERTEXATTRIB1SVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib1svNV")) == null) || r;
  r = ((glVertexAttrib2dNV = (PFNGLVERTEXATTRIB2DNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib2dNV")) == null) || r;
  r = ((glVertexAttrib2dvNV = (PFNGLVERTEXATTRIB2DVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib2dvNV")) == null) || r;
  r = ((glVertexAttrib2fNV = (PFNGLVERTEXATTRIB2FNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib2fNV")) == null) || r;
  r = ((glVertexAttrib2fvNV = (PFNGLVERTEXATTRIB2FVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib2fvNV")) == null) || r;
  r = ((glVertexAttrib2sNV = (PFNGLVERTEXATTRIB2SNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib2sNV")) == null) || r;
  r = ((glVertexAttrib2svNV = (PFNGLVERTEXATTRIB2SVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib2svNV")) == null) || r;
  r = ((glVertexAttrib3dNV = (PFNGLVERTEXATTRIB3DNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib3dNV")) == null) || r;
  r = ((glVertexAttrib3dvNV = (PFNGLVERTEXATTRIB3DVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib3dvNV")) == null) || r;
  r = ((glVertexAttrib3fNV = (PFNGLVERTEXATTRIB3FNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib3fNV")) == null) || r;
  r = ((glVertexAttrib3fvNV = (PFNGLVERTEXATTRIB3FVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib3fvNV")) == null) || r;
  r = ((glVertexAttrib3sNV = (PFNGLVERTEXATTRIB3SNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib3sNV")) == null) || r;
  r = ((glVertexAttrib3svNV = (PFNGLVERTEXATTRIB3SVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib3svNV")) == null) || r;
  r = ((glVertexAttrib4dNV = (PFNGLVERTEXATTRIB4DNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4dNV")) == null) || r;
  r = ((glVertexAttrib4dvNV = (PFNGLVERTEXATTRIB4DVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4dvNV")) == null) || r;
  r = ((glVertexAttrib4fNV = (PFNGLVERTEXATTRIB4FNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4fNV")) == null) || r;
  r = ((glVertexAttrib4fvNV = (PFNGLVERTEXATTRIB4FVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4fvNV")) == null) || r;
  r = ((glVertexAttrib4sNV = (PFNGLVERTEXATTRIB4SNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4sNV")) == null) || r;
  r = ((glVertexAttrib4svNV = (PFNGLVERTEXATTRIB4SVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4svNV")) == null) || r;
  r = ((glVertexAttrib4ubNV = (PFNGLVERTEXATTRIB4UBNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4ubNV")) == null) || r;
  r = ((glVertexAttrib4ubvNV = (PFNGLVERTEXATTRIB4UBVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttrib4ubvNV")) == null) || r;
  r = ((glVertexAttribPointerNV = (PFNGLVERTEXATTRIBPOINTERNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribPointerNV")) == null) || r;
  r = ((glVertexAttribs1dvNV = (PFNGLVERTEXATTRIBS1DVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribs1dvNV")) == null) || r;
  r = ((glVertexAttribs1fvNV = (PFNGLVERTEXATTRIBS1FVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribs1fvNV")) == null) || r;
  r = ((glVertexAttribs1svNV = (PFNGLVERTEXATTRIBS1SVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribs1svNV")) == null) || r;
  r = ((glVertexAttribs2dvNV = (PFNGLVERTEXATTRIBS2DVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribs2dvNV")) == null) || r;
  r = ((glVertexAttribs2fvNV = (PFNGLVERTEXATTRIBS2FVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribs2fvNV")) == null) || r;
  r = ((glVertexAttribs2svNV = (PFNGLVERTEXATTRIBS2SVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribs2svNV")) == null) || r;
  r = ((glVertexAttribs3dvNV = (PFNGLVERTEXATTRIBS3DVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribs3dvNV")) == null) || r;
  r = ((glVertexAttribs3fvNV = (PFNGLVERTEXATTRIBS3FVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribs3fvNV")) == null) || r;
  r = ((glVertexAttribs3svNV = (PFNGLVERTEXATTRIBS3SVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribs3svNV")) == null) || r;
  r = ((glVertexAttribs4dvNV = (PFNGLVERTEXATTRIBS4DVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribs4dvNV")) == null) || r;
  r = ((glVertexAttribs4fvNV = (PFNGLVERTEXATTRIBS4FVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribs4fvNV")) == null) || r;
  r = ((glVertexAttribs4svNV = (PFNGLVERTEXATTRIBS4SVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribs4svNV")) == null) || r;
  r = ((glVertexAttribs4ubvNV = (PFNGLVERTEXATTRIBS4UBVNVPROC)glewGetProcAddress((const GLubyte*)"glVertexAttribs4ubvNV")) == null) || r;

  return r;
}

#endif /* GL_NV_vertex_program */

#ifdef GL_NV_vertex_program1_1

#endif /* GL_NV_vertex_program1_1 */

#ifdef GL_NV_vertex_program2

#endif /* GL_NV_vertex_program2 */

#ifdef GL_NV_vertex_program2_option

#endif /* GL_NV_vertex_program2_option */

#ifdef GL_NV_vertex_program3

#endif /* GL_NV_vertex_program3 */

#ifdef GL_NV_vertex_program4

#endif /* GL_NV_vertex_program4 */

#ifdef GL_OES_byte_coordinates

#endif /* GL_OES_byte_coordinates */

#ifdef GL_OES_compressed_paletted_texture

#endif /* GL_OES_compressed_paletted_texture */

#ifdef GL_OES_read_format

#endif /* GL_OES_read_format */

#ifdef GL_OES_single_precision

static GLboolean _glewInit_GL_OES_single_precision (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glClearDepthfOES = (PFNGLCLEARDEPTHFOESPROC)glewGetProcAddress((const GLubyte*)"glClearDepthfOES")) == null) || r;
  r = ((glClipPlanefOES = (PFNGLCLIPPLANEFOESPROC)glewGetProcAddress((const GLubyte*)"glClipPlanefOES")) == null) || r;
  r = ((glDepthRangefOES = (PFNGLDEPTHRANGEFOESPROC)glewGetProcAddress((const GLubyte*)"glDepthRangefOES")) == null) || r;
  r = ((glFrustumfOES = (PFNGLFRUSTUMFOESPROC)glewGetProcAddress((const GLubyte*)"glFrustumfOES")) == null) || r;
  r = ((glGetClipPlanefOES = (PFNGLGETCLIPPLANEFOESPROC)glewGetProcAddress((const GLubyte*)"glGetClipPlanefOES")) == null) || r;
  r = ((glOrthofOES = (PFNGLORTHOFOESPROC)glewGetProcAddress((const GLubyte*)"glOrthofOES")) == null) || r;

  return r;
}

#endif /* GL_OES_single_precision */

#ifdef GL_OML_interlace

#endif /* GL_OML_interlace */

#ifdef GL_OML_resample

#endif /* GL_OML_resample */

#ifdef GL_OML_subsample

#endif /* GL_OML_subsample */

#ifdef GL_PGI_misc_hints

#endif /* GL_PGI_misc_hints */

#ifdef GL_PGI_vertex_hints

#endif /* GL_PGI_vertex_hints */

#ifdef GL_REND_screen_coordinates

#endif /* GL_REND_screen_coordinates */

#ifdef GL_S3_s3tc

#endif /* GL_S3_s3tc */

#ifdef GL_SGIS_color_range

#endif /* GL_SGIS_color_range */

#ifdef GL_SGIS_detail_texture

static GLboolean _glewInit_GL_SGIS_detail_texture (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glDetailTexFuncSGIS = (PFNGLDETAILTEXFUNCSGISPROC)glewGetProcAddress((const GLubyte*)"glDetailTexFuncSGIS")) == null) || r;
  r = ((glGetDetailTexFuncSGIS = (PFNGLGETDETAILTEXFUNCSGISPROC)glewGetProcAddress((const GLubyte*)"glGetDetailTexFuncSGIS")) == null) || r;

  return r;
}

#endif /* GL_SGIS_detail_texture */

#ifdef GL_SGIS_fog_function

static GLboolean _glewInit_GL_SGIS_fog_function (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glFogFuncSGIS = (PFNGLFOGFUNCSGISPROC)glewGetProcAddress((const GLubyte*)"glFogFuncSGIS")) == null) || r;
  r = ((glGetFogFuncSGIS = (PFNGLGETFOGFUNCSGISPROC)glewGetProcAddress((const GLubyte*)"glGetFogFuncSGIS")) == null) || r;

  return r;
}

#endif /* GL_SGIS_fog_function */

#ifdef GL_SGIS_generate_mipmap

#endif /* GL_SGIS_generate_mipmap */

#ifdef GL_SGIS_multisample

static GLboolean _glewInit_GL_SGIS_multisample (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glSampleMaskSGIS = (PFNGLSAMPLEMASKSGISPROC)glewGetProcAddress((const GLubyte*)"glSampleMaskSGIS")) == null) || r;
  r = ((glSamplePatternSGIS = (PFNGLSAMPLEPATTERNSGISPROC)glewGetProcAddress((const GLubyte*)"glSamplePatternSGIS")) == null) || r;

  return r;
}

#endif /* GL_SGIS_multisample */

#ifdef GL_SGIS_pixel_texture

#endif /* GL_SGIS_pixel_texture */

#ifdef GL_SGIS_point_line_texgen

#endif /* GL_SGIS_point_line_texgen */

#ifdef GL_SGIS_sharpen_texture

static GLboolean _glewInit_GL_SGIS_sharpen_texture (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glGetSharpenTexFuncSGIS = (PFNGLGETSHARPENTEXFUNCSGISPROC)glewGetProcAddress((const GLubyte*)"glGetSharpenTexFuncSGIS")) == null) || r;
  r = ((glSharpenTexFuncSGIS = (PFNGLSHARPENTEXFUNCSGISPROC)glewGetProcAddress((const GLubyte*)"glSharpenTexFuncSGIS")) == null) || r;

  return r;
}

#endif /* GL_SGIS_sharpen_texture */

#ifdef GL_SGIS_texture4D

static GLboolean _glewInit_GL_SGIS_texture4D (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glTexImage4DSGIS = (PFNGLTEXIMAGE4DSGISPROC)glewGetProcAddress((const GLubyte*)"glTexImage4DSGIS")) == null) || r;
  r = ((glTexSubImage4DSGIS = (PFNGLTEXSUBIMAGE4DSGISPROC)glewGetProcAddress((const GLubyte*)"glTexSubImage4DSGIS")) == null) || r;

  return r;
}

#endif /* GL_SGIS_texture4D */

#ifdef GL_SGIS_texture_border_clamp

#endif /* GL_SGIS_texture_border_clamp */

#ifdef GL_SGIS_texture_edge_clamp

#endif /* GL_SGIS_texture_edge_clamp */

#ifdef GL_SGIS_texture_filter4

static GLboolean _glewInit_GL_SGIS_texture_filter4 (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glGetTexFilterFuncSGIS = (PFNGLGETTEXFILTERFUNCSGISPROC)glewGetProcAddress((const GLubyte*)"glGetTexFilterFuncSGIS")) == null) || r;
  r = ((glTexFilterFuncSGIS = (PFNGLTEXFILTERFUNCSGISPROC)glewGetProcAddress((const GLubyte*)"glTexFilterFuncSGIS")) == null) || r;

  return r;
}

#endif /* GL_SGIS_texture_filter4 */

#ifdef GL_SGIS_texture_lod

#endif /* GL_SGIS_texture_lod */

#ifdef GL_SGIS_texture_select

#endif /* GL_SGIS_texture_select */

#ifdef GL_SGIX_async

static GLboolean _glewInit_GL_SGIX_async (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glAsyncMarkerSGIX = (PFNGLASYNCMARKERSGIXPROC)glewGetProcAddress((const GLubyte*)"glAsyncMarkerSGIX")) == null) || r;
  r = ((glDeleteAsyncMarkersSGIX = (PFNGLDELETEASYNCMARKERSSGIXPROC)glewGetProcAddress((const GLubyte*)"glDeleteAsyncMarkersSGIX")) == null) || r;
  r = ((glFinishAsyncSGIX = (PFNGLFINISHASYNCSGIXPROC)glewGetProcAddress((const GLubyte*)"glFinishAsyncSGIX")) == null) || r;
  r = ((glGenAsyncMarkersSGIX = (PFNGLGENASYNCMARKERSSGIXPROC)glewGetProcAddress((const GLubyte*)"glGenAsyncMarkersSGIX")) == null) || r;
  r = ((glIsAsyncMarkerSGIX = (PFNGLISASYNCMARKERSGIXPROC)glewGetProcAddress((const GLubyte*)"glIsAsyncMarkerSGIX")) == null) || r;
  r = ((glPollAsyncSGIX = (PFNGLPOLLASYNCSGIXPROC)glewGetProcAddress((const GLubyte*)"glPollAsyncSGIX")) == null) || r;

  return r;
}

#endif /* GL_SGIX_async */

#ifdef GL_SGIX_async_histogram

#endif /* GL_SGIX_async_histogram */

#ifdef GL_SGIX_async_pixel

#endif /* GL_SGIX_async_pixel */

#ifdef GL_SGIX_blend_alpha_minmax

#endif /* GL_SGIX_blend_alpha_minmax */

#ifdef GL_SGIX_clipmap

#endif /* GL_SGIX_clipmap */

#ifdef GL_SGIX_convolution_accuracy

#endif /* GL_SGIX_convolution_accuracy */

#ifdef GL_SGIX_depth_texture

#endif /* GL_SGIX_depth_texture */

#ifdef GL_SGIX_flush_raster

static GLboolean _glewInit_GL_SGIX_flush_raster (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glFlushRasterSGIX = (PFNGLFLUSHRASTERSGIXPROC)glewGetProcAddress((const GLubyte*)"glFlushRasterSGIX")) == null) || r;

  return r;
}

#endif /* GL_SGIX_flush_raster */

#ifdef GL_SGIX_fog_offset

#endif /* GL_SGIX_fog_offset */

#ifdef GL_SGIX_fog_texture

static GLboolean _glewInit_GL_SGIX_fog_texture (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glTextureFogSGIX = (PFNGLTEXTUREFOGSGIXPROC)glewGetProcAddress((const GLubyte*)"glTextureFogSGIX")) == null) || r;

  return r;
}

#endif /* GL_SGIX_fog_texture */

#ifdef GL_SGIX_fragment_specular_lighting

static GLboolean _glewInit_GL_SGIX_fragment_specular_lighting (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glFragmentColorMaterialSGIX = (PFNGLFRAGMENTCOLORMATERIALSGIXPROC)glewGetProcAddress((const GLubyte*)"glFragmentColorMaterialSGIX")) == null) || r;
  r = ((glFragmentLightModelfSGIX = (PFNGLFRAGMENTLIGHTMODELFSGIXPROC)glewGetProcAddress((const GLubyte*)"glFragmentLightModelfSGIX")) == null) || r;
  r = ((glFragmentLightModelfvSGIX = (PFNGLFRAGMENTLIGHTMODELFVSGIXPROC)glewGetProcAddress((const GLubyte*)"glFragmentLightModelfvSGIX")) == null) || r;
  r = ((glFragmentLightModeliSGIX = (PFNGLFRAGMENTLIGHTMODELISGIXPROC)glewGetProcAddress((const GLubyte*)"glFragmentLightModeliSGIX")) == null) || r;
  r = ((glFragmentLightModelivSGIX = (PFNGLFRAGMENTLIGHTMODELIVSGIXPROC)glewGetProcAddress((const GLubyte*)"glFragmentLightModelivSGIX")) == null) || r;
  r = ((glFragmentLightfSGIX = (PFNGLFRAGMENTLIGHTFSGIXPROC)glewGetProcAddress((const GLubyte*)"glFragmentLightfSGIX")) == null) || r;
  r = ((glFragmentLightfvSGIX = (PFNGLFRAGMENTLIGHTFVSGIXPROC)glewGetProcAddress((const GLubyte*)"glFragmentLightfvSGIX")) == null) || r;
  r = ((glFragmentLightiSGIX = (PFNGLFRAGMENTLIGHTISGIXPROC)glewGetProcAddress((const GLubyte*)"glFragmentLightiSGIX")) == null) || r;
  r = ((glFragmentLightivSGIX = (PFNGLFRAGMENTLIGHTIVSGIXPROC)glewGetProcAddress((const GLubyte*)"glFragmentLightivSGIX")) == null) || r;
  r = ((glFragmentMaterialfSGIX = (PFNGLFRAGMENTMATERIALFSGIXPROC)glewGetProcAddress((const GLubyte*)"glFragmentMaterialfSGIX")) == null) || r;
  r = ((glFragmentMaterialfvSGIX = (PFNGLFRAGMENTMATERIALFVSGIXPROC)glewGetProcAddress((const GLubyte*)"glFragmentMaterialfvSGIX")) == null) || r;
  r = ((glFragmentMaterialiSGIX = (PFNGLFRAGMENTMATERIALISGIXPROC)glewGetProcAddress((const GLubyte*)"glFragmentMaterialiSGIX")) == null) || r;
  r = ((glFragmentMaterialivSGIX = (PFNGLFRAGMENTMATERIALIVSGIXPROC)glewGetProcAddress((const GLubyte*)"glFragmentMaterialivSGIX")) == null) || r;
  r = ((glGetFragmentLightfvSGIX = (PFNGLGETFRAGMENTLIGHTFVSGIXPROC)glewGetProcAddress((const GLubyte*)"glGetFragmentLightfvSGIX")) == null) || r;
  r = ((glGetFragmentLightivSGIX = (PFNGLGETFRAGMENTLIGHTIVSGIXPROC)glewGetProcAddress((const GLubyte*)"glGetFragmentLightivSGIX")) == null) || r;
  r = ((glGetFragmentMaterialfvSGIX = (PFNGLGETFRAGMENTMATERIALFVSGIXPROC)glewGetProcAddress((const GLubyte*)"glGetFragmentMaterialfvSGIX")) == null) || r;
  r = ((glGetFragmentMaterialivSGIX = (PFNGLGETFRAGMENTMATERIALIVSGIXPROC)glewGetProcAddress((const GLubyte*)"glGetFragmentMaterialivSGIX")) == null) || r;

  return r;
}

#endif /* GL_SGIX_fragment_specular_lighting */

#ifdef GL_SGIX_framezoom

static GLboolean _glewInit_GL_SGIX_framezoom (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glFrameZoomSGIX = (PFNGLFRAMEZOOMSGIXPROC)glewGetProcAddress((const GLubyte*)"glFrameZoomSGIX")) == null) || r;

  return r;
}

#endif /* GL_SGIX_framezoom */

#ifdef GL_SGIX_interlace

#endif /* GL_SGIX_interlace */

#ifdef GL_SGIX_ir_instrument1

#endif /* GL_SGIX_ir_instrument1 */

#ifdef GL_SGIX_list_priority

#endif /* GL_SGIX_list_priority */

#ifdef GL_SGIX_pixel_texture

static GLboolean _glewInit_GL_SGIX_pixel_texture (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glPixelTexGenSGIX = (PFNGLPIXELTEXGENSGIXPROC)glewGetProcAddress((const GLubyte*)"glPixelTexGenSGIX")) == null) || r;

  return r;
}

#endif /* GL_SGIX_pixel_texture */

#ifdef GL_SGIX_pixel_texture_bits

#endif /* GL_SGIX_pixel_texture_bits */

#ifdef GL_SGIX_reference_plane

static GLboolean _glewInit_GL_SGIX_reference_plane (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glReferencePlaneSGIX = (PFNGLREFERENCEPLANESGIXPROC)glewGetProcAddress((const GLubyte*)"glReferencePlaneSGIX")) == null) || r;

  return r;
}

#endif /* GL_SGIX_reference_plane */

#ifdef GL_SGIX_resample

#endif /* GL_SGIX_resample */

#ifdef GL_SGIX_shadow

#endif /* GL_SGIX_shadow */

#ifdef GL_SGIX_shadow_ambient

#endif /* GL_SGIX_shadow_ambient */

#ifdef GL_SGIX_sprite

static GLboolean _glewInit_GL_SGIX_sprite (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glSpriteParameterfSGIX = (PFNGLSPRITEPARAMETERFSGIXPROC)glewGetProcAddress((const GLubyte*)"glSpriteParameterfSGIX")) == null) || r;
  r = ((glSpriteParameterfvSGIX = (PFNGLSPRITEPARAMETERFVSGIXPROC)glewGetProcAddress((const GLubyte*)"glSpriteParameterfvSGIX")) == null) || r;
  r = ((glSpriteParameteriSGIX = (PFNGLSPRITEPARAMETERISGIXPROC)glewGetProcAddress((const GLubyte*)"glSpriteParameteriSGIX")) == null) || r;
  r = ((glSpriteParameterivSGIX = (PFNGLSPRITEPARAMETERIVSGIXPROC)glewGetProcAddress((const GLubyte*)"glSpriteParameterivSGIX")) == null) || r;

  return r;
}

#endif /* GL_SGIX_sprite */

#ifdef GL_SGIX_tag_sample_buffer

static GLboolean _glewInit_GL_SGIX_tag_sample_buffer (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glTagSampleBufferSGIX = (PFNGLTAGSAMPLEBUFFERSGIXPROC)glewGetProcAddress((const GLubyte*)"glTagSampleBufferSGIX")) == null) || r;

  return r;
}

#endif /* GL_SGIX_tag_sample_buffer */

#ifdef GL_SGIX_texture_add_env

#endif /* GL_SGIX_texture_add_env */

#ifdef GL_SGIX_texture_coordinate_clamp

#endif /* GL_SGIX_texture_coordinate_clamp */

#ifdef GL_SGIX_texture_lod_bias

#endif /* GL_SGIX_texture_lod_bias */

#ifdef GL_SGIX_texture_multi_buffer

#endif /* GL_SGIX_texture_multi_buffer */

#ifdef GL_SGIX_texture_range

#endif /* GL_SGIX_texture_range */

#ifdef GL_SGIX_texture_scale_bias

#endif /* GL_SGIX_texture_scale_bias */

#ifdef GL_SGIX_vertex_preclip

#endif /* GL_SGIX_vertex_preclip */

#ifdef GL_SGIX_vertex_preclip_hint

#endif /* GL_SGIX_vertex_preclip_hint */

#ifdef GL_SGIX_ycrcb

#endif /* GL_SGIX_ycrcb */

#ifdef GL_SGI_color_matrix

#endif /* GL_SGI_color_matrix */

#ifdef GL_SGI_color_table

static GLboolean _glewInit_GL_SGI_color_table (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glColorTableParameterfvSGI = (PFNGLCOLORTABLEPARAMETERFVSGIPROC)glewGetProcAddress((const GLubyte*)"glColorTableParameterfvSGI")) == null) || r;
  r = ((glColorTableParameterivSGI = (PFNGLCOLORTABLEPARAMETERIVSGIPROC)glewGetProcAddress((const GLubyte*)"glColorTableParameterivSGI")) == null) || r;
  r = ((glColorTableSGI = (PFNGLCOLORTABLESGIPROC)glewGetProcAddress((const GLubyte*)"glColorTableSGI")) == null) || r;
  r = ((glCopyColorTableSGI = (PFNGLCOPYCOLORTABLESGIPROC)glewGetProcAddress((const GLubyte*)"glCopyColorTableSGI")) == null) || r;
  r = ((glGetColorTableParameterfvSGI = (PFNGLGETCOLORTABLEPARAMETERFVSGIPROC)glewGetProcAddress((const GLubyte*)"glGetColorTableParameterfvSGI")) == null) || r;
  r = ((glGetColorTableParameterivSGI = (PFNGLGETCOLORTABLEPARAMETERIVSGIPROC)glewGetProcAddress((const GLubyte*)"glGetColorTableParameterivSGI")) == null) || r;
  r = ((glGetColorTableSGI = (PFNGLGETCOLORTABLESGIPROC)glewGetProcAddress((const GLubyte*)"glGetColorTableSGI")) == null) || r;

  return r;
}

#endif /* GL_SGI_color_table */

#ifdef GL_SGI_texture_color_table

#endif /* GL_SGI_texture_color_table */

#ifdef GL_SUNX_constant_data

static GLboolean _glewInit_GL_SUNX_constant_data (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glFinishTextureSUNX = (PFNGLFINISHTEXTURESUNXPROC)glewGetProcAddress((const GLubyte*)"glFinishTextureSUNX")) == null) || r;

  return r;
}

#endif /* GL_SUNX_constant_data */

#ifdef GL_SUN_convolution_border_modes

#endif /* GL_SUN_convolution_border_modes */

#ifdef GL_SUN_global_alpha

static GLboolean _glewInit_GL_SUN_global_alpha (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glGlobalAlphaFactorbSUN = (PFNGLGLOBALALPHAFACTORBSUNPROC)glewGetProcAddress((const GLubyte*)"glGlobalAlphaFactorbSUN")) == null) || r;
  r = ((glGlobalAlphaFactordSUN = (PFNGLGLOBALALPHAFACTORDSUNPROC)glewGetProcAddress((const GLubyte*)"glGlobalAlphaFactordSUN")) == null) || r;
  r = ((glGlobalAlphaFactorfSUN = (PFNGLGLOBALALPHAFACTORFSUNPROC)glewGetProcAddress((const GLubyte*)"glGlobalAlphaFactorfSUN")) == null) || r;
  r = ((glGlobalAlphaFactoriSUN = (PFNGLGLOBALALPHAFACTORISUNPROC)glewGetProcAddress((const GLubyte*)"glGlobalAlphaFactoriSUN")) == null) || r;
  r = ((glGlobalAlphaFactorsSUN = (PFNGLGLOBALALPHAFACTORSSUNPROC)glewGetProcAddress((const GLubyte*)"glGlobalAlphaFactorsSUN")) == null) || r;
  r = ((glGlobalAlphaFactorubSUN = (PFNGLGLOBALALPHAFACTORUBSUNPROC)glewGetProcAddress((const GLubyte*)"glGlobalAlphaFactorubSUN")) == null) || r;
  r = ((glGlobalAlphaFactoruiSUN = (PFNGLGLOBALALPHAFACTORUISUNPROC)glewGetProcAddress((const GLubyte*)"glGlobalAlphaFactoruiSUN")) == null) || r;
  r = ((glGlobalAlphaFactorusSUN = (PFNGLGLOBALALPHAFACTORUSSUNPROC)glewGetProcAddress((const GLubyte*)"glGlobalAlphaFactorusSUN")) == null) || r;

  return r;
}

#endif /* GL_SUN_global_alpha */

#ifdef GL_SUN_mesh_array

#endif /* GL_SUN_mesh_array */

#ifdef GL_SUN_read_video_pixels

static GLboolean _glewInit_GL_SUN_read_video_pixels (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glReadVideoPixelsSUN = (PFNGLREADVIDEOPIXELSSUNPROC)glewGetProcAddress((const GLubyte*)"glReadVideoPixelsSUN")) == null) || r;

  return r;
}

#endif /* GL_SUN_read_video_pixels */

#ifdef GL_SUN_slice_accum

#endif /* GL_SUN_slice_accum */

#ifdef GL_SUN_triangle_list

static GLboolean _glewInit_GL_SUN_triangle_list (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glReplacementCodePointerSUN = (PFNGLREPLACEMENTCODEPOINTERSUNPROC)glewGetProcAddress((const GLubyte*)"glReplacementCodePointerSUN")) == null) || r;
  r = ((glReplacementCodeubSUN = (PFNGLREPLACEMENTCODEUBSUNPROC)glewGetProcAddress((const GLubyte*)"glReplacementCodeubSUN")) == null) || r;
  r = ((glReplacementCodeubvSUN = (PFNGLREPLACEMENTCODEUBVSUNPROC)glewGetProcAddress((const GLubyte*)"glReplacementCodeubvSUN")) == null) || r;
  r = ((glReplacementCodeuiSUN = (PFNGLREPLACEMENTCODEUISUNPROC)glewGetProcAddress((const GLubyte*)"glReplacementCodeuiSUN")) == null) || r;
  r = ((glReplacementCodeuivSUN = (PFNGLREPLACEMENTCODEUIVSUNPROC)glewGetProcAddress((const GLubyte*)"glReplacementCodeuivSUN")) == null) || r;
  r = ((glReplacementCodeusSUN = (PFNGLREPLACEMENTCODEUSSUNPROC)glewGetProcAddress((const GLubyte*)"glReplacementCodeusSUN")) == null) || r;
  r = ((glReplacementCodeusvSUN = (PFNGLREPLACEMENTCODEUSVSUNPROC)glewGetProcAddress((const GLubyte*)"glReplacementCodeusvSUN")) == null) || r;

  return r;
}

#endif /* GL_SUN_triangle_list */

#ifdef GL_SUN_vertex

static GLboolean _glewInit_GL_SUN_vertex (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glColor3fVertex3fSUN = (PFNGLCOLOR3FVERTEX3FSUNPROC)glewGetProcAddress((const GLubyte*)"glColor3fVertex3fSUN")) == null) || r;
  r = ((glColor3fVertex3fvSUN = (PFNGLCOLOR3FVERTEX3FVSUNPROC)glewGetProcAddress((const GLubyte*)"glColor3fVertex3fvSUN")) == null) || r;
  r = ((glColor4fNormal3fVertex3fSUN = (PFNGLCOLOR4FNORMAL3FVERTEX3FSUNPROC)glewGetProcAddress((const GLubyte*)"glColor4fNormal3fVertex3fSUN")) == null) || r;
  r = ((glColor4fNormal3fVertex3fvSUN = (PFNGLCOLOR4FNORMAL3FVERTEX3FVSUNPROC)glewGetProcAddress((const GLubyte*)"glColor4fNormal3fVertex3fvSUN")) == null) || r;
  r = ((glColor4ubVertex2fSUN = (PFNGLCOLOR4UBVERTEX2FSUNPROC)glewGetProcAddress((const GLubyte*)"glColor4ubVertex2fSUN")) == null) || r;
  r = ((glColor4ubVertex2fvSUN = (PFNGLCOLOR4UBVERTEX2FVSUNPROC)glewGetProcAddress((const GLubyte*)"glColor4ubVertex2fvSUN")) == null) || r;
  r = ((glColor4ubVertex3fSUN = (PFNGLCOLOR4UBVERTEX3FSUNPROC)glewGetProcAddress((const GLubyte*)"glColor4ubVertex3fSUN")) == null) || r;
  r = ((glColor4ubVertex3fvSUN = (PFNGLCOLOR4UBVERTEX3FVSUNPROC)glewGetProcAddress((const GLubyte*)"glColor4ubVertex3fvSUN")) == null) || r;
  r = ((glNormal3fVertex3fSUN = (PFNGLNORMAL3FVERTEX3FSUNPROC)glewGetProcAddress((const GLubyte*)"glNormal3fVertex3fSUN")) == null) || r;
  r = ((glNormal3fVertex3fvSUN = (PFNGLNORMAL3FVERTEX3FVSUNPROC)glewGetProcAddress((const GLubyte*)"glNormal3fVertex3fvSUN")) == null) || r;
  r = ((glReplacementCodeuiColor3fVertex3fSUN = (PFNGLREPLACEMENTCODEUICOLOR3FVERTEX3FSUNPROC)glewGetProcAddress((const GLubyte*)"glReplacementCodeuiColor3fVertex3fSUN")) == null) || r;
  r = ((glReplacementCodeuiColor3fVertex3fvSUN = (PFNGLREPLACEMENTCODEUICOLOR3FVERTEX3FVSUNPROC)glewGetProcAddress((const GLubyte*)"glReplacementCodeuiColor3fVertex3fvSUN")) == null) || r;
  r = ((glReplacementCodeuiColor4fNormal3fVertex3fSUN = (PFNGLREPLACEMENTCODEUICOLOR4FNORMAL3FVERTEX3FSUNPROC)glewGetProcAddress((const GLubyte*)"glReplacementCodeuiColor4fNormal3fVertex3fSUN")) == null) || r;
  r = ((glReplacementCodeuiColor4fNormal3fVertex3fvSUN = (PFNGLREPLACEMENTCODEUICOLOR4FNORMAL3FVERTEX3FVSUNPROC)glewGetProcAddress((const GLubyte*)"glReplacementCodeuiColor4fNormal3fVertex3fvSUN")) == null) || r;
  r = ((glReplacementCodeuiColor4ubVertex3fSUN = (PFNGLREPLACEMENTCODEUICOLOR4UBVERTEX3FSUNPROC)glewGetProcAddress((const GLubyte*)"glReplacementCodeuiColor4ubVertex3fSUN")) == null) || r;
  r = ((glReplacementCodeuiColor4ubVertex3fvSUN = (PFNGLREPLACEMENTCODEUICOLOR4UBVERTEX3FVSUNPROC)glewGetProcAddress((const GLubyte*)"glReplacementCodeuiColor4ubVertex3fvSUN")) == null) || r;
  r = ((glReplacementCodeuiNormal3fVertex3fSUN = (PFNGLREPLACEMENTCODEUINORMAL3FVERTEX3FSUNPROC)glewGetProcAddress((const GLubyte*)"glReplacementCodeuiNormal3fVertex3fSUN")) == null) || r;
  r = ((glReplacementCodeuiNormal3fVertex3fvSUN = (PFNGLREPLACEMENTCODEUINORMAL3FVERTEX3FVSUNPROC)glewGetProcAddress((const GLubyte*)"glReplacementCodeuiNormal3fVertex3fvSUN")) == null) || r;
  r = ((glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN = (PFNGLREPLACEMENTCODEUITEXCOORD2FCOLOR4FNORMAL3FVERTEX3FSUNPROC)glewGetProcAddress((const GLubyte*)"glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN")) == null) || r;
  r = ((glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN = (PFNGLREPLACEMENTCODEUITEXCOORD2FCOLOR4FNORMAL3FVERTEX3FVSUNPROC)glewGetProcAddress((const GLubyte*)"glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN")) == null) || r;
  r = ((glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN = (PFNGLREPLACEMENTCODEUITEXCOORD2FNORMAL3FVERTEX3FSUNPROC)glewGetProcAddress((const GLubyte*)"glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN")) == null) || r;
  r = ((glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN = (PFNGLREPLACEMENTCODEUITEXCOORD2FNORMAL3FVERTEX3FVSUNPROC)glewGetProcAddress((const GLubyte*)"glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN")) == null) || r;
  r = ((glReplacementCodeuiTexCoord2fVertex3fSUN = (PFNGLREPLACEMENTCODEUITEXCOORD2FVERTEX3FSUNPROC)glewGetProcAddress((const GLubyte*)"glReplacementCodeuiTexCoord2fVertex3fSUN")) == null) || r;
  r = ((glReplacementCodeuiTexCoord2fVertex3fvSUN = (PFNGLREPLACEMENTCODEUITEXCOORD2FVERTEX3FVSUNPROC)glewGetProcAddress((const GLubyte*)"glReplacementCodeuiTexCoord2fVertex3fvSUN")) == null) || r;
  r = ((glReplacementCodeuiVertex3fSUN = (PFNGLREPLACEMENTCODEUIVERTEX3FSUNPROC)glewGetProcAddress((const GLubyte*)"glReplacementCodeuiVertex3fSUN")) == null) || r;
  r = ((glReplacementCodeuiVertex3fvSUN = (PFNGLREPLACEMENTCODEUIVERTEX3FVSUNPROC)glewGetProcAddress((const GLubyte*)"glReplacementCodeuiVertex3fvSUN")) == null) || r;
  r = ((glTexCoord2fColor3fVertex3fSUN = (PFNGLTEXCOORD2FCOLOR3FVERTEX3FSUNPROC)glewGetProcAddress((const GLubyte*)"glTexCoord2fColor3fVertex3fSUN")) == null) || r;
  r = ((glTexCoord2fColor3fVertex3fvSUN = (PFNGLTEXCOORD2FCOLOR3FVERTEX3FVSUNPROC)glewGetProcAddress((const GLubyte*)"glTexCoord2fColor3fVertex3fvSUN")) == null) || r;
  r = ((glTexCoord2fColor4fNormal3fVertex3fSUN = (PFNGLTEXCOORD2FCOLOR4FNORMAL3FVERTEX3FSUNPROC)glewGetProcAddress((const GLubyte*)"glTexCoord2fColor4fNormal3fVertex3fSUN")) == null) || r;
  r = ((glTexCoord2fColor4fNormal3fVertex3fvSUN = (PFNGLTEXCOORD2FCOLOR4FNORMAL3FVERTEX3FVSUNPROC)glewGetProcAddress((const GLubyte*)"glTexCoord2fColor4fNormal3fVertex3fvSUN")) == null) || r;
  r = ((glTexCoord2fColor4ubVertex3fSUN = (PFNGLTEXCOORD2FCOLOR4UBVERTEX3FSUNPROC)glewGetProcAddress((const GLubyte*)"glTexCoord2fColor4ubVertex3fSUN")) == null) || r;
  r = ((glTexCoord2fColor4ubVertex3fvSUN = (PFNGLTEXCOORD2FCOLOR4UBVERTEX3FVSUNPROC)glewGetProcAddress((const GLubyte*)"glTexCoord2fColor4ubVertex3fvSUN")) == null) || r;
  r = ((glTexCoord2fNormal3fVertex3fSUN = (PFNGLTEXCOORD2FNORMAL3FVERTEX3FSUNPROC)glewGetProcAddress((const GLubyte*)"glTexCoord2fNormal3fVertex3fSUN")) == null) || r;
  r = ((glTexCoord2fNormal3fVertex3fvSUN = (PFNGLTEXCOORD2FNORMAL3FVERTEX3FVSUNPROC)glewGetProcAddress((const GLubyte*)"glTexCoord2fNormal3fVertex3fvSUN")) == null) || r;
  r = ((glTexCoord2fVertex3fSUN = (PFNGLTEXCOORD2FVERTEX3FSUNPROC)glewGetProcAddress((const GLubyte*)"glTexCoord2fVertex3fSUN")) == null) || r;
  r = ((glTexCoord2fVertex3fvSUN = (PFNGLTEXCOORD2FVERTEX3FVSUNPROC)glewGetProcAddress((const GLubyte*)"glTexCoord2fVertex3fvSUN")) == null) || r;
  r = ((glTexCoord4fColor4fNormal3fVertex4fSUN = (PFNGLTEXCOORD4FCOLOR4FNORMAL3FVERTEX4FSUNPROC)glewGetProcAddress((const GLubyte*)"glTexCoord4fColor4fNormal3fVertex4fSUN")) == null) || r;
  r = ((glTexCoord4fColor4fNormal3fVertex4fvSUN = (PFNGLTEXCOORD4FCOLOR4FNORMAL3FVERTEX4FVSUNPROC)glewGetProcAddress((const GLubyte*)"glTexCoord4fColor4fNormal3fVertex4fvSUN")) == null) || r;
  r = ((glTexCoord4fVertex4fSUN = (PFNGLTEXCOORD4FVERTEX4FSUNPROC)glewGetProcAddress((const GLubyte*)"glTexCoord4fVertex4fSUN")) == null) || r;
  r = ((glTexCoord4fVertex4fvSUN = (PFNGLTEXCOORD4FVERTEX4FVSUNPROC)glewGetProcAddress((const GLubyte*)"glTexCoord4fVertex4fvSUN")) == null) || r;

  return r;
}

#endif /* GL_SUN_vertex */

#ifdef GL_WIN_phong_shading

#endif /* GL_WIN_phong_shading */

#ifdef GL_WIN_specular_fog

#endif /* GL_WIN_specular_fog */

#ifdef GL_WIN_swap_hint

static GLboolean _glewInit_GL_WIN_swap_hint (GLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glAddSwapHintRectWIN = (PFNGLADDSWAPHINTRECTWINPROC)glewGetProcAddress((const GLubyte*)"glAddSwapHintRectWIN")) == null) || r;

  return r;
}

#endif /* GL_WIN_swap_hint */

/* ------------------------------------------------------------------------- */

/* 
 * Search for name in the extensions string. Use of strstr()
 * is not sufficient because extension names can be prefixes of
 * other extension names. Could use strtok() but the constant
 * string returned by glGetString might be in read-only memory.
 */
GLboolean glewGetExtension (const char* name)
{    
  GLubyte* p;
  GLubyte* end;
  GLuint len = _glewStrLen((const GLubyte*)name);
  p = (GLubyte*)glGetString(GL_EXTENSIONS);
  if (0 == p) return GL_FALSE;
  end = p + _glewStrLen(p);
  while (p < end)
  {
    GLuint n = _glewStrCLen(p, ' ');
    if (len == n && _glewStrSame((const GLubyte*)name, p, n)) return GL_TRUE;
    p += n+1;
  }
  return GL_FALSE;
}

/* ------------------------------------------------------------------------- */

#ifndef GLEW_MX
static
#endif
GLenum glewContextInit (GLEW_CONTEXT_ARG_DEF_LIST)
{
  const GLubyte* s;
  GLuint dot;
  GLint major, minor;
  /* query opengl version */
  s = glGetString(GL_VERSION);
  dot = _glewStrCLen(s, '.');
  if (dot == 0)
    return GLEW_ERROR_NO_GL_VERSION;
  
  major = s[dot-1]-'0';
  minor = s[dot+1]-'0';

  if (minor < 0 || minor > 9)
    minor = 0;
  if (major<0 || major>9)
    return GLEW_ERROR_NO_GL_VERSION;
  

  if (major == 1 && minor == 0)
  {
    return GLEW_ERROR_GL_VERSION_10_ONLY;
  }
  else
  {
    CONST_CAST(GLEW_VERSION_3_2) = ( major > 3 )               || ( major == 3 && minor >= 2 ) ? GL_TRUE : GL_FALSE;
    CONST_CAST(GLEW_VERSION_3_1) = GLEW_VERSION_3_2 == GL_TRUE || ( major == 3 && minor >= 1 ) ? GL_TRUE : GL_FALSE;
    CONST_CAST(GLEW_VERSION_3_0) = GLEW_VERSION_3_1 == GL_TRUE || ( major == 3               ) ? GL_TRUE : GL_FALSE;
    CONST_CAST(GLEW_VERSION_2_1) = GLEW_VERSION_3_0 == GL_TRUE || ( major == 2 && minor >= 1 ) ? GL_TRUE : GL_FALSE;    
    CONST_CAST(GLEW_VERSION_2_0) = GLEW_VERSION_2_1 == GL_TRUE || ( major == 2               ) ? GL_TRUE : GL_FALSE;
    CONST_CAST(GLEW_VERSION_1_5) = GLEW_VERSION_2_0 == GL_TRUE || ( major == 1 && minor >= 5 ) ? GL_TRUE : GL_FALSE;
    CONST_CAST(GLEW_VERSION_1_4) = GLEW_VERSION_1_5 == GL_TRUE || ( major == 1 && minor >= 4 ) ? GL_TRUE : GL_FALSE;
    CONST_CAST(GLEW_VERSION_1_3) = GLEW_VERSION_1_4 == GL_TRUE || ( major == 1 && minor >= 3 ) ? GL_TRUE : GL_FALSE;
    CONST_CAST(GLEW_VERSION_1_2) = GLEW_VERSION_1_3 == GL_TRUE || ( major == 1 && minor >= 2 ) ? GL_TRUE : GL_FALSE;
    CONST_CAST(GLEW_VERSION_1_1) = GLEW_VERSION_1_2 == GL_TRUE || ( major == 1 && minor >= 1 ) ? GL_TRUE : GL_FALSE;
  }
  /* initialize extensions */
#ifdef GL_VERSION_1_2
  if (glewExperimental || GLEW_VERSION_1_2) CONST_CAST(GLEW_VERSION_1_2) = !_glewInit_GL_VERSION_1_2(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_VERSION_1_2 */
#ifdef GL_VERSION_1_3
  if (glewExperimental || GLEW_VERSION_1_3) CONST_CAST(GLEW_VERSION_1_3) = !_glewInit_GL_VERSION_1_3(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_VERSION_1_3 */
#ifdef GL_VERSION_1_4
  if (glewExperimental || GLEW_VERSION_1_4) CONST_CAST(GLEW_VERSION_1_4) = !_glewInit_GL_VERSION_1_4(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_VERSION_1_4 */
#ifdef GL_VERSION_1_5
  if (glewExperimental || GLEW_VERSION_1_5) CONST_CAST(GLEW_VERSION_1_5) = !_glewInit_GL_VERSION_1_5(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_VERSION_1_5 */
#ifdef GL_VERSION_2_0
  if (glewExperimental || GLEW_VERSION_2_0) CONST_CAST(GLEW_VERSION_2_0) = !_glewInit_GL_VERSION_2_0(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_VERSION_2_0 */
#ifdef GL_VERSION_2_1
  if (glewExperimental || GLEW_VERSION_2_1) CONST_CAST(GLEW_VERSION_2_1) = !_glewInit_GL_VERSION_2_1(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_VERSION_2_1 */
#ifdef GL_VERSION_3_0
  if (glewExperimental || GLEW_VERSION_3_0) CONST_CAST(GLEW_VERSION_3_0) = !_glewInit_GL_VERSION_3_0(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_VERSION_3_0 */
#ifdef GL_VERSION_3_1
  if (glewExperimental || GLEW_VERSION_3_1) CONST_CAST(GLEW_VERSION_3_1) = !_glewInit_GL_VERSION_3_1(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_VERSION_3_1 */
#ifdef GL_VERSION_3_2
  if (glewExperimental || GLEW_VERSION_3_2) CONST_CAST(GLEW_VERSION_3_2) = !_glewInit_GL_VERSION_3_2(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_VERSION_3_2 */
#ifdef GL_3DFX_multisample
  CONST_CAST(GLEW_3DFX_multisample) = glewGetExtension("GL_3DFX_multisample");
#endif /* GL_3DFX_multisample */
#ifdef GL_3DFX_tbuffer
  CONST_CAST(GLEW_3DFX_tbuffer) = glewGetExtension("GL_3DFX_tbuffer");
  if (glewExperimental || GLEW_3DFX_tbuffer) CONST_CAST(GLEW_3DFX_tbuffer) = !_glewInit_GL_3DFX_tbuffer(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_3DFX_tbuffer */
#ifdef GL_3DFX_texture_compression_FXT1
  CONST_CAST(GLEW_3DFX_texture_compression_FXT1) = glewGetExtension("GL_3DFX_texture_compression_FXT1");
#endif /* GL_3DFX_texture_compression_FXT1 */
#ifdef GL_AMD_draw_buffers_blend
  CONST_CAST(GLEW_AMD_draw_buffers_blend) = glewGetExtension("GL_AMD_draw_buffers_blend");
  if (glewExperimental || GLEW_AMD_draw_buffers_blend) CONST_CAST(GLEW_AMD_draw_buffers_blend) = !_glewInit_GL_AMD_draw_buffers_blend(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_AMD_draw_buffers_blend */
#ifdef GL_AMD_performance_monitor
  CONST_CAST(GLEW_AMD_performance_monitor) = glewGetExtension("GL_AMD_performance_monitor");
  if (glewExperimental || GLEW_AMD_performance_monitor) CONST_CAST(GLEW_AMD_performance_monitor) = !_glewInit_GL_AMD_performance_monitor(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_AMD_performance_monitor */
#ifdef GL_AMD_seamless_cubemap_per_texture
  CONST_CAST(GLEW_AMD_seamless_cubemap_per_texture) = glewGetExtension("GL_AMD_seamless_cubemap_per_texture");
#endif /* GL_AMD_seamless_cubemap_per_texture */
#ifdef GL_AMD_shader_stencil_export
  CONST_CAST(GLEW_AMD_shader_stencil_export) = glewGetExtension("GL_AMD_shader_stencil_export");
#endif /* GL_AMD_shader_stencil_export */
#ifdef GL_AMD_texture_texture4
  CONST_CAST(GLEW_AMD_texture_texture4) = glewGetExtension("GL_AMD_texture_texture4");
#endif /* GL_AMD_texture_texture4 */
#ifdef GL_AMD_vertex_shader_tessellator
  CONST_CAST(GLEW_AMD_vertex_shader_tessellator) = glewGetExtension("GL_AMD_vertex_shader_tessellator");
  if (glewExperimental || GLEW_AMD_vertex_shader_tessellator) CONST_CAST(GLEW_AMD_vertex_shader_tessellator) = !_glewInit_GL_AMD_vertex_shader_tessellator(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_AMD_vertex_shader_tessellator */
#ifdef GL_APPLE_aux_depth_stencil
  CONST_CAST(GLEW_APPLE_aux_depth_stencil) = glewGetExtension("GL_APPLE_aux_depth_stencil");
#endif /* GL_APPLE_aux_depth_stencil */
#ifdef GL_APPLE_client_storage
  CONST_CAST(GLEW_APPLE_client_storage) = glewGetExtension("GL_APPLE_client_storage");
#endif /* GL_APPLE_client_storage */
#ifdef GL_APPLE_element_array
  CONST_CAST(GLEW_APPLE_element_array) = glewGetExtension("GL_APPLE_element_array");
  if (glewExperimental || GLEW_APPLE_element_array) CONST_CAST(GLEW_APPLE_element_array) = !_glewInit_GL_APPLE_element_array(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_APPLE_element_array */
#ifdef GL_APPLE_fence
  CONST_CAST(GLEW_APPLE_fence) = glewGetExtension("GL_APPLE_fence");
  if (glewExperimental || GLEW_APPLE_fence) CONST_CAST(GLEW_APPLE_fence) = !_glewInit_GL_APPLE_fence(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_APPLE_fence */
#ifdef GL_APPLE_float_pixels
  CONST_CAST(GLEW_APPLE_float_pixels) = glewGetExtension("GL_APPLE_float_pixels");
#endif /* GL_APPLE_float_pixels */
#ifdef GL_APPLE_flush_buffer_range
  CONST_CAST(GLEW_APPLE_flush_buffer_range) = glewGetExtension("GL_APPLE_flush_buffer_range");
  if (glewExperimental || GLEW_APPLE_flush_buffer_range) CONST_CAST(GLEW_APPLE_flush_buffer_range) = !_glewInit_GL_APPLE_flush_buffer_range(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_APPLE_flush_buffer_range */
#ifdef GL_APPLE_object_purgeable
  CONST_CAST(GLEW_APPLE_object_purgeable) = glewGetExtension("GL_APPLE_object_purgeable");
  if (glewExperimental || GLEW_APPLE_object_purgeable) CONST_CAST(GLEW_APPLE_object_purgeable) = !_glewInit_GL_APPLE_object_purgeable(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_APPLE_object_purgeable */
#ifdef GL_APPLE_pixel_buffer
  CONST_CAST(GLEW_APPLE_pixel_buffer) = glewGetExtension("GL_APPLE_pixel_buffer");
#endif /* GL_APPLE_pixel_buffer */
#ifdef GL_APPLE_rgb_422
  CONST_CAST(GLEW_APPLE_rgb_422) = glewGetExtension("GL_APPLE_rgb_422");
#endif /* GL_APPLE_rgb_422 */
#ifdef GL_APPLE_row_bytes
  CONST_CAST(GLEW_APPLE_row_bytes) = glewGetExtension("GL_APPLE_row_bytes");
#endif /* GL_APPLE_row_bytes */
#ifdef GL_APPLE_specular_vector
  CONST_CAST(GLEW_APPLE_specular_vector) = glewGetExtension("GL_APPLE_specular_vector");
#endif /* GL_APPLE_specular_vector */
#ifdef GL_APPLE_texture_range
  CONST_CAST(GLEW_APPLE_texture_range) = glewGetExtension("GL_APPLE_texture_range");
  if (glewExperimental || GLEW_APPLE_texture_range) CONST_CAST(GLEW_APPLE_texture_range) = !_glewInit_GL_APPLE_texture_range(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_APPLE_texture_range */
#ifdef GL_APPLE_transform_hint
  CONST_CAST(GLEW_APPLE_transform_hint) = glewGetExtension("GL_APPLE_transform_hint");
#endif /* GL_APPLE_transform_hint */
#ifdef GL_APPLE_vertex_array_object
  CONST_CAST(GLEW_APPLE_vertex_array_object) = glewGetExtension("GL_APPLE_vertex_array_object");
  if (glewExperimental || GLEW_APPLE_vertex_array_object) CONST_CAST(GLEW_APPLE_vertex_array_object) = !_glewInit_GL_APPLE_vertex_array_object(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_APPLE_vertex_array_object */
#ifdef GL_APPLE_vertex_array_range
  CONST_CAST(GLEW_APPLE_vertex_array_range) = glewGetExtension("GL_APPLE_vertex_array_range");
  if (glewExperimental || GLEW_APPLE_vertex_array_range) CONST_CAST(GLEW_APPLE_vertex_array_range) = !_glewInit_GL_APPLE_vertex_array_range(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_APPLE_vertex_array_range */
#ifdef GL_APPLE_vertex_program_evaluators
  CONST_CAST(GLEW_APPLE_vertex_program_evaluators) = glewGetExtension("GL_APPLE_vertex_program_evaluators");
  if (glewExperimental || GLEW_APPLE_vertex_program_evaluators) CONST_CAST(GLEW_APPLE_vertex_program_evaluators) = !_glewInit_GL_APPLE_vertex_program_evaluators(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_APPLE_vertex_program_evaluators */
#ifdef GL_APPLE_ycbcr_422
  CONST_CAST(GLEW_APPLE_ycbcr_422) = glewGetExtension("GL_APPLE_ycbcr_422");
#endif /* GL_APPLE_ycbcr_422 */
#ifdef GL_ARB_color_buffer_float
  CONST_CAST(GLEW_ARB_color_buffer_float) = glewGetExtension("GL_ARB_color_buffer_float");
  if (glewExperimental || GLEW_ARB_color_buffer_float) CONST_CAST(GLEW_ARB_color_buffer_float) = !_glewInit_GL_ARB_color_buffer_float(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_color_buffer_float */
#ifdef GL_ARB_compatibility
  CONST_CAST(GLEW_ARB_compatibility) = glewGetExtension("GL_ARB_compatibility");
#endif /* GL_ARB_compatibility */
#ifdef GL_ARB_copy_buffer
  CONST_CAST(GLEW_ARB_copy_buffer) = glewGetExtension("GL_ARB_copy_buffer");
  if (glewExperimental || GLEW_ARB_copy_buffer) CONST_CAST(GLEW_ARB_copy_buffer) = !_glewInit_GL_ARB_copy_buffer(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_copy_buffer */
#ifdef GL_ARB_depth_buffer_float
  CONST_CAST(GLEW_ARB_depth_buffer_float) = glewGetExtension("GL_ARB_depth_buffer_float");
#endif /* GL_ARB_depth_buffer_float */
#ifdef GL_ARB_depth_clamp
  CONST_CAST(GLEW_ARB_depth_clamp) = glewGetExtension("GL_ARB_depth_clamp");
#endif /* GL_ARB_depth_clamp */
#ifdef GL_ARB_depth_texture
  CONST_CAST(GLEW_ARB_depth_texture) = glewGetExtension("GL_ARB_depth_texture");
#endif /* GL_ARB_depth_texture */
#ifdef GL_ARB_draw_buffers
  CONST_CAST(GLEW_ARB_draw_buffers) = glewGetExtension("GL_ARB_draw_buffers");
  if (glewExperimental || GLEW_ARB_draw_buffers) CONST_CAST(GLEW_ARB_draw_buffers) = !_glewInit_GL_ARB_draw_buffers(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_draw_buffers */
#ifdef GL_ARB_draw_buffers_blend
  CONST_CAST(GLEW_ARB_draw_buffers_blend) = glewGetExtension("GL_ARB_draw_buffers_blend");
  if (glewExperimental || GLEW_ARB_draw_buffers_blend) CONST_CAST(GLEW_ARB_draw_buffers_blend) = !_glewInit_GL_ARB_draw_buffers_blend(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_draw_buffers_blend */
#ifdef GL_ARB_draw_elements_base_vertex
  CONST_CAST(GLEW_ARB_draw_elements_base_vertex) = glewGetExtension("GL_ARB_draw_elements_base_vertex");
  if (glewExperimental || GLEW_ARB_draw_elements_base_vertex) CONST_CAST(GLEW_ARB_draw_elements_base_vertex) = !_glewInit_GL_ARB_draw_elements_base_vertex(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_draw_elements_base_vertex */
#ifdef GL_ARB_draw_instanced
  CONST_CAST(GLEW_ARB_draw_instanced) = glewGetExtension("GL_ARB_draw_instanced");
  if (glewExperimental || GLEW_ARB_draw_instanced) CONST_CAST(GLEW_ARB_draw_instanced) = !_glewInit_GL_ARB_draw_instanced(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_draw_instanced */
#ifdef GL_ARB_fragment_coord_conventions
  CONST_CAST(GLEW_ARB_fragment_coord_conventions) = glewGetExtension("GL_ARB_fragment_coord_conventions");
#endif /* GL_ARB_fragment_coord_conventions */
#ifdef GL_ARB_fragment_program
  CONST_CAST(GLEW_ARB_fragment_program) = glewGetExtension("GL_ARB_fragment_program");
#endif /* GL_ARB_fragment_program */
#ifdef GL_ARB_fragment_program_shadow
  CONST_CAST(GLEW_ARB_fragment_program_shadow) = glewGetExtension("GL_ARB_fragment_program_shadow");
#endif /* GL_ARB_fragment_program_shadow */
#ifdef GL_ARB_fragment_shader
  CONST_CAST(GLEW_ARB_fragment_shader) = glewGetExtension("GL_ARB_fragment_shader");
#endif /* GL_ARB_fragment_shader */
#ifdef GL_ARB_framebuffer_object
  CONST_CAST(GLEW_ARB_framebuffer_object) = glewGetExtension("GL_ARB_framebuffer_object");
  if (glewExperimental || GLEW_ARB_framebuffer_object) CONST_CAST(GLEW_ARB_framebuffer_object) = !_glewInit_GL_ARB_framebuffer_object(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_framebuffer_object */
#ifdef GL_ARB_framebuffer_sRGB
  CONST_CAST(GLEW_ARB_framebuffer_sRGB) = glewGetExtension("GL_ARB_framebuffer_sRGB");
#endif /* GL_ARB_framebuffer_sRGB */
#ifdef GL_ARB_geometry_shader4
  CONST_CAST(GLEW_ARB_geometry_shader4) = glewGetExtension("GL_ARB_geometry_shader4");
  if (glewExperimental || GLEW_ARB_geometry_shader4) CONST_CAST(GLEW_ARB_geometry_shader4) = !_glewInit_GL_ARB_geometry_shader4(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_geometry_shader4 */
#ifdef GL_ARB_half_float_pixel
  CONST_CAST(GLEW_ARB_half_float_pixel) = glewGetExtension("GL_ARB_half_float_pixel");
#endif /* GL_ARB_half_float_pixel */
#ifdef GL_ARB_half_float_vertex
  CONST_CAST(GLEW_ARB_half_float_vertex) = glewGetExtension("GL_ARB_half_float_vertex");
#endif /* GL_ARB_half_float_vertex */
#ifdef GL_ARB_imaging
  CONST_CAST(GLEW_ARB_imaging) = glewGetExtension("GL_ARB_imaging");
  if (glewExperimental || GLEW_ARB_imaging) CONST_CAST(GLEW_ARB_imaging) = !_glewInit_GL_ARB_imaging(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_imaging */
#ifdef GL_ARB_instanced_arrays
  CONST_CAST(GLEW_ARB_instanced_arrays) = glewGetExtension("GL_ARB_instanced_arrays");
  if (glewExperimental || GLEW_ARB_instanced_arrays) CONST_CAST(GLEW_ARB_instanced_arrays) = !_glewInit_GL_ARB_instanced_arrays(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_instanced_arrays */
#ifdef GL_ARB_map_buffer_range
  CONST_CAST(GLEW_ARB_map_buffer_range) = glewGetExtension("GL_ARB_map_buffer_range");
  if (glewExperimental || GLEW_ARB_map_buffer_range) CONST_CAST(GLEW_ARB_map_buffer_range) = !_glewInit_GL_ARB_map_buffer_range(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_map_buffer_range */
#ifdef GL_ARB_matrix_palette
  CONST_CAST(GLEW_ARB_matrix_palette) = glewGetExtension("GL_ARB_matrix_palette");
  if (glewExperimental || GLEW_ARB_matrix_palette) CONST_CAST(GLEW_ARB_matrix_palette) = !_glewInit_GL_ARB_matrix_palette(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_matrix_palette */
#ifdef GL_ARB_multisample
  CONST_CAST(GLEW_ARB_multisample) = glewGetExtension("GL_ARB_multisample");
  if (glewExperimental || GLEW_ARB_multisample) CONST_CAST(GLEW_ARB_multisample) = !_glewInit_GL_ARB_multisample(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_multisample */
#ifdef GL_ARB_multitexture
  CONST_CAST(GLEW_ARB_multitexture) = glewGetExtension("GL_ARB_multitexture");
  if (glewExperimental || GLEW_ARB_multitexture) CONST_CAST(GLEW_ARB_multitexture) = !_glewInit_GL_ARB_multitexture(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_multitexture */
#ifdef GL_ARB_occlusion_query
  CONST_CAST(GLEW_ARB_occlusion_query) = glewGetExtension("GL_ARB_occlusion_query");
  if (glewExperimental || GLEW_ARB_occlusion_query) CONST_CAST(GLEW_ARB_occlusion_query) = !_glewInit_GL_ARB_occlusion_query(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_occlusion_query */
#ifdef GL_ARB_pixel_buffer_object
  CONST_CAST(GLEW_ARB_pixel_buffer_object) = glewGetExtension("GL_ARB_pixel_buffer_object");
#endif /* GL_ARB_pixel_buffer_object */
#ifdef GL_ARB_point_parameters
  CONST_CAST(GLEW_ARB_point_parameters) = glewGetExtension("GL_ARB_point_parameters");
  if (glewExperimental || GLEW_ARB_point_parameters) CONST_CAST(GLEW_ARB_point_parameters) = !_glewInit_GL_ARB_point_parameters(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_point_parameters */
#ifdef GL_ARB_point_sprite
  CONST_CAST(GLEW_ARB_point_sprite) = glewGetExtension("GL_ARB_point_sprite");
#endif /* GL_ARB_point_sprite */
#ifdef GL_ARB_provoking_vertex
  CONST_CAST(GLEW_ARB_provoking_vertex) = glewGetExtension("GL_ARB_provoking_vertex");
  if (glewExperimental || GLEW_ARB_provoking_vertex) CONST_CAST(GLEW_ARB_provoking_vertex) = !_glewInit_GL_ARB_provoking_vertex(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_provoking_vertex */
#ifdef GL_ARB_sample_shading
  CONST_CAST(GLEW_ARB_sample_shading) = glewGetExtension("GL_ARB_sample_shading");
  if (glewExperimental || GLEW_ARB_sample_shading) CONST_CAST(GLEW_ARB_sample_shading) = !_glewInit_GL_ARB_sample_shading(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_sample_shading */
#ifdef GL_ARB_seamless_cube_map
  CONST_CAST(GLEW_ARB_seamless_cube_map) = glewGetExtension("GL_ARB_seamless_cube_map");
#endif /* GL_ARB_seamless_cube_map */
#ifdef GL_ARB_shader_objects
  CONST_CAST(GLEW_ARB_shader_objects) = glewGetExtension("GL_ARB_shader_objects");
  if (glewExperimental || GLEW_ARB_shader_objects) CONST_CAST(GLEW_ARB_shader_objects) = !_glewInit_GL_ARB_shader_objects(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_shader_objects */
#ifdef GL_ARB_shader_texture_lod
  CONST_CAST(GLEW_ARB_shader_texture_lod) = glewGetExtension("GL_ARB_shader_texture_lod");
#endif /* GL_ARB_shader_texture_lod */
#ifdef GL_ARB_shading_language_100
  CONST_CAST(GLEW_ARB_shading_language_100) = glewGetExtension("GL_ARB_shading_language_100");
#endif /* GL_ARB_shading_language_100 */
#ifdef GL_ARB_shadow
  CONST_CAST(GLEW_ARB_shadow) = glewGetExtension("GL_ARB_shadow");
#endif /* GL_ARB_shadow */
#ifdef GL_ARB_shadow_ambient
  CONST_CAST(GLEW_ARB_shadow_ambient) = glewGetExtension("GL_ARB_shadow_ambient");
#endif /* GL_ARB_shadow_ambient */
#ifdef GL_ARB_sync
  CONST_CAST(GLEW_ARB_sync) = glewGetExtension("GL_ARB_sync");
  if (glewExperimental || GLEW_ARB_sync) CONST_CAST(GLEW_ARB_sync) = !_glewInit_GL_ARB_sync(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_sync */
#ifdef GL_ARB_texture_border_clamp
  CONST_CAST(GLEW_ARB_texture_border_clamp) = glewGetExtension("GL_ARB_texture_border_clamp");
#endif /* GL_ARB_texture_border_clamp */
#ifdef GL_ARB_texture_buffer_object
  CONST_CAST(GLEW_ARB_texture_buffer_object) = glewGetExtension("GL_ARB_texture_buffer_object");
  if (glewExperimental || GLEW_ARB_texture_buffer_object) CONST_CAST(GLEW_ARB_texture_buffer_object) = !_glewInit_GL_ARB_texture_buffer_object(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_texture_buffer_object */
#ifdef GL_ARB_texture_compression
  CONST_CAST(GLEW_ARB_texture_compression) = glewGetExtension("GL_ARB_texture_compression");
  if (glewExperimental || GLEW_ARB_texture_compression) CONST_CAST(GLEW_ARB_texture_compression) = !_glewInit_GL_ARB_texture_compression(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_texture_compression */
#ifdef GL_ARB_texture_compression_rgtc
  CONST_CAST(GLEW_ARB_texture_compression_rgtc) = glewGetExtension("GL_ARB_texture_compression_rgtc");
#endif /* GL_ARB_texture_compression_rgtc */
#ifdef GL_ARB_texture_cube_map
  CONST_CAST(GLEW_ARB_texture_cube_map) = glewGetExtension("GL_ARB_texture_cube_map");
#endif /* GL_ARB_texture_cube_map */
#ifdef GL_ARB_texture_cube_map_array
  CONST_CAST(GLEW_ARB_texture_cube_map_array) = glewGetExtension("GL_ARB_texture_cube_map_array");
#endif /* GL_ARB_texture_cube_map_array */
#ifdef GL_ARB_texture_env_add
  CONST_CAST(GLEW_ARB_texture_env_add) = glewGetExtension("GL_ARB_texture_env_add");
#endif /* GL_ARB_texture_env_add */
#ifdef GL_ARB_texture_env_combine
  CONST_CAST(GLEW_ARB_texture_env_combine) = glewGetExtension("GL_ARB_texture_env_combine");
#endif /* GL_ARB_texture_env_combine */
#ifdef GL_ARB_texture_env_crossbar
  CONST_CAST(GLEW_ARB_texture_env_crossbar) = glewGetExtension("GL_ARB_texture_env_crossbar");
#endif /* GL_ARB_texture_env_crossbar */
#ifdef GL_ARB_texture_env_dot3
  CONST_CAST(GLEW_ARB_texture_env_dot3) = glewGetExtension("GL_ARB_texture_env_dot3");
#endif /* GL_ARB_texture_env_dot3 */
#ifdef GL_ARB_texture_float
  CONST_CAST(GLEW_ARB_texture_float) = glewGetExtension("GL_ARB_texture_float");
#endif /* GL_ARB_texture_float */
#ifdef GL_ARB_texture_gather
  CONST_CAST(GLEW_ARB_texture_gather) = glewGetExtension("GL_ARB_texture_gather");
#endif /* GL_ARB_texture_gather */
#ifdef GL_ARB_texture_mirrored_repeat
  CONST_CAST(GLEW_ARB_texture_mirrored_repeat) = glewGetExtension("GL_ARB_texture_mirrored_repeat");
#endif /* GL_ARB_texture_mirrored_repeat */
#ifdef GL_ARB_texture_multisample
  CONST_CAST(GLEW_ARB_texture_multisample) = glewGetExtension("GL_ARB_texture_multisample");
  if (glewExperimental || GLEW_ARB_texture_multisample) CONST_CAST(GLEW_ARB_texture_multisample) = !_glewInit_GL_ARB_texture_multisample(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_texture_multisample */
#ifdef GL_ARB_texture_non_power_of_two
  CONST_CAST(GLEW_ARB_texture_non_power_of_two) = glewGetExtension("GL_ARB_texture_non_power_of_two");
#endif /* GL_ARB_texture_non_power_of_two */
#ifdef GL_ARB_texture_query_lod
  CONST_CAST(GLEW_ARB_texture_query_lod) = glewGetExtension("GL_ARB_texture_query_lod");
#endif /* GL_ARB_texture_query_lod */
#ifdef GL_ARB_texture_rectangle
  CONST_CAST(GLEW_ARB_texture_rectangle) = glewGetExtension("GL_ARB_texture_rectangle");
#endif /* GL_ARB_texture_rectangle */
#ifdef GL_ARB_texture_rg
  CONST_CAST(GLEW_ARB_texture_rg) = glewGetExtension("GL_ARB_texture_rg");
#endif /* GL_ARB_texture_rg */
#ifdef GL_ARB_transpose_matrix
  CONST_CAST(GLEW_ARB_transpose_matrix) = glewGetExtension("GL_ARB_transpose_matrix");
  if (glewExperimental || GLEW_ARB_transpose_matrix) CONST_CAST(GLEW_ARB_transpose_matrix) = !_glewInit_GL_ARB_transpose_matrix(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_transpose_matrix */
#ifdef GL_ARB_uniform_buffer_object
  CONST_CAST(GLEW_ARB_uniform_buffer_object) = glewGetExtension("GL_ARB_uniform_buffer_object");
  if (glewExperimental || GLEW_ARB_uniform_buffer_object) CONST_CAST(GLEW_ARB_uniform_buffer_object) = !_glewInit_GL_ARB_uniform_buffer_object(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_uniform_buffer_object */
#ifdef GL_ARB_vertex_array_bgra
  CONST_CAST(GLEW_ARB_vertex_array_bgra) = glewGetExtension("GL_ARB_vertex_array_bgra");
#endif /* GL_ARB_vertex_array_bgra */
#ifdef GL_ARB_vertex_array_object
  CONST_CAST(GLEW_ARB_vertex_array_object) = glewGetExtension("GL_ARB_vertex_array_object");
  if (glewExperimental || GLEW_ARB_vertex_array_object) CONST_CAST(GLEW_ARB_vertex_array_object) = !_glewInit_GL_ARB_vertex_array_object(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_vertex_array_object */
#ifdef GL_ARB_vertex_blend
  CONST_CAST(GLEW_ARB_vertex_blend) = glewGetExtension("GL_ARB_vertex_blend");
  if (glewExperimental || GLEW_ARB_vertex_blend) CONST_CAST(GLEW_ARB_vertex_blend) = !_glewInit_GL_ARB_vertex_blend(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_vertex_blend */
#ifdef GL_ARB_vertex_buffer_object
  CONST_CAST(GLEW_ARB_vertex_buffer_object) = glewGetExtension("GL_ARB_vertex_buffer_object");
  if (glewExperimental || GLEW_ARB_vertex_buffer_object) CONST_CAST(GLEW_ARB_vertex_buffer_object) = !_glewInit_GL_ARB_vertex_buffer_object(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_vertex_buffer_object */
#ifdef GL_ARB_vertex_program
  CONST_CAST(GLEW_ARB_vertex_program) = glewGetExtension("GL_ARB_vertex_program");
  if (glewExperimental || GLEW_ARB_vertex_program) CONST_CAST(GLEW_ARB_vertex_program) = !_glewInit_GL_ARB_vertex_program(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_vertex_program */
#ifdef GL_ARB_vertex_shader
  CONST_CAST(GLEW_ARB_vertex_shader) = glewGetExtension("GL_ARB_vertex_shader");
  if (glewExperimental || GLEW_ARB_vertex_shader) CONST_CAST(GLEW_ARB_vertex_shader) = !_glewInit_GL_ARB_vertex_shader(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_vertex_shader */
#ifdef GL_ARB_window_pos
  CONST_CAST(GLEW_ARB_window_pos) = glewGetExtension("GL_ARB_window_pos");
  if (glewExperimental || GLEW_ARB_window_pos) CONST_CAST(GLEW_ARB_window_pos) = !_glewInit_GL_ARB_window_pos(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ARB_window_pos */
#ifdef GL_ATIX_point_sprites
  CONST_CAST(GLEW_ATIX_point_sprites) = glewGetExtension("GL_ATIX_point_sprites");
#endif /* GL_ATIX_point_sprites */
#ifdef GL_ATIX_texture_env_combine3
  CONST_CAST(GLEW_ATIX_texture_env_combine3) = glewGetExtension("GL_ATIX_texture_env_combine3");
#endif /* GL_ATIX_texture_env_combine3 */
#ifdef GL_ATIX_texture_env_route
  CONST_CAST(GLEW_ATIX_texture_env_route) = glewGetExtension("GL_ATIX_texture_env_route");
#endif /* GL_ATIX_texture_env_route */
#ifdef GL_ATIX_vertex_shader_output_point_size
  CONST_CAST(GLEW_ATIX_vertex_shader_output_point_size) = glewGetExtension("GL_ATIX_vertex_shader_output_point_size");
#endif /* GL_ATIX_vertex_shader_output_point_size */
#ifdef GL_ATI_draw_buffers
  CONST_CAST(GLEW_ATI_draw_buffers) = glewGetExtension("GL_ATI_draw_buffers");
  if (glewExperimental || GLEW_ATI_draw_buffers) CONST_CAST(GLEW_ATI_draw_buffers) = !_glewInit_GL_ATI_draw_buffers(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ATI_draw_buffers */
#ifdef GL_ATI_element_array
  CONST_CAST(GLEW_ATI_element_array) = glewGetExtension("GL_ATI_element_array");
  if (glewExperimental || GLEW_ATI_element_array) CONST_CAST(GLEW_ATI_element_array) = !_glewInit_GL_ATI_element_array(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ATI_element_array */
#ifdef GL_ATI_envmap_bumpmap
  CONST_CAST(GLEW_ATI_envmap_bumpmap) = glewGetExtension("GL_ATI_envmap_bumpmap");
  if (glewExperimental || GLEW_ATI_envmap_bumpmap) CONST_CAST(GLEW_ATI_envmap_bumpmap) = !_glewInit_GL_ATI_envmap_bumpmap(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ATI_envmap_bumpmap */
#ifdef GL_ATI_fragment_shader
  CONST_CAST(GLEW_ATI_fragment_shader) = glewGetExtension("GL_ATI_fragment_shader");
  if (glewExperimental || GLEW_ATI_fragment_shader) CONST_CAST(GLEW_ATI_fragment_shader) = !_glewInit_GL_ATI_fragment_shader(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ATI_fragment_shader */
#ifdef GL_ATI_map_object_buffer
  CONST_CAST(GLEW_ATI_map_object_buffer) = glewGetExtension("GL_ATI_map_object_buffer");
  if (glewExperimental || GLEW_ATI_map_object_buffer) CONST_CAST(GLEW_ATI_map_object_buffer) = !_glewInit_GL_ATI_map_object_buffer(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ATI_map_object_buffer */
#ifdef GL_ATI_meminfo
  CONST_CAST(GLEW_ATI_meminfo) = glewGetExtension("GL_ATI_meminfo");
#endif /* GL_ATI_meminfo */
#ifdef GL_ATI_pn_triangles
  CONST_CAST(GLEW_ATI_pn_triangles) = glewGetExtension("GL_ATI_pn_triangles");
  if (glewExperimental || GLEW_ATI_pn_triangles) CONST_CAST(GLEW_ATI_pn_triangles) = !_glewInit_GL_ATI_pn_triangles(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ATI_pn_triangles */
#ifdef GL_ATI_separate_stencil
  CONST_CAST(GLEW_ATI_separate_stencil) = glewGetExtension("GL_ATI_separate_stencil");
  if (glewExperimental || GLEW_ATI_separate_stencil) CONST_CAST(GLEW_ATI_separate_stencil) = !_glewInit_GL_ATI_separate_stencil(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ATI_separate_stencil */
#ifdef GL_ATI_shader_texture_lod
  CONST_CAST(GLEW_ATI_shader_texture_lod) = glewGetExtension("GL_ATI_shader_texture_lod");
#endif /* GL_ATI_shader_texture_lod */
#ifdef GL_ATI_text_fragment_shader
  CONST_CAST(GLEW_ATI_text_fragment_shader) = glewGetExtension("GL_ATI_text_fragment_shader");
#endif /* GL_ATI_text_fragment_shader */
#ifdef GL_ATI_texture_compression_3dc
  CONST_CAST(GLEW_ATI_texture_compression_3dc) = glewGetExtension("GL_ATI_texture_compression_3dc");
#endif /* GL_ATI_texture_compression_3dc */
#ifdef GL_ATI_texture_env_combine3
  CONST_CAST(GLEW_ATI_texture_env_combine3) = glewGetExtension("GL_ATI_texture_env_combine3");
#endif /* GL_ATI_texture_env_combine3 */
#ifdef GL_ATI_texture_float
  CONST_CAST(GLEW_ATI_texture_float) = glewGetExtension("GL_ATI_texture_float");
#endif /* GL_ATI_texture_float */
#ifdef GL_ATI_texture_mirror_once
  CONST_CAST(GLEW_ATI_texture_mirror_once) = glewGetExtension("GL_ATI_texture_mirror_once");
#endif /* GL_ATI_texture_mirror_once */
#ifdef GL_ATI_vertex_array_object
  CONST_CAST(GLEW_ATI_vertex_array_object) = glewGetExtension("GL_ATI_vertex_array_object");
  if (glewExperimental || GLEW_ATI_vertex_array_object) CONST_CAST(GLEW_ATI_vertex_array_object) = !_glewInit_GL_ATI_vertex_array_object(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ATI_vertex_array_object */
#ifdef GL_ATI_vertex_attrib_array_object
  CONST_CAST(GLEW_ATI_vertex_attrib_array_object) = glewGetExtension("GL_ATI_vertex_attrib_array_object");
  if (glewExperimental || GLEW_ATI_vertex_attrib_array_object) CONST_CAST(GLEW_ATI_vertex_attrib_array_object) = !_glewInit_GL_ATI_vertex_attrib_array_object(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ATI_vertex_attrib_array_object */
#ifdef GL_ATI_vertex_streams
  CONST_CAST(GLEW_ATI_vertex_streams) = glewGetExtension("GL_ATI_vertex_streams");
  if (glewExperimental || GLEW_ATI_vertex_streams) CONST_CAST(GLEW_ATI_vertex_streams) = !_glewInit_GL_ATI_vertex_streams(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_ATI_vertex_streams */
#ifdef GL_EXT_422_pixels
  CONST_CAST(GLEW_EXT_422_pixels) = glewGetExtension("GL_EXT_422_pixels");
#endif /* GL_EXT_422_pixels */
#ifdef GL_EXT_Cg_shader
  CONST_CAST(GLEW_EXT_Cg_shader) = glewGetExtension("GL_EXT_Cg_shader");
#endif /* GL_EXT_Cg_shader */
#ifdef GL_EXT_abgr
  CONST_CAST(GLEW_EXT_abgr) = glewGetExtension("GL_EXT_abgr");
#endif /* GL_EXT_abgr */
#ifdef GL_EXT_bgra
  CONST_CAST(GLEW_EXT_bgra) = glewGetExtension("GL_EXT_bgra");
#endif /* GL_EXT_bgra */
#ifdef GL_EXT_bindable_uniform
  CONST_CAST(GLEW_EXT_bindable_uniform) = glewGetExtension("GL_EXT_bindable_uniform");
  if (glewExperimental || GLEW_EXT_bindable_uniform) CONST_CAST(GLEW_EXT_bindable_uniform) = !_glewInit_GL_EXT_bindable_uniform(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_bindable_uniform */
#ifdef GL_EXT_blend_color
  CONST_CAST(GLEW_EXT_blend_color) = glewGetExtension("GL_EXT_blend_color");
  if (glewExperimental || GLEW_EXT_blend_color) CONST_CAST(GLEW_EXT_blend_color) = !_glewInit_GL_EXT_blend_color(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_blend_color */
#ifdef GL_EXT_blend_equation_separate
  CONST_CAST(GLEW_EXT_blend_equation_separate) = glewGetExtension("GL_EXT_blend_equation_separate");
  if (glewExperimental || GLEW_EXT_blend_equation_separate) CONST_CAST(GLEW_EXT_blend_equation_separate) = !_glewInit_GL_EXT_blend_equation_separate(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_blend_equation_separate */
#ifdef GL_EXT_blend_func_separate
  CONST_CAST(GLEW_EXT_blend_func_separate) = glewGetExtension("GL_EXT_blend_func_separate");
  if (glewExperimental || GLEW_EXT_blend_func_separate) CONST_CAST(GLEW_EXT_blend_func_separate) = !_glewInit_GL_EXT_blend_func_separate(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_blend_func_separate */
#ifdef GL_EXT_blend_logic_op
  CONST_CAST(GLEW_EXT_blend_logic_op) = glewGetExtension("GL_EXT_blend_logic_op");
#endif /* GL_EXT_blend_logic_op */
#ifdef GL_EXT_blend_minmax
  CONST_CAST(GLEW_EXT_blend_minmax) = glewGetExtension("GL_EXT_blend_minmax");
  if (glewExperimental || GLEW_EXT_blend_minmax) CONST_CAST(GLEW_EXT_blend_minmax) = !_glewInit_GL_EXT_blend_minmax(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_blend_minmax */
#ifdef GL_EXT_blend_subtract
  CONST_CAST(GLEW_EXT_blend_subtract) = glewGetExtension("GL_EXT_blend_subtract");
#endif /* GL_EXT_blend_subtract */
#ifdef GL_EXT_clip_volume_hint
  CONST_CAST(GLEW_EXT_clip_volume_hint) = glewGetExtension("GL_EXT_clip_volume_hint");
#endif /* GL_EXT_clip_volume_hint */
#ifdef GL_EXT_cmyka
  CONST_CAST(GLEW_EXT_cmyka) = glewGetExtension("GL_EXT_cmyka");
#endif /* GL_EXT_cmyka */
#ifdef GL_EXT_color_subtable
  CONST_CAST(GLEW_EXT_color_subtable) = glewGetExtension("GL_EXT_color_subtable");
  if (glewExperimental || GLEW_EXT_color_subtable) CONST_CAST(GLEW_EXT_color_subtable) = !_glewInit_GL_EXT_color_subtable(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_color_subtable */
#ifdef GL_EXT_compiled_vertex_array
  CONST_CAST(GLEW_EXT_compiled_vertex_array) = glewGetExtension("GL_EXT_compiled_vertex_array");
  if (glewExperimental || GLEW_EXT_compiled_vertex_array) CONST_CAST(GLEW_EXT_compiled_vertex_array) = !_glewInit_GL_EXT_compiled_vertex_array(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_compiled_vertex_array */
#ifdef GL_EXT_convolution
  CONST_CAST(GLEW_EXT_convolution) = glewGetExtension("GL_EXT_convolution");
  if (glewExperimental || GLEW_EXT_convolution) CONST_CAST(GLEW_EXT_convolution) = !_glewInit_GL_EXT_convolution(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_convolution */
#ifdef GL_EXT_coordinate_frame
  CONST_CAST(GLEW_EXT_coordinate_frame) = glewGetExtension("GL_EXT_coordinate_frame");
  if (glewExperimental || GLEW_EXT_coordinate_frame) CONST_CAST(GLEW_EXT_coordinate_frame) = !_glewInit_GL_EXT_coordinate_frame(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_coordinate_frame */
#ifdef GL_EXT_copy_texture
  CONST_CAST(GLEW_EXT_copy_texture) = glewGetExtension("GL_EXT_copy_texture");
  if (glewExperimental || GLEW_EXT_copy_texture) CONST_CAST(GLEW_EXT_copy_texture) = !_glewInit_GL_EXT_copy_texture(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_copy_texture */
#ifdef GL_EXT_cull_vertex
  CONST_CAST(GLEW_EXT_cull_vertex) = glewGetExtension("GL_EXT_cull_vertex");
  if (glewExperimental || GLEW_EXT_cull_vertex) CONST_CAST(GLEW_EXT_cull_vertex) = !_glewInit_GL_EXT_cull_vertex(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_cull_vertex */
#ifdef GL_EXT_depth_bounds_test
  CONST_CAST(GLEW_EXT_depth_bounds_test) = glewGetExtension("GL_EXT_depth_bounds_test");
  if (glewExperimental || GLEW_EXT_depth_bounds_test) CONST_CAST(GLEW_EXT_depth_bounds_test) = !_glewInit_GL_EXT_depth_bounds_test(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_depth_bounds_test */
#ifdef GL_EXT_direct_state_access
  CONST_CAST(GLEW_EXT_direct_state_access) = glewGetExtension("GL_EXT_direct_state_access");
  if (glewExperimental || GLEW_EXT_direct_state_access) CONST_CAST(GLEW_EXT_direct_state_access) = !_glewInit_GL_EXT_direct_state_access(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_direct_state_access */
#ifdef GL_EXT_draw_buffers2
  CONST_CAST(GLEW_EXT_draw_buffers2) = glewGetExtension("GL_EXT_draw_buffers2");
  if (glewExperimental || GLEW_EXT_draw_buffers2) CONST_CAST(GLEW_EXT_draw_buffers2) = !_glewInit_GL_EXT_draw_buffers2(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_draw_buffers2 */
#ifdef GL_EXT_draw_instanced
  CONST_CAST(GLEW_EXT_draw_instanced) = glewGetExtension("GL_EXT_draw_instanced");
  if (glewExperimental || GLEW_EXT_draw_instanced) CONST_CAST(GLEW_EXT_draw_instanced) = !_glewInit_GL_EXT_draw_instanced(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_draw_instanced */
#ifdef GL_EXT_draw_range_elements
  CONST_CAST(GLEW_EXT_draw_range_elements) = glewGetExtension("GL_EXT_draw_range_elements");
  if (glewExperimental || GLEW_EXT_draw_range_elements) CONST_CAST(GLEW_EXT_draw_range_elements) = !_glewInit_GL_EXT_draw_range_elements(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_draw_range_elements */
#ifdef GL_EXT_fog_coord
  CONST_CAST(GLEW_EXT_fog_coord) = glewGetExtension("GL_EXT_fog_coord");
  if (glewExperimental || GLEW_EXT_fog_coord) CONST_CAST(GLEW_EXT_fog_coord) = !_glewInit_GL_EXT_fog_coord(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_fog_coord */
#ifdef GL_EXT_fragment_lighting
  CONST_CAST(GLEW_EXT_fragment_lighting) = glewGetExtension("GL_EXT_fragment_lighting");
  if (glewExperimental || GLEW_EXT_fragment_lighting) CONST_CAST(GLEW_EXT_fragment_lighting) = !_glewInit_GL_EXT_fragment_lighting(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_fragment_lighting */
#ifdef GL_EXT_framebuffer_blit
  CONST_CAST(GLEW_EXT_framebuffer_blit) = glewGetExtension("GL_EXT_framebuffer_blit");
  if (glewExperimental || GLEW_EXT_framebuffer_blit) CONST_CAST(GLEW_EXT_framebuffer_blit) = !_glewInit_GL_EXT_framebuffer_blit(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_framebuffer_blit */
#ifdef GL_EXT_framebuffer_multisample
  CONST_CAST(GLEW_EXT_framebuffer_multisample) = glewGetExtension("GL_EXT_framebuffer_multisample");
  if (glewExperimental || GLEW_EXT_framebuffer_multisample) CONST_CAST(GLEW_EXT_framebuffer_multisample) = !_glewInit_GL_EXT_framebuffer_multisample(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_framebuffer_multisample */
#ifdef GL_EXT_framebuffer_object
  CONST_CAST(GLEW_EXT_framebuffer_object) = glewGetExtension("GL_EXT_framebuffer_object");
  if (glewExperimental || GLEW_EXT_framebuffer_object) CONST_CAST(GLEW_EXT_framebuffer_object) = !_glewInit_GL_EXT_framebuffer_object(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_framebuffer_object */
#ifdef GL_EXT_framebuffer_sRGB
  CONST_CAST(GLEW_EXT_framebuffer_sRGB) = glewGetExtension("GL_EXT_framebuffer_sRGB");
#endif /* GL_EXT_framebuffer_sRGB */
#ifdef GL_EXT_geometry_shader4
  CONST_CAST(GLEW_EXT_geometry_shader4) = glewGetExtension("GL_EXT_geometry_shader4");
  if (glewExperimental || GLEW_EXT_geometry_shader4) CONST_CAST(GLEW_EXT_geometry_shader4) = !_glewInit_GL_EXT_geometry_shader4(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_geometry_shader4 */
#ifdef GL_EXT_gpu_program_parameters
  CONST_CAST(GLEW_EXT_gpu_program_parameters) = glewGetExtension("GL_EXT_gpu_program_parameters");
  if (glewExperimental || GLEW_EXT_gpu_program_parameters) CONST_CAST(GLEW_EXT_gpu_program_parameters) = !_glewInit_GL_EXT_gpu_program_parameters(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_gpu_program_parameters */
#ifdef GL_EXT_gpu_shader4
  CONST_CAST(GLEW_EXT_gpu_shader4) = glewGetExtension("GL_EXT_gpu_shader4");
  if (glewExperimental || GLEW_EXT_gpu_shader4) CONST_CAST(GLEW_EXT_gpu_shader4) = !_glewInit_GL_EXT_gpu_shader4(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_gpu_shader4 */
#ifdef GL_EXT_histogram
  CONST_CAST(GLEW_EXT_histogram) = glewGetExtension("GL_EXT_histogram");
  if (glewExperimental || GLEW_EXT_histogram) CONST_CAST(GLEW_EXT_histogram) = !_glewInit_GL_EXT_histogram(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_histogram */
#ifdef GL_EXT_index_array_formats
  CONST_CAST(GLEW_EXT_index_array_formats) = glewGetExtension("GL_EXT_index_array_formats");
#endif /* GL_EXT_index_array_formats */
#ifdef GL_EXT_index_func
  CONST_CAST(GLEW_EXT_index_func) = glewGetExtension("GL_EXT_index_func");
  if (glewExperimental || GLEW_EXT_index_func) CONST_CAST(GLEW_EXT_index_func) = !_glewInit_GL_EXT_index_func(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_index_func */
#ifdef GL_EXT_index_material
  CONST_CAST(GLEW_EXT_index_material) = glewGetExtension("GL_EXT_index_material");
  if (glewExperimental || GLEW_EXT_index_material) CONST_CAST(GLEW_EXT_index_material) = !_glewInit_GL_EXT_index_material(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_index_material */
#ifdef GL_EXT_index_texture
  CONST_CAST(GLEW_EXT_index_texture) = glewGetExtension("GL_EXT_index_texture");
#endif /* GL_EXT_index_texture */
#ifdef GL_EXT_light_texture
  CONST_CAST(GLEW_EXT_light_texture) = glewGetExtension("GL_EXT_light_texture");
  if (glewExperimental || GLEW_EXT_light_texture) CONST_CAST(GLEW_EXT_light_texture) = !_glewInit_GL_EXT_light_texture(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_light_texture */
#ifdef GL_EXT_misc_attribute
  CONST_CAST(GLEW_EXT_misc_attribute) = glewGetExtension("GL_EXT_misc_attribute");
#endif /* GL_EXT_misc_attribute */
#ifdef GL_EXT_multi_draw_arrays
  CONST_CAST(GLEW_EXT_multi_draw_arrays) = glewGetExtension("GL_EXT_multi_draw_arrays");
  if (glewExperimental || GLEW_EXT_multi_draw_arrays) CONST_CAST(GLEW_EXT_multi_draw_arrays) = !_glewInit_GL_EXT_multi_draw_arrays(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_multi_draw_arrays */
#ifdef GL_EXT_multisample
  CONST_CAST(GLEW_EXT_multisample) = glewGetExtension("GL_EXT_multisample");
  if (glewExperimental || GLEW_EXT_multisample) CONST_CAST(GLEW_EXT_multisample) = !_glewInit_GL_EXT_multisample(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_multisample */
#ifdef GL_EXT_packed_depth_stencil
  CONST_CAST(GLEW_EXT_packed_depth_stencil) = glewGetExtension("GL_EXT_packed_depth_stencil");
#endif /* GL_EXT_packed_depth_stencil */
#ifdef GL_EXT_packed_float
  CONST_CAST(GLEW_EXT_packed_float) = glewGetExtension("GL_EXT_packed_float");
#endif /* GL_EXT_packed_float */
#ifdef GL_EXT_packed_pixels
  CONST_CAST(GLEW_EXT_packed_pixels) = glewGetExtension("GL_EXT_packed_pixels");
#endif /* GL_EXT_packed_pixels */
#ifdef GL_EXT_paletted_texture
  CONST_CAST(GLEW_EXT_paletted_texture) = glewGetExtension("GL_EXT_paletted_texture");
  if (glewExperimental || GLEW_EXT_paletted_texture) CONST_CAST(GLEW_EXT_paletted_texture) = !_glewInit_GL_EXT_paletted_texture(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_paletted_texture */
#ifdef GL_EXT_pixel_buffer_object
  CONST_CAST(GLEW_EXT_pixel_buffer_object) = glewGetExtension("GL_EXT_pixel_buffer_object");
#endif /* GL_EXT_pixel_buffer_object */
#ifdef GL_EXT_pixel_transform
  CONST_CAST(GLEW_EXT_pixel_transform) = glewGetExtension("GL_EXT_pixel_transform");
  if (glewExperimental || GLEW_EXT_pixel_transform) CONST_CAST(GLEW_EXT_pixel_transform) = !_glewInit_GL_EXT_pixel_transform(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_pixel_transform */
#ifdef GL_EXT_pixel_transform_color_table
  CONST_CAST(GLEW_EXT_pixel_transform_color_table) = glewGetExtension("GL_EXT_pixel_transform_color_table");
#endif /* GL_EXT_pixel_transform_color_table */
#ifdef GL_EXT_point_parameters
  CONST_CAST(GLEW_EXT_point_parameters) = glewGetExtension("GL_EXT_point_parameters");
  if (glewExperimental || GLEW_EXT_point_parameters) CONST_CAST(GLEW_EXT_point_parameters) = !_glewInit_GL_EXT_point_parameters(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_point_parameters */
#ifdef GL_EXT_polygon_offset
  CONST_CAST(GLEW_EXT_polygon_offset) = glewGetExtension("GL_EXT_polygon_offset");
  if (glewExperimental || GLEW_EXT_polygon_offset) CONST_CAST(GLEW_EXT_polygon_offset) = !_glewInit_GL_EXT_polygon_offset(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_polygon_offset */
#ifdef GL_EXT_provoking_vertex
  CONST_CAST(GLEW_EXT_provoking_vertex) = glewGetExtension("GL_EXT_provoking_vertex");
  if (glewExperimental || GLEW_EXT_provoking_vertex) CONST_CAST(GLEW_EXT_provoking_vertex) = !_glewInit_GL_EXT_provoking_vertex(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_provoking_vertex */
#ifdef GL_EXT_rescale_normal
  CONST_CAST(GLEW_EXT_rescale_normal) = glewGetExtension("GL_EXT_rescale_normal");
#endif /* GL_EXT_rescale_normal */
#ifdef GL_EXT_scene_marker
  CONST_CAST(GLEW_EXT_scene_marker) = glewGetExtension("GL_EXT_scene_marker");
  if (glewExperimental || GLEW_EXT_scene_marker) CONST_CAST(GLEW_EXT_scene_marker) = !_glewInit_GL_EXT_scene_marker(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_scene_marker */
#ifdef GL_EXT_secondary_color
  CONST_CAST(GLEW_EXT_secondary_color) = glewGetExtension("GL_EXT_secondary_color");
  if (glewExperimental || GLEW_EXT_secondary_color) CONST_CAST(GLEW_EXT_secondary_color) = !_glewInit_GL_EXT_secondary_color(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_secondary_color */
#ifdef GL_EXT_separate_shader_objects
  CONST_CAST(GLEW_EXT_separate_shader_objects) = glewGetExtension("GL_EXT_separate_shader_objects");
  if (glewExperimental || GLEW_EXT_separate_shader_objects) CONST_CAST(GLEW_EXT_separate_shader_objects) = !_glewInit_GL_EXT_separate_shader_objects(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_separate_shader_objects */
#ifdef GL_EXT_separate_specular_color
  CONST_CAST(GLEW_EXT_separate_specular_color) = glewGetExtension("GL_EXT_separate_specular_color");
#endif /* GL_EXT_separate_specular_color */
#ifdef GL_EXT_shadow_funcs
  CONST_CAST(GLEW_EXT_shadow_funcs) = glewGetExtension("GL_EXT_shadow_funcs");
#endif /* GL_EXT_shadow_funcs */
#ifdef GL_EXT_shared_texture_palette
  CONST_CAST(GLEW_EXT_shared_texture_palette) = glewGetExtension("GL_EXT_shared_texture_palette");
#endif /* GL_EXT_shared_texture_palette */
#ifdef GL_EXT_stencil_clear_tag
  CONST_CAST(GLEW_EXT_stencil_clear_tag) = glewGetExtension("GL_EXT_stencil_clear_tag");
#endif /* GL_EXT_stencil_clear_tag */
#ifdef GL_EXT_stencil_two_side
  CONST_CAST(GLEW_EXT_stencil_two_side) = glewGetExtension("GL_EXT_stencil_two_side");
  if (glewExperimental || GLEW_EXT_stencil_two_side) CONST_CAST(GLEW_EXT_stencil_two_side) = !_glewInit_GL_EXT_stencil_two_side(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_stencil_two_side */
#ifdef GL_EXT_stencil_wrap
  CONST_CAST(GLEW_EXT_stencil_wrap) = glewGetExtension("GL_EXT_stencil_wrap");
#endif /* GL_EXT_stencil_wrap */
#ifdef GL_EXT_subtexture
  CONST_CAST(GLEW_EXT_subtexture) = glewGetExtension("GL_EXT_subtexture");
  if (glewExperimental || GLEW_EXT_subtexture) CONST_CAST(GLEW_EXT_subtexture) = !_glewInit_GL_EXT_subtexture(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_subtexture */
#ifdef GL_EXT_texture
  CONST_CAST(GLEW_EXT_texture) = glewGetExtension("GL_EXT_texture");
#endif /* GL_EXT_texture */
#ifdef GL_EXT_texture3D
  CONST_CAST(GLEW_EXT_texture3D) = glewGetExtension("GL_EXT_texture3D");
  if (glewExperimental || GLEW_EXT_texture3D) CONST_CAST(GLEW_EXT_texture3D) = !_glewInit_GL_EXT_texture3D(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_texture3D */
#ifdef GL_EXT_texture_array
  CONST_CAST(GLEW_EXT_texture_array) = glewGetExtension("GL_EXT_texture_array");
#endif /* GL_EXT_texture_array */
#ifdef GL_EXT_texture_buffer_object
  CONST_CAST(GLEW_EXT_texture_buffer_object) = glewGetExtension("GL_EXT_texture_buffer_object");
  if (glewExperimental || GLEW_EXT_texture_buffer_object) CONST_CAST(GLEW_EXT_texture_buffer_object) = !_glewInit_GL_EXT_texture_buffer_object(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_texture_buffer_object */
#ifdef GL_EXT_texture_compression_dxt1
  CONST_CAST(GLEW_EXT_texture_compression_dxt1) = glewGetExtension("GL_EXT_texture_compression_dxt1");
#endif /* GL_EXT_texture_compression_dxt1 */
#ifdef GL_EXT_texture_compression_latc
  CONST_CAST(GLEW_EXT_texture_compression_latc) = glewGetExtension("GL_EXT_texture_compression_latc");
#endif /* GL_EXT_texture_compression_latc */
#ifdef GL_EXT_texture_compression_rgtc
  CONST_CAST(GLEW_EXT_texture_compression_rgtc) = glewGetExtension("GL_EXT_texture_compression_rgtc");
#endif /* GL_EXT_texture_compression_rgtc */
#ifdef GL_EXT_texture_compression_s3tc
  CONST_CAST(GLEW_EXT_texture_compression_s3tc) = glewGetExtension("GL_EXT_texture_compression_s3tc");
#endif /* GL_EXT_texture_compression_s3tc */
#ifdef GL_EXT_texture_cube_map
  CONST_CAST(GLEW_EXT_texture_cube_map) = glewGetExtension("GL_EXT_texture_cube_map");
#endif /* GL_EXT_texture_cube_map */
#ifdef GL_EXT_texture_edge_clamp
  CONST_CAST(GLEW_EXT_texture_edge_clamp) = glewGetExtension("GL_EXT_texture_edge_clamp");
#endif /* GL_EXT_texture_edge_clamp */
#ifdef GL_EXT_texture_env
  CONST_CAST(GLEW_EXT_texture_env) = glewGetExtension("GL_EXT_texture_env");
#endif /* GL_EXT_texture_env */
#ifdef GL_EXT_texture_env_add
  CONST_CAST(GLEW_EXT_texture_env_add) = glewGetExtension("GL_EXT_texture_env_add");
#endif /* GL_EXT_texture_env_add */
#ifdef GL_EXT_texture_env_combine
  CONST_CAST(GLEW_EXT_texture_env_combine) = glewGetExtension("GL_EXT_texture_env_combine");
#endif /* GL_EXT_texture_env_combine */
#ifdef GL_EXT_texture_env_dot3
  CONST_CAST(GLEW_EXT_texture_env_dot3) = glewGetExtension("GL_EXT_texture_env_dot3");
#endif /* GL_EXT_texture_env_dot3 */
#ifdef GL_EXT_texture_filter_anisotropic
  CONST_CAST(GLEW_EXT_texture_filter_anisotropic) = glewGetExtension("GL_EXT_texture_filter_anisotropic");
#endif /* GL_EXT_texture_filter_anisotropic */
#ifdef GL_EXT_texture_integer
  CONST_CAST(GLEW_EXT_texture_integer) = glewGetExtension("GL_EXT_texture_integer");
  if (glewExperimental || GLEW_EXT_texture_integer) CONST_CAST(GLEW_EXT_texture_integer) = !_glewInit_GL_EXT_texture_integer(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_texture_integer */
#ifdef GL_EXT_texture_lod_bias
  CONST_CAST(GLEW_EXT_texture_lod_bias) = glewGetExtension("GL_EXT_texture_lod_bias");
#endif /* GL_EXT_texture_lod_bias */
#ifdef GL_EXT_texture_mirror_clamp
  CONST_CAST(GLEW_EXT_texture_mirror_clamp) = glewGetExtension("GL_EXT_texture_mirror_clamp");
#endif /* GL_EXT_texture_mirror_clamp */
#ifdef GL_EXT_texture_object
  CONST_CAST(GLEW_EXT_texture_object) = glewGetExtension("GL_EXT_texture_object");
  if (glewExperimental || GLEW_EXT_texture_object) CONST_CAST(GLEW_EXT_texture_object) = !_glewInit_GL_EXT_texture_object(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_texture_object */
#ifdef GL_EXT_texture_perturb_normal
  CONST_CAST(GLEW_EXT_texture_perturb_normal) = glewGetExtension("GL_EXT_texture_perturb_normal");
  if (glewExperimental || GLEW_EXT_texture_perturb_normal) CONST_CAST(GLEW_EXT_texture_perturb_normal) = !_glewInit_GL_EXT_texture_perturb_normal(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_texture_perturb_normal */
#ifdef GL_EXT_texture_rectangle
  CONST_CAST(GLEW_EXT_texture_rectangle) = glewGetExtension("GL_EXT_texture_rectangle");
#endif /* GL_EXT_texture_rectangle */
#ifdef GL_EXT_texture_sRGB
  CONST_CAST(GLEW_EXT_texture_sRGB) = glewGetExtension("GL_EXT_texture_sRGB");
#endif /* GL_EXT_texture_sRGB */
#ifdef GL_EXT_texture_shared_exponent
  CONST_CAST(GLEW_EXT_texture_shared_exponent) = glewGetExtension("GL_EXT_texture_shared_exponent");
#endif /* GL_EXT_texture_shared_exponent */
#ifdef GL_EXT_texture_snorm
  CONST_CAST(GLEW_EXT_texture_snorm) = glewGetExtension("GL_EXT_texture_snorm");
#endif /* GL_EXT_texture_snorm */
#ifdef GL_EXT_texture_swizzle
  CONST_CAST(GLEW_EXT_texture_swizzle) = glewGetExtension("GL_EXT_texture_swizzle");
#endif /* GL_EXT_texture_swizzle */
#ifdef GL_EXT_timer_query
  CONST_CAST(GLEW_EXT_timer_query) = glewGetExtension("GL_EXT_timer_query");
  if (glewExperimental || GLEW_EXT_timer_query) CONST_CAST(GLEW_EXT_timer_query) = !_glewInit_GL_EXT_timer_query(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_timer_query */
#ifdef GL_EXT_transform_feedback
  CONST_CAST(GLEW_EXT_transform_feedback) = glewGetExtension("GL_EXT_transform_feedback");
  if (glewExperimental || GLEW_EXT_transform_feedback) CONST_CAST(GLEW_EXT_transform_feedback) = !_glewInit_GL_EXT_transform_feedback(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_transform_feedback */
#ifdef GL_EXT_vertex_array
  CONST_CAST(GLEW_EXT_vertex_array) = glewGetExtension("GL_EXT_vertex_array");
  if (glewExperimental || GLEW_EXT_vertex_array) CONST_CAST(GLEW_EXT_vertex_array) = !_glewInit_GL_EXT_vertex_array(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_vertex_array */
#ifdef GL_EXT_vertex_array_bgra
  CONST_CAST(GLEW_EXT_vertex_array_bgra) = glewGetExtension("GL_EXT_vertex_array_bgra");
#endif /* GL_EXT_vertex_array_bgra */
#ifdef GL_EXT_vertex_shader
  CONST_CAST(GLEW_EXT_vertex_shader) = glewGetExtension("GL_EXT_vertex_shader");
  if (glewExperimental || GLEW_EXT_vertex_shader) CONST_CAST(GLEW_EXT_vertex_shader) = !_glewInit_GL_EXT_vertex_shader(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_vertex_shader */
#ifdef GL_EXT_vertex_weighting
  CONST_CAST(GLEW_EXT_vertex_weighting) = glewGetExtension("GL_EXT_vertex_weighting");
  if (glewExperimental || GLEW_EXT_vertex_weighting) CONST_CAST(GLEW_EXT_vertex_weighting) = !_glewInit_GL_EXT_vertex_weighting(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_EXT_vertex_weighting */
#ifdef GL_GREMEDY_frame_terminator
  CONST_CAST(GLEW_GREMEDY_frame_terminator) = glewGetExtension("GL_GREMEDY_frame_terminator");
  if (glewExperimental || GLEW_GREMEDY_frame_terminator) CONST_CAST(GLEW_GREMEDY_frame_terminator) = !_glewInit_GL_GREMEDY_frame_terminator(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_GREMEDY_frame_terminator */
#ifdef GL_GREMEDY_string_marker
  CONST_CAST(GLEW_GREMEDY_string_marker) = glewGetExtension("GL_GREMEDY_string_marker");
  if (glewExperimental || GLEW_GREMEDY_string_marker) CONST_CAST(GLEW_GREMEDY_string_marker) = !_glewInit_GL_GREMEDY_string_marker(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_GREMEDY_string_marker */
#ifdef GL_HP_convolution_border_modes
  CONST_CAST(GLEW_HP_convolution_border_modes) = glewGetExtension("GL_HP_convolution_border_modes");
#endif /* GL_HP_convolution_border_modes */
#ifdef GL_HP_image_transform
  CONST_CAST(GLEW_HP_image_transform) = glewGetExtension("GL_HP_image_transform");
  if (glewExperimental || GLEW_HP_image_transform) CONST_CAST(GLEW_HP_image_transform) = !_glewInit_GL_HP_image_transform(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_HP_image_transform */
#ifdef GL_HP_occlusion_test
  CONST_CAST(GLEW_HP_occlusion_test) = glewGetExtension("GL_HP_occlusion_test");
#endif /* GL_HP_occlusion_test */
#ifdef GL_HP_texture_lighting
  CONST_CAST(GLEW_HP_texture_lighting) = glewGetExtension("GL_HP_texture_lighting");
#endif /* GL_HP_texture_lighting */
#ifdef GL_IBM_cull_vertex
  CONST_CAST(GLEW_IBM_cull_vertex) = glewGetExtension("GL_IBM_cull_vertex");
#endif /* GL_IBM_cull_vertex */
#ifdef GL_IBM_multimode_draw_arrays
  CONST_CAST(GLEW_IBM_multimode_draw_arrays) = glewGetExtension("GL_IBM_multimode_draw_arrays");
  if (glewExperimental || GLEW_IBM_multimode_draw_arrays) CONST_CAST(GLEW_IBM_multimode_draw_arrays) = !_glewInit_GL_IBM_multimode_draw_arrays(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_IBM_multimode_draw_arrays */
#ifdef GL_IBM_rasterpos_clip
  CONST_CAST(GLEW_IBM_rasterpos_clip) = glewGetExtension("GL_IBM_rasterpos_clip");
#endif /* GL_IBM_rasterpos_clip */
#ifdef GL_IBM_static_data
  CONST_CAST(GLEW_IBM_static_data) = glewGetExtension("GL_IBM_static_data");
#endif /* GL_IBM_static_data */
#ifdef GL_IBM_texture_mirrored_repeat
  CONST_CAST(GLEW_IBM_texture_mirrored_repeat) = glewGetExtension("GL_IBM_texture_mirrored_repeat");
#endif /* GL_IBM_texture_mirrored_repeat */
#ifdef GL_IBM_vertex_array_lists
  CONST_CAST(GLEW_IBM_vertex_array_lists) = glewGetExtension("GL_IBM_vertex_array_lists");
  if (glewExperimental || GLEW_IBM_vertex_array_lists) CONST_CAST(GLEW_IBM_vertex_array_lists) = !_glewInit_GL_IBM_vertex_array_lists(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_IBM_vertex_array_lists */
#ifdef GL_INGR_color_clamp
  CONST_CAST(GLEW_INGR_color_clamp) = glewGetExtension("GL_INGR_color_clamp");
#endif /* GL_INGR_color_clamp */
#ifdef GL_INGR_interlace_read
  CONST_CAST(GLEW_INGR_interlace_read) = glewGetExtension("GL_INGR_interlace_read");
#endif /* GL_INGR_interlace_read */
#ifdef GL_INTEL_parallel_arrays
  CONST_CAST(GLEW_INTEL_parallel_arrays) = glewGetExtension("GL_INTEL_parallel_arrays");
  if (glewExperimental || GLEW_INTEL_parallel_arrays) CONST_CAST(GLEW_INTEL_parallel_arrays) = !_glewInit_GL_INTEL_parallel_arrays(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_INTEL_parallel_arrays */
#ifdef GL_INTEL_texture_scissor
  CONST_CAST(GLEW_INTEL_texture_scissor) = glewGetExtension("GL_INTEL_texture_scissor");
  if (glewExperimental || GLEW_INTEL_texture_scissor) CONST_CAST(GLEW_INTEL_texture_scissor) = !_glewInit_GL_INTEL_texture_scissor(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_INTEL_texture_scissor */
#ifdef GL_KTX_buffer_region
  CONST_CAST(GLEW_KTX_buffer_region) = glewGetExtension("GL_KTX_buffer_region");
  if (glewExperimental || GLEW_KTX_buffer_region) CONST_CAST(GLEW_KTX_buffer_region) = !_glewInit_GL_KTX_buffer_region(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_KTX_buffer_region */
#ifdef GL_MESAX_texture_stack
  CONST_CAST(GLEW_MESAX_texture_stack) = glewGetExtension("GL_MESAX_texture_stack");
#endif /* GL_MESAX_texture_stack */
#ifdef GL_MESA_pack_invert
  CONST_CAST(GLEW_MESA_pack_invert) = glewGetExtension("GL_MESA_pack_invert");
#endif /* GL_MESA_pack_invert */
#ifdef GL_MESA_resize_buffers
  CONST_CAST(GLEW_MESA_resize_buffers) = glewGetExtension("GL_MESA_resize_buffers");
  if (glewExperimental || GLEW_MESA_resize_buffers) CONST_CAST(GLEW_MESA_resize_buffers) = !_glewInit_GL_MESA_resize_buffers(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_MESA_resize_buffers */
#ifdef GL_MESA_window_pos
  CONST_CAST(GLEW_MESA_window_pos) = glewGetExtension("GL_MESA_window_pos");
  if (glewExperimental || GLEW_MESA_window_pos) CONST_CAST(GLEW_MESA_window_pos) = !_glewInit_GL_MESA_window_pos(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_MESA_window_pos */
#ifdef GL_MESA_ycbcr_texture
  CONST_CAST(GLEW_MESA_ycbcr_texture) = glewGetExtension("GL_MESA_ycbcr_texture");
#endif /* GL_MESA_ycbcr_texture */
#ifdef GL_NV_blend_square
  CONST_CAST(GLEW_NV_blend_square) = glewGetExtension("GL_NV_blend_square");
#endif /* GL_NV_blend_square */
#ifdef GL_NV_conditional_render
  CONST_CAST(GLEW_NV_conditional_render) = glewGetExtension("GL_NV_conditional_render");
  if (glewExperimental || GLEW_NV_conditional_render) CONST_CAST(GLEW_NV_conditional_render) = !_glewInit_GL_NV_conditional_render(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_conditional_render */
#ifdef GL_NV_copy_depth_to_color
  CONST_CAST(GLEW_NV_copy_depth_to_color) = glewGetExtension("GL_NV_copy_depth_to_color");
#endif /* GL_NV_copy_depth_to_color */
#ifdef GL_NV_copy_image
  CONST_CAST(GLEW_NV_copy_image) = glewGetExtension("GL_NV_copy_image");
  if (glewExperimental || GLEW_NV_copy_image) CONST_CAST(GLEW_NV_copy_image) = !_glewInit_GL_NV_copy_image(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_copy_image */
#ifdef GL_NV_depth_buffer_float
  CONST_CAST(GLEW_NV_depth_buffer_float) = glewGetExtension("GL_NV_depth_buffer_float");
  if (glewExperimental || GLEW_NV_depth_buffer_float) CONST_CAST(GLEW_NV_depth_buffer_float) = !_glewInit_GL_NV_depth_buffer_float(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_depth_buffer_float */
#ifdef GL_NV_depth_clamp
  CONST_CAST(GLEW_NV_depth_clamp) = glewGetExtension("GL_NV_depth_clamp");
#endif /* GL_NV_depth_clamp */
#ifdef GL_NV_depth_range_unclamped
  CONST_CAST(GLEW_NV_depth_range_unclamped) = glewGetExtension("GL_NV_depth_range_unclamped");
#endif /* GL_NV_depth_range_unclamped */
#ifdef GL_NV_evaluators
  CONST_CAST(GLEW_NV_evaluators) = glewGetExtension("GL_NV_evaluators");
  if (glewExperimental || GLEW_NV_evaluators) CONST_CAST(GLEW_NV_evaluators) = !_glewInit_GL_NV_evaluators(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_evaluators */
#ifdef GL_NV_explicit_multisample
  CONST_CAST(GLEW_NV_explicit_multisample) = glewGetExtension("GL_NV_explicit_multisample");
  if (glewExperimental || GLEW_NV_explicit_multisample) CONST_CAST(GLEW_NV_explicit_multisample) = !_glewInit_GL_NV_explicit_multisample(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_explicit_multisample */
#ifdef GL_NV_fence
  CONST_CAST(GLEW_NV_fence) = glewGetExtension("GL_NV_fence");
  if (glewExperimental || GLEW_NV_fence) CONST_CAST(GLEW_NV_fence) = !_glewInit_GL_NV_fence(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_fence */
#ifdef GL_NV_float_buffer
  CONST_CAST(GLEW_NV_float_buffer) = glewGetExtension("GL_NV_float_buffer");
#endif /* GL_NV_float_buffer */
#ifdef GL_NV_fog_distance
  CONST_CAST(GLEW_NV_fog_distance) = glewGetExtension("GL_NV_fog_distance");
#endif /* GL_NV_fog_distance */
#ifdef GL_NV_fragment_program
  CONST_CAST(GLEW_NV_fragment_program) = glewGetExtension("GL_NV_fragment_program");
  if (glewExperimental || GLEW_NV_fragment_program) CONST_CAST(GLEW_NV_fragment_program) = !_glewInit_GL_NV_fragment_program(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_fragment_program */
#ifdef GL_NV_fragment_program2
  CONST_CAST(GLEW_NV_fragment_program2) = glewGetExtension("GL_NV_fragment_program2");
#endif /* GL_NV_fragment_program2 */
#ifdef GL_NV_fragment_program4
  CONST_CAST(GLEW_NV_fragment_program4) = glewGetExtension("GL_NV_fragment_program4");
#endif /* GL_NV_fragment_program4 */
#ifdef GL_NV_fragment_program_option
  CONST_CAST(GLEW_NV_fragment_program_option) = glewGetExtension("GL_NV_fragment_program_option");
#endif /* GL_NV_fragment_program_option */
#ifdef GL_NV_framebuffer_multisample_coverage
  CONST_CAST(GLEW_NV_framebuffer_multisample_coverage) = glewGetExtension("GL_NV_framebuffer_multisample_coverage");
  if (glewExperimental || GLEW_NV_framebuffer_multisample_coverage) CONST_CAST(GLEW_NV_framebuffer_multisample_coverage) = !_glewInit_GL_NV_framebuffer_multisample_coverage(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_framebuffer_multisample_coverage */
#ifdef GL_NV_geometry_program4
  CONST_CAST(GLEW_NV_geometry_program4) = glewGetExtension("GL_NV_geometry_program4");
  if (glewExperimental || GLEW_NV_geometry_program4) CONST_CAST(GLEW_NV_geometry_program4) = !_glewInit_GL_NV_geometry_program4(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_geometry_program4 */
#ifdef GL_NV_geometry_shader4
  CONST_CAST(GLEW_NV_geometry_shader4) = glewGetExtension("GL_NV_geometry_shader4");
#endif /* GL_NV_geometry_shader4 */
#ifdef GL_NV_gpu_program4
  CONST_CAST(GLEW_NV_gpu_program4) = glewGetExtension("GL_NV_gpu_program4");
  if (glewExperimental || GLEW_NV_gpu_program4) CONST_CAST(GLEW_NV_gpu_program4) = !_glewInit_GL_NV_gpu_program4(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_gpu_program4 */
#ifdef GL_NV_half_float
  CONST_CAST(GLEW_NV_half_float) = glewGetExtension("GL_NV_half_float");
  if (glewExperimental || GLEW_NV_half_float) CONST_CAST(GLEW_NV_half_float) = !_glewInit_GL_NV_half_float(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_half_float */
#ifdef GL_NV_light_max_exponent
  CONST_CAST(GLEW_NV_light_max_exponent) = glewGetExtension("GL_NV_light_max_exponent");
#endif /* GL_NV_light_max_exponent */
#ifdef GL_NV_multisample_filter_hint
  CONST_CAST(GLEW_NV_multisample_filter_hint) = glewGetExtension("GL_NV_multisample_filter_hint");
#endif /* GL_NV_multisample_filter_hint */
#ifdef GL_NV_occlusion_query
  CONST_CAST(GLEW_NV_occlusion_query) = glewGetExtension("GL_NV_occlusion_query");
  if (glewExperimental || GLEW_NV_occlusion_query) CONST_CAST(GLEW_NV_occlusion_query) = !_glewInit_GL_NV_occlusion_query(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_occlusion_query */
#ifdef GL_NV_packed_depth_stencil
  CONST_CAST(GLEW_NV_packed_depth_stencil) = glewGetExtension("GL_NV_packed_depth_stencil");
#endif /* GL_NV_packed_depth_stencil */
#ifdef GL_NV_parameter_buffer_object
  CONST_CAST(GLEW_NV_parameter_buffer_object) = glewGetExtension("GL_NV_parameter_buffer_object");
  if (glewExperimental || GLEW_NV_parameter_buffer_object) CONST_CAST(GLEW_NV_parameter_buffer_object) = !_glewInit_GL_NV_parameter_buffer_object(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_parameter_buffer_object */
#ifdef GL_NV_parameter_buffer_object2
  CONST_CAST(GLEW_NV_parameter_buffer_object2) = glewGetExtension("GL_NV_parameter_buffer_object2");
#endif /* GL_NV_parameter_buffer_object2 */
#ifdef GL_NV_pixel_data_range
  CONST_CAST(GLEW_NV_pixel_data_range) = glewGetExtension("GL_NV_pixel_data_range");
  if (glewExperimental || GLEW_NV_pixel_data_range) CONST_CAST(GLEW_NV_pixel_data_range) = !_glewInit_GL_NV_pixel_data_range(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_pixel_data_range */
#ifdef GL_NV_point_sprite
  CONST_CAST(GLEW_NV_point_sprite) = glewGetExtension("GL_NV_point_sprite");
  if (glewExperimental || GLEW_NV_point_sprite) CONST_CAST(GLEW_NV_point_sprite) = !_glewInit_GL_NV_point_sprite(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_point_sprite */
#ifdef GL_NV_present_video
  CONST_CAST(GLEW_NV_present_video) = glewGetExtension("GL_NV_present_video");
  if (glewExperimental || GLEW_NV_present_video) CONST_CAST(GLEW_NV_present_video) = !_glewInit_GL_NV_present_video(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_present_video */
#ifdef GL_NV_primitive_restart
  CONST_CAST(GLEW_NV_primitive_restart) = glewGetExtension("GL_NV_primitive_restart");
  if (glewExperimental || GLEW_NV_primitive_restart) CONST_CAST(GLEW_NV_primitive_restart) = !_glewInit_GL_NV_primitive_restart(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_primitive_restart */
#ifdef GL_NV_register_combiners
  CONST_CAST(GLEW_NV_register_combiners) = glewGetExtension("GL_NV_register_combiners");
  if (glewExperimental || GLEW_NV_register_combiners) CONST_CAST(GLEW_NV_register_combiners) = !_glewInit_GL_NV_register_combiners(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_register_combiners */
#ifdef GL_NV_register_combiners2
  CONST_CAST(GLEW_NV_register_combiners2) = glewGetExtension("GL_NV_register_combiners2");
  if (glewExperimental || GLEW_NV_register_combiners2) CONST_CAST(GLEW_NV_register_combiners2) = !_glewInit_GL_NV_register_combiners2(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_register_combiners2 */
#ifdef GL_NV_shader_buffer_load
  CONST_CAST(GLEW_NV_shader_buffer_load) = glewGetExtension("GL_NV_shader_buffer_load");
  if (glewExperimental || GLEW_NV_shader_buffer_load) CONST_CAST(GLEW_NV_shader_buffer_load) = !_glewInit_GL_NV_shader_buffer_load(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_shader_buffer_load */
#ifdef GL_NV_texgen_emboss
  CONST_CAST(GLEW_NV_texgen_emboss) = glewGetExtension("GL_NV_texgen_emboss");
#endif /* GL_NV_texgen_emboss */
#ifdef GL_NV_texgen_reflection
  CONST_CAST(GLEW_NV_texgen_reflection) = glewGetExtension("GL_NV_texgen_reflection");
#endif /* GL_NV_texgen_reflection */
#ifdef GL_NV_texture_barrier
  CONST_CAST(GLEW_NV_texture_barrier) = glewGetExtension("GL_NV_texture_barrier");
  if (glewExperimental || GLEW_NV_texture_barrier) CONST_CAST(GLEW_NV_texture_barrier) = !_glewInit_GL_NV_texture_barrier(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_texture_barrier */
#ifdef GL_NV_texture_compression_vtc
  CONST_CAST(GLEW_NV_texture_compression_vtc) = glewGetExtension("GL_NV_texture_compression_vtc");
#endif /* GL_NV_texture_compression_vtc */
#ifdef GL_NV_texture_env_combine4
  CONST_CAST(GLEW_NV_texture_env_combine4) = glewGetExtension("GL_NV_texture_env_combine4");
#endif /* GL_NV_texture_env_combine4 */
#ifdef GL_NV_texture_expand_normal
  CONST_CAST(GLEW_NV_texture_expand_normal) = glewGetExtension("GL_NV_texture_expand_normal");
#endif /* GL_NV_texture_expand_normal */
#ifdef GL_NV_texture_rectangle
  CONST_CAST(GLEW_NV_texture_rectangle) = glewGetExtension("GL_NV_texture_rectangle");
#endif /* GL_NV_texture_rectangle */
#ifdef GL_NV_texture_shader
  CONST_CAST(GLEW_NV_texture_shader) = glewGetExtension("GL_NV_texture_shader");
#endif /* GL_NV_texture_shader */
#ifdef GL_NV_texture_shader2
  CONST_CAST(GLEW_NV_texture_shader2) = glewGetExtension("GL_NV_texture_shader2");
#endif /* GL_NV_texture_shader2 */
#ifdef GL_NV_texture_shader3
  CONST_CAST(GLEW_NV_texture_shader3) = glewGetExtension("GL_NV_texture_shader3");
#endif /* GL_NV_texture_shader3 */
#ifdef GL_NV_transform_feedback
  CONST_CAST(GLEW_NV_transform_feedback) = glewGetExtension("GL_NV_transform_feedback");
  if (glewExperimental || GLEW_NV_transform_feedback) CONST_CAST(GLEW_NV_transform_feedback) = !_glewInit_GL_NV_transform_feedback(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_transform_feedback */
#ifdef GL_NV_transform_feedback2
  CONST_CAST(GLEW_NV_transform_feedback2) = glewGetExtension("GL_NV_transform_feedback2");
  if (glewExperimental || GLEW_NV_transform_feedback2) CONST_CAST(GLEW_NV_transform_feedback2) = !_glewInit_GL_NV_transform_feedback2(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_transform_feedback2 */
#ifdef GL_NV_vertex_array_range
  CONST_CAST(GLEW_NV_vertex_array_range) = glewGetExtension("GL_NV_vertex_array_range");
  if (glewExperimental || GLEW_NV_vertex_array_range) CONST_CAST(GLEW_NV_vertex_array_range) = !_glewInit_GL_NV_vertex_array_range(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_vertex_array_range */
#ifdef GL_NV_vertex_array_range2
  CONST_CAST(GLEW_NV_vertex_array_range2) = glewGetExtension("GL_NV_vertex_array_range2");
#endif /* GL_NV_vertex_array_range2 */
#ifdef GL_NV_vertex_buffer_unified_memory
  CONST_CAST(GLEW_NV_vertex_buffer_unified_memory) = glewGetExtension("GL_NV_vertex_buffer_unified_memory");
  if (glewExperimental || GLEW_NV_vertex_buffer_unified_memory) CONST_CAST(GLEW_NV_vertex_buffer_unified_memory) = !_glewInit_GL_NV_vertex_buffer_unified_memory(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_vertex_buffer_unified_memory */
#ifdef GL_NV_vertex_program
  CONST_CAST(GLEW_NV_vertex_program) = glewGetExtension("GL_NV_vertex_program");
  if (glewExperimental || GLEW_NV_vertex_program) CONST_CAST(GLEW_NV_vertex_program) = !_glewInit_GL_NV_vertex_program(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_NV_vertex_program */
#ifdef GL_NV_vertex_program1_1
  CONST_CAST(GLEW_NV_vertex_program1_1) = glewGetExtension("GL_NV_vertex_program1_1");
#endif /* GL_NV_vertex_program1_1 */
#ifdef GL_NV_vertex_program2
  CONST_CAST(GLEW_NV_vertex_program2) = glewGetExtension("GL_NV_vertex_program2");
#endif /* GL_NV_vertex_program2 */
#ifdef GL_NV_vertex_program2_option
  CONST_CAST(GLEW_NV_vertex_program2_option) = glewGetExtension("GL_NV_vertex_program2_option");
#endif /* GL_NV_vertex_program2_option */
#ifdef GL_NV_vertex_program3
  CONST_CAST(GLEW_NV_vertex_program3) = glewGetExtension("GL_NV_vertex_program3");
#endif /* GL_NV_vertex_program3 */
#ifdef GL_NV_vertex_program4
  CONST_CAST(GLEW_NV_vertex_program4) = glewGetExtension("GL_NV_vertex_program4");
#endif /* GL_NV_vertex_program4 */
#ifdef GL_OES_byte_coordinates
  CONST_CAST(GLEW_OES_byte_coordinates) = glewGetExtension("GL_OES_byte_coordinates");
#endif /* GL_OES_byte_coordinates */
#ifdef GL_OES_compressed_paletted_texture
  CONST_CAST(GLEW_OES_compressed_paletted_texture) = glewGetExtension("GL_OES_compressed_paletted_texture");
#endif /* GL_OES_compressed_paletted_texture */
#ifdef GL_OES_read_format
  CONST_CAST(GLEW_OES_read_format) = glewGetExtension("GL_OES_read_format");
#endif /* GL_OES_read_format */
#ifdef GL_OES_single_precision
  CONST_CAST(GLEW_OES_single_precision) = glewGetExtension("GL_OES_single_precision");
  if (glewExperimental || GLEW_OES_single_precision) CONST_CAST(GLEW_OES_single_precision) = !_glewInit_GL_OES_single_precision(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_OES_single_precision */
#ifdef GL_OML_interlace
  CONST_CAST(GLEW_OML_interlace) = glewGetExtension("GL_OML_interlace");
#endif /* GL_OML_interlace */
#ifdef GL_OML_resample
  CONST_CAST(GLEW_OML_resample) = glewGetExtension("GL_OML_resample");
#endif /* GL_OML_resample */
#ifdef GL_OML_subsample
  CONST_CAST(GLEW_OML_subsample) = glewGetExtension("GL_OML_subsample");
#endif /* GL_OML_subsample */
#ifdef GL_PGI_misc_hints
  CONST_CAST(GLEW_PGI_misc_hints) = glewGetExtension("GL_PGI_misc_hints");
#endif /* GL_PGI_misc_hints */
#ifdef GL_PGI_vertex_hints
  CONST_CAST(GLEW_PGI_vertex_hints) = glewGetExtension("GL_PGI_vertex_hints");
#endif /* GL_PGI_vertex_hints */
#ifdef GL_REND_screen_coordinates
  CONST_CAST(GLEW_REND_screen_coordinates) = glewGetExtension("GL_REND_screen_coordinates");
#endif /* GL_REND_screen_coordinates */
#ifdef GL_S3_s3tc
  CONST_CAST(GLEW_S3_s3tc) = glewGetExtension("GL_S3_s3tc");
#endif /* GL_S3_s3tc */
#ifdef GL_SGIS_color_range
  CONST_CAST(GLEW_SGIS_color_range) = glewGetExtension("GL_SGIS_color_range");
#endif /* GL_SGIS_color_range */
#ifdef GL_SGIS_detail_texture
  CONST_CAST(GLEW_SGIS_detail_texture) = glewGetExtension("GL_SGIS_detail_texture");
  if (glewExperimental || GLEW_SGIS_detail_texture) CONST_CAST(GLEW_SGIS_detail_texture) = !_glewInit_GL_SGIS_detail_texture(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_SGIS_detail_texture */
#ifdef GL_SGIS_fog_function
  CONST_CAST(GLEW_SGIS_fog_function) = glewGetExtension("GL_SGIS_fog_function");
  if (glewExperimental || GLEW_SGIS_fog_function) CONST_CAST(GLEW_SGIS_fog_function) = !_glewInit_GL_SGIS_fog_function(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_SGIS_fog_function */
#ifdef GL_SGIS_generate_mipmap
  CONST_CAST(GLEW_SGIS_generate_mipmap) = glewGetExtension("GL_SGIS_generate_mipmap");
#endif /* GL_SGIS_generate_mipmap */
#ifdef GL_SGIS_multisample
  CONST_CAST(GLEW_SGIS_multisample) = glewGetExtension("GL_SGIS_multisample");
  if (glewExperimental || GLEW_SGIS_multisample) CONST_CAST(GLEW_SGIS_multisample) = !_glewInit_GL_SGIS_multisample(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_SGIS_multisample */
#ifdef GL_SGIS_pixel_texture
  CONST_CAST(GLEW_SGIS_pixel_texture) = glewGetExtension("GL_SGIS_pixel_texture");
#endif /* GL_SGIS_pixel_texture */
#ifdef GL_SGIS_point_line_texgen
  CONST_CAST(GLEW_SGIS_point_line_texgen) = glewGetExtension("GL_SGIS_point_line_texgen");
#endif /* GL_SGIS_point_line_texgen */
#ifdef GL_SGIS_sharpen_texture
  CONST_CAST(GLEW_SGIS_sharpen_texture) = glewGetExtension("GL_SGIS_sharpen_texture");
  if (glewExperimental || GLEW_SGIS_sharpen_texture) CONST_CAST(GLEW_SGIS_sharpen_texture) = !_glewInit_GL_SGIS_sharpen_texture(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_SGIS_sharpen_texture */
#ifdef GL_SGIS_texture4D
  CONST_CAST(GLEW_SGIS_texture4D) = glewGetExtension("GL_SGIS_texture4D");
  if (glewExperimental || GLEW_SGIS_texture4D) CONST_CAST(GLEW_SGIS_texture4D) = !_glewInit_GL_SGIS_texture4D(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_SGIS_texture4D */
#ifdef GL_SGIS_texture_border_clamp
  CONST_CAST(GLEW_SGIS_texture_border_clamp) = glewGetExtension("GL_SGIS_texture_border_clamp");
#endif /* GL_SGIS_texture_border_clamp */
#ifdef GL_SGIS_texture_edge_clamp
  CONST_CAST(GLEW_SGIS_texture_edge_clamp) = glewGetExtension("GL_SGIS_texture_edge_clamp");
#endif /* GL_SGIS_texture_edge_clamp */
#ifdef GL_SGIS_texture_filter4
  CONST_CAST(GLEW_SGIS_texture_filter4) = glewGetExtension("GL_SGIS_texture_filter4");
  if (glewExperimental || GLEW_SGIS_texture_filter4) CONST_CAST(GLEW_SGIS_texture_filter4) = !_glewInit_GL_SGIS_texture_filter4(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_SGIS_texture_filter4 */
#ifdef GL_SGIS_texture_lod
  CONST_CAST(GLEW_SGIS_texture_lod) = glewGetExtension("GL_SGIS_texture_lod");
#endif /* GL_SGIS_texture_lod */
#ifdef GL_SGIS_texture_select
  CONST_CAST(GLEW_SGIS_texture_select) = glewGetExtension("GL_SGIS_texture_select");
#endif /* GL_SGIS_texture_select */
#ifdef GL_SGIX_async
  CONST_CAST(GLEW_SGIX_async) = glewGetExtension("GL_SGIX_async");
  if (glewExperimental || GLEW_SGIX_async) CONST_CAST(GLEW_SGIX_async) = !_glewInit_GL_SGIX_async(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_SGIX_async */
#ifdef GL_SGIX_async_histogram
  CONST_CAST(GLEW_SGIX_async_histogram) = glewGetExtension("GL_SGIX_async_histogram");
#endif /* GL_SGIX_async_histogram */
#ifdef GL_SGIX_async_pixel
  CONST_CAST(GLEW_SGIX_async_pixel) = glewGetExtension("GL_SGIX_async_pixel");
#endif /* GL_SGIX_async_pixel */
#ifdef GL_SGIX_blend_alpha_minmax
  CONST_CAST(GLEW_SGIX_blend_alpha_minmax) = glewGetExtension("GL_SGIX_blend_alpha_minmax");
#endif /* GL_SGIX_blend_alpha_minmax */
#ifdef GL_SGIX_clipmap
  CONST_CAST(GLEW_SGIX_clipmap) = glewGetExtension("GL_SGIX_clipmap");
#endif /* GL_SGIX_clipmap */
#ifdef GL_SGIX_convolution_accuracy
  CONST_CAST(GLEW_SGIX_convolution_accuracy) = glewGetExtension("GL_SGIX_convolution_accuracy");
#endif /* GL_SGIX_convolution_accuracy */
#ifdef GL_SGIX_depth_texture
  CONST_CAST(GLEW_SGIX_depth_texture) = glewGetExtension("GL_SGIX_depth_texture");
#endif /* GL_SGIX_depth_texture */
#ifdef GL_SGIX_flush_raster
  CONST_CAST(GLEW_SGIX_flush_raster) = glewGetExtension("GL_SGIX_flush_raster");
  if (glewExperimental || GLEW_SGIX_flush_raster) CONST_CAST(GLEW_SGIX_flush_raster) = !_glewInit_GL_SGIX_flush_raster(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_SGIX_flush_raster */
#ifdef GL_SGIX_fog_offset
  CONST_CAST(GLEW_SGIX_fog_offset) = glewGetExtension("GL_SGIX_fog_offset");
#endif /* GL_SGIX_fog_offset */
#ifdef GL_SGIX_fog_texture
  CONST_CAST(GLEW_SGIX_fog_texture) = glewGetExtension("GL_SGIX_fog_texture");
  if (glewExperimental || GLEW_SGIX_fog_texture) CONST_CAST(GLEW_SGIX_fog_texture) = !_glewInit_GL_SGIX_fog_texture(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_SGIX_fog_texture */
#ifdef GL_SGIX_fragment_specular_lighting
  CONST_CAST(GLEW_SGIX_fragment_specular_lighting) = glewGetExtension("GL_SGIX_fragment_specular_lighting");
  if (glewExperimental || GLEW_SGIX_fragment_specular_lighting) CONST_CAST(GLEW_SGIX_fragment_specular_lighting) = !_glewInit_GL_SGIX_fragment_specular_lighting(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_SGIX_fragment_specular_lighting */
#ifdef GL_SGIX_framezoom
  CONST_CAST(GLEW_SGIX_framezoom) = glewGetExtension("GL_SGIX_framezoom");
  if (glewExperimental || GLEW_SGIX_framezoom) CONST_CAST(GLEW_SGIX_framezoom) = !_glewInit_GL_SGIX_framezoom(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_SGIX_framezoom */
#ifdef GL_SGIX_interlace
  CONST_CAST(GLEW_SGIX_interlace) = glewGetExtension("GL_SGIX_interlace");
#endif /* GL_SGIX_interlace */
#ifdef GL_SGIX_ir_instrument1
  CONST_CAST(GLEW_SGIX_ir_instrument1) = glewGetExtension("GL_SGIX_ir_instrument1");
#endif /* GL_SGIX_ir_instrument1 */
#ifdef GL_SGIX_list_priority
  CONST_CAST(GLEW_SGIX_list_priority) = glewGetExtension("GL_SGIX_list_priority");
#endif /* GL_SGIX_list_priority */
#ifdef GL_SGIX_pixel_texture
  CONST_CAST(GLEW_SGIX_pixel_texture) = glewGetExtension("GL_SGIX_pixel_texture");
  if (glewExperimental || GLEW_SGIX_pixel_texture) CONST_CAST(GLEW_SGIX_pixel_texture) = !_glewInit_GL_SGIX_pixel_texture(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_SGIX_pixel_texture */
#ifdef GL_SGIX_pixel_texture_bits
  CONST_CAST(GLEW_SGIX_pixel_texture_bits) = glewGetExtension("GL_SGIX_pixel_texture_bits");
#endif /* GL_SGIX_pixel_texture_bits */
#ifdef GL_SGIX_reference_plane
  CONST_CAST(GLEW_SGIX_reference_plane) = glewGetExtension("GL_SGIX_reference_plane");
  if (glewExperimental || GLEW_SGIX_reference_plane) CONST_CAST(GLEW_SGIX_reference_plane) = !_glewInit_GL_SGIX_reference_plane(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_SGIX_reference_plane */
#ifdef GL_SGIX_resample
  CONST_CAST(GLEW_SGIX_resample) = glewGetExtension("GL_SGIX_resample");
#endif /* GL_SGIX_resample */
#ifdef GL_SGIX_shadow
  CONST_CAST(GLEW_SGIX_shadow) = glewGetExtension("GL_SGIX_shadow");
#endif /* GL_SGIX_shadow */
#ifdef GL_SGIX_shadow_ambient
  CONST_CAST(GLEW_SGIX_shadow_ambient) = glewGetExtension("GL_SGIX_shadow_ambient");
#endif /* GL_SGIX_shadow_ambient */
#ifdef GL_SGIX_sprite
  CONST_CAST(GLEW_SGIX_sprite) = glewGetExtension("GL_SGIX_sprite");
  if (glewExperimental || GLEW_SGIX_sprite) CONST_CAST(GLEW_SGIX_sprite) = !_glewInit_GL_SGIX_sprite(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_SGIX_sprite */
#ifdef GL_SGIX_tag_sample_buffer
  CONST_CAST(GLEW_SGIX_tag_sample_buffer) = glewGetExtension("GL_SGIX_tag_sample_buffer");
  if (glewExperimental || GLEW_SGIX_tag_sample_buffer) CONST_CAST(GLEW_SGIX_tag_sample_buffer) = !_glewInit_GL_SGIX_tag_sample_buffer(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_SGIX_tag_sample_buffer */
#ifdef GL_SGIX_texture_add_env
  CONST_CAST(GLEW_SGIX_texture_add_env) = glewGetExtension("GL_SGIX_texture_add_env");
#endif /* GL_SGIX_texture_add_env */
#ifdef GL_SGIX_texture_coordinate_clamp
  CONST_CAST(GLEW_SGIX_texture_coordinate_clamp) = glewGetExtension("GL_SGIX_texture_coordinate_clamp");
#endif /* GL_SGIX_texture_coordinate_clamp */
#ifdef GL_SGIX_texture_lod_bias
  CONST_CAST(GLEW_SGIX_texture_lod_bias) = glewGetExtension("GL_SGIX_texture_lod_bias");
#endif /* GL_SGIX_texture_lod_bias */
#ifdef GL_SGIX_texture_multi_buffer
  CONST_CAST(GLEW_SGIX_texture_multi_buffer) = glewGetExtension("GL_SGIX_texture_multi_buffer");
#endif /* GL_SGIX_texture_multi_buffer */
#ifdef GL_SGIX_texture_range
  CONST_CAST(GLEW_SGIX_texture_range) = glewGetExtension("GL_SGIX_texture_range");
#endif /* GL_SGIX_texture_range */
#ifdef GL_SGIX_texture_scale_bias
  CONST_CAST(GLEW_SGIX_texture_scale_bias) = glewGetExtension("GL_SGIX_texture_scale_bias");
#endif /* GL_SGIX_texture_scale_bias */
#ifdef GL_SGIX_vertex_preclip
  CONST_CAST(GLEW_SGIX_vertex_preclip) = glewGetExtension("GL_SGIX_vertex_preclip");
#endif /* GL_SGIX_vertex_preclip */
#ifdef GL_SGIX_vertex_preclip_hint
  CONST_CAST(GLEW_SGIX_vertex_preclip_hint) = glewGetExtension("GL_SGIX_vertex_preclip_hint");
#endif /* GL_SGIX_vertex_preclip_hint */
#ifdef GL_SGIX_ycrcb
  CONST_CAST(GLEW_SGIX_ycrcb) = glewGetExtension("GL_SGIX_ycrcb");
#endif /* GL_SGIX_ycrcb */
#ifdef GL_SGI_color_matrix
  CONST_CAST(GLEW_SGI_color_matrix) = glewGetExtension("GL_SGI_color_matrix");
#endif /* GL_SGI_color_matrix */
#ifdef GL_SGI_color_table
  CONST_CAST(GLEW_SGI_color_table) = glewGetExtension("GL_SGI_color_table");
  if (glewExperimental || GLEW_SGI_color_table) CONST_CAST(GLEW_SGI_color_table) = !_glewInit_GL_SGI_color_table(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_SGI_color_table */
#ifdef GL_SGI_texture_color_table
  CONST_CAST(GLEW_SGI_texture_color_table) = glewGetExtension("GL_SGI_texture_color_table");
#endif /* GL_SGI_texture_color_table */
#ifdef GL_SUNX_constant_data
  CONST_CAST(GLEW_SUNX_constant_data) = glewGetExtension("GL_SUNX_constant_data");
  if (glewExperimental || GLEW_SUNX_constant_data) CONST_CAST(GLEW_SUNX_constant_data) = !_glewInit_GL_SUNX_constant_data(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_SUNX_constant_data */
#ifdef GL_SUN_convolution_border_modes
  CONST_CAST(GLEW_SUN_convolution_border_modes) = glewGetExtension("GL_SUN_convolution_border_modes");
#endif /* GL_SUN_convolution_border_modes */
#ifdef GL_SUN_global_alpha
  CONST_CAST(GLEW_SUN_global_alpha) = glewGetExtension("GL_SUN_global_alpha");
  if (glewExperimental || GLEW_SUN_global_alpha) CONST_CAST(GLEW_SUN_global_alpha) = !_glewInit_GL_SUN_global_alpha(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_SUN_global_alpha */
#ifdef GL_SUN_mesh_array
  CONST_CAST(GLEW_SUN_mesh_array) = glewGetExtension("GL_SUN_mesh_array");
#endif /* GL_SUN_mesh_array */
#ifdef GL_SUN_read_video_pixels
  CONST_CAST(GLEW_SUN_read_video_pixels) = glewGetExtension("GL_SUN_read_video_pixels");
  if (glewExperimental || GLEW_SUN_read_video_pixels) CONST_CAST(GLEW_SUN_read_video_pixels) = !_glewInit_GL_SUN_read_video_pixels(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_SUN_read_video_pixels */
#ifdef GL_SUN_slice_accum
  CONST_CAST(GLEW_SUN_slice_accum) = glewGetExtension("GL_SUN_slice_accum");
#endif /* GL_SUN_slice_accum */
#ifdef GL_SUN_triangle_list
  CONST_CAST(GLEW_SUN_triangle_list) = glewGetExtension("GL_SUN_triangle_list");
  if (glewExperimental || GLEW_SUN_triangle_list) CONST_CAST(GLEW_SUN_triangle_list) = !_glewInit_GL_SUN_triangle_list(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_SUN_triangle_list */
#ifdef GL_SUN_vertex
  CONST_CAST(GLEW_SUN_vertex) = glewGetExtension("GL_SUN_vertex");
  if (glewExperimental || GLEW_SUN_vertex) CONST_CAST(GLEW_SUN_vertex) = !_glewInit_GL_SUN_vertex(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_SUN_vertex */
#ifdef GL_WIN_phong_shading
  CONST_CAST(GLEW_WIN_phong_shading) = glewGetExtension("GL_WIN_phong_shading");
#endif /* GL_WIN_phong_shading */
#ifdef GL_WIN_specular_fog
  CONST_CAST(GLEW_WIN_specular_fog) = glewGetExtension("GL_WIN_specular_fog");
#endif /* GL_WIN_specular_fog */
#ifdef GL_WIN_swap_hint
  CONST_CAST(GLEW_WIN_swap_hint) = glewGetExtension("GL_WIN_swap_hint");
  if (glewExperimental || GLEW_WIN_swap_hint) CONST_CAST(GLEW_WIN_swap_hint) = !_glewInit_GL_WIN_swap_hint(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GL_WIN_swap_hint */

  return GLEW_OK;
}


#if defined(_WIN32)

#if !defined(GLEW_MX)

PFNWGLSETSTEREOEMITTERSTATE3DLPROC __wglewSetStereoEmitterState3DL = null;

PFNWGLBLITCONTEXTFRAMEBUFFERAMDPROC __wglewBlitContextFramebufferAMD = null;
PFNWGLCREATEASSOCIATEDCONTEXTAMDPROC __wglewCreateAssociatedContextAMD = null;
PFNWGLCREATEASSOCIATEDCONTEXTATTRIBSAMDPROC __wglewCreateAssociatedContextAttribsAMD = null;
PFNWGLDELETEASSOCIATEDCONTEXTAMDPROC __wglewDeleteAssociatedContextAMD = null;
PFNWGLGETCONTEXTGPUIDAMDPROC __wglewGetContextGPUIDAMD = null;
PFNWGLGETCURRENTASSOCIATEDCONTEXTAMDPROC __wglewGetCurrentAssociatedContextAMD = null;
PFNWGLGETGPUIDSAMDPROC __wglewGetGPUIDsAMD = null;
PFNWGLGETGPUINFOAMDPROC __wglewGetGPUInfoAMD = null;
PFNWGLMAKEASSOCIATEDCONTEXTCURRENTAMDPROC __wglewMakeAssociatedContextCurrentAMD = null;

PFNWGLCREATEBUFFERREGIONARBPROC __wglewCreateBufferRegionARB = null;
PFNWGLDELETEBUFFERREGIONARBPROC __wglewDeleteBufferRegionARB = null;
PFNWGLRESTOREBUFFERREGIONARBPROC __wglewRestoreBufferRegionARB = null;
PFNWGLSAVEBUFFERREGIONARBPROC __wglewSaveBufferRegionARB = null;

PFNWGLCREATECONTEXTATTRIBSARBPROC __wglewCreateContextAttribsARB = null;

PFNWGLGETEXTENSIONSSTRINGARBPROC __wglewGetExtensionsStringARB = null;

PFNWGLGETCURRENTREADDCARBPROC __wglewGetCurrentReadDCARB = null;
PFNWGLMAKECONTEXTCURRENTARBPROC __wglewMakeContextCurrentARB = null;

PFNWGLCREATEPBUFFERARBPROC __wglewCreatePbufferARB = null;
PFNWGLDESTROYPBUFFERARBPROC __wglewDestroyPbufferARB = null;
PFNWGLGETPBUFFERDCARBPROC __wglewGetPbufferDCARB = null;
PFNWGLQUERYPBUFFERARBPROC __wglewQueryPbufferARB = null;
PFNWGLRELEASEPBUFFERDCARBPROC __wglewReleasePbufferDCARB = null;

PFNWGLCHOOSEPIXELFORMATARBPROC __wglewChoosePixelFormatARB = null;
PFNWGLGETPIXELFORMATATTRIBFVARBPROC __wglewGetPixelFormatAttribfvARB = null;
PFNWGLGETPIXELFORMATATTRIBIVARBPROC __wglewGetPixelFormatAttribivARB = null;

PFNWGLBINDTEXIMAGEARBPROC __wglewBindTexImageARB = null;
PFNWGLRELEASETEXIMAGEARBPROC __wglewReleaseTexImageARB = null;
PFNWGLSETPBUFFERATTRIBARBPROC __wglewSetPbufferAttribARB = null;

PFNWGLBINDDISPLAYCOLORTABLEEXTPROC __wglewBindDisplayColorTableEXT = null;
PFNWGLCREATEDISPLAYCOLORTABLEEXTPROC __wglewCreateDisplayColorTableEXT = null;
PFNWGLDESTROYDISPLAYCOLORTABLEEXTPROC __wglewDestroyDisplayColorTableEXT = null;
PFNWGLLOADDISPLAYCOLORTABLEEXTPROC __wglewLoadDisplayColorTableEXT = null;

PFNWGLGETEXTENSIONSSTRINGEXTPROC __wglewGetExtensionsStringEXT = null;

PFNWGLGETCURRENTREADDCEXTPROC __wglewGetCurrentReadDCEXT = null;
PFNWGLMAKECONTEXTCURRENTEXTPROC __wglewMakeContextCurrentEXT = null;

PFNWGLCREATEPBUFFEREXTPROC __wglewCreatePbufferEXT = null;
PFNWGLDESTROYPBUFFEREXTPROC __wglewDestroyPbufferEXT = null;
PFNWGLGETPBUFFERDCEXTPROC __wglewGetPbufferDCEXT = null;
PFNWGLQUERYPBUFFEREXTPROC __wglewQueryPbufferEXT = null;
PFNWGLRELEASEPBUFFERDCEXTPROC __wglewReleasePbufferDCEXT = null;

PFNWGLCHOOSEPIXELFORMATEXTPROC __wglewChoosePixelFormatEXT = null;
PFNWGLGETPIXELFORMATATTRIBFVEXTPROC __wglewGetPixelFormatAttribfvEXT = null;
PFNWGLGETPIXELFORMATATTRIBIVEXTPROC __wglewGetPixelFormatAttribivEXT = null;

PFNWGLGETSWAPINTERVALEXTPROC __wglewGetSwapIntervalEXT = null;
PFNWGLSWAPINTERVALEXTPROC __wglewSwapIntervalEXT = null;

PFNWGLGETDIGITALVIDEOPARAMETERSI3DPROC __wglewGetDigitalVideoParametersI3D = null;
PFNWGLSETDIGITALVIDEOPARAMETERSI3DPROC __wglewSetDigitalVideoParametersI3D = null;

PFNWGLGETGAMMATABLEI3DPROC __wglewGetGammaTableI3D = null;
PFNWGLGETGAMMATABLEPARAMETERSI3DPROC __wglewGetGammaTableParametersI3D = null;
PFNWGLSETGAMMATABLEI3DPROC __wglewSetGammaTableI3D = null;
PFNWGLSETGAMMATABLEPARAMETERSI3DPROC __wglewSetGammaTableParametersI3D = null;

PFNWGLDISABLEGENLOCKI3DPROC __wglewDisableGenlockI3D = null;
PFNWGLENABLEGENLOCKI3DPROC __wglewEnableGenlockI3D = null;
PFNWGLGENLOCKSAMPLERATEI3DPROC __wglewGenlockSampleRateI3D = null;
PFNWGLGENLOCKSOURCEDELAYI3DPROC __wglewGenlockSourceDelayI3D = null;
PFNWGLGENLOCKSOURCEEDGEI3DPROC __wglewGenlockSourceEdgeI3D = null;
PFNWGLGENLOCKSOURCEI3DPROC __wglewGenlockSourceI3D = null;
PFNWGLGETGENLOCKSAMPLERATEI3DPROC __wglewGetGenlockSampleRateI3D = null;
PFNWGLGETGENLOCKSOURCEDELAYI3DPROC __wglewGetGenlockSourceDelayI3D = null;
PFNWGLGETGENLOCKSOURCEEDGEI3DPROC __wglewGetGenlockSourceEdgeI3D = null;
PFNWGLGETGENLOCKSOURCEI3DPROC __wglewGetGenlockSourceI3D = null;
PFNWGLISENABLEDGENLOCKI3DPROC __wglewIsEnabledGenlockI3D = null;
PFNWGLQUERYGENLOCKMAXSOURCEDELAYI3DPROC __wglewQueryGenlockMaxSourceDelayI3D = null;

PFNWGLASSOCIATEIMAGEBUFFEREVENTSI3DPROC __wglewAssociateImageBufferEventsI3D = null;
PFNWGLCREATEIMAGEBUFFERI3DPROC __wglewCreateImageBufferI3D = null;
PFNWGLDESTROYIMAGEBUFFERI3DPROC __wglewDestroyImageBufferI3D = null;
PFNWGLRELEASEIMAGEBUFFEREVENTSI3DPROC __wglewReleaseImageBufferEventsI3D = null;

PFNWGLDISABLEFRAMELOCKI3DPROC __wglewDisableFrameLockI3D = null;
PFNWGLENABLEFRAMELOCKI3DPROC __wglewEnableFrameLockI3D = null;
PFNWGLISENABLEDFRAMELOCKI3DPROC __wglewIsEnabledFrameLockI3D = null;
PFNWGLQUERYFRAMELOCKMASTERI3DPROC __wglewQueryFrameLockMasterI3D = null;

PFNWGLBEGINFRAMETRACKINGI3DPROC __wglewBeginFrameTrackingI3D = null;
PFNWGLENDFRAMETRACKINGI3DPROC __wglewEndFrameTrackingI3D = null;
PFNWGLGETFRAMEUSAGEI3DPROC __wglewGetFrameUsageI3D = null;
PFNWGLQUERYFRAMETRACKINGI3DPROC __wglewQueryFrameTrackingI3D = null;

PFNWGLCOPYIMAGESUBDATANVPROC __wglewCopyImageSubDataNV = null;

PFNWGLCREATEAFFINITYDCNVPROC __wglewCreateAffinityDCNV = null;
PFNWGLDELETEDCNVPROC __wglewDeleteDCNV = null;
PFNWGLENUMGPUDEVICESNVPROC __wglewEnumGpuDevicesNV = null;
PFNWGLENUMGPUSFROMAFFINITYDCNVPROC __wglewEnumGpusFromAffinityDCNV = null;
PFNWGLENUMGPUSNVPROC __wglewEnumGpusNV = null;

PFNWGLBINDVIDEODEVICENVPROC __wglewBindVideoDeviceNV = null;
PFNWGLENUMERATEVIDEODEVICESNVPROC __wglewEnumerateVideoDevicesNV = null;
PFNWGLQUERYCURRENTCONTEXTNVPROC __wglewQueryCurrentContextNV = null;

PFNWGLBINDSWAPBARRIERNVPROC __wglewBindSwapBarrierNV = null;
PFNWGLJOINSWAPGROUPNVPROC __wglewJoinSwapGroupNV = null;
PFNWGLQUERYFRAMECOUNTNVPROC __wglewQueryFrameCountNV = null;
PFNWGLQUERYMAXSWAPGROUPSNVPROC __wglewQueryMaxSwapGroupsNV = null;
PFNWGLQUERYSWAPGROUPNVPROC __wglewQuerySwapGroupNV = null;
PFNWGLRESETFRAMECOUNTNVPROC __wglewResetFrameCountNV = null;

PFNWGLALLOCATEMEMORYNVPROC __wglewAllocateMemoryNV = null;
PFNWGLFREEMEMORYNVPROC __wglewFreeMemoryNV = null;

PFNWGLBINDVIDEOIMAGENVPROC __wglewBindVideoImageNV = null;
PFNWGLGETVIDEODEVICENVPROC __wglewGetVideoDeviceNV = null;
PFNWGLGETVIDEOINFONVPROC __wglewGetVideoInfoNV = null;
PFNWGLRELEASEVIDEODEVICENVPROC __wglewReleaseVideoDeviceNV = null;
PFNWGLRELEASEVIDEOIMAGENVPROC __wglewReleaseVideoImageNV = null;
PFNWGLSENDPBUFFERTOVIDEONVPROC __wglewSendPbufferToVideoNV = null;

PFNWGLGETMSCRATEOMLPROC __wglewGetMscRateOML = null;
PFNWGLGETSYNCVALUESOMLPROC __wglewGetSyncValuesOML = null;
PFNWGLSWAPBUFFERSMSCOMLPROC __wglewSwapBuffersMscOML = null;
PFNWGLSWAPLAYERBUFFERSMSCOMLPROC __wglewSwapLayerBuffersMscOML = null;
PFNWGLWAITFORMSCOMLPROC __wglewWaitForMscOML = null;
PFNWGLWAITFORSBCOMLPROC __wglewWaitForSbcOML = null;
GLboolean __WGLEW_3DFX_multisample = GL_FALSE;
GLboolean __WGLEW_3DL_stereo_control = GL_FALSE;
GLboolean __WGLEW_AMD_gpu_association = GL_FALSE;
GLboolean __WGLEW_ARB_buffer_region = GL_FALSE;
GLboolean __WGLEW_ARB_create_context = GL_FALSE;
GLboolean __WGLEW_ARB_create_context_profile = GL_FALSE;
GLboolean __WGLEW_ARB_extensions_string = GL_FALSE;
GLboolean __WGLEW_ARB_framebuffer_sRGB = GL_FALSE;
GLboolean __WGLEW_ARB_make_current_read = GL_FALSE;
GLboolean __WGLEW_ARB_multisample = GL_FALSE;
GLboolean __WGLEW_ARB_pbuffer = GL_FALSE;
GLboolean __WGLEW_ARB_pixel_format = GL_FALSE;
GLboolean __WGLEW_ARB_pixel_format_float = GL_FALSE;
GLboolean __WGLEW_ARB_render_texture = GL_FALSE;
GLboolean __WGLEW_ATI_pixel_format_float = GL_FALSE;
GLboolean __WGLEW_ATI_render_texture_rectangle = GL_FALSE;
GLboolean __WGLEW_EXT_depth_float = GL_FALSE;
GLboolean __WGLEW_EXT_display_color_table = GL_FALSE;
GLboolean __WGLEW_EXT_extensions_string = GL_FALSE;
GLboolean __WGLEW_EXT_framebuffer_sRGB = GL_FALSE;
GLboolean __WGLEW_EXT_make_current_read = GL_FALSE;
GLboolean __WGLEW_EXT_multisample = GL_FALSE;
GLboolean __WGLEW_EXT_pbuffer = GL_FALSE;
GLboolean __WGLEW_EXT_pixel_format = GL_FALSE;
GLboolean __WGLEW_EXT_pixel_format_packed_float = GL_FALSE;
GLboolean __WGLEW_EXT_swap_control = GL_FALSE;
GLboolean __WGLEW_I3D_digital_video_control = GL_FALSE;
GLboolean __WGLEW_I3D_gamma = GL_FALSE;
GLboolean __WGLEW_I3D_genlock = GL_FALSE;
GLboolean __WGLEW_I3D_image_buffer = GL_FALSE;
GLboolean __WGLEW_I3D_swap_frame_lock = GL_FALSE;
GLboolean __WGLEW_I3D_swap_frame_usage = GL_FALSE;
GLboolean __WGLEW_NV_copy_image = GL_FALSE;
GLboolean __WGLEW_NV_float_buffer = GL_FALSE;
GLboolean __WGLEW_NV_gpu_affinity = GL_FALSE;
GLboolean __WGLEW_NV_present_video = GL_FALSE;
GLboolean __WGLEW_NV_render_depth_texture = GL_FALSE;
GLboolean __WGLEW_NV_render_texture_rectangle = GL_FALSE;
GLboolean __WGLEW_NV_swap_group = GL_FALSE;
GLboolean __WGLEW_NV_vertex_array_range = GL_FALSE;
GLboolean __WGLEW_NV_video_output = GL_FALSE;
GLboolean __WGLEW_OML_sync_control = GL_FALSE;

#endif /* !GLEW_MX */

#ifdef WGL_3DFX_multisample

#endif /* WGL_3DFX_multisample */

#ifdef WGL_3DL_stereo_control

static GLboolean _glewInit_WGL_3DL_stereo_control (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglSetStereoEmitterState3DL = (PFNWGLSETSTEREOEMITTERSTATE3DLPROC)glewGetProcAddress((const GLubyte*)"wglSetStereoEmitterState3DL")) == null) || r;

  return r;
}

#endif /* WGL_3DL_stereo_control */

#ifdef WGL_AMD_gpu_association

static GLboolean _glewInit_WGL_AMD_gpu_association (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglBlitContextFramebufferAMD = (PFNWGLBLITCONTEXTFRAMEBUFFERAMDPROC)glewGetProcAddress((const GLubyte*)"wglBlitContextFramebufferAMD")) == null) || r;
  r = ((wglCreateAssociatedContextAMD = (PFNWGLCREATEASSOCIATEDCONTEXTAMDPROC)glewGetProcAddress((const GLubyte*)"wglCreateAssociatedContextAMD")) == null) || r;
  r = ((wglCreateAssociatedContextAttribsAMD = (PFNWGLCREATEASSOCIATEDCONTEXTATTRIBSAMDPROC)glewGetProcAddress((const GLubyte*)"wglCreateAssociatedContextAttribsAMD")) == null) || r;
  r = ((wglDeleteAssociatedContextAMD = (PFNWGLDELETEASSOCIATEDCONTEXTAMDPROC)glewGetProcAddress((const GLubyte*)"wglDeleteAssociatedContextAMD")) == null) || r;
  r = ((wglGetContextGPUIDAMD = (PFNWGLGETCONTEXTGPUIDAMDPROC)glewGetProcAddress((const GLubyte*)"wglGetContextGPUIDAMD")) == null) || r;
  r = ((wglGetCurrentAssociatedContextAMD = (PFNWGLGETCURRENTASSOCIATEDCONTEXTAMDPROC)glewGetProcAddress((const GLubyte*)"wglGetCurrentAssociatedContextAMD")) == null) || r;
  r = ((wglGetGPUIDsAMD = (PFNWGLGETGPUIDSAMDPROC)glewGetProcAddress((const GLubyte*)"wglGetGPUIDsAMD")) == null) || r;
  r = ((wglGetGPUInfoAMD = (PFNWGLGETGPUINFOAMDPROC)glewGetProcAddress((const GLubyte*)"wglGetGPUInfoAMD")) == null) || r;
  r = ((wglMakeAssociatedContextCurrentAMD = (PFNWGLMAKEASSOCIATEDCONTEXTCURRENTAMDPROC)glewGetProcAddress((const GLubyte*)"wglMakeAssociatedContextCurrentAMD")) == null) || r;

  return r;
}

#endif /* WGL_AMD_gpu_association */

#ifdef WGL_ARB_buffer_region

static GLboolean _glewInit_WGL_ARB_buffer_region (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglCreateBufferRegionARB = (PFNWGLCREATEBUFFERREGIONARBPROC)glewGetProcAddress((const GLubyte*)"wglCreateBufferRegionARB")) == null) || r;
  r = ((wglDeleteBufferRegionARB = (PFNWGLDELETEBUFFERREGIONARBPROC)glewGetProcAddress((const GLubyte*)"wglDeleteBufferRegionARB")) == null) || r;
  r = ((wglRestoreBufferRegionARB = (PFNWGLRESTOREBUFFERREGIONARBPROC)glewGetProcAddress((const GLubyte*)"wglRestoreBufferRegionARB")) == null) || r;
  r = ((wglSaveBufferRegionARB = (PFNWGLSAVEBUFFERREGIONARBPROC)glewGetProcAddress((const GLubyte*)"wglSaveBufferRegionARB")) == null) || r;

  return r;
}

#endif /* WGL_ARB_buffer_region */

#ifdef WGL_ARB_create_context

static GLboolean _glewInit_WGL_ARB_create_context (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)glewGetProcAddress((const GLubyte*)"wglCreateContextAttribsARB")) == null) || r;

  return r;
}

#endif /* WGL_ARB_create_context */

#ifdef WGL_ARB_create_context_profile

#endif /* WGL_ARB_create_context_profile */

#ifdef WGL_ARB_extensions_string

static GLboolean _glewInit_WGL_ARB_extensions_string (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)glewGetProcAddress((const GLubyte*)"wglGetExtensionsStringARB")) == null) || r;

  return r;
}

#endif /* WGL_ARB_extensions_string */

#ifdef WGL_ARB_framebuffer_sRGB

#endif /* WGL_ARB_framebuffer_sRGB */

#ifdef WGL_ARB_make_current_read

static GLboolean _glewInit_WGL_ARB_make_current_read (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglGetCurrentReadDCARB = (PFNWGLGETCURRENTREADDCARBPROC)glewGetProcAddress((const GLubyte*)"wglGetCurrentReadDCARB")) == null) || r;
  r = ((wglMakeContextCurrentARB = (PFNWGLMAKECONTEXTCURRENTARBPROC)glewGetProcAddress((const GLubyte*)"wglMakeContextCurrentARB")) == null) || r;

  return r;
}

#endif /* WGL_ARB_make_current_read */

#ifdef WGL_ARB_multisample

#endif /* WGL_ARB_multisample */

#ifdef WGL_ARB_pbuffer

static GLboolean _glewInit_WGL_ARB_pbuffer (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglCreatePbufferARB = (PFNWGLCREATEPBUFFERARBPROC)glewGetProcAddress((const GLubyte*)"wglCreatePbufferARB")) == null) || r;
  r = ((wglDestroyPbufferARB = (PFNWGLDESTROYPBUFFERARBPROC)glewGetProcAddress((const GLubyte*)"wglDestroyPbufferARB")) == null) || r;
  r = ((wglGetPbufferDCARB = (PFNWGLGETPBUFFERDCARBPROC)glewGetProcAddress((const GLubyte*)"wglGetPbufferDCARB")) == null) || r;
  r = ((wglQueryPbufferARB = (PFNWGLQUERYPBUFFERARBPROC)glewGetProcAddress((const GLubyte*)"wglQueryPbufferARB")) == null) || r;
  r = ((wglReleasePbufferDCARB = (PFNWGLRELEASEPBUFFERDCARBPROC)glewGetProcAddress((const GLubyte*)"wglReleasePbufferDCARB")) == null) || r;

  return r;
}

#endif /* WGL_ARB_pbuffer */

#ifdef WGL_ARB_pixel_format

static GLboolean _glewInit_WGL_ARB_pixel_format (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)glewGetProcAddress((const GLubyte*)"wglChoosePixelFormatARB")) == null) || r;
  r = ((wglGetPixelFormatAttribfvARB = (PFNWGLGETPIXELFORMATATTRIBFVARBPROC)glewGetProcAddress((const GLubyte*)"wglGetPixelFormatAttribfvARB")) == null) || r;
  r = ((wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)glewGetProcAddress((const GLubyte*)"wglGetPixelFormatAttribivARB")) == null) || r;

  return r;
}

#endif /* WGL_ARB_pixel_format */

#ifdef WGL_ARB_pixel_format_float

#endif /* WGL_ARB_pixel_format_float */

#ifdef WGL_ARB_render_texture

static GLboolean _glewInit_WGL_ARB_render_texture (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglBindTexImageARB = (PFNWGLBINDTEXIMAGEARBPROC)glewGetProcAddress((const GLubyte*)"wglBindTexImageARB")) == null) || r;
  r = ((wglReleaseTexImageARB = (PFNWGLRELEASETEXIMAGEARBPROC)glewGetProcAddress((const GLubyte*)"wglReleaseTexImageARB")) == null) || r;
  r = ((wglSetPbufferAttribARB = (PFNWGLSETPBUFFERATTRIBARBPROC)glewGetProcAddress((const GLubyte*)"wglSetPbufferAttribARB")) == null) || r;

  return r;
}

#endif /* WGL_ARB_render_texture */

#ifdef WGL_ATI_pixel_format_float

#endif /* WGL_ATI_pixel_format_float */

#ifdef WGL_ATI_render_texture_rectangle

#endif /* WGL_ATI_render_texture_rectangle */

#ifdef WGL_EXT_depth_float

#endif /* WGL_EXT_depth_float */

#ifdef WGL_EXT_display_color_table

static GLboolean _glewInit_WGL_EXT_display_color_table (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglBindDisplayColorTableEXT = (PFNWGLBINDDISPLAYCOLORTABLEEXTPROC)glewGetProcAddress((const GLubyte*)"wglBindDisplayColorTableEXT")) == null) || r;
  r = ((wglCreateDisplayColorTableEXT = (PFNWGLCREATEDISPLAYCOLORTABLEEXTPROC)glewGetProcAddress((const GLubyte*)"wglCreateDisplayColorTableEXT")) == null) || r;
  r = ((wglDestroyDisplayColorTableEXT = (PFNWGLDESTROYDISPLAYCOLORTABLEEXTPROC)glewGetProcAddress((const GLubyte*)"wglDestroyDisplayColorTableEXT")) == null) || r;
  r = ((wglLoadDisplayColorTableEXT = (PFNWGLLOADDISPLAYCOLORTABLEEXTPROC)glewGetProcAddress((const GLubyte*)"wglLoadDisplayColorTableEXT")) == null) || r;

  return r;
}

#endif /* WGL_EXT_display_color_table */

#ifdef WGL_EXT_extensions_string

static GLboolean _glewInit_WGL_EXT_extensions_string (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)glewGetProcAddress((const GLubyte*)"wglGetExtensionsStringEXT")) == null) || r;

  return r;
}

#endif /* WGL_EXT_extensions_string */

#ifdef WGL_EXT_framebuffer_sRGB

#endif /* WGL_EXT_framebuffer_sRGB */

#ifdef WGL_EXT_make_current_read

static GLboolean _glewInit_WGL_EXT_make_current_read (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglGetCurrentReadDCEXT = (PFNWGLGETCURRENTREADDCEXTPROC)glewGetProcAddress((const GLubyte*)"wglGetCurrentReadDCEXT")) == null) || r;
  r = ((wglMakeContextCurrentEXT = (PFNWGLMAKECONTEXTCURRENTEXTPROC)glewGetProcAddress((const GLubyte*)"wglMakeContextCurrentEXT")) == null) || r;

  return r;
}

#endif /* WGL_EXT_make_current_read */

#ifdef WGL_EXT_multisample

#endif /* WGL_EXT_multisample */

#ifdef WGL_EXT_pbuffer

static GLboolean _glewInit_WGL_EXT_pbuffer (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglCreatePbufferEXT = (PFNWGLCREATEPBUFFEREXTPROC)glewGetProcAddress((const GLubyte*)"wglCreatePbufferEXT")) == null) || r;
  r = ((wglDestroyPbufferEXT = (PFNWGLDESTROYPBUFFEREXTPROC)glewGetProcAddress((const GLubyte*)"wglDestroyPbufferEXT")) == null) || r;
  r = ((wglGetPbufferDCEXT = (PFNWGLGETPBUFFERDCEXTPROC)glewGetProcAddress((const GLubyte*)"wglGetPbufferDCEXT")) == null) || r;
  r = ((wglQueryPbufferEXT = (PFNWGLQUERYPBUFFEREXTPROC)glewGetProcAddress((const GLubyte*)"wglQueryPbufferEXT")) == null) || r;
  r = ((wglReleasePbufferDCEXT = (PFNWGLRELEASEPBUFFERDCEXTPROC)glewGetProcAddress((const GLubyte*)"wglReleasePbufferDCEXT")) == null) || r;

  return r;
}

#endif /* WGL_EXT_pbuffer */

#ifdef WGL_EXT_pixel_format

static GLboolean _glewInit_WGL_EXT_pixel_format (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglChoosePixelFormatEXT = (PFNWGLCHOOSEPIXELFORMATEXTPROC)glewGetProcAddress((const GLubyte*)"wglChoosePixelFormatEXT")) == null) || r;
  r = ((wglGetPixelFormatAttribfvEXT = (PFNWGLGETPIXELFORMATATTRIBFVEXTPROC)glewGetProcAddress((const GLubyte*)"wglGetPixelFormatAttribfvEXT")) == null) || r;
  r = ((wglGetPixelFormatAttribivEXT = (PFNWGLGETPIXELFORMATATTRIBIVEXTPROC)glewGetProcAddress((const GLubyte*)"wglGetPixelFormatAttribivEXT")) == null) || r;

  return r;
}

#endif /* WGL_EXT_pixel_format */

#ifdef WGL_EXT_pixel_format_packed_float

#endif /* WGL_EXT_pixel_format_packed_float */

#ifdef WGL_EXT_swap_control

static GLboolean _glewInit_WGL_EXT_swap_control (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)glewGetProcAddress((const GLubyte*)"wglGetSwapIntervalEXT")) == null) || r;
  r = ((wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)glewGetProcAddress((const GLubyte*)"wglSwapIntervalEXT")) == null) || r;

  return r;
}

#endif /* WGL_EXT_swap_control */

#ifdef WGL_I3D_digital_video_control

static GLboolean _glewInit_WGL_I3D_digital_video_control (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglGetDigitalVideoParametersI3D = (PFNWGLGETDIGITALVIDEOPARAMETERSI3DPROC)glewGetProcAddress((const GLubyte*)"wglGetDigitalVideoParametersI3D")) == null) || r;
  r = ((wglSetDigitalVideoParametersI3D = (PFNWGLSETDIGITALVIDEOPARAMETERSI3DPROC)glewGetProcAddress((const GLubyte*)"wglSetDigitalVideoParametersI3D")) == null) || r;

  return r;
}

#endif /* WGL_I3D_digital_video_control */

#ifdef WGL_I3D_gamma

static GLboolean _glewInit_WGL_I3D_gamma (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglGetGammaTableI3D = (PFNWGLGETGAMMATABLEI3DPROC)glewGetProcAddress((const GLubyte*)"wglGetGammaTableI3D")) == null) || r;
  r = ((wglGetGammaTableParametersI3D = (PFNWGLGETGAMMATABLEPARAMETERSI3DPROC)glewGetProcAddress((const GLubyte*)"wglGetGammaTableParametersI3D")) == null) || r;
  r = ((wglSetGammaTableI3D = (PFNWGLSETGAMMATABLEI3DPROC)glewGetProcAddress((const GLubyte*)"wglSetGammaTableI3D")) == null) || r;
  r = ((wglSetGammaTableParametersI3D = (PFNWGLSETGAMMATABLEPARAMETERSI3DPROC)glewGetProcAddress((const GLubyte*)"wglSetGammaTableParametersI3D")) == null) || r;

  return r;
}

#endif /* WGL_I3D_gamma */

#ifdef WGL_I3D_genlock

static GLboolean _glewInit_WGL_I3D_genlock (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglDisableGenlockI3D = (PFNWGLDISABLEGENLOCKI3DPROC)glewGetProcAddress((const GLubyte*)"wglDisableGenlockI3D")) == null) || r;
  r = ((wglEnableGenlockI3D = (PFNWGLENABLEGENLOCKI3DPROC)glewGetProcAddress((const GLubyte*)"wglEnableGenlockI3D")) == null) || r;
  r = ((wglGenlockSampleRateI3D = (PFNWGLGENLOCKSAMPLERATEI3DPROC)glewGetProcAddress((const GLubyte*)"wglGenlockSampleRateI3D")) == null) || r;
  r = ((wglGenlockSourceDelayI3D = (PFNWGLGENLOCKSOURCEDELAYI3DPROC)glewGetProcAddress((const GLubyte*)"wglGenlockSourceDelayI3D")) == null) || r;
  r = ((wglGenlockSourceEdgeI3D = (PFNWGLGENLOCKSOURCEEDGEI3DPROC)glewGetProcAddress((const GLubyte*)"wglGenlockSourceEdgeI3D")) == null) || r;
  r = ((wglGenlockSourceI3D = (PFNWGLGENLOCKSOURCEI3DPROC)glewGetProcAddress((const GLubyte*)"wglGenlockSourceI3D")) == null) || r;
  r = ((wglGetGenlockSampleRateI3D = (PFNWGLGETGENLOCKSAMPLERATEI3DPROC)glewGetProcAddress((const GLubyte*)"wglGetGenlockSampleRateI3D")) == null) || r;
  r = ((wglGetGenlockSourceDelayI3D = (PFNWGLGETGENLOCKSOURCEDELAYI3DPROC)glewGetProcAddress((const GLubyte*)"wglGetGenlockSourceDelayI3D")) == null) || r;
  r = ((wglGetGenlockSourceEdgeI3D = (PFNWGLGETGENLOCKSOURCEEDGEI3DPROC)glewGetProcAddress((const GLubyte*)"wglGetGenlockSourceEdgeI3D")) == null) || r;
  r = ((wglGetGenlockSourceI3D = (PFNWGLGETGENLOCKSOURCEI3DPROC)glewGetProcAddress((const GLubyte*)"wglGetGenlockSourceI3D")) == null) || r;
  r = ((wglIsEnabledGenlockI3D = (PFNWGLISENABLEDGENLOCKI3DPROC)glewGetProcAddress((const GLubyte*)"wglIsEnabledGenlockI3D")) == null) || r;
  r = ((wglQueryGenlockMaxSourceDelayI3D = (PFNWGLQUERYGENLOCKMAXSOURCEDELAYI3DPROC)glewGetProcAddress((const GLubyte*)"wglQueryGenlockMaxSourceDelayI3D")) == null) || r;

  return r;
}

#endif /* WGL_I3D_genlock */

#ifdef WGL_I3D_image_buffer

static GLboolean _glewInit_WGL_I3D_image_buffer (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglAssociateImageBufferEventsI3D = (PFNWGLASSOCIATEIMAGEBUFFEREVENTSI3DPROC)glewGetProcAddress((const GLubyte*)"wglAssociateImageBufferEventsI3D")) == null) || r;
  r = ((wglCreateImageBufferI3D = (PFNWGLCREATEIMAGEBUFFERI3DPROC)glewGetProcAddress((const GLubyte*)"wglCreateImageBufferI3D")) == null) || r;
  r = ((wglDestroyImageBufferI3D = (PFNWGLDESTROYIMAGEBUFFERI3DPROC)glewGetProcAddress((const GLubyte*)"wglDestroyImageBufferI3D")) == null) || r;
  r = ((wglReleaseImageBufferEventsI3D = (PFNWGLRELEASEIMAGEBUFFEREVENTSI3DPROC)glewGetProcAddress((const GLubyte*)"wglReleaseImageBufferEventsI3D")) == null) || r;

  return r;
}

#endif /* WGL_I3D_image_buffer */

#ifdef WGL_I3D_swap_frame_lock

static GLboolean _glewInit_WGL_I3D_swap_frame_lock (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglDisableFrameLockI3D = (PFNWGLDISABLEFRAMELOCKI3DPROC)glewGetProcAddress((const GLubyte*)"wglDisableFrameLockI3D")) == null) || r;
  r = ((wglEnableFrameLockI3D = (PFNWGLENABLEFRAMELOCKI3DPROC)glewGetProcAddress((const GLubyte*)"wglEnableFrameLockI3D")) == null) || r;
  r = ((wglIsEnabledFrameLockI3D = (PFNWGLISENABLEDFRAMELOCKI3DPROC)glewGetProcAddress((const GLubyte*)"wglIsEnabledFrameLockI3D")) == null) || r;
  r = ((wglQueryFrameLockMasterI3D = (PFNWGLQUERYFRAMELOCKMASTERI3DPROC)glewGetProcAddress((const GLubyte*)"wglQueryFrameLockMasterI3D")) == null) || r;

  return r;
}

#endif /* WGL_I3D_swap_frame_lock */

#ifdef WGL_I3D_swap_frame_usage

static GLboolean _glewInit_WGL_I3D_swap_frame_usage (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglBeginFrameTrackingI3D = (PFNWGLBEGINFRAMETRACKINGI3DPROC)glewGetProcAddress((const GLubyte*)"wglBeginFrameTrackingI3D")) == null) || r;
  r = ((wglEndFrameTrackingI3D = (PFNWGLENDFRAMETRACKINGI3DPROC)glewGetProcAddress((const GLubyte*)"wglEndFrameTrackingI3D")) == null) || r;
  r = ((wglGetFrameUsageI3D = (PFNWGLGETFRAMEUSAGEI3DPROC)glewGetProcAddress((const GLubyte*)"wglGetFrameUsageI3D")) == null) || r;
  r = ((wglQueryFrameTrackingI3D = (PFNWGLQUERYFRAMETRACKINGI3DPROC)glewGetProcAddress((const GLubyte*)"wglQueryFrameTrackingI3D")) == null) || r;

  return r;
}

#endif /* WGL_I3D_swap_frame_usage */

#ifdef WGL_NV_copy_image

static GLboolean _glewInit_WGL_NV_copy_image (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglCopyImageSubDataNV = (PFNWGLCOPYIMAGESUBDATANVPROC)glewGetProcAddress((const GLubyte*)"wglCopyImageSubDataNV")) == null) || r;

  return r;
}

#endif /* WGL_NV_copy_image */

#ifdef WGL_NV_float_buffer

#endif /* WGL_NV_float_buffer */

#ifdef WGL_NV_gpu_affinity

static GLboolean _glewInit_WGL_NV_gpu_affinity (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglCreateAffinityDCNV = (PFNWGLCREATEAFFINITYDCNVPROC)glewGetProcAddress((const GLubyte*)"wglCreateAffinityDCNV")) == null) || r;
  r = ((wglDeleteDCNV = (PFNWGLDELETEDCNVPROC)glewGetProcAddress((const GLubyte*)"wglDeleteDCNV")) == null) || r;
  r = ((wglEnumGpuDevicesNV = (PFNWGLENUMGPUDEVICESNVPROC)glewGetProcAddress((const GLubyte*)"wglEnumGpuDevicesNV")) == null) || r;
  r = ((wglEnumGpusFromAffinityDCNV = (PFNWGLENUMGPUSFROMAFFINITYDCNVPROC)glewGetProcAddress((const GLubyte*)"wglEnumGpusFromAffinityDCNV")) == null) || r;
  r = ((wglEnumGpusNV = (PFNWGLENUMGPUSNVPROC)glewGetProcAddress((const GLubyte*)"wglEnumGpusNV")) == null) || r;

  return r;
}

#endif /* WGL_NV_gpu_affinity */

#ifdef WGL_NV_present_video

static GLboolean _glewInit_WGL_NV_present_video (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglBindVideoDeviceNV = (PFNWGLBINDVIDEODEVICENVPROC)glewGetProcAddress((const GLubyte*)"wglBindVideoDeviceNV")) == null) || r;
  r = ((wglEnumerateVideoDevicesNV = (PFNWGLENUMERATEVIDEODEVICESNVPROC)glewGetProcAddress((const GLubyte*)"wglEnumerateVideoDevicesNV")) == null) || r;
  r = ((wglQueryCurrentContextNV = (PFNWGLQUERYCURRENTCONTEXTNVPROC)glewGetProcAddress((const GLubyte*)"wglQueryCurrentContextNV")) == null) || r;

  return r;
}

#endif /* WGL_NV_present_video */

#ifdef WGL_NV_render_depth_texture

#endif /* WGL_NV_render_depth_texture */

#ifdef WGL_NV_render_texture_rectangle

#endif /* WGL_NV_render_texture_rectangle */

#ifdef WGL_NV_swap_group

static GLboolean _glewInit_WGL_NV_swap_group (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglBindSwapBarrierNV = (PFNWGLBINDSWAPBARRIERNVPROC)glewGetProcAddress((const GLubyte*)"wglBindSwapBarrierNV")) == null) || r;
  r = ((wglJoinSwapGroupNV = (PFNWGLJOINSWAPGROUPNVPROC)glewGetProcAddress((const GLubyte*)"wglJoinSwapGroupNV")) == null) || r;
  r = ((wglQueryFrameCountNV = (PFNWGLQUERYFRAMECOUNTNVPROC)glewGetProcAddress((const GLubyte*)"wglQueryFrameCountNV")) == null) || r;
  r = ((wglQueryMaxSwapGroupsNV = (PFNWGLQUERYMAXSWAPGROUPSNVPROC)glewGetProcAddress((const GLubyte*)"wglQueryMaxSwapGroupsNV")) == null) || r;
  r = ((wglQuerySwapGroupNV = (PFNWGLQUERYSWAPGROUPNVPROC)glewGetProcAddress((const GLubyte*)"wglQuerySwapGroupNV")) == null) || r;
  r = ((wglResetFrameCountNV = (PFNWGLRESETFRAMECOUNTNVPROC)glewGetProcAddress((const GLubyte*)"wglResetFrameCountNV")) == null) || r;

  return r;
}

#endif /* WGL_NV_swap_group */

#ifdef WGL_NV_vertex_array_range

static GLboolean _glewInit_WGL_NV_vertex_array_range (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglAllocateMemoryNV = (PFNWGLALLOCATEMEMORYNVPROC)glewGetProcAddress((const GLubyte*)"wglAllocateMemoryNV")) == null) || r;
  r = ((wglFreeMemoryNV = (PFNWGLFREEMEMORYNVPROC)glewGetProcAddress((const GLubyte*)"wglFreeMemoryNV")) == null) || r;

  return r;
}

#endif /* WGL_NV_vertex_array_range */

#ifdef WGL_NV_video_output

static GLboolean _glewInit_WGL_NV_video_output (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglBindVideoImageNV = (PFNWGLBINDVIDEOIMAGENVPROC)glewGetProcAddress((const GLubyte*)"wglBindVideoImageNV")) == null) || r;
  r = ((wglGetVideoDeviceNV = (PFNWGLGETVIDEODEVICENVPROC)glewGetProcAddress((const GLubyte*)"wglGetVideoDeviceNV")) == null) || r;
  r = ((wglGetVideoInfoNV = (PFNWGLGETVIDEOINFONVPROC)glewGetProcAddress((const GLubyte*)"wglGetVideoInfoNV")) == null) || r;
  r = ((wglReleaseVideoDeviceNV = (PFNWGLRELEASEVIDEODEVICENVPROC)glewGetProcAddress((const GLubyte*)"wglReleaseVideoDeviceNV")) == null) || r;
  r = ((wglReleaseVideoImageNV = (PFNWGLRELEASEVIDEOIMAGENVPROC)glewGetProcAddress((const GLubyte*)"wglReleaseVideoImageNV")) == null) || r;
  r = ((wglSendPbufferToVideoNV = (PFNWGLSENDPBUFFERTOVIDEONVPROC)glewGetProcAddress((const GLubyte*)"wglSendPbufferToVideoNV")) == null) || r;

  return r;
}

#endif /* WGL_NV_video_output */

#ifdef WGL_OML_sync_control

static GLboolean _glewInit_WGL_OML_sync_control (WGLEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((wglGetMscRateOML = (PFNWGLGETMSCRATEOMLPROC)glewGetProcAddress((const GLubyte*)"wglGetMscRateOML")) == null) || r;
  r = ((wglGetSyncValuesOML = (PFNWGLGETSYNCVALUESOMLPROC)glewGetProcAddress((const GLubyte*)"wglGetSyncValuesOML")) == null) || r;
  r = ((wglSwapBuffersMscOML = (PFNWGLSWAPBUFFERSMSCOMLPROC)glewGetProcAddress((const GLubyte*)"wglSwapBuffersMscOML")) == null) || r;
  r = ((wglSwapLayerBuffersMscOML = (PFNWGLSWAPLAYERBUFFERSMSCOMLPROC)glewGetProcAddress((const GLubyte*)"wglSwapLayerBuffersMscOML")) == null) || r;
  r = ((wglWaitForMscOML = (PFNWGLWAITFORMSCOMLPROC)glewGetProcAddress((const GLubyte*)"wglWaitForMscOML")) == null) || r;
  r = ((wglWaitForSbcOML = (PFNWGLWAITFORSBCOMLPROC)glewGetProcAddress((const GLubyte*)"wglWaitForSbcOML")) == null) || r;

  return r;
}

#endif /* WGL_OML_sync_control */

/* ------------------------------------------------------------------------- */

static PFNWGLGETEXTENSIONSSTRINGARBPROC _wglewGetExtensionsStringARB = null;
static PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglewGetExtensionsStringEXT = null;

GLboolean wglewGetExtension (const char* name)
{    
  GLubyte* p;
  GLubyte* end;
  GLuint len = _glewStrLen((const GLubyte*)name);
  if (_wglewGetExtensionsStringARB == null)
    if (_wglewGetExtensionsStringEXT == null)
      return GL_FALSE;
    else
      p = (GLubyte*)_wglewGetExtensionsStringEXT();
  else
    p = (GLubyte*)_wglewGetExtensionsStringARB(wglGetCurrentDC());
  if (0 == p) return GL_FALSE;
  end = p + _glewStrLen(p);
  while (p < end)
  {
    GLuint n = _glewStrCLen(p, ' ');
    if (len == n && _glewStrSame((const GLubyte*)name, p, n)) return GL_TRUE;
    p += n+1;
  }
  return GL_FALSE;
}

GLenum wglewContextInit (WGLEW_CONTEXT_ARG_DEF_LIST)
{
  GLboolean crippled;
  /* find wgl extension string query functions */
  _wglewGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)glewGetProcAddress((const GLubyte*)"wglGetExtensionsStringARB");
  _wglewGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)glewGetProcAddress((const GLubyte*)"wglGetExtensionsStringEXT");
  /* initialize extensions */
  crippled = _wglewGetExtensionsStringARB == null && _wglewGetExtensionsStringEXT == null;
#ifdef WGL_3DFX_multisample
  CONST_CAST(WGLEW_3DFX_multisample) = wglewGetExtension("WGL_3DFX_multisample");
#endif /* WGL_3DFX_multisample */
#ifdef WGL_3DL_stereo_control
  CONST_CAST(WGLEW_3DL_stereo_control) = wglewGetExtension("WGL_3DL_stereo_control");
  if (glewExperimental || WGLEW_3DL_stereo_control|| crippled) CONST_CAST(WGLEW_3DL_stereo_control)= !_glewInit_WGL_3DL_stereo_control(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_3DL_stereo_control */
#ifdef WGL_AMD_gpu_association
  CONST_CAST(WGLEW_AMD_gpu_association) = wglewGetExtension("WGL_AMD_gpu_association");
  if (glewExperimental || WGLEW_AMD_gpu_association|| crippled) CONST_CAST(WGLEW_AMD_gpu_association)= !_glewInit_WGL_AMD_gpu_association(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_AMD_gpu_association */
#ifdef WGL_ARB_buffer_region
  CONST_CAST(WGLEW_ARB_buffer_region) = wglewGetExtension("WGL_ARB_buffer_region");
  if (glewExperimental || WGLEW_ARB_buffer_region|| crippled) CONST_CAST(WGLEW_ARB_buffer_region)= !_glewInit_WGL_ARB_buffer_region(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_ARB_buffer_region */
#ifdef WGL_ARB_create_context
  CONST_CAST(WGLEW_ARB_create_context) = wglewGetExtension("WGL_ARB_create_context");
  if (glewExperimental || WGLEW_ARB_create_context|| crippled) CONST_CAST(WGLEW_ARB_create_context)= !_glewInit_WGL_ARB_create_context(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_ARB_create_context */
#ifdef WGL_ARB_create_context_profile
  CONST_CAST(WGLEW_ARB_create_context_profile) = wglewGetExtension("WGL_ARB_create_context_profile");
#endif /* WGL_ARB_create_context_profile */
#ifdef WGL_ARB_extensions_string
  CONST_CAST(WGLEW_ARB_extensions_string) = wglewGetExtension("WGL_ARB_extensions_string");
  if (glewExperimental || WGLEW_ARB_extensions_string|| crippled) CONST_CAST(WGLEW_ARB_extensions_string)= !_glewInit_WGL_ARB_extensions_string(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_ARB_extensions_string */
#ifdef WGL_ARB_framebuffer_sRGB
  CONST_CAST(WGLEW_ARB_framebuffer_sRGB) = wglewGetExtension("WGL_ARB_framebuffer_sRGB");
#endif /* WGL_ARB_framebuffer_sRGB */
#ifdef WGL_ARB_make_current_read
  CONST_CAST(WGLEW_ARB_make_current_read) = wglewGetExtension("WGL_ARB_make_current_read");
  if (glewExperimental || WGLEW_ARB_make_current_read|| crippled) CONST_CAST(WGLEW_ARB_make_current_read)= !_glewInit_WGL_ARB_make_current_read(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_ARB_make_current_read */
#ifdef WGL_ARB_multisample
  CONST_CAST(WGLEW_ARB_multisample) = wglewGetExtension("WGL_ARB_multisample");
#endif /* WGL_ARB_multisample */
#ifdef WGL_ARB_pbuffer
  CONST_CAST(WGLEW_ARB_pbuffer) = wglewGetExtension("WGL_ARB_pbuffer");
  if (glewExperimental || WGLEW_ARB_pbuffer|| crippled) CONST_CAST(WGLEW_ARB_pbuffer)= !_glewInit_WGL_ARB_pbuffer(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_ARB_pbuffer */
#ifdef WGL_ARB_pixel_format
  CONST_CAST(WGLEW_ARB_pixel_format) = wglewGetExtension("WGL_ARB_pixel_format");
  if (glewExperimental || WGLEW_ARB_pixel_format|| crippled) CONST_CAST(WGLEW_ARB_pixel_format)= !_glewInit_WGL_ARB_pixel_format(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_ARB_pixel_format */
#ifdef WGL_ARB_pixel_format_float
  CONST_CAST(WGLEW_ARB_pixel_format_float) = wglewGetExtension("WGL_ARB_pixel_format_float");
#endif /* WGL_ARB_pixel_format_float */
#ifdef WGL_ARB_render_texture
  CONST_CAST(WGLEW_ARB_render_texture) = wglewGetExtension("WGL_ARB_render_texture");
  if (glewExperimental || WGLEW_ARB_render_texture|| crippled) CONST_CAST(WGLEW_ARB_render_texture)= !_glewInit_WGL_ARB_render_texture(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_ARB_render_texture */
#ifdef WGL_ATI_pixel_format_float
  CONST_CAST(WGLEW_ATI_pixel_format_float) = wglewGetExtension("WGL_ATI_pixel_format_float");
#endif /* WGL_ATI_pixel_format_float */
#ifdef WGL_ATI_render_texture_rectangle
  CONST_CAST(WGLEW_ATI_render_texture_rectangle) = wglewGetExtension("WGL_ATI_render_texture_rectangle");
#endif /* WGL_ATI_render_texture_rectangle */
#ifdef WGL_EXT_depth_float
  CONST_CAST(WGLEW_EXT_depth_float) = wglewGetExtension("WGL_EXT_depth_float");
#endif /* WGL_EXT_depth_float */
#ifdef WGL_EXT_display_color_table
  CONST_CAST(WGLEW_EXT_display_color_table) = wglewGetExtension("WGL_EXT_display_color_table");
  if (glewExperimental || WGLEW_EXT_display_color_table|| crippled) CONST_CAST(WGLEW_EXT_display_color_table)= !_glewInit_WGL_EXT_display_color_table(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_EXT_display_color_table */
#ifdef WGL_EXT_extensions_string
  CONST_CAST(WGLEW_EXT_extensions_string) = wglewGetExtension("WGL_EXT_extensions_string");
  if (glewExperimental || WGLEW_EXT_extensions_string|| crippled) CONST_CAST(WGLEW_EXT_extensions_string)= !_glewInit_WGL_EXT_extensions_string(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_EXT_extensions_string */
#ifdef WGL_EXT_framebuffer_sRGB
  CONST_CAST(WGLEW_EXT_framebuffer_sRGB) = wglewGetExtension("WGL_EXT_framebuffer_sRGB");
#endif /* WGL_EXT_framebuffer_sRGB */
#ifdef WGL_EXT_make_current_read
  CONST_CAST(WGLEW_EXT_make_current_read) = wglewGetExtension("WGL_EXT_make_current_read");
  if (glewExperimental || WGLEW_EXT_make_current_read|| crippled) CONST_CAST(WGLEW_EXT_make_current_read)= !_glewInit_WGL_EXT_make_current_read(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_EXT_make_current_read */
#ifdef WGL_EXT_multisample
  CONST_CAST(WGLEW_EXT_multisample) = wglewGetExtension("WGL_EXT_multisample");
#endif /* WGL_EXT_multisample */
#ifdef WGL_EXT_pbuffer
  CONST_CAST(WGLEW_EXT_pbuffer) = wglewGetExtension("WGL_EXT_pbuffer");
  if (glewExperimental || WGLEW_EXT_pbuffer|| crippled) CONST_CAST(WGLEW_EXT_pbuffer)= !_glewInit_WGL_EXT_pbuffer(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_EXT_pbuffer */
#ifdef WGL_EXT_pixel_format
  CONST_CAST(WGLEW_EXT_pixel_format) = wglewGetExtension("WGL_EXT_pixel_format");
  if (glewExperimental || WGLEW_EXT_pixel_format|| crippled) CONST_CAST(WGLEW_EXT_pixel_format)= !_glewInit_WGL_EXT_pixel_format(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_EXT_pixel_format */
#ifdef WGL_EXT_pixel_format_packed_float
  CONST_CAST(WGLEW_EXT_pixel_format_packed_float) = wglewGetExtension("WGL_EXT_pixel_format_packed_float");
#endif /* WGL_EXT_pixel_format_packed_float */
#ifdef WGL_EXT_swap_control
  CONST_CAST(WGLEW_EXT_swap_control) = wglewGetExtension("WGL_EXT_swap_control");
  if (glewExperimental || WGLEW_EXT_swap_control|| crippled) CONST_CAST(WGLEW_EXT_swap_control)= !_glewInit_WGL_EXT_swap_control(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_EXT_swap_control */
#ifdef WGL_I3D_digital_video_control
  CONST_CAST(WGLEW_I3D_digital_video_control) = wglewGetExtension("WGL_I3D_digital_video_control");
  if (glewExperimental || WGLEW_I3D_digital_video_control|| crippled) CONST_CAST(WGLEW_I3D_digital_video_control)= !_glewInit_WGL_I3D_digital_video_control(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_I3D_digital_video_control */
#ifdef WGL_I3D_gamma
  CONST_CAST(WGLEW_I3D_gamma) = wglewGetExtension("WGL_I3D_gamma");
  if (glewExperimental || WGLEW_I3D_gamma|| crippled) CONST_CAST(WGLEW_I3D_gamma)= !_glewInit_WGL_I3D_gamma(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_I3D_gamma */
#ifdef WGL_I3D_genlock
  CONST_CAST(WGLEW_I3D_genlock) = wglewGetExtension("WGL_I3D_genlock");
  if (glewExperimental || WGLEW_I3D_genlock|| crippled) CONST_CAST(WGLEW_I3D_genlock)= !_glewInit_WGL_I3D_genlock(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_I3D_genlock */
#ifdef WGL_I3D_image_buffer
  CONST_CAST(WGLEW_I3D_image_buffer) = wglewGetExtension("WGL_I3D_image_buffer");
  if (glewExperimental || WGLEW_I3D_image_buffer|| crippled) CONST_CAST(WGLEW_I3D_image_buffer)= !_glewInit_WGL_I3D_image_buffer(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_I3D_image_buffer */
#ifdef WGL_I3D_swap_frame_lock
  CONST_CAST(WGLEW_I3D_swap_frame_lock) = wglewGetExtension("WGL_I3D_swap_frame_lock");
  if (glewExperimental || WGLEW_I3D_swap_frame_lock|| crippled) CONST_CAST(WGLEW_I3D_swap_frame_lock)= !_glewInit_WGL_I3D_swap_frame_lock(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_I3D_swap_frame_lock */
#ifdef WGL_I3D_swap_frame_usage
  CONST_CAST(WGLEW_I3D_swap_frame_usage) = wglewGetExtension("WGL_I3D_swap_frame_usage");
  if (glewExperimental || WGLEW_I3D_swap_frame_usage|| crippled) CONST_CAST(WGLEW_I3D_swap_frame_usage)= !_glewInit_WGL_I3D_swap_frame_usage(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_I3D_swap_frame_usage */
#ifdef WGL_NV_copy_image
  CONST_CAST(WGLEW_NV_copy_image) = wglewGetExtension("WGL_NV_copy_image");
  if (glewExperimental || WGLEW_NV_copy_image|| crippled) CONST_CAST(WGLEW_NV_copy_image)= !_glewInit_WGL_NV_copy_image(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_NV_copy_image */
#ifdef WGL_NV_float_buffer
  CONST_CAST(WGLEW_NV_float_buffer) = wglewGetExtension("WGL_NV_float_buffer");
#endif /* WGL_NV_float_buffer */
#ifdef WGL_NV_gpu_affinity
  CONST_CAST(WGLEW_NV_gpu_affinity) = wglewGetExtension("WGL_NV_gpu_affinity");
  if (glewExperimental || WGLEW_NV_gpu_affinity|| crippled) CONST_CAST(WGLEW_NV_gpu_affinity)= !_glewInit_WGL_NV_gpu_affinity(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_NV_gpu_affinity */
#ifdef WGL_NV_present_video
  CONST_CAST(WGLEW_NV_present_video) = wglewGetExtension("WGL_NV_present_video");
  if (glewExperimental || WGLEW_NV_present_video|| crippled) CONST_CAST(WGLEW_NV_present_video)= !_glewInit_WGL_NV_present_video(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_NV_present_video */
#ifdef WGL_NV_render_depth_texture
  CONST_CAST(WGLEW_NV_render_depth_texture) = wglewGetExtension("WGL_NV_render_depth_texture");
#endif /* WGL_NV_render_depth_texture */
#ifdef WGL_NV_render_texture_rectangle
  CONST_CAST(WGLEW_NV_render_texture_rectangle) = wglewGetExtension("WGL_NV_render_texture_rectangle");
#endif /* WGL_NV_render_texture_rectangle */
#ifdef WGL_NV_swap_group
  CONST_CAST(WGLEW_NV_swap_group) = wglewGetExtension("WGL_NV_swap_group");
  if (glewExperimental || WGLEW_NV_swap_group|| crippled) CONST_CAST(WGLEW_NV_swap_group)= !_glewInit_WGL_NV_swap_group(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_NV_swap_group */
#ifdef WGL_NV_vertex_array_range
  CONST_CAST(WGLEW_NV_vertex_array_range) = wglewGetExtension("WGL_NV_vertex_array_range");
  if (glewExperimental || WGLEW_NV_vertex_array_range|| crippled) CONST_CAST(WGLEW_NV_vertex_array_range)= !_glewInit_WGL_NV_vertex_array_range(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_NV_vertex_array_range */
#ifdef WGL_NV_video_output
  CONST_CAST(WGLEW_NV_video_output) = wglewGetExtension("WGL_NV_video_output");
  if (glewExperimental || WGLEW_NV_video_output|| crippled) CONST_CAST(WGLEW_NV_video_output)= !_glewInit_WGL_NV_video_output(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_NV_video_output */
#ifdef WGL_OML_sync_control
  CONST_CAST(WGLEW_OML_sync_control) = wglewGetExtension("WGL_OML_sync_control");
  if (glewExperimental || WGLEW_OML_sync_control|| crippled) CONST_CAST(WGLEW_OML_sync_control)= !_glewInit_WGL_OML_sync_control(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* WGL_OML_sync_control */

  return GLEW_OK;
}

#elif !defined(__APPLE__) || defined(GLEW_APPLE_GLX)

PFNGLXGETCURRENTDISPLAYPROC __glewXGetCurrentDisplay = null;

PFNGLXCHOOSEFBCONFIGPROC __glewXChooseFBConfig = null;
PFNGLXCREATENEWCONTEXTPROC __glewXCreateNewContext = null;
PFNGLXCREATEPBUFFERPROC __glewXCreatePbuffer = null;
PFNGLXCREATEPIXMAPPROC __glewXCreatePixmap = null;
PFNGLXCREATEWINDOWPROC __glewXCreateWindow = null;
PFNGLXDESTROYPBUFFERPROC __glewXDestroyPbuffer = null;
PFNGLXDESTROYPIXMAPPROC __glewXDestroyPixmap = null;
PFNGLXDESTROYWINDOWPROC __glewXDestroyWindow = null;
PFNGLXGETCURRENTREADDRAWABLEPROC __glewXGetCurrentReadDrawable = null;
PFNGLXGETFBCONFIGATTRIBPROC __glewXGetFBConfigAttrib = null;
PFNGLXGETFBCONFIGSPROC __glewXGetFBConfigs = null;
PFNGLXGETSELECTEDEVENTPROC __glewXGetSelectedEvent = null;
PFNGLXGETVISUALFROMFBCONFIGPROC __glewXGetVisualFromFBConfig = null;
PFNGLXMAKECONTEXTCURRENTPROC __glewXMakeContextCurrent = null;
PFNGLXQUERYCONTEXTPROC __glewXQueryContext = null;
PFNGLXQUERYDRAWABLEPROC __glewXQueryDrawable = null;
PFNGLXSELECTEVENTPROC __glewXSelectEvent = null;

PFNGLXCREATECONTEXTATTRIBSARBPROC __glewXCreateContextAttribsARB = null;

PFNGLXBINDTEXIMAGEATIPROC __glewXBindTexImageATI = null;
PFNGLXDRAWABLEATTRIBATIPROC __glewXDrawableAttribATI = null;
PFNGLXRELEASETEXIMAGEATIPROC __glewXReleaseTexImageATI = null;

PFNGLXFREECONTEXTEXTPROC __glewXFreeContextEXT = null;
PFNGLXGETCONTEXTIDEXTPROC __glewXGetContextIDEXT = null;
PFNGLXIMPORTCONTEXTEXTPROC __glewXImportContextEXT = null;
PFNGLXQUERYCONTEXTINFOEXTPROC __glewXQueryContextInfoEXT = null;

#ifdef _WIN32
PFNGLXSWAPINTERVALEXTPROC __glewXSwapIntervalEXT = null;
#endif

PFNGLXBINDTEXIMAGEEXTPROC __glewXBindTexImageEXT = null;
PFNGLXRELEASETEXIMAGEEXTPROC __glewXReleaseTexImageEXT = null;

PFNGLXGETAGPOFFSETMESAPROC __glewXGetAGPOffsetMESA = null;

PFNGLXCOPYSUBBUFFERMESAPROC __glewXCopySubBufferMESA = null;

PFNGLXCREATEGLXPIXMAPMESAPROC __glewXCreateGLXPixmapMESA = null;

PFNGLXRELEASEBUFFERSMESAPROC __glewXReleaseBuffersMESA = null;

PFNGLXSET3DFXMODEMESAPROC __glewXSet3DfxModeMESA = null;

#ifdef _WIN32
PFNGLXCOPYIMAGESUBDATANVPROC __glewXCopyImageSubDataNV = null;
#endif

PFNGLXBINDVIDEODEVICENVPROC __glewXBindVideoDeviceNV = null;
PFNGLXENUMERATEVIDEODEVICESNVPROC __glewXEnumerateVideoDevicesNV = null;

PFNGLXBINDSWAPBARRIERNVPROC __glewXBindSwapBarrierNV = null;
PFNGLXJOINSWAPGROUPNVPROC __glewXJoinSwapGroupNV = null;
PFNGLXQUERYFRAMECOUNTNVPROC __glewXQueryFrameCountNV = null;
PFNGLXQUERYMAXSWAPGROUPSNVPROC __glewXQueryMaxSwapGroupsNV = null;
PFNGLXQUERYSWAPGROUPNVPROC __glewXQuerySwapGroupNV = null;
PFNGLXRESETFRAMECOUNTNVPROC __glewXResetFrameCountNV = null;

PFNGLXALLOCATEMEMORYNVPROC __glewXAllocateMemoryNV = null;
PFNGLXFREEMEMORYNVPROC __glewXFreeMemoryNV = null;

PFNGLXBINDVIDEOIMAGENVPROC __glewXBindVideoImageNV = null;
PFNGLXGETVIDEODEVICENVPROC __glewXGetVideoDeviceNV = null;
PFNGLXGETVIDEOINFONVPROC __glewXGetVideoInfoNV = null;
PFNGLXRELEASEVIDEODEVICENVPROC __glewXReleaseVideoDeviceNV = null;
PFNGLXRELEASEVIDEOIMAGENVPROC __glewXReleaseVideoImageNV = null;
PFNGLXSENDPBUFFERTOVIDEONVPROC __glewXSendPbufferToVideoNV = null;

#ifdef GLX_OML_sync_control
PFNGLXGETMSCRATEOMLPROC __glewXGetMscRateOML = null;
PFNGLXGETSYNCVALUESOMLPROC __glewXGetSyncValuesOML = null;
PFNGLXSWAPBUFFERSMSCOMLPROC __glewXSwapBuffersMscOML = null;
PFNGLXWAITFORMSCOMLPROC __glewXWaitForMscOML = null;
PFNGLXWAITFORSBCOMLPROC __glewXWaitForSbcOML = null;
#endif

PFNGLXCHOOSEFBCONFIGSGIXPROC __glewXChooseFBConfigSGIX = null;
PFNGLXCREATECONTEXTWITHCONFIGSGIXPROC __glewXCreateContextWithConfigSGIX = null;
PFNGLXCREATEGLXPIXMAPWITHCONFIGSGIXPROC __glewXCreateGLXPixmapWithConfigSGIX = null;
PFNGLXGETFBCONFIGATTRIBSGIXPROC __glewXGetFBConfigAttribSGIX = null;
PFNGLXGETFBCONFIGFROMVISUALSGIXPROC __glewXGetFBConfigFromVisualSGIX = null;
PFNGLXGETVISUALFROMFBCONFIGSGIXPROC __glewXGetVisualFromFBConfigSGIX = null;

PFNGLXBINDHYPERPIPESGIXPROC __glewXBindHyperpipeSGIX = null;
PFNGLXDESTROYHYPERPIPECONFIGSGIXPROC __glewXDestroyHyperpipeConfigSGIX = null;
PFNGLXHYPERPIPEATTRIBSGIXPROC __glewXHyperpipeAttribSGIX = null;
PFNGLXHYPERPIPECONFIGSGIXPROC __glewXHyperpipeConfigSGIX = null;
PFNGLXQUERYHYPERPIPEATTRIBSGIXPROC __glewXQueryHyperpipeAttribSGIX = null;
PFNGLXQUERYHYPERPIPEBESTATTRIBSGIXPROC __glewXQueryHyperpipeBestAttribSGIX = null;
PFNGLXQUERYHYPERPIPECONFIGSGIXPROC __glewXQueryHyperpipeConfigSGIX = null;
PFNGLXQUERYHYPERPIPENETWORKSGIXPROC __glewXQueryHyperpipeNetworkSGIX = null;

PFNGLXCREATEGLXPBUFFERSGIXPROC __glewXCreateGLXPbufferSGIX = null;
PFNGLXDESTROYGLXPBUFFERSGIXPROC __glewXDestroyGLXPbufferSGIX = null;
PFNGLXGETSELECTEDEVENTSGIXPROC __glewXGetSelectedEventSGIX = null;
PFNGLXQUERYGLXPBUFFERSGIXPROC __glewXQueryGLXPbufferSGIX = null;
PFNGLXSELECTEVENTSGIXPROC __glewXSelectEventSGIX = null;

PFNGLXBINDSWAPBARRIERSGIXPROC __glewXBindSwapBarrierSGIX = null;
PFNGLXQUERYMAXSWAPBARRIERSSGIXPROC __glewXQueryMaxSwapBarriersSGIX = null;

PFNGLXJOINSWAPGROUPSGIXPROC __glewXJoinSwapGroupSGIX = null;

PFNGLXBINDCHANNELTOWINDOWSGIXPROC __glewXBindChannelToWindowSGIX = null;
PFNGLXCHANNELRECTSGIXPROC __glewXChannelRectSGIX = null;
PFNGLXCHANNELRECTSYNCSGIXPROC __glewXChannelRectSyncSGIX = null;
PFNGLXQUERYCHANNELDELTASSGIXPROC __glewXQueryChannelDeltasSGIX = null;
PFNGLXQUERYCHANNELRECTSGIXPROC __glewXQueryChannelRectSGIX = null;

PFNGLXCUSHIONSGIPROC __glewXCushionSGI = null;

PFNGLXGETCURRENTREADDRAWABLESGIPROC __glewXGetCurrentReadDrawableSGI = null;
PFNGLXMAKECURRENTREADSGIPROC __glewXMakeCurrentReadSGI = null;

PFNGLXSWAPINTERVALSGIPROC __glewXSwapIntervalSGI = null;

PFNGLXGETVIDEOSYNCSGIPROC __glewXGetVideoSyncSGI = null;
PFNGLXWAITVIDEOSYNCSGIPROC __glewXWaitVideoSyncSGI = null;

PFNGLXGETTRANSPARENTINDEXSUNPROC __glewXGetTransparentIndexSUN = null;

PFNGLXGETVIDEORESIZESUNPROC __glewXGetVideoResizeSUN = null;
PFNGLXVIDEORESIZESUNPROC __glewXVideoResizeSUN = null;

#if !defined(GLEW_MX)

GLboolean __GLXEW_VERSION_1_0 = GL_FALSE;
GLboolean __GLXEW_VERSION_1_1 = GL_FALSE;
GLboolean __GLXEW_VERSION_1_2 = GL_FALSE;
GLboolean __GLXEW_VERSION_1_3 = GL_FALSE;
GLboolean __GLXEW_VERSION_1_4 = GL_FALSE;
GLboolean __GLXEW_3DFX_multisample = GL_FALSE;
GLboolean __GLXEW_ARB_create_context = GL_FALSE;
GLboolean __GLXEW_ARB_create_context_profile = GL_FALSE;
GLboolean __GLXEW_ARB_fbconfig_float = GL_FALSE;
GLboolean __GLXEW_ARB_framebuffer_sRGB = GL_FALSE;
GLboolean __GLXEW_ARB_get_proc_address = GL_FALSE;
GLboolean __GLXEW_ARB_multisample = GL_FALSE;
GLboolean __GLXEW_ATI_pixel_format_float = GL_FALSE;
GLboolean __GLXEW_ATI_render_texture = GL_FALSE;
GLboolean __GLXEW_EXT_fbconfig_packed_float = GL_FALSE;
GLboolean __GLXEW_EXT_framebuffer_sRGB = GL_FALSE;
GLboolean __GLXEW_EXT_import_context = GL_FALSE;
GLboolean __GLXEW_EXT_scene_marker = GL_FALSE;
GLboolean __GLXEW_EXT_swap_control = GL_FALSE;
GLboolean __GLXEW_EXT_texture_from_pixmap = GL_FALSE;
GLboolean __GLXEW_EXT_visual_info = GL_FALSE;
GLboolean __GLXEW_EXT_visual_rating = GL_FALSE;
GLboolean __GLXEW_INTEL_swap_event = GL_FALSE;
GLboolean __GLXEW_MESA_agp_offset = GL_FALSE;
GLboolean __GLXEW_MESA_copy_sub_buffer = GL_FALSE;
GLboolean __GLXEW_MESA_pixmap_colormap = GL_FALSE;
GLboolean __GLXEW_MESA_release_buffers = GL_FALSE;
GLboolean __GLXEW_MESA_set_3dfx_mode = GL_FALSE;
GLboolean __GLXEW_NV_copy_image = GL_FALSE;
GLboolean __GLXEW_NV_float_buffer = GL_FALSE;
GLboolean __GLXEW_NV_present_video = GL_FALSE;
GLboolean __GLXEW_NV_swap_group = GL_FALSE;
GLboolean __GLXEW_NV_vertex_array_range = GL_FALSE;
GLboolean __GLXEW_NV_video_output = GL_FALSE;
GLboolean __GLXEW_OML_swap_method = GL_FALSE;
#ifdef GLX_OML_sync_control
GLboolean __GLXEW_OML_sync_control = GL_FALSE;
#endif
GLboolean __GLXEW_SGIS_blended_overlay = GL_FALSE;
GLboolean __GLXEW_SGIS_color_range = GL_FALSE;
GLboolean __GLXEW_SGIS_multisample = GL_FALSE;
GLboolean __GLXEW_SGIS_shared_multisample = GL_FALSE;
GLboolean __GLXEW_SGIX_fbconfig = GL_FALSE;
GLboolean __GLXEW_SGIX_hyperpipe = GL_FALSE;
GLboolean __GLXEW_SGIX_pbuffer = GL_FALSE;
GLboolean __GLXEW_SGIX_swap_barrier = GL_FALSE;
GLboolean __GLXEW_SGIX_swap_group = GL_FALSE;
GLboolean __GLXEW_SGIX_video_resize = GL_FALSE;
GLboolean __GLXEW_SGIX_visual_select_group = GL_FALSE;
GLboolean __GLXEW_SGI_cushion = GL_FALSE;
GLboolean __GLXEW_SGI_make_current_read = GL_FALSE;
GLboolean __GLXEW_SGI_swap_control = GL_FALSE;
GLboolean __GLXEW_SGI_video_sync = GL_FALSE;
GLboolean __GLXEW_SUN_get_transparent_index = GL_FALSE;
GLboolean __GLXEW_SUN_video_resize = GL_FALSE;

#endif /* !GLEW_MX */

#ifdef GLX_VERSION_1_2

static GLboolean _glewInit_GLX_VERSION_1_2 (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXGetCurrentDisplay = (PFNGLXGETCURRENTDISPLAYPROC)glewGetProcAddress((const GLubyte*)"glXGetCurrentDisplay")) == null) || r;

  return r;
}

#endif /* GLX_VERSION_1_2 */

#ifdef GLX_VERSION_1_3

static GLboolean _glewInit_GLX_VERSION_1_3 (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXChooseFBConfig = (PFNGLXCHOOSEFBCONFIGPROC)glewGetProcAddress((const GLubyte*)"glXChooseFBConfig")) == null) || r;
  r = ((glXCreateNewContext = (PFNGLXCREATENEWCONTEXTPROC)glewGetProcAddress((const GLubyte*)"glXCreateNewContext")) == null) || r;
  r = ((glXCreatePbuffer = (PFNGLXCREATEPBUFFERPROC)glewGetProcAddress((const GLubyte*)"glXCreatePbuffer")) == null) || r;
  r = ((glXCreatePixmap = (PFNGLXCREATEPIXMAPPROC)glewGetProcAddress((const GLubyte*)"glXCreatePixmap")) == null) || r;
  r = ((glXCreateWindow = (PFNGLXCREATEWINDOWPROC)glewGetProcAddress((const GLubyte*)"glXCreateWindow")) == null) || r;
  r = ((glXDestroyPbuffer = (PFNGLXDESTROYPBUFFERPROC)glewGetProcAddress((const GLubyte*)"glXDestroyPbuffer")) == null) || r;
  r = ((glXDestroyPixmap = (PFNGLXDESTROYPIXMAPPROC)glewGetProcAddress((const GLubyte*)"glXDestroyPixmap")) == null) || r;
  r = ((glXDestroyWindow = (PFNGLXDESTROYWINDOWPROC)glewGetProcAddress((const GLubyte*)"glXDestroyWindow")) == null) || r;
  r = ((glXGetCurrentReadDrawable = (PFNGLXGETCURRENTREADDRAWABLEPROC)glewGetProcAddress((const GLubyte*)"glXGetCurrentReadDrawable")) == null) || r;
  r = ((glXGetFBConfigAttrib = (PFNGLXGETFBCONFIGATTRIBPROC)glewGetProcAddress((const GLubyte*)"glXGetFBConfigAttrib")) == null) || r;
  r = ((glXGetFBConfigs = (PFNGLXGETFBCONFIGSPROC)glewGetProcAddress((const GLubyte*)"glXGetFBConfigs")) == null) || r;
  r = ((glXGetSelectedEvent = (PFNGLXGETSELECTEDEVENTPROC)glewGetProcAddress((const GLubyte*)"glXGetSelectedEvent")) == null) || r;
  r = ((glXGetVisualFromFBConfig = (PFNGLXGETVISUALFROMFBCONFIGPROC)glewGetProcAddress((const GLubyte*)"glXGetVisualFromFBConfig")) == null) || r;
  r = ((glXMakeContextCurrent = (PFNGLXMAKECONTEXTCURRENTPROC)glewGetProcAddress((const GLubyte*)"glXMakeContextCurrent")) == null) || r;
  r = ((glXQueryContext = (PFNGLXQUERYCONTEXTPROC)glewGetProcAddress((const GLubyte*)"glXQueryContext")) == null) || r;
  r = ((glXQueryDrawable = (PFNGLXQUERYDRAWABLEPROC)glewGetProcAddress((const GLubyte*)"glXQueryDrawable")) == null) || r;
  r = ((glXSelectEvent = (PFNGLXSELECTEVENTPROC)glewGetProcAddress((const GLubyte*)"glXSelectEvent")) == null) || r;

  return r;
}

#endif /* GLX_VERSION_1_3 */

#ifdef GLX_VERSION_1_4

#endif /* GLX_VERSION_1_4 */

#ifdef GLX_3DFX_multisample

#endif /* GLX_3DFX_multisample */

#ifdef GLX_ARB_create_context

static GLboolean _glewInit_GLX_ARB_create_context (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)glewGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB")) == null) || r;

  return r;
}

#endif /* GLX_ARB_create_context */

#ifdef GLX_ARB_create_context_profile

#endif /* GLX_ARB_create_context_profile */

#ifdef GLX_ARB_fbconfig_float

#endif /* GLX_ARB_fbconfig_float */

#ifdef GLX_ARB_framebuffer_sRGB

#endif /* GLX_ARB_framebuffer_sRGB */

#ifdef GLX_ARB_get_proc_address

#endif /* GLX_ARB_get_proc_address */

#ifdef GLX_ARB_multisample

#endif /* GLX_ARB_multisample */

#ifdef GLX_ATI_pixel_format_float

#endif /* GLX_ATI_pixel_format_float */

#ifdef GLX_ATI_render_texture

static GLboolean _glewInit_GLX_ATI_render_texture (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXBindTexImageATI = (PFNGLXBINDTEXIMAGEATIPROC)glewGetProcAddress((const GLubyte*)"glXBindTexImageATI")) == null) || r;
  r = ((glXDrawableAttribATI = (PFNGLXDRAWABLEATTRIBATIPROC)glewGetProcAddress((const GLubyte*)"glXDrawableAttribATI")) == null) || r;
  r = ((glXReleaseTexImageATI = (PFNGLXRELEASETEXIMAGEATIPROC)glewGetProcAddress((const GLubyte*)"glXReleaseTexImageATI")) == null) || r;

  return r;
}

#endif /* GLX_ATI_render_texture */

#ifdef GLX_EXT_fbconfig_packed_float

#endif /* GLX_EXT_fbconfig_packed_float */

#ifdef GLX_EXT_framebuffer_sRGB

#endif /* GLX_EXT_framebuffer_sRGB */

#ifdef GLX_EXT_import_context

static GLboolean _glewInit_GLX_EXT_import_context (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXFreeContextEXT = (PFNGLXFREECONTEXTEXTPROC)glewGetProcAddress((const GLubyte*)"glXFreeContextEXT")) == null) || r;
  r = ((glXGetContextIDEXT = (PFNGLXGETCONTEXTIDEXTPROC)glewGetProcAddress((const GLubyte*)"glXGetContextIDEXT")) == null) || r;
  r = ((glXImportContextEXT = (PFNGLXIMPORTCONTEXTEXTPROC)glewGetProcAddress((const GLubyte*)"glXImportContextEXT")) == null) || r;
  r = ((glXQueryContextInfoEXT = (PFNGLXQUERYCONTEXTINFOEXTPROC)glewGetProcAddress((const GLubyte*)"glXQueryContextInfoEXT")) == null) || r;

  return r;
}

#endif /* GLX_EXT_import_context */

#ifdef GLX_EXT_scene_marker

#endif /* GLX_EXT_scene_marker */

#ifdef GLX_EXT_swap_control

static GLboolean _glewInit_GLX_EXT_swap_control (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC)glewGetProcAddress((const GLubyte*)"glXSwapIntervalEXT")) == null) || r;

  return r;
}

#endif /* GLX_EXT_swap_control */

#ifdef GLX_EXT_texture_from_pixmap

static GLboolean _glewInit_GLX_EXT_texture_from_pixmap (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXBindTexImageEXT = (PFNGLXBINDTEXIMAGEEXTPROC)glewGetProcAddress((const GLubyte*)"glXBindTexImageEXT")) == null) || r;
  r = ((glXReleaseTexImageEXT = (PFNGLXRELEASETEXIMAGEEXTPROC)glewGetProcAddress((const GLubyte*)"glXReleaseTexImageEXT")) == null) || r;

  return r;
}

#endif /* GLX_EXT_texture_from_pixmap */

#ifdef GLX_EXT_visual_info

#endif /* GLX_EXT_visual_info */

#ifdef GLX_EXT_visual_rating

#endif /* GLX_EXT_visual_rating */

#ifdef GLX_INTEL_swap_event

#endif /* GLX_INTEL_swap_event */

#ifdef GLX_MESA_agp_offset

static GLboolean _glewInit_GLX_MESA_agp_offset (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXGetAGPOffsetMESA = (PFNGLXGETAGPOFFSETMESAPROC)glewGetProcAddress((const GLubyte*)"glXGetAGPOffsetMESA")) == null) || r;

  return r;
}

#endif /* GLX_MESA_agp_offset */

#ifdef GLX_MESA_copy_sub_buffer

static GLboolean _glewInit_GLX_MESA_copy_sub_buffer (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXCopySubBufferMESA = (PFNGLXCOPYSUBBUFFERMESAPROC)glewGetProcAddress((const GLubyte*)"glXCopySubBufferMESA")) == null) || r;

  return r;
}

#endif /* GLX_MESA_copy_sub_buffer */

#ifdef GLX_MESA_pixmap_colormap

static GLboolean _glewInit_GLX_MESA_pixmap_colormap (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXCreateGLXPixmapMESA = (PFNGLXCREATEGLXPIXMAPMESAPROC)glewGetProcAddress((const GLubyte*)"glXCreateGLXPixmapMESA")) == null) || r;

  return r;
}

#endif /* GLX_MESA_pixmap_colormap */

#ifdef GLX_MESA_release_buffers

static GLboolean _glewInit_GLX_MESA_release_buffers (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXReleaseBuffersMESA = (PFNGLXRELEASEBUFFERSMESAPROC)glewGetProcAddress((const GLubyte*)"glXReleaseBuffersMESA")) == null) || r;

  return r;
}

#endif /* GLX_MESA_release_buffers */

#ifdef GLX_MESA_set_3dfx_mode

static GLboolean _glewInit_GLX_MESA_set_3dfx_mode (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXSet3DfxModeMESA = (PFNGLXSET3DFXMODEMESAPROC)glewGetProcAddress((const GLubyte*)"glXSet3DfxModeMESA")) == null) || r;

  return r;
}

#endif /* GLX_MESA_set_3dfx_mode */

#ifdef GLX_NV_copy_image

static GLboolean _glewInit_GLX_NV_copy_image (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXCopyImageSubDataNV = (PFNGLXCOPYIMAGESUBDATANVPROC)glewGetProcAddress((const GLubyte*)"glXCopyImageSubDataNV")) == null) || r;

  return r;
}

#endif /* GLX_NV_copy_image */

#ifdef GLX_NV_float_buffer

#endif /* GLX_NV_float_buffer */

#ifdef GLX_NV_present_video

static GLboolean _glewInit_GLX_NV_present_video (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXBindVideoDeviceNV = (PFNGLXBINDVIDEODEVICENVPROC)glewGetProcAddress((const GLubyte*)"glXBindVideoDeviceNV")) == null) || r;
  r = ((glXEnumerateVideoDevicesNV = (PFNGLXENUMERATEVIDEODEVICESNVPROC)glewGetProcAddress((const GLubyte*)"glXEnumerateVideoDevicesNV")) == null) || r;

  return r;
}

#endif /* GLX_NV_present_video */

#ifdef GLX_NV_swap_group

static GLboolean _glewInit_GLX_NV_swap_group (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXBindSwapBarrierNV = (PFNGLXBINDSWAPBARRIERNVPROC)glewGetProcAddress((const GLubyte*)"glXBindSwapBarrierNV")) == null) || r;
  r = ((glXJoinSwapGroupNV = (PFNGLXJOINSWAPGROUPNVPROC)glewGetProcAddress((const GLubyte*)"glXJoinSwapGroupNV")) == null) || r;
  r = ((glXQueryFrameCountNV = (PFNGLXQUERYFRAMECOUNTNVPROC)glewGetProcAddress((const GLubyte*)"glXQueryFrameCountNV")) == null) || r;
  r = ((glXQueryMaxSwapGroupsNV = (PFNGLXQUERYMAXSWAPGROUPSNVPROC)glewGetProcAddress((const GLubyte*)"glXQueryMaxSwapGroupsNV")) == null) || r;
  r = ((glXQuerySwapGroupNV = (PFNGLXQUERYSWAPGROUPNVPROC)glewGetProcAddress((const GLubyte*)"glXQuerySwapGroupNV")) == null) || r;
  r = ((glXResetFrameCountNV = (PFNGLXRESETFRAMECOUNTNVPROC)glewGetProcAddress((const GLubyte*)"glXResetFrameCountNV")) == null) || r;

  return r;
}

#endif /* GLX_NV_swap_group */

#ifdef GLX_NV_vertex_array_range

static GLboolean _glewInit_GLX_NV_vertex_array_range (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXAllocateMemoryNV = (PFNGLXALLOCATEMEMORYNVPROC)glewGetProcAddress((const GLubyte*)"glXAllocateMemoryNV")) == null) || r;
  r = ((glXFreeMemoryNV = (PFNGLXFREEMEMORYNVPROC)glewGetProcAddress((const GLubyte*)"glXFreeMemoryNV")) == null) || r;

  return r;
}

#endif /* GLX_NV_vertex_array_range */

#ifdef GLX_NV_video_output

static GLboolean _glewInit_GLX_NV_video_output (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXBindVideoImageNV = (PFNGLXBINDVIDEOIMAGENVPROC)glewGetProcAddress((const GLubyte*)"glXBindVideoImageNV")) == null) || r;
  r = ((glXGetVideoDeviceNV = (PFNGLXGETVIDEODEVICENVPROC)glewGetProcAddress((const GLubyte*)"glXGetVideoDeviceNV")) == null) || r;
  r = ((glXGetVideoInfoNV = (PFNGLXGETVIDEOINFONVPROC)glewGetProcAddress((const GLubyte*)"glXGetVideoInfoNV")) == null) || r;
  r = ((glXReleaseVideoDeviceNV = (PFNGLXRELEASEVIDEODEVICENVPROC)glewGetProcAddress((const GLubyte*)"glXReleaseVideoDeviceNV")) == null) || r;
  r = ((glXReleaseVideoImageNV = (PFNGLXRELEASEVIDEOIMAGENVPROC)glewGetProcAddress((const GLubyte*)"glXReleaseVideoImageNV")) == null) || r;
  r = ((glXSendPbufferToVideoNV = (PFNGLXSENDPBUFFERTOVIDEONVPROC)glewGetProcAddress((const GLubyte*)"glXSendPbufferToVideoNV")) == null) || r;

  return r;
}

#endif /* GLX_NV_video_output */

#ifdef GLX_OML_swap_method

#endif /* GLX_OML_swap_method */

#if defined(GLX_OML_sync_control) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#include <inttypes.h>

static GLboolean _glewInit_GLX_OML_sync_control (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXGetMscRateOML = (PFNGLXGETMSCRATEOMLPROC)glewGetProcAddress((const GLubyte*)"glXGetMscRateOML")) == null) || r;
  r = ((glXGetSyncValuesOML = (PFNGLXGETSYNCVALUESOMLPROC)glewGetProcAddress((const GLubyte*)"glXGetSyncValuesOML")) == null) || r;
  r = ((glXSwapBuffersMscOML = (PFNGLXSWAPBUFFERSMSCOMLPROC)glewGetProcAddress((const GLubyte*)"glXSwapBuffersMscOML")) == null) || r;
  r = ((glXWaitForMscOML = (PFNGLXWAITFORMSCOMLPROC)glewGetProcAddress((const GLubyte*)"glXWaitForMscOML")) == null) || r;
  r = ((glXWaitForSbcOML = (PFNGLXWAITFORSBCOMLPROC)glewGetProcAddress((const GLubyte*)"glXWaitForSbcOML")) == null) || r;

  return r;
}

#endif /* GLX_OML_sync_control */

#ifdef GLX_SGIS_blended_overlay

#endif /* GLX_SGIS_blended_overlay */

#ifdef GLX_SGIS_color_range

#endif /* GLX_SGIS_color_range */

#ifdef GLX_SGIS_multisample

#endif /* GLX_SGIS_multisample */

#ifdef GLX_SGIS_shared_multisample

#endif /* GLX_SGIS_shared_multisample */

#ifdef GLX_SGIX_fbconfig

static GLboolean _glewInit_GLX_SGIX_fbconfig (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXChooseFBConfigSGIX = (PFNGLXCHOOSEFBCONFIGSGIXPROC)glewGetProcAddress((const GLubyte*)"glXChooseFBConfigSGIX")) == null) || r;
  r = ((glXCreateContextWithConfigSGIX = (PFNGLXCREATECONTEXTWITHCONFIGSGIXPROC)glewGetProcAddress((const GLubyte*)"glXCreateContextWithConfigSGIX")) == null) || r;
  r = ((glXCreateGLXPixmapWithConfigSGIX = (PFNGLXCREATEGLXPIXMAPWITHCONFIGSGIXPROC)glewGetProcAddress((const GLubyte*)"glXCreateGLXPixmapWithConfigSGIX")) == null) || r;
  r = ((glXGetFBConfigAttribSGIX = (PFNGLXGETFBCONFIGATTRIBSGIXPROC)glewGetProcAddress((const GLubyte*)"glXGetFBConfigAttribSGIX")) == null) || r;
  r = ((glXGetFBConfigFromVisualSGIX = (PFNGLXGETFBCONFIGFROMVISUALSGIXPROC)glewGetProcAddress((const GLubyte*)"glXGetFBConfigFromVisualSGIX")) == null) || r;
  r = ((glXGetVisualFromFBConfigSGIX = (PFNGLXGETVISUALFROMFBCONFIGSGIXPROC)glewGetProcAddress((const GLubyte*)"glXGetVisualFromFBConfigSGIX")) == null) || r;

  return r;
}

#endif /* GLX_SGIX_fbconfig */

#ifdef GLX_SGIX_hyperpipe

static GLboolean _glewInit_GLX_SGIX_hyperpipe (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXBindHyperpipeSGIX = (PFNGLXBINDHYPERPIPESGIXPROC)glewGetProcAddress((const GLubyte*)"glXBindHyperpipeSGIX")) == null) || r;
  r = ((glXDestroyHyperpipeConfigSGIX = (PFNGLXDESTROYHYPERPIPECONFIGSGIXPROC)glewGetProcAddress((const GLubyte*)"glXDestroyHyperpipeConfigSGIX")) == null) || r;
  r = ((glXHyperpipeAttribSGIX = (PFNGLXHYPERPIPEATTRIBSGIXPROC)glewGetProcAddress((const GLubyte*)"glXHyperpipeAttribSGIX")) == null) || r;
  r = ((glXHyperpipeConfigSGIX = (PFNGLXHYPERPIPECONFIGSGIXPROC)glewGetProcAddress((const GLubyte*)"glXHyperpipeConfigSGIX")) == null) || r;
  r = ((glXQueryHyperpipeAttribSGIX = (PFNGLXQUERYHYPERPIPEATTRIBSGIXPROC)glewGetProcAddress((const GLubyte*)"glXQueryHyperpipeAttribSGIX")) == null) || r;
  r = ((glXQueryHyperpipeBestAttribSGIX = (PFNGLXQUERYHYPERPIPEBESTATTRIBSGIXPROC)glewGetProcAddress((const GLubyte*)"glXQueryHyperpipeBestAttribSGIX")) == null) || r;
  r = ((glXQueryHyperpipeConfigSGIX = (PFNGLXQUERYHYPERPIPECONFIGSGIXPROC)glewGetProcAddress((const GLubyte*)"glXQueryHyperpipeConfigSGIX")) == null) || r;
  r = ((glXQueryHyperpipeNetworkSGIX = (PFNGLXQUERYHYPERPIPENETWORKSGIXPROC)glewGetProcAddress((const GLubyte*)"glXQueryHyperpipeNetworkSGIX")) == null) || r;

  return r;
}

#endif /* GLX_SGIX_hyperpipe */

#ifdef GLX_SGIX_pbuffer

static GLboolean _glewInit_GLX_SGIX_pbuffer (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXCreateGLXPbufferSGIX = (PFNGLXCREATEGLXPBUFFERSGIXPROC)glewGetProcAddress((const GLubyte*)"glXCreateGLXPbufferSGIX")) == null) || r;
  r = ((glXDestroyGLXPbufferSGIX = (PFNGLXDESTROYGLXPBUFFERSGIXPROC)glewGetProcAddress((const GLubyte*)"glXDestroyGLXPbufferSGIX")) == null) || r;
  r = ((glXGetSelectedEventSGIX = (PFNGLXGETSELECTEDEVENTSGIXPROC)glewGetProcAddress((const GLubyte*)"glXGetSelectedEventSGIX")) == null) || r;
  r = ((glXQueryGLXPbufferSGIX = (PFNGLXQUERYGLXPBUFFERSGIXPROC)glewGetProcAddress((const GLubyte*)"glXQueryGLXPbufferSGIX")) == null) || r;
  r = ((glXSelectEventSGIX = (PFNGLXSELECTEVENTSGIXPROC)glewGetProcAddress((const GLubyte*)"glXSelectEventSGIX")) == null) || r;

  return r;
}

#endif /* GLX_SGIX_pbuffer */

#ifdef GLX_SGIX_swap_barrier

static GLboolean _glewInit_GLX_SGIX_swap_barrier (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXBindSwapBarrierSGIX = (PFNGLXBINDSWAPBARRIERSGIXPROC)glewGetProcAddress((const GLubyte*)"glXBindSwapBarrierSGIX")) == null) || r;
  r = ((glXQueryMaxSwapBarriersSGIX = (PFNGLXQUERYMAXSWAPBARRIERSSGIXPROC)glewGetProcAddress((const GLubyte*)"glXQueryMaxSwapBarriersSGIX")) == null) || r;

  return r;
}

#endif /* GLX_SGIX_swap_barrier */

#ifdef GLX_SGIX_swap_group

static GLboolean _glewInit_GLX_SGIX_swap_group (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXJoinSwapGroupSGIX = (PFNGLXJOINSWAPGROUPSGIXPROC)glewGetProcAddress((const GLubyte*)"glXJoinSwapGroupSGIX")) == null) || r;

  return r;
}

#endif /* GLX_SGIX_swap_group */

#ifdef GLX_SGIX_video_resize

static GLboolean _glewInit_GLX_SGIX_video_resize (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXBindChannelToWindowSGIX = (PFNGLXBINDCHANNELTOWINDOWSGIXPROC)glewGetProcAddress((const GLubyte*)"glXBindChannelToWindowSGIX")) == null) || r;
  r = ((glXChannelRectSGIX = (PFNGLXCHANNELRECTSGIXPROC)glewGetProcAddress((const GLubyte*)"glXChannelRectSGIX")) == null) || r;
  r = ((glXChannelRectSyncSGIX = (PFNGLXCHANNELRECTSYNCSGIXPROC)glewGetProcAddress((const GLubyte*)"glXChannelRectSyncSGIX")) == null) || r;
  r = ((glXQueryChannelDeltasSGIX = (PFNGLXQUERYCHANNELDELTASSGIXPROC)glewGetProcAddress((const GLubyte*)"glXQueryChannelDeltasSGIX")) == null) || r;
  r = ((glXQueryChannelRectSGIX = (PFNGLXQUERYCHANNELRECTSGIXPROC)glewGetProcAddress((const GLubyte*)"glXQueryChannelRectSGIX")) == null) || r;

  return r;
}

#endif /* GLX_SGIX_video_resize */

#ifdef GLX_SGIX_visual_select_group

#endif /* GLX_SGIX_visual_select_group */

#ifdef GLX_SGI_cushion

static GLboolean _glewInit_GLX_SGI_cushion (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXCushionSGI = (PFNGLXCUSHIONSGIPROC)glewGetProcAddress((const GLubyte*)"glXCushionSGI")) == null) || r;

  return r;
}

#endif /* GLX_SGI_cushion */

#ifdef GLX_SGI_make_current_read

static GLboolean _glewInit_GLX_SGI_make_current_read (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXGetCurrentReadDrawableSGI = (PFNGLXGETCURRENTREADDRAWABLESGIPROC)glewGetProcAddress((const GLubyte*)"glXGetCurrentReadDrawableSGI")) == null) || r;
  r = ((glXMakeCurrentReadSGI = (PFNGLXMAKECURRENTREADSGIPROC)glewGetProcAddress((const GLubyte*)"glXMakeCurrentReadSGI")) == null) || r;

  return r;
}

#endif /* GLX_SGI_make_current_read */

#ifdef GLX_SGI_swap_control

static GLboolean _glewInit_GLX_SGI_swap_control (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXSwapIntervalSGI = (PFNGLXSWAPINTERVALSGIPROC)glewGetProcAddress((const GLubyte*)"glXSwapIntervalSGI")) == null) || r;

  return r;
}

#endif /* GLX_SGI_swap_control */

#ifdef GLX_SGI_video_sync

static GLboolean _glewInit_GLX_SGI_video_sync (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXGetVideoSyncSGI = (PFNGLXGETVIDEOSYNCSGIPROC)glewGetProcAddress((const GLubyte*)"glXGetVideoSyncSGI")) == null) || r;
  r = ((glXWaitVideoSyncSGI = (PFNGLXWAITVIDEOSYNCSGIPROC)glewGetProcAddress((const GLubyte*)"glXWaitVideoSyncSGI")) == null) || r;

  return r;
}

#endif /* GLX_SGI_video_sync */

#ifdef GLX_SUN_get_transparent_index

static GLboolean _glewInit_GLX_SUN_get_transparent_index (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXGetTransparentIndexSUN = (PFNGLXGETTRANSPARENTINDEXSUNPROC)glewGetProcAddress((const GLubyte*)"glXGetTransparentIndexSUN")) == null) || r;

  return r;
}

#endif /* GLX_SUN_get_transparent_index */

#ifdef GLX_SUN_video_resize

static GLboolean _glewInit_GLX_SUN_video_resize (GLXEW_CONTEXT_ARG_DEF_INIT)
{
  GLboolean r = GL_FALSE;

  r = ((glXGetVideoResizeSUN = (PFNGLXGETVIDEORESIZESUNPROC)glewGetProcAddress((const GLubyte*)"glXGetVideoResizeSUN")) == null) || r;
  r = ((glXVideoResizeSUN = (PFNGLXVIDEORESIZESUNPROC)glewGetProcAddress((const GLubyte*)"glXVideoResizeSUN")) == null) || r;

  return r;
}

#endif /* GLX_SUN_video_resize */

/* ------------------------------------------------------------------------ */

GLboolean glxewGetExtension (const char* name)
{    
  GLubyte* p;
  GLubyte* end;
  GLuint len;

  if (glXGetCurrentDisplay == null) return GL_FALSE;
  len = _glewStrLen((const GLubyte*)name);
  p = (GLubyte*)glXGetClientString(glXGetCurrentDisplay(), GLX_EXTENSIONS);
  if (0 == p) return GL_FALSE;
  end = p + _glewStrLen(p);
  while (p < end)
  {
    GLuint n = _glewStrCLen(p, ' ');
    if (len == n && _glewStrSame((const GLubyte*)name, p, n)) return GL_TRUE;
    p += n+1;
  }
  return GL_FALSE;
}

GLenum glxewContextInit (GLXEW_CONTEXT_ARG_DEF_LIST)
{
  int major, minor;
  /* initialize core GLX 1.2 */
  if (_glewInit_GLX_VERSION_1_2(GLEW_CONTEXT_ARG_VAR_INIT)) return GLEW_ERROR_GLX_VERSION_11_ONLY;
  /* initialize flags */
  CONST_CAST(GLXEW_VERSION_1_0) = GL_TRUE;
  CONST_CAST(GLXEW_VERSION_1_1) = GL_TRUE;
  CONST_CAST(GLXEW_VERSION_1_2) = GL_TRUE;
  CONST_CAST(GLXEW_VERSION_1_3) = GL_TRUE;
  CONST_CAST(GLXEW_VERSION_1_4) = GL_TRUE;
  /* query GLX version */
  glXQueryVersion(glXGetCurrentDisplay(), &major, &minor);
  if (major == 1 && minor <= 3)
  {
    switch (minor)
    {
      case 3:
      CONST_CAST(GLXEW_VERSION_1_4) = GL_FALSE;
      break;
      case 2:
      CONST_CAST(GLXEW_VERSION_1_4) = GL_FALSE;
      CONST_CAST(GLXEW_VERSION_1_3) = GL_FALSE;
      break;
      default:
      return GLEW_ERROR_GLX_VERSION_11_ONLY;
      break;
    }
  }
  /* initialize extensions */
#ifdef GLX_VERSION_1_3
  if (glewExperimental || GLXEW_VERSION_1_3) CONST_CAST(GLXEW_VERSION_1_3) = !_glewInit_GLX_VERSION_1_3(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_VERSION_1_3 */
#ifdef GLX_3DFX_multisample
  CONST_CAST(GLXEW_3DFX_multisample) = glxewGetExtension("GLX_3DFX_multisample");
#endif /* GLX_3DFX_multisample */
#ifdef GLX_ARB_create_context
  CONST_CAST(GLXEW_ARB_create_context) = glxewGetExtension("GLX_ARB_create_context");
  if (glewExperimental || GLXEW_ARB_create_context) CONST_CAST(GLXEW_ARB_create_context) = !_glewInit_GLX_ARB_create_context(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_ARB_create_context */
#ifdef GLX_ARB_create_context_profile
  CONST_CAST(GLXEW_ARB_create_context_profile) = glxewGetExtension("GLX_ARB_create_context_profile");
#endif /* GLX_ARB_create_context_profile */
#ifdef GLX_ARB_fbconfig_float
  CONST_CAST(GLXEW_ARB_fbconfig_float) = glxewGetExtension("GLX_ARB_fbconfig_float");
#endif /* GLX_ARB_fbconfig_float */
#ifdef GLX_ARB_framebuffer_sRGB
  CONST_CAST(GLXEW_ARB_framebuffer_sRGB) = glxewGetExtension("GLX_ARB_framebuffer_sRGB");
#endif /* GLX_ARB_framebuffer_sRGB */
#ifdef GLX_ARB_get_proc_address
  CONST_CAST(GLXEW_ARB_get_proc_address) = glxewGetExtension("GLX_ARB_get_proc_address");
#endif /* GLX_ARB_get_proc_address */
#ifdef GLX_ARB_multisample
  CONST_CAST(GLXEW_ARB_multisample) = glxewGetExtension("GLX_ARB_multisample");
#endif /* GLX_ARB_multisample */
#ifdef GLX_ATI_pixel_format_float
  CONST_CAST(GLXEW_ATI_pixel_format_float) = glxewGetExtension("GLX_ATI_pixel_format_float");
#endif /* GLX_ATI_pixel_format_float */
#ifdef GLX_ATI_render_texture
  CONST_CAST(GLXEW_ATI_render_texture) = glxewGetExtension("GLX_ATI_render_texture");
  if (glewExperimental || GLXEW_ATI_render_texture) CONST_CAST(GLXEW_ATI_render_texture) = !_glewInit_GLX_ATI_render_texture(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_ATI_render_texture */
#ifdef GLX_EXT_fbconfig_packed_float
  CONST_CAST(GLXEW_EXT_fbconfig_packed_float) = glxewGetExtension("GLX_EXT_fbconfig_packed_float");
#endif /* GLX_EXT_fbconfig_packed_float */
#ifdef GLX_EXT_framebuffer_sRGB
  CONST_CAST(GLXEW_EXT_framebuffer_sRGB) = glxewGetExtension("GLX_EXT_framebuffer_sRGB");
#endif /* GLX_EXT_framebuffer_sRGB */
#ifdef GLX_EXT_import_context
  CONST_CAST(GLXEW_EXT_import_context) = glxewGetExtension("GLX_EXT_import_context");
  if (glewExperimental || GLXEW_EXT_import_context) CONST_CAST(GLXEW_EXT_import_context) = !_glewInit_GLX_EXT_import_context(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_EXT_import_context */
#ifdef GLX_EXT_scene_marker
  CONST_CAST(GLXEW_EXT_scene_marker) = glxewGetExtension("GLX_EXT_scene_marker");
#endif /* GLX_EXT_scene_marker */
#ifdef GLX_EXT_swap_control
  CONST_CAST(GLXEW_EXT_swap_control) = glxewGetExtension("GLX_EXT_swap_control");
  if (glewExperimental || GLXEW_EXT_swap_control) CONST_CAST(GLXEW_EXT_swap_control) = !_glewInit_GLX_EXT_swap_control(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_EXT_swap_control */
#ifdef GLX_EXT_texture_from_pixmap
  CONST_CAST(GLXEW_EXT_texture_from_pixmap) = glxewGetExtension("GLX_EXT_texture_from_pixmap");
  if (glewExperimental || GLXEW_EXT_texture_from_pixmap) CONST_CAST(GLXEW_EXT_texture_from_pixmap) = !_glewInit_GLX_EXT_texture_from_pixmap(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_EXT_texture_from_pixmap */
#ifdef GLX_EXT_visual_info
  CONST_CAST(GLXEW_EXT_visual_info) = glxewGetExtension("GLX_EXT_visual_info");
#endif /* GLX_EXT_visual_info */
#ifdef GLX_EXT_visual_rating
  CONST_CAST(GLXEW_EXT_visual_rating) = glxewGetExtension("GLX_EXT_visual_rating");
#endif /* GLX_EXT_visual_rating */
#ifdef GLX_INTEL_swap_event
  CONST_CAST(GLXEW_INTEL_swap_event) = glxewGetExtension("GLX_INTEL_swap_event");
#endif /* GLX_INTEL_swap_event */
#ifdef GLX_MESA_agp_offset
  CONST_CAST(GLXEW_MESA_agp_offset) = glxewGetExtension("GLX_MESA_agp_offset");
  if (glewExperimental || GLXEW_MESA_agp_offset) CONST_CAST(GLXEW_MESA_agp_offset) = !_glewInit_GLX_MESA_agp_offset(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_MESA_agp_offset */
#ifdef GLX_MESA_copy_sub_buffer
  CONST_CAST(GLXEW_MESA_copy_sub_buffer) = glxewGetExtension("GLX_MESA_copy_sub_buffer");
  if (glewExperimental || GLXEW_MESA_copy_sub_buffer) CONST_CAST(GLXEW_MESA_copy_sub_buffer) = !_glewInit_GLX_MESA_copy_sub_buffer(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_MESA_copy_sub_buffer */
#ifdef GLX_MESA_pixmap_colormap
  CONST_CAST(GLXEW_MESA_pixmap_colormap) = glxewGetExtension("GLX_MESA_pixmap_colormap");
  if (glewExperimental || GLXEW_MESA_pixmap_colormap) CONST_CAST(GLXEW_MESA_pixmap_colormap) = !_glewInit_GLX_MESA_pixmap_colormap(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_MESA_pixmap_colormap */
#ifdef GLX_MESA_release_buffers
  CONST_CAST(GLXEW_MESA_release_buffers) = glxewGetExtension("GLX_MESA_release_buffers");
  if (glewExperimental || GLXEW_MESA_release_buffers) CONST_CAST(GLXEW_MESA_release_buffers) = !_glewInit_GLX_MESA_release_buffers(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_MESA_release_buffers */
#ifdef GLX_MESA_set_3dfx_mode
  CONST_CAST(GLXEW_MESA_set_3dfx_mode) = glxewGetExtension("GLX_MESA_set_3dfx_mode");
  if (glewExperimental || GLXEW_MESA_set_3dfx_mode) CONST_CAST(GLXEW_MESA_set_3dfx_mode) = !_glewInit_GLX_MESA_set_3dfx_mode(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_MESA_set_3dfx_mode */
#ifdef GLX_NV_copy_image
  CONST_CAST(GLXEW_NV_copy_image) = glxewGetExtension("GLX_NV_copy_image");
  if (glewExperimental || GLXEW_NV_copy_image) CONST_CAST(GLXEW_NV_copy_image) = !_glewInit_GLX_NV_copy_image(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_NV_copy_image */
#ifdef GLX_NV_float_buffer
  CONST_CAST(GLXEW_NV_float_buffer) = glxewGetExtension("GLX_NV_float_buffer");
#endif /* GLX_NV_float_buffer */
#ifdef GLX_NV_present_video
  CONST_CAST(GLXEW_NV_present_video) = glxewGetExtension("GLX_NV_present_video");
  if (glewExperimental || GLXEW_NV_present_video) CONST_CAST(GLXEW_NV_present_video) = !_glewInit_GLX_NV_present_video(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_NV_present_video */
#ifdef GLX_NV_swap_group
  CONST_CAST(GLXEW_NV_swap_group) = glxewGetExtension("GLX_NV_swap_group");
  if (glewExperimental || GLXEW_NV_swap_group) CONST_CAST(GLXEW_NV_swap_group) = !_glewInit_GLX_NV_swap_group(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_NV_swap_group */
#ifdef GLX_NV_vertex_array_range
  CONST_CAST(GLXEW_NV_vertex_array_range) = glxewGetExtension("GLX_NV_vertex_array_range");
  if (glewExperimental || GLXEW_NV_vertex_array_range) CONST_CAST(GLXEW_NV_vertex_array_range) = !_glewInit_GLX_NV_vertex_array_range(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_NV_vertex_array_range */
#ifdef GLX_NV_video_output
  CONST_CAST(GLXEW_NV_video_output) = glxewGetExtension("GLX_NV_video_output");
  if (glewExperimental || GLXEW_NV_video_output) CONST_CAST(GLXEW_NV_video_output) = !_glewInit_GLX_NV_video_output(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_NV_video_output */
#ifdef GLX_OML_swap_method
  CONST_CAST(GLXEW_OML_swap_method) = glxewGetExtension("GLX_OML_swap_method");
#endif /* GLX_OML_swap_method */
#if defined(GLX_OML_sync_control) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#include <inttypes.h>
  CONST_CAST(GLXEW_OML_sync_control) = glxewGetExtension("GLX_OML_sync_control");
  if (glewExperimental || GLXEW_OML_sync_control) CONST_CAST(GLXEW_OML_sync_control) = !_glewInit_GLX_OML_sync_control(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_OML_sync_control */
#ifdef GLX_SGIS_blended_overlay
  CONST_CAST(GLXEW_SGIS_blended_overlay) = glxewGetExtension("GLX_SGIS_blended_overlay");
#endif /* GLX_SGIS_blended_overlay */
#ifdef GLX_SGIS_color_range
  CONST_CAST(GLXEW_SGIS_color_range) = glxewGetExtension("GLX_SGIS_color_range");
#endif /* GLX_SGIS_color_range */
#ifdef GLX_SGIS_multisample
  CONST_CAST(GLXEW_SGIS_multisample) = glxewGetExtension("GLX_SGIS_multisample");
#endif /* GLX_SGIS_multisample */
#ifdef GLX_SGIS_shared_multisample
  CONST_CAST(GLXEW_SGIS_shared_multisample) = glxewGetExtension("GLX_SGIS_shared_multisample");
#endif /* GLX_SGIS_shared_multisample */
#ifdef GLX_SGIX_fbconfig
  CONST_CAST(GLXEW_SGIX_fbconfig) = glxewGetExtension("GLX_SGIX_fbconfig");
  if (glewExperimental || GLXEW_SGIX_fbconfig) CONST_CAST(GLXEW_SGIX_fbconfig) = !_glewInit_GLX_SGIX_fbconfig(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_SGIX_fbconfig */
#ifdef GLX_SGIX_hyperpipe
  CONST_CAST(GLXEW_SGIX_hyperpipe) = glxewGetExtension("GLX_SGIX_hyperpipe");
  if (glewExperimental || GLXEW_SGIX_hyperpipe) CONST_CAST(GLXEW_SGIX_hyperpipe) = !_glewInit_GLX_SGIX_hyperpipe(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_SGIX_hyperpipe */
#ifdef GLX_SGIX_pbuffer
  CONST_CAST(GLXEW_SGIX_pbuffer) = glxewGetExtension("GLX_SGIX_pbuffer");
  if (glewExperimental || GLXEW_SGIX_pbuffer) CONST_CAST(GLXEW_SGIX_pbuffer) = !_glewInit_GLX_SGIX_pbuffer(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_SGIX_pbuffer */
#ifdef GLX_SGIX_swap_barrier
  CONST_CAST(GLXEW_SGIX_swap_barrier) = glxewGetExtension("GLX_SGIX_swap_barrier");
  if (glewExperimental || GLXEW_SGIX_swap_barrier) CONST_CAST(GLXEW_SGIX_swap_barrier) = !_glewInit_GLX_SGIX_swap_barrier(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_SGIX_swap_barrier */
#ifdef GLX_SGIX_swap_group
  CONST_CAST(GLXEW_SGIX_swap_group) = glxewGetExtension("GLX_SGIX_swap_group");
  if (glewExperimental || GLXEW_SGIX_swap_group) CONST_CAST(GLXEW_SGIX_swap_group) = !_glewInit_GLX_SGIX_swap_group(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_SGIX_swap_group */
#ifdef GLX_SGIX_video_resize
  CONST_CAST(GLXEW_SGIX_video_resize) = glxewGetExtension("GLX_SGIX_video_resize");
  if (glewExperimental || GLXEW_SGIX_video_resize) CONST_CAST(GLXEW_SGIX_video_resize) = !_glewInit_GLX_SGIX_video_resize(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_SGIX_video_resize */
#ifdef GLX_SGIX_visual_select_group
  CONST_CAST(GLXEW_SGIX_visual_select_group) = glxewGetExtension("GLX_SGIX_visual_select_group");
#endif /* GLX_SGIX_visual_select_group */
#ifdef GLX_SGI_cushion
  CONST_CAST(GLXEW_SGI_cushion) = glxewGetExtension("GLX_SGI_cushion");
  if (glewExperimental || GLXEW_SGI_cushion) CONST_CAST(GLXEW_SGI_cushion) = !_glewInit_GLX_SGI_cushion(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_SGI_cushion */
#ifdef GLX_SGI_make_current_read
  CONST_CAST(GLXEW_SGI_make_current_read) = glxewGetExtension("GLX_SGI_make_current_read");
  if (glewExperimental || GLXEW_SGI_make_current_read) CONST_CAST(GLXEW_SGI_make_current_read) = !_glewInit_GLX_SGI_make_current_read(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_SGI_make_current_read */
#ifdef GLX_SGI_swap_control
  CONST_CAST(GLXEW_SGI_swap_control) = glxewGetExtension("GLX_SGI_swap_control");
  if (glewExperimental || GLXEW_SGI_swap_control) CONST_CAST(GLXEW_SGI_swap_control) = !_glewInit_GLX_SGI_swap_control(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_SGI_swap_control */
#ifdef GLX_SGI_video_sync
  CONST_CAST(GLXEW_SGI_video_sync) = glxewGetExtension("GLX_SGI_video_sync");
  if (glewExperimental || GLXEW_SGI_video_sync) CONST_CAST(GLXEW_SGI_video_sync) = !_glewInit_GLX_SGI_video_sync(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_SGI_video_sync */
#ifdef GLX_SUN_get_transparent_index
  CONST_CAST(GLXEW_SUN_get_transparent_index) = glxewGetExtension("GLX_SUN_get_transparent_index");
  if (glewExperimental || GLXEW_SUN_get_transparent_index) CONST_CAST(GLXEW_SUN_get_transparent_index) = !_glewInit_GLX_SUN_get_transparent_index(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_SUN_get_transparent_index */
#ifdef GLX_SUN_video_resize
  CONST_CAST(GLXEW_SUN_video_resize) = glxewGetExtension("GLX_SUN_video_resize");
  if (glewExperimental || GLXEW_SUN_video_resize) CONST_CAST(GLXEW_SUN_video_resize) = !_glewInit_GLX_SUN_video_resize(GLEW_CONTEXT_ARG_VAR_INIT);
#endif /* GLX_SUN_video_resize */

  return GLEW_OK;
}

#endif /* !__APPLE__ || GLEW_APPLE_GLX */

/* ------------------------------------------------------------------------ */

const GLubyte* glewGetErrorString (GLenum error)
{
  static const GLubyte* _glewErrorString[] =
  {
    (const GLubyte*)"No error",
    (const GLubyte*)"Missing GL version",
    (const GLubyte*)"GL 1.1 and up are not supported",
    (const GLubyte*)"GLX 1.2 and up are not supported",
    (const GLubyte*)"Unknown error"
  };
  const int max_error = sizeof(_glewErrorString)/sizeof(*_glewErrorString) - 1;
  return _glewErrorString[(int)error > max_error ? max_error : (int)error];
}

const GLubyte* glewGetString (GLenum name)
{
  static const GLubyte* _glewString[] =
  {
    (const GLubyte*)null,
    (const GLubyte*)"1.5.3",
    (const GLubyte*)"1",
    (const GLubyte*)"5",
    (const GLubyte*)"3"
  };
  const int max_string = sizeof(_glewString)/sizeof(*_glewString) - 1;
  return _glewString[(int)name > max_string ? 0 : (int)name];
}

/* ------------------------------------------------------------------------ */

GLboolean glewExperimental = GL_FALSE;

#if !defined(GLEW_MX)

#if defined(_WIN32)
extern GLenum wglewContextInit (void);
#elif !defined(__APPLE__) || defined(GLEW_APPLE_GLX) /* _UNIX */
extern GLenum glxewContextInit (void);
#endif /* _WIN32 */

GLenum glewInit ()
{
  GLenum r;
  if ( (r = glewContextInit()) ) return r;
#if defined(_WIN32)
  return wglewContextInit();
#elif !defined(__APPLE__) || defined(GLEW_APPLE_GLX) /* _UNIX */
  return glxewContextInit();
#else
  return r;
#endif /* _WIN32 */
}

#endif /* !GLEW_MX */
#ifdef GLEW_MX
GLboolean glewContextIsSupported (GLEWContext* ctx, const char* name)
#else
GLboolean glewIsSupported (const char* name)
#endif
{
  GLubyte* pos = (GLubyte*)name;
  GLuint len = _glewStrLen(pos);
  GLboolean ret = GL_TRUE;
  while (ret && len > 0)
  {
    if (_glewStrSame1(&pos, &len, (const GLubyte*)"GL_", 3))
    {
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"VERSION_", 8))
      {
#ifdef GL_VERSION_1_2
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"1_2", 3))
        {
          ret = GLEW_VERSION_1_2;
          continue;
        }
#endif
#ifdef GL_VERSION_1_3
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"1_3", 3))
        {
          ret = GLEW_VERSION_1_3;
          continue;
        }
#endif
#ifdef GL_VERSION_1_4
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"1_4", 3))
        {
          ret = GLEW_VERSION_1_4;
          continue;
        }
#endif
#ifdef GL_VERSION_1_5
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"1_5", 3))
        {
          ret = GLEW_VERSION_1_5;
          continue;
        }
#endif
#ifdef GL_VERSION_2_0
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"2_0", 3))
        {
          ret = GLEW_VERSION_2_0;
          continue;
        }
#endif
#ifdef GL_VERSION_2_1
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"2_1", 3))
        {
          ret = GLEW_VERSION_2_1;
          continue;
        }
#endif
#ifdef GL_VERSION_3_0
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"3_0", 3))
        {
          ret = GLEW_VERSION_3_0;
          continue;
        }
#endif
#ifdef GL_VERSION_3_1
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"3_1", 3))
        {
          ret = GLEW_VERSION_3_1;
          continue;
        }
#endif
#ifdef GL_VERSION_3_2
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"3_2", 3))
        {
          ret = GLEW_VERSION_3_2;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"3DFX_", 5))
      {
#ifdef GL_3DFX_multisample
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"multisample", 11))
        {
          ret = GLEW_3DFX_multisample;
          continue;
        }
#endif
#ifdef GL_3DFX_tbuffer
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"tbuffer", 7))
        {
          ret = GLEW_3DFX_tbuffer;
          continue;
        }
#endif
#ifdef GL_3DFX_texture_compression_FXT1
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_compression_FXT1", 24))
        {
          ret = GLEW_3DFX_texture_compression_FXT1;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"AMD_", 4))
      {
#ifdef GL_AMD_draw_buffers_blend
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"draw_buffers_blend", 18))
        {
          ret = GLEW_AMD_draw_buffers_blend;
          continue;
        }
#endif
#ifdef GL_AMD_performance_monitor
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"performance_monitor", 19))
        {
          ret = GLEW_AMD_performance_monitor;
          continue;
        }
#endif
#ifdef GL_AMD_seamless_cubemap_per_texture
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"seamless_cubemap_per_texture", 28))
        {
          ret = GLEW_AMD_seamless_cubemap_per_texture;
          continue;
        }
#endif
#ifdef GL_AMD_shader_stencil_export
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"shader_stencil_export", 21))
        {
          ret = GLEW_AMD_shader_stencil_export;
          continue;
        }
#endif
#ifdef GL_AMD_texture_texture4
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_texture4", 16))
        {
          ret = GLEW_AMD_texture_texture4;
          continue;
        }
#endif
#ifdef GL_AMD_vertex_shader_tessellator
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_shader_tessellator", 25))
        {
          ret = GLEW_AMD_vertex_shader_tessellator;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"APPLE_", 6))
      {
#ifdef GL_APPLE_aux_depth_stencil
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"aux_depth_stencil", 17))
        {
          ret = GLEW_APPLE_aux_depth_stencil;
          continue;
        }
#endif
#ifdef GL_APPLE_client_storage
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"client_storage", 14))
        {
          ret = GLEW_APPLE_client_storage;
          continue;
        }
#endif
#ifdef GL_APPLE_element_array
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"element_array", 13))
        {
          ret = GLEW_APPLE_element_array;
          continue;
        }
#endif
#ifdef GL_APPLE_fence
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"fence", 5))
        {
          ret = GLEW_APPLE_fence;
          continue;
        }
#endif
#ifdef GL_APPLE_float_pixels
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"float_pixels", 12))
        {
          ret = GLEW_APPLE_float_pixels;
          continue;
        }
#endif
#ifdef GL_APPLE_flush_buffer_range
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"flush_buffer_range", 18))
        {
          ret = GLEW_APPLE_flush_buffer_range;
          continue;
        }
#endif
#ifdef GL_APPLE_object_purgeable
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"object_purgeable", 16))
        {
          ret = GLEW_APPLE_object_purgeable;
          continue;
        }
#endif
#ifdef GL_APPLE_pixel_buffer
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"pixel_buffer", 12))
        {
          ret = GLEW_APPLE_pixel_buffer;
          continue;
        }
#endif
#ifdef GL_APPLE_rgb_422
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"rgb_422", 7))
        {
          ret = GLEW_APPLE_rgb_422;
          continue;
        }
#endif
#ifdef GL_APPLE_row_bytes
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"row_bytes", 9))
        {
          ret = GLEW_APPLE_row_bytes;
          continue;
        }
#endif
#ifdef GL_APPLE_specular_vector
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"specular_vector", 15))
        {
          ret = GLEW_APPLE_specular_vector;
          continue;
        }
#endif
#ifdef GL_APPLE_texture_range
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_range", 13))
        {
          ret = GLEW_APPLE_texture_range;
          continue;
        }
#endif
#ifdef GL_APPLE_transform_hint
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"transform_hint", 14))
        {
          ret = GLEW_APPLE_transform_hint;
          continue;
        }
#endif
#ifdef GL_APPLE_vertex_array_object
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_array_object", 19))
        {
          ret = GLEW_APPLE_vertex_array_object;
          continue;
        }
#endif
#ifdef GL_APPLE_vertex_array_range
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_array_range", 18))
        {
          ret = GLEW_APPLE_vertex_array_range;
          continue;
        }
#endif
#ifdef GL_APPLE_vertex_program_evaluators
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_program_evaluators", 25))
        {
          ret = GLEW_APPLE_vertex_program_evaluators;
          continue;
        }
#endif
#ifdef GL_APPLE_ycbcr_422
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"ycbcr_422", 9))
        {
          ret = GLEW_APPLE_ycbcr_422;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"ARB_", 4))
      {
#ifdef GL_ARB_color_buffer_float
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"color_buffer_float", 18))
        {
          ret = GLEW_ARB_color_buffer_float;
          continue;
        }
#endif
#ifdef GL_ARB_compatibility
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"compatibility", 13))
        {
          ret = GLEW_ARB_compatibility;
          continue;
        }
#endif
#ifdef GL_ARB_copy_buffer
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"copy_buffer", 11))
        {
          ret = GLEW_ARB_copy_buffer;
          continue;
        }
#endif
#ifdef GL_ARB_depth_buffer_float
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"depth_buffer_float", 18))
        {
          ret = GLEW_ARB_depth_buffer_float;
          continue;
        }
#endif
#ifdef GL_ARB_depth_clamp
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"depth_clamp", 11))
        {
          ret = GLEW_ARB_depth_clamp;
          continue;
        }
#endif
#ifdef GL_ARB_depth_texture
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"depth_texture", 13))
        {
          ret = GLEW_ARB_depth_texture;
          continue;
        }
#endif
#ifdef GL_ARB_draw_buffers
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"draw_buffers", 12))
        {
          ret = GLEW_ARB_draw_buffers;
          continue;
        }
#endif
#ifdef GL_ARB_draw_buffers_blend
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"draw_buffers_blend", 18))
        {
          ret = GLEW_ARB_draw_buffers_blend;
          continue;
        }
#endif
#ifdef GL_ARB_draw_elements_base_vertex
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"draw_elements_base_vertex", 25))
        {
          ret = GLEW_ARB_draw_elements_base_vertex;
          continue;
        }
#endif
#ifdef GL_ARB_draw_instanced
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"draw_instanced", 14))
        {
          ret = GLEW_ARB_draw_instanced;
          continue;
        }
#endif
#ifdef GL_ARB_fragment_coord_conventions
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"fragment_coord_conventions", 26))
        {
          ret = GLEW_ARB_fragment_coord_conventions;
          continue;
        }
#endif
#ifdef GL_ARB_fragment_program
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"fragment_program", 16))
        {
          ret = GLEW_ARB_fragment_program;
          continue;
        }
#endif
#ifdef GL_ARB_fragment_program_shadow
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"fragment_program_shadow", 23))
        {
          ret = GLEW_ARB_fragment_program_shadow;
          continue;
        }
#endif
#ifdef GL_ARB_fragment_shader
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"fragment_shader", 15))
        {
          ret = GLEW_ARB_fragment_shader;
          continue;
        }
#endif
#ifdef GL_ARB_framebuffer_object
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"framebuffer_object", 18))
        {
          ret = GLEW_ARB_framebuffer_object;
          continue;
        }
#endif
#ifdef GL_ARB_framebuffer_sRGB
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"framebuffer_sRGB", 16))
        {
          ret = GLEW_ARB_framebuffer_sRGB;
          continue;
        }
#endif
#ifdef GL_ARB_geometry_shader4
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"geometry_shader4", 16))
        {
          ret = GLEW_ARB_geometry_shader4;
          continue;
        }
#endif
#ifdef GL_ARB_half_float_pixel
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"half_float_pixel", 16))
        {
          ret = GLEW_ARB_half_float_pixel;
          continue;
        }
#endif
#ifdef GL_ARB_half_float_vertex
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"half_float_vertex", 17))
        {
          ret = GLEW_ARB_half_float_vertex;
          continue;
        }
#endif
#ifdef GL_ARB_imaging
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"imaging", 7))
        {
          ret = GLEW_ARB_imaging;
          continue;
        }
#endif
#ifdef GL_ARB_instanced_arrays
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"instanced_arrays", 16))
        {
          ret = GLEW_ARB_instanced_arrays;
          continue;
        }
#endif
#ifdef GL_ARB_map_buffer_range
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"map_buffer_range", 16))
        {
          ret = GLEW_ARB_map_buffer_range;
          continue;
        }
#endif
#ifdef GL_ARB_matrix_palette
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"matrix_palette", 14))
        {
          ret = GLEW_ARB_matrix_palette;
          continue;
        }
#endif
#ifdef GL_ARB_multisample
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"multisample", 11))
        {
          ret = GLEW_ARB_multisample;
          continue;
        }
#endif
#ifdef GL_ARB_multitexture
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"multitexture", 12))
        {
          ret = GLEW_ARB_multitexture;
          continue;
        }
#endif
#ifdef GL_ARB_occlusion_query
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"occlusion_query", 15))
        {
          ret = GLEW_ARB_occlusion_query;
          continue;
        }
#endif
#ifdef GL_ARB_pixel_buffer_object
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"pixel_buffer_object", 19))
        {
          ret = GLEW_ARB_pixel_buffer_object;
          continue;
        }
#endif
#ifdef GL_ARB_point_parameters
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"point_parameters", 16))
        {
          ret = GLEW_ARB_point_parameters;
          continue;
        }
#endif
#ifdef GL_ARB_point_sprite
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"point_sprite", 12))
        {
          ret = GLEW_ARB_point_sprite;
          continue;
        }
#endif
#ifdef GL_ARB_provoking_vertex
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"provoking_vertex", 16))
        {
          ret = GLEW_ARB_provoking_vertex;
          continue;
        }
#endif
#ifdef GL_ARB_sample_shading
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"sample_shading", 14))
        {
          ret = GLEW_ARB_sample_shading;
          continue;
        }
#endif
#ifdef GL_ARB_seamless_cube_map
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"seamless_cube_map", 17))
        {
          ret = GLEW_ARB_seamless_cube_map;
          continue;
        }
#endif
#ifdef GL_ARB_shader_objects
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"shader_objects", 14))
        {
          ret = GLEW_ARB_shader_objects;
          continue;
        }
#endif
#ifdef GL_ARB_shader_texture_lod
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"shader_texture_lod", 18))
        {
          ret = GLEW_ARB_shader_texture_lod;
          continue;
        }
#endif
#ifdef GL_ARB_shading_language_100
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"shading_language_100", 20))
        {
          ret = GLEW_ARB_shading_language_100;
          continue;
        }
#endif
#ifdef GL_ARB_shadow
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"shadow", 6))
        {
          ret = GLEW_ARB_shadow;
          continue;
        }
#endif
#ifdef GL_ARB_shadow_ambient
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"shadow_ambient", 14))
        {
          ret = GLEW_ARB_shadow_ambient;
          continue;
        }
#endif
#ifdef GL_ARB_sync
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"sync", 4))
        {
          ret = GLEW_ARB_sync;
          continue;
        }
#endif
#ifdef GL_ARB_texture_border_clamp
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_border_clamp", 20))
        {
          ret = GLEW_ARB_texture_border_clamp;
          continue;
        }
#endif
#ifdef GL_ARB_texture_buffer_object
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_buffer_object", 21))
        {
          ret = GLEW_ARB_texture_buffer_object;
          continue;
        }
#endif
#ifdef GL_ARB_texture_compression
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_compression", 19))
        {
          ret = GLEW_ARB_texture_compression;
          continue;
        }
#endif
#ifdef GL_ARB_texture_compression_rgtc
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_compression_rgtc", 24))
        {
          ret = GLEW_ARB_texture_compression_rgtc;
          continue;
        }
#endif
#ifdef GL_ARB_texture_cube_map
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_cube_map", 16))
        {
          ret = GLEW_ARB_texture_cube_map;
          continue;
        }
#endif
#ifdef GL_ARB_texture_cube_map_array
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_cube_map_array", 22))
        {
          ret = GLEW_ARB_texture_cube_map_array;
          continue;
        }
#endif
#ifdef GL_ARB_texture_env_add
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_env_add", 15))
        {
          ret = GLEW_ARB_texture_env_add;
          continue;
        }
#endif
#ifdef GL_ARB_texture_env_combine
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_env_combine", 19))
        {
          ret = GLEW_ARB_texture_env_combine;
          continue;
        }
#endif
#ifdef GL_ARB_texture_env_crossbar
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_env_crossbar", 20))
        {
          ret = GLEW_ARB_texture_env_crossbar;
          continue;
        }
#endif
#ifdef GL_ARB_texture_env_dot3
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_env_dot3", 16))
        {
          ret = GLEW_ARB_texture_env_dot3;
          continue;
        }
#endif
#ifdef GL_ARB_texture_float
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_float", 13))
        {
          ret = GLEW_ARB_texture_float;
          continue;
        }
#endif
#ifdef GL_ARB_texture_gather
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_gather", 14))
        {
          ret = GLEW_ARB_texture_gather;
          continue;
        }
#endif
#ifdef GL_ARB_texture_mirrored_repeat
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_mirrored_repeat", 23))
        {
          ret = GLEW_ARB_texture_mirrored_repeat;
          continue;
        }
#endif
#ifdef GL_ARB_texture_multisample
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_multisample", 19))
        {
          ret = GLEW_ARB_texture_multisample;
          continue;
        }
#endif
#ifdef GL_ARB_texture_non_power_of_two
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_non_power_of_two", 24))
        {
          ret = GLEW_ARB_texture_non_power_of_two;
          continue;
        }
#endif
#ifdef GL_ARB_texture_query_lod
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_query_lod", 17))
        {
          ret = GLEW_ARB_texture_query_lod;
          continue;
        }
#endif
#ifdef GL_ARB_texture_rectangle
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_rectangle", 17))
        {
          ret = GLEW_ARB_texture_rectangle;
          continue;
        }
#endif
#ifdef GL_ARB_texture_rg
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_rg", 10))
        {
          ret = GLEW_ARB_texture_rg;
          continue;
        }
#endif
#ifdef GL_ARB_transpose_matrix
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"transpose_matrix", 16))
        {
          ret = GLEW_ARB_transpose_matrix;
          continue;
        }
#endif
#ifdef GL_ARB_uniform_buffer_object
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"uniform_buffer_object", 21))
        {
          ret = GLEW_ARB_uniform_buffer_object;
          continue;
        }
#endif
#ifdef GL_ARB_vertex_array_bgra
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_array_bgra", 17))
        {
          ret = GLEW_ARB_vertex_array_bgra;
          continue;
        }
#endif
#ifdef GL_ARB_vertex_array_object
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_array_object", 19))
        {
          ret = GLEW_ARB_vertex_array_object;
          continue;
        }
#endif
#ifdef GL_ARB_vertex_blend
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_blend", 12))
        {
          ret = GLEW_ARB_vertex_blend;
          continue;
        }
#endif
#ifdef GL_ARB_vertex_buffer_object
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_buffer_object", 20))
        {
          ret = GLEW_ARB_vertex_buffer_object;
          continue;
        }
#endif
#ifdef GL_ARB_vertex_program
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_program", 14))
        {
          ret = GLEW_ARB_vertex_program;
          continue;
        }
#endif
#ifdef GL_ARB_vertex_shader
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_shader", 13))
        {
          ret = GLEW_ARB_vertex_shader;
          continue;
        }
#endif
#ifdef GL_ARB_window_pos
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"window_pos", 10))
        {
          ret = GLEW_ARB_window_pos;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"ATIX_", 5))
      {
#ifdef GL_ATIX_point_sprites
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"point_sprites", 13))
        {
          ret = GLEW_ATIX_point_sprites;
          continue;
        }
#endif
#ifdef GL_ATIX_texture_env_combine3
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_env_combine3", 20))
        {
          ret = GLEW_ATIX_texture_env_combine3;
          continue;
        }
#endif
#ifdef GL_ATIX_texture_env_route
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_env_route", 17))
        {
          ret = GLEW_ATIX_texture_env_route;
          continue;
        }
#endif
#ifdef GL_ATIX_vertex_shader_output_point_size
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_shader_output_point_size", 31))
        {
          ret = GLEW_ATIX_vertex_shader_output_point_size;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"ATI_", 4))
      {
#ifdef GL_ATI_draw_buffers
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"draw_buffers", 12))
        {
          ret = GLEW_ATI_draw_buffers;
          continue;
        }
#endif
#ifdef GL_ATI_element_array
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"element_array", 13))
        {
          ret = GLEW_ATI_element_array;
          continue;
        }
#endif
#ifdef GL_ATI_envmap_bumpmap
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"envmap_bumpmap", 14))
        {
          ret = GLEW_ATI_envmap_bumpmap;
          continue;
        }
#endif
#ifdef GL_ATI_fragment_shader
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"fragment_shader", 15))
        {
          ret = GLEW_ATI_fragment_shader;
          continue;
        }
#endif
#ifdef GL_ATI_map_object_buffer
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"map_object_buffer", 17))
        {
          ret = GLEW_ATI_map_object_buffer;
          continue;
        }
#endif
#ifdef GL_ATI_meminfo
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"meminfo", 7))
        {
          ret = GLEW_ATI_meminfo;
          continue;
        }
#endif
#ifdef GL_ATI_pn_triangles
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"pn_triangles", 12))
        {
          ret = GLEW_ATI_pn_triangles;
          continue;
        }
#endif
#ifdef GL_ATI_separate_stencil
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"separate_stencil", 16))
        {
          ret = GLEW_ATI_separate_stencil;
          continue;
        }
#endif
#ifdef GL_ATI_shader_texture_lod
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"shader_texture_lod", 18))
        {
          ret = GLEW_ATI_shader_texture_lod;
          continue;
        }
#endif
#ifdef GL_ATI_text_fragment_shader
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"text_fragment_shader", 20))
        {
          ret = GLEW_ATI_text_fragment_shader;
          continue;
        }
#endif
#ifdef GL_ATI_texture_compression_3dc
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_compression_3dc", 23))
        {
          ret = GLEW_ATI_texture_compression_3dc;
          continue;
        }
#endif
#ifdef GL_ATI_texture_env_combine3
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_env_combine3", 20))
        {
          ret = GLEW_ATI_texture_env_combine3;
          continue;
        }
#endif
#ifdef GL_ATI_texture_float
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_float", 13))
        {
          ret = GLEW_ATI_texture_float;
          continue;
        }
#endif
#ifdef GL_ATI_texture_mirror_once
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_mirror_once", 19))
        {
          ret = GLEW_ATI_texture_mirror_once;
          continue;
        }
#endif
#ifdef GL_ATI_vertex_array_object
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_array_object", 19))
        {
          ret = GLEW_ATI_vertex_array_object;
          continue;
        }
#endif
#ifdef GL_ATI_vertex_attrib_array_object
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_attrib_array_object", 26))
        {
          ret = GLEW_ATI_vertex_attrib_array_object;
          continue;
        }
#endif
#ifdef GL_ATI_vertex_streams
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_streams", 14))
        {
          ret = GLEW_ATI_vertex_streams;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"EXT_", 4))
      {
#ifdef GL_EXT_422_pixels
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"422_pixels", 10))
        {
          ret = GLEW_EXT_422_pixels;
          continue;
        }
#endif
#ifdef GL_EXT_Cg_shader
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"Cg_shader", 9))
        {
          ret = GLEW_EXT_Cg_shader;
          continue;
        }
#endif
#ifdef GL_EXT_abgr
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"abgr", 4))
        {
          ret = GLEW_EXT_abgr;
          continue;
        }
#endif
#ifdef GL_EXT_bgra
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"bgra", 4))
        {
          ret = GLEW_EXT_bgra;
          continue;
        }
#endif
#ifdef GL_EXT_bindable_uniform
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"bindable_uniform", 16))
        {
          ret = GLEW_EXT_bindable_uniform;
          continue;
        }
#endif
#ifdef GL_EXT_blend_color
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"blend_color", 11))
        {
          ret = GLEW_EXT_blend_color;
          continue;
        }
#endif
#ifdef GL_EXT_blend_equation_separate
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"blend_equation_separate", 23))
        {
          ret = GLEW_EXT_blend_equation_separate;
          continue;
        }
#endif
#ifdef GL_EXT_blend_func_separate
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"blend_func_separate", 19))
        {
          ret = GLEW_EXT_blend_func_separate;
          continue;
        }
#endif
#ifdef GL_EXT_blend_logic_op
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"blend_logic_op", 14))
        {
          ret = GLEW_EXT_blend_logic_op;
          continue;
        }
#endif
#ifdef GL_EXT_blend_minmax
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"blend_minmax", 12))
        {
          ret = GLEW_EXT_blend_minmax;
          continue;
        }
#endif
#ifdef GL_EXT_blend_subtract
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"blend_subtract", 14))
        {
          ret = GLEW_EXT_blend_subtract;
          continue;
        }
#endif
#ifdef GL_EXT_clip_volume_hint
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"clip_volume_hint", 16))
        {
          ret = GLEW_EXT_clip_volume_hint;
          continue;
        }
#endif
#ifdef GL_EXT_cmyka
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"cmyka", 5))
        {
          ret = GLEW_EXT_cmyka;
          continue;
        }
#endif
#ifdef GL_EXT_color_subtable
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"color_subtable", 14))
        {
          ret = GLEW_EXT_color_subtable;
          continue;
        }
#endif
#ifdef GL_EXT_compiled_vertex_array
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"compiled_vertex_array", 21))
        {
          ret = GLEW_EXT_compiled_vertex_array;
          continue;
        }
#endif
#ifdef GL_EXT_convolution
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"convolution", 11))
        {
          ret = GLEW_EXT_convolution;
          continue;
        }
#endif
#ifdef GL_EXT_coordinate_frame
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"coordinate_frame", 16))
        {
          ret = GLEW_EXT_coordinate_frame;
          continue;
        }
#endif
#ifdef GL_EXT_copy_texture
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"copy_texture", 12))
        {
          ret = GLEW_EXT_copy_texture;
          continue;
        }
#endif
#ifdef GL_EXT_cull_vertex
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"cull_vertex", 11))
        {
          ret = GLEW_EXT_cull_vertex;
          continue;
        }
#endif
#ifdef GL_EXT_depth_bounds_test
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"depth_bounds_test", 17))
        {
          ret = GLEW_EXT_depth_bounds_test;
          continue;
        }
#endif
#ifdef GL_EXT_direct_state_access
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"direct_state_access", 19))
        {
          ret = GLEW_EXT_direct_state_access;
          continue;
        }
#endif
#ifdef GL_EXT_draw_buffers2
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"draw_buffers2", 13))
        {
          ret = GLEW_EXT_draw_buffers2;
          continue;
        }
#endif
#ifdef GL_EXT_draw_instanced
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"draw_instanced", 14))
        {
          ret = GLEW_EXT_draw_instanced;
          continue;
        }
#endif
#ifdef GL_EXT_draw_range_elements
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"draw_range_elements", 19))
        {
          ret = GLEW_EXT_draw_range_elements;
          continue;
        }
#endif
#ifdef GL_EXT_fog_coord
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"fog_coord", 9))
        {
          ret = GLEW_EXT_fog_coord;
          continue;
        }
#endif
#ifdef GL_EXT_fragment_lighting
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"fragment_lighting", 17))
        {
          ret = GLEW_EXT_fragment_lighting;
          continue;
        }
#endif
#ifdef GL_EXT_framebuffer_blit
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"framebuffer_blit", 16))
        {
          ret = GLEW_EXT_framebuffer_blit;
          continue;
        }
#endif
#ifdef GL_EXT_framebuffer_multisample
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"framebuffer_multisample", 23))
        {
          ret = GLEW_EXT_framebuffer_multisample;
          continue;
        }
#endif
#ifdef GL_EXT_framebuffer_object
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"framebuffer_object", 18))
        {
          ret = GLEW_EXT_framebuffer_object;
          continue;
        }
#endif
#ifdef GL_EXT_framebuffer_sRGB
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"framebuffer_sRGB", 16))
        {
          ret = GLEW_EXT_framebuffer_sRGB;
          continue;
        }
#endif
#ifdef GL_EXT_geometry_shader4
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"geometry_shader4", 16))
        {
          ret = GLEW_EXT_geometry_shader4;
          continue;
        }
#endif
#ifdef GL_EXT_gpu_program_parameters
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"gpu_program_parameters", 22))
        {
          ret = GLEW_EXT_gpu_program_parameters;
          continue;
        }
#endif
#ifdef GL_EXT_gpu_shader4
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"gpu_shader4", 11))
        {
          ret = GLEW_EXT_gpu_shader4;
          continue;
        }
#endif
#ifdef GL_EXT_histogram
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"histogram", 9))
        {
          ret = GLEW_EXT_histogram;
          continue;
        }
#endif
#ifdef GL_EXT_index_array_formats
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"index_array_formats", 19))
        {
          ret = GLEW_EXT_index_array_formats;
          continue;
        }
#endif
#ifdef GL_EXT_index_func
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"index_func", 10))
        {
          ret = GLEW_EXT_index_func;
          continue;
        }
#endif
#ifdef GL_EXT_index_material
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"index_material", 14))
        {
          ret = GLEW_EXT_index_material;
          continue;
        }
#endif
#ifdef GL_EXT_index_texture
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"index_texture", 13))
        {
          ret = GLEW_EXT_index_texture;
          continue;
        }
#endif
#ifdef GL_EXT_light_texture
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"light_texture", 13))
        {
          ret = GLEW_EXT_light_texture;
          continue;
        }
#endif
#ifdef GL_EXT_misc_attribute
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"misc_attribute", 14))
        {
          ret = GLEW_EXT_misc_attribute;
          continue;
        }
#endif
#ifdef GL_EXT_multi_draw_arrays
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"multi_draw_arrays", 17))
        {
          ret = GLEW_EXT_multi_draw_arrays;
          continue;
        }
#endif
#ifdef GL_EXT_multisample
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"multisample", 11))
        {
          ret = GLEW_EXT_multisample;
          continue;
        }
#endif
#ifdef GL_EXT_packed_depth_stencil
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"packed_depth_stencil", 20))
        {
          ret = GLEW_EXT_packed_depth_stencil;
          continue;
        }
#endif
#ifdef GL_EXT_packed_float
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"packed_float", 12))
        {
          ret = GLEW_EXT_packed_float;
          continue;
        }
#endif
#ifdef GL_EXT_packed_pixels
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"packed_pixels", 13))
        {
          ret = GLEW_EXT_packed_pixels;
          continue;
        }
#endif
#ifdef GL_EXT_paletted_texture
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"paletted_texture", 16))
        {
          ret = GLEW_EXT_paletted_texture;
          continue;
        }
#endif
#ifdef GL_EXT_pixel_buffer_object
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"pixel_buffer_object", 19))
        {
          ret = GLEW_EXT_pixel_buffer_object;
          continue;
        }
#endif
#ifdef GL_EXT_pixel_transform
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"pixel_transform", 15))
        {
          ret = GLEW_EXT_pixel_transform;
          continue;
        }
#endif
#ifdef GL_EXT_pixel_transform_color_table
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"pixel_transform_color_table", 27))
        {
          ret = GLEW_EXT_pixel_transform_color_table;
          continue;
        }
#endif
#ifdef GL_EXT_point_parameters
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"point_parameters", 16))
        {
          ret = GLEW_EXT_point_parameters;
          continue;
        }
#endif
#ifdef GL_EXT_polygon_offset
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"polygon_offset", 14))
        {
          ret = GLEW_EXT_polygon_offset;
          continue;
        }
#endif
#ifdef GL_EXT_provoking_vertex
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"provoking_vertex", 16))
        {
          ret = GLEW_EXT_provoking_vertex;
          continue;
        }
#endif
#ifdef GL_EXT_rescale_normal
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"rescale_normal", 14))
        {
          ret = GLEW_EXT_rescale_normal;
          continue;
        }
#endif
#ifdef GL_EXT_scene_marker
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"scene_marker", 12))
        {
          ret = GLEW_EXT_scene_marker;
          continue;
        }
#endif
#ifdef GL_EXT_secondary_color
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"secondary_color", 15))
        {
          ret = GLEW_EXT_secondary_color;
          continue;
        }
#endif
#ifdef GL_EXT_separate_shader_objects
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"separate_shader_objects", 23))
        {
          ret = GLEW_EXT_separate_shader_objects;
          continue;
        }
#endif
#ifdef GL_EXT_separate_specular_color
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"separate_specular_color", 23))
        {
          ret = GLEW_EXT_separate_specular_color;
          continue;
        }
#endif
#ifdef GL_EXT_shadow_funcs
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"shadow_funcs", 12))
        {
          ret = GLEW_EXT_shadow_funcs;
          continue;
        }
#endif
#ifdef GL_EXT_shared_texture_palette
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"shared_texture_palette", 22))
        {
          ret = GLEW_EXT_shared_texture_palette;
          continue;
        }
#endif
#ifdef GL_EXT_stencil_clear_tag
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"stencil_clear_tag", 17))
        {
          ret = GLEW_EXT_stencil_clear_tag;
          continue;
        }
#endif
#ifdef GL_EXT_stencil_two_side
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"stencil_two_side", 16))
        {
          ret = GLEW_EXT_stencil_two_side;
          continue;
        }
#endif
#ifdef GL_EXT_stencil_wrap
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"stencil_wrap", 12))
        {
          ret = GLEW_EXT_stencil_wrap;
          continue;
        }
#endif
#ifdef GL_EXT_subtexture
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"subtexture", 10))
        {
          ret = GLEW_EXT_subtexture;
          continue;
        }
#endif
#ifdef GL_EXT_texture
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture", 7))
        {
          ret = GLEW_EXT_texture;
          continue;
        }
#endif
#ifdef GL_EXT_texture3D
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture3D", 9))
        {
          ret = GLEW_EXT_texture3D;
          continue;
        }
#endif
#ifdef GL_EXT_texture_array
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_array", 13))
        {
          ret = GLEW_EXT_texture_array;
          continue;
        }
#endif
#ifdef GL_EXT_texture_buffer_object
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_buffer_object", 21))
        {
          ret = GLEW_EXT_texture_buffer_object;
          continue;
        }
#endif
#ifdef GL_EXT_texture_compression_dxt1
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_compression_dxt1", 24))
        {
          ret = GLEW_EXT_texture_compression_dxt1;
          continue;
        }
#endif
#ifdef GL_EXT_texture_compression_latc
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_compression_latc", 24))
        {
          ret = GLEW_EXT_texture_compression_latc;
          continue;
        }
#endif
#ifdef GL_EXT_texture_compression_rgtc
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_compression_rgtc", 24))
        {
          ret = GLEW_EXT_texture_compression_rgtc;
          continue;
        }
#endif
#ifdef GL_EXT_texture_compression_s3tc
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_compression_s3tc", 24))
        {
          ret = GLEW_EXT_texture_compression_s3tc;
          continue;
        }
#endif
#ifdef GL_EXT_texture_cube_map
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_cube_map", 16))
        {
          ret = GLEW_EXT_texture_cube_map;
          continue;
        }
#endif
#ifdef GL_EXT_texture_edge_clamp
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_edge_clamp", 18))
        {
          ret = GLEW_EXT_texture_edge_clamp;
          continue;
        }
#endif
#ifdef GL_EXT_texture_env
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_env", 11))
        {
          ret = GLEW_EXT_texture_env;
          continue;
        }
#endif
#ifdef GL_EXT_texture_env_add
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_env_add", 15))
        {
          ret = GLEW_EXT_texture_env_add;
          continue;
        }
#endif
#ifdef GL_EXT_texture_env_combine
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_env_combine", 19))
        {
          ret = GLEW_EXT_texture_env_combine;
          continue;
        }
#endif
#ifdef GL_EXT_texture_env_dot3
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_env_dot3", 16))
        {
          ret = GLEW_EXT_texture_env_dot3;
          continue;
        }
#endif
#ifdef GL_EXT_texture_filter_anisotropic
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_filter_anisotropic", 26))
        {
          ret = GLEW_EXT_texture_filter_anisotropic;
          continue;
        }
#endif
#ifdef GL_EXT_texture_integer
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_integer", 15))
        {
          ret = GLEW_EXT_texture_integer;
          continue;
        }
#endif
#ifdef GL_EXT_texture_lod_bias
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_lod_bias", 16))
        {
          ret = GLEW_EXT_texture_lod_bias;
          continue;
        }
#endif
#ifdef GL_EXT_texture_mirror_clamp
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_mirror_clamp", 20))
        {
          ret = GLEW_EXT_texture_mirror_clamp;
          continue;
        }
#endif
#ifdef GL_EXT_texture_object
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_object", 14))
        {
          ret = GLEW_EXT_texture_object;
          continue;
        }
#endif
#ifdef GL_EXT_texture_perturb_normal
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_perturb_normal", 22))
        {
          ret = GLEW_EXT_texture_perturb_normal;
          continue;
        }
#endif
#ifdef GL_EXT_texture_rectangle
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_rectangle", 17))
        {
          ret = GLEW_EXT_texture_rectangle;
          continue;
        }
#endif
#ifdef GL_EXT_texture_sRGB
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_sRGB", 12))
        {
          ret = GLEW_EXT_texture_sRGB;
          continue;
        }
#endif
#ifdef GL_EXT_texture_shared_exponent
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_shared_exponent", 23))
        {
          ret = GLEW_EXT_texture_shared_exponent;
          continue;
        }
#endif
#ifdef GL_EXT_texture_snorm
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_snorm", 13))
        {
          ret = GLEW_EXT_texture_snorm;
          continue;
        }
#endif
#ifdef GL_EXT_texture_swizzle
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_swizzle", 15))
        {
          ret = GLEW_EXT_texture_swizzle;
          continue;
        }
#endif
#ifdef GL_EXT_timer_query
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"timer_query", 11))
        {
          ret = GLEW_EXT_timer_query;
          continue;
        }
#endif
#ifdef GL_EXT_transform_feedback
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"transform_feedback", 18))
        {
          ret = GLEW_EXT_transform_feedback;
          continue;
        }
#endif
#ifdef GL_EXT_vertex_array
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_array", 12))
        {
          ret = GLEW_EXT_vertex_array;
          continue;
        }
#endif
#ifdef GL_EXT_vertex_array_bgra
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_array_bgra", 17))
        {
          ret = GLEW_EXT_vertex_array_bgra;
          continue;
        }
#endif
#ifdef GL_EXT_vertex_shader
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_shader", 13))
        {
          ret = GLEW_EXT_vertex_shader;
          continue;
        }
#endif
#ifdef GL_EXT_vertex_weighting
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_weighting", 16))
        {
          ret = GLEW_EXT_vertex_weighting;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"GREMEDY_", 8))
      {
#ifdef GL_GREMEDY_frame_terminator
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"frame_terminator", 16))
        {
          ret = GLEW_GREMEDY_frame_terminator;
          continue;
        }
#endif
#ifdef GL_GREMEDY_string_marker
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"string_marker", 13))
        {
          ret = GLEW_GREMEDY_string_marker;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"HP_", 3))
      {
#ifdef GL_HP_convolution_border_modes
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"convolution_border_modes", 24))
        {
          ret = GLEW_HP_convolution_border_modes;
          continue;
        }
#endif
#ifdef GL_HP_image_transform
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"image_transform", 15))
        {
          ret = GLEW_HP_image_transform;
          continue;
        }
#endif
#ifdef GL_HP_occlusion_test
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"occlusion_test", 14))
        {
          ret = GLEW_HP_occlusion_test;
          continue;
        }
#endif
#ifdef GL_HP_texture_lighting
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_lighting", 16))
        {
          ret = GLEW_HP_texture_lighting;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"IBM_", 4))
      {
#ifdef GL_IBM_cull_vertex
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"cull_vertex", 11))
        {
          ret = GLEW_IBM_cull_vertex;
          continue;
        }
#endif
#ifdef GL_IBM_multimode_draw_arrays
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"multimode_draw_arrays", 21))
        {
          ret = GLEW_IBM_multimode_draw_arrays;
          continue;
        }
#endif
#ifdef GL_IBM_rasterpos_clip
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"rasterpos_clip", 14))
        {
          ret = GLEW_IBM_rasterpos_clip;
          continue;
        }
#endif
#ifdef GL_IBM_static_data
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"static_data", 11))
        {
          ret = GLEW_IBM_static_data;
          continue;
        }
#endif
#ifdef GL_IBM_texture_mirrored_repeat
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_mirrored_repeat", 23))
        {
          ret = GLEW_IBM_texture_mirrored_repeat;
          continue;
        }
#endif
#ifdef GL_IBM_vertex_array_lists
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_array_lists", 18))
        {
          ret = GLEW_IBM_vertex_array_lists;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"INGR_", 5))
      {
#ifdef GL_INGR_color_clamp
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"color_clamp", 11))
        {
          ret = GLEW_INGR_color_clamp;
          continue;
        }
#endif
#ifdef GL_INGR_interlace_read
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"interlace_read", 14))
        {
          ret = GLEW_INGR_interlace_read;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"INTEL_", 6))
      {
#ifdef GL_INTEL_parallel_arrays
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"parallel_arrays", 15))
        {
          ret = GLEW_INTEL_parallel_arrays;
          continue;
        }
#endif
#ifdef GL_INTEL_texture_scissor
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_scissor", 15))
        {
          ret = GLEW_INTEL_texture_scissor;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"KTX_", 4))
      {
#ifdef GL_KTX_buffer_region
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"buffer_region", 13))
        {
          ret = GLEW_KTX_buffer_region;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"MESAX_", 6))
      {
#ifdef GL_MESAX_texture_stack
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_stack", 13))
        {
          ret = GLEW_MESAX_texture_stack;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"MESA_", 5))
      {
#ifdef GL_MESA_pack_invert
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"pack_invert", 11))
        {
          ret = GLEW_MESA_pack_invert;
          continue;
        }
#endif
#ifdef GL_MESA_resize_buffers
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"resize_buffers", 14))
        {
          ret = GLEW_MESA_resize_buffers;
          continue;
        }
#endif
#ifdef GL_MESA_window_pos
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"window_pos", 10))
        {
          ret = GLEW_MESA_window_pos;
          continue;
        }
#endif
#ifdef GL_MESA_ycbcr_texture
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"ycbcr_texture", 13))
        {
          ret = GLEW_MESA_ycbcr_texture;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"NV_", 3))
      {
#ifdef GL_NV_blend_square
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"blend_square", 12))
        {
          ret = GLEW_NV_blend_square;
          continue;
        }
#endif
#ifdef GL_NV_conditional_render
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"conditional_render", 18))
        {
          ret = GLEW_NV_conditional_render;
          continue;
        }
#endif
#ifdef GL_NV_copy_depth_to_color
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"copy_depth_to_color", 19))
        {
          ret = GLEW_NV_copy_depth_to_color;
          continue;
        }
#endif
#ifdef GL_NV_copy_image
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"copy_image", 10))
        {
          ret = GLEW_NV_copy_image;
          continue;
        }
#endif
#ifdef GL_NV_depth_buffer_float
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"depth_buffer_float", 18))
        {
          ret = GLEW_NV_depth_buffer_float;
          continue;
        }
#endif
#ifdef GL_NV_depth_clamp
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"depth_clamp", 11))
        {
          ret = GLEW_NV_depth_clamp;
          continue;
        }
#endif
#ifdef GL_NV_depth_range_unclamped
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"depth_range_unclamped", 21))
        {
          ret = GLEW_NV_depth_range_unclamped;
          continue;
        }
#endif
#ifdef GL_NV_evaluators
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"evaluators", 10))
        {
          ret = GLEW_NV_evaluators;
          continue;
        }
#endif
#ifdef GL_NV_explicit_multisample
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"explicit_multisample", 20))
        {
          ret = GLEW_NV_explicit_multisample;
          continue;
        }
#endif
#ifdef GL_NV_fence
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"fence", 5))
        {
          ret = GLEW_NV_fence;
          continue;
        }
#endif
#ifdef GL_NV_float_buffer
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"float_buffer", 12))
        {
          ret = GLEW_NV_float_buffer;
          continue;
        }
#endif
#ifdef GL_NV_fog_distance
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"fog_distance", 12))
        {
          ret = GLEW_NV_fog_distance;
          continue;
        }
#endif
#ifdef GL_NV_fragment_program
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"fragment_program", 16))
        {
          ret = GLEW_NV_fragment_program;
          continue;
        }
#endif
#ifdef GL_NV_fragment_program2
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"fragment_program2", 17))
        {
          ret = GLEW_NV_fragment_program2;
          continue;
        }
#endif
#ifdef GL_NV_fragment_program4
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"fragment_program4", 17))
        {
          ret = GLEW_NV_fragment_program4;
          continue;
        }
#endif
#ifdef GL_NV_fragment_program_option
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"fragment_program_option", 23))
        {
          ret = GLEW_NV_fragment_program_option;
          continue;
        }
#endif
#ifdef GL_NV_framebuffer_multisample_coverage
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"framebuffer_multisample_coverage", 32))
        {
          ret = GLEW_NV_framebuffer_multisample_coverage;
          continue;
        }
#endif
#ifdef GL_NV_geometry_program4
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"geometry_program4", 17))
        {
          ret = GLEW_NV_geometry_program4;
          continue;
        }
#endif
#ifdef GL_NV_geometry_shader4
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"geometry_shader4", 16))
        {
          ret = GLEW_NV_geometry_shader4;
          continue;
        }
#endif
#ifdef GL_NV_gpu_program4
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"gpu_program4", 12))
        {
          ret = GLEW_NV_gpu_program4;
          continue;
        }
#endif
#ifdef GL_NV_half_float
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"half_float", 10))
        {
          ret = GLEW_NV_half_float;
          continue;
        }
#endif
#ifdef GL_NV_light_max_exponent
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"light_max_exponent", 18))
        {
          ret = GLEW_NV_light_max_exponent;
          continue;
        }
#endif
#ifdef GL_NV_multisample_filter_hint
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"multisample_filter_hint", 23))
        {
          ret = GLEW_NV_multisample_filter_hint;
          continue;
        }
#endif
#ifdef GL_NV_occlusion_query
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"occlusion_query", 15))
        {
          ret = GLEW_NV_occlusion_query;
          continue;
        }
#endif
#ifdef GL_NV_packed_depth_stencil
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"packed_depth_stencil", 20))
        {
          ret = GLEW_NV_packed_depth_stencil;
          continue;
        }
#endif
#ifdef GL_NV_parameter_buffer_object
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"parameter_buffer_object", 23))
        {
          ret = GLEW_NV_parameter_buffer_object;
          continue;
        }
#endif
#ifdef GL_NV_parameter_buffer_object2
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"parameter_buffer_object2", 24))
        {
          ret = GLEW_NV_parameter_buffer_object2;
          continue;
        }
#endif
#ifdef GL_NV_pixel_data_range
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"pixel_data_range", 16))
        {
          ret = GLEW_NV_pixel_data_range;
          continue;
        }
#endif
#ifdef GL_NV_point_sprite
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"point_sprite", 12))
        {
          ret = GLEW_NV_point_sprite;
          continue;
        }
#endif
#ifdef GL_NV_present_video
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"present_video", 13))
        {
          ret = GLEW_NV_present_video;
          continue;
        }
#endif
#ifdef GL_NV_primitive_restart
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"primitive_restart", 17))
        {
          ret = GLEW_NV_primitive_restart;
          continue;
        }
#endif
#ifdef GL_NV_register_combiners
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"register_combiners", 18))
        {
          ret = GLEW_NV_register_combiners;
          continue;
        }
#endif
#ifdef GL_NV_register_combiners2
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"register_combiners2", 19))
        {
          ret = GLEW_NV_register_combiners2;
          continue;
        }
#endif
#ifdef GL_NV_shader_buffer_load
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"shader_buffer_load", 18))
        {
          ret = GLEW_NV_shader_buffer_load;
          continue;
        }
#endif
#ifdef GL_NV_texgen_emboss
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texgen_emboss", 13))
        {
          ret = GLEW_NV_texgen_emboss;
          continue;
        }
#endif
#ifdef GL_NV_texgen_reflection
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texgen_reflection", 17))
        {
          ret = GLEW_NV_texgen_reflection;
          continue;
        }
#endif
#ifdef GL_NV_texture_barrier
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_barrier", 15))
        {
          ret = GLEW_NV_texture_barrier;
          continue;
        }
#endif
#ifdef GL_NV_texture_compression_vtc
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_compression_vtc", 23))
        {
          ret = GLEW_NV_texture_compression_vtc;
          continue;
        }
#endif
#ifdef GL_NV_texture_env_combine4
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_env_combine4", 20))
        {
          ret = GLEW_NV_texture_env_combine4;
          continue;
        }
#endif
#ifdef GL_NV_texture_expand_normal
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_expand_normal", 21))
        {
          ret = GLEW_NV_texture_expand_normal;
          continue;
        }
#endif
#ifdef GL_NV_texture_rectangle
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_rectangle", 17))
        {
          ret = GLEW_NV_texture_rectangle;
          continue;
        }
#endif
#ifdef GL_NV_texture_shader
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_shader", 14))
        {
          ret = GLEW_NV_texture_shader;
          continue;
        }
#endif
#ifdef GL_NV_texture_shader2
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_shader2", 15))
        {
          ret = GLEW_NV_texture_shader2;
          continue;
        }
#endif
#ifdef GL_NV_texture_shader3
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_shader3", 15))
        {
          ret = GLEW_NV_texture_shader3;
          continue;
        }
#endif
#ifdef GL_NV_transform_feedback
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"transform_feedback", 18))
        {
          ret = GLEW_NV_transform_feedback;
          continue;
        }
#endif
#ifdef GL_NV_transform_feedback2
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"transform_feedback2", 19))
        {
          ret = GLEW_NV_transform_feedback2;
          continue;
        }
#endif
#ifdef GL_NV_vertex_array_range
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_array_range", 18))
        {
          ret = GLEW_NV_vertex_array_range;
          continue;
        }
#endif
#ifdef GL_NV_vertex_array_range2
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_array_range2", 19))
        {
          ret = GLEW_NV_vertex_array_range2;
          continue;
        }
#endif
#ifdef GL_NV_vertex_buffer_unified_memory
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_buffer_unified_memory", 28))
        {
          ret = GLEW_NV_vertex_buffer_unified_memory;
          continue;
        }
#endif
#ifdef GL_NV_vertex_program
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_program", 14))
        {
          ret = GLEW_NV_vertex_program;
          continue;
        }
#endif
#ifdef GL_NV_vertex_program1_1
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_program1_1", 17))
        {
          ret = GLEW_NV_vertex_program1_1;
          continue;
        }
#endif
#ifdef GL_NV_vertex_program2
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_program2", 15))
        {
          ret = GLEW_NV_vertex_program2;
          continue;
        }
#endif
#ifdef GL_NV_vertex_program2_option
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_program2_option", 22))
        {
          ret = GLEW_NV_vertex_program2_option;
          continue;
        }
#endif
#ifdef GL_NV_vertex_program3
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_program3", 15))
        {
          ret = GLEW_NV_vertex_program3;
          continue;
        }
#endif
#ifdef GL_NV_vertex_program4
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_program4", 15))
        {
          ret = GLEW_NV_vertex_program4;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"OES_", 4))
      {
#ifdef GL_OES_byte_coordinates
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"byte_coordinates", 16))
        {
          ret = GLEW_OES_byte_coordinates;
          continue;
        }
#endif
#ifdef GL_OES_compressed_paletted_texture
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"compressed_paletted_texture", 27))
        {
          ret = GLEW_OES_compressed_paletted_texture;
          continue;
        }
#endif
#ifdef GL_OES_read_format
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"read_format", 11))
        {
          ret = GLEW_OES_read_format;
          continue;
        }
#endif
#ifdef GL_OES_single_precision
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"single_precision", 16))
        {
          ret = GLEW_OES_single_precision;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"OML_", 4))
      {
#ifdef GL_OML_interlace
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"interlace", 9))
        {
          ret = GLEW_OML_interlace;
          continue;
        }
#endif
#ifdef GL_OML_resample
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"resample", 8))
        {
          ret = GLEW_OML_resample;
          continue;
        }
#endif
#ifdef GL_OML_subsample
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"subsample", 9))
        {
          ret = GLEW_OML_subsample;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"PGI_", 4))
      {
#ifdef GL_PGI_misc_hints
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"misc_hints", 10))
        {
          ret = GLEW_PGI_misc_hints;
          continue;
        }
#endif
#ifdef GL_PGI_vertex_hints
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_hints", 12))
        {
          ret = GLEW_PGI_vertex_hints;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"REND_", 5))
      {
#ifdef GL_REND_screen_coordinates
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"screen_coordinates", 18))
        {
          ret = GLEW_REND_screen_coordinates;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"S3_", 3))
      {
#ifdef GL_S3_s3tc
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"s3tc", 4))
        {
          ret = GLEW_S3_s3tc;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"SGIS_", 5))
      {
#ifdef GL_SGIS_color_range
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"color_range", 11))
        {
          ret = GLEW_SGIS_color_range;
          continue;
        }
#endif
#ifdef GL_SGIS_detail_texture
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"detail_texture", 14))
        {
          ret = GLEW_SGIS_detail_texture;
          continue;
        }
#endif
#ifdef GL_SGIS_fog_function
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"fog_function", 12))
        {
          ret = GLEW_SGIS_fog_function;
          continue;
        }
#endif
#ifdef GL_SGIS_generate_mipmap
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"generate_mipmap", 15))
        {
          ret = GLEW_SGIS_generate_mipmap;
          continue;
        }
#endif
#ifdef GL_SGIS_multisample
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"multisample", 11))
        {
          ret = GLEW_SGIS_multisample;
          continue;
        }
#endif
#ifdef GL_SGIS_pixel_texture
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"pixel_texture", 13))
        {
          ret = GLEW_SGIS_pixel_texture;
          continue;
        }
#endif
#ifdef GL_SGIS_point_line_texgen
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"point_line_texgen", 17))
        {
          ret = GLEW_SGIS_point_line_texgen;
          continue;
        }
#endif
#ifdef GL_SGIS_sharpen_texture
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"sharpen_texture", 15))
        {
          ret = GLEW_SGIS_sharpen_texture;
          continue;
        }
#endif
#ifdef GL_SGIS_texture4D
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture4D", 9))
        {
          ret = GLEW_SGIS_texture4D;
          continue;
        }
#endif
#ifdef GL_SGIS_texture_border_clamp
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_border_clamp", 20))
        {
          ret = GLEW_SGIS_texture_border_clamp;
          continue;
        }
#endif
#ifdef GL_SGIS_texture_edge_clamp
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_edge_clamp", 18))
        {
          ret = GLEW_SGIS_texture_edge_clamp;
          continue;
        }
#endif
#ifdef GL_SGIS_texture_filter4
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_filter4", 15))
        {
          ret = GLEW_SGIS_texture_filter4;
          continue;
        }
#endif
#ifdef GL_SGIS_texture_lod
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_lod", 11))
        {
          ret = GLEW_SGIS_texture_lod;
          continue;
        }
#endif
#ifdef GL_SGIS_texture_select
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_select", 14))
        {
          ret = GLEW_SGIS_texture_select;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"SGIX_", 5))
      {
#ifdef GL_SGIX_async
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"async", 5))
        {
          ret = GLEW_SGIX_async;
          continue;
        }
#endif
#ifdef GL_SGIX_async_histogram
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"async_histogram", 15))
        {
          ret = GLEW_SGIX_async_histogram;
          continue;
        }
#endif
#ifdef GL_SGIX_async_pixel
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"async_pixel", 11))
        {
          ret = GLEW_SGIX_async_pixel;
          continue;
        }
#endif
#ifdef GL_SGIX_blend_alpha_minmax
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"blend_alpha_minmax", 18))
        {
          ret = GLEW_SGIX_blend_alpha_minmax;
          continue;
        }
#endif
#ifdef GL_SGIX_clipmap
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"clipmap", 7))
        {
          ret = GLEW_SGIX_clipmap;
          continue;
        }
#endif
#ifdef GL_SGIX_convolution_accuracy
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"convolution_accuracy", 20))
        {
          ret = GLEW_SGIX_convolution_accuracy;
          continue;
        }
#endif
#ifdef GL_SGIX_depth_texture
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"depth_texture", 13))
        {
          ret = GLEW_SGIX_depth_texture;
          continue;
        }
#endif
#ifdef GL_SGIX_flush_raster
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"flush_raster", 12))
        {
          ret = GLEW_SGIX_flush_raster;
          continue;
        }
#endif
#ifdef GL_SGIX_fog_offset
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"fog_offset", 10))
        {
          ret = GLEW_SGIX_fog_offset;
          continue;
        }
#endif
#ifdef GL_SGIX_fog_texture
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"fog_texture", 11))
        {
          ret = GLEW_SGIX_fog_texture;
          continue;
        }
#endif
#ifdef GL_SGIX_fragment_specular_lighting
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"fragment_specular_lighting", 26))
        {
          ret = GLEW_SGIX_fragment_specular_lighting;
          continue;
        }
#endif
#ifdef GL_SGIX_framezoom
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"framezoom", 9))
        {
          ret = GLEW_SGIX_framezoom;
          continue;
        }
#endif
#ifdef GL_SGIX_interlace
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"interlace", 9))
        {
          ret = GLEW_SGIX_interlace;
          continue;
        }
#endif
#ifdef GL_SGIX_ir_instrument1
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"ir_instrument1", 14))
        {
          ret = GLEW_SGIX_ir_instrument1;
          continue;
        }
#endif
#ifdef GL_SGIX_list_priority
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"list_priority", 13))
        {
          ret = GLEW_SGIX_list_priority;
          continue;
        }
#endif
#ifdef GL_SGIX_pixel_texture
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"pixel_texture", 13))
        {
          ret = GLEW_SGIX_pixel_texture;
          continue;
        }
#endif
#ifdef GL_SGIX_pixel_texture_bits
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"pixel_texture_bits", 18))
        {
          ret = GLEW_SGIX_pixel_texture_bits;
          continue;
        }
#endif
#ifdef GL_SGIX_reference_plane
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"reference_plane", 15))
        {
          ret = GLEW_SGIX_reference_plane;
          continue;
        }
#endif
#ifdef GL_SGIX_resample
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"resample", 8))
        {
          ret = GLEW_SGIX_resample;
          continue;
        }
#endif
#ifdef GL_SGIX_shadow
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"shadow", 6))
        {
          ret = GLEW_SGIX_shadow;
          continue;
        }
#endif
#ifdef GL_SGIX_shadow_ambient
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"shadow_ambient", 14))
        {
          ret = GLEW_SGIX_shadow_ambient;
          continue;
        }
#endif
#ifdef GL_SGIX_sprite
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"sprite", 6))
        {
          ret = GLEW_SGIX_sprite;
          continue;
        }
#endif
#ifdef GL_SGIX_tag_sample_buffer
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"tag_sample_buffer", 17))
        {
          ret = GLEW_SGIX_tag_sample_buffer;
          continue;
        }
#endif
#ifdef GL_SGIX_texture_add_env
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_add_env", 15))
        {
          ret = GLEW_SGIX_texture_add_env;
          continue;
        }
#endif
#ifdef GL_SGIX_texture_coordinate_clamp
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_coordinate_clamp", 24))
        {
          ret = GLEW_SGIX_texture_coordinate_clamp;
          continue;
        }
#endif
#ifdef GL_SGIX_texture_lod_bias
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_lod_bias", 16))
        {
          ret = GLEW_SGIX_texture_lod_bias;
          continue;
        }
#endif
#ifdef GL_SGIX_texture_multi_buffer
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_multi_buffer", 20))
        {
          ret = GLEW_SGIX_texture_multi_buffer;
          continue;
        }
#endif
#ifdef GL_SGIX_texture_range
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_range", 13))
        {
          ret = GLEW_SGIX_texture_range;
          continue;
        }
#endif
#ifdef GL_SGIX_texture_scale_bias
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_scale_bias", 18))
        {
          ret = GLEW_SGIX_texture_scale_bias;
          continue;
        }
#endif
#ifdef GL_SGIX_vertex_preclip
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_preclip", 14))
        {
          ret = GLEW_SGIX_vertex_preclip;
          continue;
        }
#endif
#ifdef GL_SGIX_vertex_preclip_hint
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_preclip_hint", 19))
        {
          ret = GLEW_SGIX_vertex_preclip_hint;
          continue;
        }
#endif
#ifdef GL_SGIX_ycrcb
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"ycrcb", 5))
        {
          ret = GLEW_SGIX_ycrcb;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"SGI_", 4))
      {
#ifdef GL_SGI_color_matrix
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"color_matrix", 12))
        {
          ret = GLEW_SGI_color_matrix;
          continue;
        }
#endif
#ifdef GL_SGI_color_table
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"color_table", 11))
        {
          ret = GLEW_SGI_color_table;
          continue;
        }
#endif
#ifdef GL_SGI_texture_color_table
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_color_table", 19))
        {
          ret = GLEW_SGI_texture_color_table;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"SUNX_", 5))
      {
#ifdef GL_SUNX_constant_data
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"constant_data", 13))
        {
          ret = GLEW_SUNX_constant_data;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"SUN_", 4))
      {
#ifdef GL_SUN_convolution_border_modes
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"convolution_border_modes", 24))
        {
          ret = GLEW_SUN_convolution_border_modes;
          continue;
        }
#endif
#ifdef GL_SUN_global_alpha
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"global_alpha", 12))
        {
          ret = GLEW_SUN_global_alpha;
          continue;
        }
#endif
#ifdef GL_SUN_mesh_array
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"mesh_array", 10))
        {
          ret = GLEW_SUN_mesh_array;
          continue;
        }
#endif
#ifdef GL_SUN_read_video_pixels
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"read_video_pixels", 17))
        {
          ret = GLEW_SUN_read_video_pixels;
          continue;
        }
#endif
#ifdef GL_SUN_slice_accum
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"slice_accum", 11))
        {
          ret = GLEW_SUN_slice_accum;
          continue;
        }
#endif
#ifdef GL_SUN_triangle_list
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"triangle_list", 13))
        {
          ret = GLEW_SUN_triangle_list;
          continue;
        }
#endif
#ifdef GL_SUN_vertex
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex", 6))
        {
          ret = GLEW_SUN_vertex;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"WIN_", 4))
      {
#ifdef GL_WIN_phong_shading
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"phong_shading", 13))
        {
          ret = GLEW_WIN_phong_shading;
          continue;
        }
#endif
#ifdef GL_WIN_specular_fog
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"specular_fog", 12))
        {
          ret = GLEW_WIN_specular_fog;
          continue;
        }
#endif
#ifdef GL_WIN_swap_hint
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"swap_hint", 9))
        {
          ret = GLEW_WIN_swap_hint;
          continue;
        }
#endif
      }
    }
    ret = (len == 0);
  }
  return ret;
}

#if defined(_WIN32)

#if defined(GLEW_MX)
GLboolean wglewContextIsSupported (WGLEWContext* ctx, const char* name)
#else
GLboolean wglewIsSupported (const char* name)
#endif
{
  GLubyte* pos = (GLubyte*)name;
  GLuint len = _glewStrLen(pos);
  GLboolean ret = GL_TRUE;
  while (ret && len > 0)
  {
    if (_glewStrSame1(&pos, &len, (const GLubyte*)"WGL_", 4))
    {
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"3DFX_", 5))
      {
#ifdef WGL_3DFX_multisample
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"multisample", 11))
        {
          ret = WGLEW_3DFX_multisample;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"3DL_", 4))
      {
#ifdef WGL_3DL_stereo_control
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"stereo_control", 14))
        {
          ret = WGLEW_3DL_stereo_control;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"AMD_", 4))
      {
#ifdef WGL_AMD_gpu_association
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"gpu_association", 15))
        {
          ret = WGLEW_AMD_gpu_association;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"ARB_", 4))
      {
#ifdef WGL_ARB_buffer_region
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"buffer_region", 13))
        {
          ret = WGLEW_ARB_buffer_region;
          continue;
        }
#endif
#ifdef WGL_ARB_create_context
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"create_context", 14))
        {
          ret = WGLEW_ARB_create_context;
          continue;
        }
#endif
#ifdef WGL_ARB_create_context_profile
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"create_context_profile", 22))
        {
          ret = WGLEW_ARB_create_context_profile;
          continue;
        }
#endif
#ifdef WGL_ARB_extensions_string
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"extensions_string", 17))
        {
          ret = WGLEW_ARB_extensions_string;
          continue;
        }
#endif
#ifdef WGL_ARB_framebuffer_sRGB
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"framebuffer_sRGB", 16))
        {
          ret = WGLEW_ARB_framebuffer_sRGB;
          continue;
        }
#endif
#ifdef WGL_ARB_make_current_read
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"make_current_read", 17))
        {
          ret = WGLEW_ARB_make_current_read;
          continue;
        }
#endif
#ifdef WGL_ARB_multisample
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"multisample", 11))
        {
          ret = WGLEW_ARB_multisample;
          continue;
        }
#endif
#ifdef WGL_ARB_pbuffer
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"pbuffer", 7))
        {
          ret = WGLEW_ARB_pbuffer;
          continue;
        }
#endif
#ifdef WGL_ARB_pixel_format
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"pixel_format", 12))
        {
          ret = WGLEW_ARB_pixel_format;
          continue;
        }
#endif
#ifdef WGL_ARB_pixel_format_float
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"pixel_format_float", 18))
        {
          ret = WGLEW_ARB_pixel_format_float;
          continue;
        }
#endif
#ifdef WGL_ARB_render_texture
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"render_texture", 14))
        {
          ret = WGLEW_ARB_render_texture;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"ATI_", 4))
      {
#ifdef WGL_ATI_pixel_format_float
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"pixel_format_float", 18))
        {
          ret = WGLEW_ATI_pixel_format_float;
          continue;
        }
#endif
#ifdef WGL_ATI_render_texture_rectangle
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"render_texture_rectangle", 24))
        {
          ret = WGLEW_ATI_render_texture_rectangle;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"EXT_", 4))
      {
#ifdef WGL_EXT_depth_float
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"depth_float", 11))
        {
          ret = WGLEW_EXT_depth_float;
          continue;
        }
#endif
#ifdef WGL_EXT_display_color_table
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"display_color_table", 19))
        {
          ret = WGLEW_EXT_display_color_table;
          continue;
        }
#endif
#ifdef WGL_EXT_extensions_string
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"extensions_string", 17))
        {
          ret = WGLEW_EXT_extensions_string;
          continue;
        }
#endif
#ifdef WGL_EXT_framebuffer_sRGB
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"framebuffer_sRGB", 16))
        {
          ret = WGLEW_EXT_framebuffer_sRGB;
          continue;
        }
#endif
#ifdef WGL_EXT_make_current_read
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"make_current_read", 17))
        {
          ret = WGLEW_EXT_make_current_read;
          continue;
        }
#endif
#ifdef WGL_EXT_multisample
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"multisample", 11))
        {
          ret = WGLEW_EXT_multisample;
          continue;
        }
#endif
#ifdef WGL_EXT_pbuffer
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"pbuffer", 7))
        {
          ret = WGLEW_EXT_pbuffer;
          continue;
        }
#endif
#ifdef WGL_EXT_pixel_format
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"pixel_format", 12))
        {
          ret = WGLEW_EXT_pixel_format;
          continue;
        }
#endif
#ifdef WGL_EXT_pixel_format_packed_float
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"pixel_format_packed_float", 25))
        {
          ret = WGLEW_EXT_pixel_format_packed_float;
          continue;
        }
#endif
#ifdef WGL_EXT_swap_control
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"swap_control", 12))
        {
          ret = WGLEW_EXT_swap_control;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"I3D_", 4))
      {
#ifdef WGL_I3D_digital_video_control
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"digital_video_control", 21))
        {
          ret = WGLEW_I3D_digital_video_control;
          continue;
        }
#endif
#ifdef WGL_I3D_gamma
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"gamma", 5))
        {
          ret = WGLEW_I3D_gamma;
          continue;
        }
#endif
#ifdef WGL_I3D_genlock
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"genlock", 7))
        {
          ret = WGLEW_I3D_genlock;
          continue;
        }
#endif
#ifdef WGL_I3D_image_buffer
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"image_buffer", 12))
        {
          ret = WGLEW_I3D_image_buffer;
          continue;
        }
#endif
#ifdef WGL_I3D_swap_frame_lock
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"swap_frame_lock", 15))
        {
          ret = WGLEW_I3D_swap_frame_lock;
          continue;
        }
#endif
#ifdef WGL_I3D_swap_frame_usage
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"swap_frame_usage", 16))
        {
          ret = WGLEW_I3D_swap_frame_usage;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"NV_", 3))
      {
#ifdef WGL_NV_copy_image
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"copy_image", 10))
        {
          ret = WGLEW_NV_copy_image;
          continue;
        }
#endif
#ifdef WGL_NV_float_buffer
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"float_buffer", 12))
        {
          ret = WGLEW_NV_float_buffer;
          continue;
        }
#endif
#ifdef WGL_NV_gpu_affinity
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"gpu_affinity", 12))
        {
          ret = WGLEW_NV_gpu_affinity;
          continue;
        }
#endif
#ifdef WGL_NV_present_video
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"present_video", 13))
        {
          ret = WGLEW_NV_present_video;
          continue;
        }
#endif
#ifdef WGL_NV_render_depth_texture
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"render_depth_texture", 20))
        {
          ret = WGLEW_NV_render_depth_texture;
          continue;
        }
#endif
#ifdef WGL_NV_render_texture_rectangle
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"render_texture_rectangle", 24))
        {
          ret = WGLEW_NV_render_texture_rectangle;
          continue;
        }
#endif
#ifdef WGL_NV_swap_group
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"swap_group", 10))
        {
          ret = WGLEW_NV_swap_group;
          continue;
        }
#endif
#ifdef WGL_NV_vertex_array_range
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_array_range", 18))
        {
          ret = WGLEW_NV_vertex_array_range;
          continue;
        }
#endif
#ifdef WGL_NV_video_output
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"video_output", 12))
        {
          ret = WGLEW_NV_video_output;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"OML_", 4))
      {
#ifdef WGL_OML_sync_control
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"sync_control", 12))
        {
          ret = WGLEW_OML_sync_control;
          continue;
        }
#endif
      }
    }
    ret = (len == 0);
  }
  return ret;
}

#elif !defined(__APPLE__) || defined(GLEW_APPLE_GLX)

#if defined(GLEW_MX)
GLboolean glxewContextIsSupported (GLXEWContext* ctx, const char* name)
#else
GLboolean glxewIsSupported (const char* name)
#endif
{
  GLubyte* pos = (GLubyte*)name;
  GLuint len = _glewStrLen(pos);
  GLboolean ret = GL_TRUE;
  while (ret && len > 0)
  {
    if(_glewStrSame1(&pos, &len, (const GLubyte*)"GLX_", 4))
    {
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"VERSION_", 8))
      {
#ifdef GLX_VERSION_1_2
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"1_2", 3))
        {
          ret = GLXEW_VERSION_1_2;
          continue;
        }
#endif
#ifdef GLX_VERSION_1_3
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"1_3", 3))
        {
          ret = GLXEW_VERSION_1_3;
          continue;
        }
#endif
#ifdef GLX_VERSION_1_4
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"1_4", 3))
        {
          ret = GLXEW_VERSION_1_4;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"3DFX_", 5))
      {
#ifdef GLX_3DFX_multisample
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"multisample", 11))
        {
          ret = GLXEW_3DFX_multisample;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"ARB_", 4))
      {
#ifdef GLX_ARB_create_context
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"create_context", 14))
        {
          ret = GLXEW_ARB_create_context;
          continue;
        }
#endif
#ifdef GLX_ARB_create_context_profile
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"create_context_profile", 22))
        {
          ret = GLXEW_ARB_create_context_profile;
          continue;
        }
#endif
#ifdef GLX_ARB_fbconfig_float
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"fbconfig_float", 14))
        {
          ret = GLXEW_ARB_fbconfig_float;
          continue;
        }
#endif
#ifdef GLX_ARB_framebuffer_sRGB
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"framebuffer_sRGB", 16))
        {
          ret = GLXEW_ARB_framebuffer_sRGB;
          continue;
        }
#endif
#ifdef GLX_ARB_get_proc_address
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"get_proc_address", 16))
        {
          ret = GLXEW_ARB_get_proc_address;
          continue;
        }
#endif
#ifdef GLX_ARB_multisample
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"multisample", 11))
        {
          ret = GLXEW_ARB_multisample;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"ATI_", 4))
      {
#ifdef GLX_ATI_pixel_format_float
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"pixel_format_float", 18))
        {
          ret = GLXEW_ATI_pixel_format_float;
          continue;
        }
#endif
#ifdef GLX_ATI_render_texture
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"render_texture", 14))
        {
          ret = GLXEW_ATI_render_texture;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"EXT_", 4))
      {
#ifdef GLX_EXT_fbconfig_packed_float
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"fbconfig_packed_float", 21))
        {
          ret = GLXEW_EXT_fbconfig_packed_float;
          continue;
        }
#endif
#ifdef GLX_EXT_framebuffer_sRGB
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"framebuffer_sRGB", 16))
        {
          ret = GLXEW_EXT_framebuffer_sRGB;
          continue;
        }
#endif
#ifdef GLX_EXT_import_context
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"import_context", 14))
        {
          ret = GLXEW_EXT_import_context;
          continue;
        }
#endif
#ifdef GLX_EXT_scene_marker
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"scene_marker", 12))
        {
          ret = GLXEW_EXT_scene_marker;
          continue;
        }
#endif
#ifdef GLX_EXT_swap_control
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"swap_control", 12))
        {
          ret = GLXEW_EXT_swap_control;
          continue;
        }
#endif
#ifdef GLX_EXT_texture_from_pixmap
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"texture_from_pixmap", 19))
        {
          ret = GLXEW_EXT_texture_from_pixmap;
          continue;
        }
#endif
#ifdef GLX_EXT_visual_info
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"visual_info", 11))
        {
          ret = GLXEW_EXT_visual_info;
          continue;
        }
#endif
#ifdef GLX_EXT_visual_rating
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"visual_rating", 13))
        {
          ret = GLXEW_EXT_visual_rating;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"INTEL_", 6))
      {
#ifdef GLX_INTEL_swap_event
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"swap_event", 10))
        {
          ret = GLXEW_INTEL_swap_event;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"MESA_", 5))
      {
#ifdef GLX_MESA_agp_offset
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"agp_offset", 10))
        {
          ret = GLXEW_MESA_agp_offset;
          continue;
        }
#endif
#ifdef GLX_MESA_copy_sub_buffer
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"copy_sub_buffer", 15))
        {
          ret = GLXEW_MESA_copy_sub_buffer;
          continue;
        }
#endif
#ifdef GLX_MESA_pixmap_colormap
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"pixmap_colormap", 15))
        {
          ret = GLXEW_MESA_pixmap_colormap;
          continue;
        }
#endif
#ifdef GLX_MESA_release_buffers
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"release_buffers", 15))
        {
          ret = GLXEW_MESA_release_buffers;
          continue;
        }
#endif
#ifdef GLX_MESA_set_3dfx_mode
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"set_3dfx_mode", 13))
        {
          ret = GLXEW_MESA_set_3dfx_mode;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"NV_", 3))
      {
#ifdef GLX_NV_copy_image
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"copy_image", 10))
        {
          ret = GLXEW_NV_copy_image;
          continue;
        }
#endif
#ifdef GLX_NV_float_buffer
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"float_buffer", 12))
        {
          ret = GLXEW_NV_float_buffer;
          continue;
        }
#endif
#ifdef GLX_NV_present_video
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"present_video", 13))
        {
          ret = GLXEW_NV_present_video;
          continue;
        }
#endif
#ifdef GLX_NV_swap_group
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"swap_group", 10))
        {
          ret = GLXEW_NV_swap_group;
          continue;
        }
#endif
#ifdef GLX_NV_vertex_array_range
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"vertex_array_range", 18))
        {
          ret = GLXEW_NV_vertex_array_range;
          continue;
        }
#endif
#ifdef GLX_NV_video_output
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"video_output", 12))
        {
          ret = GLXEW_NV_video_output;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"OML_", 4))
      {
#ifdef GLX_OML_swap_method
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"swap_method", 11))
        {
          ret = GLXEW_OML_swap_method;
          continue;
        }
#endif
#if defined(GLX_OML_sync_control) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#include <inttypes.h>
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"sync_control", 12))
        {
          ret = GLXEW_OML_sync_control;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"SGIS_", 5))
      {
#ifdef GLX_SGIS_blended_overlay
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"blended_overlay", 15))
        {
          ret = GLXEW_SGIS_blended_overlay;
          continue;
        }
#endif
#ifdef GLX_SGIS_color_range
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"color_range", 11))
        {
          ret = GLXEW_SGIS_color_range;
          continue;
        }
#endif
#ifdef GLX_SGIS_multisample
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"multisample", 11))
        {
          ret = GLXEW_SGIS_multisample;
          continue;
        }
#endif
#ifdef GLX_SGIS_shared_multisample
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"shared_multisample", 18))
        {
          ret = GLXEW_SGIS_shared_multisample;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"SGIX_", 5))
      {
#ifdef GLX_SGIX_fbconfig
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"fbconfig", 8))
        {
          ret = GLXEW_SGIX_fbconfig;
          continue;
        }
#endif
#ifdef GLX_SGIX_hyperpipe
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"hyperpipe", 9))
        {
          ret = GLXEW_SGIX_hyperpipe;
          continue;
        }
#endif
#ifdef GLX_SGIX_pbuffer
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"pbuffer", 7))
        {
          ret = GLXEW_SGIX_pbuffer;
          continue;
        }
#endif
#ifdef GLX_SGIX_swap_barrier
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"swap_barrier", 12))
        {
          ret = GLXEW_SGIX_swap_barrier;
          continue;
        }
#endif
#ifdef GLX_SGIX_swap_group
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"swap_group", 10))
        {
          ret = GLXEW_SGIX_swap_group;
          continue;
        }
#endif
#ifdef GLX_SGIX_video_resize
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"video_resize", 12))
        {
          ret = GLXEW_SGIX_video_resize;
          continue;
        }
#endif
#ifdef GLX_SGIX_visual_select_group
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"visual_select_group", 19))
        {
          ret = GLXEW_SGIX_visual_select_group;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"SGI_", 4))
      {
#ifdef GLX_SGI_cushion
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"cushion", 7))
        {
          ret = GLXEW_SGI_cushion;
          continue;
        }
#endif
#ifdef GLX_SGI_make_current_read
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"make_current_read", 17))
        {
          ret = GLXEW_SGI_make_current_read;
          continue;
        }
#endif
#ifdef GLX_SGI_swap_control
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"swap_control", 12))
        {
          ret = GLXEW_SGI_swap_control;
          continue;
        }
#endif
#ifdef GLX_SGI_video_sync
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"video_sync", 10))
        {
          ret = GLXEW_SGI_video_sync;
          continue;
        }
#endif
      }
      if (_glewStrSame2(&pos, &len, (const GLubyte*)"SUN_", 4))
      {
#ifdef GLX_SUN_get_transparent_index
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"get_transparent_index", 21))
        {
          ret = GLXEW_SUN_get_transparent_index;
          continue;
        }
#endif
#ifdef GLX_SUN_video_resize
        if (_glewStrSame3(&pos, &len, (const GLubyte*)"video_resize", 12))
        {
          ret = GLXEW_SUN_video_resize;
          continue;
        }
#endif
      }
    }
    ret = (len == 0);
  }
  return ret;
}

#endif /* _WIN32 */
