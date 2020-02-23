// TexAlignTool.cpp
//

#include <math.h>
#include "stdafx.h"
#include "afxpriv.h"
#include "dle-xp.h"

#include "dlcDoc.h"
#include "mineview.h"
#include "toolview.h"

#include "PaletteManager.h"
#include "TextureManager.h"
#include "global.h"
#include "textures.h"
#include "FileManager.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE [] = __FILE__;
#endif

extern short nDbgSeg, nDbgSide;
extern int nDbgVertex;
static int rotMasks [4] = {0, 3, 2, 1};

/////////////////////////////////////////////////////////////////////////////
// CToolView

BEGIN_MESSAGE_MAP(CTextureAlignTool, CTextureTabDlg)
	ON_WM_PAINT ()
	ON_WM_HSCROLL ()
	ON_WM_VSCROLL ()
	ON_BN_CLICKED (IDC_TEXALIGN_HALEFT, OnAlignLeft)
	ON_BN_CLICKED (IDC_TEXALIGN_HARIGHT, OnAlignRight)
	ON_BN_CLICKED (IDC_TEXALIGN_VAUP, OnAlignUp)
	ON_BN_CLICKED (IDC_TEXALIGN_VADOWN, OnAlignDown)
	ON_BN_CLICKED (IDC_TEXALIGN_RALEFT, OnAlignRotLeft)
	ON_BN_CLICKED (IDC_TEXALIGN_RARIGHT, OnAlignRotRight)
	ON_BN_CLICKED (IDC_TEXALIGN_HFLIP, OnHFlip)
	ON_BN_CLICKED (IDC_TEXALIGN_VFLIP, OnVFlip)
	ON_BN_CLICKED (IDC_TEXALIGN_HSHRINK, OnHShrink)
	ON_BN_CLICKED (IDC_TEXALIGN_VSHRINK, OnVShrink)
	ON_BN_CLICKED (IDC_TEXALIGN_RESET, OnAlignReset)
	ON_BN_CLICKED (IDC_TEXALIGN_RESET_TAGGED, OnAlignResetTagged)
	ON_BN_CLICKED (IDC_TEXALIGN_STRETCH2FIT, OnAlignStretch2Fit)
	ON_BN_CLICKED (IDC_TEXALIGN_CHILDALIGN, OnAlignChildren)
	ON_BN_CLICKED (IDC_TEXALIGN_ALIGNALL, OnAlignAll)
	ON_BN_CLICKED (IDC_TEXALIGN_ZOOMIN, OnZoomIn)
	ON_BN_CLICKED (IDC_TEXALIGN_ZOOMOUT, OnZoomOut)
	ON_BN_CLICKED (IDC_TEXALIGN_ROT0, OnRot2nd0)
	ON_BN_CLICKED (IDC_TEXALIGN_ROT90, OnRot2nd90)
	ON_BN_CLICKED (IDC_TEXALIGN_ROT180, OnRot2nd180)
	ON_BN_CLICKED (IDC_TEXALIGN_ROT270, OnRot2nd270)
	ON_BN_CLICKED (IDC_TEXALIGN_IGNOREPLANE, OnAlignIgnorePlane)
	ON_EN_KILLFOCUS (IDC_TEXALIGN_HALIGN, OnAlignX)
	ON_EN_KILLFOCUS (IDC_TEXALIGN_VALIGN, OnAlignY)
	ON_EN_KILLFOCUS (IDC_TEXALIGN_RALIGN, OnAlignRot)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

CTextureAlignTool::~CTextureAlignTool ()
{
	if (IsWindow (m_alignWnd))
		m_alignWnd.DestroyWindow ();
}

//------------------------------------------------------------------------------

