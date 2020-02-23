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
#include "texedit.h"
#include "FileManager.h"
#include "TextureManager.h"
#include "AVLTree.h"
#include "SLL.h"
#include "PogDialog.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE [] = __FILE__;
#endif

#define TEXTOOLDLG 1

/////////////////////////////////////////////////////////////////////////////
// CToolView

BEGIN_MESSAGE_MAP(CTextureTool, CTexToolDlg)
	ON_WM_PAINT ()
	ON_WM_TIMER ()
	ON_WM_HSCROLL ()
	ON_WM_VSCROLL ()
	ON_BN_CLICKED (IDC_TEXTURE_SHOWFRAMES, LoadTextureListBoxes)
	ON_BN_CLICKED (IDC_TEXTURE_EDIT, OnEditTexture)
	ON_BN_CLICKED (IDC_TEXTURE_COPY, OnSaveTexture)
	ON_BN_CLICKED (IDC_TEXTURE_PASTESIDE, OnPasteSide)
	ON_BN_CLICKED (IDC_TEXTURE_PASTETOUCHING, OnPasteTouching)
	ON_BN_CLICKED (IDC_TEXTURE_TAG_PLANE, OnTagPlane)
	ON_BN_CLICKED (IDC_TEXTURE_TAG_TEXTURES, OnTagTextures)
	ON_BN_CLICKED (IDC_TEXTURE_REPLACE, OnReplace)
	ON_BN_CLICKED (IDC_TEXTURE_PASTE1ST, OnPaste1st)
	ON_BN_CLICKED (IDC_TEXTURE_PASTE2ND, OnPaste2nd)
	ON_BN_CLICKED (IDC_TEXTURE_CLEANUP, OnCleanup)
	ON_CBN_SELCHANGE (IDC_TEXTURE_1ST, OnSelect1st)
	ON_CBN_SELCHANGE (IDC_TEXTURE_2ND, OnSelect2nd)
	ON_EN_KILLFOCUS (IDC_TEXTURE_LIGHT1, OnSetLight)
	ON_EN_KILLFOCUS (IDC_TEXTURE_LIGHT2, OnSetLight)
	ON_EN_KILLFOCUS (IDC_TEXTURE_LIGHT3, OnSetLight)
	ON_EN_KILLFOCUS (IDC_TEXTURE_LIGHT4, OnSetLight)
	ON_EN_KILLFOCUS (IDC_TEXTURE_BRIGHTNESS, OnBrightnessEdit)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

CTextureTool::CTextureTool (CPropertySheet *pParent)
	: CTexToolDlg (IDD_TEXTUREDATA_HORZ + nLayout, pParent, 1, IDC_TEXTURE_SHOW, RGB (0,0,0))
{
m_lastTexture [0] = -1;
m_lastTexture [1] = 0;
m_saveTexture [0] = -1;
m_saveTexture [1] = 0;
int i;
for (i = 0; i < 4; i++)
	m_saveUVLs [i].l = defaultUVLs [i].l;
#if TEXTOODLG == 0
m_frame [0] = 0;
m_frame [1] = 0;
#endif
m_bUse1st = TRUE;
m_bUse2nd = FALSE;
m_bInitTextureListBoxes = true;
}

//------------------------------------------------------------------------------

CTextureTool::~CTextureTool ()
{
if (m_bInited) {
	if (IsWindow (m_textureWnd))
		m_textureWnd.DestroyWindow ();
	}
}

//------------------------------------------------------------------------------

