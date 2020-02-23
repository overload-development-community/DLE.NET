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

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CColorControl, CWnd)
	ON_WM_ERASEBKGND ()
	ON_WM_PAINT ()
END_MESSAGE_MAP ()

//------------------------------------------------------------------------------

void CColorControl::OnPaint ()
{
if (GetSafeHwnd ()) {
	CDC *pDC = GetDC ();
	CRect rc;
	GetClientRect (rc);
	pDC->FillSolidRect (&rc, m_bkColor);
	ReleaseDC (pDC);
	}
CWnd::OnPaint ();
}

//------------------------------------------------------------------------------

void CColorControl::OnEraseBkGnd (CDC *pDC)
{
CRect rc;
GetClientRect (rc);
pDC->FillSolidRect (&rc, m_bkColor);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CDlgHelpers::DDX_Double (CDataExchange * pDX, int nIDC, double& fVal, double min, double max, LPSTR pszFmt, LPSTR pszErrMsg)
{
   HWND  hWndCtrl = pDX->PrepareEditCtrl (nIDC);
	char	szVal [100];

if (pDX->m_bSaveAndValidate) {
   ::GetWindowText (hWndCtrl, szVal, sizeof (szVal));
	fVal = atof (szVal);
	if ((min < max) && ((fVal < min) || (fVal > max))) {
		if (fVal < min)
			fVal = min;
		else if (fVal > max)
			fVal = max;
		}
	}
else {
	sprintf_s (szVal, sizeof (szVal), pszFmt && *pszFmt ? pszFmt : "%1.2f", fVal);
   AfxSetWindowText (hWndCtrl, szVal);
	}
}

//------------------------------------------------------------------------------

int CDlgHelpers::DDX_Int (CDataExchange * pDX, int nIDC, int i)
{
   HWND  hWndCtrl = pDX->PrepareEditCtrl (nIDC);
	char	szVal [100];

if (pDX->m_bSaveAndValidate) {
   ::GetWindowText (hWndCtrl, szVal, sizeof (szVal));
	return atoi (szVal);
	}
else {
	sprintf_s (szVal, sizeof (szVal), "%d", i);
   AfxSetWindowText (hWndCtrl, szVal);
	return i;
	}
}

//------------------------------------------------------------------------------

int CDlgHelpers::DDX_Flag (CDataExchange * pDX, int nIDC, int i)
{
DDX_Check (pDX, nIDC, i);
return i;
}

//------------------------------------------------------------------------------

void CDlgHelpers::InitSlider (int nIdC, int nMin, int nMax) 
{
	CSliderCtrl	*ps;

if (ps = (CSliderCtrl *) m_pParent->GetDlgItem (nIdC)) {
	ps->SetRange (nMin, nMax, TRUE);
	ps->SetPos (nMin);
	}
}

//------------------------------------------------------------------------------

void CDlgHelpers::DDX_Slider (CDataExchange * pDX, int nIdC, int& nTic) 
{
	CSliderCtrl	*ps;

ps = (CSliderCtrl *) m_pParent->GetDlgItem (nIdC);
if (pDX->m_bSaveAndValidate)
	nTic = ps->GetPos ();
else
	ps->SetPos (nTic);
if (nTic != ps->GetPos ())
	nTic = 0;
}

//------------------------------------------------------------------------------

int CDlgHelpers::GetCheck (int nIdC)
{
	CButton	*pb;

return (pb = (CButton *) m_pParent->GetDlgItem (nIdC)) ? pb->GetCheck () : FALSE;
}

//------------------------------------------------------------------------------

void CDlgHelpers::EnableControls (int nIdFirst, int nIdLast, BOOL bEnable)
{
CWnd *pWnd;
for (int i = nIdFirst; i <= nIdLast; i++)
	if (pWnd = m_pParent->GetDlgItem (i))
		pWnd->EnableWindow (bEnable);
}

//------------------------------------------------------------------------------

int CDlgHelpers::CBAddString (CComboBox *pcb, char *str)
{
	int	i = 0, m = 0, l = 0, r = pcb->GetCount () - 1;
	char	h [80], *hsz, *psz;

psz = isalpha (*str) ? str : strstr (str, ":") + 1;
while (l <= r) {
	m = (l + r) / 2;
	pcb->GetLBText (m, h);
	hsz = isalpha (*h) ? h: strstr (h, ":") + 1;
	i = strcmp (psz, hsz);
	if (i < 0)
		r = m - 1;
	else if (i > 0)
		l = m + 1;
	else
		break;
	}
if (i > 0)
	m++;
return pcb->InsertString (m, str);
}

//------------------------------------------------------------------------------

void CDlgHelpers::SelectItemData (CComboBox *pcb, int nItemData)
{
//if (nItemData >= 0) 
	{
	int i, h = pcb->GetCount ();
	for (i = 0; i < h; i++)
		if (pcb->GetItemData (i) == (DWORD) nItemData) {
			pcb->SetCurSel (i);
			return;
			}
	}
pcb->SetCurSel (-1);
}

//------------------------------------------------------------------------------

void CDlgHelpers::CBInit (CComboBox *pcb, char** pszNames, ubyte *pIndex, ubyte *pItemData, int nMax, int nType, bool bAddNone)
{
	int h, j;
	DWORD nErr;
	
pcb->ResetContent ();
if (bAddNone) {
	j = pcb->AddString ("(none)");
	pcb->SetItemData (j, -1);
	}

CStringResource res;

for (int i = 0; i < nMax; i++) {
	res.Clear ();
	switch (nType) {
		case 0:
			h = pIndex ? pIndex [i]: i;
			sprintf_s (res.Value (), res.Size (), "%s", pszNames [h]);
			break;
		case 1:
			sprintf_s (res.Value (), res.Size (), "%d: ", i);
			res.Load (int (pszNames) + i);
			h = i;
			break;
		case 2:
			sprintf_s (res.Value (), res.Size (), "%s %d", (char *) pszNames, i);
			h = pIndex ? pIndex [i]: i;
			break;
		case 3:
			res.Load (int (pszNames) + i);
			nErr = GetLastError ();
			h = i;
			break;
		default:
			return;
		}
	if (!strstr (res.Value (), "(not used)")) {
		j = pcb->AddString (res.Value ());
		pcb->SetItemData (j, pItemData ? pItemData [h]: h);
		}
	}
pcb->SetCurSel (0);
}

//------------------------------------------------------------------------------

void CDlgHelpers::CBUpdateListWidth (CComboBox *pcb)
{
	int width = 0;
	CDC *dc = pcb->GetDC ();
	CFont *fontOld = dc->SelectObject (pcb->GetFont ());
	CString itemString;

for (int i = 0; i < pcb->GetCount (); i++) {
	pcb->GetLBText (i, itemString);
	width = max (width, dc->GetTextExtent (itemString).cx);
	}

dc->SelectObject (fontOld);
pcb->ReleaseDC (dc);
width += ::GetSystemMetrics (SM_CXVSCROLL) + 2 * ::GetSystemMetrics (SM_CXEDGE);
pcb->SetDroppedWidth (width);
pcb->SetEditSel (-1, 0);
}

//------------------------------------------------------------------------------
		
BOOL CDlgHelpers::SelectColor (BYTE& red, BYTE& green, BYTE& blue)
{
	CHOOSECOLOR	cc;

memset (&cc, 0, sizeof (cc));
cc.lStructSize = sizeof (cc);
cc.hwndOwner = DLE.MainFrame ()->m_hWnd;
cc.rgbResult = RGB (red, green, blue);
cc.lpCustColors = m_custColors;
cc.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT | CC_SHOWHELP;
if (!ChooseColor (&cc)) 
	return FALSE;
blue = ((ubyte) (cc.rgbResult >> 16)) & 0xFF;
green = ((ubyte) (cc.rgbResult >> 8)) & 0xFF;
red = ((ubyte) cc.rgbResult) & 0xFF;
return TRUE;
}

//------------------------------------------------------------------------------

void CDlgHelpers::CreateColorCtrl (CColorControl *pWnd, int nIdC)
{
CWnd *pParentWnd = m_pParent->GetDlgItem (nIdC);
CRect rc;
pParentWnd->GetClientRect (rc);
pWnd->Create (null, null, WS_CHILD | WS_VISIBLE, rc, pParentWnd, 0);
}

//------------------------------------------------------------------------------

void CDlgHelpers::UpdateColorCtrl (CColorControl *pWnd, COLORREF color)
{
#if 1
pWnd->SetColor (color);
#else
if (pWnd->GetSafeHwnd ()) {
	CDC *pDC = pWnd->GetDC ();
	CRect rc;
	pWnd->GetClientRect (rc);
	pDC->FillSolidRect (&rc, color);
	pWnd->ReleaseDC (pDC);
	pWnd->Invalidate ();
	pWnd->UpdateWindow ();
	}
#endif
}

//------------------------------------------------------------------------------

BOOL CDlgHelpers::OnToolTipNotify (UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
// need to handle both ANSI and UNICODE versions of the message
TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
//CString strTipText;
char strTipText [100], *psz;
UINT nID = UINT (pNMHDR->idFrom);
if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
	 pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND)) {
	// idFrom is actually the HWND of the tool
	nID = int (::GetDlgCtrlID (HWND (UINT_PTR (nID))));
}
if (nID != 0) // will be zero on a separator
	if (!LoadString (AfxGetApp()->m_hInstance, nID, strTipText, sizeof (strTipText)))
		*strTipText = '\0';
	else if (psz = strchr (strTipText, '\n'))
		*psz = '\0';
//	strTipText.Format ("Control ID = %d", nID);
if (pNMHDR->code == TTN_NEEDTEXTA)
	lstrcpyn(pTTTA->szText, strTipText, sizeof (pTTTA->szText));
else
	_mbstowcsz(pTTTW->szText, strTipText, sizeof (pTTTW->szText));
*pResult = 0;
return TRUE;    // message was handled
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//eof DlgHelpers.cpp