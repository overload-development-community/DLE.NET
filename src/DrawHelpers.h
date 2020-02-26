#pragma once

class DrawHelpers
{
public:
	static void DrawGameObjectBitmap(CGameObject* object, CWnd* pWindow);

	static void RgbFromIndex(int nIndex, PALETTEENTRY& rgb);

	static bool PaintTexture(CWnd* pWindow, int bkColor = -1, int texture1 = -1, int texture2 = 0, int xOffset = 0, int yOffset = 0);

	static bool PaintTexture(CWnd* pWindow, int bkColor = -1,
		const CTexture* pBaseTex = null, const CTexture* pOvlTex = null, short nOvlAlignment = 0,
		int xOffset = 0, int yOffset = 0, bool bCurrentFrameOnly = true);

	static bool TGA2Bitmap(tRGBA* pTGA, ubyte* pBM, int nWidth, int nHeight);
	static bool LoadTGATexture(CTexture* texture, char* pszFile);
	static void LoadTextureFromResource(CTexture* texture, int nId);
	static bool CreateBitmapFromTexture(CTexture* texture, CBitmap** ppImage, bool bScale = false, int width = -1, int height = -1) const;

	static bool GLCreateTexture(CTexture* texture, bool bForce);
	static GLuint GLBindTexture(CTexture* texture, GLuint nTMU, GLuint nMode);
	static void GLReleaseTexture(CTexture* texture);
};
