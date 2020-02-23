#ifndef __renderer_h
#define __renderer_h

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Matrix.h"
#include "Textures.h"
#include "FBO.h"
#include "Frustum.h"
#include "glew.h"

// -----------------------------------------------------------------------------

extern double zoomScales [2];

// -----------------------------------------------------------------------------

#if 1
typedef float depthType;
#define MAX_DEPTH 1e30f
#else
typedef long depthType;
#define MAX_DEPTH 0x7FFFFFFF
#endif

// -----------------------------------------------------------------------------

typedef float vec2 [2];
typedef float vec3 [3];
typedef float vec4 [4];

// -----------------------------------------------------------------------------

typedef struct tTexCoord2d {
	double u, v;
} tTexCoord2d;

// -----------------------------------------------------------------------------

enum eObjectViewFlags {
	eViewObjectsNone          = 0,
	eViewObjectsRobots        = (1 << 0),
	eViewObjectsPlayers		  = (1 << 1),
	eViewObjectsWeapons       = (1 << 2),
	eViewObjectsPowerups      = (1 << 3),
	eViewObjectsKeys          = (1 << 4),
	eViewObjectsHostages      = (1 << 5),
	eViewObjectsControlCenter = (1 << 6),
	eViewObjectsEffects       = (1 << 7),
	eViewObjectsAll           = 0xff
	};

// -----------------------------------------------------------------------------

enum eMineViewFlags {
	eViewMineWalls            = (1 << 0),
	eViewMineSpecial          = (1 << 1),
	eViewMineLights           = (1 << 2),
	eViewMineIllumination     = (1 << 3),
	eViewMineVariableLights   = (1 << 4),
	eViewMineUsedTextures	  = (1 << 5),
	eViewMineSkyBox			  = (1 << 6)
	};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

class CFaceListEntry : public CSideKey {
	public:
		long	m_zMin, m_zMax;
		bool m_bTransparent;
	};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

typedef enum ePenColor {
	penBlack,
	penWhite,
	penGold,
	penRed,
	penMedRed,
	penGray,
	penLtGray, 
	penDkGray,
	penGreen,
	penMedGreen,
	penDkGreen,
	penDkCyan,
	penMedCyan,
	penLtCyan,
	penBlue,
	penMedBlue, 
	penLtBlue,
	penYellow,
	penOrange,
	penMagenta,
	penCount
};

class CRenderData {
public:
	CDoubleVector	m_vCenter;
	CDoubleVector	m_vTranslate;
	CDoubleVector	m_vScale;
	CDoubleVector	m_vRotate;

	CRect				m_viewport;
	int 				m_viewWidth;	// in pixels
	int 				m_viewHeight;	// in pixels
	int 				m_viewDepth;	// in bytes

	double			m_moveRate [2];

	CVertex			m_minViewPoint;
	CVertex			m_maxViewPoint;
	//CLongVector		m_minVPIdx;
	//CLongVector		m_maxVPIdx;

	bool 				m_bIgnoreDepth;
	bool 				m_bDepthTest;
	ubyte				m_alpha;

	CBGR*				m_renderBuffer;
	depthType*		m_depthBuffer;

	CPen*				m_pens [2][penCount];

	uint				m_viewObjectFlags;
	uint				m_viewMineFlags;

	ePenColor		m_pen;
	float				m_penWeight;

public:
	CRenderData ();
	~CRenderData ();
	COLORREF PenColor (int nPen);
};



// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

class CRenderer {
	public:
		CRenderData&	m_renderData;
		CWnd*				m_pParent;
		CDC*				m_pDC;
		bool				m_bOrtho;
		bool				m_bRTT;
		bool				m_bHaveRTT;

	public:
		CRenderer (CRenderData& renderData) : m_renderData (renderData), m_pParent (null), m_pDC (null), m_bOrtho (false), m_bRTT (false), m_bHaveRTT (false) {}

		inline void SetData (CRenderData& renderData) { m_renderData = renderData; }

