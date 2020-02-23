 // dlcView.cpp : implementation of the CMineView class
//

#include <math.h>
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

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE [] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolView

BEGIN_MESSAGE_MAP(CTextureLightTool, CTextureTabDlg)
	ON_WM_PAINT ()
	ON_WM_HSCROLL ()
	ON_WM_VSCROLL ()
	ON_WM_LBUTTONDOWN ()
	ON_BN_CLICKED (IDC_TEXLIGHT_1, OnLight1)
	ON_BN_CLICKED (IDC_TEXLIGHT_2, OnLight2)
	ON_BN_CLICKED (IDC_TEXLIGHT_3, OnLight3)
	ON_BN_CLICKED (IDC_TEXLIGHT_4, OnLight4)
	ON_BN_CLICKED (IDC_TEXLIGHT_5, OnLight5)
	ON_BN_CLICKED (IDC_TEXLIGHT_6, OnLight6)
	ON_BN_CLICKED (IDC_TEXLIGHT_7, OnLight7)
	ON_BN_CLICKED (IDC_TEXLIGHT_8, OnLight8)
	ON_BN_CLICKED (IDC_TEXLIGHT_9, OnLight9)
	ON_BN_CLICKED (IDC_TEXLIGHT_10, OnLight10)
	ON_BN_CLICKED (IDC_TEXLIGHT_11, OnLight11)
	ON_BN_CLICKED (IDC_TEXLIGHT_12, OnLight12)
	ON_BN_CLICKED (IDC_TEXLIGHT_13, OnLight13)
	ON_BN_CLICKED (IDC_TEXLIGHT_14, OnLight14)
	ON_BN_CLICKED (IDC_TEXLIGHT_15, OnLight15)
	ON_BN_CLICKED (IDC_TEXLIGHT_16, OnLight16)
	ON_BN_CLICKED (IDC_TEXLIGHT_17, OnLight17)
	ON_BN_CLICKED (IDC_TEXLIGHT_18, OnLight18)
	ON_BN_CLICKED (IDC_TEXLIGHT_19, OnLight19)
	ON_BN_CLICKED (IDC_TEXLIGHT_20, OnLight20)
	ON_BN_CLICKED (IDC_TEXLIGHT_21, OnLight21)
	ON_BN_CLICKED (IDC_TEXLIGHT_22, OnLight22)
	ON_BN_CLICKED (IDC_TEXLIGHT_23, OnLight23)
	ON_BN_CLICKED (IDC_TEXLIGHT_24, OnLight24)
	ON_BN_CLICKED (IDC_TEXLIGHT_25, OnLight25)
	ON_BN_CLICKED (IDC_TEXLIGHT_26, OnLight26)
	ON_BN_CLICKED (IDC_TEXLIGHT_27, OnLight27)
	ON_BN_CLICKED (IDC_TEXLIGHT_28, OnLight28)
	ON_BN_CLICKED (IDC_TEXLIGHT_29, OnLight29)
	ON_BN_CLICKED (IDC_TEXLIGHT_30, OnLight30)
	ON_BN_CLICKED (IDC_TEXLIGHT_31, OnLight31)
	ON_BN_CLICKED (IDC_TEXLIGHT_32, OnLight32)
	ON_BN_CLICKED (IDC_TEXLIGHT_OFF, OnLightOff)
	ON_BN_CLICKED (IDC_TEXLIGHT_ON, OnLightOn)
	ON_BN_CLICKED (IDC_TEXLIGHT_STROBE4, OnLightStrobe4)
	ON_BN_CLICKED (IDC_TEXLIGHT_STROBE8, OnLightStrobe8)
	ON_BN_CLICKED (IDC_TEXLIGHT_FLICKER, OnLightFlicker)
	ON_BN_CLICKED (IDC_TEXLIGHT_ADD, OnAddLight)
	ON_BN_CLICKED (IDC_TEXLIGHT_DELETE, OnDeleteLight)
	ON_BN_CLICKED (IDC_TEXLIGHT_RGBCOLOR, OnSelectColor)
	ON_EN_KILLFOCUS (IDC_TEXLIGHT_TIMER, OnLightTimerEdit)
	ON_EN_KILLFOCUS (IDC_TEXLIGHT_EDIT, OnLightEdit)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

