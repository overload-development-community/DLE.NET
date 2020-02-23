// render.cpp

#include "stdafx.h"
#include <math.h>

#include "types.h"
#include "matrix.h"
#include "global.h"
#include "mine.h"
#include "segment.h"
#include "GameObject.h"
#include "dle-xp.h"
#include "dlcdoc.h"
#include "MainFrame.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "mineview.h"

extern short nDbgSeg, nDbgSide;
extern int nDbgVertex;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

static int LineLineIntersection (CLongVector& vi, CLongVector& v1, CLongVector& v2, CLongVector& v3, CLongVector& v4)
{
	double A1 = v2.y - v1.y;
	double B1 = v1.x - v2.x;
	double A2 = v4.y - v3.y;
	double B2 = v3.x - v4.x;

double det = A1 * B2 - A2 * B1;
if (det == 0)
	return 0;

double C1 = A1 * v1.x + B1 * v1.y;
double C2 = A2 * v3.x + B2 * v3.y;

vi.x = (long) ((B2 * C1 - B1 * C2) / det);
vi.y = (long) ((A1 * C2 - A2 * C1) / det);
return 1;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

CRendererSW::CRendererSW (CRenderData& renderData) : CRenderer (renderData), m_DIB (0)
{
}

//------------------------------------------------------------------------------

void CRendererSW::Reset (void)
{
CRenderer::Reset ();
ViewMatrix ()->Setup (Translation (), Scale (), Rotation ());
}

//------------------------------------------------------------------------------

void CRendererSW::Release (void)
{
if (m_DIB) {
	DeleteObject (m_DIB);
	m_DIB = 0;
	}
if (m_DC.m_hDC)
	m_DC.DeleteDC ();
}

//------------------------------------------------------------------------------

int CRendererSW::Setup (CWnd* pParent, CDC *pDC)
{
if (!CRenderer::Setup (pParent, pDC))
	return 0;
m_viewDC = pDC;

int bUpdate = 0;
int depth = 3; // force 24-bit DIB
if (InitViewDimensions () || (depth != ViewDepth ())) {
	Release ();
	bUpdate = 1;
	}
ViewDepth () = depth;

if (m_DIB == 0) {
	if (m_DC.m_hDC == 0)
		m_DC.CreateCompatibleDC (pDC);
	if (m_DC.m_hDC) {
		BITMAPINFO bmi = {{sizeof (BITMAPINFOHEADER), ViewWidth (), -ViewHeight (), 1, ViewDepth () * 8, BI_RGB, 0, 0, 0, 0, 0}, {255,255,255,0}};
	m_DIB = ::CreateDIBSection (null, (BITMAPINFO *) &bmi, DIB_RGB_COLORS, (void**) &m_renderData.m_renderBuffer, null, 0);
	if (DepthBuffer () != null) 
		delete DepthBuffer ();
	SetDepthBuffer (new depthType [ViewWidth () * ViewHeight ()]);
	}
}
// if DIB exists, then use our own DC instead of the View DC
if (m_DIB != 0) {
	m_pDC = &m_DC;
	m_DC.SelectObject (m_DIB);
	depthType* pBuffer = DepthBuffer ();
	if (pBuffer != null) {
		int h = ViewWidth () * ViewHeight ();
#pragma omp parallel for
		for (int i = 0; i < h; i++)
			pBuffer [i] = MAX_DEPTH;
		}
	bUpdate = 1;
	}
return bUpdate ? -1 : 1;
}

//------------------------------------------------------------------------------

void CRendererSW::ClearView (void)
{
if (m_DIB)
	memset (RenderBuffer (), 0, ViewWidth () * ViewHeight () * ViewDepth ());
else {
	CRect rect;
	m_pParent->GetClientRect (rect);
	FillRect (m_pDC->m_hDC, rect, (HBRUSH) GetStockObject (BLACK_BRUSH));
	}
}

//------------------------------------------------------------------------------

void CRendererSW::EndRender (bool bSwapBuffers) 
{
if (bSwapBuffers && (m_DIB != 0))
	m_viewDC->BitBlt (0,0, ViewWidth (), ViewHeight (), &m_DC, 0, 0, SRCCOPY);
}

//------------------------------------------------------------------------------

int CRendererSW::Project (CRect *pRC, bool bCheckBehind)
{
CHECKMINEV(0);

ViewMatrix ()->Translate (Translation ());
InitViewDimensions ();
ViewMatrix ()->SetViewInfo (ViewWidth (), ViewHeight ());

	bool	bBehind = false;
	int	nProjected = 0;
	int	j = vertexManager.Count ();

#pragma omp parallel for reduction(+: nProjected) if (j > 15)
for (int i = 0; i < j; i++) {
	CVertex& v = vertexManager [i];
	if (v.Status () == 255) // skybox inclusion/exclusion
		continue;
	v.Transform (ViewMatrix ());
	if (v.m_view.v.z < 0.0)
		bBehind = true;
	v.Project (ViewMatrix ());
	nProjected++;
	}
if (!nProjected)
	return 0;
if (bCheckBehind && bBehind)
	return -1;
ComputeViewLimits (pRC);
return nProjected;
}

//------------------------------------------------------------------------------

int CRendererSW::ZoomFactor (int nSteps, double min, double max)
{
double zoom;
int i;

for (zoom = log (10 * Scale ().v.x), i = 0; i < nSteps; i++) {
	zoom /= log (zoomScales [0]);
	if ((zoom < min) || (zoom > max))
		return i;
	}
return nSteps; //(int) ((zoom > 0) ? zoom + 0.5: zoom - 0.5);
}

//------------------------------------------------------------------------------

int CRendererSW::ZoomIn (int nSteps, bool bSlow)
{
if (nSteps = ZoomFactor (nSteps, -100, 25))
	Zoom (nSteps, zoomScales [0]);
else
	INFOMSG("Already at maximum zoom")
/*
		ErrorMsg ("Already at maximum zoom\n\n"
					"Hint: Try using the 'A' and 'Z' keys to\n"
					"move in forward and backwards.");
*/
return nSteps;
}

//------------------------------------------------------------------------------

int CRendererSW::ZoomOut (int nSteps, bool bSlow)
{
if (nSteps = ZoomFactor (nSteps, -5, 100))
	Zoom (nSteps, 1.0 / zoomScales [0]);
else
	INFOMSG("Already at minimum zoom")
return nSteps;
}

//------------------------------------------------------------------------------

void CRendererSW::SetCenter (CVertex v, int nType)
{
if (nType == 2) // object
	Translation () -= v;
else 
	Translation () = v;
}

//------------------------------------------------------------------------------

void CRendererSW::Pan (char direction, double offset)
{
if (offset == 0.0)
	return;
int i = direction - 'X';
if ((i < 0) || (i > 2))
	i = 1;
Translation () -= CDoubleVector (ViewMatrix ()->m_data [0].m_transformation [1].m.rVec [i], 
										   ViewMatrix ()->m_data [0].m_transformation [1].m.uVec [i], 
										   ViewMatrix ()->m_data [0].m_transformation [1].m.fVec [i]) * (ViewMoveRate () * offset);
}

//------------------------------------------------------------------------------

inline double sqr (long v) { return double (v) * double (v); }

inline double CRendererSW::ZRange (int x0, int x1, int y, double& z)
{
	CLongVector v, v0, v1, p0 = {x0, y}, p1 = {x1, y}, vi [4];
	int h = -1, j = 0;
	double zi [4];

v1 = m_screenCoord [0];
for (int i = 1; (i <= 4) && (j < 2); i++) {
	v0 = v1;
	v1 = m_screenCoord [i % 4];
	if (!LineLineIntersection (v, v0, v1, p0, p1))
		continue;
	if ((j > 0) && (v.x == vi [j - 1].x) && (v.y == vi [j - 1].y))
		continue;
#if 1
	if (v0.x < v1.x) {
		if ((v.x < v0.x) || (v.x > v1.x))
			continue;
		}
	else {
		if ((v.x < v1.x) || (v.x > v0.x))
			continue;
		}
	if (v0.y < v1.y) {
		if ((v.y < v0.y) || (v.y > v1.y))
			continue;
		}
	else {
		if ((v.y < v1.y) || (v.y > v0.y))
			continue;
		}
#else
	if (Dot (v0, v1) > 0.0)
		continue;
#endif
#if 0
	double l1 = _hypot (v0.x - v.x, v0.y - v.y);
	double l2 = _hypot (v1.x - v.x, v1.y - v.y);
#else
	double l1 = sqrt (sqr (v0.x - v.x) + sqr (v0.y - v.y));
	double l2 = sqrt (sqr (v1.x - v.x) + sqr (v1.y - v.y));
#endif
	vi [j] = v;
	zi [j++] = v0.z + (v1.z - v0.z) * l1 / (l1 + l2);
	}

if (j == 0) {
	z = MAX_DEPTH;
	return 0.0;
	}
if (j == 1) {
	z = zi [0];
	return 0.0;
	}
if (vi [0].x <= vi [1].x) {
	z = zi [0];
	return (zi [1] - zi [0]) / (double) (x1 - x0);
	}
else {
	z = zi [1];
	return (zi [0] - zi [1]) / (double) (x1 - x0);
	}	
}

//------------------------------------------------------------------------------

inline bool CRendererSW::Blend (CBGR& dest, const CBGRA& src, depthType& depth, depthType z, short brightness)
{
if (brightness == 0)
	return false;
if (src.a == 0)
	return false;

if (DepthTest ()) {
	if (depth <= z)
		return false;
#ifdef _DEBUG
	if (depth != MAX_DEPTH)
		z = z;
#endif
	if (!IgnoreDepth ())
		depth = z;
	}

int a = int (src.a * m_alpha);
if (brightness == 32767) {
	if (a == 255) {
		dest = src;
		return true;
		}

	int b = 255 - a;
	dest.r = (ubyte) (((int) dest.r * b + (int) src.r * a) / 255);
	dest.g = (ubyte) (((int) dest.g * b + (int) src.g * a) / 255);
	dest.b = (ubyte) (((int) dest.b * b + (int) src.b * a) / 255);
	return true;
	}

if (a == 255) {
	dest.r = (ubyte) ((int) src.r * brightness / 32767);
	dest.g = (ubyte) ((int) src.g * brightness / 32767);
	dest.b = (ubyte) ((int) src.b * brightness / 32767);
	return true;
	}

int b = 255 - a;
a *= brightness;
dest.r = (ubyte) (((int) dest.r * b + (int) src.r * a / 32767) / 255);
dest.g = (ubyte) (((int) dest.g * b + (int) src.g * a / 32767) / 255);
dest.b = (ubyte) (((int) dest.b * b + (int) src.b * a / 32767) / 255);
return true;
}

//------------------------------------------------------------------------------

void CRendererSW::RenderFace (CFaceListEntry& fle, const CTexture* pTexture, ushort rowOffset, CBGRA* pColor)
{
	int				i;
	CLongVector		minPt, maxPt;
	CDoubleMatrix	A, IA, B, UV;
	CUVL*				uvls;
	bool				bD2XLights = (DLE.LevelVersion () >= 15) && (theMine->Info ().fileInfo.version >= 34);
	CSegment*		pSegment = segmentManager.Segment (fle.m_nSegment);
	CSide&			side = *pSegment->Side (fle.m_nSide);
	ushort			light [4];
	ushort			bmWidth2;
	ubyte*				fadeTables = paletteManager.FadeTable ();
	bool				bTexColor = (pColor == null);
	double			scale = (double) max (pTexture->RenderWidth (), pTexture->RenderHeight ());
	const CBGRA*	colorBuf = pTexture->Buffer ();
	int				bIlluminate = RenderIllumination ();

bmWidth2 = pTexture->RenderWidth () / 2;
m_alpha = float (Alpha ()) / 255.0f;
// define 4 corners of texture to be displayed on the screen
for (i = 0; i < 4; i++) {
	ushort nVertex = pSegment->m_info.vertexIds [side.VertexIdIndex (i)];
	m_screenCoord [i] = vertexManager [nVertex].m_screen;
	}
// determin min/max points
minPt.x = minPt.y = 32767; // some number > any screen resolution
maxPt.x = maxPt.y = -32767;
for (i = 0; i < 4; i++) {
	long h = m_screenCoord [i].x;
	minPt.x = min (minPt.x, h);
	maxPt.x = max (maxPt.x, h);
	h = m_screenCoord [i].y;
	minPt.y = min (minPt.y, h);
	maxPt.y = max (maxPt.y, h);
	}

// clip min/max with screen min/max
minPt.x = max (minPt.x, 0);
maxPt.x = min (maxPt.x, ViewWidth () - 1);
minPt.y = max (minPt.y, 0);
maxPt.y = min (maxPt.y, ViewHeight () - 1);


// map unit square into texture coordinate
A.MapUnitSquareToQuad (m_screenCoord);

// calculate adjoint matrix (same as inverse)
IA = A.Adjoint ();

// store uv coordinates into m_texCoord []
// fill in texture
CLongVector m_texCoord [4];  // Descent's (u,v) coordinates for textures
uvls = pSegment->m_sides [fle.m_nSide].m_info.uvls;
for (i = 0; i < 4; i++) {
	m_texCoord [i].x = (int) Round (uvls [i].u * 2048.0);
	m_texCoord [i].y = (int) Round (uvls [i].v * 2048.0);
	}
	
ComputeBrightness (fle, light, RenderVariableLights ());
// map unit square into uv coordinates
// uv of 0x800 = 64 pixels in texture
// therefore uv of 32 = 1 pixel
UV.MapUnitSquareToQuad (m_texCoord);
//scale_matrix (UV, 1.0 / 2048.0);
UV.Scale (1.0 / 2048.0);
//multiply_matrix (B, IA, UV);
B = IA * UV;

#ifndef _DEBUG
#	pragma omp parallel for
#endif
for (int y = minPt.y; y < maxPt.y; y++) {
	int i;
	const CBGRA* pTexColor = pColor;
	short deltaLight, scanLight;
	// Determine min and max x for this y.
	// Check each of the four lines of the quadrilateral
	// to figure out the min and max x
	int x0 = maxPt.x; // start out w/ min point all the way to the right
	int x1 = minPt.x; // and max point to the left
	for (i = 0; i < 4; i++) {
		// if line intersects this y then update x0 & x1
		int j = (i + 1) % 4; // j = other point of line
		long yi = m_screenCoord [i].y;
		long yj = m_screenCoord [j].y;
		if ((y >= yi && y <= yj) || (y >= yj && y <= yi)) {
			double w = yi - yj;
			if (w != 0.0) { // avoid divide by zero
				double di = (double) (y - yi);
				double dj = (double) (y - yj);
				int x = (int) (((double) m_screenCoord [i].x * dj - (double) m_screenCoord [j].x * di) / w);
				if (x < x0) {
					scanLight = (int) (((double) light [i] * dj - (double) light [j] * di) / w);
					x0 = x;
					}
				if (x > x1) {
					deltaLight = (int) (((double) light [i] * dj - (double) light [j] * di) / w);
					x1 = x;
					}
				}
			}
		} // end for
	
	// clip
	x0 = max (x0, minPt.x);
	x1 = min (x1, maxPt.x);
	
	// Instead of finding every point using the matrix transformation,
	// just define the end points and delta values then simply
	// add the delta values to u and v
	if (abs (x0 - x1) > 0) {
		double u0, u1, v0, v1, w0, w1, h, x0d, x1d;
		uint u, v, du, dv, m, vd, vm, dx;
		deltaLight = (deltaLight - scanLight) / (x1 - x0);
		
		// loop for every 32 bytes
		for (int xEnd = x1; x0 < xEnd; x0 += bmWidth2) {
			if (xEnd - x0 > bmWidth2)
				x1 = bmWidth2 + x0;
			else
				x1 = xEnd;

			h = B.m.uVec.v.z * (double) y + B.m.fVec.v.z;
			x0d = (double) x0;
			x1d = (double) x1;
			w0 = (B.m.rVec.v.z * x0d + h) / scale; // scale factor (64 pixels = 1.0 unit)
			w1 = (B.m.rVec.v.z * x1d + h) / scale;
			if ((fabs (w0) > 0.0001) && (fabs (w1) > 0.0001)) {
				h = B.m.uVec.v.x * (double) y + B.m.fVec.v.x;
				u0 = (B.m.rVec.v.x * x0d + h) / w0;
				u1 = (B.m.rVec.v.x * x1d + h) / w1;
				h = B.m.uVec.v.y * (double) y + B.m.fVec.v.y;
				v0 = (B.m.rVec.v.y * x0d + h) / w0;
				v1 = (B.m.rVec.v.y * x1d + h) / w1;
				
				// use 22.10 integer math
				// the 22 allows for large texture bitmap sizes
				// the 10 gives more than enough accuracy for the delta values

				m = min (pTexture->RenderWidth (), pTexture->RenderHeight ());
				if (!m)
					m = 64;
				m *= 1024;
				dx = x1 - x0;
				if (!dx)
					dx = 1;
				du = ((uint) (((u1 - u0) * 1024.0) / dx) % m);
				// v0 & v1 swapped since pTexture->Buffer () is flipped
				dv = ((uint) (((v0 - v1) * 1024.0) / dx) % m);
				u = ((uint) (u0 * 1024.0)) % m;
				v = ((uint) (-v0 * 1024.0)) % m;
				vd = 1024 / pTexture->RenderHeight ();
				vm = pTexture->RenderWidth () * (pTexture->RenderHeight () - 1);
				
				i = (uint) y/*(ViewHeight () - y - 1)*/ * (uint) rowOffset + x0;
				double z, dz;
				if (DepthTest ())
					dz = ZRange (x0, x1, y, z);
				else
					z = dz = 0.0;
				
				if (bIlluminate) {
					for (int x = x0; x < x1; x++, i++) {
						// m_screenCoord fade value denotes the brightness of m_screenCoord color
						// scanLight / 4 is the index in the fadeTables which consists of 34 tables with 256 entries each
						// so for each color there are 34 fade (brightness) values ranges from 1/34 to 34/34
						// actually the fade tables contain palette indices denoting the dimmed color corresponding to the 
						// fade value
						// We don't need this anymore here since we're rendering RGB and can compute the brightness directly
						// from scanLight, the maximum of which is 8191
						// ubyte fade = fadeTables [j + ((scanLight / 4) & 0x1f00)];
#if 1
						if (bTexColor) {
							u += du;
							u %= m;
							v += dv;
							v %= m;
							pTexColor = &colorBuf [(u / 1024) + ((v / vd) & vm)];
							}
						Blend (RenderBuffer (i), *pTexColor, DepthBuffer (i), (depthType) z, scanLight);
						z += dz;
#else
						int t = (u / 1024) + ((v / vd) & vm);
						if (pTexture->Buffer () [t].m_screenCoord > 0) {
							CBGR c = pTexture->Buffer () [t];
							CBGR* pixel = m_renderBuffer + i;
							pPixel->r = (ubyte) ((int) (c.r) * fade / 8191);
							pPixel->g = (ubyte) ((int) (c.g) * fade / 8191);
							pPixel->b = (ubyte) ((int) (c.b) * fade / 8191);
							pPixel->m_texCoord = (ubyte) ((int) (c.m_texCoord) * fade / 8191);
							}
#endif
						scanLight += deltaLight;
						} 
					}
				else {
					for (int x = x0; x < x1; x++, i++) {
						if (bTexColor) {
							u += du;
							u %= m;
							v += dv;
							v %= m;
							pTexColor = &colorBuf [(u / 1024) + ((v / vd) & vm)];
							}
#if 1
						Blend (RenderBuffer (i), *pTexColor, DepthBuffer (i), (depthType) z);
						z += dz;
#else
						int t = (u / 1024) + ((v / vd) & vm);
						if (pTexture->Buffer () [t].m_screenCoord > 0)
							m_renderBuffer [i] = pTexture->Buffer () [t];
#endif
						}
					}
				}
			} // end of 32 ubyte loop
		}
	}
}

//----------------------------------------------------------------------------

int CRendererSW::FaceIsVisible (CSegment* pSegment, CSide* pSide)
{
if (4 < 4)
	return 1;
CLongVector& p0 = vertexManager [pSegment->m_info.vertexIds [pSide->VertexIdIndex (0)]].m_screen;
CLongVector& p1 = vertexManager [pSegment->m_info.vertexIds [pSide->m_vertexIdIndex [1]]].m_screen;
CLongVector& p3 = vertexManager [pSegment->m_info.vertexIds [pSide->m_vertexIdIndex [pSide->VertexCount () - 1]]].m_screen;

CDoubleVector	a ((double) (p1.x - p0.x), (double) (p1.y - p0.y), 0.0), 
					b ((double) (p3.x - p0.x), (double) (p3.y - p0.y), 0.0);
return (a.v.x * b.v.y > a.v.y * b.v.x);
}

//--------------------------------------------------------------------------

void CRendererSW::DrawFaceTextured (CFaceListEntry& fle) 
{
	const CTexture* pTexture [2] = {null, null};

	CTexture		tex (textureManager.SharedBuffer ());
	CSegment*	pSegment = segmentManager.Segment (fle);
	CSide*		pSide = segmentManager.Side (fle);
	CWall*		pWall = segmentManager.Wall (fle);
	CBGRA			color, * pColor = null;

#ifdef _DEBUG
if ((fle.m_nSegment == nDbgSeg) && ((nDbgSide < 0) || (fle.m_nSide == nDbgSide)))
	nDbgSeg = nDbgSeg;
#endif

SetAlpha (255);

short nBaseTex = pSide->BaseTex ();
short nOvlTex = pSide->OvlTex (1);
if (((nOvlTex & TEXTURE_MASK) != 0) && !pSide->OvlAlignment () && !textureManager.Textures (nOvlTex)->Transparent ()) {
	nBaseTex = nOvlTex;
	nOvlTex = 0;
	}

if (textureManager.IsScrolling (pSide->BaseTex ())) {
	pTexture [0] = &tex;
	tex.BlendTextures (pSide->BaseTex (), pSide->OvlTex (), 0, 0);
	tex.DrawAnimDirArrows (pSide->BaseTex ());
	}
else if (nOvlTex) {
	pTexture [0] = &tex;
	tex.BlendTextures (nBaseTex, nOvlTex, 0, 0);
	}
else {
	pTexture [0] = textureManager.Textures (nBaseTex);
	if (pWall != null) {
		m_alpha = pWall->Alpha ();
		if (pWall->IsTransparent () || pWall->IsCloaked ()) {
			CBGRA color;
			if (pWall->IsCloaked ())
				color = CBGRA (0, 0, 0, 255);
			else {
				CColor* pTexColor = lightManager.GetTexColor (pSide->BaseTex (), true);
				color = CBGRA (pTexColor->Red (), pTexColor->Green (), pTexColor->Blue (), 255);
				}
			pColor = &color;
			}
		}
	}

RenderFace (fle, pTexture [0], (ViewWidth () + 3) & ~3, pColor);
}

//------------------------------------------------------------------------------

void CRendererSW::RenderFaces (CFaceListEntry* faceRenderList, int faceCount, int bRenderSideKeys)
{
BeginRender ();
if (DepthTest ()) {
	SetIgnoreDepth (false);
	for (int nFace = faceCount - 1; nFace >= 0; nFace--)
		if (!faceRenderList [nFace].m_bTransparent)
	 		DrawFaceTextured (faceRenderList [nFace]);
	SetIgnoreDepth (true);
	for (int nFace = 0; nFace < faceCount; nFace++)
		if (faceRenderList [nFace].m_bTransparent)
	 		DrawFaceTextured (faceRenderList [nFace]);
	}
else {
	for (int nFace = 0; nFace < faceCount; nFace++)
 		DrawFaceTextured (faceRenderList [nFace]);
	}
EndRender ();
}

//------------------------------------------------------------------------------

void CRendererSW::SelectPen (int nPen, float nWeight)
{
if (nPen > 0)
	SelectObject (Pen (nPen - 1, int (nWeight)));
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
