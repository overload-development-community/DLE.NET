
#include "stdafx.h"
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <commdlg.h>
#include <math.h>

#include "mine.h"
#include "dle-xp.h"
#include "toolview.h"
#include "PaletteManager.h"
#include "TextureManager.h"

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CPaletteWnd, CWnd)
#if 0
	ON_WM_LBUTTONDOWN ()
	ON_WM_RBUTTONDOWN ()
	ON_WM_LBUTTONUP ()
	ON_WM_RBUTTONUP ()
#endif
END_MESSAGE_MAP ()

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CTextureEdit, CDialog)
	ON_WM_PAINT ()
	ON_WM_MOUSEMOVE ()
	ON_WM_LBUTTONDOWN ()
	ON_WM_RBUTTONDOWN ()
	ON_WM_LBUTTONUP ()
	ON_WM_RBUTTONUP ()
	ON_BN_CLICKED (IDC_TEXEDIT_DEFAULT, OnDefault)
	ON_BN_CLICKED (IDC_TEXEDIT_UNDO, OnUndo)
	ON_BN_CLICKED (IDC_TEXEDIT_LOAD, OnLoad)
	ON_BN_CLICKED (IDC_TEXEDIT_SAVE, OnSave)
END_MESSAGE_MAP ()

//------------------------------------------------------------------------------

CPaletteWnd::CPaletteWnd ()
{
m_nWidth =
m_nHeight = 0;
m_pDC = null;
m_pOldPal = null;
}

//------------------------------------------------------------------------------

CPaletteWnd::~CPaletteWnd ()
{
}

//------------------------------------------------------------------------------

#define	MINRGB(rgb)	(((rgb)->peRed < (rgb)->peGreen) ? ((rgb)->peRed < (rgb)->peBlue) ? (rgb)->peRed : (rgb)->peBlue : ((rgb)->peGreen < (rgb)->peBlue) ? (rgb)->peGreen : (rgb)->peBlue)
#define	MAXRGB(rgb)	(((rgb)->peRed > (rgb)->peGreen) ? ((rgb)->peRed > (rgb)->peBlue) ? (rgb)->peRed : (rgb)->peBlue : ((rgb)->peGreen > (rgb)->peBlue) ? (rgb)->peGreen : (rgb)->peBlue)

#define sqr(v)	(((int)(v))*((int)(v)))

int CPaletteWnd::CmpColors (PALETTEENTRY *c, PALETTEENTRY *m)
{
int i = c->peRed + c->peGreen + c->peBlue; //Luminance (c->peRed, c->peGreen, c->peBlue);
int j = m->peRed + m->peGreen + m->peBlue; //Luminance (m->peRed, m->peGreen, m->peBlue);
if (i < j)
	return -1;
if (i > j)
	return 1;
if (c->peRed < m->peRed)
	return -1;
if (c->peRed > m->peRed)
	return 1;
if (c->peGreen < m->peGreen)
	return -1;
if (c->peGreen > m->peGreen)
	return 1;
if (c->peBlue < m->peBlue)
	return -1;
if (c->peBlue > m->peBlue)
	return 1;
return 0;
}

//------------------------------------------------------------------------------

void CPaletteWnd::SortPalette (int left, int right)
{
	int				l = left, 
						r = right;
	PALETTEENTRY	m = m_palColors [(l + r) / 2];

do {
	while (CmpColors (m_palColors + l, &m) < 0)
		l++;
	while (CmpColors (m_palColors + r, &m) > 0)
		r--;
	if (l <= r) {
		if (l < r) {
			PALETTEENTRY h = m_palColors [l];
			m_palColors [l] = h = m_palColors [r];
			m_palColors [r] = h;
			ubyte i = m_nSortedPalIdx [l];
			m_nSortedPalIdx [l] = m_nSortedPalIdx [r];
			m_nSortedPalIdx [r] = i;
			}
		l++;
		r--;
		}
	}
while (l <= r);
if (left < r)
	SortPalette (left, r);
if (l < right)
	SortPalette (l, right);
}

//------------------------------------------------------------------------------

void CPaletteWnd::CreatePalette ()
{
for (int i = 0; i < 256; i++) {
	m_nSortedPalIdx [i] = i;
	RgbFromIndex (i, m_palColors [i]);
	}
}
//------------------------------------------------------------------------------

void CPaletteWnd::Update ()
{
InvalidateRect (null);
UpdateWindow ();
}

