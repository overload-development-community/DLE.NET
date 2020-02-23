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

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolView

BEGIN_MESSAGE_MAP(CLightTool, CToolDlg)
	ON_WM_HSCROLL ()
	ON_BN_CLICKED (IDC_LIGHT_SHOWDELTA, OnShowVariableLights)
	ON_BN_CLICKED (IDC_LIGHT_QUALITY, OnSetQuality)
	ON_BN_CLICKED (IDC_LIGHT_SET_VERTEXLIGHT, OnSetVertexLight)
	ON_BN_CLICKED (IDC_DEFAULT_LIGHT_AND_COLOR, OnDefaultLightAndColor)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

CLightTool::CLightTool (CPropertySheet *pParent)
	: CToolDlg (IDD_LIGHTDATA_HORZ + nLayout, pParent)
{
SetDefaults ();
}

//------------------------------------------------------------------------------

void CLightTool::SetDefaults (void)
{
m_bIlluminate = 1;
m_bAvgCornerLight = 1;
m_bScaleLight = 0;
m_bSegmentLight = 1;
m_bDynSegmentLights = 1;
m_bDeltaLight = 1;
m_fVertexLight = 50.0;
m_nNoLightDeltas = 2;
m_bCopyTexLights = 0;
m_staticRenderDepth = DEFAULT_LIGHT_RENDER_DEPTH;
m_deltaRenderDepth = DEFAULT_LIGHT_RENDER_DEPTH;
lightManager.SetRenderDepth ();
m_deltaLightFrameRate = 30;
}

//------------------------------------------------------------------------------

BOOL CLightTool::OnInitDialog ()
{
if (!CToolDlg::OnInitDialog ())
	return FALSE;
#if 1
m_renderControls [0].Init (this, IDC_LIGHT_RENDER_DEPTH_SLIDER, IDC_LIGHT_RENDER_DEPTH_SPINNER, -IDT_LIGHT_RENDER_DEPTH, 1, 10);
m_renderControls [1].Init (this, IDC_LIGHT_DELTA_RENDER_DEPTH_SLIDER, IDC_LIGHT_DELTA_RENDER_DEPTH_SPINNER, -IDT_LIGHT_DELTA_RENDER_DEPTH, 1, 10);
m_renderControls [2].Init (this, IDC_LIGHT_DELTA_FRAMERATE_SLIDER, IDC_LIGHT_DELTA_FRAMERATE_SPINNER, -IDT_LIGHT_DELTA_FRAMERATE, 10, 100, 10.0, 1.0, 10);
#else
InitSlider (IDC_LIGHT_RENDER_DEPTH, 1, 10);
InitSlider (IDC_LIGHT_DELTA_RENDER_DEPTH, 1, 10);
InitSlider (IDC_LIGHT_DELTA_FRAMERATE, 10, 100);
for (int i = 20; i < 100; i += 10)
	SlCtrl (IDC_LIGHT_DELTA_FRAMERATE)->SetTic (i);
#endif
m_bInited = true;
return TRUE;
}

//------------------------------------------------------------------------------

void CLightTool::DoDataExchange (CDataExchange * pDX)
{
if (!HaveData (pDX)) 
	return;
DDX_Check (pDX, IDC_LIGHT_ILLUMINATE, m_bIlluminate);
DDX_Check (pDX, IDC_LIGHT_AVGCORNERLIGHT, m_bAvgCornerLight);
DDX_Check (pDX, IDC_LIGHT_SCALE, m_bScaleLight);
DDX_Check (pDX, IDC_LIGHT_CUBELIGHT, m_bSegmentLight);
DDX_Check (pDX, IDC_LIGHT_DYNCUBELIGHTS, m_bDynSegmentLights);
DDX_Check (pDX, IDC_LIGHT_CALCDELTA, m_bDeltaLight);
DDX_Double (pDX, IDC_LIGHT_EDIT_ILLUMINATE, lightManager.LightScale (), 0, 1000, null, "brightness must be between 0 and 1000");
DDX_Double (pDX, IDC_LIGHT_EDIT_SCALE, lightManager.Brightness (), 0, 200, null, "light scale must be between 0 and 200%");
DDX_Double (pDX, IDC_LIGHT_EDIT_CUBELIGHT, lightManager.ObjectLight (), 0, 100, null, "robot brightness must be between 0 and 100%");
DDX_Double (pDX, IDC_LIGHT_EDIT_DELTA, lightManager.DeltaLightScale (), 0, 1000, null, "exploding/blinking light brightness must be between 0 and 1000");
DDX_Radio (pDX, IDC_LIGHT_DELTA_ALL, m_nNoLightDeltas);
#if 1
m_renderControls [0].DoDataExchange (pDX, m_staticRenderDepth);
m_renderControls [1].DoDataExchange (pDX, m_deltaRenderDepth);
m_renderControls [2].DoDataExchange (pDX, m_deltaLightFrameRate);
#else
DDX_Slider (pDX, IDC_LIGHT_RENDER_DEPTH, m_staticRenderDepth);
DDX_Slider (pDX, IDC_LIGHT_DELTA_RENDER_DEPTH, m_deltaRenderDepth);
DDX_Slider (pDX, IDC_LIGHT_DELTA_FRAMERATE, m_deltaLightFrameRate);
#endif
DDX_Double (pDX, IDC_LIGHT_VERTEXLIGHT, m_fVertexLight);
if (!pDX->m_bSaveAndValidate)
	DDX_Text (pDX, IDT_LIGHT_DELTA_FRAMERATE, m_deltaLightFrameRate);
DDX_Check (pDX, IDC_LIGHT_QUALITY, lightManager.Quality ());
if (!pDX->m_bSaveAndValidate)
	((CWnd *) GetDlgItem (IDC_LIGHT_SHOWDELTA))->SetWindowText (DLE.MineView ()->RenderVariableLights () ? "stop" : "animate");
DDX_Check (pDX, IDC_LIGHT_COPYTEXLIGHTS, m_bCopyTexLights);
}

