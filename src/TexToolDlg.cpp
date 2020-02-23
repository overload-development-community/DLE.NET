// dlcView.cpp : implementation of the CMineView class
//

#include "stdafx.h"
#include "afxpriv.h"
#include "dle-xp.h"

#include "dlcDoc.h"
#include "mineview.h"
#include "toolview.h"

#include "PaletteManager.h"
#include "textures.h"
#include "global.h"
#include "FileManager.h"
#include "Selection.h"
#include "TextureManager.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

IMPLEMENT_DYNCREATE(CTexToolDlg, CToolDlg)

BEGIN_MESSAGE_MAP(CTexToolDlg, CToolDlg)
	ON_WM_PAINT ()
	ON_WM_TIMER ()
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

CTexToolDlg::CTexToolDlg (UINT nIdTemplate, CPropertySheet *pParent, 
								  int nTexWndId, int nTimerId, COLORREF bkColor,
								  bool bOtherSegment)
	: CToolDlg (nIdTemplate, pParent)
{
m_nTexWndId = nTexWndId;
m_nTimerId = nTimerId;
m_bkColor = bkColor;
m_nTimer = -1;
m_frame [0] = 0;
m_frame [1] = 0;
m_bOtherSegment = bOtherSegment;
}

//------------------------------------------------------------------------------

CTexToolDlg::~CTexToolDlg ()
{
if (m_bInited) {
	if (IsWindow (m_textureWnd))
		m_textureWnd.DestroyWindow ();
	}
}

//------------------------------------------------------------------------------

BOOL CTexToolDlg::OnInitDialog ()
{
if (!CToolDlg::OnInitDialog ())
	return FALSE;
CreateImgWnd (&m_textureWnd, m_nTexWndId);
m_bInited = true;
return TRUE;
}

//------------------------------------------------------------------------------

BOOL CTexToolDlg::TextureIsVisible ()
{
return TRUE;
}

//------------------------------------------------------------------------------

bool CTexToolDlg::Refresh (short nBaseTex, short nOvlTex, short nVisible)
{
m_frame [0] = 0;
m_frame [1] = 0;
if (nVisible < 0)
	nVisible = (short) TextureIsVisible ();
if (nVisible > 0) {
	if (nBaseTex < 0) {
		short nSegment = m_bOtherSegment ? other->m_nSegment : current->SegmentId ();
		short nSide = m_bOtherSegment ? other->m_nSide : current->SideId ();
		if (nVisible = segmentManager.IsWall (CSideKey (nSegment, nSide))) {
			CSide *pSide = m_bOtherSegment ? other->Side () : current->Side ();
			nBaseTex = pSide->BaseTex ();
			nOvlTex = pSide->OvlTex (0);
			}
		}
	}
if (nVisible > 0)
	return PaintTexture (&m_textureWnd, m_bkColor, nBaseTex, nOvlTex);
return PaintTexture (&m_textureWnd, m_bkColor, 0);
}

//------------------------------------------------------------------------------

void CTexToolDlg::OnPaint ()
{
CToolDlg::OnPaint ();
if (m_textureWnd.m_hWnd) {
	if (TextureIsVisible ()) {
		CSide *pSide = m_bOtherSegment ? other->Side () : current->Side ();
		PaintTexture (&m_textureWnd, m_bkColor, pSide->BaseTex (), pSide->OvlTex (0));
		}
	else
		PaintTexture (&m_textureWnd, m_bkColor, -1);
	m_textureWnd.InvalidateRect (null);
	m_textureWnd.UpdateWindow ();
	}
}

//------------------------------------------------------------------------------

BOOL CTexToolDlg::OnSetActive ()
{
if (m_nTimerId >= 0)
	m_nTimer = SetTimer ((UINT) m_nTimerId, 100U, null);
return CToolDlg::OnSetActive ();
}

//------------------------------------------------------------------------------

BOOL CTexToolDlg::OnKillActive ()
{
if (m_nTimer >= 0) {
	KillTimer (m_nTimer);
	m_nTimer = -1;
	}
return CToolDlg::OnKillActive ();
}

//------------------------------------------------------------------------------

bool CTexToolDlg::ScrollTexture (short texIds [])
{
	int x, y;

if (!textureManager.CheckScrollingTexture (texIds [0], m_xScrollOffset, m_yScrollOffset, x, y))
	return false;
PaintTexture (&m_textureWnd, m_bkColor, texIds [0], texIds [1], m_xScrollOffset [0], m_yScrollOffset [0]);
textureManager.UpdateScrollingTexture (m_xScrollOffset, m_yScrollOffset, x, y);

if (!textureManager.ScrollSpeed (texIds [0], &x, &y)) {
	m_xScrollOffset [0] =
	m_yScrollOffset [0] = 0;
	return false;
	}

PaintTexture (&m_textureWnd, m_bkColor, texIds [0], texIds [1], m_xScrollOffset [0], m_yScrollOffset [0]);
//	DrawTexture (texIds [0], texIds [1], m_xScrollOffset [0], m_yScrollOffset [0]);
if (m_xScrollOffset [1] != x || m_yScrollOffset [1] != y) {
	m_xScrollOffset [0] = 0;
	m_yScrollOffset [0] = 0;
	}
m_xScrollOffset [1] = x;
m_yScrollOffset [1] = y;
m_xScrollOffset [0] += x;
m_yScrollOffset [0] += y;
m_xScrollOffset [0] &= 63;
m_yScrollOffset [0] &= 63;
return true;
}