		inline CDoubleVector& Center (void) { return m_renderData.m_vCenter; }
		inline CDoubleVector& Translation (void) { return m_renderData.m_vTranslate; }
		inline CDoubleVector& Scale (void) { return m_renderData.m_vScale; }
		inline CDoubleVector& Rotation (void) { return m_renderData.m_vRotate; }
		inline CRect& Viewport (void) { return m_renderData.m_viewport; }
		inline int& ViewWidth (void) { return m_renderData.m_viewWidth; }
		inline int& ViewHeight (void) { return m_renderData.m_viewHeight; }
		inline int& ViewDepth (void) { return m_renderData.m_viewDepth; }
		inline CBGR* RenderBuffer (void) { return m_renderData.m_renderBuffer; }
		inline depthType* DepthBuffer (void) { return m_renderData.m_depthBuffer; }
		inline CBGR& RenderBuffer (int i) { return m_renderData.m_renderBuffer [i]; }
		inline depthType& DepthBuffer (int i) { return m_renderData.m_depthBuffer [i]; }
		inline void SetDepthBuffer (depthType* buffer) { m_renderData.m_depthBuffer = buffer; }
		inline CPen* Pen (int nPen, int nWeight = 1) { return (nPen < penCount) ? m_renderData.m_pens [nWeight > 1][nPen] : null; }
		inline COLORREF PenColor (int nPen) { return m_renderData.PenColor (nPen); }
		inline CVertex& MinViewPoint (void) { return m_renderData.m_minViewPoint; }
		inline CVertex& MaxViewPoint (void) { return m_renderData.m_maxViewPoint; }
		inline bool IgnoreDepth (void) { return m_renderData.m_bIgnoreDepth; }
		inline void SetIgnoreDepth (bool bIgnoreDepth) { m_renderData.m_bIgnoreDepth = bIgnoreDepth; }
		inline bool DepthTest (void) { return m_renderData.m_bDepthTest; }
		inline void SetDepthTest (bool bDepthTest) { m_renderData.m_bDepthTest = bDepthTest; }
		inline int RenderIllumination (void) { return (m_renderData.m_viewMineFlags & eViewMineIllumination) != 0; }
		inline int RenderVariableLights (void) { return RenderIllumination () && (m_renderData.m_viewMineFlags & eViewMineVariableLights) != 0; }
		inline ubyte Alpha (void) { return m_renderData.m_alpha; }
		inline void SetAlpha (ubyte alpha) { m_renderData.m_alpha = alpha; }
		inline double ViewMoveRate (void) { return m_renderData.m_moveRate [1]; }
		inline uint& ViewMineFlags (void) { return m_renderData.m_viewMineFlags; }
		inline uint& ViewObjectFlags (void) { return m_renderData.m_viewObjectFlags; }
		inline void SetDC (CDC* pDC) { m_pDC = pDC; }
		inline CDC* DC (void) { return m_pDC; }
		virtual CPen* SelectObject (CPen* pen) = 0;
		virtual CBrush* SelectObject (CBrush* brush) = 0;
		virtual HGDIOBJ SelectObject (HGDIOBJ object) = 0;

		virtual int Setup (CWnd* pParent, CDC* pDC) { 
			m_pParent = pParent, m_pDC = pDC; 
			return 1;
			}
		virtual void Release (void) = 0;
		virtual void ClearView (void) = 0;
		virtual void Zoom (int nSteps, double zoom);
		virtual int Project (CRect* pRC = null, bool bCheckBehind = false) = 0; 
		virtual void DrawFaceTextured (CFaceListEntry& fle) = 0; 
		virtual int FaceIsVisible (CSegment* pSegment, CSide* pSide) = 0; 
		virtual void BeginRender (bool bOrtho = false) = 0; 
		virtual void EndRender (bool bSwapBuffers = false) = 0; 
		virtual int ZoomIn (int nSteps = 1, bool bSlow = false) = 0; 
		virtual int ZoomOut (int nSteps = 1, bool bSlow = false) = 0; 
		virtual CViewMatrix* ViewMatrix (void) = 0; 
		virtual void SetCenter (CVertex v, int nType) = 0; 
		virtual void Pan (char direction, double offset) = 0; 
		virtual void Reset (void);
		virtual void RenderFaces (CFaceListEntry* faceRenderList, int faceCount, int bRenderSideKeys) = 0;
		virtual void FitToView (void) = 0;
		virtual bool CanFitToView (void) = 0;

