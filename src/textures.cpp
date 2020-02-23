#include <assert.h>

#include "stdafx.h"
#include "define.h"
#include "textures.h"
#include "FileManager.h"
#include "dle-xp-res.h"
#include "global.h"
#include "PaletteManager.h"
#include "TextureManager.h"
#include "dle-xp.h"
#include "glew.h"

short nDbgTexture = -1;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

inline CBGRA& Blend (CBGRA& dest, const CBGRA& src)
{
if (paletteManager.SuperTransp (src))
	dest.r =
	dest.g =
	dest.b =
	dest.a = 0;
else if (src.a > 0) {
	if (src.a == 255)
		dest = src;
	else {
		ubyte b = 255 - src.a;
		dest.r = (ubyte) (((int) dest.r * b + (int) src.r * src.a) / 255);
		dest.g = (ubyte) (((int) dest.g * b + (int) src.g * src.a) / 255);
		dest.b = (ubyte) (((int) dest.b * b + (int) src.b * src.a) / 255);
		}
	}
return dest;
}

//------------------------------------------------------------------------------
// BlendTextures ()
//
// Builds a texture for rendering by blending base and optional overlay texture
// and writes the result into a temporary buffer
//------------------------------------------------------------------------------

int CTexture::BlendTextures (short nBaseTex, short nOvlTex, int x0, int y0) 
{
if (!textureManager.Available ())
	return 1;

	short nTextures [2], mode;
	const CTexture* pTexture [2];

mode = (nOvlTex >> ALIGNMENT_SHIFT) & 3;
nTextures [0] = nBaseTex;
nTextures [1] = nOvlTex & TEXTURE_MASK;
for (int i = 0; i < 2; i++) {
	if ((nTextures [i] < 0) || (nTextures [i] >= MAX_TEXTURES))
		nTextures [i] = 0;
	pTexture [i] = textureManager.Textures (nTextures [i]);
	}
if (nTextures [1] == 0)
	pTexture [1] = null;

return BlendTextures (pTexture [0], pTexture [1], mode, x0, y0);
}

//------------------------------------------------------------------------

int CTexture::BlendTextures (const CTexture* pBaseTex, const CTexture* pOvlTex, short nOvlAlignment, int x0, int y0, bool bCurrentFrameOnly)
{
if (!textureManager.Available ())
	return 1;

	typedef struct tFrac {
		int	c, d;
	} tFrac;

	short			w, h;
	int			offs;
	tFrac			scale;
	float			fScale;
	//int			rc; // return code

	CBGRA*		bmBufP = Buffer ();
	int			fileType = DLE.FileType ();

// Define bmBufP based on texture numbers and rotation
m_info.width = pBaseTex->Width ();
m_info.height = bCurrentFrameOnly ? pBaseTex->FrameHeight () : pBaseTex->Height ();
m_bValid = true;

offs = bCurrentFrameOnly ? pBaseTex->FrameOffset () : 0;
const CBGRA* srcDataP = pBaseTex->Buffer (offs);

#ifdef _DEBUG
if (pBaseTex && (pBaseTex->Id () == nDbgTexture))
	nDbgTexture = nDbgTexture;
if (pOvlTex && (pOvlTex->Id () == nDbgTexture))
	nDbgTexture = nDbgTexture;
#endif

if (srcDataP != null) {
	// if not rotated, then copy directly
	if (x0 == 0 && y0 == 0) {
		memcpy (bmBufP, srcDataP, Size () * sizeof (CBGRA));
		}
	else {
		// otherwise, copy bit by bit
		w = pBaseTex->Width ();
#if 0
		int l1 = y0 * w + x0;
		int l2 = pBaseTex->Size () - l1;
		memcpy (bmBufP, srcDataP + l1, l2 * sizeof (CBGRA));
		memcpy (bmBufP + l2, srcDataP, l1 * sizeof (CBGRA));
#else
		h = m_info.height;
#ifdef NDEBUG
//#pragma omp parallel for 
#endif
		int dx = (w - x0) % w;
		for (int y = 0; y < h; y++) {
			int dy = ((y - y0 + h) % h) * w;
#if 1
			memcpy (bmBufP + y * w, srcDataP + dy + dx, (w - dx) * sizeof (CBGRA));
			if (dx)
				memcpy (bmBufP + y * w + (w - dx), srcDataP + dy, dx * sizeof (CBGRA));
#else
			CBGRA* destDataP = bmBufP + y * w;
			for (int x = 0; x < w; x++) {
				int i = dy + ((x - x0 + w) % w);
				*destDataP++ = srcDataP [i];
				}
#endif
			}
#endif
		}
	}

// Overlay texture 2 if present

if (pOvlTex == null)
	return 0;
srcDataP = pOvlTex->Buffer (pOvlTex->FrameOffset ());
if (srcDataP == null)
	return 0;

if (pBaseTex->Width () == pOvlTex->Width ()) {
	scale.c = scale.d = 1;
	fScale = 1.0;
	}
else if (pBaseTex->Width () < pOvlTex->Width ()) {
	scale.c = pOvlTex->Width () / pBaseTex->Width ();
	scale.d = 1;
	fScale = float (pOvlTex->Width ()) / float (pBaseTex->Width ());
	}
else {
	scale.d = pBaseTex->Width () / pOvlTex->Width ();
	scale.c = 1;
	fScale = float (pOvlTex->Width ()) / float (pBaseTex->Width ());
	}
offs = 0;
w = pOvlTex->Width () / scale.c * scale.d;
h = pOvlTex->FrameHeight () / scale.c * scale.d;

if (!(x0 || y0)) {
	if (nOvlAlignment == 0) {
//#pragma omp parallel for
		for (int y = 0; y < h; y++) {
			CBGRA* destDataP = bmBufP + y * w;
			float offset = float (int (y * fScale) * int (w * fScale));
			for (int x = 0; x < w; x++, destDataP++, offset += fScale) 
				Blend (*destDataP, srcDataP [(y * scale.c / scale.d) * (w * scale.c / scale.d) + x * scale.c / scale.d]);
				//Blend (*destDataP, srcDataP [int (offset)]);
			}
		}
	else if (nOvlAlignment == 1) {
#if 0
		CBGRA* destDataP = bmBufP + h - 1;
		for (int y = 0; y < h; y++, destDataP--) {
			CBGRA* destData2 = destDataP;
			for (int x = 0; x < w; x++, destData2 += w) {
				Blend (*destData2, srcDataP [(y * scale.c / scale.d) * (w * scale.c / scale.d) + x * scale.c / scale.d]);
				}
			}
#else
		bmBufP += h - 1;
#pragma omp parallel for
		for (int y = 0; y < h; y++) {
			CBGRA* destDataP = bmBufP - y;
			float offset = float (int (y * fScale) * int (w * fScale));
			for (int x = 0; x < w; x++, destDataP += w, offset += fScale)
				//Blend (*destDataP, srcDataP [(y * scale.c / scale.d) * (w * scale.c / scale.d) + x * scale.c / scale.d]);
				Blend (*destDataP, srcDataP [int (offset)]);
			}
#endif
		}
	else if (nOvlAlignment == 2) {
#if 0
		CBGRA* destDataP = bmBufP + h * w;
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				Blend (*(--destDataP), srcDataP [(y * scale.c / scale.d) * (w * scale.c / scale.d) + x * scale.c / scale.d]);
				}
			}
#else
		bmBufP += h * w;
#pragma omp parallel for
		for (int y = 0; y < h; y++) {
			CBGRA* destDataP = bmBufP - y * w;
			float offset = float (int (y * fScale) * int (w * fScale));
			for (int x = 0; x < w; x++, offset += fScale)
				//Blend (*(--destDataP), srcDataP [(y * scale.c / scale.d) * (w * scale.c / scale.d) + x * scale.c / scale.d]);
				Blend (*(--destDataP), srcDataP [int (offset)]);
			}
#endif
		}
	else if (nOvlAlignment == 3) {
#if 0
		CBGRA* destDataP = bmBufP + (h - 1) * w;
		for (int y = 0; y < h; y++, destDataP++) {
			CBGRA* destData2 = destDataP;
			for (int x = 0; x < w; x++, destData2 -= w) {
				Blend (*destData2, srcDataP [(y * scale.c / scale.d) * (w * scale.c / scale.d) + x * scale.c / scale.d]);
				}
			}
		}