BOOL CTextureAlignTool::OnInitDialog ()
{
if (!CTabDlg::OnInitDialog ())
	return FALSE;

/*
m_btnZoomIn.SubclassDlgItem (IDC_TEXALIGN_ZOOMIN, this);
m_btnZoomOut.SubclassDlgItem (IDC_TEXALIGN_ZOOMOUT, this);
m_btnHShrink.SubclassDlgItem (IDC_TEXALIGN_HSHRINK, this);
m_btnVShrink.SubclassDlgItem (IDC_TEXALIGN_VSHRINK, this);
m_btnHALeft.SubclassDlgItem (IDC_TEXALIGN_HALEFT, this);
m_btnHARight.SubclassDlgItem (IDC_TEXALIGN_HARIGHT, this);
m_btnVAUp.SubclassDlgItem (IDC_TEXALIGN_VAUP, this);
m_btnVADown.SubclassDlgItem (IDC_TEXALIGN_VADOWN, this);
m_btnRALeft.SubclassDlgItem (IDC_TEXALIGN_RALEFT, this);
m_btnRARight.SubclassDlgItem (IDC_TEXALIGN_RARIGHT, this);
*/
m_btnZoomIn.AutoLoad (IDC_TEXALIGN_ZOOMIN, this);
m_btnZoomOut.AutoLoad (IDC_TEXALIGN_ZOOMOUT, this);
m_btnHShrink.AutoLoad (IDC_TEXALIGN_HSHRINK, this);
m_btnVShrink.AutoLoad (IDC_TEXALIGN_VSHRINK, this);
m_btnHALeft.AutoLoad (IDC_TEXALIGN_HALEFT, this);
m_btnHARight.AutoLoad (IDC_TEXALIGN_HARIGHT, this);
m_btnVAUp.AutoLoad (IDC_TEXALIGN_VAUP, this);
m_btnVADown.AutoLoad (IDC_TEXALIGN_VADOWN, this);
m_btnRALeft.AutoLoad (IDC_TEXALIGN_RALEFT, this);
m_btnRARight.AutoLoad (IDC_TEXALIGN_RARIGHT, this);

m_btnStretch2Fit.AutoLoad (IDC_TEXALIGN_STRETCH2FIT, this);
m_btnReset.AutoLoad (IDC_TEXALIGN_RESET, this);
m_btnResetTagged.AutoLoad (IDC_TEXALIGN_RESET_TAGGED, this);
m_btnChildAlign.AutoLoad (IDC_TEXALIGN_CHILDALIGN, this);
m_btnAlignAll.AutoLoad (IDC_TEXALIGN_ALIGNALL, this);
m_btnHFlip.AutoLoad (IDC_TEXALIGN_HFLIP, this);
m_btnVFlip.AutoLoad (IDC_TEXALIGN_VFLIP, this);

m_bShowChildren = TRUE;
m_bShowTexture = TRUE;
m_zoom = 1.0;
m_alignX = 0;
m_alignY = 0;
m_alignAngle = 0;
m_alignRot2nd = 0;
m_bIgnorePlane = true;
m_bIgnoreWalls = TRUE;
m_nTimer = -1;
m_nEditTimer = -1;
m_nEditFunc = -1;

CreateImgWnd (&m_alignWnd, IDC_TEXALIGN_SHOW);
HScrollAlign ()->SetScrollRange (-100, 100, FALSE);
HScrollAlign ()->SetScrollPos (0, TRUE);
VScrollAlign ()->SetScrollRange (-100, 100, FALSE);
VScrollAlign ()->SetScrollPos (0, TRUE);

m_bInited = TRUE;
UpdateData (FALSE);
return TRUE;
}

//------------------------------------------------------------------------------

void CTextureAlignTool::DoDataExchange (CDataExchange * pDX)
{
if (!HaveData (pDX)) 
	return;

DDX_Check (pDX, IDC_TEXALIGN_SHOWTEXTURE, m_bShowTexture);
DDX_Check (pDX, IDC_TEXALIGN_SHOWCHILDREN, m_bShowChildren);
DDX_Check (pDX, IDC_TEXALIGN_IGNOREPLANE, m_bIgnorePlane);
//DDX_Check (pDX, IDC_TEXALIGN_IGNOREWALLS, m_bIgnoreWalls);
DDX_Double (pDX, IDC_TEXALIGN_HALIGN, m_alignX);
DDX_Double (pDX, IDC_TEXALIGN_VALIGN, m_alignY);
DDX_Double (pDX, IDC_TEXALIGN_RALIGN, m_alignAngle);
DDX_Radio (pDX, IDC_TEXALIGN_ROT0, m_alignRot2nd);
}

//------------------------------------------------------------------------------