CTextureLightTool::~CTextureLightTool ()
{
	if (IsWindow (m_lightWnd))
		m_lightWnd.DestroyWindow ();
	if (IsWindow (m_colorWnd))
		m_colorWnd.DestroyWindow ();
	if (IsWindow (m_paletteWnd))
		m_paletteWnd.DestroyWindow ();
}


//------------------------------------------------------------------------------

void CTextureLightTool::UpdateColorData (void)
{
CWall* pWall = current->Wall ();
CColor* pColor = current->LightColor ();
m_nColorIndex = ((pWall != null) && (pWall->Type () == WALL_COLORED)) ? pWall->Info ().cloakValue : pColor->m_info.index;
m_rgbColor.peRed = (char) (255.0 * pColor->m_info.color.r);
m_rgbColor.peGreen = (char) (255.0 * pColor->m_info.color.g);
m_rgbColor.peBlue = (char) (255.0 * pColor->m_info.color.b);
}

//------------------------------------------------------------------------------

BOOL CTextureLightTool::OnInitDialog ()
{
if (!CTabDlg::OnInitDialog ())
	return FALSE;

m_nLightDelay = 1000;
m_nLightTime = 1.0;
m_iLight = -1;
m_nHighlight = -1;
memset (m_szLight, 0, sizeof (m_szLight));
m_bLightEnabled = TRUE;
m_nLightTimer = -1;

UpdateColorData ();

m_btnAddLight.AutoLoad (IDC_TEXLIGHT_ADD, this);
m_btnDelLight.AutoLoad (IDC_TEXLIGHT_DELETE, this);

m_lightTimerCtrl.Init (this, -IDC_TEXLIGHT_TIMERSLIDER, IDC_TEXLIGHT_TIMERSPINNER, IDC_TEXLIGHT_TIMER, 0, 1000, 50.0, 1.0, 2);
CreateColorCtrl (&m_lightWnd, IDC_TEXLIGHT_SHOW);
UpdateLightWnd ();
CreateColorCtrl (&m_colorWnd, IDC_TEXLIGHT_COLOR);
m_paletteWnd.Create (GetDlgItem (IDC_TEXLIGHT_PALETTE), -1, -1);

m_bInited = TRUE;
UpdateData (FALSE);
return TRUE;
}

//------------------------------------------------------------------------------

void CTextureLightTool::DoDataExchange (CDataExchange * pDX)
{
if (!HaveData (pDX))
	return;

//DDX_Double (pDX, IDC_TEXLIGHT_TIMER, m_nLightTime);
DDX_Text (pDX, IDC_TEXLIGHT_EDIT, m_szLight, sizeof (m_szLight));
DDX_Text (pDX, IDC_TEXLIGHT_COLORINDEX, m_nColorIndex);
}

//------------------------------------------------------------------------------

void CTextureLightTool::OnPaint ()
{
if (theMine == null) 
	return;
CTextureTabDlg::OnPaint ();
UpdatePaletteWnd ();
}

//------------------------------------------------------------------------------

bool CTextureLightTool::Refresh (void)
{
UpdateLightWnd ();
UpdatePaletteWnd ();
UpdateData (FALSE);
return true;
}

//------------------------------------------------------------------------------

BOOL CTextureLightTool::OnSetActive ()
{
if (m_iLight >= 0)
	m_nLightTimer = m_pParentWnd->SetTimer (2, m_nLightDelay, null);
Refresh ();
return CTextureTabDlg::OnSetActive ();
}

//------------------------------------------------------------------------------

BOOL CTextureLightTool::OnKillActive ()
{
if (m_nLightTimer >= 0) {
	m_pParentWnd->KillTimer (m_nLightTimer);
	m_nLightTimer = -1;
	}
return CTextureTabDlg::OnKillActive ();
}

//------------------------------------------------------------------------------

BOOL CTextureLightTool::HandleTimer (UINT_PTR nIdEvent)
{
if (nIdEvent == 2) {
	AnimateLight ();
	return TRUE;
	}
return FALSE;
}

//------------------------------------------------------------------------------