#else
		bmBufP += (h - 1) * w;
#pragma omp parallel for
		for (int y = 0; y < h; y++) {
			CBGRA* destDataP = bmBufP + y;
			float offset = float (int (y * fScale) * int (w * fScale));
			for (int x = 0; x < w; x++, destDataP -= w, offset += fScale)
				//Blend (*destDataP, srcDataP [(y * scale.c / scale.d) * (w * scale.c / scale.d) + x * scale.c / scale.d]);
				Blend (*destDataP, srcDataP [int (offset)]);
			}
		}
#endif
	} 
else {
	if (nOvlAlignment == 0) {
#pragma omp parallel for
		for (int y = 0; y < h; y++) {
			int y1 = ((y + y0) % h) * w;
			float offset = float (int (y * fScale) * int (w * fScale));
			for (int x = 0; x < w; x++, offset += fScale)
				//Blend (bmBufP [y1 + (x + x0) % w], srcDataP [(y * scale.c / scale.d) * (w * scale.c / scale.d) + x * scale.c / scale.d]);
				Blend (bmBufP [y1 + (x + x0) % w], srcDataP [int (offset)]);
			}
		}
	else if (nOvlAlignment == 1) {
#pragma omp parallel for
		for (int y = h - 1; y >= 0; y--) {
			int y1 = (y + x0) % w;
			float offset = float (int (y * fScale) * int (w * fScale));
			for (int x = 0; x < w; x++, offset += fScale)
				//Blend (bmBufP [y1 + ((x + y0) % h) * w], srcDataP [(y * scale.c / scale.d) * (w * scale.c / scale.d) + x * scale.c / scale.d]);
				Blend (bmBufP [((x + y0) % h) * w + (y + x0) % w], srcDataP [int (offset)]);
			}
		}
	else if (nOvlAlignment == 2) {
#pragma omp parallel for
		for (int y = h - 1; y >= 0; y--) {
			int y1 = ((y + y0) % h) * w;
			float offset = float (int (y * fScale) * int (w * fScale));
			for (int x = w - 1; x >= 0; x--, offset += fScale)
				//Blend (bmBufP [y1 + (x + x0) % w], srcDataP [(y * scale.c / scale.d) * (w * scale.c / scale.d) + x * scale.c / scale.d]);
				Blend (bmBufP [y1 + (x + x0) % w], srcDataP [int (offset)]);
			}
		}
	else if (nOvlAlignment == 3) {
#pragma omp parallel for
		for (int y = 0; y < h; y++) {
			int y1 = (y + x0) % w;
			float offset = float (int (y * fScale) * int (w * fScale));
			for (int x = w - 1; x >= 0; x--) {
				Blend (bmBufP [y1 + ((x + y0) % h) * w], srcDataP [int (offset)]);
				}
			}
		}
	}

return 0;
}

//------------------------------------------------------------------------------

void RgbFromIndex (int nIndex, PALETTEENTRY& rgb)
{
CBGR* color = paletteManager.Current (nIndex);
if (color != null) {
	rgb.peRed = color->r;
	rgb.peGreen = color->g;
	rgb.peBlue = color->b;
	rgb.peFlags = 0;
	}
}

// -------------------------------------------------------------------------- 

bool PaintTexture (CWnd *pWindow, int bkColor, int texture1, int texture2, int xOffset, int yOffset)
{
#ifdef _DEBUG
if (texture1 == nDbgTexture)
	nDbgTexture = nDbgTexture;
#endif

	CTexture *pBaseTex = (texture1 >= 0) ? textureManager.Textures (texture1) : null;
	if (pBaseTex && pBaseTex->Format ())
		pBaseTex->SetCurrentFrame (0);
	int maskedTexture2 = (texture2 < 0) ? texture2 : texture2 & TEXTURE_MASK;
	CTexture *pOvlTex = (maskedTexture2 != 0) ? textureManager.Textures (maskedTexture2) : null;
	if (pOvlTex && pOvlTex->Format ())
		pOvlTex->SetCurrentFrame (0);
	short nOvlAlignment = (maskedTexture2 > 0) ? (texture2 & ALIGNMENT_MASK) >> ALIGNMENT_SHIFT : 0;
	
return PaintTexture (pWindow, bkColor, pBaseTex, pOvlTex, nOvlAlignment, xOffset, yOffset);
}

// --------------------------------------------------------------------------