		virtual void SelectPen (int nPen, float nWeight = 1.0f) = 0;
		void GetPen (ePenColor& nPen, float& penWeight)  { nPen = m_renderData.m_pen, penWeight = m_renderData.m_penWeight; }
		virtual void MoveTo (int x, int y) = 0;
		virtual void LineTo (int x, int y) = 0;
		virtual void Rectangle (int left, int top, int right, int bottom) = 0;
		virtual void Rectangle (CVertex& center, int xRad, int yRad) { 
			Rectangle (center.m_screen.x - xRad, center.m_screen.y - yRad, center.m_screen.x + xRad, center.m_screen.y + yRad); 
			}
		virtual void PolyLine (CPoint* points, int nPoints) = 0;
		virtual void Polygon (CPoint* points, int nPoints) = 0;
		virtual void Ellipse (CVertex& center, double xRad, double yRad) = 0;
		virtual void Ellipse (CPoint& p, int xRad, int yRad) { Ellipse (p.x, p.y, xRad, yRad); }
		virtual void Ellipse (int x, int y, int xRad, int yRad) {
			CVertex v;
			v.m_screen.x = x, v.m_screen.y = y;
			Ellipse (v, xRad, yRad);
			}
		virtual void MoveTo (CVertex& v) { MoveTo (v.m_screen.x, v.m_screen.y); }
		virtual void LineTo (CVertex& v) { LineTo (v.m_screen.x, v.m_screen.y); }
		virtual void PolyLine (CVertex* vertices, int nVertices) = 0;
		virtual void Polygon (CVertex* vertices, int nVertices) = 0;
		virtual void Polygon (CVertex* vertices, int nVertices, ushort* index) = 0;
		virtual void TexturedPolygon (const CTexture* pTexture, tTexCoord2d* texCoords, rgbColord* color, CVertex* vertices, int nVertices, ushort* index) = 0;
		virtual void TexturedPolygon (short nTexture, tTexCoord2d* texCoords, rgbColord* color, CVertex* vertices, int nVertices, ushort* index) = 0;
		virtual void Sprite (const CTexture* pTexture, CVertex center, double width, double height, bool bAlways = false) = 0;

		virtual int Type (void) = 0;
		virtual bool SetPerspective (int nPerspective, bool bKeepPosition = false) = 0;
		virtual int Perspective (void) = 0;
		virtual int GetSideKey (int x, int y, short& nSegment, short& nSide) = 0;
		virtual CFrustum* Frustum (void) { return null; }

		virtual bool Ortho (void) { return m_bOrtho; }
		virtual bool RTT (void) { return m_bRTT; }
		virtual void SetRTT (bool bRTT) { m_bRTT = bRTT; }
		virtual bool IsRTTSupported () { return m_bHaveRTT; }

		bool InitViewDimensions (void);
		void ComputeViewLimits (CRect* pRC);
		void ComputeBrightness (CFaceListEntry& fle, ushort brightness [4], int bVariableLights);

};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

class CRendererSW : public CRenderer {
	public:
		CViewMatrixSW	m_viewMatrix;
		HBITMAP			m_DIB;
		CDC*				m_viewDC;
		CDC				m_DC;

	public:
		CRendererSW (CRenderData& renderData);