//------------------------------------------------------------------------------

int CPaletteWnd::Create (CWnd *pParentWnd, int nWidth, int nHeight)
{
	CRect	rc;

m_pParentWnd = pParentWnd;
pParentWnd->GetClientRect (rc);
m_nWidth = nWidth;
m_nHeight = nHeight;
if (m_nWidth < 0)
	m_nWidth = rc.Width () / 8;
if (m_nHeight < 0) {
	m_nHeight = rc.Height () / 8;
	if (m_nWidth * m_nHeight > 256)
		m_nHeight = (256 + m_nWidth - 1) / m_nWidth;
	}
return CWnd::Create (null, null, WS_CHILD | WS_VISIBLE, rc, pParentWnd, 0);
}

//------------------------------------------------------------------------------

bool CPaletteWnd::SelectColor (CPoint& point, int& color, PALETTEENTRY *pRGB)
{
	CRect	rcPal;

GetClientRect (rcPal);
//ClientToScreen (rcPal);
// if over palette, redefine foreground color
if (PtInRect (rcPal, point)) {
	int x, y;
//	x = ((point.x - rcPal.left) >> 3)&127;
//	y = ((point.y - rcPal.top) >> 3)&31;
	x = (int) ((double) (point.x - rcPal.left) * ((double) m_nWidth / rcPal.Width ()));
	y = (int) ((double) (point.y - rcPal.top) * ((double) m_nHeight / rcPal.Height ()));
	int c = m_nWidth * y + x;
	if (c > 255)
		return false;
	color = m_nSortedPalIdx [c];
	if (pRGB)
		*pRGB = m_palColors [c];
	//RgbFromIndex (color, pRGB);
	return true;
	}
return false;
}

//------------------------------------------------------------------------------

void CPaletteWnd::SetPalettePixel (int x, int y) 
{
	CRect	rc;

GetClientRect (&rc);
int dx, dy;
for (dy = 0; dy < 8; dy++)
	for (dx = 0; dx < 8; dx++)
		m_pDC->SetPixel ((x << 3) + dx + rc.left, (y << 3) + dy + rc.top, /*PALETTEINDEX*/ (y * m_nWidth + x));
}

//------------------------------------------------------------------------------

void CPaletteWnd::DrawPalette (void) 
{
if (!BeginPaint ())
	return;
CreatePalette ();
//SortPalette (0, 255);
CRect rc;
GetClientRect (&rc);
ubyte *bmPalette = new ubyte [m_nWidth * m_nHeight];
int h, i, c, w, x, y;
for (c = 0, y = m_nHeight - 1; (y >= 0); y--) {
	for (x = 0, h = y * m_nWidth; x < m_nWidth; x++, h++) {
		if (!y)
			y = 0;
		bmPalette [h] = (c < 256) ? m_nSortedPalIdx [c++] : 0;
		}
	}
BITMAPINFO* bmi = paletteManager.BMI ();
bmi->bmiHeader.biWidth = m_nWidth;
bmi->bmiHeader.biHeight = m_nHeight;
bmi->bmiHeader.biBitCount = 8;
bmi->bmiHeader.biClrUsed = 0;
//CPalette *pOldPalette = m_pDC->SelectPalette (paletteManager.Render (), FALSE);
//m_pDC->RealizePalette ();
if (m_nWidth & 1)
	for (i = 0; i < m_nHeight; i++) {
		w = (i == m_nHeight - 1) ? 256 % m_nWidth : m_nWidth;
		StretchDIBits (m_pDC->m_hDC, 0, i * 8, w * 8, 8, 0, 0, w, 1, 
						   (void *) (bmPalette + (m_nHeight - i - 1) * m_nWidth), bmi, 
							DIB_RGB_COLORS, SRCCOPY);
		}
else
	StretchDIBits (m_pDC->m_hDC, 0, 0, m_nWidth * 8, m_nHeight * 8, 0, 0, m_nWidth, m_nHeight, 
					   (void *) bmPalette, bmi, DIB_RGB_COLORS, SRCCOPY);
//m_pDC->SelectPalette (pOldPalette, FALSE);
free (bmPalette);
EndPaint ();
}

//------------------------------------------------------------------------------

bool CPaletteWnd::BeginPaint ()
{
if (!IsWindow (m_hWnd))
	return false;
if (m_pDC)
	return false;
if (!(m_pDC = GetDC ()))
	 return false;
m_pOldPal = m_pDC->SelectPalette (paletteManager.Render (), FALSE);
m_pDC->RealizePalette ();
return true;
}

