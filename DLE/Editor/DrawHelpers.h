#pragma once

class DrawHelpers
{
public:
	static void DrawGameObjectBitmap(CGameObject* object, CWnd* pWindow);

	static bool TGA2Bitmap(tRGBA* pTGA, ubyte* pBM, int nWidth, int nHeight);
	static bool CreateBitmapFromTexture(const CTexture* texture, CBitmap** ppImage, bool bScale = false, int width = -1, int height = -1);
};

void RgbFromIndex(int nIndex, PALETTEENTRY& rgb);
bool PaintTexture(CWnd* pWindow, int bkColor = -1, int texture1 = -1, int texture2 = 0, int xOffset = 0, int yOffset = 0);
bool PaintTexture(CWnd* pWindow, int bkColor = -1,
	const CTexture* pBaseTex = null, const CTexture* pOvlTex = null, short nOvlAlignment = 0,
	int xOffset = 0, int yOffset = 0, bool bCurrentFrameOnly = true);