bool PaintTexture (CWnd *pWindow, int bkColor, const CTexture *pBaseTex, const CTexture *pOvlTex, short nOvlAlignment, int xOffset, int yOffset, bool bCurrentFrameOnly)
{
if (!pWindow->m_hWnd)
	return false;

	static int nOffset [2] = {0, 0};

	CDC			*pDC = pWindow->GetDC ();

if (!pDC)
	return false;

	CBitmap			bmTexture;
	CFileManager	fp;
	char				szFile [256];
	BITMAP			bm;
	CDC				memDC;
	bool				bShowTexture = true, bDescent1 = DLE.IsD1File ();
	char*				path = bDescent1 ? descentFolder [0] : descentFolder [1];

CRect	rc;
pWindow->GetClientRect (rc);

if (pBaseTex == null)
	bShowTexture = false;

if (bShowTexture) {
	// check pig file
	if (nOffset [bDescent1] == 0) {
		strcpy_s (szFile, sizeof (szFile), (bDescent1) ? descentFolder [0] : descentFolder [1]);
		if (!fp.Open (szFile, "rb"))
			nOffset [bDescent1] = -1;  // pig file not found
		else {
			fp.Seek (0, SEEK_SET);
			nOffset [bDescent1] = fp.ReadInt32 ();  // determine type of pig file
			fp.Close ();
			}
		}
	if (nOffset [bDescent1] > 0x10000L) {  // pig file type is v1.4a or descent 2 type
		CTexture tex (textureManager.SharedBuffer ());
		if (tex.BlendTextures (pBaseTex, pOvlTex, nOvlAlignment, xOffset, yOffset, bCurrentFrameOnly))
			DEBUGMSG (" Texture renderer: Texture not found (BlendTextures failed)");
		//CPalette *pOldPalette = pDC->SelectPalette (paletteManager.Render (), FALSE);
		//pDC->RealizePalette ();

		// paint the background in case some of the window is unused
#if 0
		if (bkColor >= 0)
			pDC->FillSolidRect (&rc, (COLORREF) bkColor);
#endif
		if (pDC->GetDeviceCaps (RASTERCAPS) & RC_STRETCHDIB) {
			double scale = min ((double)rc.Width () / (double)tex.Width (), (double)rc.Height () / (double)tex.Height ());
			// try to round the scale to reduce distortion
			if (scale > 1) {
				if (scale - (int)scale < 0.05)
					scale = (int)scale;
				else if (scale - (int)scale > 0.95)
					scale = (int)scale + 1;
				}
			else {
				double denominator = 1.0 / scale;
				if (denominator - (int)denominator < 0.05)
					denominator = (int)denominator;
				else if (denominator - (int)denominator > 0.95)
					denominator = (int)denominator + 1;
				// StretchDIBits is not very good at handling large reductions
				// so we'll do the integral part ourselves
				if (denominator >= 2.0) {
					tex.Shrink ((int) denominator, (int) denominator);
					denominator /= (int) denominator;
					}
				scale = 1.0 / denominator;
				}
			tex.ApplyAlpha ();
			int destWidth = (int)(tex.Width () * scale);
			int destHeight = (int)(tex.Height () * scale);
			int destX = (rc.Width () - destWidth) / 2;
			int destY = (rc.Height () - destHeight) / 2;

			BITMAPINFO* bmi = paletteManager.BMI ();
			bmi->bmiHeader.biWidth = tex.Width ();
			bmi->bmiHeader.biHeight = tex.Height ();
			bmi->bmiHeader.biBitCount = 32;
			bmi->bmiHeader.biSizeImage = ((((bmi->bmiHeader.biWidth * bmi->bmiHeader.biBitCount) + 31) & ~31) >> 3) * bmi->bmiHeader.biHeight;
			StretchDIBits (pDC->m_hDC, destX, destY, destWidth, destHeight, 0, 0, tex.Width (), tex.Height (),
			               (void *) tex.Buffer (), bmi, DIB_RGB_COLORS, SRCCOPY);
			}
		else {
			double scale = tex.Scale ();
			int w = tex.Width ();
			if (scale == 1.0) {
				for (int i = 0, y = w - 1; y >= 0; y--)
					for (int x = 0; x < w; x++)
						pDC->SetPixel ((int) x, (int) y, textureManager.SharedBuffer () [i++].ColorRef ());
				}
			else {
				for (int y = w - 1; y >= 0; y = (int) (y - scale))
					for (int x = 0; x < w; x = (int) (x + scale))
						pDC->SetPixel ((int) (x / scale), (int) (y / scale), textureManager.SharedBuffer () [y * w + x].ColorRef ());
				}
			}
		//pDC->SelectPalette (pOldPalette, FALSE);
		}
	else {
		HGDIOBJ hgdiobj1;
		bmTexture.LoadBitmap ((nOffset [bDescent1] < 0) ? "NO_PIG_BITMAP" : "WRONG_PIG_BITMAP");
		bmTexture.GetObject (sizeof (BITMAP), &bm);
		memDC.CreateCompatibleDC (pDC);
		hgdiobj1 = memDC.SelectObject (bmTexture);
		pDC->StretchBlt (0, 0, rc.Width (), rc.Height (), &memDC, 0, 0, 64, 64, SRCCOPY);
		memDC.SelectObject (hgdiobj1);
		DeleteObject (bmTexture);
//		DeleteDC (memDC.m_hDC);
		}
	}
else if (bkColor < 0) {
	HGDIOBJ hgdiobj;
	// set bitmap
	bmTexture.LoadBitmap ("NO_TEXTURE_BITMAP");
	bmTexture.GetObject (sizeof (BITMAP), &bm);
	memDC.CreateCompatibleDC (pDC);
	hgdiobj = memDC.SelectObject (bmTexture);
	pDC->StretchBlt (0, 0, rc.Width (), rc.Height (), &memDC, 0, 0, 64, 64, SRCCOPY);
	memDC.SelectObject (hgdiobj);
	DeleteObject (bmTexture);
//	DeleteDC (memDC.m_hDC);
	}
else if (bkColor >= 0)
	pDC->FillSolidRect (&rc, (COLORREF) bkColor);
pWindow->ReleaseDC (pDC);
pWindow->InvalidateRect (null, TRUE);
pWindow->UpdateWindow ();
return bShowTexture;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CTexture::ComputeIndex (ubyte* palIndex) const
{
	CBGR* palette = paletteManager.Current ();

#ifndef _DEBUG
#	pragma omp parallel for
#endif
for (int y = 0; y < (int) m_info.height; y++) {
	int i = y * m_info.width;
	int k = Size () - m_info.width - i;
	for (int x = 0; x < (int) m_info.width; x++) {
		palIndex [k + x] = paletteManager.ClosestColor (m_data [i + x]);
		}
	}
}

//------------------------------------------------------------------------------

bool CTexture::Allocate (int nSize)
{
if ((m_data != null) && (m_info.bufSize != nSize))
	Release ();
if (m_data == null) {
#ifdef _DEBUG
	if (nSize > 4096 * 4096 * 4)
		m_data = null;
	else
#endif
	m_data = new CBGRA [nSize];
	m_info.bufSize = m_data ? nSize : 0;
	}
return (m_data != null);
}

//------------------------------------------------------------------------

void CTexture::Release (void) 
{
GLRelease ();
if (m_renderBuffer != null) {
	if (m_renderBuffer != m_data)
		delete m_renderBuffer;
	m_renderBuffer = null;
	}
if (!m_bExtBuffer) {
	if (m_data != null) {
		delete m_data;
		m_data = null;
		}
	}
else {
	// External buffer; stop using it, but don't delete it because we don't own it
	m_data = null;
	m_bExtBuffer = false;
	}
m_bValid = false;
}

//------------------------------------------------------------------------

void CTexture::GenerateRenderBuffer ()
{
	int width = RenderWidth ();
	int height = RenderHeight ();
	uint xOffset = MaxVal (0, int (width - Width ())) / 2;
	uint yOffset = MaxVal (0, int (height - Height ())) / 2;

if (m_renderBuffer != null && m_renderBuffer != m_data)
	delete m_renderBuffer;
m_renderBuffer = null;
// Don't need a separate buffer if the render buffer would be the same size. Also not used for TGA
if ((RenderSize () == Size ()) || (m_info.format == TGA)) {
	m_renderBuffer = m_data;
	GLCreate ();
	return;
	}

// bitmaps > 64x64 will not be used by DLE and will therefore not get centered
if ((m_info.width > 64) || (m_info.height > 64))
	return;
// bitmaps < 64x64 may be needed for rendering items in the level;
// these will be centered in a 64x64 pixel buffer
// BufSize() computes the buffer size so that it is big enough for a square image
// the side length of which is a multiple of 2 that is big enough to contain the original image
int nSize = RenderSize ();
if (m_data == null || nSize == 0)
	return; // no buffer to copy from
m_renderBuffer = new CBGRA [nSize];
if (!m_renderBuffer)
	return; // allocation failed
size_t elementSize = sizeof (m_renderBuffer [0]);
memset (m_renderBuffer, 0, nSize * elementSize);

// Copy bitmap data into the rendering buffer (row by row)
for (uint y = 0; y < m_info.height; y++) {
	uint destOffset = (y + yOffset) * width + xOffset;
	uint sourceOffset = y * m_info.width;
	memcpy_s (m_renderBuffer + destOffset, (nSize - destOffset) * elementSize,
		m_data + sourceOffset, m_info.width * elementSize);
	}

// Create OpenGL texture so we can use it for rendering later
GLCreate ();
}

//------------------------------------------------------------------------

void CTexture::LoadFromResource (int nId) 
{
	CResource res;

Clear ();
ubyte* pData = res.Load (nId);
if (!pData)
	return;
tTgaHeader* h = (tTgaHeader*) pData;
int nSize = int (h->width) * int (h->height);
if (res.Size () < sizeof (tTgaHeader) + nSize * sizeof (CBGRA))
	return;
if (!Allocate (nSize))
	return;
memcpy (m_data, pData + sizeof (tTgaHeader), nSize * sizeof (m_data [0]));
m_info.width = h->width;
m_info.height = h->height;
m_info.format = TGA;
#ifdef _DEBUG
for (pData += sizeof (tTgaHeader), nSize *= 4; nSize; nSize--, pData++)
	if (*pData)
		break;
#endif
GenerateRenderBuffer ();
m_bValid = true;
}

//------------------------------------------------------------------------

#define RLE_CODE			0xE0
#define NOT_RLE_CODE		31
#define IS_RLE_CODE(x)	(((x) & RLE_CODE) == RLE_CODE)

void CTexture::Load (CFileManager& fp, CPigTexture& info) 
{
	ushort	rowSize [4096];
	ubyte		rowBuf [4096], *rowPtr;
	ubyte		palIndex, runLength;
	CBGR*		palette = paletteManager.Current ();

memcpy (m_info.szName, info.name, sizeof (info.name));
m_info.szName [sizeof (m_info.szName) - 1] = '\0';
m_info.width = info.width;
m_info.height = info.height;
m_info.bTransparent = false;
m_info.bSuperTransparent = false;
m_info.bAnimated = (info.dflags & BM_DFLAG_ANIMATED) != 0;
m_info.nFrame = info.dflags & BM_DFLAG_ANIMFRAME_MASK;
//if (m_info.bAnimated)
//	strcat (m_info.szName, "#0");
memset (m_data, 0, m_info.bufSize * sizeof (m_data [0]));

if (m_info.format == TGA) {
#ifdef _DEBUG
	if (strstr (m_info.szName, "door34"))
		m_info.szName [sizeof (m_info.szName) - 1] = '\0';
#endif
	tRGBA color;
#if 1
	uint h = info.width * (info.height - 1);
	for (uint i = info.height; i; i--) {
		for (uint j = info.width; j; j--, h++) {
			fp.Read (&color, sizeof (color), 1);
			m_data [h] = color; // different data types -> type conversion!
			if (color.a < 255)
				m_info.bTransparent = true;
			if (paletteManager.SuperTransp (CBGRA () = color))
				m_info.bSuperTransparent = true;
			}
		h -= 2 * info.width;
		}
#else
	for (uint i = 0, h = Size (); i < h; i++) {
		fp.Read (&color, sizeof (color), 1);
		m_data [i] = color;
		if (color.a < 255)
			m_info.bTransparent = true;
		}
#endif
	//pTexture->m_info.bValid = TGA2Bitmap (pTexture->m_data, pTexture->m_data, (int) pigTexInfo.width, (int) pigTexInfo.height);
	}
else if (info.flags & BM_FLAG_RLE) { // RLE packed
	int nSize = fp.ReadInt32 ();
	if (info.flags & BM_FLAG_RLE_BIG) // long scan lines (>= 256 bytes)
		fp.ReadBytes (rowSize, info.height * sizeof (ushort));
	else {
		for (int i = 0; i < info.height; i++)
		rowSize [i] = (ushort) fp.ReadUByte ();
		}
	int nRow = 0;
	for (int y = info.height - 1; y >= 0; y--) {
		fp.ReadBytes (rowBuf, rowSize [nRow++]);
		rowPtr = rowBuf;
		int h = y * info.width;
		for (int x = 0; x < info.width; ) {
			palIndex = *rowPtr++;
			if (IS_RLE_CODE (palIndex)) {
				runLength = palIndex & ~RLE_CODE;
				if (runLength > info.width - x)
					runLength = info.width - x;
				palIndex = *rowPtr++;
				CBGR color = palette [palIndex];
				ubyte alpha = (palIndex < 254) ? 255 : 0;
				for (int j = 0; j < runLength; j++, x++, h++) {
					m_data [h] = color;
					m_data [h].a = alpha;
					}
				if (alpha < 255)
					m_info.bTransparent = true;
				if (palIndex == 254)
					m_info.bSuperTransparent = true;
				}
			else {
				m_data [h] = palette [palIndex];
				if (palIndex >= 254) {
					m_data [h].a = 0;
					m_info.bTransparent = true;
					if (palIndex == 254)
						m_info.bSuperTransparent = true;
					}
				x++, h++;
				}
			}
		}
	}
else {
	for (int y = info.height - 1; y >= 0; y--) {
		int h = y * info.width;
		for (int x = 0; x < info.width; x++, h++) {
			palIndex = fp.ReadUByte ();
			m_data [h] = palette [palIndex];
			if (palIndex >= 254) {
				m_data [h].a = 0;
				m_info.bTransparent = true;
				if (palIndex == 254)
					m_info.bSuperTransparent = true;
				}
			}
		}
	}
GenerateRenderBuffer ();
m_bValid = true;
}

//------------------------------------------------------------------------

int CTexture::LoadFromPig (CFileManager& fp, uint nIndex, int nVersion) 
{
if (m_info.bCustom)
	return 0;

#ifdef _DEBUG
if (nIndex == nDbgTexture)
	nDbgTexture = nDbgTexture;
#endif
CPigTexture* pInfo = textureManager.Info (nVersion, nIndex);
if (!pInfo)
	return 0;

int nSize = pInfo->width * pInfo->height;
if (m_data && (m_info.bufSize == nSize))
	return 0; // already loaded
m_info.nIndex = nIndex;
m_info.nTexture = textureManager.TexIdFromIndex (nIndex, nVersion);
m_info.format = BMP;
if (!Allocate (nSize)) 
	return 1;
fp.Seek (textureManager.TexDataOffset (nVersion) + pInfo->offset, SEEK_SET);
Load (fp, *pInfo);
return 0;
}

//------------------------------------------------------------------------

bool CTexture::LoadFromPog (CFileManager& fp, CPigTexture& info)
{
	uint nSize = (uint) info.width * (uint) info.height;

m_info.format = ((info.flags & 0x80) != 0) ? TGA : BMP;
if (!Allocate (nSize))
	return false;

Load (fp, info);
// POGs can override some things that aren't necessarily safe or have no practical effect -
// so we copy those from the base texture to avoid problems
const CTexture *pBaseTexture = textureManager.BaseTextures (Index ());
memcpy (m_info.szName, pBaseTexture->m_info.szName, sizeof (m_info.szName));
m_info.bAnimated = pBaseTexture->m_info.bAnimated;
m_info.nFrame = pBaseTexture->m_info.nFrame;
m_info.bCustom = 1;
return true;
}

//------------------------------------------------------------------------

bool CTexture::LoadFromFile (char* pszFile, bool bErrorOnOpenFail)
{
	char szExt [4] = "bmp";
	CFileManager fp;
	bool bSuccess = false;

if (strchr (pszFile, '.')) {
	strncpy_s (szExt, ARRAYSIZE (szExt), strrchr (pszFile, '.') + 1, 3);
	_strlwr_s (szExt, ARRAYSIZE (szExt));
	}
if (!fp.Open (pszFile, "rb")) {
	if (bErrorOnOpenFail)
		ErrorMsg ("Could not open texture file.");
	}
else {
	if (strcmp (szExt, "bmp") == 0)
		bSuccess = LoadBMP (fp);
	else if (strcmp (szExt, "tga") == 0)
		bSuccess = LoadTGA (fp);
	else {
		sprintf_s (message, ARRAYSIZE (message), "Unknown texture file type %s.", szExt);
		ErrorMsg (message);
		}
	}

fp.Close ();
if (bSuccess) {
	GenerateRenderBuffer ();
	m_bValid = true;
	}
return bSuccess;
}

//------------------------------------------------------------------------------

bool CTexture::LoadTGA (CFileManager& fp)
{
	tTgaHeader	tgaHeader;
	char			imgIdent [255];
	tBGRA			color;

fp.Read (&tgaHeader, sizeof (tgaHeader), 1);
int h = tgaHeader.width * tgaHeader.height;
if (h > 4096 * 4096) {
	ErrorMsg ("Image too large.");
	return false;
	}
if (!Allocate (h))
	return false;
m_info.width = tgaHeader.width;
m_info.height = tgaHeader.height;
SetFrameCount (MaxVal ((uint) 1, m_info.height / m_info.width));
if (tgaHeader.identSize)
	fp.Read (imgIdent, tgaHeader.identSize, 1);
int s = (tgaHeader.bits == 32) ? 4 : 3;
color.a = 255;

// textures are getting reversed here for easier rendering!
if (tgaHeader.yStart != 0) { 
	h = m_info.width * (m_info.height - 1);
	for (int i = m_info.height; i; i--) {
		for (int j = m_info.width; j; j--, h++) {
			fp.Read (&color, s, 1);
			m_data [h] = color;
			}
		h -= 2 * m_info.width;
		}
	}
else {
	h = 0;
	for (int i = m_info.height; i; i--) {
		for (int j = m_info.width; j; j--, h++) {
			fp.Read (&color, s, 1);
			m_data [h] = color;
			}
		}
	}
// Unsure so far whether this line is needed for anything - but it can mess with re-saving etc
//if (DLE.IsD2XLevel ())
	m_info.format = TGA;
m_bValid = true;
GenerateRenderBuffer ();
#if 0
else {
	if (!ToBitmap ())
		return false;
	m_info.nFormat = 0;
	}
#endif
return true;
}

//------------------------------------------------------------------------------

bool CTexture::LoadTGA (char* pszFile)
{
	CFileManager fp;

if (!fp.Open (pszFile, "rb"))
	return false;
bool bSuccess = LoadTGA (fp);
fp.Close ();
return bSuccess;
}

//------------------------------------------------------------------------------

inline int Sqr (int i)
{
return i * i;
}

//------------------------------------------------------------------------------

inline int ColorDelta (RGBQUAD *bmPal, PALETTEENTRY *sysPal, int j)
{
sysPal += j;
return 
	Sqr (int (bmPal->rgbBlue) - int (sysPal->peBlue)) + 
	Sqr (int (bmPal->rgbGreen) - int (sysPal->peGreen)) + 
	Sqr (int (bmPal->rgbRed) - int (sysPal->peRed));
}

//------------------------------------------------------------------------

bool CTexture::LoadBMP (CFileManager& fp)
{
	RGBQUAD* palette = null;
	PALETTEENTRY* sysPal = null;
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;
	ubyte palIndexTable [256];
	bool bFuncRes = false;
	uint x, y, width, paletteSize;
	bool bTopDown = false;

palette = new RGBQUAD [256];
if (palette == null) {
	ErrorMsg ("Not enough memory for palette.");
	goto errorExit;
	}

sysPal = new PALETTEENTRY [256];
if (sysPal == null) {
	ErrorMsg ("Not enough memory for palette.");
	goto errorExit;
	}

// read the header information
fp.Read (&bmfh, sizeof (bmfh), 1);
fp.Read (&bmih, sizeof (bmih), 1);

// handle exceptions
if (bmih.biClrUsed == 0)  
	bmih.biClrUsed = 256;
if (bmih.biHeight < 0) {
	bmih.biHeight = -bmih.biHeight;
	bTopDown = true;
	}

// make sure it is a bitmap file
if (bmfh.bfType != 'B' + (((ushort) 'M') << 8) ) {
	ErrorMsg ("This is not a bitmap file.");
	goto errorExit;
	}

// make sure it is a 256 or 16 color bitmap
if (bmih.biBitCount != 8 && bmih.biBitCount != 4) {
	ErrorMsg ("DLE only reads 16 or 256 color bitmap files.\n\n"
	"Hint: Load this image into a paint program\n"
	"then save it as a 16 or 256 color *.bmp file.");
	goto errorExit;
	}

// make sure the data is not compressed
if (bmih.biCompression != BI_RGB) {
	ErrorMsg ("Cannot read compressed bitmap files.\n\n"
	"Hint: Try to load this image into a paint program\n"
	"then save it as a 256 color *.bmp file with the\n"
	"compression option off.");
	goto errorExit;
	}

// read palette
paletteSize = min ((int) bmih.biClrUsed, 256);
if (paletteSize == 0)
	paletteSize = 1 << bmih.biBitCount;
fp.Read (palette, sizeof (RGBQUAD), paletteSize);

// read the logical palette entries
paletteManager.Render ()->GetPaletteEntries (0, 256, sysPal);

// check color palette
int i;
for (i = 0; i < int (paletteSize); i++) {
	palIndexTable [i] = i;
	if ((palette [i].rgbRed != sysPal [i].peRed) ||
		 (palette [i].rgbGreen != sysPal [i].peGreen) ||
		 (palette [i].rgbBlue != sysPal [i].peBlue)) {
		break;
		}		
	}

if (i != int (paletteSize)) {
	if (!DLE.ExpertMode ())
		ErrorMsg ("The palette of this bitmap file is not exactly the\n"
					 "the same as the Descent palette. Therefore, some color\n"
					 "changes may occur.\n\n"
					 "Hint: If you want the palettes to match, then save one of\n"
					 "the Descent textures to a file and use it as a starting point.\n"
					 "If you plan to use transparencies, then you may want to start\n"
					 "with the texture called 'empty'.");

	bool bAllowTransparent = false;
	bool bAllowColorMatchToTransparent = false;
	if (bmih.biClrUsed == 256)
		bAllowTransparent = Query2Msg ("Do you want to allow transparency for this texture?\n"
			"(The last two colors in the texture's palette will be treated as "
			"see-thru and transparent, respectively.)", MB_YESNO) == IDYES;
	else {
		bAllowTransparent = Query2Msg ("Do you want to allow transparency for this texture?\n"
			"(Because this texture has less than 256 colors, pixels will be "
			"treated as see-thru or transparent if they most closely match "
			"the current palette's see-thru or transparent color.)", MB_YESNO) == IDYES;
		bAllowColorMatchToTransparent = bAllowTransparent;
		}
	int colorsToMatch = bAllowColorMatchToTransparent ? 256 : 254;

	for (i = 0; i < int (paletteSize); i++) {
		uint closestIndex = i;
		if ((palette [i].rgbRed != sysPal [i].peRed) ||
			 (palette [i].rgbGreen != sysPal [i].peGreen) ||
			 (palette [i].rgbBlue != sysPal [i].peBlue)) {
			if (!bAllowTransparent || bAllowColorMatchToTransparent || ((uint) i < paletteSize - 2)) {
				uint closestDelta = 0x7fffffff;
				for (int j = 0; j < colorsToMatch; j++) {
					uint delta = ColorDelta (palette + i, sysPal, j);
					if (delta < closestDelta) {
						closestIndex = j;
						if (!(closestDelta = delta))
							break;
						}
					}
				}
			palette [i].rgbRed = sysPal [closestIndex].peRed;
			palette [i].rgbGreen = sysPal [closestIndex].peGreen;
			palette [i].rgbBlue = sysPal [closestIndex].peBlue;
			}
		palIndexTable [i] = closestIndex;
		}
	}

int x0, x1, y0, y1;
// if size is not 64 x 64, ask if they want to "size to fit"
if ((bmih.biWidth != Width ()) || (bmih.biHeight != Height ())) {
	sprintf_s (message, sizeof (message), 
				  "The bitmap being loaded is a %d x %d image.\n"
				  "Do you want the image to be sized to fit\n"
				  "the current %d x %d texture size?\n\n"
				  "(Press No to see another option.)", 
				 (int) bmih.biWidth, (int) bmih.biHeight, 
				 (int) Width (), (int) Height ());
	switch (Query2Msg (message, MB_YESNOCANCEL)) {
		case IDYES:
			x0 = 0;
			y0 = 0;
			x1 = (int) bmih.biWidth + 1;
			y1 = (int) bmih.biHeight + 1;
			break;

		case IDNO:
			if (Query2Msg ("Would you like to center/tile the image?\n"
			               "(Press No to use the image's current size.)", MB_YESNO) == IDYES) {
				x0 = (int)(bmih.biWidth - Width ()) / 2;
				y0 = (int)(bmih.biHeight - Height ()) / 2;
				x1 = x0 + Width ();
				y1 = y0 + Height ();
				}
			else if ((bmih.biWidth > 1024) || (bmih.biHeight > 1024)) 
				goto errorExit;
			else {
				x0 = 0;
				y0 = 0;
				x1 = m_info.width = (ushort) bmih.biWidth;
				y1 = m_info.height = (ushort) bmih.biHeight;
				if (!Allocate (m_info.width * m_info.height)) {
					ErrorMsg ("Unable to allocate space for new texture.");
					goto errorExit;
					}
				}
			break;

			default:
				goto errorExit;
		}
	}
else {
	x0 = 0;
	y0 = 0;
	x1 = m_info.width;
	y1 = m_info.height;
	}
if (bTopDown) {
	y0 = ~y0;
	y1 = ~y1;
	}

// save bitmap for undo command
//m_nSize = m_nWidth * m_nHeight;

// read data into bitmap
width = (((int)(bmih.biWidth * bmih.biBitCount + 31) >> 3)) & ~3;
double mx, my;
mx = (x1 - x0) / (double) m_info.width;
my = (y1 - y0) / (double) m_info.height;
uint z = 0;
for (y = 0; y < m_info.height; y++) {
	for (x = 0; x < m_info.width; x++, z++) {
		int u = (int) (mx * x + x0);
		int v = (int) (my * y + y0);
		u %= (int) bmih.biWidth;         //  -width to width
		if (u < 0) 
			u += (int) bmih.biWidth;		//       0 to width
		v %= (int) bmih.biHeight;        // -height to height
		if (v < 0) 
			v += (int) bmih.biHeight;		//       0 to height
	
		ubyte palIndex, i;

		if (bmih.biBitCount == 4) {
			long offset = (int) v * (int) width + (int) u / 2;
			fp.Seek ((int) bmfh.bfOffBits + offset, SEEK_SET);
			fp.Read (&palIndex, 1, 1);
			if (!(u & 1))
				palIndex >>= 4;
			palIndex &= 0x0f;
			i = palIndexTable [palIndex];
			(*this) [z] = i;
			}
		else {
			fp.Seek ((int) bmfh.bfOffBits + (int) v *(int) width + (int) u, SEEK_SET);
			fp.Read (&palIndex, 1, 1);
			i = palIndexTable [palIndex];
			(*this) [z] = palette [palIndex];
			}
		if (i >= 254)
			(*this) [z].a = 0;
		}
	}

m_info.format = BMP;
GenerateRenderBuffer ();
bFuncRes = true;

errorExit:

if (palette) 
	delete palette;
if (sysPal) 
	delete sysPal;
return bFuncRes;
}

//------------------------------------------------------------------------

void CTexture::CommitBufferChanges (void)
{
// Currently everything we need to do is called by this function anyway
GenerateRenderBuffer ();
}

//------------------------------------------------------------------------

bool CTexture::Save (char* pszFile) const
{
	CFileManager fp;
	bool bSuccess = false;

if (!fp.Open (pszFile, "wb")) {
	ErrorMsg ("Could not create texture file.");
	bSuccess = false;
	}
else {
	switch (Format ()) {
		case TGA:
			bSuccess = SaveTGA (fp);
			break;
		case BMP:
			bSuccess = SaveBMP (fp);
			break;
		default:
			sprintf_s (message, ARRAYSIZE (message), "Unknown texture format (%u).", Format ());
			ErrorMsg (message);
			bSuccess = false;
			break;
		}
	}
fp.Close ();
return bSuccess;
}

//------------------------------------------------------------------------

bool CTexture::SaveTGA (CFileManager& fp) const
{
	tTgaHeader	h;

memset (&h, 0, sizeof (h));
h.imageType = 2;
h.width = Width ();
h.height = Height ();
h.bits = 32;
fp.Write (&h, sizeof (h), 1);
int j = h.width * h.height;
for (int i = h.height; i; i--) {
	j -= h.width;
	fp.Write (&m_data [j], sizeof (CBGRA), h.width);
	}

return true;
}

//------------------------------------------------------------------------

bool CTexture::SaveBMP (CFileManager& fp) const
{
BITMAPFILEHEADER bmfh;

bmfh.bfType = 'B' + ('M' << 8);
bmfh.bfSize = sizeof (BITMAPFILEHEADER) + sizeof (BITMAPINFOHEADER) + 256 * 4 + Size ();
bmfh.bfReserved1 = 0;
bmfh.bfReserved2 = 0;
bmfh.bfOffBits   = sizeof (BITMAPFILEHEADER) + sizeof (BITMAPINFOHEADER) + 256 * 4;

// define the bitmap header (top-down)
BITMAPINFO* bmi = paletteManager.BMI ();
bmi->bmiHeader.biWidth = m_info.width;
bmi->bmiHeader.biHeight = -((LONG)m_info.height);
switch (Format ()) {
	case TGA:
		// For exporting TGAs to 32-bit bitmaps
		bmi->bmiHeader.biBitCount = 32;
		break;
	case BMP:
		bmi->bmiHeader.biBitCount = 8;
		break;
	default:
		break;
	}

// write the headers
fp.Write (&bmfh, sizeof (BITMAPFILEHEADER), 1);
fp.Write (&bmi->bmiHeader, sizeof (BITMAPINFOHEADER), 1);

// write palette
fp.Write (bmi->bmiColors, sizeof (RGBQUAD), 256);

// save bitmap data
textureManager.WriteCustomTexture (fp, this, true);

return true;
}

//------------------------------------------------------------------------

double CTexture::Scale (short nTexture) const
{
//if (!m_info.width)
//	if (nTexture < 0)
//		return 1.0;
//	else
//		Load (nTexture);
return m_info.width ? (double) m_info.width / 64.0 : 1.0;
}

//	-----------------------------------------------------------------------------

#if 1

#define BPP 4

int CTexture::Shrink (int xFactor, int yFactor)
{
	int		xSrc, ySrc, xMax, yMax, xDest, yDest, x, y, w, h, bSuperTransp;
	CBGRA		* pData, * pSrc, * pDest;
	CBGRA		superTranspKey;
	double	cSum [4], nFactor2, nSuperTransp;

if (!m_data)
	return 0;
if ((xFactor < 1) || (yFactor < 1))
	return 0;
if ((xFactor == 1) && (yFactor == 1))
	return 1;
superTranspKey = paletteManager.SuperTranspKey ();
superTranspKey.a = 0;
w = Width ();
h = Height ();
xMax = w / xFactor;
yMax = h / yFactor;
nFactor2 = xFactor * yFactor;
if (!(pData = new CBGRA [xMax * yMax]))
	return 0;
pDest = pData;

for (yDest = 0; yDest < yMax; yDest++) {
	for (xDest = 0; xDest < xMax; xDest++, pDest++) {
		memset (&cSum, 0, sizeof (cSum));
		ySrc = yDest * yFactor;
		nSuperTransp = 0;
		for (y = yFactor; y; ySrc++, y--) {
			xSrc = xDest * xFactor;
			pSrc = m_data + (ySrc * w + xSrc);
			for (x = xFactor; x; xSrc++, x--, pSrc++) {
				bSuperTransp = *pSrc == superTranspKey;
				if (bSuperTransp)
					nSuperTransp++;
				else {
					cSum [0] += pSrc->r;
					cSum [1] += pSrc->g;
					cSum [2] += pSrc->b;
					cSum [3] += pSrc->a;
					}
				}
			}
		if (nSuperTransp >= nFactor2 / 2)
			*pDest = superTranspKey;
		else {
			double scale = double (nFactor2 - nSuperTransp);
			pDest->r = (ubyte) (cSum [0] / scale + 0.5);
			pDest->g = (ubyte) (cSum [1] / scale + 0.5);
			pDest->b = (ubyte) (cSum [2] / scale + 0.5);
			pDest->a = (ubyte) (cSum [3] / scale + 0.5);
			}
		}
	}
Release ();
m_data = pData;
m_info.width = xMax;
m_info.height = yMax;
m_info.bufSize = xMax * yMax;
m_bValid = true;
GenerateRenderBuffer ();
return 1;
}

#endif

//------------------------------------------------------------------------

void CTexture::ApplyAlpha (void)
{

if (!m_data)
	return;

CBGRA	* pData = m_data;
CBGRA superTranspKey;

superTranspKey = paletteManager.SuperTranspKey ();
superTranspKey.a = 0;

for (int i = Width () * Height (); i; i--, pData++) {
	if (*pData == superTranspKey)
		pData->r =
		pData->g =
		pData->b =
		pData->a = 0;
	else {
		pData->r = ubyte (int (pData->r) * int (pData->a) / 255);
		pData->g = ubyte (int (pData->g) * int (pData->a) / 255);
		pData->b = ubyte (int (pData->b) * int (pData->a) / 255);
		}
	}
}

//------------------------------------------------------------------------

ubyte* CTexture::ToBitmap (void) const
{
	const CBGRA* pColor = Buffer ();
	CBGR* palette = paletteManager.Current ();
	int h = Size ();

ubyte* palIndex = new ubyte [h];
if (!palIndex)
	return null;
#if 1
ComputeIndex (palIndex);
#else
#ifndef _DEBUG
#	pragma omp parallel for
#endif
for (int i = 0; i < h; i++) 
	palIndex [i] = palette [paletteManager.ClosestColor (m_data [i])];
#endif
return palIndex;
}

//------------------------------------------------------------------------

ubyte* CTexture::ToBitmap (bool bShrink)
{
if (bShrink && !Shrink (Width () / 64, Height () / 64))
	return 0;
return ToBitmap ();
}

//------------------------------------------------------------------------

bool CTexture::GLCreate (bool bForce)
{
if (m_glHandle)
	if (!bForce)
		return true;
	else
		GLRelease ();

glGenTextures (1, &m_glHandle);
if (!m_glHandle)
	return false;
glEnable (GL_TEXTURE_2D);
glBindTexture (GL_TEXTURE_2D, m_glHandle);
// don't use height: Hires animations have all frames vertically stacked in one texture!
glTexImage2D (GL_TEXTURE_2D, 0, 4, RenderWidth (), RenderHeight (), 0, GL_BGRA, GL_UNSIGNED_BYTE, RenderBuffer ());
glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glDisable (GL_TEXTURE_2D);
return true;
}

//------------------------------------------------------------------------

GLuint CTexture::GLBind (GLuint nTMU, GLuint nMode) const
{
glActiveTexture (nTMU);
glClientActiveTexture (nTMU);
glEnable (GL_TEXTURE_2D);
if (m_glHandle)
	glBindTexture (GL_TEXTURE_2D, m_glHandle);
else
	return 0;
glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, nMode);
return m_glHandle;
}