//------------------------------------------------------------------------------

bool CTexToolDlg::UpdateTextureClip (short texIds [])
{
	static int direction [2] = {1, 1};
	static short prevTexIds [2] = {-1, -1};

	int			nVersion = DLE.IsD1File ();
	bool			bAnimate = false;
	CTexture*	textures [2] = { null, null };

for (int i = 0; i < (texIds [1] ? 2 : 1); i++) {
	if (textures [i] = textureManager.UpdateTextureClip (texIds [i], prevTexIds [i], m_frame [i], direction [i]))
		bAnimate = true;
	else
		textures [i] = textureManager.Textures (texIds [i]);
	}
if (!bAnimate)
	return false;
PaintTexture (&m_textureWnd, m_bkColor, textures [0], textures [1]);
return true;
}

//------------------------------------------------------------------------------

bool CTexToolDlg::AnimateTexture (void)
{
	CSegment *pSegment = m_bOtherSegment ? other->Segment () : current->Segment ();
	CSide	*pSide = m_bOtherSegment ? other->Side () : current->Side ();
	short texture [2] = { pSide->BaseTex (), pSide->OvlTex (0) };

return ScrollTexture (texture) || UpdateTextureClip (texture);
}

//------------------------------------------------------------------------------

void CTexToolDlg::OnTimer (UINT_PTR nIdEvent)
{
if (nIdEvent == (UINT) m_nTimerId)
	AnimateTexture ();
else 
	CToolDlg::OnTimer (nIdEvent);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CAnimTexWnd, CWnd)
	ON_WM_TIMER ()
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

CAnimTexWnd::CAnimTexWnd ()
	: CWnd ()
{
m_nAnimTimer = -1;
}

//------------------------------------------------------------------------------

void CAnimTexWnd::OnDestroy (void)
{
StopAnimation ();
}

//------------------------------------------------------------------------------

bool CAnimTexWnd::StopAnimation ()
{
	BOOL result = TRUE;

if (m_nAnimTimer >= 0) {
	result = KillTimer (m_nAnimTimer);
	m_nAnimTimer = -1;
	}

return TRUE == result;
}

//------------------------------------------------------------------------------

bool CAnimTexWnd::StartAnimation (const CTexture *pTexture)
{
if (m_nAnimTimer >= 0)
	StopAnimation ();

m_nAnimTimer = SetTimer (1, pTexture->FrameTime (), null);
m_nTexId = -int (pTexture->Index ()) - 1;
m_nFrame = 0;

if (0 == m_nAnimTimer) {
	// failed
	m_nAnimTimer = -1;
	return false;
	}
return true;
}

//------------------------------------------------------------------------------

void CAnimTexWnd::OnTimer (UINT_PTR nIdEvent)
{
if (nIdEvent != (UINT) m_nAnimTimer) 
	CWnd::OnTimer (nIdEvent);
else {
	if (!AnimateTexture (m_nTexId)) {
		const CTexture *pTexture = textureManager.Textures (m_nTexId);
		PaintTexture (this, IMG_BKCOLOR, pTexture->GetFrame (m_nFrame));
		m_nFrame = (m_nFrame + 1) % pTexture->GetFrameCount ();
		}
	}
}

//------------------------------------------------------------------------------

bool CAnimTexWnd::ScrollTexture (short nTexId)
{
	int x, y;

if (!textureManager.CheckScrollingTexture (nTexId, m_xScrollOffset, m_yScrollOffset, x, y))
	return false;
PaintTexture (this, IMG_BKCOLOR, nTexId, -1, m_xScrollOffset [0], m_yScrollOffset [0]);
textureManager.UpdateScrollingTexture (m_xScrollOffset, m_yScrollOffset, x, y);
return true;
}

//------------------------------------------------------------------------------

bool CAnimTexWnd::UpdateTextureClip (short nTexId)
{
	static int nDirection = 1;
	static short nPrevTexId = -1;

CTexture *pTexture = textureManager.UpdateTextureClip (nTexId, nPrevTexId, m_nFrame, nDirection);
if (!pTexture)
	return false;
PaintTexture (this, IMG_BKCOLOR, pTexture);
return true;
}

//------------------------------------------------------------------------------

bool CAnimTexWnd::AnimateTexture (short nTexId)
{
return ScrollTexture (nTexId) || UpdateTextureClip (nTexId);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//eof tooldlg.cpp