//------------------------------------------------------------------------------

void CLightTool::OnOK ()
{
CHECKMINE;

	bool		bAll;

UpdateData (TRUE);
// make sure there are marked blocks
theMine->m_nNoLightDeltas = m_nNoLightDeltas;
lightManager.SetRenderDepth (m_staticRenderDepth, m_deltaRenderDepth);
if (bAll = !segmentManager.HasTaggedSides ())
	STATUSMSG (" illuminating entire mine ...");
undoManager.Begin (__FUNCTION__, udLight | udSegments);
if (m_bIlluminate)
	lightManager.ComputeStaticLight (bAll, m_bCopyTexLights != 0);
if (m_bAvgCornerLight)
	lightManager.CalcAverageCornerLight (bAll);
if (m_bScaleLight)
	lightManager.ScaleCornerLight (bAll);
if (m_bSegmentLight)
	lightManager.SetObjectLight ((int) bAll, m_bDynSegmentLights != 0);
if (m_bDeltaLight)
	lightManager.ComputeVariableLight ((int) bAll);
lightManager.Normalize ();
undoManager.End (__FUNCTION__);
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------------

void CLightTool::OnCancel ()
{
SetDefaults ();
UpdateData (FALSE);
}

//------------------------------------------------------------------------------

void CLightTool::OnSetQuality ()
{
lightManager.Quality () = BtnCtrl (IDC_LIGHT_QUALITY)->GetCheck ();
}

//------------------------------------------------------------------------------

void CLightTool::OnShowVariableLights ()
{
CHECKMINE;

if (!::IsWindow(m_hWnd))
	return;
UpdateData (TRUE);
if (DLE.MineView ()->RenderVariableLights ()) {
	((CWnd *) GetDlgItem (IDC_LIGHT_SHOWDELTA))->SetWindowText ("animate");
	DLE.MineView ()->EnableDeltaShading (0, m_deltaLightFrameRate, 0);
	}
else {
	((CWnd *) GetDlgItem (IDC_LIGHT_SHOWDELTA))->SetWindowText ("stop");
	DLE.MineView ()->EnableDeltaShading (1, m_deltaLightFrameRate, 0);
	}
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------------

void CLightTool::OnSetVertexLight ()
{
CHECKMINE;

	int	nVertexLight;

if (!::IsWindow(m_hWnd))
	return;
UpdateData (TRUE);
nVertexLight = (int) (m_fVertexLight * f1_0 / 100.0);

	CSegment*	pSegment = segmentManager.Segment (0);
	bool			bChange = false;

undoManager.Begin (__FUNCTION__, udSegments);
for (CSegmentIterator si; si; si++) {
	pSegment = &(*si);
	CSide* pSide = pSegment->m_sides;
	for (ubyte nSide = 0; nSide < 6; nSide++, pSide++) {
		for (int i = 0; i < 4; i++) {
			ushort nVertex = pSegment->m_info.vertexIds [pSegment->Side (nSide)->VertexIdIndex (i)];
			if (vertexManager.Status (nVertex) & TAGGED_MASK) {
				pSide->m_info.uvls [i].l = nVertexLight;
				bChange = true;
				}
			}
		}
	}
undoManager.End (__FUNCTION__);
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------------

void CLightTool::OnDefaultLightAndColor ()
{
lightManager.LoadDefaults ();
}

//------------------------------------------------------------------------------

void CLightTool::OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
#if 1
for (int i = 0; i < sizeofa (m_renderControls); i++) {
	if (m_renderControls [i].OnScroll (scrollCode, thumbPos, pScrollBar)) {
		UpdateData (TRUE);
		return;
		}
	}
CToolDlg::OnHScroll (scrollCode, thumbPos, pScrollBar);
#else
	int	nPos = pScrollBar->GetScrollPos ();
	CRect	rc;

if (pScrollBar == (CScrollBar *) GetDlgItem (IDC_LIGHT_DELTA_FRAMERATE)) {
	switch (scrollCode) {
		case SB_LINEUP:
			nPos++;
			break;
		case SB_LINEDOWN:
			nPos--;
			break;
		case SB_PAGEUP:
			nPos += 10;
			break;
		case SB_PAGEDOWN:
			nPos -= 10;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			nPos = thumbPos;
			break;
		case SB_ENDSCROLL:
			return;
		}
	if (nPos < 10)
		nPos = 10;
	else if (nPos > 100)
		nPos = 100;
	m_deltaLightFrameRate = nPos;
	UpdateData (FALSE);
	}
#endif
}

//------------------------------------------------------------------------------


//eof lightdlg.cpp