//------------------------------------------------------------------------------

void CPaletteWnd::EndPaint ()
{
if (m_pDC) {
	if (m_pOldPal) {
		m_pDC->SelectPalette (m_pOldPal, FALSE);
		m_pOldPal = null;
		}
	ReleaseDC (m_pDC);
	m_pDC = null;
	}
Update ();
}

//------------------------------------------------------------------------------
#if 0
void CPaletteWnd::OnLButtonDown (UINT nFlags, CPoint point)
{
m_pParentWnd->SendMessage (WM_LBUTTONDOWN, (WPARAM) nFlags, (LPARAM) point.x + (((LPARAM) point.y) << 16));
}

//------------------------------------------------------------------------------

void CPaletteWnd::OnRButtonDown (UINT nFlags, CPoint point)
{
m_pParentWnd->SendMessage (WM_RBUTTONDOWN, (WPARAM) nFlags, (LPARAM) point.x + (((LPARAM) point.y) << 16));
}

//------------------------------------------------------------------------------

void CPaletteWnd::OnLButtonUp (UINT nFlags, CPoint point)
{
m_pParentWnd->SendMessage (WM_LBUTTONUP, (WPARAM) nFlags, (LPARAM) point.x + (((LPARAM) point.y) << 16));
}

//------------------------------------------------------------------------------

void CPaletteWnd::OnRButtonUp (UINT nFlags, CPoint point)
{
m_pParentWnd->SendMessage (WM_RBUTTONUP, (WPARAM) nFlags, (LPARAM) point.x + (((LPARAM) point.y) << 16));
}
#endif

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

CTextureEdit::CTextureEdit (const CTexture *pTexture, CWnd *pParent)
	: CDialog (IDD_EDITTEXTURE, pParent) 
{
*m_szColors = '\0';
m_pDC = null;
m_pPaintWnd = null;
m_pOldPal = null;
m_lBtnDown =
m_rBtnDown = false;
m_nTexAll = pTexture->Index ();
strcpy_s (m_szName, sizeof (m_szName), pTexture->Name ());
_strlwr_s (m_szName, sizeof (m_szName));
}

//------------------------------------------------------------------------------

CTextureEdit::~CTextureEdit ()
{
m_texture [0].Clear ();
m_texture [1].Clear ();
}

//------------------------------------------------------------------------------

BOOL CTextureEdit::OnInitDialog ()
{
if (!textureManager.Available ())
	return FALSE;

CDialog::OnInitDialog ();

	CWnd*	pWnd;
	CRect	rc;
	const CTexture *pTexture = textureManager.TextureByIndex (m_nTexAll);

pWnd = GetDlgItem (IDC_TEXEDIT_TEXTURE);
pWnd->GetClientRect (rc);
m_textureWnd.Create (null, null, WS_CHILD | WS_VISIBLE, rc, pWnd, 0);
pWnd = GetDlgItem (IDC_TEXEDIT_PALETTE);
pWnd->GetClientRect (rc);
m_paletteWnd.Create (pWnd, 32, 8);
pWnd = GetDlgItem (IDC_TEXEDIT_LAYERS);
pWnd->GetClientRect (rc);
m_layerWnd.Create (null, null, WS_CHILD | WS_VISIBLE, rc, pWnd, 0);
// set cursor styles for bitmap windows
SetCursor (LoadCursor (AfxGetInstanceHandle (), "PENCIL_CURSOR"));
//  PaletteButton->SetCursor(null, IDC_CROSS);
m_fgColor = 0; // black
m_bgColor = 1; // white
m_lBtnDown  = false;
m_rBtnDown = false;
m_bModified = false;
m_bPendingRevert = false;
if ((pTexture->Buffer () == null) || !pTexture->IsLoaded ()) {
	DEBUGMSG (" Texture tool: Invalid texture");
	EndDialog (IDCANCEL);
	}
else if (!m_texture [0].Copy (*pTexture)) {
	DEBUGMSG (" Texture tool: Not enough memory for texture editing");
	EndDialog (IDCANCEL);
	}
m_texture [1].Clear ();
if (!m_texture [1].Copy (m_texture [0]))
	DEBUGMSG (" Texture tool: Not enough memory for undo function");
Backup ();
Refresh ();
m_nWidth = pTexture->Width ();
m_nHeight = pTexture->Height ();
return TRUE;
}