void CTextureLightTool::AnimateLight (void)
{
LightButton (m_nHighlight)->SetState (0);
m_nHighlight++;
m_nHighlight &= 0x1f; // 0..31

CButton *pb = LightButton (m_nHighlight);
pb->SetState (1);

CRect rc;
pb->GetClientRect (rc);
rc.InflateRect (-3, -4);
rc.left += 2;
rc.right++;
CDC *pDC = pb->GetDC ();
COLORREF	color = pb->GetCheck () ? RGB (0,0,0) : RGB (196,196,196);
CBrush br;
br.CreateSolidBrush (color);
CBrush *pOldBrush = pDC->SelectObject (&br);
CPen pen;
pen.CreatePen (PS_SOLID, 1, color);
CPen *pOldPen = pDC->SelectObject (&pen);
pDC->Ellipse (&rc);
pDC->SelectObject (pOldPen);
pDC->SelectObject (pOldBrush);
pb->ReleaseDC (pDC);
UpdateColorCtrl (
	&m_lightWnd, 
	pb->GetCheck () ? 
		(m_nColorIndex > 0) ?
			RGB (m_rgbColor.peRed, m_rgbColor.peGreen, m_rgbColor.peBlue) :
			RGB (255,196,0) : 
		RGB (0,0,0));
}

//------------------------------------------------------------------------------

void CTextureLightTool::ToggleLight (int i)
{
CButton *pb = LightButton (i - 1);
pb->SetCheck (!pb->GetCheck ());
SetLightString ();
}

//------------------------------------------------------------------------------

void CTextureLightTool::UpdateLight (void)
{
	bool bChange = false;

if (m_iLight < 0)
	return;
if (m_iLight >= MAX_VARIABLE_LIGHTS)
	return;

uint nLightMask = 0;
for (int i = 0; i < 32; i++)
	if (m_szLight [i] == '1')
		nLightMask |= (1 << i);
short nDelay = (short) (I2X (m_nLightDelay) / 1000);

CVariableLight* pLight = lightManager.VariableLight (m_iLight);
if ((pLight->m_info.mask != nLightMask) || (pLight->m_info.delay != nDelay)) {
	undoManager.Begin (__FUNCTION__, udVariableLights);
	pLight->m_info.mask = nLightMask;
	pLight->m_info.delay = nDelay;
	undoManager.End (__FUNCTION__);
	}
//m_nLightDelay = (1000 * nDelay + F0_5) / F1_0;
}

//------------------------------------------------------------------------------

bool CTextureLightTool::SetLightDelay (int nSpeed)
{
if (nSpeed < 0)
	return false;
if (m_nLightTimer)
	m_pParentWnd->KillTimer (m_nLightTimer);
if ((m_iLight >= 0) && nSpeed) {
	m_nLightDelay = nSpeed;
	m_nLightTimer = m_pParentWnd->SetTimer (2, m_nLightDelay, null);
#if 1
	m_lightTimerCtrl.SetValue (m_nLightDelay);
#else
	((CSliderCtrl *) TimerSlider ())->SetPos (20 - (m_nLightDelay + 25) / 50);
#endif
	}
else {
	m_nLightDelay = 0;
	m_nLightTimer = -1;
	}
m_nLightTime = m_nLightDelay / 1000.0;
UpdateLight ();
UpdateData (FALSE);
return true;
}

//------------------------------------------------------------------------------

void CTextureLightTool::SetLightString (void)
{
	static char cLight [2] = {'0', '1'};
	char szLight [33];

int i;
for (i = 0; i < 32; i++)
	szLight [i] = cLight [LightButton (i)->GetCheck ()];
szLight [32] = '\0';
if (strcmp (szLight, m_szLight)) {
	strcpy_s (m_szLight, sizeof (m_szLight), szLight);
	UpdateLight ();
	UpdateData (FALSE);
	}
}
		
//------------------------------------------------------------------------------

void CTextureLightTool::SetLightButtons (LPSTR szLight, int nSpeed)
{
	bool	bDefault = false;

if (szLight) {
	if (szLight != m_szLight)
		strcpy_s (m_szLight, sizeof (m_szLight), szLight);
	}
else
	UpdateData (TRUE);
int i;
for (i = 0; i < 32; i++) {
	if (!bDefault && (m_szLight [i] == '\0'))
		bDefault = true;
	if (bDefault)
		m_szLight [i] = '0';
	LightButton (i)->SetCheck (m_szLight [i] == '1');
	}
m_szLight [32] = '\0';
if (!SetLightDelay (nSpeed)) {
	UpdateLight ();
	UpdateData (FALSE);
	}
}

