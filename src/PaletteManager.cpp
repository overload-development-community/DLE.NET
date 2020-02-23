// palette.cpp

#include "stdafx.h"
#include "dle-xp-res.h"

#include <string.h>

#include "define.h"
#include "types.h"
#include "dle-xp.h"
#include "mine.h"
#include "global.h"
#include "FileManager.h"
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

void CPaletteManager::FreeRender (void)
{
if (m_render) {
	delete m_render;
	m_render = null;
	}
if (m_dlcLog) {
	delete m_dlcLog;
	m_dlcLog = null;
	}
if (m_colorMap) {
	delete m_colorMap;
	m_colorMap = null;
	}
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

short CPaletteManager::SetupRender (CBGR* palette)
{
FreeRender ();
if (!(m_dlcLog = (LPLOGPALETTE) new ubyte [sizeof (LOGPALETTE) + sizeof (PALETTEENTRY) * 256]))
	return 1;
m_dlcLog->palVersion = 0x300;
m_dlcLog->palNumEntries = 256;
PALETTEENTRY* rgb = &m_dlcLog->palPalEntry [0];
for (int i = 0; i < 256; i++) {
	rgb [i].peRed = palette [i].r;
	rgb [i].peGreen = palette [i].g;
	rgb [i].peBlue = palette [i].b;
	}
if (!(m_render = new CPalette ()))
	return 1;
m_render->CreatePalette (m_dlcLog);
m_colorMap = new PALETTEENTRY [256];
m_render->GetPaletteEntries (0, 256, m_colorMap);
return m_render == null;
return 1;
}

//------------------------------------------------------------------------

CBGR* CPaletteManager::LoadDefault (char* pszName)
{
CResource res;
if (!res.Load (SelectResource (pszName)))
	return null;
memcpy (m_rawData, res.Data (), sizeof (m_rawData));
Decode (m_default);
CreateFadeTable ();
SetupRender (m_default);
SetupBMI (m_default);
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
// PaletteResource()
//
// Action - returns the name of the palette resource.  Neat part about
//          this function is that the strings are automatically stored
//          in the local heap so the string is static.
//
//------------------------------------------------------------------------

const char* CPaletteManager::SelectResource (char* pszName) 
{
	typedef struct tPalExt {
		char	szFile [256];
		int	nIdPal;
	} tPalExt;

	static tPalExt palExt [] = {
		{"groupa", IDR_GROUPA_256}, 
		{"alien1", IDR_ALIEN1_256}, 
		{"alien2", IDR_ALIEN2_256}, 
		{"fire", IDR_FIRE_256}, 
		{"water", IDR_WATER_256}, 
		{"ice", IDR_ICE_256},
		{"", 0}
	};

	tPalExt	*ppe;
	char		szName [256];

if (!pszName || !*pszName) {
	if (theMine && DLE.IsD1File ())
		return MAKEINTRESOURCE (IDR_PALETTE_256);
	CFileManager::SplitPath (descentFolder [1], null, szName, null);
	pszName = szName;
	}
else {
	CFileManager::SplitPath (pszName, NULL, szName, NULL);
	_strlwr (szName);
	if (!_stricmp ("descent", szName))
		return MAKEINTRESOURCE (IDR_PALETTE_256);
	for (ppe = palExt; *(ppe->szFile); ppe++)
		if (!_stricmp (ppe->szFile, szName))
			return MAKEINTRESOURCE (ppe->nIdPal);
	}
return MAKEINTRESOURCE (IDR_GROUPA_256);
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