//------------------------------------------------------------------------------

void CTextureEdit::DoDataExchange (CDataExchange *pDX)
{
DDX_Text (pDX, IDC_TEXEDIT_COLORS, m_szColors, sizeof (m_szColors));
}

//------------------------------------------------------------------------------

void CTextureEdit::Backup (void)
{
if (m_texture [1].Buffer ())
	m_texture [1].Copy (m_texture [0]);
}

//------------------------------------------------------------------------------

bool CTextureEdit::PtInRect (CRect& rc, CPoint& pt)
{
return (pt.x >= rc.left) && (pt.x < rc.right) && (pt.y >= rc.top) && (pt.y < rc.bottom);
}

//------------------------------------------------------------------------------

void CTextureEdit::OnButtonDown (UINT nFlags, CPoint point, int& color)
{
	CRect	rcEdit, rcPal;

GetClientRect (&m_textureWnd, rcEdit);
GetClientRect (&m_paletteWnd, rcPal);
if (PtInRect (rcEdit, point)) {
	Backup ();
	ColorPoint (nFlags, point, color);
	}
else if (PtInRect (rcPal, point)) {
	point.x -= rcPal.left;
	point.y -= rcPal.top;
	if (m_paletteWnd.SelectColor (point, color))
		DrawLayers ();
	}
}

//------------------------------------------------------------------------------

void CTextureEdit::OnOK ()
{
if (m_bModified)
	textureManager.OverrideTexture (m_nTexAll, &m_texture [0]);
else if (m_bPendingRevert)
	textureManager.RevertTexture (m_nTexAll);
CDialog::OnOK ();
}

//------------------------------------------------------------------------------

void CTextureEdit::OnLButtonDown (UINT nFlags, CPoint point)
{
m_lBtnDown = TRUE;
OnButtonDown (nFlags, point, m_fgColor);
}

//------------------------------------------------------------------------------

void CTextureEdit::OnRButtonDown (UINT nFlags, CPoint point)
{
m_rBtnDown = TRUE;
OnButtonDown (nFlags, point, m_bgColor);
}

//------------------------------------------------------------------------------

void CTextureEdit::OnLButtonUp (UINT nFlags, CPoint point)
{
m_lBtnDown = FALSE;
}

//------------------------------------------------------------------------------

void CTextureEdit::OnRButtonUp (UINT nFlags, CPoint point)
{
m_rBtnDown = FALSE;
}

//------------------------------------------------------------------------------

void CTextureEdit::OnMouseMove (UINT nFlags, CPoint point)
{
if (m_lBtnDown)
	ColorPoint (nFlags, point, m_fgColor);
else if (m_rBtnDown)
	ColorPoint (nFlags, point, m_bgColor);
}


//------------------------------------------------------------------------------

bool CTextureEdit::BeginPaint (CWnd *pWnd)
{
if (m_pDC)
	return false;
if (!(m_pDC = pWnd->GetDC ()))
	 return false;
m_pPaintWnd = pWnd;
m_pOldPal = m_pDC->SelectPalette (paletteManager.Render (), FALSE);
m_pDC->RealizePalette ();
return true;
}

//------------------------------------------------------------------------------

void CTextureEdit::EndPaint ()
{
if (m_pPaintWnd) {
	if (m_pDC) {
		if (m_pOldPal) {
			m_pDC->SelectPalette (m_pOldPal, FALSE);
			m_pOldPal = null;
			}
		m_pPaintWnd->ReleaseDC (m_pDC);
		m_pDC = null;
		}
	Update (m_pPaintWnd);
	m_pPaintWnd = null;
	}
}

//------------------------------------------------------------------------------

void CTextureEdit::GetClientRect (CWnd *pWnd, CRect& rc)
{
	CRect	rcc;
	int	dx, dy;

pWnd->GetClientRect (&rcc);
pWnd->GetWindowRect (rc);
dx = rc.Width () - rcc.Width ();
dy = rc.Height () - rcc.Height ();
ScreenToClient (rc);
rc.DeflateRect (dx / 2, dy / 2);
}

//------------------------------------------------------------------------------
// CTextureEdit - ColorPoint
//
// Action - Uses coordinates to determine which pixel mouse cursor is
// over.  If it is over the palette, the palette box will be updated with
// the new color.  If it is over the texture, the texture will be updated
// with the color.
// If control key is held down, color is defined by bitmap instead.
//------------------------------------------------------------------------------