int round_int (int value, int round) 
{
if (value >= 0)
	value += round/2;
else
	value -= round/2;
return (value / round) * round;
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnPaint ()
{
if (theMine == null) 
	return;
CTextureTabDlg::OnPaint ();
UpdateAlignWnd ();
}

//------------------------------------------------------------------------------

bool CTextureAlignTool::Refresh (void)
{
RefreshAlignment ();
UpdateAlignWnd ();
UpdateData (FALSE);
return true;
}

//------------------------------------------------------------------------------

void CTextureAlignTool::UpdateAlignWnd (void)
{
CHECKMINE;
if (m_alignWnd.m_hWnd) {
	RefreshAlignWnd ();
	m_alignWnd.InvalidateRect (null);
	m_alignWnd.UpdateWindow ();
	DLE.MineView ()->Refresh (false);
	}
}

//------------------------------------------------------------------------------

void CTextureAlignTool::RefreshAlignWnd (void) 
{
CHECKMINE;

if (!m_alignWnd.m_hWnd)
	return;

	int			x, y, i, uv, dx, dy;
	CSegment*	pSegment, * pChildSeg;
	CSide*		pSide;
	int			nSide,
					nLine;
	CPen			hPenAxis, 
					hPenGrid;
	CPen			hPenCurrentPoint, 
					hPenCurrentLine,
					hPenCurrentSide;
	CDC			*pDC;
	CPoint		offset;
	CRgn			hRgn;

// read scroll bar
offset.x = int (m_zoom * (double) HScrollAlign ()->GetScrollPos ()) + m_centerPt.x;
offset.y = int (m_zoom * (double) VScrollAlign ()->GetScrollPos ()) + m_centerPt.y;

/*
RefreshX();
RefreshY();
RefreshAngle();
RefreshChecks();
*/
// setup drawing area
POINT	minRect, maxRect;

minRect.x = 12;
minRect.y = 10;
maxRect.x = minRect.x + 166;
maxRect.y = minRect.y + 166;
m_centerPt.x = minRect.x + 166 / 2;
m_centerPt.y = minRect.y + 166 / 2;

pSegment = current->Segment ();
pSide = current->Side ();
nSide = current->SideId ();
nLine = current->Edge ();
short nEdges = pSide->VertexCount ();

// get device context handle
pDC = m_alignWnd.GetDC ();

// create brush, pen, and region handles
hPenAxis.CreatePen (PS_DOT, 1, RGB (192,192,192));
hPenGrid.CreatePen (PS_DOT, 1, RGB (128,128,128));
uint select_mode = DLE.MineView ()->GetSelectMode ();
hPenCurrentPoint.CreatePen (PS_SOLID, 1, (select_mode == POINT_MODE) ? RGB (255,0,0) : RGB(255,196,0)); // red
hPenCurrentLine.CreatePen (PS_SOLID, 1, (select_mode == LINE_MODE) ? RGB (255,0,0) : RGB(255,196,0)); // red
hPenCurrentSide.CreatePen (PS_SOLID, 1, (select_mode == LINE_MODE) ? RGB (255,0,0) : RGB(0,255,0)); // red
CRect rc;
m_alignWnd.GetClientRect (rc);
minRect.x = rc.left;
minRect.y = rc.top;
maxRect.x = rc.right;
maxRect.y = rc.bottom;
m_centerPt.x = rc.Width () / 2;
m_centerPt.y = rc.Height () / 2;
hRgn.CreateRectRgn (minRect.x, minRect.y, maxRect.x, maxRect.y);

// clear texture region
i = pDC->SelectObject (&hRgn);
CBrush	brBlack (RGB (0,0,0));
brBlack.GetSafeHandle ();
hRgn.GetSafeHandle ();
pDC->FillRgn (&hRgn, &brBlack);

// draw grid
pDC->SetBkMode (TRANSPARENT);
y = 16;
for (x = -32 * y; x < 32 * y; x += 32) {
	dx = (int) (m_zoom * x);
	dy = (int) (m_zoom * y * 32);
	pDC->SelectObject((x==0) ? hPenAxis : hPenGrid);
	pDC->MoveTo ((int) (offset.x + dx), (int) (offset.y - dy));
	pDC->LineTo ((int) (offset.x + dx), (int) (offset.y + dy));
	pDC->MoveTo ((int) (offset.x - dy), (int) (offset.y + dx));
	pDC->LineTo ((int) (offset.x + dy), (int) (offset.y + dx));
	}	

if (segmentManager.IsWall ()) {
	// define array of screen points for (u,v) coordinates
	for (i = 0; i < nEdges; i++) {
		x = offset.x + (int) Round (m_zoom * pSide->m_info.uvls [i].u * 32.0);
		y = offset.y + (int) Round (m_zoom * pSide->m_info.uvls [i].v * 32.0);
		m_apts [i].x = x;
		m_apts [i].y = y;
		if (i == 0) {
			m_minPt.x = m_maxPt.x = x;
			m_minPt.y = m_maxPt.y = y;
			}
		else {
			m_minPt.x = min (m_minPt.x,x);
			m_maxPt.x = max (m_maxPt.x,x);
			m_minPt.y = min (m_minPt.y,y);
			m_maxPt.y = max (m_maxPt.y,y);
			}
		}
	m_minPt.x = max (m_minPt.x, minRect.x);
	m_maxPt.x = min (m_maxPt.x, maxRect.x);
	m_minPt.y = max (m_minPt.y, minRect.y);
	m_maxPt.y = min (m_maxPt.y, maxRect.y);

	if (m_bShowChildren) {
		short nChild, nChildLine, iChildSide, iChildLine;
		ushort vert0, vert1, childVert0, childVert1;
		int	x0, y0;
		POINT childPoints [4];

		CEdgeList edgeList;
		pSegment->BuildEdgeList (edgeList);

		// draw all sides (u,v)
		pDC->SelectObject (hPenGrid);
		for (nChildLine = 0; nChildLine < nEdges; nChildLine++) {
			ubyte s1, s2, 
				  v1 = pSide->VertexIdIndex (nChildLine), 
				  v2 = pSide->VertexIdIndex (nChildLine + 1);
			if (0 > edgeList.Find (0, s1, s2, v1, v2))
				continue;
			vert0 = pSegment->VertexId (v1);
			vert1 = pSegment->VertexId (v2);
			nChild = pSegment->ChildId ((s1 == nSide) ? s2 : s1);
			if (nChild > -1) {
				pChildSeg = segmentManager.Segment (nChild);
				// figure out which side of child shares two points w/ current->side
				for (iChildSide = 0; iChildSide < 6; iChildSide++) {
					// ignore children of different textures (or no texture)
					CSide *pChildSide = pChildSeg->m_sides + iChildSide;
					if (pChildSide->Shape () > SIDE_SHAPE_TRIANGLE)
						continue;
					short nChildEdges = pChildSide->VertexCount ();
					if (segmentManager.IsWall (CSideKey (nChild, iChildSide)) && (pChildSide->BaseTex () == pSide->BaseTex ())) {
						for (iChildLine = 0; iChildLine < nChildEdges; iChildLine++) {
							childVert0 = pChildSeg->VertexId (iChildSide, iChildLine);
							childVert1 = pChildSeg->VertexId (iChildSide, iChildLine + 1);
							// if both points of line == either point of parent
							if ((childVert0 == vert0 && childVert1 == vert1) ||
								 (childVert0 == vert1 && childVert1 == vert0)) {

								// now we know the child's side & line which touches the parent
								// so, we need to translate the child's points by even increments
								// ..of the texture size in order to make it line up on the screen
								// start by copying points into an array
								for (i = 0; i < nChildEdges; i++) {
									x = offset.x + (int) Round (m_zoom * pChildSide->m_info.uvls [i].u * 32.0);
									y = offset.y + (int) Round (m_zoom * pChildSide->m_info.uvls [i].v * 32.0);
									childPoints [i].x = x;
									childPoints [i].y = y;
									}
								// now, calculate offset
								uv = (iChildLine + 1) % nChildEdges;
								x0 = childPoints [uv].x - m_apts [nChildLine].x;
								y0 = childPoints [uv].y - m_apts [nChildLine].y;
								x0 = round_int (x0, int (32.0 * m_zoom));
								y0 = round_int (y0, int (32.0 * m_zoom));
								// translate child points
								for (i = 0; i < nChildEdges; i++) {
									childPoints [i].x -= x0;
									childPoints [i].y -= y0;
									}
								// draw child (u,v)
								pDC->SelectObject (hPenCurrentPoint); // color = cyan
								pDC->MoveTo (childPoints [nChildEdges - 1].x,childPoints [nChildEdges - 1].y);
								for (i = 0; i < nChildEdges; i++) {
									pDC->LineTo (childPoints [i].x, childPoints [i].y);
									}
								}
							}
						}
					}
				}
			}
		}

	// highlight current point
	pDC->SelectObject ((HBRUSH)GetStockObject(NULL_BRUSH));
	pDC->SelectObject (hPenCurrentPoint);
	x = m_apts [current->Point ()].x;
	y = m_apts [current->Point ()].y;
	pDC->Ellipse((int) (x - 4 * m_zoom), (int) (y - 4 * m_zoom), (int) (x + 4 * m_zoom), (int) (y + 4 * m_zoom));
	// fill in texture
	DrawAlignment (pDC);
	pDC->SelectObject (hRgn);
	// draw CUVL
	pDC->SelectObject (hPenCurrentSide);
	pDC->MoveTo (m_apts [nEdges - 1].x, m_apts [nEdges - 1].y);
	for (i = 0; i < nEdges; i++)
		pDC->LineTo (m_apts [i].x, m_apts [i].y);
	// highlight current line
	pDC->SelectObject(hPenCurrentLine);
	pDC->MoveTo (m_apts [nLine].x, m_apts [nLine].y);
	pDC->LineTo (m_apts [(nLine + 1) % nEdges].x, m_apts [(nLine + 1) % nEdges].y);
	}

// release dc
m_alignWnd.ReleaseDC (pDC);
// delete Objects ()
DeleteObject(hRgn);
DeleteObject(hPenCurrentSide);
DeleteObject(hPenCurrentLine);
DeleteObject(hPenCurrentPoint);
DeleteObject(hPenAxis);
DeleteObject(hPenGrid);
}

//------------------------------------------------------------------------------

extern short nDbgTexture;

void CTextureAlignTool::DrawAlignment (CDC *pDC)
{
CHECKMINE;
if (!m_bShowTexture)
	return;

	CPalette*	oldPalette;
	CRgn			hRgn;
	int			h, i, j, x, y;
	POINT			offset;
	CSide*		pSide = current->Side ();
	CTexture		tex (textureManager.SharedBuffer ());
	ushort		scale;

offset.x = (int) (m_zoom * (double) HScrollAlign ()->GetScrollPos ()) + m_centerPt.x - 128;
offset.y = (int) (m_zoom * (double) VScrollAlign ()->GetScrollPos ()) + m_centerPt.y - 128;

memset (tex.Buffer (), 0, textureManager.SharedBufferSize ());
#ifdef DEBUG
if (pSide->BaseTex () == nDbgTexture)
	nDbgTexture = nDbgTexture;
#endif
if (tex.BlendTextures (pSide->BaseTex (), pSide->OvlTex (), 0, 0)) {
	DEBUGMSG (" Texture tool: Texture not found (textureManager.BlendTextures failed)");
	return;
	}
oldPalette = pDC->SelectPalette (paletteManager.Render (), FALSE);
pDC->RealizePalette();
hRgn.CreatePolygonRgn (m_apts, pSide->VertexCount (), ALTERNATE);
pDC->SelectObject (&hRgn);
uint w = tex.Width ();
scale = w / 64;
for (x = m_minPt.x; x < m_maxPt.x; x++) {
	for (y = m_minPt.y; y < m_maxPt.y; y++) {
		i = ((int) ((((x - offset.x) * 2) / m_zoom)) % 64) * scale;
		j = ((int) ((((y - offset.y) * 2) / m_zoom)) % 64) * scale;
		pDC->SetPixel (x, y, h = tex.Buffer ((w - j) * w + i)->ColorRef ());
		}
	}
DeleteObject(hRgn);
pDC->SelectPalette (oldPalette, FALSE);
}

//------------------------------------------------------------------------------

BOOL CTextureAlignTool::OnKillActive ()
{
if (m_nEditTimer >= 0) {
	m_pParentWnd->KillTimer (m_nEditTimer);
	m_nEditTimer = -1;
	}
return CTextureTabDlg::OnKillActive ();
}

//------------------------------------------------------------------------------

BOOL CTextureAlignTool::HandleTimer (UINT_PTR nIdEvent)
{
if (nIdEvent == 3) {
	OnEditTimer ();
	return TRUE;
	}
return FALSE;
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnAlignX ()
{
UpdateData (TRUE);

	CSide*	pSide = current->Side ();
	double	delta = pSide->m_info.uvls [current->Point ()].u - m_alignX / 20.0;

if (delta) {
	UpdateData (TRUE);
	undoManager.Begin (__FUNCTION__, udSegments, true);
	switch (DLE.MineView ()->GetSelectMode ()) {
		case POINT_MODE:
			pSide->m_info.uvls [current->Point ()].u -= delta;
			break;
		case LINE_MODE:
			pSide->m_info.uvls [current->Edge ()].u -= delta;
			pSide->m_info.uvls [(current->Edge () + 1) % pSide->VertexCount ()].u -= delta;
			break;
		default:
			for (int i = 0; i < pSide->VertexCount (); i++)
				pSide->m_info.uvls [i].u -= delta;
		}  
	undoManager.End (__FUNCTION__);
	UpdateAlignWnd ();
	}
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnAlignY ()
{
UpdateData (TRUE);

	CSide*	pSide = current->Side ();
	double	delta = pSide->m_info.uvls [current->Point ()].v - m_alignY / 20.0;

if (delta) {
	UpdateData (TRUE);
	undoManager.Begin (__FUNCTION__, udSegments, true);
	switch (DLE.MineView ()->GetSelectMode ()) {
		case POINT_MODE:
			pSide->m_info.uvls [current->Point ()].v -= delta;
			break;
		case LINE_MODE:
			pSide->m_info.uvls [current->Edge ()].v -= delta;
			pSide->m_info.uvls [(current->Edge () + 1) % pSide->VertexCount ()].v -= delta;
			break;
		default:
			for (int i = 0; i < pSide->VertexCount (); i++)
				pSide->m_info.uvls [i].v -= delta;
		}  
	undoManager.End (__FUNCTION__);
	UpdateAlignWnd ();
	}
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnAlignRot ()
{
UpdateData (TRUE);
  
	double	delta, dx, dy, angle;
	CSide*	pSide = current->Side ();

dx = pSide->m_info.uvls [1].u - pSide->m_info.uvls [0].u;
dy = pSide->m_info.uvls [1].v - pSide->m_info.uvls [0].v;
angle = (dx || dy) ? atan3 (dy, dx) - M_PI_2 : 0;
delta = angle - m_alignAngle * PI / 180.0;
RotateUV (delta, FALSE);
}

//------------------------------------------------------------------------------

void CTextureAlignTool::RefreshAlignment ()
{
	CSide* pSide = current->Side ();

m_alignX = pSide->m_info.uvls [current->Point ()].u * 20.0;
m_alignY = pSide->m_info.uvls [current->Point ()].v * 20.0;

double dx = pSide->m_info.uvls [1].u - pSide->m_info.uvls [0].u;
double dy = pSide->m_info.uvls [1].v - pSide->m_info.uvls [0].v;
m_alignAngle = ((dx || dy) ? atan3 (dy,dx) - M_PI_2 : 0) * 180.0 / M_PI;
if (m_alignAngle < 0)
	m_alignAngle += 360.0;
else if (m_alignAngle > 360.9)
	m_alignAngle -= 360.0;
int h = pSide->OvlAlignment ();
for (m_alignRot2nd = 0; m_alignRot2nd < 4; m_alignRot2nd++)
	if (rotMasks [m_alignRot2nd] == h)
		break;
}

//------------------------------------------------------------------------------

void CTextureAlignTool::RotateUV (double angle, bool bUpdate)
{
	CSide*	pSide = current->Side ();

UpdateData (TRUE);
undoManager.Begin (__FUNCTION__, udSegments, true);
for (int i = 0; i < pSide->VertexCount (); i++) 
	pSide->Uvls (i)->Rotate (angle);
if (bUpdate)
	m_alignAngle -= angle * 180.0 / PI;
UpdateData (FALSE);
undoManager.End (__FUNCTION__);
UpdateAlignWnd ();
}

//------------------------------------------------------------------------------

void CTextureAlignTool::HFlip (void)
{
	CSide*	pSide = current->Side ();

UpdateData (TRUE);
undoManager.Begin (__FUNCTION__, udSegments);
switch (DLE.MineView ()->GetSelectMode ()) {
	case POINT_MODE:
		break;
	case LINE_MODE:
		Swap (pSide->m_info.uvls [current->Edge ()].u, pSide->m_info.uvls [(current->Edge () + 1) % pSide->VertexCount ()].u);
		break;
	default:
		for (int i = 0; i < 2; i++) 
			Swap (pSide->m_info.uvls [i].u, pSide->m_info.uvls [i + 2].u);
	}
UpdateData (FALSE);
undoManager.End (__FUNCTION__);
UpdateAlignWnd ();
}

//------------------------------------------------------------------------------

void CTextureAlignTool::VFlip (void)
{
	CSide*	pSide = current->Side ();

UpdateData (TRUE);
undoManager.Begin (__FUNCTION__, udSegments);
switch (DLE.MineView ()->GetSelectMode ()) {
	case POINT_MODE:
		break;
	case LINE_MODE:
		Swap (pSide->m_info.uvls [current->Edge ()].v, pSide->m_info.uvls [(current->Edge () + 1) % pSide->VertexCount ()].v);
		break;
	default:
		for (int i = 0; i < 2; i++) 
			Swap (pSide->m_info.uvls [i].v, pSide->m_info.uvls [i + 2].v);
	}
UpdateData (FALSE);
undoManager.End (__FUNCTION__);
UpdateAlignWnd ();
}

//------------------------------------------------------------------------------

void CTextureAlignTool::HAlign (int dir)
{
	CSide*	pSide = current->Side ();
	double	delta = DLE.MineView ()->MineMoveRate () / 8.0 / m_zoom * dir;

UpdateData (TRUE);
undoManager.Begin (__FUNCTION__, udSegments);
switch (DLE.MineView ()->GetSelectMode ()) {
	case POINT_MODE:
		pSide->m_info.uvls [current->Point ()].u += delta;
		break;
	case LINE_MODE:
		pSide->m_info.uvls [current->Edge ()].u += delta;
		pSide->m_info.uvls [(current->Edge () + 1) % pSide->VertexCount ()].u += delta;
		break;
	default:
		for (int i = 0; i < pSide->VertexCount (); i++)
			pSide->m_info.uvls [i].u += delta;
	}
m_alignX = (double) pSide->m_info.uvls [current->Point ()].u * 20.0;
UpdateData (FALSE);
undoManager.End (__FUNCTION__);
UpdateAlignWnd ();
}

//------------------------------------------------------------------------------

void CTextureAlignTool::VAlign (int dir)
{
	CSide*	pSide = current->Side ();
	double	delta = DLE.MineView ()->MineMoveRate () / 8.0 / m_zoom * dir;

UpdateData (TRUE);
undoManager.Begin (__FUNCTION__, udSegments);
switch (DLE.MineView ()->GetSelectMode ()) {
	case POINT_MODE:
		pSide->m_info.uvls [current->Point ()].v += delta;
		break;
	case LINE_MODE:
		pSide->m_info.uvls [current->Edge ()].v += delta;
		pSide->m_info.uvls [(current->Edge () + 1) % pSide->VertexCount ()].v += delta;
		break;
	default:
		for (int i = 0; i < pSide->VertexCount (); i++)
			pSide->m_info.uvls [i].v += delta;
	}
m_alignY = (double)pSide->m_info.uvls [current->Point ()].v * 20.0;
UpdateData (FALSE);
undoManager.End (__FUNCTION__);
UpdateAlignWnd ();
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnHFlip (void)
{
HFlip ();
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnVFlip (void)
{
VFlip ();
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnAlignLeft (void)
{
HAlign (-1);
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnAlignRight (void)
{
HAlign (1);
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnAlignUp (void)
{
VAlign (-1);
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnAlignDown (void)
{
VAlign (1);
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnAlignRotLeft (void)
{
RotateUV (theMine->RotateRate ());
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnAlignRotRight (void)
{
RotateUV (-theMine->RotateRate ());
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnHShrink ()
{
	CSide		*pSide = current->Side ();
	int		h = pSide->VertexCount ();
	double	delta = DLE.MineView ()->MineMoveRate () / 8.0 / m_zoom;

UpdateData (TRUE);
undoManager.Begin (__FUNCTION__, udSegments);
pSide->m_info.uvls [0].u -= delta;
if (h > 1) {
	pSide->m_info.uvls [1].u -= delta;
	if (h > 2) {
		pSide->m_info.uvls [2].u += delta;
		if (h > 3) 
			pSide->m_info.uvls [3].u += delta;
		}
	}
undoManager.End (__FUNCTION__);
UpdateAlignWnd ();
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnVShrink ()
{
	CSide*	pSide = current->Side ();
	int		h = pSide->VertexCount ();
	double	delta = DLE.MineView ()->MineMoveRate () / 8.0 / m_zoom;

UpdateData (TRUE);
undoManager.Begin (__FUNCTION__, udSegments);
pSide->m_info.uvls [0].v += delta;
if (h > 1) {
	pSide->m_info.uvls [3].v += delta;
	if (h > 2) {
		pSide->m_info.uvls [1].v -= delta;
		if (h > 3) 
			pSide->m_info.uvls [2].v -= delta;
		}
	}
undoManager.End (__FUNCTION__);
UpdateAlignWnd ();
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnAlignReset ()
{
UpdateData (TRUE);
undoManager.Begin (__FUNCTION__, udSegments);
current->Segment ()->SetUV (current->SideId (), 0.0, 0.0);
m_alignX = 0.0;
m_alignY = 0.0;
m_alignAngle = 0.0;
Rot2nd (0);
UpdateData (FALSE);
undoManager.End (__FUNCTION__);
UpdateAlignWnd ();
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnAlignResetTagged ()
{	
	short nWalls = wallManager.WallCount ();
	BOOL bModified = FALSE;

UpdateData (TRUE);
undoManager.Begin (__FUNCTION__, udSegments);
CSegment* pSegment = segmentManager.Segment (0);
int nSegments = segmentManager.Count ();
for (int nSegment = 0; nSegment < nSegments; nSegment++, pSegment++) {
	CSide* pSide = pSegment->Side (0);
	for (short nSide = 0; nSide < 6; nSide++, pSide++) {
		if (pSide->Shape () > SIDE_SHAPE_TRIANGLE) 
			continue;
		if (segmentManager.IsTagged (CSideKey (nSegment, nSide))) {
			if ((pSegment->ChildId (nSide) == -1) || (pSegment->m_sides [nSide].m_info.nWall < nWalls)) {
				pSegment->m_sides [nSide].m_info.nOvlTex &= TEXTURE_MASK; // rotate 0
				pSegment->SetUV (nSide, 0.0, 0.0);
				bModified = TRUE;
				}
			}
		}
	}
undoManager.End (__FUNCTION__);
DLE.MineView ()->Refresh (false);
UpdateAlignWnd ();
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnAlignStretch2Fit ()
{
	CSide*		pSide = current->Side ();
	CSegment*	pSegment;
	short			nSegment, nSide;
	int			i;

UpdateData (TRUE);
undoManager.Begin (__FUNCTION__, udSegments);
if (!segmentManager.HasTaggedSides ()) {
	for (i = 0; i < pSide->VertexCount (); i++) {
		pSide->m_info.uvls [i].u = defaultUVLs [i].u;
		pSide->m_info.uvls [i].v = defaultUVLs [i].v;
		}
	}
else {
	for (nSegment = 0, pSegment = segmentManager.Segment (0); nSegment < segmentManager.Count (); nSegment++, pSegment++) {
		for (nSide = 0, pSide = pSegment->m_sides; nSide < 6; nSide++, pSide++) {
			if (segmentManager.IsTagged (CSideKey (nSegment, nSide))) {
				for (i = 0; i < pSide->VertexCount (); i++) {
					pSide->m_info.uvls [i].u = defaultUVLs [i].u;
					pSide->m_info.uvls [i].v = defaultUVLs [i].v;
					}
				}
			}
		}
	}
undoManager.End (__FUNCTION__);
DLE.MineView ()->Refresh (false);
UpdateAlignWnd ();
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnAlignAll (void)
{
	CTextureTool *texTool = DLE.ToolView ()->TextureTool ();

UpdateData (TRUE);
undoManager.Begin (__FUNCTION__, udSegments);
segmentManager.AlignTextures (current->SegmentId (), current->SideId (), texTool->m_bUse1st, texTool->m_bUse2nd, m_bIgnorePlane, true);
undoManager.End (__FUNCTION__);
UpdateAlignWnd ();
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnAlignChildren ()
{
	CTextureTool *texTool = DLE.ToolView ()->TextureTool ();

UpdateData (TRUE);
undoManager.Begin (__FUNCTION__, udSegments);
// the alignment function will take care of only aligning tagged sides (provided they are all connected)
segmentManager.AlignTextures (current->SegmentId (), current->SideId (), texTool->m_bUse1st, texTool->m_bUse2nd, m_bIgnorePlane, false);
undoManager.End (__FUNCTION__);
UpdateAlignWnd ();
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnZoomIn ()
{
if (m_zoom < 16.0) {
	m_zoom *= 2.0;
	UpdateAlignWnd ();
	}
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnZoomOut ()
{
if (m_zoom > 1.0/16.0) {
	m_zoom /= 2.0;
	UpdateAlignWnd ();
	}
}

//------------------------------------------------------------------------------

void CTextureAlignTool::Rot2nd (int iAngle)
{
	CSide *pSide = current->Side ();
 
if ((pSide->OvlTex (0)) && ((pSide->OvlAlignment ()) != rotMasks [iAngle])) {
	undoManager.Begin (__FUNCTION__, udSegments);
	pSide->m_info.nOvlTex &= TEXTURE_MASK;
   pSide->m_info.nOvlTex |= (rotMasks [iAngle] << 14);
	m_alignRot2nd = iAngle;
	UpdateData (FALSE);
	undoManager.End (__FUNCTION__);
	UpdateAlignWnd ();
	}
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnRot2nd0 ()
{
Rot2nd (0);
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnRot2nd90 ()
{
Rot2nd (1);
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnRot2nd180 ()
{
Rot2nd (2);
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnRot2nd270 ()
{
Rot2nd (3);
}

//------------------------------------------------------------------------------
		
void CTextureAlignTool::OnAlignIgnorePlane ()
{
m_bIgnorePlane = !m_bIgnorePlane;
Refresh ();
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnAlignIgnoreWalls ()
{
m_bIgnoreWalls = !m_bIgnoreWalls;
Refresh ();
}

//------------------------------------------------------------------------------

BOOL CTextureAlignTool::OnNotify (WPARAM wParam, LPARAM lParam, LRESULT *pResult)
{
LPNMHDR	nmHdr = (LPNMHDR) lParam;
int		nMsg = nmHdr->code;

switch (wParam) {
	case IDC_TEXALIGN_HALIGN:
	case IDC_TEXALIGN_VALIGN:
	case IDC_TEXALIGN_RALIGN:
	case IDC_TEXALIGN_ZOOMIN:
	case IDC_TEXALIGN_ZOOMOUT:
	case IDC_TEXALIGN_HALEFT:
	case IDC_TEXALIGN_HARIGHT:
	case IDC_TEXALIGN_VAUP:
	case IDC_TEXALIGN_VADOWN:
	case IDC_TEXALIGN_RALEFT:
	case IDC_TEXALIGN_RARIGHT:
	case IDC_TEXALIGN_HSHRINK:
	case IDC_TEXALIGN_VSHRINK:
		if (((LPNMHDR) lParam)->code == WM_LBUTTONDOWN) {
			m_nEditFunc = int (wParam);
			m_nEditTimer = m_pParentWnd->SetTimer (3, m_nTimerDelay = 250U, null);
			}
		else {
			m_nEditFunc = -1;
			if (m_nEditTimer >= 0) {
				m_pParentWnd->KillTimer (m_nEditTimer);
				m_nEditTimer = -1;
				}
			}
		break;

	default:
		return CTextureTabDlg::OnNotify (wParam, lParam, pResult);
	}
*pResult = 0;
return TRUE;
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnEditTimer (void)
{
switch (m_nEditFunc) {
	case IDC_TEXALIGN_HALIGN:
		OnAlignX ();
		break;
	case IDC_TEXALIGN_VALIGN:
		OnAlignY ();
		break;
	case IDC_TEXALIGN_ZOOMIN:
		OnZoomIn ();
		break;
	case IDC_TEXALIGN_ZOOMOUT:
		OnZoomOut ();
		break;
	case IDC_TEXALIGN_HALEFT:
		OnAlignLeft ();
		break;
	case IDC_TEXALIGN_HARIGHT:
		OnAlignRight ();
		break;
	case IDC_TEXALIGN_VAUP:
		OnAlignUp ();
		break;
	case IDC_TEXALIGN_VADOWN:
		OnAlignDown ();
		break;
	case IDC_TEXALIGN_RALEFT:
		OnAlignRotLeft ();
		break;
	case IDC_TEXALIGN_RARIGHT:
		OnAlignRotRight ();
		break;
	case IDC_TEXALIGN_HSHRINK:
		OnHShrink ();
		break;
	case IDC_TEXALIGN_VSHRINK:
		OnVShrink ();
		break;
	default:
		break;
	}
UINT i = (m_nTimerDelay * 9) / 10;
if (i >= 25) {
	m_pParentWnd->KillTimer (m_nTimer);
	m_nTimer = m_pParentWnd->SetTimer (3, m_nTimerDelay = i, null);
	}
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
int nPos = pScrollBar->GetScrollPos ();
CRect rc;
m_alignWnd.GetClientRect (rc);
switch (scrollCode) {
	case SB_LINEUP:
		nPos--;
		break;
	case SB_LINEDOWN:
		nPos++;
		break;
	case SB_PAGEUP:
		nPos -= rc.Width () / 4;
		break;
	case SB_PAGEDOWN:
		nPos += rc.Width () / 4;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		nPos = thumbPos;
		break;
	case SB_ENDSCROLL:
		return;
	}
pScrollBar->SetScrollPos (nPos, TRUE);
UpdateAlignWnd ();
}

//------------------------------------------------------------------------------

void CTextureAlignTool::OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
int nPos = pScrollBar->GetScrollPos ();
CRect rc;
m_alignWnd.GetClientRect (rc);
switch (scrollCode) {
	case SB_LINEUP:
		nPos--;
		break;
	case SB_LINEDOWN:
		nPos++;
		break;
	case SB_PAGEUP:
		nPos -= rc.Height () / 4;
		break;
	case SB_PAGEDOWN:
		nPos += rc.Height () / 4;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		nPos = thumbPos;
		break;
	case SB_ENDSCROLL:
		return;
	}
pScrollBar->SetScrollPos (nPos, TRUE);
UpdateAlignWnd ();
}

//------------------------------------------------------------------------------

//eof TexAlignTool.cpp