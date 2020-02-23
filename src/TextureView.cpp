// dlcView.cpp : implementation of the CMineView class
//

#include "stdafx.h"
#include "dle-xp.h"

#include "dlcDoc.h"
#include "mineview.h"

#include "PaletteManager.h"
#include "TextureManager.h"
#include "global.h"
#include "FileManager.h"
#include "light.h"
#include "toolview.h"
#include "texturefilter.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TEXTURE_SCALE	2

/////////////////////////////////////////////////////////////////////////////
// CMineView

IMPLEMENT_DYNCREATE(CTextureView, CWnd)
BEGIN_MESSAGE_MAP(CTextureView, CWnd)
//	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

//------------------------------------------------------------------------

CTextureView::CTextureView () 
	: CWnd ()
{
m_pTextures = null;
m_bLButtonDown = m_bRButtonDown = false;
m_penCyan = new CPen (PS_SOLID, 1, RGB (0,255,255));
int scale = TEXTURE_SCALE;
m_iconSize.cx = 64 / scale;
m_iconSize.cy = 64 / scale;
m_iconSpace.cx = m_iconSize.cx + 6;
m_iconSpace.cy = m_iconSize.cy + 6;
m_paneSize.cx = 
m_paneSize.cy = 0;
m_viewFlags = 0;
m_filter.Filter () = GetPrivateProfileInt ("DLE", "TextureFilter", 0xFFFFFFFF, DLE.IniFile ());
m_bShowAll = TRUE;
m_bDelayRefresh = false;
};

//------------------------------------------------------------------------

CTextureView::~CTextureView ()
{
if (m_pTextures)
	free (m_pTextures);
if (m_penCyan)
	delete m_penCyan;
};

//------------------------------------------------------------------------

void CTextureView::Refresh (bool bRepaint) 
{
CHECKMINE;
if (bRepaint) {
	InvalidateRect (null, TRUE);
	UpdateWindow ();
	}
if (!m_bDelayRefresh) {
	DLE.MineView ()->Refresh (false);
	DLE.ToolView ()->Refresh ();
	}
}

//------------------------------------------------------------------------

void CTextureView::Reset () 
{
CHECKMINE;
//textureManager.Release ();
Refresh ();
}

//------------------------------------------------------------------------
// CTextureView::EvVScroll()
//
// Action - Receives scroll movements messages and updates scroll position
//          Also calls refreshdata() to update the bitmap.
//------------------------------------------------------------------------

void CTextureView::OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
UINT nPos = GetScrollPos (SB_VERT);
CRect rect;
GetClientRect(rect);

m_bDelayRefresh = true;
switch (scrollCode) {
	case SB_LINEUP:
		nPos--;
		break;
	case SB_LINEDOWN:
		nPos++;
		break;
	case SB_PAGEUP:
		nPos -= m_viewSpace.cy;
		break;
	case SB_PAGEDOWN:
		nPos += m_viewSpace.cy;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		nPos = thumbPos;
		break;
	case SB_ENDSCROLL:
		m_bDelayRefresh = false;
		Refresh (false);
		return;
}
SetScrollPos (SB_VERT, nPos, TRUE);
Refresh ();
}

//------------------------------------------------------------------------
// CTextureView::EvLButtonDown ()
//
// Action - Sets texture 1 of current side to texture under mouse pointer
//
// Notes - If shift is held down and object is poly type, then object's
//         texture is changed instead of segment's texture.
//------------------------------------------------------------------------

void CTextureView::OnLButtonDown (UINT nFlags, CPoint point)
{
if (!m_bRButtonDown) 
	m_bLButtonDown = true;
else {
	m_bRButtonDown = false;
	short nTexture;
	if (!PickTexture (point, nTexture))
		DLE.MineView ()->LocateTexture (nTexture);
	}
}

void CTextureView::OnRButtonDown (UINT nFlags, CPoint point)
{
if (!m_bLButtonDown) 
	m_bRButtonDown = true;
else {
	m_bLButtonDown = false;
	short nTexture;
	if (!PickTexture (point, nTexture))
		DLE.MineView ()->LocateTexture (nTexture);
	}
}

//------------------------------------------------------------------------