void CTextureEdit::ColorPoint (UINT nFlags, CPoint& point, int& color) 
{
	CRect		rcEdit;

GetClientRect (&m_textureWnd, rcEdit);

if (m_texture [0].Format () == TGA) {
	m_lBtnDown = m_rBtnDown = false;
	ErrorMsg ("Cannot edit TGA images.");
	}
else if (PtInRect (rcEdit, point)) {
	int x, y, nPixel;
	m_bModified = TRUE;  // mark this as m_bModified
//	x = ((point.x - rcEdit.left) >> 2) & 63;
//	y = ((point.y - rcEdit.top) >> 2) & 63;
	x = (int) ((double) (point.x - rcEdit.left) * (double (m_nWidth) / rcEdit.Width ()));
	y = (int) ((double) (point.y - rcEdit.top) * (double (m_nHeight) / rcEdit.Height ()));
	nPixel = m_nWidth * (m_nHeight - 1 - y) + x;
	if (nFlags & MK_CONTROL) {
		color = paletteManager.ClosestColor (m_texture [0][nPixel]);
		DrawLayers ();
		}
	else if (BeginPaint (&m_textureWnd)) {
		m_texture [0][nPixel] = *paletteManager.Current (color);
		if (color >= 254)
			m_texture [0][nPixel].a = 0;
		SetTexturePixel (x, y);
		EndPaint ();
		}
	}
}

//------------------------------------------------------------------------------

void CTextureEdit::OnPaint () //EvDrawItem(UINT, DRAWITEMSTRUCT &) 
{
CDialog::OnPaint ();
Refresh ();
}

//------------------------------------------------------------------------------

void CTextureEdit::OnLoad () 
{
	char szFile [MAX_PATH] = {0};
	const char *szDefExt = m_texture [0].Format () == BMP ? "bmp" : "tga";
	CFileManager::tFileFilter filters [] = {
		{ "Truevision Targa", "tga" },
		{ "256 color Bitmap Files", "bmp" }
	};
	bool bFuncRes;

sprintf_s (szFile, ARRAYSIZE (szFile), "*.%s", szDefExt);
if (CFileManager::RunOpenFileDialog (szFile, ARRAYSIZE (szFile), filters, ARRAYSIZE (filters), m_hWnd)) {
	Backup ();
	bFuncRes = m_texture [0].LoadFromFile (szFile);
	if (bFuncRes) {
		Refresh ();
		m_nWidth = m_texture [0].Width ();
		m_nHeight = m_texture [0].Height ();
		m_nSize = m_texture [0].Size ();
		m_bModified = TRUE;
		}
	else
		OnUndo ();
	}
}

//------------------------------------------------------------------------------

void CTextureEdit::OnSave ()
{
	char szFile [MAX_PATH] = { 0 };
	const char *szExt = m_texture [0].Format () == BMP ? "bmp" : "tga";
	CFileManager::tFileFilter filters [] = {
		{ "256 color Bitmap Files", "bmp" },
		{ "Truevision Targa", "tga" }
	};
	CFileManager::tFileFilter *filter = m_texture [0].Format () == BMP ? &filters [0] : &filters [1];

sprintf_s (szFile, ARRAYSIZE (szFile), "%s.%s", m_szName, szExt);
if (CFileManager::RunSaveFileDialog (szFile, ARRAYSIZE (szFile), filter, 1, m_hWnd)) {
	_strlwr_s (szFile, sizeof (szFile));
	m_texture [0].Save (szFile);
	}
}

//------------------------------------------------------------------------------

void CTextureEdit::OnUndo ()
{
if (m_texture [1].Buffer ()) {
	m_texture [0].Copy (m_texture [1]);
	// This combination should only happen immediately after OnDefault.
	// In this case we reverse it so the user is still prompted on save.
	if (m_bPendingRevert && !m_bModified) {
		m_bModified = true;
		m_bPendingRevert = false;
		}
	Refresh ();
	}
}

void CTextureEdit::Update (CWnd *pWnd) 
{
pWnd->InvalidateRect (null);
pWnd->UpdateWindow ();
}

//------------------------------------------------------------------------------

void CTextureEdit::OnDefault (void) 
{
if (QueryMsg("Are you sure you want to restore this texture\n"
				 "back to its original texture\n") == IDYES) {
	Backup ();
	m_texture [0].Copy (*textureManager.BaseTextures (m_nTexAll));
	m_bModified = false;
	m_bPendingRevert = true;
	Refresh ();
	}
}

