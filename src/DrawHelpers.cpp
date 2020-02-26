#include "DrawHelpers.h"

void DrawHelpers::DrawGameObjectBitmap (CGameObject* object, CWnd* pWindow)
{
	static int powerupTable [48] = {
		 0,  1,  2,  3,  4,  5,  6, -1, -1, -1, 
		 7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 
		17, 18, 19, 20, -1, 21, -1, -1, 22, 23, 
		24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 
		34, 35, 36, 37, 38, 39, 40, 41
		};

	int nBitmap = -1;

// figure out object number based on object type and id

switch (object->m_info.type) {
	case OBJ_PLAYER:
		nBitmap = 0;
		break;
	case OBJ_COOP:
		nBitmap = 0;
		break;
	case OBJ_ROBOT:
		nBitmap = (object->m_info.id < 66) ? 1 + object->m_info.id : 118 + (object->m_info.id - 66);
		break;
	case OBJ_REACTOR:
	if (DLE.IsD1File ())
		nBitmap = 67;
	else
		switch (object->m_info.id) {
			case 0: nBitmap = 67; break;
			case 1: nBitmap = 68; break;
			case 2: nBitmap = 69; break;
			case 3: nBitmap = 70; break;
			case 4: nBitmap = 71; break;
			case 5: nBitmap = 72; break;
			case 6: nBitmap = 73; break;
			default: nBitmap = 69; break; // level 1's reactor
			}
		break;
	case OBJ_WEAPON:
		nBitmap = 74;
		break;
	case OBJ_HOSTAGE:
		nBitmap = 75;
		break;
	case OBJ_POWERUP:
		if ((object->m_info.id >= 0) && (object->m_info.id < MAX_POWERUP_IDS) && (powerupTable [object->m_info.id] >= 0))
			nBitmap = 76 + powerupTable [object->m_info.id];
		break;
	default:
		nBitmap = -1; // undefined
}

CDC *pDC = pWindow->GetDC ();
CRect rc;
pWindow->GetClientRect (rc);
pDC->FillSolidRect (&rc, IMG_BKCOLOR);
if ((nBitmap >= 0) && (nBitmap <= 129)) {
	sprintf_s (message, sizeof (message),"OBJ_%03d_BMP", nBitmap);
	CResource res;
	BITMAPINFO *bmi = (BITMAPINFO *) res.Load (message, RT_BITMAP);
	if (bmi) {	//if not, there is a problem in the resource file
		int ncolors = (int) bmi->bmiHeader.biClrUsed;
		if (ncolors == 0)
			ncolors = 1 << (bmi->bmiHeader.biBitCount); // 256 colors for 8-bit data
		char *pImage = (char*) bmi + sizeof (BITMAPINFOHEADER) + ncolors * 4;
		int width = (int) bmi->bmiHeader.biWidth;
		int height = (int) bmi->bmiHeader.biHeight;
		int xoffset = (64 - width) / 2;
		int yoffset = (64 - height) / 2;
		SetDIBitsToDevice (pDC->m_hDC, xoffset, yoffset, width, height, 0, 0, 0, height,pImage, bmi, DIB_RGB_COLORS);
		}
	}
pWindow->ReleaseDC (pDC);
pWindow->InvalidateRect (null, TRUE);
pWindow->UpdateWindow ();
}

//------------------------------------------------------------------------------

void DrawHelpers::RgbFromIndex (int nIndex, PALETTEENTRY& rgb)
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

bool DrawHelpers::PaintTexture (CWnd *pWindow, int bkColor, int texture1, int texture2, int xOffset, int yOffset)
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

bool DrawHelpers::PaintTexture (CWnd *pWindow, int bkColor, const CTexture *pBaseTex, const CTexture *pOvlTex, short nOvlAlignment, int xOffset, int yOffset, bool bCurrentFrameOnly)
{
if (!pWindow->m_hWnd)
	return false;

	static int nOffset [2] = {0, 0};

	CDC			*pDC = pWindow->GetDC ();

if (!pDC)
	return false;

	CBitmap			bmTexture;
	IFileManager	fp;
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

bool DrawHelpers::LoadTGATexture(CTexture* texture, char* pszFile)
{
	CFileManager fp;

	if (!fp.Open(pszFile, "rb"))
		return false;
	bool bSuccess = texture->LoadTGA(fp);
	fp.Close();
	return bSuccess;
}

void DrawHelpers::LoadTextureFromResource(CTexture* texture, int nId)
{
	CResource res;
	ubyte* pData = res.Load(nId);
	if (!pData)
		return;
	texture->LoadFromData(pData, res.Size());
}

bool DrawHelpers::CreateBitmapFromTexture(CTexture* texture, CBitmap** ppImage, bool bScale, int width, int height) const
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

bool DrawHelpers::GLCreateTexture (CTexture* texture, bool bForce)
{
if (texture->m_glHandle)
	if (!bForce)
		return true;
	else
		GLRelease ();

glGenTextures (1, &texture->m_glHandle);
if (!texture->m_glHandle)
	return false;
glEnable (GL_TEXTURE_2D);
glBindTexture (GL_TEXTURE_2D, texture->m_glHandle);
// don't use height: Hires animations have all frames vertically stacked in one texture!
glTexImage2D (GL_TEXTURE_2D, 0, 4, texture->RenderWidth (), texture->RenderHeight (), 0, GL_BGRA, GL_UNSIGNED_BYTE, texture->RenderBuffer ());
glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glDisable (GL_TEXTURE_2D);
return true;
}

//------------------------------------------------------------------------

GLuint DrawHelpers::GLBindTexture (CTexture* texture, GLuint nTMU, GLuint nMode)
{
glActiveTexture (nTMU);
glClientActiveTexture (nTMU);
glEnable (GL_TEXTURE_2D);
if (texture->m_glHandle)
	glBindTexture (GL_TEXTURE_2D, texture->m_glHandle);
else
	return 0;
glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, nMode);
return texture->m_glHandle;
}

//------------------------------------------------------------------------

void DrawHelpers::GLReleaseTexture (CTexture* texture)
{
if (texture->m_glHandle) {
	glDeleteTextures (1, &texture->m_glHandle);
	texture->m_glHandle = 0;
	}
}