//------------------------------------------------------------------------

void CTexture::GLRelease (void)
{
if (m_glHandle) {
	glDeleteTextures (1, &m_glHandle);
	m_glHandle = 0;
	}
}

//------------------------------------------------------------------------------

void CTexture::DrawLine (POINT pt0, POINT pt1, CBGRA color) 
{
CHECKMINE;

	int i, x, y;
	int dx = pt1.x - pt0.x;
	int dy = pt1.y - pt0.y;

	int xInc, yInc;
	double scale;
	int nStep = 0;

if (dx > 0)
	xInc = 1;
else {
	xInc = -1;
	dx = -dx;
	}
if (dy > 0)
	yInc = 1;
else {
	yInc = -1;
	dy = -dy;
	}
scale = Scale ();
xInc = (int) ((double) xInc * scale);
yInc = (int) ((double) yInc * scale);

x = pt0.x;
y = pt0.y;

#if 0	//most universal
int xStep = 0, yStep = 0;
int dd = (dx >= dy) ? dx: dy;
for (i = dd + 1; i; i--) {
	*Buffer (y * pTexture->m_info.width + x) = color;
	yStep += dy;
	if (yStep >= dx) {
		y += yInc;
		yStep = dx ? yStep % dx: 0;
		}
	xStep += dx;
	if (xStep >= dy) {
		x += xInc;
		xStep = dy ? xStep % dy: 0;
		}
	}
#else //0; faster
if (dx >= dy) {
	for (i = dx + 1; i; i--, x += xInc) {
		*Buffer (y * m_info.width + x) = color;
		nStep += dy;
		if (nStep >= dx) {
			y += yInc;
			nStep -= dx;
			}
		}
	}
else {
	for (i = dy + 1; i; i--, y += yInc) {
		*Buffer (y * m_info.width + x) = color;
		nStep += dx;
		if (nStep >= dy) {
			x += xInc;
			nStep -= dy;
			}
		}
	}
#endif //0
}

