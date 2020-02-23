
#include "stdafx.h"

#include "define.h"
#include "types.h"
#include "mine.h"
#include "dle-xp.h"
#include "TextureManager.h"
#include "texturefilter.h"

//------------------------------------------------------------------------

#define TEXFILTER_SIZE_D1		154
#define TEXFILTER_SIZE_D2		174

#define TEXFILTER_SIZE	(DLE.IsD1File () ? TEXFILTER_SIZE_D1 : TEXFILTER_SIZE_D2)
#define TEXTURE_FILTERS (DLE.IsD1File () ? texFiltersD1 : texFiltersD2)

//------------------------------------------------------------------------

static tTexFilterInfo texFiltersD1 [TEXFILTER_SIZE_D1] = {
	{0, 5, TEX_GRAY_ROCK, 0},
	{6, 6, TEX_BLUE_ROCK, 0},
	{7, 7, TEX_YELLOW_ROCK, 0},
	{8, 8, TEX_GRAY_ROCK, 0},
	{9, 10, TEX_RED_ROCK, 0},
	{11, 11, TEX_BLUE_ROCK, 0},
	{12, 12, TEX_GRAY_ROCK, TEX_STONES},
	{13, 13, TEX_GRAY_ROCK, 0},
	{14, 14, TEX_TARMAC, 0},
	{15, 15, TEX_GRAY_ROCK, 0},
	{16, 16, TEX_BLUE_ROCK, 0},
	{17, 17, TEX_RED_ROCK, 0},
	{18, 18, TEX_BLUE_ROCK, TEX_GRAY_ROCK},
	{19, 19, TEX_RED_ROCK, 0},
	{20, 21, TEX_STEEL, 0},
	{22, 30, TEX_RED_ROCK, 0},
	{31, 31, TEX_BROWN_ROCK, 0},
	{32, 32, TEX_RED_ROCK, TEX_STONES},
	{33, 38, TEX_RED_ROCK, 0},
	{39, 39, TEX_BRICK, 0},
	{40, 40, TEX_GRAY_ROCK, 0},
	{41, 41, TEX_FLOOR, 0},
	{42, 42, TEX_GRAY_ROCK, 0},
	{43, 43, TEX_RED_ROCK, 0},
	{44, 44, TEX_BLUE_ROCK, 0},
	{45, 45, TEX_RED_ROCK, TEX_LAVA},
	{46, 47, TEX_RED_ROCK, 0},
	{48, 49, TEX_GRAY_ROCK, 0},
	{50, 50, TEX_RED_ROCK, 0},
	{51, 53, TEX_BROWN_ROCK, 0},
	{54, 55, TEX_RED_ROCK, 0},
	{56, 56, TEX_BROWN_ROCK, 0},
	{57, 57, TEX_RED_ROCK, 0},
	{58, 58, TEX_GRAY_ROCK, TEX_STONES},
	{59, 59, TEX_ICE, 0},
	{60, 60, TEX_BROWN_ROCK, TEX_STONES},
	{61, 61, TEX_GRAY_ROCK, 0},
	{62, 66, TEX_BROWN_ROCK, 0},
	{67, 69, TEX_GRAY_ROCK, 0},
	{70, 73, TEX_GREEN_ROCK, 0},
	{74, 75, TEX_RED_ROCK, 0},
	{76, 77, TEX_BLUE_ROCK, 0},
	{78, 78, TEX_BROWN_ROCK, 0},
	{79, 79, TEX_BLUE_ROCK, TEX_ICE},
	{80, 81, TEX_BROWN_ROCK, 0},
	{82, 82, TEX_GRAY_ROCK, 0},
	{83, 83, TEX_BLUE_ROCK, 0},
	{84, 85, TEX_GRAY_ROCK, 0},
	{86, 87, TEX_BLUE_ROCK, 0},
	{88, 89, TEX_BROWN_ROCK, 0},
	{90, 90, TEX_BLUE_ROCK, 0},
	{91, 91, TEX_BROWN_ROCK, 0},
	{92, 96, TEX_RED_ROCK, 0},
	{97, 97, TEX_BROWN_ROCK, 0},
	{98, 98, TEX_GREEN_ROCK, 0},
	{99, 99, TEX_BROWN_ROCK, 0},
	{100, 100, TEX_GRAY_ROCK, 0},
	{101, 105, TEX_BROWN_ROCK, 0},
	{106, 109, TEX_GREEN_ROCK, 0},
	{110, 110, TEX_BLUE_ROCK, TEX_ICE},
	{111, 116, TEX_GREEN_ROCK, TEX_GRASS},
	{117, 118, TEX_BROWN_ROCK, 0},
	{119, 120, TEX_CONCRETE, 0},
	{121, 122, TEX_BROWN_ROCK, 0},
	{123, 123, TEX_GREEN_ROCK, TEX_GRASS},
	{124, 125, TEX_BROWN_ROCK, 0},
	{126, 126, TEX_BROWN_ROCK, TEX_BLUE_ROCK},
	{127, 127, TEX_BLUE_ROCK, 0},
	{128, 141, TEX_RED_ROCK, 0},
	{142, 143, TEX_RED_ROCK, TEX_LAVA},
	{144, 144, TEX_BLUE_ROCK, TEX_ICE},
	{145, 145, TEX_GREEN_ROCK, 0},
	{146, 146, TEX_BLUE_ROCK, TEX_ICE},
	{147, 149, TEX_BROWN_ROCK, 0},
	{150, 151, TEX_RED_ROCK, 0},
	{152, 153, TEX_GREEN_ROCK, TEX_STONES},
	{154, 154, TEX_GRAY_ROCK, TEX_LAVA},
	{155, 155, TEX_BROWN_ROCK, TEX_LAVA},
	{156, 157, TEX_STEEL, 0},
	{158, 158, TEX_CONCRETE, TEX_WALL},
	{159, 159, TEX_TECH, TEX_WALL},
	{160, 160, TEX_CONCRETE, TEX_WALL},
	{161, 161, TEX_TECH, TEX_WALL},
	{162, 163, TEX_CONCRETE, TEX_WALL},
	{164, 164, TEX_CONCRETE, TEX_SIGN | TEX_WALL},
	{165, 171, TEX_CONCRETE, TEX_WALL},
	{172, 173, TEX_CONCRETE, TEX_SIGN | TEX_WALL},
	{174, 185, TEX_CONCRETE, TEX_WALL},
	{186, 190, TEX_STEEL, 0},
	{191, 196, TEX_CONCRETE, TEX_WALL},
	{197, 197, TEX_STEEL, 0},
	{198, 199, TEX_CONCRETE, TEX_WALL},
	{200, 200, TEX_CONCRETE, TEX_WALL | TEX_GRATE},
	{201, 207, TEX_CONCRETE, TEX_WALL},
	{208, 208, TEX_CONCRETE, TEX_WALL | TEX_SIGN},
	{209, 211, TEX_CONCRETE, TEX_WALL},
	{212, 214, TEX_CONCRETE, TEX_WALL | TEX_LIGHT},
	{215, 217, TEX_CONCRETE, TEX_WALL},
	{218, 218, TEX_CONCRETE, TEX_WALL | TEX_GRATE},
	{219, 219, TEX_CONCRETE, TEX_WALL},
	{220, 220, TEX_CONCRETE, TEX_WALL | TEX_GRATE},
	{221, 222, TEX_CONCRETE, TEX_WALL | TEX_LIGHT},
	{223, 230, TEX_TECH, 0},
	{231, 234, TEX_CONCRETE, TEX_WALL},
	{235, 237, TEX_CONCRETE, TEX_WALL | TEX_GRATE},
	{238, 238, TEX_CONCRETE, TEX_WALL},
	{239, 239, TEX_STEEL, TEX_GRATE},
	{240, 243, TEX_TECH, 0},
	{244, 244, TEX_CONCRETE, TEX_WALL},
	{245, 245, TEX_GRATE, TEX_STEEL},
	{246, 246, TEX_GRATE, 0},
	{247, 249, TEX_GRATE, TEX_STEEL},
	{250, 253, TEX_LIGHT, 0},
	{254, 255, TEX_GRATE, 0},
	{256, 260, TEX_CONCRETE, TEX_WALL},
	{261, 262, TEX_CEILING, TEX_STEEL},
	{263, 263, TEX_FLOOR, TEX_STEEL},
	{264, 269, TEX_LIGHT, 0},
	{270, 277, TEX_FLOOR, 0},
	{278, 289, TEX_LIGHT, 0},
	{290, 291, TEX_FLOOR, 0},
	{292, 294, TEX_LIGHT, 0},
	{295, 296, TEX_GRATE, TEX_TECH},
	{297, 297, TEX_GRATE, 0},
	{298, 298, TEX_SIGN, 0},
	{299, 300, TEX_TECH, TEX_SIGN},
	{301, 301, TEX_ENERGY, 0},
	{302, 308, TEX_SIGN, TEX_LABEL},
	{309, 309, TEX_GRATE, 0},
	{310, 312, TEX_SIGN, TEX_LABEL},
	{313, 314, TEX_SIGN, TEX_STRIPES},
	{315, 315, TEX_TECH, 0},
	{316, 316, TEX_RED_ROCK, TEX_TECH},
	{317, 317, TEX_SIGN, TEX_LABEL},
	{318, 318, TEX_SIGN, TEX_STRIPES},
	{319, 321, TEX_SIGN, TEX_LABEL},
	{322, 322, TEX_ENERGY, 0},
	{323, 323, TEX_SIGN, TEX_LABEL},
	{324, 324, TEX_GRATE, 0},
	{325, 325, TEX_FAN, 0},
	{326, 326, TEX_TECH, TEX_SIGN},
	{327, 327, TEX_SIGN, TEX_LIGHT | TEX_MOVING},
	{328, 328, TEX_ENERGY, TEX_LIGHT},
	{329, 329, TEX_FAN, 0},
	{330, 331, TEX_SIGN, TEX_LIGHT | TEX_MOVING},
	{332, 332, TEX_GREEN_ROCK, TEX_TECH},
	{333, 333, TEX_RED_ROCK, TEX_TECH},
	{334, 337, TEX_CONCRETE, TEX_TECH},
	{338, 339, TEX_TECH, 0},
	{340, 340, TEX_TARMAC, 0},
	{341, 354, TEX_SIGN, TEX_LIGHT | TEX_MONITOR},
	{355, 356, TEX_LAVA, TEX_LIGHT},
	{357, 370, TEX_SIGN, TEX_LIGHT | TEX_MONITOR},
	{371, 577, TEX_DOOR, 0}
};

