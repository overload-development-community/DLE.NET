#ifndef __FBO_H
#define __FBO_H

#include <windows.h>
#include <stddef.h>

#define MAX_COLOR_BUFFERS 16

typedef struct tFrameBuffer {
	GLuint	hFBO; // OpenGL FBO handle
	GLuint	hColorBuffers [MAX_COLOR_BUFFERS]; // color buffer handles
	GLuint	bufferIds [MAX_COLOR_BUFFERS]; // color buffer attachment ids
	GLuint	hDepthBuffer; // depth buffer handle
	GLuint	hStencilBuffer; // stencil buffer handle
	int		nColorBuffers; // number of available color buffers
	int		nBuffer; // currently selected color buffer(s)
	int		nType; // depth buffer type (depth only / depth + stencil)
	int		nWidth; // render buffer width
	int		nHeight; // render buffer height
	int		bActive; // FBO is draw buffer
	GLenum	nStatus; // result of availability test
} tFrameBuffer;

class CFBO {
	private:
		tFrameBuffer	m_info;

	public:
		CFBO () { Init (); }

		~CFBO () { Destroy (); }

		static bool Setup (void);

		void Init (void);

		int Create (int nWidth, int nHeight, int nType = 1, int nColorBuffers = 1);

		void Destroy (void);

		int Available (void);

		int Enable (int nColorBuffers = 0);

		int Disable (void);

		inline int GetWidth (void) { return m_info.nWidth; }

		inline void SetWidth (int nWidth) { m_info.nWidth = nWidth; }

		inline int GetHeight (void) { return m_info.nHeight; }

		inline void SetHeight (int nHeight) { m_info.nHeight = nHeight; }

		inline GLenum GetStatus (void) { return m_info.nStatus; }

		inline void SetStatus (GLenum nStatus) { m_info.nStatus = nStatus; }

		inline int Active (void) { return m_info.bActive; }

		inline GLuint BufferCount (void) { return m_info.nColorBuffers; }

		void SelectColorBuffers (int nBuffer = 0);

		GLuint Handle (void) { return m_info.hFBO; }

		GLuint& ColorBuffer (int i = 0) { return m_info.hColorBuffers [(i < m_info.nColorBuffers) ? i : 0]; }

		GLuint& ColorBufferId (int i = 0) { return m_info.bufferIds [(i < m_info.nColorBuffers) ? i : 0]; }

		GLuint& DepthBuffer (void) { return m_info.hDepthBuffer; }

		GLuint& StencilBuffer (void) { return m_info.hStencilBuffer; }

		void CFBO::Flush (CRect viewport);
		
	private:
		int CreateColorBuffers (int nBuffers);

		int CreateDepthBuffer (void);

		void AttachBuffers (void);
	};

#endif //__FBO_H