BOOL CTextureTool::OnInitDialog ()
{
	CRect	rc;

if (!CToolDlg::OnInitDialog ())
   return FALSE;

#if 0
m_btnZoomIn.EnableToolTips (TRUE);
m_btnZoomOut.EnableToolTips (TRUE);
m_btnHShrink.EnableToolTips (TRUE);
m_btnVShrink.EnableToolTips (TRUE);
m_btnHALeft.EnableToolTips (TRUE);
m_btnHARight.EnableToolTips (TRUE);
m_btnVAUp.EnableToolTips (TRUE);
m_btnVADown.EnableToolTips (TRUE);
m_btnRALeft.EnableToolTips (TRUE);
m_btnRARight.EnableToolTips (TRUE);
m_btnStretch2Fit.EnableToolTips (TRUE);
m_btnReset.EnableToolTips (TRUE);
m_btnResetTagged.EnableToolTips (TRUE);
m_btnChildAlign.EnableToolTips (TRUE);
m_btnAlignAll.EnableToolTips (TRUE);
m_btnAddLight.EnableToolTips (TRUE);
m_btnDelLight.EnableToolTips (TRUE);
#endif
#if 1
m_brightnessCtrl.Init (this, (nLayout == 1) ? -IDC_TEXTURE_BRIGHTSLIDER : IDC_TEXTURE_BRIGHTSLIDER, IDC_TEXTURE_BRIGHTSPINNER, IDC_TEXTURE_BRIGHTNESS, 0, 100, 1.0, 1.0, 10);
#else
InitSlider (IDC_TEXTURE_BRIGHTSLIDER, 0, 100);
BrightnessSlider ()->SetTicFreq (10);
InitSlider (IDC_TEXLIGHT_TIMERSLIDER, 0, 20);
BrightnessSpinner ()->SetRange (0, 100);
#endif
LoadTextureListBoxes ();

m_textureTools.Init (this);
if (nLayout) {
	m_textureTools.Add (new CTextureAlignTool (IDD_TEXALIGN_VERT, this), IDD_TEXALIGN_VERT, "Alignment");
	m_textureTools.Add (new CTextureProjectTool (IDD_TEXPROJ_VERT, this), IDD_TEXPROJ_VERT, "Projection");
	m_textureTools.Add (new CTextureLightTool (IDD_TEXLIGHT_VERT, this), IDD_TEXLIGHT_VERT, "Lighting");
	}
else {
	m_textureTools.Add (new CTextureAlignTool (IDD_TEXALIGN_HORZ, this), IDD_TEXALIGN_HORZ, "Alignment");
	m_textureTools.Add (new CTextureProjectTool (IDD_TEXPROJ_HORZ, this), IDD_TEXPROJ_HORZ, "Projection");
	m_textureTools.Add (new CTextureLightTool (IDD_TEXLIGHT_HORZ, this), IDD_TEXLIGHT_HORZ, "Lighting");
	}
m_textureTools.Setup ();

CreateImgWnd (&m_textureWnd, IDC_TEXTURE_SHOW);
UpdateData (FALSE);
m_bInited = true;
return TRUE;
}

//------------------------------------------------------------------------------

void CTextureTool::DoDataExchange (CDataExchange *pDX)
{
DDX_Control (pDX, IDC_TOOL_TAB, m_textureTools);
if (!HaveData (pDX)) 
	return;

for (int i = 0; i < 4; i++)
	DDX_Double (pDX, IDC_TEXTURE_LIGHT1 + i, m_lights [i], 0, 200, "%1.1f");
DDX_Check (pDX, IDC_TEXTURE_PASTE1ST, m_bUse1st);
DDX_Check (pDX, IDC_TEXTURE_PASTE2ND, m_bUse2nd);

#if 1
if (pDX->m_bSaveAndValidate)
	m_nBrightness = m_brightnessCtrl.GetValue ();
#else
int nBrightness;
char szBrightness [20];
sprintf_s (szBrightness, sizeof (szBrightness), "%d", m_nBrightness);
DDX_Text (pDX, IDC_TEXTURE_BRIGHTNESS, szBrightness, sizeof (szBrightness));
if (pDX->m_bSaveAndValidate && *szBrightness) {
	m_nBrightness = atoi (szBrightness);
	nBrightness = (m_nBrightness < 0) ? 0 : (m_nBrightness > 100) ? 100 : m_nBrightness;
	BrightnessSlider ()->SetPos (100 - nBrightness);
	BrightnessSpinner ()->SetPos (nBrightness);
	SetBrightness (nBrightness);
	}
#endif
}

//------------------------------------------------------------------------------

void CTextureTool::LoadTextureListBoxes (void) 
{
CHECKMINE;

	int			bShowFrames;
	int			nTextures, i, index;
	CComboBox	*cbTexture1 = CBTexture1 ();
	CComboBox	*cbTexture2 = CBTexture2 ();

bShowFrames = GetCheck (IDC_TEXTURE_SHOWFRAMES);

short texture1 = current->Side ()->BaseTex ();
short texture2 = current->Side ()->OvlTex (0);

if ((texture1 < 0) || (texture1 >= MAX_TEXTURES))
	texture1 = 0;
if ((texture2 < 0) || (texture2 >= MAX_TEXTURES))
	texture2 = 0;

cbTexture1->ResetContent ();
cbTexture2->ResetContent ();
index = cbTexture1->AddString ("(none)");
nTextures = textureManager.MaxTextures ();
for (i = 0; i < nTextures; i++) {
	char* p = textureManager.Name (-1, i);
	if (bShowFrames || !strstr (p, "frame")) {
		index = cbTexture1->AddString (p);
		cbTexture1->SetItemData (index, i);
		if (texture1 == i)
			cbTexture1->SetCurSel (index);
		index = cbTexture2->AddString (i ? p : "(none)");
		if (texture2 == i)
			cbTexture2->SetCurSel (index);
		cbTexture2->SetItemData (index, i);
		}
	}
CToolDlg::CBUpdateListWidth (cbTexture1);
CToolDlg::CBUpdateListWidth (cbTexture2);
}