//------------------------------------------------------------------------

static tTexFilterInfo texFiltersD2 [TEXFILTER_SIZE_D2] = {
	{0, 0, TEX_GRAY_ROCK, TEX_CONCRETE},
	{1, 5, TEX_GRAY_ROCK, 0},
	{6, 6, TEX_BLUE_ROCK, 0},
	{7, 7, TEX_RED_ROCK, 0},
	{8, 14, TEX_GRAY_ROCK, 0},
	{15, 15, TEX_BROWN_ROCK, 0},
	{16, 16, TEX_RED_ROCK, TEX_STONES},
	{17, 21, TEX_RED_ROCK, 0},
	{22, 23, TEX_GRAY_ROCK, 0},
	{24, 24, TEX_RED_ROCK, 0},
	{25, 25, TEX_RED_ROCK, TEX_LAVA},
	{26, 27, TEX_RED_ROCK, 0},
	{28, 28, TEX_GRAY_ROCK, 0},
	{29, 31, TEX_BROWN_ROCK, 0},
	{32, 32, TEX_RED_ROCK, 0},
	{33, 33, TEX_BROWN_ROCK, 0},
	{34, 34, TEX_GRAY_ROCK, TEX_STONES},
	{35, 35, TEX_ICE, 0},
	{36, 36, TEX_BROWN_ROCK, TEX_STONES},
	{37, 37, TEX_GRAY_ROCK, 0},
	{38, 42, TEX_BROWN_ROCK, 0},
	{43, 43, TEX_GRAY_ROCK, 0},
	{44, 44, TEX_RED_ROCK, 0},
	{45, 45, TEX_TARMAC, 0},
	{46, 49, TEX_GREEN_ROCK, 0},
	{50, 51, TEX_RED_ROCK, 0},
	{52, 53, TEX_BLUE_ROCK, 0},
	{54, 54, TEX_BROWN_ROCK, 0},
	{55, 55, TEX_BLUE_ROCK, TEX_ICE},
	{56, 58, TEX_BROWN_ROCK, 0},
	{59, 59, TEX_BLUE_ROCK, 0},
	{60, 61, TEX_GRAY_ROCK, 0},
	{62, 63, TEX_BLUE_ROCK, 0},
	{64, 64, TEX_BROWN_ROCK, 0},
	{65, 65, TEX_BLUE_ROCK, 0},
	{66, 66, TEX_BROWN_ROCK, 0},
	{67, 71, TEX_RED_ROCK, 0},
	{72, 72, TEX_BROWN_ROCK, 0},
	{73, 73, TEX_GREEN_ROCK, 0},
	{74, 74, TEX_BROWN_ROCK, 0},
	{75, 75, TEX_GRAY_ROCK, 0},
	{76, 80, TEX_BROWN_ROCK, 0},
	{81, 81, TEX_GREEN_ROCK, TEX_ICE},
	{82, 82, TEX_GREEN_ROCK, 0},
	{83, 83, TEX_GREEN_ROCK, TEX_ICE},
	{84, 84, TEX_GREEN_ROCK, 0},
	{85, 85, TEX_GREEN_ROCK, TEX_ICE},
	{85, 85, TEX_GREEN_ROCK | TEX_BLUE_ROCK, TEX_ICE},
	{86, 91, TEX_GREEN_ROCK, TEX_GRASS},
	{92, 93, TEX_BROWN_ROCK, 0},
	{94, 95, TEX_CONCRETE, 0},
	{96, 97, TEX_BROWN_ROCK, 0},
	{98, 98, TEX_GREEN_ROCK, TEX_GRASS},
	{99, 100, TEX_BROWN_ROCK, TEX_RED_ROCK},
	{101, 101, TEX_BROWN_ROCK | TEX_BLUE_ROCK, TEX_ICE},
	{102, 102, TEX_BLUE_ROCK, 0},
	{103, 103, TEX_BROWN_ROCK, 0},
	{104, 105, TEX_RED_ROCK, TEX_LAVA},
	{106, 106, TEX_RED_ROCK, 0},
	{107, 111, TEX_BROWN_ROCK, 0},
	{112, 114, TEX_RED_ROCK, 0},
	{115, 116, TEX_RED_ROCK, TEX_LAVA},
	{117, 117, TEX_BLUE_ROCK, TEX_ICE},
	{118, 118, TEX_GREEN_ROCK, 0},
	{119, 119, TEX_BLUE_ROCK, 0},
	{120, 121, TEX_BROWN_ROCK, 0},
	{122, 123, TEX_BROWN_ROCK | TEX_RED_ROCK, 0},
	{124, 125, TEX_GREEN_ROCK, TEX_GRASS},
	{126, 127, TEX_RED_ROCK, 0},
	{128, 138, TEX_ICE, 0},
	{139, 139, TEX_ICE, 0},
	{140, 142, TEX_BROWN_ROCK, 0},
	{143, 145, TEX_SAND, 0},
	{146, 147, TEX_RED_ROCK, 0},
	{148, 148, TEX_BROWN_ROCK, 0},
	{149, 151, TEX_BROWN_ROCK, TEX_SAND},
	{152, 152, TEX_RED_ROCK, 0},
	{153, 154, TEX_BROWN_ROCK, TEX_SAND},
	{155, 168, TEX_RED_ROCK, 0},
	{169, 171, TEX_GREEN_ROCK, TEX_GRASS},
	{172, 172, TEX_BLUE_ROCK, TEX_ICE},
	{173, 176, TEX_ICE, 0},
	{177, 177, TEX_BROWN_ROCK},
	{178, 183, TEX_ICE},
	{184, 184, TEX_GREEN_ROCK | TEX_GRASS},
	{185, 190, TEX_ICE, 0},
	{191, 191, TEX_CONCRETE, 0},
	{192, 193, TEX_ICE, 0},
	{194, 194, TEX_GREEN_ROCK, TEX_ICE},
	{195, 195, TEX_ICE, 0},
	{196, 196, TEX_GREEN_ROCK, TEX_ICE},
	{197, 197, TEX_BROWN_ROCK, 0},
	{198, 198, TEX_GRAY_ROCK, 0},
	{199, 199, TEX_BROWN_ROCK, 0},
	{200, 201, TEX_STEEL, 0},
	{202, 217, TEX_CONCRETE, TEX_WALL},
	{204, 204, TEX_CONCRETE, TEX_SIGN | TEX_WALL},
	{205, 206, TEX_CONCRETE, TEX_WALL},
	{207, 208, TEX_CONCRETE, TEX_SIGN | TEX_WALL},
	{209, 217, TEX_CONCRETE, TEX_WALL},
	{218, 222, TEX_STEEL, 0},
	{223, 225, TEX_CONCRETE, TEX_WALL},
	{226, 226, TEX_STEEL, 0},
	{227, 232, TEX_CONCRETE, TEX_WALL},
	{233, 233, TEX_CONCRETE, TEX_SIGN | TEX_WALL},
	{234, 234, TEX_CONCRETE, TEX_WALL},
	{235, 237, TEX_CONCRETE, TEX_LIGHT | TEX_WALL},
	{238, 240, TEX_CONCRETE, TEX_WALL},
	{241, 241, TEX_CONCRETE, TEX_GRATE | TEX_WALL},
	{242, 242, TEX_CONCRETE, TEX_WALL},
	{243, 244, TEX_CONCRETE, TEX_LIGHT | TEX_WALL},
	{245, 246, TEX_TECH, 0},
	{247, 248, TEX_GRATE, TEX_STEEL},
	{249, 252, TEX_CONCRETE, TEX_WALL},
	{253, 255, TEX_CONCRETE, TEX_GRATE | TEX_WALL},
	{256, 256, TEX_CONCRETE, TEX_WALL},
	{257, 257, TEX_GRATE, TEX_STEEL},
	{258, 258, TEX_CONCRETE, TEX_WALL},
	{259, 262, TEX_GRATE, TEX_STEEL},
	{263, 264, TEX_GRATE, 0},
	{265, 265, TEX_CONCRETE, 0},
	{266, 266, TEX_FLOOR, 0},
	{267, 269, TEX_GRATE, 0},
	{270, 272, TEX_TECH, 0},
	{273, 274, TEX_STEEL, TEX_CEILING},
	{275, 279, TEX_LIGHT, 0},
	{280, 287, TEX_FLOOR, 0},
	{288, 302, TEX_LIGHT, 0},
	{303, 304, TEX_FLOOR, 0},
	{305, 307, TEX_LIGHT, 0},
	{308, 310, TEX_GRATE, 0},
	{311, 312, TEX_SIGN, TEX_TECH | TEX_STRIPES},
	{313, 313, TEX_ENERGY, 0},
	{314, 320, TEX_SIGN, TEX_LABEL},
	{321, 321, TEX_GRATE, 0},
	{322, 324, TEX_SIGN, TEX_LABEL},
	{325, 326, TEX_SIGN, TEX_STRIPES},
	{327, 327, TEX_RED_ROCK, TEX_TECH},
	{328, 328, TEX_SIGN, TEX_LABEL},
	{329, 329, TEX_SIGN, TEX_STRIPES},
	{330, 332, TEX_SIGN, TEX_LABEL},
	{333, 333, TEX_ENERGY, 0},
	{334, 334, TEX_SIGN, TEX_LABEL},
	{335, 335, TEX_GRATE, 0},
	{336, 336, TEX_FAN, 0},
	{337, 337, TEX_SIGN, TEX_TECH | TEX_STRIPES},
	{338, 339, TEX_SIGN, TEX_STRIPES},
	{340, 347, TEX_LIGHT, 0},
	{348, 349, TEX_LIGHT, TEX_STEEL | TEX_MOVING},
	{350, 350, TEX_GRATE, TEX_STEEL | TEX_MOVING},
	{351, 351, TEX_SIGN, TEX_LABEL},
	{352, 352, TEX_SIGN, TEX_LIGHT | TEX_MOVING},
	{353, 353, TEX_ENERGY, TEX_LIGHT},
	{354, 354, TEX_FAN, 0},
	{355, 355, TEX_GREEN_ROCK, TEX_TECH},
	{356, 359, TEX_CONCRETE , TEX_TECH | TEX_WALL | TEX_LIGHT},
	{360, 361, TEX_LIGHT, TEX_TECH},
	{362, 362, TEX_TARMAC, 0},
	{363, 377, TEX_SIGN, TEX_LIGHT | TEX_MONITOR},
	{378, 378, TEX_LAVA, 0},
	{379, 398, TEX_SIGN, TEX_LIGHT},
	{399, 403, TEX_WATER, 0},
	{404, 409, TEX_LAVA, 0},
	{410, 412, TEX_LIGHT, 0},
	{413, 419, TEX_SWITCH, 0},
	{420, 420, TEX_FORCEFIELD, TEX_LIGHT},
	{423, 424, TEX_SIGN, TEX_LIGHT | TEX_STRIPES},
	{425, 425, TEX_SIGN, TEX_LIGHT},
	{426, 426, TEX_LIGHT, TEX_TECH},
	{427, 429, TEX_LIGHT, 0},
	{430, 431, TEX_SIGN, TEX_LIGHT},
	{432, 432, TEX_FORCEFIELD, TEX_LIGHT},
	{433, 434, TEX_LIGHT, TEX_TECH},
	{435, 901, TEX_DOOR, 0}
	};