void CTextureView::OnLButtonUp (UINT nFlags, CPoint point)
{
CHECKMINE;

if (!m_bLButtonDown)
	return;
m_bLButtonDown = false;

short nTexture;
if (PickTexture (point, nTexture))
	return;

if (nFlags & MK_SHIFT) {
	CGameObject *pObject = objectManager.Object (current->ObjectId ());
   if (pObject->m_info.renderType != RT_POLYOBJ) 
		return;
	pObject->rType.polyModelInfo.nOverrideTexture = nTexture;
  } 
else if (nFlags & MK_CONTROL) {
	DLE.ToolView ()->TriggerTool ()->SetTexture (nTexture, -1);
	DLE.ToolView ()->TriggerTool ()->Refresh ();
	}
else {
	CSegment* pSegment = segmentManager.Segment (0);
	undoManager.Begin (__FUNCTION__, udSegments); 
	for (short nSegment = 0, nSegments = segmentManager.Count (); nSegment < nSegments; nSegment++, pSegment++) 
		for (short nSide = 0; nSide < 6; nSide++)
			if (pSegment->IsTagged (nSide)) 
				segmentManager.SetTextures (CSideKey (nSegment, nSide), nTexture, -1);
	segmentManager.SetTextures (*current, nTexture, -1);
	undoManager.End (__FUNCTION__); 
	Refresh ();
	}
}

//------------------------------------------------------------------------

void CTextureView::OnRButtonUp (UINT nFlags, CPoint point)
{
CHECKMINE;

if (!m_bRButtonDown)
	return;
m_bRButtonDown = false;

	CSide *pSide = current->Side ();
	short	nTexture;

if (PickTexture (point, nTexture))
	return;
if (nFlags & MK_CONTROL) {
	DLE.ToolView ()->TriggerTool ()->SetTexture (-1, nTexture);
	DLE.ToolView ()->TriggerTool ()->Refresh ();
	}
else {
	CSegment* pSegment = segmentManager.Segment (0);
	for (short nSegment = 0, nSegments = segmentManager.Count (); nSegment < nSegments; nSegment++, pSegment++) 
		for (short nSide = 0; nSide < 6; nSide++)
			if (pSegment->IsTagged (nSide)) 
				segmentManager.SetTextures (CSideKey (nSegment, nSide), -1, nTexture);
	segmentManager.SetTextures (*current, -1, nTexture);
	Refresh ();
	}
}

//------------------------------------------------------------------------

void CTextureView::OnMButtonDown (UINT nFlags, CPoint point)
{
CHECKMINE;

short nTexture;
if (!PickTexture (point, nTexture))
	DLE.MineView ()->LocateTexture (nTexture);
}

//------------------------------------------------------------------------
// CTextureView::PickTexture()
//
// Action - calculates position of textures and matches it up with mouse.
// If a match is found, nBaseTex is defined and 0 is returned
//------------------------------------------------------------------------

int CTextureView::PickTexture (CPoint &point,short &nTexture) 
{
if (theMine == null)
	return 1;
if (!textureManager.Available ())
	return 1;

//if (!m_pTextures)
//	return 0;

  nTexture = 0;  // set default in case value is used when function fails

  CRect rect;
  GetClientRect(rect);

UINT nPos = GetScrollPos (SB_VERT);
int nOffset = nPos * m_viewSpace.cx;
ubyte filterP [1 + MAX_TEXTURES_D2 / 8];

m_filter.Process (filterP, m_bShowAll);
int x = 0;
int y = 0;

x = point.x / m_iconSpace.cx;
y = point.y / m_iconSpace.cy;
int h = nOffset + y * m_viewSpace.cx + x + 1;
int i;
for (i = 0; i < m_filter.Count (1); i++) {
	if (GETBIT (filterP, i)) //filterP [i / 8] & (1 << (i & 7)))
		if (!--h) {
			nTexture = m_filter.MapViewToTex (i); //m_pTextures [i];
			return 0; // return success
			}
	}
return 1; // return failure
}

								/*---------------------------*/