//------------------------------------------------------------------------------

void CTextureTool::OnPaint ()
{
if (theMine == null) 
	return;
CTexToolDlg::OnPaint ();
#if TEXTOOLDLG == 0
UpdateTextureWnd ();
#endif
}

//------------------------------------------------------------------------------

void CTextureTool::UpdateTextureWnd (void)
{
#if TEXTOOLDLG == 0
RefreshTextureWnd ();
m_textureWnd.InvalidateRect (null);
m_textureWnd.UpdateWindow ();
#endif
}

//------------------------------------------------------------------------------

int lightIdxFromMode [4] = {0, 3, 2, 1};

void CTextureTool::Refresh (void)
{
if (!(m_bInited && theMine))
	return;
if (m_bInitTextureListBoxes) {
	m_bInitTextureListBoxes = false;
	LoadTextureListBoxes ();
	}

//Beep (1000,100);
	CComboBox	*cbTexture1 = CBTexture1 ();
	CComboBox	*cbTexture2 = CBTexture2 ();
	bool			bShowTexture;
	int			i, j;
// enable buttons as required
/*
EditButton->EnableWindow((IsD1File () || path [0] == null) ? FALSE: TRUE);
LightButton->EnableWindow((m_fileType==RDL_FILE) ? FALSE:TRUE);
PickButton->EnableWindow(path [0] ? TRUE:FALSE);
*/
// set animation frames to zero
#if TEXTOOLDLG == 0
m_frame [0] = 0;
m_frame [1] = 0;
#endif

CSegment* pSegment = current->Segment ();
CSide* pSide = current->Side ();
int nSide = current->SideId ();
short texture1 = pSide->BaseTex ();
short texture2 = pSide->OvlTex (0);
if ((texture1 < 0) || (texture1 >= MAX_TEXTURES))
	texture1 = 0;
if ((texture2 < 0) || (texture2 >= MAX_TEXTURES))
	texture2 = 0;
GetBrightness ((m_bUse2nd && !m_bUse1st) ? texture2 : texture1);
short ovlAlign = pSide->OvlAlignment ();
// set edit fields to % of light and enable them
for (i = 0; i < 4; i++) {
	j = (i + lightIdxFromMode [ovlAlign]) % 4;
	double h = 327.68 * lightManager.LightScale ();
	m_lights [j] = (pSide->m_info.uvls [i].l >= h) ? h / 327.68 : (double) (pSide->m_info.uvls [i].l) / 327.68;
	}

if (pSegment->ChildId (nSide)==-1)
	bShowTexture = TRUE;
else {
	ushort nWall = pSide->m_info.nWall;
	bShowTexture = (nWall >= 0) && (nWall < wallManager.WallCount ());
	}
if (bShowTexture) {
	if ((texture1 != m_lastTexture [0]) || (texture2 != m_lastTexture [1]) || (ovlAlign != m_lastMode)) {
		m_lastTexture [0] = texture1;
		m_lastTexture [1] = texture2;
		m_lastMode = ovlAlign;
		strcpy_s (message, sizeof (message), textureManager.Name (-1, texture1));
		cbTexture1->SetCurSel (i = cbTexture1->SelectString (-1, message));  // unselect if string not found
		if (texture2) {
			strcpy_s (message, sizeof (message), textureManager.Name (-1, texture2));
			cbTexture2->SetCurSel (cbTexture2->SelectString (-1, message));  // unselect if string not found
			}
		else
			cbTexture2->SelectString (-1, "(none)");
		}
	}
else {
	m_lastTexture [0] = -1;
	m_lastTexture [1] = -1;
	cbTexture1->SelectString (-1, "(none)");
	cbTexture2->SelectString (-1, "(none)");
	}
UpdateData (FALSE);
#if TEXTOOLDLG
CTexToolDlg::Refresh ();
#else
UpdateTextureWnd ();
#endif
Current ()->Refresh ();
}

//------------------------------------------------------------------------------

