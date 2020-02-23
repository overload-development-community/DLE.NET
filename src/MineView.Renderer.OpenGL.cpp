// render.cpp

#include "stdafx.h"
#include <math.h>

#include "define.h"
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

static int bInitSinCosTable = 1;

typedef struct tSinCosf {
	float	fSin, fCos;
} tSinCosf;

void ComputeSinCosTable (tSinCosf *pSinCos, int nPoints)
{
for (int i = 0; i < nPoints; i++, pSinCos++) {
	double a = 2.0 * PI * i / nPoints;
	pSinCos->fSin = (float) sin (a);
	pSinCos->fCos = (float) cos (a);
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

CRendererGL::CRendererGL (CRenderData& renderData) : CRenderer (renderData), m_x (0), m_y (0)
{
m_glRC = null;
m_glDC = null;
m_sideKeys = null;
}

// -----------------------------------------------------------------------------

CRendererGL::~CRendererGL ()
{
if (m_sideKeys) {
	delete m_sideKeys;
	m_sideKeys = null;
	}
}

// -----------------------------------------------------------------------------

int CRendererGL::Setup (CWnd* pParent, CDC* m_pDC)	
{
if (!CRenderer::Setup (pParent, m_pDC))
	return 0;
if (!CreateContext ())
	return 0;
SetRTT (m_bHaveRTT);

InitProjection ();

m_pParent->GetClientRect (Viewport ());
if (!(Viewport ().Width () && Viewport ().Height ()))
	return 0;
glViewport (Viewport ().left, Viewport ().top, Viewport ().Width (), Viewport ().Height ());
m_glAspectRatio = (GLfloat) Viewport ().Width () / (GLfloat) Viewport ().Height ();
SetupProjection ();
textureManager.CreateGLTextures ();
return -1;
}

//------------------------------------------------------------------------------

void CRendererGL::Reset (void)
{
CRenderer::Reset ();
m_vZoom.Clear ();
m_viewMatrix.Origin ().Clear ();
if (Perspective ()) {
	Rotation ().Clear ();
	Translation () = -current->Segment ()->Center ();
	}
ViewMatrix ()->Setup (Translation (), Scale (), Rotation ());
}

//------------------------------------------------------------------------------

void CRendererGL::TranslateCameraPosition (int nNewPerspective)
{
	CDoubleVector newTranslation;
	CDoubleVector newOrigin;

if (nNewPerspective) {
	// First-person - translation is a camera position
	newTranslation = -m_viewMatrix.Translation ();
	newTranslation = m_viewMatrix.Transformation (1) * newTranslation;
	newTranslation += m_viewMatrix.Origin ();

	// We want to keep the origin in the same place relative to the camera
	// while in first-person mode. Since the origin isn't actually updated
	// in that state we "store" it as its transformation in view space so we
	// can untransform it to a new location when we switch back
	newOrigin = m_viewMatrix.Origin ();
	newOrigin -= newTranslation;
	newOrigin = m_viewMatrix.Transformation (0) * newOrigin;
	}
else {
	// Third-person - translation is the view distance from the origin
	// First restore the origin vector
	newOrigin = m_viewMatrix.Origin ();
	newOrigin = m_viewMatrix.Transformation (1) * newOrigin;
	newOrigin += m_viewMatrix.Translation ();

	// Now determine the translation versus that vector
	newTranslation = m_viewMatrix.Translation () - newOrigin;
	newTranslation = m_viewMatrix.Transformation (0) * newTranslation;
	newTranslation = -newTranslation;
	}

ViewMatrix ()->Translation () = Translation () = newTranslation;
ViewMatrix ()->Origin () = newOrigin;
}

//------------------------------------------------------------------------------

void CRendererGL::ClearView (void)
{
if (RTT ())
	m_renderBuffers.Enable (-1);
glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
if (RTT ())
	m_renderBuffers.Disable ();
}

// -----------------------------------------------------------------------------

GLvoid CRendererGL::DestroyContext (void)
{
if (m_glRC)	{
	if (!wglMakeCurrent (null, null))
		ErrorMsg ("OpenGL: Release of DC and RC failed.");
	if (!wglDeleteContext(m_glRC))
		ErrorMsg ("OpenGL: Release of rendering context failed.");
		m_glRC = null;
	}
if (m_glDC && !::ReleaseDC (m_pParent->m_hWnd, m_glHDC))	 {
	//ErrorMsg ("OpenGL: Release of device context failed.")
		;
	m_glDC = null;	
	}
#if 0
if (!UnregisterClass ("OpenGL",AfxGetInstance ())) {
	ErrorMsg ("OpenGL: Could Not Unregister Class.");
	}
#endif
}

// -----------------------------------------------------------------------------

BOOL CRendererGL::CreateContext (void)
{
if (m_glDC)
	return TRUE;

	GLuint PixelFormat;

static PIXELFORMATDESCRIPTOR pfd = {
	sizeof (PIXELFORMATDESCRIPTOR),
	1,	
	PFD_DRAW_TO_WINDOW |	PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
	PFD_TYPE_RGBA,
	32,
	0, 0, 0, 0, 0, 0,
	0,	
	0,
	0,
	0, 0, 0, 0,
	32,
	0,
	0,
	PFD_MAIN_PLANE,
	0,
	0, 0, 0
	};

if (!(m_glDC = m_pParent->GetDC ())) {
	DestroyContext ();
	ErrorMsg ("OpenGL: Can't create device context.");
	return FALSE;
	}
m_glHDC = m_glDC->GetSafeHdc ();
if (!(PixelFormat = ChoosePixelFormat (m_glHDC, &pfd))) {
	DestroyContext ();
	sprintf_s (message, sizeof (message), "OpenGL: Can't find a suitable pixel format. (%d)", GetLastError ());
	ErrorMsg (message);
	return FALSE;
	}
if (!SetPixelFormat (m_glHDC, PixelFormat, &pfd)) {
		DestroyContext();
		sprintf_s (message, sizeof (message), "OpenGL: Can't set the pixel format (%d).", GetLastError ());
		ErrorMsg (message);
		return FALSE;
	}
if (!(m_glRC = wglCreateContext (m_glHDC))) {
	DestroyContext ();
	sprintf_s (message, sizeof (message), "OpenGL: Can't create a rendering context (%d).", GetLastError ());
	ErrorMsg (message);
	return FALSE;
	}

if (!wglMakeCurrent (m_glHDC, m_glRC)) {
	DestroyContext ();
	sprintf_s (message, sizeof (message), "OpenGL: Can't activate the rendering context (%d).", GetLastError ());
	ErrorMsg (message);
	return FALSE;
	}

glewInit (); // must happen after OpenGL context creation!
shaderManager.Setup ();
textureManager.InitShaders ();
if (m_bHaveRTT = CFBO::Setup ()) {
	m_bHaveRTT = m_renderBuffers.Create (GetSystemMetrics (SM_CXSCREEN), GetSystemMetrics (SM_CYSCREEN), 1, 2) != 0;
	m_sideKeys = new rgbColor [GetSystemMetrics (SM_CXSCREEN) * GetSystemMetrics (SM_CYSCREEN)];
	}
return TRUE;
}

// -----------------------------------------------------------------------------

BOOL CRendererGL::InitProjection (GLvoid)
{
glShadeModel (GL_SMOOTH);
glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
glClearDepth (1.0f);
glEnable (GL_BLEND);
glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
glEnable (GL_DEPTH_TEST);
glDepthFunc (GL_LESS);
glEnable (GL_ALPHA_TEST);
glAlphaFunc (GL_GEQUAL, 0.0025f);	
glEnable (GL_CULL_FACE);
glCullFace (GL_FRONT);
glEnable (GL_LINE_SMOOTH);
glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
glDrawBuffer (GL_BACK);
m_glInited = false;
return TRUE;
}

// -----------------------------------------------------------------------------

static double viewAngleTable [4] = {30.0, 40.0, 50.0, 60.0};

void CRendererGL::SetupProjection (bool bOrtho) 
{
if (m_bOrtho = bOrtho) {
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glOrtho (0.0, ViewWidth (), 0.0, ViewHeight (), 0.0, 5000.0);
	glViewport (0, 0, ViewWidth (), ViewHeight ());
	ViewMatrix ()->GetProjection ();
	}
else {
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
#if 0
	gluPerspective (45.0f /** m_glAspectRatio*/, m_glAspectRatio, 1.0f, 5000.0f);
#else
#	if 0
	double h = 1.0f * tan (Radians (180.0 * m_glAspectRatio * 0.5));
	double w = h * m_glAspectRatio;
	glFrustum (w, -w, h, -h, 1.0, 5000.0);
#	else
	double fov = Perspective () ? 30.0 : viewAngleTable [m_viewMatrix.DepthPerception ()];
	double h = 1.0f * tan (Radians (fov));
	double w = h * m_glAspectRatio;
	glFrustum (-w, w, h, -h, 1.0, 50000.0);
	m_frustum.Setup (Viewport (), fov);
#	endif
#endif
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glScalef (1.0f, -1.0f, -1.0f);
	ViewMatrix ()->GetProjection ();
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_POLYGON_OFFSET_FILL);
	glPolygonOffset (1.0f, 1.0f);
	}
}

//------------------------------------------------------------------------------

int CRendererGL::Project (CRect* pRC, bool bCheckBehind)
{
CHECKMINEV(0);

	CViewMatrix* viewMatrix = ViewMatrix ();
	int	i, j, nProjected;
	bool	bBehind = false;

viewMatrix->Translate (Translation ());
InitViewDimensions ();
viewMatrix->SetViewInfo (ViewWidth (), ViewHeight ());

nProjected = 0;
j = vertexManager.Count ();
#ifdef NDEBUG
#pragma omp parallel for reduction(+: nProjected)
#endif
for (i = 0; i < j; i++) {
#ifdef _DEBUG
	if (i == nDbgVertex)
		nDbgVertex = nDbgVertex;
#endif
	CVertex& v = vertexManager [i];
	if (v.Status () == 255) // skybox inclusion/exclusion
		continue;
	v.Transform (viewMatrix);
	if (v.m_view.v.z < 0.0)
		bBehind = true;
	++nProjected;
	}

if (!nProjected)
	return 0;
if (bCheckBehind && bBehind)
	return -1;

for (i = 0; i < j; i++) {
	CVertex& v = vertexManager [i];
	if (v.Status () != 255)
		v.Project (viewMatrix);
	}
ComputeViewLimits (pRC);
return nProjected;
}

//--------------------------------------------------------------------------

void CRendererGL::BeginRender (bool bOrtho)
{
SetupProjection (bOrtho);
if (RTT ())
	m_renderBuffers.Enable (0);
}

//--------------------------------------------------------------------------

void CRendererGL::EndRender (bool bSwapBuffers)
{
for (int i = 0; i < 3; i++) {
	glActiveTexture (GL_TEXTURE0 + i);
	glClientActiveTexture (GL_TEXTURE0 + i);
	glDisableClientState (GL_COLOR_ARRAY);
	glDisableClientState (GL_TEXTURE_COORD_ARRAY);
	glDisableClientState (GL_VERTEX_ARRAY);
	glBindTexture (GL_TEXTURE_2D, 0);
	glDisable (GL_TEXTURE_2D);
	}
shaderManager.Deploy (-1);
glDisable (GL_DEPTH_TEST);
glDisable (GL_LINE_STIPPLE);
glDisable (GL_POLYGON_OFFSET_FILL);
glDepthFunc (GL_LESS);
if (bSwapBuffers) {
	if (RTT ()) {
		m_renderBuffers.Disable ();
		m_renderBuffers.Flush (Viewport ());
		}
	SwapBuffers (m_glHDC);
	}
}

//------------------------------------------------------------------------------

int CRendererGL::ZoomIn (int nSteps, bool bSlow)
{
Zoom (nSteps, zoomScales [1]);
return nSteps;
}

//------------------------------------------------------------------------------

int CRendererGL::ZoomOut (int nSteps, bool bSlow)
{
Zoom (nSteps, 1.0 / zoomScales [1]);
return nSteps;
}

//------------------------------------------------------------------------------

void CRendererGL::ComputeZoom (void)
{
if (!Perspective ()) {
	double m = Translation ().Mag ();
	m_vZoom [0] = log10 (m + 1);
	}
}

//------------------------------------------------------------------------------

void CRendererGL::Zoom (int nSteps, double zoom)
{
if (Perspective ()) {
	Pan ('Z', nSteps * ((zoom < 1.0) ? 1 : -1));
	}
else {
	if (nSteps < 0) {
		if (zoom > 1.0)
			Translation ().v.z += double (-nSteps);
		else
			Translation ().v.z -= double (-nSteps);
		}
	else {
		ComputeZoom ();
		m_vZoom [0] -= (zoom - 1.0) * double (nSteps);
		m_vZoom [0] = max (m_vZoom [0], 0);
#if 1
		Translation ().v.z = pow (10.0, m_vZoom [0]) - 1.0;
#else
		if ((m_vZoom [0] < 0) != (zoom < 1.0))
			zoom = 1.0 / zoom;
		double z = ((zoom < 1.0) && (m_vZoom [0] < 0)) 
					  ? -(100.0 + pow (1.0 / zoom, fabs (m_vZoom [0])) * ViewMoveRate ())
					  : (100.0 + pow (zoom, fabs (m_vZoom [0])) * ViewMoveRate ());
		Translation ().v.z = z;
#endif
		}
	}
//CRenderer::Zoom (nSteps, zoom);
}

//------------------------------------------------------------------------------

void CRendererGL::SetCenter (CVertex v, int nType)
{
#if 1
#	if 0
Translation ().v.x = v.v.x;
Translation ().v.y = v.v.y;
#	else
if (Perspective ()) {
	Translation () = v;
	m_viewMatrix.Translation () = v;
	}
else {
	m_viewMatrix.Origin () = v;
	Translation ().v.x = 
	Translation ().v.y = 0.0;
	ComputeZoom ();
	}
#	endif
#else
if (nType == 0) { // mine
	Translation () = v;
	m_vZoom.Clear ();
	}
else { // segment or object
	Translation ().v.x = -v.v.x;
	Translation ().v.y = -v.v.y;
	}
ComputeZoom ();
#endif
}

//------------------------------------------------------------------------------

void CRendererGL::Pan (char axis, double offset)
{
if (offset == 0.0)
	return;

if (Perspective ()) {
	CVertex vMove;
	switch (axis) {
		case 'X':
			vMove = ViewMatrix ()->Right ();
			break;
		case 'Y':
			vMove = ViewMatrix ()->Up ();
			break;
		case 'Z':
			vMove = ViewMatrix ()->Backward ();
			break;
		default:
			return;
		}
	vMove *= offset * ViewMoveRate ();
	Translation () += vMove;
	}
else {
	int i = axis -= 'X';
	if ((i < 0) || (i > 2))
		return;
#if 1
	double d = ViewMoveRate () * ((i == 1) ? offset : -offset);
	CVertex o = m_viewMatrix.Origin ();
	o.m_view = m_viewMatrix.Transformation () * o;
	o.m_view [i] -= d;
	o = m_viewMatrix.Transformation (1) * o.m_view;
	m_viewMatrix.Origin () = o;
#else
m_viewMatrix.MoveViewer (i, ViewMoveRate () * ((i == 1) ? offset : -offset));
#endif
	}
}

//------------------------------------------------------------------------------

void CRendererGL::DrawFaceTextured (CFaceListEntry& fle) 
{
	CTexture		tex (textureManager.SharedBuffer ());
	const CTexture* pTexture [3] = {null, null, null};
	CSegment*	pSegment = segmentManager.Segment (fle);
	CSide*		pSide = segmentManager.Side (fle);
	CWall*		pWall = segmentManager.Wall (fle);
	CBGRA			color, * pColor = null;
	int			bArrow = 0;

#ifdef _DEBUG
if ((fle.m_nSegment == nDbgSeg) && ((nDbgSide < 0) || (fle.m_nSide == nDbgSide)))
	nDbgSeg = nDbgSeg;
#endif

SetAlpha (255);
short nBaseTex = pSide->BaseTex ();
short nOvlTex = pSide->OvlTex (0);

if (textureManager.IsScrolling (pSide->BaseTex ()) && !(bArrow = textureManager.HaveArrow ())) {
	pTexture [0] = &tex;
	tex.BlendTextures (nBaseTex, nOvlTex, 0, 0);
	tex.DrawAnimDirArrows (nBaseTex);
	tex.GLCreate ();
	}
else {
	pTexture [0] = textureManager.Textures (nBaseTex);
	//pTexture [0]->m_nTexture = nBaseTex;
	if (nOvlTex) {
		pTexture [1] = textureManager.Textures (nOvlTex);
		//pTexture [1]->m_nTexture = nOvlTex;
		}
	if (bArrow)
		pTexture [2] = &textureManager.Arrow ();
	}
if (pWall != null) {
	SetAlpha (pWall->Alpha ());
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

RenderFace (fle, pTexture, pColor);
if	(pTexture [0] == &tex)
	tex.GLRelease ();
}

//------------------------------------------------------------------------------

static inline void RotateTexCoord2d (tTexCoord2d& dest, tTexCoord2d& src, short nOrient)
{
if (nOrient == 3) {
	dest.u = 1.0f - src.v;
	dest.v = src.u;
	}
else if (nOrient == 2) {
	dest.u = 1.0f - src.u;
	dest.v = 1.0f - src.v;
	}
else if (nOrient == 1) {
	dest.u = src.v;
	dest.v = 1.0f - src.u;
	}
else {
	dest.u = src.u;
	dest.v = src.v;
	}
}

//------------------------------------------------------------------------------

#define GL_TRANSFORM 0

void CRendererGL::RenderFace (CFaceListEntry& fle, const CTexture* pTexture [], CBGRA* pColor)
{
	static float zoom = 10.0f;
	static double scrollAngles [8] = {0.0, Radians (45.0), Radians (90.0), Radians (135.0), Radians (180.0), Radians (-135.0), Radians (-90.0), Radians (-45.0)};

	CSegment*		pSegment = segmentManager.Segment (fle.m_nSegment);
	CSide*			pSide = pSegment->Side (fle.m_nSide);
	CWall*			pWall = pSide->Wall ();
	float				alpha = float (Alpha ()) / 255.0f * (pColor ? float (pColor->a) / 255.0f : 1.0f);
	int				bIlluminate = RenderIllumination () && (pSegment->m_info.function != SEGMENT_FUNC_SKYBOX);
	ushort*			vertexIds = pSegment->m_info.vertexIds;
	ubyte*			vertexIdIndex = pSide->m_vertexIdIndex;
	int				nVertices = pSide->VertexCount (), 
						nTextures = pColor ? 0 : pTexture [1] ? 2 : 1;
	int				bArrow = pTexture [2] != null;
	double			scrollAngle = 0.0;
	short				nOvlAlignment = pSide->OvlAlignment ();
	ushort			brightness [4];
#if 1
	float				heightScale = 1.0f;
#else
	float				heightScale = 
							(nTextures == 1)
							? (float) pTexture [0]->RenderWidth () / (float) pTexture [0]->RenderHeight ()
							: (nTextures == 2)
								? MinVal ((float) pTexture [0]->RenderWidth () / (float) pTexture [0]->RenderHeight (), (float) pTexture [1]->RenderWidth () / (float) pTexture [1]->RenderHeight ())
								: 1.0f;
#endif

	tDoubleVector	vertices [4];
	tTexCoord2d		texCoords [3][4];
	rgbaColorf		colors [4];

#ifdef _DEBUG
if ((fle.m_nSegment == nDbgSeg) && ((nDbgSide < 0) || (fle.m_nSide == nDbgSide)))
	nDbgSeg = nDbgSeg;
#endif

if (bArrow) {
	int nDir = textureManager.ScrollDirection (pTexture [0]->Id ());
	if (nDir >= 0)
		scrollAngle = scrollAngles [nDir];
	}

ComputeBrightness (fle, brightness, RenderVariableLights ());

for (int i = 0, j = nOvlAlignment; i < nVertices; i++, j = (j + 1) % nVertices) {
	int nVertex = vertexIds [vertexIdIndex [i]];
#if GL_TRANSFORM
	vertices [i] = vertexManager [nVertex].v;
#else
	vertices [i] = vertexManager [nVertex].m_view.v;
#endif
	CUVL uvl = pSide->m_info.uvls [i];
	if (!pColor) {
		texCoords [0][i].u = uvl.u;
		texCoords [0][i].v = -uvl.v * heightScale;
		if (nTextures > 1)
			RotateTexCoord2d (texCoords [1][i], texCoords [0][i], nOvlAlignment);
		if (bArrow) {
			if (scrollAngle != 0.0)
				uvl.Rotate (scrollAngle);
			texCoords [nTextures][i].u = uvl.u;
			texCoords [nTextures][i].v = uvl.v - 1.0;
			}
		if (bIlluminate) {
			float b = 2.0f * X2F (brightness [i]);
			CVertexColor* vertexColor = lightManager.VertexColor (nVertex);
			colors [i].r = float (vertexColor->m_info.color.r) * b;
			colors [i].g = float (vertexColor->m_info.color.g) * b;
			colors [i].b = float (vertexColor->m_info.color.b) * b;
			}
		else
			colors [i].r = 
			colors [i].g = 
			colors [i].b = 1.0f;
		colors [i].a = pSegment->m_info.bTunnel ? alpha * 0.7f : alpha;
		}
	}

if (pColor) {
	glDisable (GL_TEXTURE_2D);
	glColor4f (float (pColor->r) / 255.0f, float (pColor->g) / 255.0f, float (pColor->b) / 255.0f, alpha);
	glDisableClientState (GL_COLOR_ARRAY);
	glDisableClientState (GL_TEXTURE_COORD_ARRAY);
	glEnableClientState (GL_VERTEX_ARRAY);
	glVertexPointer (3, GL_DOUBLE, 0, vertices);
	}
else {
	int h = 0;
	for (int i = 0; i < 3; i++) {
		if (!pTexture [i]) 
			continue;
		glActiveTexture (GL_TEXTURE0 + h);
		glClientActiveTexture (GL_TEXTURE0 + h);
		glEnable (GL_TEXTURE_2D);
		if (!pTexture [i]->GLBind (GL_TEXTURE0 + h, GL_MODULATE))
			return;
		glEnableClientState (GL_COLOR_ARRAY);
		glEnableClientState (GL_TEXTURE_COORD_ARRAY);
		glEnableClientState (GL_VERTEX_ARRAY);
		glColorPointer (4, GL_FLOAT, 0, colors);
		glTexCoordPointer (2, GL_DOUBLE, 0, texCoords [h]);
		glVertexPointer (3, GL_DOUBLE, 0, vertices);
		h++;
		}
	for (; h < 3; h++) {
		glActiveTexture (GL_TEXTURE0 + h);
		glClientActiveTexture (GL_TEXTURE0 + h);
		glBindTexture (GL_TEXTURE_2D, 0);
		glDisable (GL_TEXTURE_2D);
		}
	}

#ifdef _DEBUG
if ((fle.m_nSegment == nDbgSeg) && ((nDbgSide < 0) || (fle.m_nSide == nDbgSide)))
	nDbgSeg = nDbgSeg;
#endif

textureManager.DeployShader ((bArrow ? nTextures + bArrow : nTextures - 1), m_bRenderSideKeys ? &fle : null);

#if GL_TRANSFORM
m_viewMatrix.SetupOpenGL ();
glDrawArrays ((nVertices == 3) ? GL_TRIANGLES : GL_TRIANGLE_FAN, 0, GLsizei (nVertices));
m_viewMatrix.ResetOpenGL ();
#else
glDrawArrays ((nVertices == 3) ? GL_TRIANGLES : GL_TRIANGLE_FAN, 0, GLsizei (nVertices));
#endif
}

//------------------------------------------------------------------------------

void CRendererGL::RenderFaces (CFaceListEntry* faceRenderList, int faceCount, int bRenderSideKeys)
{
BeginRender ();
if ((m_bRenderSideKeys = bRenderSideKeys))
	m_renderBuffers.SelectColorBuffers (-1);
for (int nFace = faceCount - 1; nFace >= 0; nFace--)
	if (!faceRenderList [nFace].m_bTransparent)
 		DrawFaceTextured (faceRenderList [nFace]);
for (int nFace = 0; nFace < faceCount; nFace++)
	if (faceRenderList [nFace].m_bTransparent)
 		DrawFaceTextured (faceRenderList [nFace]);
EndRender ();
if (m_bRenderSideKeys = bRenderSideKeys)
	m_renderBuffers.SelectColorBuffers (0);
m_bHaveSideKeys = false;
}

//------------------------------------------------------------------------------

void CRendererGL::SelectPen (int nPen, float nWeight)
{
if (nPen > 0) {
	m_renderData.m_pen = ePenColor (nPen - 1);
	m_renderData.m_penWeight = nWeight;
	COLORREF color = PenColor (ePenColor (nPen - 1));
	glColor3f (float (GetRValue (color)) / 255.0f, float (GetGValue (color)) / 255.0f, float (GetBValue (color)) / 255.0f);
	glLineWidth (float (nWeight) /** 1.5f*/);
	}
}

//------------------------------------------------------------------------------

void CRendererGL::MoveTo (int x, int y)
{
m_x = x;
m_y = y;
}

//------------------------------------------------------------------------------

void CRendererGL::MoveTo (CVertex& v)
{
m_v = v;
}

//------------------------------------------------------------------------------
// This crap should work, but it doesn't ... why the heck doesn't it?

static inline double sqr (long v) { return double (v) * double (v); }

bool CRendererGL::Clip (CLongVector* line)
{
	int w = ViewWidth ();
	int h = ViewHeight ();
	int bClip [2];
	int nClip = 0;

for (int i = 0; i < 2; i++) 
	if ((bClip [i] = (line [i].x < 0) || (line [i].x >= w) || (line [i].y < 0) || (line [i].y >= h)))
		++nClip;

if (!nClip)
	return true;

double m = double (line [1].y - line [0].y) / double (line [1].x - line [0].x);
double b = double (line [0].y) - m * double (line [0].x);

CDoubleVector v1 (line [0].x, line [0].y, 0.0);
CDoubleVector v2 (line [1].x, line [1].y, 0.0);

CDoubleVector pi [4];
pi [0].v.x = 0;
pi [0].v.y = b;
pi [1].v.y = 0;
pi [1].v.x = -b / m;
pi [2].v.x = double (w) - 1;
pi [2].v.y = b + m * pi [2].v.x;
pi [3].v.y = double (h) - 1;
pi [3].v.x = (pi [3].v.y - b) / m;
for (int i = 0, j = !bClip [0]; (i < 4) && nClip; i++) 
	if ((pi [i].v.x >= 0.0) && (pi [i].v.x < w) && (pi [i].v.y >= 0.0) && (pi [i].v.y < h)) {
		CDoubleVector dv1, dv2;
		dv1 = pi [i];
		dv2 = dv1;
		dv1 -= v1;
		dv2 -= v2;
		// check whether the new point lies on the edge by computing the angle (dot product)
		// of the vectors from the new point to each of the edge's current points
		// these vectors must point away from each other -> dot < 0
		if (Dot (dv1, dv2) < 0.0) {
			line [j].x = long (Round (pi [i].v.x));
			line [j++].y = long (Round (pi [i].v.y));
			--nClip;
			}
		}
return nClip == 0;
}

//------------------------------------------------------------------------------

void CRendererGL::LineTo (int x, int y)
{
CLongVector line [2];
line [0].x = m_x;
line [0].y = m_y;
line [1].x = x;
line [1].y = y;
if (Clip (line)) {
	glBegin (GL_LINES);
	glVertex2i (line [0].x, line [0].y);
	glVertex2i (line [1].x, line [1].y);
	glEnd ();
	}
m_x = x, m_y = y;
}

//------------------------------------------------------------------------------

void CRendererGL::LineTo (CVertex& v)
{
glBegin (GL_LINES);
glVertex3dv ((GLdouble*) &m_v.m_view.v);
m_v = v;
glVertex3dv ((GLdouble*) &m_v.m_view.v);
glEnd ();
}

//------------------------------------------------------------------------------

void CRendererGL::Rectangle (int left, int top, int right, int bottom)
{
glBegin (GL_LINE_LOOP);
glVertex2i (left, top);
glVertex2i (right, top);
glVertex2i (right, bottom);
glVertex2i (left, bottom);
glEnd ();
}

//------------------------------------------------------------------------------

void CRendererGL::PolyLine (CPoint* points, int nPoints)
{
glBegin (GL_LINE_STRIP);
for (; nPoints; nPoints--, points++)
	glVertex2i (points->x, points->y);
glEnd ();
}

//------------------------------------------------------------------------------

void CRendererGL::PolyLine (CVertex* points, int nPoints)
{
#if 0//def _DEBUG
MoveTo (*points);
for (--nPoints; nPoints; nPoints--)
	LineTo (*(--points));
#else
glBegin (GL_LINE_STRIP);
for (; nPoints; nPoints--, points++)
	glVertex3dv ((GLdouble*) &points->m_view.v);
glEnd ();
#endif
}

//------------------------------------------------------------------------------

void CRendererGL::Polygon (CPoint* points, int nPoints)
{
glBegin (GL_LINE_LOOP);
for (; nPoints; nPoints--, points++)
	glVertex2i (points->x, points->y);
glEnd ();
}

//------------------------------------------------------------------------------

void CRendererGL::Polygon (CVertex* vertices, int nVertices)
{
glBegin (GL_LINE_LOOP);
for (; nVertices; nVertices--, vertices++)
	glVertex3dv ((GLdouble*) &vertices->m_view.v);
glEnd ();
}

//------------------------------------------------------------------------------

void CRendererGL::Polygon (CVertex* vertices, int nVertices, ushort* index)
{
glBegin (GL_LINE_LOOP);
for (int i = 0; i < nVertices; i++)
	glVertex3dv ((GLdouble*) &(vertices [index ? index [i] : i].m_view.v));
glEnd ();
}

//------------------------------------------------------------------------------

void CRendererGL::TexturedPolygon (const CTexture* pTexture, tTexCoord2d* texCoords, rgbColord* color, CVertex* vertices, int nVertices, ushort* index)
{
	static tTexCoord2d defaultTexCoords [4] = {{0.0, 0.0}, {0.0, 1.0}, {1.0, 1.0}, {1.0, 0.0}};

if (!pTexture) {
	glDisable (GL_TEXTURE_2D);
	glColor3dv ((GLdouble*) color);
	}
else {
	pTexture->GLBind (GL_TEXTURE0, GL_MODULATE);
	glColor3f (1.0f, 1.0f, 1.0f);
	if (!texCoords)
		texCoords = defaultTexCoords;
	}
glBegin (GL_TRIANGLE_FAN); //(nVertices == 3) ? GL_TRIANGLES : (nVertices == 4) ? GL_QUADS : GL_POLYGON);
for (int i = 0; i < nVertices; i++) {
	if (pTexture)
		glTexCoord2dv ((GLdouble*) &texCoords [i]);
	glVertex3dv ((GLdouble*) &(vertices [index ? index [i] : i].m_view.v));
	}
glEnd ();
glDisable (GL_TEXTURE_2D);
}

//------------------------------------------------------------------------------

void CRendererGL::TexturedPolygon (short nTexture, tTexCoord2d* texCoords, rgbColord* color, CVertex* vertices, int nVertices, ushort* index)
{
// This is used for polymodels - texture index is for the D2 PIG regardless of whether we're editing a D1 or D2 level
TexturedPolygon (nTexture ? textureManager.Textures (nTexture, 1) : null, texCoords, color, vertices, nVertices, index);
}

//------------------------------------------------------------------------------

void CRendererGL::Sprite (const CTexture* pTexture, CVertex center, double width, double height, bool bAlways)
{
	static rgbColord color = {1.0, 1.0, 1.0};
	static ushort index [4] = {0, 1, 2, 3};

	CVertex vertices [4];
	float				du = float (pTexture->RenderOffsetX ()) / float (pTexture->RenderWidth ());
	float				dv = float (pTexture->RenderOffsetY ()) / float (pTexture->RenderHeight ());
	tTexCoord2d		texCoords [4] = {
		{du, dv}, 
		{du, 1.0f - dv}, 
		{1.0f - du, 1.0f - dv}, 
		{1.0f - du, dv}
	};

center.Transform (&m_viewMatrix);
vertices [0].m_view.v.x = center.m_view.v.x - width;
vertices [0].m_view.v.y = center.m_view.v.y - height;
vertices [1].m_view.v.x = center.m_view.v.x - width;
vertices [1].m_view.v.y = center.m_view.v.y + height;
vertices [2].m_view.v.x = center.m_view.v.x + width;
vertices [2].m_view.v.y = center.m_view.v.y + height;
vertices [3].m_view.v.x = center.m_view.v.x + width;
vertices [3].m_view.v.y = center.m_view.v.y - height;
vertices [0].m_view.v.z =
vertices [1].m_view.v.z =
vertices [2].m_view.v.z =
vertices [3].m_view.v.z = center.m_view.v.z;
if (bAlways)
	glDepthFunc (GL_ALWAYS);
TexturedPolygon (pTexture, texCoords, &color, vertices, 4, index);
if (bAlways)
	glDepthFunc (GL_LESS);
}

//------------------------------------------------------------------------------

void CRendererGL::Ellipse (CVertex& center, double xRad, double yRad)
{
	static tSinCosf sinCosTable [32];
	static bool bHaveSinCos = false;

if (!bHaveSinCos) {
	bHaveSinCos = true;
	ComputeSinCosTable (sinCosTable, sizeofa (sinCosTable));
	}

if (!m_bOrtho/* && Perspective ()*/) {
	CVertex points [33];
	double xRadd, yRadd;

	if ((xRad < 0) && (yRad < 0)) {
		xRadd = -xRad;
		yRadd = -yRad;
		}
	else {
		// rad is in screen coordinates, so find out what deltas to use for the view coordinates 
		// to achieve the proper radius when projecting the view coordinate
		CDoubleVector s [2], v [2];
		m_viewMatrix.Project (s [0], center.m_view);
		s [1] = s [0];
		s [0].v.x -= xRad;
		s [0].v.y -= yRad;
		s [1].v.x += xRad;
		s [1].v.y += yRad;
		m_viewMatrix.Unproject (v [0], s [0]);
		m_viewMatrix.Unproject (v [1], s [1]);
		if (xRad > 0)
			xRadd = fabs (v [1].v.x - v [0].v.x);
		if (yRad > 0)
			yRadd = fabs (v [1].v.y - v [0].v.y);
		}

	for (int i = 0; i < 32; i++) {
		points [i].m_view.v.x = center.m_view.v.x + sinCosTable [i].fCos * xRadd;
		points [i].m_view.v.y = center.m_view.v.y + sinCosTable [i].fSin * yRadd;
		points [i].m_view.v.z = center.m_view.v.z;
		}
	points [32] = points [0];
	//glLineWidth (3.0);
	PolyLine (points, 33);
	//glLineWidth (1.0);
	}
else {
	CPoint points [33];
	xRad = abs (xRad);
	yRad = abs (xRad);
	for (int i = 0; i < 32; i++) {
		points [i].x = center.m_screen.x + (long) Round (sinCosTable [i].fCos * xRad);
		points [i].y = center.m_screen.y + (long) Round (sinCosTable [i].fSin * yRad);
		}
	points [32] = points [0];
	PolyLine (points, 33);
	}
}

//------------------------------------------------------------------------------

int CRendererGL::GetSideKey (int x, int y, short& nSegment, short& nSide)
{
if (!(RTT () && m_bRenderSideKeys))
	return -1;
if (m_renderBuffers.BufferCount () < 2)
	return -1;
if (!m_bHaveSideKeys) {
	m_renderBuffers.Enable (-1);
	glReadBuffer (m_renderBuffers.ColorBufferId (1));
	glReadPixels (0, 0, ViewWidth (), ViewHeight (), GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*) m_sideKeys);
	m_renderBuffers.Disable ();
	m_bHaveSideKeys = true;
	}
rgbColor& sideKey = m_sideKeys [y * ViewWidth () + x];
if (sideKey.b == 0)
	return 0;
nSegment = short (sideKey.r) * 256 + sideKey.g;
nSide = sideKey.b - 1;
return 1;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