void CTextureView::RecalcLayout (void) 
{
CHECKMINE;

if (!textureManager.Available ())
	return;
if (DLE.MainFrame ())
	DLE.MainFrame ()->RecalcLayout ();

	CRect rect;
	GetClientRect(rect);

m_viewSpace.cx = rect.Width () / m_iconSpace.cx;
m_viewSpace.cy = rect.Height () / m_iconSpace.cy;

int nOffset = 0;
m_bShowAll = ((m_viewFlags & eViewMineUsedTextures) == 0);

if (!(m_viewSpace.cx && m_viewSpace.cy))
	return;

ubyte filterP [(MAX_TEXTURES_D2 + 7) / 8];
m_filter.Process (filterP, m_bShowAll);
m_nRows [ShowAll ()] = (m_filter.Count (ShowAll ()) + m_viewSpace.cx - 1) / m_viewSpace.cx;

// read scroll position to get offset for bitmap tiles
SetScrollRange (SB_VERT, 0, m_nRows [ShowAll ()] - m_viewSpace.cy, TRUE);
// if there is enough lines to show all textures, then hide the scroll bar
if (m_nRows [ShowAll ()] <= m_viewSpace.cy) {
	ShowScrollBar (SB_VERT,FALSE);
	nOffset = 0;
	}
else {
// otherwise, calculate the number of rows to skip
	ShowScrollBar (SB_VERT,TRUE);
	UINT nPos = GetScrollPos (SB_VERT);
	int i, j = nPos * m_viewSpace.cx; 
	for (i = 0, nOffset = 0; (i < m_filter.Count (1)); i++)
		if (ShowAll () || GETBIT (filterP, i)) {
			nOffset++;
			if (--j < 0)
				break;
			}
	}
SetScrollPos (SB_VERT, nOffset / m_viewSpace.cx, TRUE);
// figure out position of current texture
int nBaseTex = current->Side ()->BaseTex ();
int nOvlTex = current->Side ()->OvlTex (0); // strip rotation info
CTexture tex (textureManager.SharedBuffer ());

CDC *pDC = GetDC();
if (!pDC) {
	ErrorMsg ("No device context for texture picker available");
	return;
	}
BITMAPINFO* bmi = paletteManager.BMI ();

CPalette *oldPalette = pDC->SelectPalette (paletteManager.Render (), FALSE);
pDC->RealizePalette ();
pDC->SetStretchBltMode (STRETCH_DELETESCANS);
int x = 0;
int y = 0;
for (int i = 0; i < m_filter.Count (1); i++) {
	if (!ShowAll ()) {
		if (!GETBIT (filterP, i))
			continue;
			}
	if (nOffset &&	--nOffset)
		continue;
	if (!tex.BlendTextures (m_filter.MapViewToTex (i), 0, 0, 0)) {
#if 1
		bmi->bmiHeader.biWidth = tex.Width ();
		bmi->bmiHeader.biHeight = tex.Width ();
		bmi->bmiHeader.biBitCount = 32;
		StretchDIBits (*pDC, 3 + x * m_iconSpace.cx, 3 + y * m_iconSpace.cy, 
							m_iconSize.cx, m_iconSize.cy, 0, 0, tex.Width (), tex.Width (), 
							(void*) tex.Buffer (), bmi, DIB_RGB_COLORS, SRCCOPY);
#else
		double scale = 1.0 / tex.Scale ();
		double xStep = scale * (double) tex.Width () / (double) m_iconSize.cx;
		double yStep = scale * (double) tex.m_info.height / (double) m_iconSize.cy;
		int left = 3 + x * m_iconSpace.cx;
		for (double hx = 0; hx < tex.Width (); hx += xStep) {
			int top = 3 + y * m_iconSpace.cy;
			for (double hy = 0; hy < tex.Width (); hy += yStep) 
				pDC->SetPixel (left, top++, tex.Buffer () [(int) hy * tex.Width () + (int) hx].ColorRef ());
			left++;
			}
#endif
		}
// pick color for box drawn around texture
	if (m_filter.MapViewToTex (i) == nBaseTex)
		pDC->SelectObject (GetStockObject (WHITE_PEN));
	else if (i && (m_filter.MapViewToTex (i) == nOvlTex)) // note: 0 means no texture
		pDC->SelectObject (m_penCyan);
	else
		pDC->SelectObject (GetStockObject (BLACK_PEN));
	pDC->MoveTo (1 + x * m_iconSpace.cx, 1 + y * m_iconSpace.cy);
	pDC->LineTo (4 + x * m_iconSpace.cx + m_iconSize.cx,1 + y * m_iconSpace.cy);
	pDC->LineTo (4 + x * m_iconSpace.cx + m_iconSize.cx,4 + y * m_iconSpace.cy + m_iconSize.cy);
	pDC->LineTo (1 + x * m_iconSpace.cx, 4 + y * m_iconSpace.cy + m_iconSize.cy);
	pDC->LineTo (1 + x * m_iconSpace.cx, 1 + y * m_iconSpace.cy);

// draw black boxes around and inside this box
//	  SelectObject(hdc, GetStockObject(BLACK_PEN));
	pDC->MoveTo (0 + x * m_iconSpace.cx, 0 + y * m_iconSpace.cy);
	pDC->LineTo (5 + x * m_iconSpace.cx + m_iconSize.cx,0 + y * m_iconSpace.cy);
	pDC->LineTo (5 + x * m_iconSpace.cx + m_iconSize.cx,5 + y * m_iconSpace.cy + m_iconSize.cy);
	pDC->LineTo (0 + x * m_iconSpace.cx, 5 + y * m_iconSpace.cy + m_iconSize.cy);
	pDC->LineTo (0 + x * m_iconSpace.cx, 0 + y * m_iconSpace.cy);
	pDC->MoveTo (2 + x * m_iconSpace.cx, 2 + y * m_iconSpace.cy);
	pDC->LineTo (3 + x * m_iconSpace.cx + m_iconSize.cx, 2 + y * m_iconSpace.cy);
	pDC->LineTo (3 + x * m_iconSpace.cx + m_iconSize.cx, 3 + y * m_iconSpace.cy + m_iconSize.cy);
	pDC->LineTo (2 + x * m_iconSpace.cx, 3 + y * m_iconSpace.cy + m_iconSize.cy);
	pDC->LineTo (2 + x * m_iconSpace.cx, 2 + y * m_iconSpace.cy);
	if (++x >= m_viewSpace.cx) {
		x = 0;
		if (++y >= m_viewSpace.cy)
			break;
			}
		} 
pDC->SelectPalette (oldPalette, FALSE);
ReleaseDC (pDC);
}

								/*---------------------------*/