void CTextureTool::RefreshTextureWnd ()
{
CHECKMINE;

#if TEXTOOLDLG
if (!CTexToolDlg::Refresh ())
#else
if (!PaintTexture (&m_textureWnd))
#endif
	m_lastTexture [0] =
	m_lastTexture [1] = -1;
}

//------------------------------------------------------------------------------
#if TEXTOOLDLG == 0
void CTextureTool::DrawTexture (short texture1, short texture2, int x0, int y0) 
{
	ubyte bitmap_array [64*64];
	int x_offset;
	int y_offset;
	x_offset = 0;
	y_offset = 0;
	CDC	*pDC = m_textureWnd.GetDC ();
	CTexture tx (bmBuf);

memset (bitmap_array,0,sizeof (bitmap_array));
if (textureManager.BlendTextures(texture1,texture2,&tx,x0,y0))
	return;
BITMAPINFO *bmi;
bmi = MakeBitmap ();
CPalette	*oldPalette = pDC->SelectPalette (m_currentPalette, FALSE);
pDC->RealizePalette ();
//SetDIBitsToDevice (pDC->m_hDC, x_offset, y_offset, 64, 64, 0, 0, 0, 64, bitmap_array, bmi, DIB_RGB_COLORS);
CRect	rc;
m_textureWnd.GetClientRect (&rc);
StretchDIBits (pDC->m_hDC, x_offset, y_offset, rc.Width (), rc.Height (), 0, 0, tx.width, tx.height,
		        	(void *) bitmap_array, bmi, DIB_RGB_COLORS, SRCCOPY);
pDC->SelectPalette(oldPalette, FALSE);
m_textureWnd.ReleaseDC (pDC);
}
#endif
//------------------------------------------------------------------------------

BOOL CTextureTool::OnSetActive ()
{
#if TEXTOOLDLG
CTexToolDlg::OnSetActive ();
#else
m_nTimer = SetTimer (1, 100U, null);
#endif
Current ()->OnSetActive ();
Refresh ();
return TRUE; //CTexToolDlg::OnSetActive ();
}

//------------------------------------------------------------------------------

BOOL CTextureTool::OnKillActive ()
{
#if TEXTOOLDLG
CTexToolDlg::OnKillActive ();
#else
if (m_nTimer >= 0) {
	KillTimer (m_nTimer);
	m_nTimer = -1;
	}
#endif
Current ()->OnKillActive ();
return CToolDlg::OnKillActive ();
}

//------------------------------------------------------------------------------

#if TEXTOOLDLG == 0

void CTextureTool::AnimateTexture (void)
{
	CSegment *pSegment = current->Segment ();

	ushort texture [2];
	static int m_xScrollOffset [0] = 0;
	static int m_yScrollOffset [0] = 0;
	int bScroll;
	int x,y;
	static int m_xScrollOffset [1],m_yScrollOffset [1];

	CSide	*pSide = current->Side ();

texture [0] = pSide->BaseTex ();
texture [1] = pSide->OvlTex (1);

// if texture1 is a scrolling texture, then offset the textures and
// redraw them, then return
bScroll = textureManager.ScrollSpeed (texture [0], &x, &y);
if (bScroll) {
	DrawTexture (texture [0], texture [1], m_xScrollOffset [0], m_yScrollOffset [0]);
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
	return;
	}

m_xScrollOffset [0] = 0;
m_yScrollOffset [0] = 0;

// abort if this is not a wall
#ifndef _DEBUG
ushort nWall = pSide->m_info.nWall;
if (nWall >= wallManager.WallCount ())
	return;

// abort if this wall is not a door
if (wallManager.Wall () [nWall].type != WALL_DOOR)
	return;
#endif
	int i;
	static int hold_time [2] = {0,0};
	static int inc [2]= {1,1}; // 1=forward or -1=backwards
	int index [2];
	static ushort d1_anims [] = {
		371, 376, 387, 399, 413, 419, 424, 436, 444, 459,
		472, 486, 492, 500, 508, 515, 521, 529, 536, 543,
		550, 563, 570, 577, 584, 0
		};
// note: 584 is not an anim texture, but it is used to calculate
//       the number of frames in 577
// The 0 is used to end the search

	static ushort d2_anims [] = {
		435, 440, 451, 463, 477, 483, 488, 500, 508, 523,
		536, 550, 556, 564, 572, 579, 585, 593, 600, 608,
		615, 628, 635, 642, 649, 664, 672, 687, 702, 717,
		725, 731, 738, 745, 754, 763, 772, 780, 790, 806,
		817 ,827 ,838 ,849 ,858 ,863 ,871 ,886, 901 ,910,
		0
		};
// note: 910 is not an anim texture, but it is used to calculate
//       the number of frames in 901
// The 0 is used to end the search
	ushort *anim; // points to d1_anim or d2_anim depending on m_fileType

// first find out if one of the textures is animated
anim = (IsD1File ()) ? d1_anims : d2_anims;

for (i=0; i<2;i++)
	for (index [i] = 0; anim [index [i]]; index [i]++)
		if (texture [i] == anim [index [i]])
			break;

if (anim [index [0]] || anim [index [1]]) {
	// calculate new texture numbers
	for (i = 0; i < 2; i++) {
		if (anim [index [i]]) {
		// if hold time has not expired, then return
			if (hold_time [i] < 5)
				hold_time [i]++;
			else
				m_frame [i] += inc [i];
			if (m_frame [i] < 0) {
				m_frame [i] = 0;
				hold_time [i] = 0;
				inc [i] = 1;
				}
			if (anim [index [i]] + m_frame [i] >= anim [index [i] + 1]) {
				m_frame [i] = (anim [index [i] + 1] - anim [index [i]]) - 1;
				hold_time [i] = 0;
				inc [i] = -1;
				}
			texture [i] = anim [index [i]] + m_frame [i];
			}
		}
	DrawTexture (texture [0], texture [1], 0, 0);
	}
}
#endif
//------------------------------------------------------------------------------