//------------------------------------------------------------------------------

void CTextureLightTool::EnableLightControls (BOOL bEnable)
{
int i;
for (i = IDC_TEXLIGHT_OFF; i <= IDC_TEXLIGHT_TIMER; i++)
	GetDlgItem (i)->EnableWindow (bEnable);
}

//------------------------------------------------------------------------------

void CTextureLightTool::UpdateLightWnd (void)
{
CHECKMINE;

CWall *pWall = current->Wall ();
if (!SideHasLight ()) {
	if (m_bLightEnabled)
		EnableLightControls (m_bLightEnabled = FALSE);
	if (DLE.IsD2XLevel ())
		current->LightColor ()->Clear ();
	}
else {
	if (!m_bLightEnabled)
		EnableLightControls (m_bLightEnabled = TRUE);
	if (DLE.IsD2XLevel ()) {
		CColor *plc = current->LightColor ();
		if (!plc->m_info.index) {	// set light color to white for new lights
			plc->m_info.index = 255;
			plc->m_info.color.r =
			plc->m_info.color.g =
			plc->m_info.color.b = 1.0;
			}
		}
	}
m_iLight = lightManager.VariableLight ();
if (m_iLight < 0) {
	OnLightOff ();
	return;
	}

long nLightMask = lightManager.VariableLight (m_iLight)->m_info.mask;
int i;
for (i = 0; i < 32; i++)
	m_szLight [i] = (nLightMask & (1 << i)) ? '1' : '0';
m_szLight [32] = '\0';
SetLightButtons (m_szLight, (int) (((1000 * lightManager.VariableLight (m_iLight)->m_info.delay + F0_5) / F1_0)));
}

//------------------------------------------------------------------------------

void CTextureLightTool::OnLightEdit ()
{
if (m_iLight < 0)
	UpdateData (FALSE);
else {
	UpdateData (TRUE);
	SetLightButtons ();
	}
}

//------------------------------------------------------------------------------

void CTextureLightTool::OnLightTimerEdit ()
{
#if 1
if (m_lightTimerCtrl.OnEdit ())
	SetLightDelay (m_lightTimerCtrl.GetValue ());
#else
if (m_iLight < 0)
	UpdateData (FALSE);
else {
	UpdateData (TRUE);
	SetLightDelay ((int) (1000 * m_nLightTime));
	}
#endif
}

//------------------------------------------------------------------------------

void CTextureLightTool::OnAddLight ()
{
if (m_iLight >= 0)
	INFOMSG (" There is already a variable light.")
else if (0 <= (m_iLight = lightManager.AddVariableLight (*current, 0xAAAAAAAAL, F1_0 / 4))) {
	UpdateLightWnd ();
	DLE.MineView ()->Refresh ();
	}
}

//------------------------------------------------------------------------------

void CTextureLightTool::OnDeleteLight ()
{
if (m_iLight < 0)
	INFOMSG (" There is no variable light.")
else if (lightManager.DeleteVariableLight ()) {
	m_iLight = -1;
	UpdateLightWnd ();
	DLE.MineView ()->Refresh ();
	}
}

//------------------------------------------------------------------------------

void CTextureLightTool::OnLightOff () { SetLightButtons ("", 1000); }
void CTextureLightTool::OnLightOn () { SetLightButtons ("11111111111111111111111111111111", 1000); }
void CTextureLightTool::OnLightStrobe4 () { SetLightButtons ("10000000100000001000000010000000", 250); }
void CTextureLightTool::OnLightStrobe8 () { SetLightButtons ("10001000100010001000100010001000", 250); }
void CTextureLightTool::OnLightFlicker () { SetLightButtons ("11111111000000111100010011011110", 100); }
void CTextureLightTool::OnLightDefault () { SetLightButtons ("10101010101010101010101010101010", 250); }

//------------------------------------------------------------------------------

