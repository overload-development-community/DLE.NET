// palette.cpp

#include "stdafx.h"
#include "PaletteManager.h"

HGLOBAL hPalette;

CPaletteManager paletteManager;

//------------------------------------------------------------------------

int Luminance (int r, int g, int b)
{
	int minColor, maxColor;

if (r < g) {
	minColor = (r < b) ? r : b;
	maxColor = (g > b) ? g : b;
	}
else {
	minColor = (g < b) ? g : b;
	maxColor = (r > b) ? r : b;
	}
return (minColor + maxColor) / 2;
}

//------------------------------------------------------------------------

void CPaletteManager::SetupBMI (CBGR* palette) 
{
m_bmi.header.biSize = sizeof (BITMAPINFOHEADER);
m_bmi.header.biWidth = 64;
m_bmi.header.biHeight = 64;
m_bmi.header.biPlanes = 1;
m_bmi.header.biBitCount = 8;
m_bmi.header.biCompression = BI_RGB;
m_bmi.header.biSizeImage = 0;
m_bmi.header.biXPelsPerMeter = 0;
m_bmi.header.biYPelsPerMeter = 0;
m_bmi.header.biClrUsed = 256;
m_bmi.header.biClrImportant = 0;
uint* rgb = (uint*) &m_bmi.colors [0];
for (int i = 0; i < 256; i++) {
	m_bmi.colors [i].rgbRed = palette [i].r;
	m_bmi.colors [i].rgbGreen = palette [i].g;
	m_bmi.colors [i].rgbBlue = palette [i].b;
	}
}

//------------------------------------------------------------------------

inline ubyte CPaletteManager::FadeValue (ubyte c, int f)
{
return (ubyte) (((int) c * f) / 34);
}

//------------------------------------------------------------------------

void CPaletteManager::Decode (CBGR* dest)
{
for (int i = 0, j = 0; i < 256; i++) {
	dest [i].r = m_rawData [j++] * 4;
	dest [i].g = m_rawData [j++] * 4;
	dest [i].b = m_rawData [j++] * 4;
	}
m_superTransp = dest [254];
m_superTranspf.r = float (m_superTransp.r) / 255.0f;
m_superTranspf.g = float (m_superTransp.g) / 255.0f;
m_superTranspf.b = float (m_superTransp.b) / 255.0f;
}

//------------------------------------------------------------------------

void CPaletteManager::Encode (CBGR* src)
{
for (int i = 0, j = 0; i < 256; i++) {
	m_rawData [j++] = src [i].r / 4;
	m_rawData [j++] = src [i].g / 4;
	m_rawData [j++] = src [i].b / 4;
	}
}

//------------------------------------------------------------------------

void CPaletteManager::CreateFadeTable (void)
{
for (int i = 0; i < 256; i++) {
	ubyte c = m_rawData [i];
	for (int j = 0; j < 34; j++)
		m_fadeTables [j * 256 + i] = FadeValue (c, j + 1);
	}
}

//------------------------------------------------------------------------

int CPaletteManager::LoadCustom (CFileManager* fp, long size)
{
if (fp->Read (m_rawData, 1, sizeof (m_rawData)) != sizeof (m_rawData))
	return 0;
CreateFadeTable ();
Decode (m_custom);
m_bHaveCustom = true;
//SetupRender (m_custom);
//SetupBMI (m_custom);
return 1;
}

//------------------------------------------------------------------------

int CPaletteManager::SaveCustom (CFileManager* fp)
{
return fp->Write (m_rawData, 1, sizeof (m_rawData)) == sizeof (m_rawData);
}

//------------------------------------------------------------------------

CBGR* CPaletteManager::LoadDefault (char* pszName)
{
	auto paletteData = g_data.LoadPaletteData(pszName);
	memcpy(m_rawData, paletteData.data(), sizeof(m_rawData));
	Decode(m_default);
	CreateFadeTable();
	SetupBMI(m_default);
	m_bHaveDefault = true;
	return m_default;
}

//------------------------------------------------------------------------

CBGR* CPaletteManager::Current (int i, char* pszName)
{
if (!pszName) {
	if (m_bHaveCustom)
		return m_custom + i;
	if (m_bHaveDefault)
		return m_default + i;
	}
LoadDefault (pszName);
return (m_default == null) ? null : m_default + i;
}

//------------------------------------------------------------------------

