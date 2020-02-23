#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#include "stdafx.h"
#include "dle-xp.h"
#include "cstack.h"
#include "shadermanager.h"

#define SUSPENDABLE_SHADERS 1
#define HAVE_PRINTLOG 0

CShaderManager shaderManager;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

CShaderManager::CShaderManager ()
{
Init ();
}

//------------------------------------------------------------------------------

CShaderManager::~CShaderManager ()
{
Destroy (true);
}

//------------------------------------------------------------------------------

void CShaderManager::Init (void)
{
m_shaders.Create (100);
m_shaders.SetGrowth (100);
m_shaders.Clear ();
m_nCurrent = -1;
m_bSuspendable = false;
}

//------------------------------------------------------------------------------

void CShaderManager::Destroy (bool bAll)
{
for (int i = 0; i < int (m_shaders.ToS ()); i++) {
	Delete (i);
	if (bAll)
		Reset (i);
	}
if (bAll)
	m_shaders.Destroy ();
}

//------------------------------------------------------------------------------
// load a shader program's source code from a file

char* CShaderManager::Load (const char* fileName) //, char* Shadersource)
{
	FILE*	fp;
	char*	pBuffer = null;
	int 	fSize;
	int	f;

if (!(fileName && *fileName))
	return null;	// no fileName

if (0 > (f = _open (fileName, _O_RDONLY)))
	return null;	// couldn't open file
fSize = _lseek (f, 0, SEEK_END);
_close (f);
if (fSize <= 0)
	return null;	// empty file or seek error

if (!(fp = fopen (fileName, "rb")))
	return null;	// couldn't open file

if (!(pBuffer = new char [fSize + 1])) {
	fclose (fp);
	return null;	// out of memory
	}
fSize = (int) fread (pBuffer, sizeof (char), fSize, fp);
pBuffer [fSize] = '\0';
fclose (fp);
return pBuffer;
}

//------------------------------------------------------------------------------

void CShaderManager::PrintLog (GLhandleARB handle, int bProgram)
{
   GLint nLogLen = 0;
   GLint charsWritten = 0;
   char *infoLog;

#ifdef GL_VERSION_20
if (bProgram) {
	glGetProgramiv (GLuint (int (handle)), GL_INFO_LOG_LENGTH, &nLogLen);
	if ((nLogLen > 0) && (infoLog = new char [nLogLen])) {
		glGetProgramInfoLog (GLuint (int (handle)), nLogLen, &charsWritten, infoLog);
#if HAVE_PRINTLOG
		if (*infoLog) {
			::PrintLog (1);
			::PrintLog (0, "\n%s\n\n", infoLog);
			::PrintLog (-1);
			}
#endif
		delete[] infoLog;
		}
	}
else {
	glGetShaderiv (GLuint (int (handle)), GL_INFO_LOG_LENGTH, &nLogLen);
	if ((nLogLen > 0) && (infoLog = new char [nLogLen])) {
		glGetShaderInfoLog (GLuint (int (handle)), nLogLen, &charsWritten, infoLog);
#if HAVE_PRINTLOG
		if (*infoLog) {
			::PrintLog (1);
			::PrintLog (0, "\n%s\n\n", infoLog);
			::PrintLog (-1);
			}
#endif
		delete[] infoLog;
		}
	}
#else
glGetObjectParameterivARB (GLuint (int (handle)), GL_OBJECT_INFO_LOG_LENGTH_ARB, &nLogLen);
if ((nLogLen > 0) && (infoLog = new char [nLogLen])) {
	glGetInfoLogARB (GLuint (int (handle)), nLogLen, &charsWritten, infoLog);
	ErrorMsg (infoLog);
#if HAVE_PRINTLOG
	if (*infoLog) {
		::PrintLog (1);
		::PrintLog (0, "\n%s\n\n", infoLog);
		::PrintLog (-1);
	}
#endif
	delete[] infoLog;
	}
#endif
}

//------------------------------------------------------------------------------

int CShaderManager::Create (int nShader)
{
if ((nShader < 0) || (nShader >= int (m_shaders.ToS ())))
	return 0;
if (m_shaders [nShader].program || (m_shaders [nShader].program = glCreateProgramObjectARB ()))
	return 1;
#if HAVE_PRINTLOG
::PrintLog (0, "Couldn't create shader program object\n");
#endif
return 0;
}

//------------------------------------------------------------------------------

void CShaderManager::Dispose (GLhandleARB& shaderProg)
{
if (shaderProg) {
	glDeleteObjectARB (shaderProg);
	shaderProg = 0;
	}
}

//------------------------------------------------------------------------------

int CShaderManager::Alloc (int& nShader)
{
if ((nShader >= 0) && (nShader < int (m_shaders.ToS ())) && (m_shaders [nShader].refP == &nShader))
	return nShader;
if (!m_shaders.Grow ())
	return nShader = -1;
nShader = m_shaders.ToS () - 1;
memset (&m_shaders [nShader], 0, sizeof (m_shaders [nShader]));
m_shaders [nShader].refP = &nShader;
return nShader;
}

//------------------------------------------------------------------------------

int CShaderManager::Compile (int nShader, const char* pszFragShader, const char* pszVertShader, bool bFromFile)
{
	GLint		bCompiled [2] = {0,0};
	bool		bError = false;
	int		i;

	static GLint nShaderTypes [2] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};