//------------------------------------------------------------------------------

void CTexture::DrawAnimDirArrows (short nTexture)
{
	int sx, sy;
	int bScroll = textureManager.ScrollSpeed (nTexture, &sx, &sy);

if (!bScroll)
	return;

	POINT *pointP;
	static POINT ptp0 [4] = {{54,32},{12,32},{42,42},{42,22}};
	static POINT pt0n [4] = {{32,12},{32,54},{42,22},{22,22}};
	static POINT ptn0 [4] = {{12,32},{54,32},{22,22},{22,42}};
	static POINT pt0p [4] = {{32,54},{32,12},{22,42},{42,42}};
	static POINT ptpn [4] = {{54,12},{12,54},{54,22},{42,12}};
	static POINT ptnn [4] = {{12,12},{54,54},{22,12},{12,22}};
	static POINT ptnp [4] = {{12,54},{54,12},{12,42},{22,54}};
	static POINT ptpp [4] = {{54,54},{12,12},{42,54},{54,42}};

if (sx >0 && sy==0) pointP = ptp0;
else if (sx >0 && sy >0) pointP = ptpp;
else if (sx==0 && sy >0) pointP = pt0p;
else if (sx <0 && sy >0) pointP = ptnp;
else if (sx <0 && sy==0) pointP = ptn0;
else if (sx <0 && sy <0) pointP = ptnn;
else if (sx==0 && sy <0) pointP = pt0n;
else if (sx >0 && sy <0) pointP = ptpn;

CBGRA color (255, 255, 255, 255);

DrawLine (pointP [0], pointP [1], color);
DrawLine (pointP [0], pointP [2], color);
DrawLine (pointP [0], pointP [3], color);
}