		virtual int Setup (CWnd* pParent, CDC* pDC);
		virtual void Reset (void);
		virtual void Release (void);
		virtual int Project (CRect* pRC = null, bool bCheckBehind = false);
		virtual void ClearView (void);
		//virtual void Zoom (int nSteps, double zoom);
		virtual int ZoomIn (int nSteps = 1, bool bSlow = false);
		virtual int ZoomOut (int nSteps = 1, bool bSlow = false);
		virtual void DrawFaceTextured (CFaceListEntry& fle);
		virtual int FaceIsVisible (CSegment* pSegment, CSide* pSide);
		virtual void BeginRender (bool bOrtho = false) {}
		virtual void EndRender (bool bSwapBuffers = false);
		virtual void SetCenter (CVertex v, int nType);
		virtual void Pan (char direction, double offset);
		virtual CViewMatrix* ViewMatrix (void) { return &m_viewMatrix; }
		virtual void FitToView (void) {}
		virtual bool CanFitToView (void) { return true; }
	
		virtual void SelectPen (int nPen, float nWeight = 1.0f);
		virtual void MoveTo (int x, int y) { m_pDC->MoveTo (x, y); }
		virtual void LineTo (int x, int y) { m_pDC->LineTo (x,y ); }
		virtual void Rectangle (int left, int top, int right, int bottom) { m_pDC->Rectangle (left, top, right, bottom); }
		virtual void PolyLine (CPoint* points, int nPoints) { m_pDC->Polyline (points, nPoints); }
		virtual void Polygon (CPoint* points, int nPoints) { m_pDC->Polygon (points, nPoints); }
		virtual void Ellipse (CVertex& center, double xRad, double yRad) { 
			SelectObject ((HBRUSH) GetStockObject (HOLLOW_BRUSH));
			m_pDC->Ellipse (center.m_screen.x - int (abs (xRad)), center.m_screen.y - int (abs (yRad)), center.m_screen.x + int (abs (xRad)), center.m_screen.y + int (abs (yRad))); 
			}
		virtual void PolyLine (CVertex* vertices, int nVertices) {}
		virtual void Polygon (CVertex* vertices, int nVertices) {}
		virtual void Polygon (CVertex* vertices, int nVertices, ushort* index) {}
		virtual void TexturedPolygon (const CTexture* pTexture, tTexCoord2d* texCoords, rgbColord* color, CVertex* vertices, int nVertices, ushort* index) {}
		virtual void TexturedPolygon (short nTexture, tTexCoord2d* texCoords, rgbColord* color, CVertex* vertices, int nVertices, ushort* index) {}
		virtual void Sprite (const CTexture* pTexture, CVertex center, double width, double height, bool bAlways = false) {}

		virtual CPen* SelectObject (CPen* pen) { return m_pDC->SelectObject (pen); }
		virtual CBrush* SelectObject (CBrush* brush) { return m_pDC->SelectObject (brush); }
		virtual HGDIOBJ SelectObject (HGDIOBJ object) { return m_pDC->SelectObject (object); }

		virtual void RenderFaces (CFaceListEntry* faceRenderList, int faceCount, int bRenderSideKeys);
		void RenderFace (CFaceListEntry& fle, const CTexture* pTexture, ushort rowOffset, CBGRA* pColor = null);

		virtual int Type (void) { return 0; }
		virtual bool SetPerspective (int nPerspective, bool bKeepPosition) { return false; }
		virtual int Perspective (void) { return 0; }
		virtual int GetSideKey (int x, int y, short& nSegment, short& nSide) { return -1; }

		inline HBITMAP DIB (void) { return m_DIB; }

	private:
		CLongVector		m_screenCoord [4];
		CLongVector		m_texCoord [4];
		float				m_alpha;

		int ZoomFactor (int nSteps, double min, double max);

		inline bool Blend (CBGR& dest, const CBGRA& src, depthType& depth, depthType z, short brightness = 32767);
		inline depthType Z (CTexture& tex, CLongVector* a, int x, int y);
		inline double ZRange (int x0, int x1, int y, double& z);
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

class CRendererGL : public CRenderer {
	private:
		int		m_x, m_y;
		CVertex	m_v;

	public:
		HGLRC				m_glRC; // Permanent Rendering Context
		CDC*				m_glDC; // Private GDI Device Context
		HDC				m_glHDC;
		bool 				m_glFitToView;
		bool 				m_glInited;
		double			m_glAspectRatio;
		CViewMatrixGL	m_viewMatrix;
		CDoubleVector	m_vZoom;
		CFBO				m_renderBuffers;
		int				m_bRenderSideKeys;
		rgbColor*		m_sideKeys;
		bool				m_bHaveSideKeys;
		CFrustum			m_frustum;
		