void CPaletteManager::LoadName (CFileManager* fp)
{
int i;
for (i = 0; i < 15; i++) {
	m_name [i] = fp->ReadChar ();
	if (m_name [i] == 0x0a) {
		m_name [i] = 0;
		break;
		}
	}
m_name [sizeof (m_name) - 1] = '\0';
// replace extension with .pig
if (i < 4) 
	strcpy_s (m_name, sizeof (m_name), "groupa.pig");
else {
	int l = (int) strlen (m_name) - 4;
	strcpy_s (m_name + l, sizeof (m_name) - l, ".pig");
	}
}

//------------------------------------------------------------------------------

#define USE_CLUT 1

ubyte CPaletteManager::ClosestColor (CBGR& color, bool bAllowTransp)
{

	CBGR* palette = Current ();
	uint delta, closestDelta = 0x7fffffff;
	ubyte closestIndex = 0;

#if USE_CLUT
static CBGR* clutPalette = null;
static bool bClutTransp = true;
if (clutPalette != palette || bClutTransp != bAllowTransp) {
	clutPalette = palette;
	bClutTransp = bAllowTransp;
	ResetCLUT ();
	}
int l = (int) color.r + ((int) color.g << 8) + ((int) color.b << 16);
uint f = 1 << (l & 31);
if ((m_clutValid [l / 32] & f) != 0)
	return m_clut [l];
#endif
for (int i = 0; i < (bClutTransp ? 256 : 254); i++) {
	delta = color.Delta (palette [i]);
	if (delta < closestDelta) {
		closestIndex = i;
		if (delta == 0)
			break;
		closestDelta = delta;
		}
	}
#if USE_CLUT
m_clut [l] = closestIndex;
m_clutValid [l / 32] |= f;
#endif
return closestIndex;
}

//------------------------------------------------------------------------------

int CPaletteManager::NumAvailablePalettes (int nVersion)
{
	// Hard coding for now but in future we might allow custom palettes
	return (nVersion > 0) ? 6 : 1;
}

//------------------------------------------------------------------------------

const char* CPaletteManager::AvailablePaletteName (int nPalette, int nVersion)
{
	static const char* paletteNamesD1 [] = { "Descent" };
	static const char* paletteNamesD2 [] = { "GroupA", "Water", "Fire", "Ice", "Alien1", "Alien2" };

if (nVersion < 0 || nVersion > 1)
	return null;
else if (nVersion == 0) {
	if (nPalette < 0 || (uint)nPalette >= ARRAYSIZE (paletteNamesD1))
		return null;
	return paletteNamesD1 [nPalette];
	}
else {
	if (nPalette < 0 || (uint)nPalette >= ARRAYSIZE (paletteNamesD2))
		return null;
	return paletteNamesD2 [nPalette];
	}
}

//------------------------------------------------------------------------------

UINT CPaletteManager::GetPaletteEntries(UINT nStartIndex, UINT nNumEntries, LPPALETTEENTRY lpPaletteColors)
{
	return ::GetPaletteEntries (RenderCurrentPalette(), nStartIndex, nNumEntries, lpPaletteColors);
}

//------------------------------------------------------------------------------

HPALETTE CPaletteManager::RenderCurrentPalette()
{
	static CBGR* palette = nullptr;
	static HPALETTE renderedPalette = nullptr;
	// Return cached palette if it hasn't changed (there is a good chance pointer comparisons
	// won't work consistently here. We'll get rid of MFC soon anyway though)
	if (palette == paletteManager.Default())
	{
		return renderedPalette;
	}
	else
	{
		palette = paletteManager.Default();
		if (renderedPalette)
		{
			::DeleteObject(renderedPalette);
		}
	}
	if (palette == nullptr)
	{
		// No default palette, can't render it
		return nullptr;
	}

	std::vector<byte> dlcLogBuffer(offsetof(LOGPALETTE, palPalEntry) + sizeof(PALETTEENTRY) * 256);
	LPLOGPALETTE dlcLog = reinterpret_cast<LPLOGPALETTE>(dlcLogBuffer.data());
	dlcLog->palVersion = 0x300;
	dlcLog->palNumEntries = 256;
	PALETTEENTRY* rgb = &dlcLog->palPalEntry[0];
	for (int i = 0; i < 256; i++)
	{
		rgb[i].peRed = palette[i].r;
		rgb[i].peGreen = palette[i].g;
		rgb[i].peBlue = palette[i].b;
	}

	renderedPalette = ::CreatePalette(dlcLog);
	return renderedPalette;
}
