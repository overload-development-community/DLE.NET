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

static int DRound (double v)
{
if (v < 0)
	return int (v - 0.5);
else
	return int (v + 0.5);
}

void CExtSliderCtrl::Init (CDialog* parent, int nSliderId, int nSpinnerId, int nTextId, int nMin, int nMax, double nScale, double nTextScale, int nTicFreq, char* format)
{
m_parent = parent;
m_nSlider = (m_bReverseSlider = (nSliderId < 0)) ? -nSliderId : nSliderId;
m_nSpinner = (m_bReverseSpinner = (nSpinnerId < 0)) ? -nSpinnerId : nSpinnerId;
m_bEditableText = (nTextId > 0);
m_nText = nTextId;
m_format = format;
m_nScale = nScale;
//m_nStep = (nScale > 1.0) ? DRound (double (nMax - nMin) / m_nScale) : (nScale < 1.0) ? nScale : 1.0;
m_nTextScale = nTextScale;
m_nMin = DRound (nMin / nScale);
m_nMax = DRound (nMax / nScale);
if (!(m_nPageSize = (m_nMax - m_nMin) / 10))
	m_nPageSize = 1;
if (m_bReverseSlider)
	m_nPageSize = -m_nPageSize;
if (m_nSlider) {
	Slider ()->SetRange (m_nMin, m_nMax, TRUE);
	Slider ()->SetTicFreq ((nTicFreq > 1) ? nTicFreq : MaxVal ((nMax - nMin + 1) / 10, 1));
	Slider ()->SetPos (m_nMin);
	}
if (m_nSpinner) {
	Spinner ()->SetRange (m_nMin, m_nMax);
	Spinner ()->SetPos (m_nMin);
	}
}

//------------------------------------------------------------------------------

void CExtSliderCtrl::Update (void)
{
m_bUpdating = true;
if (m_nSlider)
	Slider ()->SetPos (m_bReverseSlider ? m_nMax - m_nValue : m_nValue);
if (m_nSpinner)
	Spinner ()->SetPos (m_bReverseSpinner ? m_nMax - m_nValue : m_nValue);
if (m_nText) {
	char szValue [100];
	if (m_formatter)
		m_formatter (szValue, m_nValue);
	else if (m_format)
		sprintf_s (szValue, sizeof (szValue), m_format, int (m_nValue * m_nScale * m_nTextScale + 0.5));
	else
		sprintf_s (szValue, sizeof (szValue), "%d", int (m_nValue * m_nScale * m_nTextScale + 0.5));
	if (m_bEditableText)
		Text ()->SetWindowText (szValue);
	else
		m_parent->SetDlgItemText (-m_nText, szValue);
	}
m_bUpdating = false;
}

//------------------------------------------------------------------------------

bool CExtSliderCtrl::SetValue (int nValue, bool bScale)
{
if (bScale)
	nValue = int (nValue / m_nScale + 0.5);
if (nValue < m_nMin)
	nValue = m_nMin;
else if (nValue > m_nMax)
	nValue = m_nMax;
//if (m_nValue == nValue)
//	return false;
m_nValue = nValue;
Update ();
return true;
}

//------------------------------------------------------------------------------

bool CExtSliderCtrl::OnEdit (void)
{
if (m_bUpdating)
	return false;
if (!m_nText) 
	return false;
char szValue [20];
Text ()->GetWindowText (szValue, sizeof (szValue));
return *szValue ? SetValue (int (atoi (szValue) / m_nTextScale + 0.5)) : false;
}

//------------------------------------------------------------------------------

bool CExtSliderCtrl::OnScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
	int nValue;

if (pScrollBar == (CScrollBar *) Slider ()) {
	//nValue = pScrollBar->GetScrollPos ();
	nValue = m_nValue;
	switch (scrollCode) {
		case SB_LINEUP:
			nValue--; // -= m_nStep;
			break;
		case SB_LINEDOWN:
			nValue++; // += m_nStep;
			break;
		case SB_PAGEUP:
			nValue -= m_nPageSize;
			break;
		case SB_PAGEDOWN:
			nValue += m_nPageSize;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			nValue = m_bReverseSlider ? m_nMax - thumbPos : thumbPos;
			break;
		case SB_ENDSCROLL:
			Update ();
			return true;
		}
	SetValue (nValue, false);
	nValue = pScrollBar->GetScrollPos ();
	return true;
	}
else if (pScrollBar == (CScrollBar *) Spinner ()) {
	if (scrollCode != SB_THUMBPOSITION) {
		Update ();
		return true;
		}
	SetValue (thumbPos, false);
	}
return false;
}

//------------------------------------------------------------------------------

void CExtSliderCtrl::DoDataExchange (CDataExchange* pDX, int& value)
{
if (pDX->m_bSaveAndValidate) {
	if (m_nText > 0)
		SetValue (DDX_Int (pDX, m_nText, value));
	value = (int) GetValue ();
	}
else
	SetValue (value);
}

//------------------------------------------------------------------------------

void CExtSliderCtrl::DoDataExchange (CDataExchange* pDX, short& value)
{
if (pDX->m_bSaveAndValidate) {
	if (m_nText > 0)
		SetValue (DDX_Int (pDX, m_nText, value));
	value = (short) GetValue ();
	}
else
	SetValue (value);
}

//------------------------------------------------------------------------------

void CExtSliderCtrl::DoDataExchange (CDataExchange* pDX, ubyte& value)
{
if (pDX->m_bSaveAndValidate) {
	if (m_nText > 0)
		SetValue (DDX_Int (pDX, m_nText, value));
	value = (ubyte) GetValue ();
	}
else
	SetValue (value);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//eof tooldlg.cpp