	public:
		CRendererGL (CRenderData& renderData);
		~CRendererGL ();

		virtual int Setup (CWnd* pParent, CDC* pDC);
		virtual void Release (void) {};
		virtual void ClearView (void);
		virtual int Project (CRect* pRC = null, bool bCheckBehind = false);
		virtual void Zoom (int nSteps, double zoom);
		virtual int ZoomIn (int nSteps = 1, bool bSlow = false);
		virtual int ZoomOut (int nSteps = 1, bool bSlow = false);
		virtual void DrawFaceTextured (CFaceListEntry& fle);
		virtual int FaceIsVisible (CSegment* pSegment, CSide* pSide) { return 1; }
		virtual void BeginRender (bool bOrtho = false);
		virtual void EndRender (bool bSwapBuffers = false);
		virtual void SetCenter (CVertex v, int nType);
		virtual void Pan (char direction, double offset);
		virtual CViewMatrix* ViewMatrix (void) { return &m_viewMatrix; }
		virtual void Reset (void);
		virtual void FitToView (void) { ComputeZoom (); }
		virtual bool CanFitToView (void) { return Perspective () == 0; }

		virtual void SelectPen (int nPen, float nWeight = 1.0f);
		virtual void MoveTo (int x, int y);
		virtual void LineTo (int x, int y);
		virtual void PolyLine (CPoint* points, int nPoints);
		virtual void Polygon (CPoint* points, int nPoints);
		virtual void Rectangle (int left, int top, int right, int bottom);
		virtual void Ellipse (CVertex& center, double xRad, double yRad);
		bool Clip (CLongVector* line);
		virtual void MoveTo (CVertex& v);
		virtual void LineTo (CVertex& v);
		virtual void PolyLine (CVertex* vertices, int nVertices);
		virtual void Polygon (CVertex* vertices, int nVertices);
		virtual void Polygon (CVertex* vertices, int nVertices, ushort* index);
		virtual void TexturedPolygon (const CTexture* textP, tTexCoord2d* texCoords, rgbColord* color, CVertex* vertices, int nVertices, ushort* index);
		virtual void TexturedPolygon (short nTexture, tTexCoord2d* texCoords, rgbColord* color, CVertex* vertices, int nVertices, ushort* index);
		virtual void Sprite (const CTexture* pTexture, CVertex center, double width, double height, bool bAlways = false);

		virtual CPen* SelectObject (CPen* pen) { return pen; }
		virtual CBrush* SelectObject (CBrush* brush) { return brush; }
		virtual HGDIOBJ SelectObject (HGDIOBJ object) { return object; }

		virtual void RenderFaces (CFaceListEntry* faceRenderList, int faceCount, int bRenderSideKeys);
		void RenderFace (CFaceListEntry& fle, const CTexture* pTexture [], CBGRA* pColor = null);
		void ComputeZoom (void);

		BOOL InitProjection (void);
		void SetupProjection (bool bOrtho = false);
		//BOOL SetupRenderer (CWnd* pParent);
		void DestroyContext (void);
		BOOL CreateContext (void);

		virtual int Type (void) { return 1; }

		virtual bool SetPerspective (int nPerspective, bool bKeepCameraPosition) { 
			if (Perspective () == nPerspective) 
				return false;
			m_viewMatrix.SetPerspective (nPerspective);
			if (bKeepCameraPosition)
				TranslateCameraPosition (nPerspective);
			else
				Reset ();
			return true;
			}
		virtual int Perspective (void) { return m_viewMatrix.Perspective (); }
		void TranslateCameraPosition (int nNewPerspective);

		virtual int GetSideKey (int x, int y, short& nSegment, short& nSide);

		virtual CFrustum* Frustum (void) { return &m_frustum; }
	};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#endif //__renderer_h