void CTextureLightTool::OnLight1 () { ToggleLight (1); }
void CTextureLightTool::OnLight2 () { ToggleLight (2); }
void CTextureLightTool::OnLight3 () { ToggleLight (3); }
void CTextureLightTool::OnLight4 () { ToggleLight (4); }
void CTextureLightTool::OnLight5 () { ToggleLight (5); }
void CTextureLightTool::OnLight6 () { ToggleLight (6); }
void CTextureLightTool::OnLight7 () { ToggleLight (7); }
void CTextureLightTool::OnLight8 () { ToggleLight (8); }
void CTextureLightTool::OnLight9 () { ToggleLight (9); }
void CTextureLightTool::OnLight10 () { ToggleLight (10); }
void CTextureLightTool::OnLight11 () { ToggleLight (11); }
void CTextureLightTool::OnLight12 () { ToggleLight (12); }
void CTextureLightTool::OnLight13 () { ToggleLight (13); }
void CTextureLightTool::OnLight14 () { ToggleLight (14); }
void CTextureLightTool::OnLight15 () { ToggleLight (15); }
void CTextureLightTool::OnLight16 () { ToggleLight (16); }
void CTextureLightTool::OnLight17 () { ToggleLight (17); }
void CTextureLightTool::OnLight18 () { ToggleLight (18); }
void CTextureLightTool::OnLight19 () { ToggleLight (19); }
void CTextureLightTool::OnLight20 () { ToggleLight (20); }
void CTextureLightTool::OnLight21 () { ToggleLight (21); }
void CTextureLightTool::OnLight22 () { ToggleLight (22); }
void CTextureLightTool::OnLight23 () { ToggleLight (23); }
void CTextureLightTool::OnLight24 () { ToggleLight (24); }
void CTextureLightTool::OnLight25 () { ToggleLight (25); }
void CTextureLightTool::OnLight26 () { ToggleLight (26); }
void CTextureLightTool::OnLight27 () { ToggleLight (27); }
void CTextureLightTool::OnLight28 () { ToggleLight (28); }
void CTextureLightTool::OnLight29 () { ToggleLight (29); }
void CTextureLightTool::OnLight30 () { ToggleLight (30); }
void CTextureLightTool::OnLight31 () { ToggleLight (31); }
void CTextureLightTool::OnLight32 () { ToggleLight (32); }

//------------------------------------------------------------------------------

void CTextureLightTool::SetWallColor (void)
{
if (lightManager.ApplyFaceLightSettingsGlobally ()) {
	short			nSegment, nSide;
	short			nBaseTex = current->Side ()->BaseTex ();
	CSegment*	pSegment = segmentManager.Segment (0);
	CSide*		pSide;
	CWall			*pWall;
	bool			bAll = !segmentManager.HasTaggedSides ();

	for (nSegment = 0; nSegment < segmentManager.Count (); nSegment++, pSegment++) {
		for (nSide = 0, pSide = pSegment->m_sides; nSide < 6; nSide++, pSide++) {
			if (pSide->m_info.nWall < 0)
				continue;
			pWall = wallManager.Wall (pSide->m_info.nWall);
			if ((pWall == null) || !pWall->IsTransparent ())
				continue;
			if (!(bAll || segmentManager.IsTagged (CSideKey (nSegment, nSide))))
				continue;
			if (pSide->BaseTex () != nBaseTex)
				continue;
			pWall->Info ().cloakValue = m_nColorIndex;
			}
		}
	}
}

//------------------------------------------------------------------------------

void CTextureLightTool::OnLButtonDown (UINT nFlags, CPoint point)
{
	CRect		rcPal;

if (/*(DLE.IsD2XLevel ()) &&*/ SideHasLight ()) {
	GetCtrlClientRect (&m_paletteWnd, rcPal);
	if (PtInRect (rcPal, point)) {
		point.x -= rcPal.left;
		point.y -= rcPal.top;
		if (m_paletteWnd.SelectColor (point, m_nColorIndex, &m_rgbColor)) {
			CWall *pWall = current->Wall ();
			if ((pWall != null) && (pWall->Type () == WALL_COLORED)) {
				pWall->Info ().cloakValue = m_nColorIndex;
				SetWallColor ();
				}
			CColor *psc = current->LightColor ();
			if (psc->m_info.index = m_nColorIndex) {
				psc->m_info.color.r = (double) m_rgbColor.peRed / 255.0;
				psc->m_info.color.g = (double) m_rgbColor.peGreen / 255.0;
				psc->m_info.color.b = (double) m_rgbColor.peBlue / 255.0;
				}
			else {
				psc->m_info.color.r =
				psc->m_info.color.g =
				psc->m_info.color.b = 1.0;
				}
			//if (!pWall || (pWall->Type () != WALL_COLORED)) 
				{
				lightManager.SetTexColor (current->Side ()->BaseTex (), psc);
				lightManager.SetTexColor (current->Side ()->OvlTex (0), psc);
				}
			UpdateData (FALSE);
			UpdatePaletteWnd ();
			}
		}
	}
}