//------------------------------------------------------------------------------

void CTextureEdit::DrawTexture (void) 
{
if (!BeginPaint (&m_textureWnd))
	return;
m_pDC->SetStretchBltMode (STRETCH_DELETESCANS);
BITMAPINFO* bmi = paletteManager.BMI ();
bmi->bmiHeader.biWidth =
bmi->bmiHeader.biHeight = m_texture [0].RenderWidth ();
bmi->bmiHeader.biBitCount = 32;
//bmi->bmiHeader.biSizeImage = m_texture [0].BufSize ();
bmi->bmiHeader.biClrUsed = 0;
CRect	rc;
m_textureWnd.GetClientRect (&rc);
StretchDIBits (m_pDC->m_hDC, 0, 0, rc.right, rc.bottom, 0, 0, m_texture [0].RenderWidth (), m_texture [0].RenderWidth (),
					(void *) m_texture [0].RenderBuffer (), bmi, DIB_RGB_COLORS, SRCCOPY);
EndPaint ();
}

//------------------------------------------------------------------------------

void CTextureEdit::DrawPalette (void) 
{
m_paletteWnd.DrawPalette ();
}

//------------------------------------------------------------------------------

void CTextureEdit::DrawLayers () 
{
if (!BeginPaint (&m_layerWnd))
	return;

CRect		rc;
m_layerWnd.GetClientRect (&rc);
rc.DeflateRect (10, 10);
rc.right -= 10;
rc.bottom -= 10;
m_pDC->FillSolidRect (&rc, PALETTEINDEX (m_bgColor));
rc.OffsetRect (10, 10);
m_pDC->FillSolidRect (&rc, PALETTEINDEX (m_fgColor));
EndPaint ();

// set message
char fg_color[30];
char bg_color[30];
switch(m_fgColor) {
	case 255: 
		strcpy_s (fg_color, sizeof (fg_color), "transparent"); 
		break;
	case 254: 
		strcpy_s (fg_color, sizeof (fg_color), "see thru"); 
		break;
	default : 
		sprintf_s (fg_color, sizeof (fg_color), "color %d", m_fgColor); 
		break;
	}
switch(m_bgColor) {
	case 255: 
		strcpy_s (bg_color, sizeof (bg_color), "transparent"); 
		break;
	case 254: 
		strcpy_s (bg_color, sizeof (bg_color), "see thru"); 
		break;
	default : 
		sprintf_s (bg_color, sizeof (bg_color), "color %d", m_bgColor); 
		break;
	}
sprintf_s (m_szColors, sizeof (m_szColors), "foreground = %s, background = %s.", fg_color, bg_color);
UpdateData (FALSE);
}

//------------------------------------------------------------------------------

void CTextureEdit::Refresh (void) 
{
DrawTexture ();
DrawPalette ();
DrawLayers ();
}

//------------------------------------------------------------------------------

void CTextureEdit::SetTexturePixel (int x, int y) 
{
	CRect		rc;
	int		cx, cy;
	double	xs, ys;
#ifdef _DEBUG
	CBGR&		rgb = m_texture [0][(63 - y) * 64 + x];
	int		color = rgb.ColorRef ();
#else
	int		color = m_texture [0][(63 - y) * 64 + x].ColorRef ();
#endif

m_textureWnd.GetClientRect (&rc);
cx = rc.Width ();
cy = rc.Height ();
xs = (double) cx / 64.0;
ys = (double) cy / 64.0;
x = rc.left + (int) ((double) x * xs);
y = rc.top + (int) ((double) y * ys);
int dx, dy;
xs /= 4.0;
ys /= 4.0;
for (dy = 0; dy < 4; dy++)
	for (dx = 0; dx < 4; dx++)
		m_pDC->SetPixel (x + (int) ((double) dx * xs), y + (int) ((double) dy * ys), color);
}

//------------------------------------------------------------------------------

void CTextureEdit::SetPalettePixel (int x, int y) 
{
	CRect	rc;

m_paletteWnd.GetClientRect (&rc);
int dx, dy;
for (dy = 0; dy < 8; dy++)
	for (dx = 0; dx < 8; dx++)
		m_pDC->SetPixel ((x << 3) + dx + rc.left, (y << 3) + dy + rc.top, PALETTEINDEX (y * 32 + x));
}

//------------------------------------------------------------------------------