//------------------------------------------------------------------------

inline void CTextureFilter::SetFilter (void) 
{ 
m_filter = TEXTURE_FILTERS; 
m_nSize = TEXFILTER_SIZE;
}

//------------------------------------------------------------------------

short CTextureFilter::FilterIndex (short nTexture)
{
	short	m, l = 0, r = m_nSize - 1;

do {
	m = (l + r) / 2;
	if (nTexture < m_filter [m].m_nMin)
		r = m - 1;
	else if (nTexture > m_filter [m].m_nMax)
		l = m + 1;
	else
		return m;
	}
while (l <= r);
return -1;
}

//------------------------------------------------------------------------

uint CTextureFilter::Filter (short nTexture)
{
	short	m = FilterIndex (nTexture);

return (m < 0) ? 0 : m_filter [m].m_nFilter1;
}

//------------------------------------------------------------------------

int CTextureFilter::CompareFilters (int nTexture, int mTexture, uint mf, uint mf2)
{
	short	n = FilterIndex (nTexture);
	uint	nf = m_filter [n].m_nFilter1,
			nf2 = m_filter [n].m_nFilter2;

//CBRK (((nf == TEX_DOOR) && (mf == TEX_SAND)) || ((mf == TEX_DOOR) && (nf == TEX_SAND)));
if (nf < mf)
	return -1;
else if (nf > mf)
	return 1;
else if (nf2 < mf2)
	return -1;
else if (nf2 > mf2)
	return 1;
else
	return (nTexture < mTexture) ? -1 : (nTexture > mTexture) ? 1 : 0;
}