//------------------------------------------------------------------------------
		
void CTextureLightTool::OnSelectColor ()
{
if (CDlgHelpers::SelectColor (m_rgbColor.peRed, m_rgbColor.peGreen, m_rgbColor.peBlue)) {
	CColor *psc = current->LightColor ();
	psc->m_info.index = m_nColorIndex = 255;
	psc->m_info.color.r = (double) m_rgbColor.peRed / 255.0;
	psc->m_info.color.g = (double) m_rgbColor.peGreen / 255.0;
	psc->m_info.color.b = (double) m_rgbColor.peBlue / 255.0;
	lightManager.SetTexColor (current->Side ()->BaseTex (), psc);
	lightManager.SetTexColor (current->Side ()->OvlTex (0), psc);
	UpdatePaletteWnd ();
	}
}

//------------------------------------------------------------------------------

bool CTextureLightTool::SideHasLight (void)
{
if (theMine == null) return false;

if	((lightManager.IsLight (current->Side ()->BaseTex ()) != -1) ||
	 ((current->Side ()->OvlTex (0) != 0) &&
	  (lightManager.IsLight (current->Side ()->OvlTex (0)) != -1)))
	return true;
CWall *pWall = current->Wall ();
return (pWall != null) && (pWall->Type () == WALL_COLORED);

}

//------------------------------------------------------------------------------

void CTextureLightTool::UpdatePaletteWnd (void)
{
if (m_paletteWnd.m_hWnd) {
	if (/*!nLayout && (DLE.IsD2XLevel ()) &&*/ SideHasLight ()) {
		UpdateColorData ();
		CDlgHelpers::EnableControls (IDC_TEXLIGHT_PALETTE + 1, IDC_TEXLIGHT_COLOR, TRUE);
		m_paletteWnd.ShowWindow (SW_SHOW);
		m_paletteWnd.DrawPalette ();
		UpdateColorCtrl (
			&m_colorWnd, 
			(m_nColorIndex > 0) ? 
			RGB (m_rgbColor.peRed, m_rgbColor.peGreen, m_rgbColor.peBlue) :
			RGB (0,0,0));
		}
	else {
		CDlgHelpers::EnableControls (IDC_TEXLIGHT_PALETTE + 1, IDC_TEXLIGHT_COLOR, FALSE);
		m_paletteWnd.ShowWindow (SW_HIDE);
		}
	}
}

//------------------------------------------------------------------------------

void CTextureLightTool::OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
if (m_lightTimerCtrl.OnScroll (scrollCode, thumbPos, pScrollBar)) {
	SetLightDelay (m_lightTimerCtrl.GetValue ());
	}
}

//------------------------------------------------------------------------------

void CTextureLightTool::OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
if (m_lightTimerCtrl.OnScroll (scrollCode, thumbPos, pScrollBar)) {
	SetLightDelay (m_lightTimerCtrl.GetValue ());
	}
}

//------------------------------------------------------------------------------

BOOL CTextureLightTool::OnNotify (WPARAM wParam, LPARAM lParam, LRESULT *pResult)
{
	LPNMHDR	nmHdr = (LPNMHDR) lParam;
	int		nMsg = nmHdr->code;

switch (wParam) {
	case IDC_TEXLIGHT_COLOR:
		return 0;
	default:
/*		if (((LPNMHDR) lParam)->code == WM_LBUTTONDOWN)
			OnLButtonDown ();
		else 
*/		return CTextureTabDlg::OnNotify (wParam, lParam, pResult);
	}
*pResult = 0;
return TRUE;
}

//------------------------------------------------------------------------------
		
		//eof texturedlg.cpp