void CTextureView::Setup (void) 
{
m_filter.Setup ();
}

								/*---------------------------*/

afx_msg BOOL CTextureView::OnEraseBkgnd (CDC* pDC)
{
if (theMine == null) 
	return FALSE;

   CRect    rc;
   CBrush   bkGnd, * pOldBrush;
   CPoint   pt (0,0);

ClientToScreen (&pt);
bkGnd.CreateStockObject (BLACK_BRUSH);
bkGnd.UnrealizeObject ();
pDC->SetBrushOrg (pt);
pOldBrush = pDC->SelectObject (&bkGnd);
GetClientRect (rc);
pDC->FillRect (&rc, &bkGnd);
pDC->SelectObject (pOldBrush);
bkGnd.DeleteObject ();
return 1;
}

								/*---------------------------*/

BOOL CTextureView::OnMouseWheel (UINT nFlags, short zDelta, CPoint pt)
{
	CRect	rc;

GetWindowRect (rc);
if ((pt.x < rc.left) || (pt.x >= rc.right) || (pt.y < rc.top) || (pt.y >= rc.bottom))
	return 1;

int nPos = GetScrollPos (SB_VERT) - zDelta / WHEEL_DELTA;

if (nPos >= m_nRows [ShowAll ()])
	nPos = m_nRows [ShowAll ()] - m_viewSpace.cy;
if (nPos < 0)
	nPos = 0;
SetScrollPos (SB_VERT, nPos, TRUE);
Refresh ();
return 0;
}

								/*---------------------------*/

void CTextureView::OnSize (UINT nType, int cx, int cy)
{
CWnd::OnSize (nType, cx, cy);
}

								/*---------------------------*/

afx_msg void CTextureView::OnPaint ()
{
	CRect	rc;
	CDC	*pDC;
	PAINTSTRUCT	ps;

if (!GetUpdateRect (rc))
	return;
pDC = BeginPaint (&ps);
RecalcLayout ();
EndPaint (&ps);
}
								/*---------------------------*/