if ((nShader < 0) || (nShader >= int (m_shaders.ToS ())))
	return 0;
tShaderData& shader = m_shaders [nShader];

for (i = 0; i < 2; i++) {
	if (shader.shaders [i]) {
		glDeleteObjectARB (shader.shaders [i]);
		shader.shaders [i] = 0;
		}
	if (!(shader.shaders [i] = glCreateShaderObjectARB (nShaderTypes [i])))
		break;
#if DBG_SHADERS
	if (bFromFile) {
		shader.shaders [i] = LoadShader (i ? pszFragShader : pszVertShader);
		if (!shader.shaders [i])
			return 0;
		}
#endif
	glShaderSource (shader.shaders [i], 1, i ? reinterpret_cast<const GLcharARB **> (&pszFragShader) : reinterpret_cast<const GLcharARB **> (&pszVertShader), null);
#if DBG_SHADERS
	if (bFromFile) {
		if (i)
			delete[] pszFragShader;
		else
			delete[] pszVertShader;
		}
#endif
	glCompileShader (shader.shaders [i]);
	glGetObjectParameterivARB (shader.shaders [i], GL_OBJECT_COMPILE_STATUS_ARB, &bCompiled [i]);
	if (!bCompiled [i])
		break;
	glAttachObjectARB (shader.program, shader.shaders [i]);
	}

for (i = 0; i < 2; i++) {
	if (!bCompiled [i]) {
		bError = true;
#if HAVE_PRINTLOG
		if (i)
			::PrintLog (0, "\nCouldn't compile fragment shader\n\n\"%s\"\n", pszFragShader);
		else
			::PrintLog (0, "\nCouldn't compile vertex shader\n\n\"%s\"\n", pszVertShader);
#endif
		if (shader.shaders [i]) {
			PrintLog (shader.shaders [i], 0);
			glDeleteObjectARB (shader.shaders [i]);
			shader.shaders [i] = 0;
			}
		}
	}
if (!bError)
	return 1;
m_shaders.Pop ();
return 0;
}

//------------------------------------------------------------------------------

int CShaderManager::Link (int nShader)
{
if ((nShader < 0) || (nShader >= int (m_shaders.ToS ())))
	return 0;
tShaderData& shader = m_shaders [nShader];
glLinkProgram (shader.program);
GLint	bLinked;
glGetObjectParameterivARB (shader.program, GL_OBJECT_LINK_STATUS_ARB, &bLinked);
if (bLinked)
	return 1;
#if HAVE_PRINTLOG
::PrintLog (0, "Couldn't link shader programs\n");
#endif
PrintLog (shader.program, 1);
Dispose (shader.program);
return 0;
}

//------------------------------------------------------------------------------

int CShaderManager::Build (int& nShader, const char* pszFragShader, const char* pszVertShader, bool bFromFile)
{
if ((nShader < 0) || (nShader >= int (m_shaders.ToS ()))) {
	if (Alloc (nShader) < 0)
		return 0;
	if (!Create (nShader))
		return 0;
	}
else {
	if (m_shaders [nShader].program)
		return 1;
	}
if (!Compile (nShader, pszFragShader, pszVertShader, bFromFile))
	return 0;
if (!Link (nShader))
	return 0;
return 1;
}

//------------------------------------------------------------------------------

void CShaderManager::Reset (int nShader)
{
if ((nShader >= 0) && (nShader < int (m_shaders.ToS ()))) {
	*m_shaders [nShader].refP = -1;
	m_shaders [nShader].refP = null;
	}
}

//------------------------------------------------------------------------------

void CShaderManager::Delete (int nShader)
{
if ((nShader >= 0) && (nShader < int (m_shaders.ToS ()))) {
	tShaderData& shader = m_shaders [nShader];
	for (int j = 0; j < 2; j++) {
		if (shader.shaders [j]) {
			glDeleteObjectARB (shader.shaders [j]);
			shader.shaders [j] = 0;
			}
		}
	if (shader.program) {
		glDeleteObjectARB (shader.program);
		shader.program = 0;
		}
	}
}

//------------------------------------------------------------------------------

int CShaderManager::Deploy (int nShader, bool bSuspendable)
{
if (nShader >= int (m_shaders.ToS ()))
	return 0;
GLhandleARB shaderProg = (nShader < 0) ? 0 : m_shaders [nShader].program;
if (m_nCurrent == nShader) {
#if SUSPENDABLE_SHADERS
	if ((nShader >= 0) && m_bSuspendable)
		Set ("bSuspended", 0);
#endif
	return -int (shaderProg); // < 0 => program already bound
	}
#if SUSPENDABLE_SHADERS
if ((nShader < 0) && bSuspendable && m_bSuspendable)
	Set ("bSuspended", 1);
else 
#endif
	{
	m_nCurrent = nShader;
	glUseProgramObjectARB (shaderProg);
#if SUSPENDABLE_SHADERS
	if ((m_bSuspendable = bSuspendable))
		Set ("bSuspended", 0);
#endif
	}
return int (shaderProg);
}

//------------------------------------------------------------------------------

void CShaderManager::Setup (void)
{
Destroy ();
Init ();
}

//------------------------------------------------------------------------------
//eof