//------------------------------------------------------------------------

void CTextureFilter::SortMap (short left, short right)
{
	short		mTexture = m_mapViewToTex [(left + right) / 2];
	short		m = FilterIndex (mTexture);
	uint		mf, mf2;
	short		h, l = left, r = right;

mf = m_filter [m].m_nFilter1;
mf2 = m_filter [m].m_nFilter2;
do {
	while (CompareFilters (m_mapViewToTex [l], mTexture, mf, mf2) < 0)
		l++;
	while (CompareFilters (m_mapViewToTex [r], mTexture, mf, mf2) > 0)
		r--;
	if (l <= r) {
		if (l < r) {
			h = m_mapViewToTex [l];
			m_mapViewToTex [l] = m_mapViewToTex [r];
			m_mapViewToTex [r] = h;
			}
		l++;
		r--;
		}
	}
while (l < r);
if (l < right)
	SortMap (l, right);
if (left < r)
	SortMap (left, r);
}

//------------------------------------------------------------------------

void CTextureFilter::CreateMap (void)
{
SortMap (0, m_nTextures [1] - 1);
for (int i = 0; i < m_nTextures [1]; i++)
	m_mapTexToView [m_mapViewToTex [i]] = i;
}

//------------------------------------------------------------------------

void CTextureFilter::Setup (void) 
{
if (!textureManager.Available ())
	return;

  int nTextures, nFrames = 0;

  nTextures = textureManager.MaxTextures ();

  // calculate total number of textures
m_nTextures [1] = 0;
for (int i = 0; i < nTextures; i++) {
	const CTexture* pTexture = textureManager.Textures (i);
	if (pTexture->IsAssignableFrame ())
	//if (textureManager.Texture (i)->IsAssignableFrame ())
		++nFrames;
	else
		m_mapViewToTex [m_nTextures [1]++] = i;
	}

// allocate memory for texture list
SetFilter ();
CreateMap ();
}

