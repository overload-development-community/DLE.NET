#ifndef _OGL_SHADER_H
#define _OGL_SHADER_H

#ifdef _WIN32
#include <windows.h>
#include <stddef.h>
#endif

#include "glew.h"
#include "cstack.h"
#include "oglmatrix.h"
#include "vector.h"
#include "renderer.h"

typedef struct tShaderData {
	GLhandleARB		shaders [2];
	GLhandleARB		program;
	int*				refP;
} tShaderData;

class CShaderManager {
	private:
		CStack<tShaderData>		m_shaders;
		int							m_nCurrent;
		bool							m_bSuspendable;

	public:
		CShaderManager ();
		~CShaderManager ();
		void Init (void);
		void Destroy (bool bAll = true);
		void Setup (void);
		int Deploy (int nShader, bool bSuspendable = false);
		int Alloc (int& nShader);
		char* Load (const char* filename);
		int Create (int nShader);
		int Compile (int nShader, const char* pszFragShader, const char* pszVertShader, bool bFromFile = 0);
		int Link (int nShader);
		int Build (int& nShader, const char* pszFragShader, const char* pszVertShader, bool bFromFile = 0);
		void Reset (int nShader);
		void Delete (int nShader);
		inline int Current (void) { return m_nCurrent; }
		inline bool IsCurrent (int nShader) { return m_nCurrent == nShader; }
		inline bool Rebuild (GLhandleARB& nShaderProg) {
			if (0 < int (nShaderProg))
				return true;
			nShaderProg = GLhandleARB (-(int (nShaderProg)));
			return false;
			}
		inline GLint Addr (const char* name) { return (m_nCurrent < 0) ? -1 : glGetUniformLocation (m_shaders [m_nCurrent].program, name); }

		inline bool Active (void) { return (m_nCurrent >= 0) && !m_bSuspendable; }

		inline bool Set (const char* name, int var) { 
			GLint addr = Addr (name);
			if (addr < 0)
				return false;
			glUniform1i (addr, (GLint) var);
			return true;
			}

		inline bool Set (const char* name, float var) { 
			GLint addr = Addr (name);
			if (addr < 0)
				return false;
			glUniform1f (addr, (GLfloat) var);
			return true;
			}

		inline bool Set (const char* name, vec2& var) { 
			GLint addr = Addr (name);
			if (addr < 0)
				return false;
			glUniform2fv (addr, 1, (GLfloat*) var);
			return true;
			}

		inline bool Set (const char* name, vec3& var) { 
			GLint addr = Addr (name);
			if (addr < 0)
				return false;
			glUniform3fv (addr, 1, (GLfloat*) var);
			return true;
			}

		inline bool Set (const char* name, vec4& var) { 
			GLint addr = Addr (name);
			if (addr < 0)
				return false;
			glUniform4fv (addr, 1, (GLfloat*) var);
			return true;
			}

		inline bool Set (const char* name, CDoubleVector& var) { 
			GLint addr = Addr (name);
			if (addr < 0)
				return false;
			vec3 v;
			v [0] = (GLfloat) var.v.x;
			v [1] = (GLfloat) var.v.y;
			v [2] = (GLfloat) var.v.z;
			glUniform3fv (addr, 1, (GLfloat*) v);
			return true;
			}

		inline bool Set (const char* name, COGLMatrix& var) { 
			GLint addr = Addr (name);
			if (addr < 0)
				return false;
			glUniformMatrix4fv (addr, 1, (GLboolean) 0, (GLfloat*) var.ToFloat ());
			return true;
			}

	private:
		void Dispose (GLhandleARB& shaderProg);
		void PrintLog (GLhandleARB handle, int bProgram);
};

extern CShaderManager shaderManager;

#endif