//------------------------------------------------------------------------

rgbaColorf& CTexture::GetAverageColor (rgbaColorf& color)
{
if (Flat ())
	color = m_info.averageColor;
else {
	CBGRA* pBuffer = Buffer ();
	color.r = color.g = color.b = color.a = 0.0f;
	if (pBuffer) {
		int bufSize = Size ();
		for (int i = bufSize; i; i--, pBuffer++) {
			color.r += pBuffer->r;
			color.g += pBuffer->g;
			color.b += pBuffer->b;
			color.a += pBuffer->a;
			}
		color.r /= float (bufSize);
		color.g /= float (bufSize);
		color.b /= float (bufSize);
		color.a /= float (bufSize);
		}
	}
return color;
}

//------------------------------------------------------------------------

bool CenterBitmapInBuffer (CBGRA *pDestBuffer, int destWidth, int destHeight, const CBGRA *pSrcBuffer, int srcWidth, int srcHeight)
{
if (!pDestBuffer || !pSrcBuffer)
	return false;

int srcX = max ((srcWidth - destWidth) / 2, 0);
int srcY = max ((srcHeight - destHeight) / 2, 0);
int destX = max ((destWidth - srcWidth) / 2, 0);
int destY = max ((destHeight - srcHeight) / 2, 0);

int nSize = destWidth * destHeight;
size_t elementSize = sizeof (pDestBuffer [0]);
memset (pDestBuffer, 0xFF, nSize * elementSize);

// Copy offset bitmap data into the buffer (row by row)
for (int y = srcY; y < min (srcHeight, destHeight); y++) {
	int destOffset = (y + destY) * destWidth + destX;
	int sourceOffset = (y + srcY) * srcWidth + srcX;
	memcpy_s (pDestBuffer + destOffset, (nSize - destOffset) * elementSize,
		pSrcBuffer + sourceOffset, min (srcWidth, destWidth) * elementSize);
	}

return true;
}