//------------------------------------------------------------------------
// TextureIndex()
//
// looks up texture index number for nBaseTex
// returns 0 to m_nTextures-1 on success
// returns -1 on failure
//------------------------------------------------------------------------

short CTextureFilter::TextureIndex (short nTexture) 
{
return ((nTexture < 0) || (nTexture >= sizeof (m_mapTexToView) / sizeof (m_mapTexToView [0]))) ? 0 : m_mapTexToView [nTexture];
}

//------------------------------------------------------------------------
// FilterTexture()
//
// Determines which textures to display based on which have been used
//------------------------------------------------------------------------

void CTextureFilter::Process (ubyte* filterP, BOOL bShowAll) 
{
SetFilter ();
if (bShowAll) {
	if (m_nFilter == 0xFFFFFFFF)
		memset (filterP, 0xFF, (MAX_TEXTURES_D2 + 7) / 8);
	else {
		memset (filterP, 0, (MAX_TEXTURES_D2 + 7) / 8);
		m_nTextures [0] = 0;
		int i, f = m_nFilter & ~TEX_MOVING;
		for (i = 0; i < m_nTextures [1]; i++) {
			int t = m_mapViewToTex [i];
			int j = FilterIndex (t);
			if ((m_filter [j].m_nFilter1 | m_filter [j].m_nFilter2) & f) {
				SETBIT (filterP, i);
				m_nTextures [0]++;
				}
			}
		}
	}
else {
	ushort nSegment,nSide;
	CSegment *pSegment;

	memset (filterP, 0, (MAX_TEXTURES_D2 + 7) / 8);
	m_nTextures [0] = 0;
	for (nSegment = 0, pSegment = segmentManager.Segment (0); nSegment < segmentManager.Count (); nSegment++, pSegment++)
      for (nSide = 0; nSide < 6; nSide++) {
			ushort nWall = pSegment->m_sides [nSide].m_info.nWall;
			if ((pSegment->ChildId (nSide) == -1) ||
				 ((nWall != NO_WALL) && wallManager.Wall (nWall)->Info ().type != WALL_OPEN)) {
				int t = pSegment->m_sides [nSide].BaseTex ();
				int i = TextureIndex (t);
				int j = FilterIndex (t);
				if ((i >= 0) && !GETBIT (filterP, i) && 
					 ((m_filter [j].m_nFilter1 | m_filter [j].m_nFilter2) & m_nFilter)) {
					SETBIT (filterP, i);
					m_nTextures [0]++;
					}
				t = pSegment->m_sides [nSide].OvlTex (0);
				i = TextureIndex (t);
				j = FilterIndex (t);
				if ((t > 0) && !GETBIT (filterP, i)) {
					SETBIT (filterP, i);
					m_nTextures [0]++;
					}
				}
			}
	}
}

//------------------------------------------------------------------------