void CTextureTool::OnTimer (UINT_PTR nIdEvent)
{
#if TEXTOOLDLG
if (!Current ()->HandleTimer (nIdEvent))
	CTexToolDlg::OnTimer (nIdEvent);
#else
if (nIdEvent == 1)
	AnimateTexture ();
else if (nIdEvent == 2)
	AnimateLight ();
else if (nIdEvent == 3)
	OnEditTimer ();
else 
	CToolDlg::OnTimer (nIdEvent);
#endif
}

//------------------------------------------------------------------------------

void CTextureTool::SelectTexture (int nIdC, bool bFirst)
{
CHECKMINE;

	CSide*		pSide = current->Side ();
	CComboBox*	pcb = bFirst ? CBTexture1 () : CBTexture2 ();
	int			index = pcb->GetCurSel ();
	
if (index <= 0)
	pSide->m_info.nOvlTex = 0;
else {
	short texture = (short) pcb->GetItemData (index);
	if (bFirst)
		segmentManager.SetTextures (*current, texture, -1);
	else
		segmentManager.SetTextures (*current, -1, texture);
	}
Refresh ();
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------------

void CTextureTool::OnSetLight () 
{
CHECKMINE;

UpdateData (TRUE);
CSide *pSide = current->Side ();
short ovlAlign = pSide->OvlAlignment ();
int i, j;
for (i = 0; i < 4; i++) {
	j = (i + lightIdxFromMode [ovlAlign]) % 4;
	pSide->m_info.uvls [i].l = (ushort) (m_lights [j] * 327.68);
	}
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------------

void CTextureTool::OnSelect1st () 
{
SelectTexture (IDC_TEXTURE_1ST, true);
}

//------------------------------------------------------------------------------

void CTextureTool::OnSelect2nd () 
{
SelectTexture (IDC_TEXTURE_2ND, false);
}

//------------------------------------------------------------------------------

void CTextureTool::OnEditTexture () 
{
int i = (int) CBTexture1 ()->GetItemData (CBTexture1 ()->GetCurSel ());
int j = m_bUse1st ? 0 : (int) CBTexture2 ()->GetItemData (CBTexture2 ()->GetCurSel ());

//CTextureEdit e (j != 0, textureManager.Name (-1, (short) (j ? j : i)));
CPogDialog e (DLE.MainFrame (), true, true, j ? j : i);

i = int (e.DoModal ());
DLE.MineView ()->Refresh (false);
Refresh ();
}

//------------------------------------------------------------------------------

void CTextureTool::OnSaveTexture () 
{
CHECKMINE;

CSide* pSide = current->Side ();
m_saveTexture [0] = pSide->BaseTex ();
m_saveTexture [1] = pSide->OvlTex (0);
int i;
for (i = 0; i < 4; i++)
	m_saveUVLs [i].l = pSide->m_info.uvls [i].l;
UpdateData (FALSE);
//SaveTextureStatic->SetText(message);
}

//------------------------------------------------------------------------------

void CTextureTool::OnPaste1st () 
{
m_bUse1st = !m_bUse1st;
Refresh ();
}

//------------------------------------------------------------------------------

void CTextureTool::OnPaste2nd () 
{
m_bUse2nd = !m_bUse2nd;
Refresh ();
}

//------------------------------------------------------------------------------

void CTextureTool::OnCleanup () 
{
textureManager.RemoveCustomTextures ();
Refresh ();
}

//------------------------------------------------------------------------------

void CTextureTool::OnPasteSide () 
{
CHECKMINE;

UpdateData (TRUE);
if (!(m_bUse1st || m_bUse2nd))
	return;

	CSide *pSide = current->Side ();

//CheckForDoor ();
undoManager.Begin (__FUNCTION__, udSegments);
segmentManager.SetTextures (*current, m_bUse1st ? m_saveTexture [0] : -1, m_bUse2nd ? m_saveTexture [1] : -1);
for (int i = 0; i < 4; i++)
	pSide->m_info.uvls [i].l = m_saveUVLs [i].l;
undoManager.End (__FUNCTION__);
Refresh ();
DLE.MineView ()->Refresh ();
}


//------------------------------------------------------------------------------

void CTextureTool::OnPasteTouching ()
{
CHECKMINE;

UpdateData (TRUE);
if (!(m_bUse1st || m_bUse2nd))
	return;
if (m_saveTexture [0] == -1 || m_saveTexture [1] == -1)
	return;
//CheckForDoor ();
// set all segment sides as not "pasted" yet
undoManager.Begin (__FUNCTION__, udSegments);
CSegment *pSegment = segmentManager.Segment (0);
for (short nSegment = segmentManager.Count (); nSegment; nSegment--, pSegment++)
	pSegment->Index () = 0;
PasteTexture (current->SegmentId (), current->SideId (), 1000);
undoManager.End (__FUNCTION__);
Refresh ();
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------------
// TODO: Beginning with the current side, walk through all adjacent sides and 
// mark all in plane

void CTextureTool::OnTagPlane () 
{
CHECKMINE;

UpdateData (TRUE);

CTagByAngle tagger;
if (tagger.Setup (segmentManager.VisibleSideCount ()))
	tagger.Run ();
Refresh ();
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------------

void CTextureTool::OnTagTextures () 
{
CHECKMINE;

UpdateData (TRUE);

CTagByTextures tagger (m_bUse1st ? current->Side ()->BaseTex () : -1, m_bUse2nd ? current->Side ()->OvlTex () : -1);
if (tagger.Setup (segmentManager.VisibleSideCount ()))
	tagger.Run ();
Refresh ();
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------------

void CTextureTool::OnReplace () 
{
CHECKMINE;

UpdateData (TRUE);
if (!(m_bUse1st || m_bUse2nd))
	return;

	short			nSegment,
					nSide;
	CSegment*	pSegment = segmentManager.Segment (0);
	CSide*		pSide;
	bool			bAll = !segmentManager.HasTaggedSegments (true);

if (bAll && (QueryMsg ("Replace textures in entire mine?") != IDYES))
	return;
undoManager.Begin (__FUNCTION__, udSegments);
if (bAll)
	INFOMSG (" Replacing textures in entire mine.");
for (nSegment = 0; nSegment < segmentManager.Count (); nSegment++, pSegment++)
	for (nSide = 0, pSide = pSegment->m_sides; nSide < 6; nSide++, pSide++)
		if (bAll || segmentManager.IsTagged (CSideKey (nSegment, nSide))) {
			if (m_bUse1st && (pSide->BaseTex () != m_lastTexture [0]))
				continue;
			if (m_bUse2nd && ((pSide->OvlTex (0)) != m_lastTexture [1]))
				continue;
			if ((pSegment->ChildId (nSide) >= 0) && (pSide->m_info.nWall == NO_WALL))
				 continue;
			segmentManager.SetTextures (CSideKey (nSegment, nSide), m_bUse1st ? m_saveTexture [0] : -1, m_bUse2nd ? m_saveTexture [1] : -1);
			}
undoManager.End (__FUNCTION__);
Refresh ();
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------------

void CTextureTool::PasteTexture (short nSegment, short nSide, short nDepth) 
{
CHECKMINE;

if (nDepth <= 0) 
	return;

	CSegment*	pSegment = segmentManager.Segment (nSegment);
	CSide*		pSide = pSegment->m_sides + nSide;
	short			oldTexture1, 
					oldTexture2;
	int			i;

// remember these texture for a comparison below
oldTexture1 = pSide->BaseTex ();
oldTexture2 = pSide->OvlTex ();
if ((oldTexture1 < 0) || (oldTexture1 >= MAX_TEXTURES))
	oldTexture1 = 0;
if ((oldTexture2 < 0) || (oldTexture2 >= MAX_TEXTURES))
	oldTexture2 = 0;
// mark segment as "pasted"
pSegment->Index () = 1;
// paste texture
segmentManager.SetTextures (CSideKey (nSegment, nSide), m_bUse1st ? m_saveTexture [0] : -1, m_bUse2nd ? m_saveTexture [1] : -1);
for (i = 0; i < 4; i++)
	pSide->m_info.uvls [i].l = m_saveUVLs [i].l;

// now check each adjing side to see it has the same texture
for (i = 0; i < pSide->VertexCount (); i++) {
	short nAdjSeg, nAdjSide;
	if (GetAdjacentSide (nSegment, nSide, i, &nAdjSeg, &nAdjSide)) {
		// if adj matches and its not "pasted" yet
		pSegment = segmentManager.Segment (nAdjSeg);
		pSide = pSegment->m_sides + nAdjSide;
		if ((pSegment->Index () == 0) &&
			 (!m_bUse1st || (pSide->BaseTex () == oldTexture1)) &&
			 (!m_bUse2nd || (pSide->OvlTex () == oldTexture2))) {
			PasteTexture (nAdjSeg, nAdjSide, --nDepth);
			}
		}
	}
}

//------------------------------------------------------------------------------

bool CTextureTool::GetAdjacentSide (short nStartSeg, short nStartSide, short nLine, short *nAdjSeg, short *nAdjSide) 
{
	CSegment *	pSegment;
	short			nSide, nChild;
	ushort		nEdgeVerts [2], nChildEdgeVerts [2];
	short			nChildSide, nChildLine;

// figure out which side of child shares two points w/ nStartSide
// find vert numbers for the line's two end points
pSegment = segmentManager.Segment (nStartSeg);
nEdgeVerts [0] = pSegment->VertexId (nStartSide, nLine);
nEdgeVerts [1] = pSegment->VertexId (nStartSide, nLine + 1);

nSide = pSegment->AdjacentSide (nStartSide, nEdgeVerts);
nChild = pSegment->ChildId (nSide);
if (nChild < 0 || nChild >= segmentManager.Count ())
	return false;
for (nChildSide = 0; nChildSide < 6; nChildSide++) {
	CSegment* pChildSeg = segmentManager.Segment (nChild);
	if ((pChildSeg->ChildId (nChildSide) == -1) || (pChildSeg->m_sides [nChildSide].Wall () != null)) {
		CSide* pChildSide = pChildSeg->Side (nChildSide);
		for (nChildLine = 0; nChildLine < pChildSide->VertexCount (); nChildLine++) {
			// find vert numbers for the line's two end points
			nChildEdgeVerts [0] = pChildSeg->VertexId (nChildSide, nChildLine);
			nChildEdgeVerts [1] = pChildSeg->VertexId (nChildSide, nChildLine + 1);
			// if points of child's line == corresponding points of parent
			if (((nChildEdgeVerts [0] == nEdgeVerts [1]) && (nChildEdgeVerts [1] == nEdgeVerts [0])) ||
				 ((nChildEdgeVerts [0] == nEdgeVerts [0]) && (nChildEdgeVerts [1] == nEdgeVerts [1]))) {
				// now we know the child's side & line which touches the parent
				// child:  nChild, nChildSide, nChildLine, nChildPoint0, nChildPoint1
				// parent: nStartSeg, nStartSide, nLine, point0, point1
				*nAdjSeg = nChild;
				*nAdjSide = nChildSide;
				return true;
				}
			}
		}
	}
return false;
}
//------------------------------------------------------------------------------

void CTextureTool::GetBrightness (int nTexture)
{
	int nBrightness;

if (nTexture < 0)
	nBrightness = 0;
else {
	nBrightness = int (*lightManager.LightMap (nTexture) /*/ TEXLIGHT_SCALE*/);
	if (nBrightness == MAX_TEXLIGHT)
		nBrightness = 100;
	else
		nBrightness = (100 * nBrightness + MAX_TEXLIGHT / 2) / MAX_TEXLIGHT;
#if 1
	m_brightnessCtrl.SetValue (nBrightness);
#else
	BrightnessSlider ()->SetPos (100 - nBrightness);
	BrightnessSpinner ()->SetPos (nBrightness);
#endif
	}
m_nBrightness = nBrightness;
}

//------------------------------------------------------------------------------

void CTextureTool::SetBrightness (int nBrightness)
{
	static	BOOL	bSemaphore = FALSE;

if (!bSemaphore) {
	bSemaphore = TRUE;

	CSide* pSide = current->Side ();
	short	texture = (m_bUse2nd && !m_bUse1st) ? pSide->OvlTex (0) : pSide->BaseTex ();

	if (texture >= 0) {
		m_nBrightness = nBrightness;
		nBrightness = int (nBrightness /** TEXLIGHT_SCALE*/);
		*lightManager.LightMap (texture) = ((nBrightness == 100) ? MAX_TEXLIGHT : nBrightness * (MAX_TEXLIGHT / 100));
		}
	Refresh ();
	bSemaphore = FALSE;
	}
}

//------------------------------------------------------------------------------

void CTextureTool::OnBrightnessEdit (void)
{
if (m_brightnessCtrl.OnEdit ()) {
	UpdateData (TRUE);
	SetBrightness (m_nBrightness);
	}
}

//------------------------------------------------------------------------------

void CTextureTool::OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
#if 1
if (m_brightnessCtrl.OnScroll (scrollCode, thumbPos, pScrollBar)) {
	SetBrightness (m_brightnessCtrl.GetValue ());
	}
#else
	int	nPos;

if (pScrollBar == (CScrollBar *) BrightnessSlider ()) {
	nPos = 100 - pScrollBar->GetScrollPos ();
	switch (scrollCode) {
		case SB_LINEUP:
			nPos--;
			break;
		case SB_LINEDOWN:
			nPos++;
			break;
		case SB_PAGEUP:
			nPos -= 10;
			break;
		case SB_PAGEDOWN:
			nPos += 10;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			nPos = thumbPos;
			break;
		case SB_ENDSCROLL:
			return;
		}
	if (nPos < 0)
		nPos = 0;
	else if (nPos > 100)
		nPos = 100;
	SetBrightness (nPos);
	UpdatePaletteWnd ();
	UpdateData (FALSE);
	pScrollBar->SetScrollPos (100 - nPos, TRUE);
	}
#endif
}

//------------------------------------------------------------------------------

void CTextureTool::OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
#if 1
if (m_brightnessCtrl.OnScroll (scrollCode, thumbPos, pScrollBar)) {
	SetBrightness (m_brightnessCtrl.GetValue ());
	}
#else
	int	nPos;
	CRect	rc;

if (pScrollBar == (CScrollBar *) TimerSlider ()) {
	nPos = pScrollBar->GetScrollPos ();
	if (m_iLight < 0)
		return;
	switch (scrollCode) {
		case SB_LINEUP:
			nPos++;
			break;
		case SB_LINEDOWN:
			nPos--;
			break;
		case SB_PAGEUP:
			nPos += 5;
			break;
		case SB_PAGEDOWN:
			nPos -= 5;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			nPos = 20 - thumbPos;
			break;
		case SB_ENDSCROLL:
			return;
		}
	if (nPos < 0)
		nPos = 0;
	else if (nPos > 20)
		nPos = 20;
	SetLightDelay (nPos * 50);
//	pScrollBar->SetScrollPos (nPos, TRUE);
	}
else if (pScrollBar == (CScrollBar *) BrightnessSlider ()) {
	nPos = 100 - pScrollBar->GetScrollPos ();
	switch (scrollCode) {
		case SB_LINEUP:
			nPos--;
			break;
		case SB_LINEDOWN:
			nPos++;
			break;
		case SB_PAGEUP:
			nPos -= 10;
			break;
		case SB_PAGEDOWN:
			nPos += 10;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			nPos = thumbPos;
			break;
		case SB_ENDSCROLL:
			return;
		}
	if (nPos < 0)
		nPos = 0;
	else if (nPos > 100)
		nPos = 100;
	SetBrightness (nPos);
	BrightnessSpinner ()->SetPos (100 - nPos);
	UpdateData (FALSE);
	pScrollBar->SetScrollPos (100 - nPos, TRUE);
	}
else if (pScrollBar == (CScrollBar *) BrightnessSpinner ()) {
	if (scrollCode != SB_THUMBPOSITION)
		return;
	nPos = thumbPos;
	SetBrightness (nPos);
	BrightnessSlider ()->SetPos (100 - nPos);
	UpdateData (FALSE);
	}
#endif
}

//------------------------------------------------------------------------------
		
		//eof texturedlg.cpp