//------------------------------------------------------------------------

bool CTexture::CreateBitmap (CBitmap **ppImage, bool bScale, int width, int height) const
{
if (!ppImage)
	return false;

int actualWidth = 0, actualHeight = 0;
if (bScale && width > 0) {
	actualWidth = width;
	if (height > 0)
		actualHeight = height;
	else
		actualHeight = actualWidth;
	}
else {
	actualWidth = m_info.width;
	actualHeight = m_info.height;
	}

CBitmap *pNewImage = new CBitmap ();
bool bOk = pNewImage != null;
if (bOk) bOk = TRUE == pNewImage->CreateBitmap (actualWidth, actualHeight, 1, 32, null);
if (bOk) {
	const CBGRA *pBuffer = null;
	bool bOwnBuffer = false;
	BITMAPINFO* bmi = paletteManager.BMI ();
	CTexture scaledTexture;

	if (bScale) {
		const CBGRA *pSourceBuffer = null;
		int srcWidth, srcHeight;

		// Scale texture to as close to the desired size as possible
		// (using FrameHeight since we currently only want the first frame for a CBitmap)
		if (actualWidth <= (int)Width () || actualHeight <= (int)FrameHeight ()) {
			if (bOk) bOk = scaledTexture.Copy (*this);
			int factor = (int)max (ceil ((float)Width () / actualWidth), ceil ((float)FrameHeight () / actualHeight));
			if (bOk) 
				bOk = (0 < scaledTexture.Shrink (factor, factor));
			pSourceBuffer = scaledTexture.Buffer ();
			srcWidth = scaledTexture.Width ();
			srcHeight = scaledTexture.FrameHeight ();
			}
		else {
			pSourceBuffer = Buffer ();
			srcWidth = Width ();
			srcHeight = FrameHeight ();
			}

		// Now center texture
		if (srcWidth == actualWidth && srcHeight == actualHeight) {
			// No centering necessary, just use the buffer as-is
			pBuffer = pSourceBuffer;
			}
		else {
			CBGRA *pDestBuffer = new CBGRA [actualWidth * actualHeight];
			if (bOk) bOk = pDestBuffer != null;
			if (bOk) bOk = CenterBitmapInBuffer (pDestBuffer, actualWidth, actualHeight, pSourceBuffer, srcWidth, srcHeight);
			pBuffer = pDestBuffer;
			bOwnBuffer = true;
			}
		}
	else {
		pBuffer = Buffer ();
		}
	bmi->bmiHeader.biWidth = actualWidth;
	bmi->bmiHeader.biHeight = actualHeight;
	bmi->bmiHeader.biBitCount = 32;
	bmi->bmiHeader.biSizeImage = ((((bmi->bmiHeader.biWidth * bmi->bmiHeader.biBitCount) + 31) & ~31) >> 3) * bmi->bmiHeader.biHeight;

	if (bOk) bOk = 0 < SetDIBits (null, *pNewImage, 0, bmi->bmiHeader.biHeight, pBuffer, bmi, DIB_RGB_COLORS);
	if (bOwnBuffer && pBuffer)
		delete pBuffer;
	}
if (bOk) *ppImage = pNewImage;

if (!bOk && pNewImage) {
	delete pNewImage;
	}
return bOk;
}

//------------------------------------------------------------------------

bool CTexture::IsAssignableFrame (void) const
{
if (m_info.nTexture >= 0)
	return strstr (textureManager.Name (-1, m_info.nTexture), "frame") != null;
return false;
}

//------------------------------------------------------------------------

void CTexture::CalculateFrameCount (void)
{
	uint nFrames = 1;

// Frame count is only intended to be used for the first frame
if (m_info.nFrame > 0)
	return;

for (uint nTexFrame = Index () + 1; nTexFrame < (uint)textureManager.GlobalTextureCount (); nTexFrame++) {
	if (textureManager.TextureByIndex (nTexFrame)->FrameNumber () == 0)
		break;
	nFrames++;
	}

// For non-animated textures we don't need a frame-count
if (nFrames == 1)
	return;
m_info.nFrameCount = nFrames;
}

//------------------------------------------------------------------------

const CTexture *CTexture::GetParent (void) const
{
// We don't want to use IsAnimated because it doesn't always count frames
if (!MaybeAnimated ())
	return null;
if (FrameNumber () == 0)
	return this;
return textureManager.TextureByIndex (Index () - FrameNumber ());
}

//------------------------------------------------------------------------

const CTexture *CTexture::GetFrame (uint nFrame) const
{
if (!IsAnimated () || FrameNumber () != 0)
	return null;
if (nFrame == 0)
	return this;
return textureManager.TextureByIndex (Index () + nFrame);
}

//------------------------------------------------------------------------

bool CTexture::IsAnimated (int nVersion) const
{
return textureManager.HaveAnimationInfo (nVersion) ? textureManager.IsAnimationRoot (-int (Index ()) - 1, nVersion) : MaybeAnimated ();
}

//------------------------------------------------------------------------

bool CTexture::IsSingleImageAnimation (int nVersion) const
{
// The "column" animation format only applies to TGAs currently
if (Format () != TGA)
	return false;

// If this is part of an animation, but is not the base frame, it doesn't count
// (though if the base frame is a single-image animation itself, this image won't be displayed)
if (!IsAnimated (nVersion) || FrameNumber () != 0)
	return false;

// Treat this image as a single-image animation only if it is taller than it is wide
// and the height is an EXACT MULTIPLE of the width, otherwise we will have problems
if (Height () <= Width ())
	return false;
return (Height () % Width () == 0);
}

//------------------------------------------------------------------------
//eof textures.cpp