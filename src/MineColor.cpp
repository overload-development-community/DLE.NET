// Copyright (c) 1998 Bryan Aamot, Brainware

#include "stdafx.h"
#include "dle-xp-res.h"

#include < math.h>
#include "define.h"
#include "types.h"
#include "global.h"
#include "mine.h"
#include "matrix.h"
#include "cfile.h"
#include "textures.h"
#include "palette.h"
#include "dle-xp.h"
#include "robot.h"
#include "HogManager.h"
#include "light.h"

#ifdef ALLOCATE_tPolyModelS
#undef ALLOCATE_tPolyModelS
#endif
#define ALLOCATE_tPolyModelS 0

#define ENABLE_TEXT_DUMP 0

// ------------------------------------------------------------------------

int CColor::Read (CFileManager& fp, int version, bool bNewFormat)
{
m_info.index = fp.ReadSByte ();
if (bNewFormat) {
	m_info.color.r = double (fp.ReadInt32 ()) / double (0x7fffffff);
	m_info.color.g = double (fp.ReadInt32 ()) / double (0x7fffffff);
	m_info.color.b = double (fp.ReadInt32 ()) / double (0x7fffffff);
	}
else {
	m_info.color.r = fp.ReadDouble ();
	m_info.color.g = fp.ReadDouble ();
	m_info.color.b = fp.ReadDouble ();
	}
return 1;
}

// ------------------------------------------------------------------------

void CColor::Write (CFileManager& fp, int version, bool bFlag) 
{
fp.Write (m_info.index);
fp.WriteInt32 ((int) (m_info.color.r * 0x7fffffff + 0.5));
fp.WriteInt32 ((int) (m_info.color.g * 0x7fffffff + 0.5));
fp.WriteInt32 ((int) (m_info.color.b * 0x7fffffff + 0.5));
}

// ------------------------------------------------------------------------

void CMine::LoadColors (CColor *pc, int nColors, int nFirstVersion, int nNewVersion, CFileManager& fp)
{
	bool bNewFormat = LevelVersion () >= nNewVersion;

if (LevelVersion () > nFirstVersion) { 
	for (; nColors; nColors--, pc++)
		pc->Read (fp, 0, bNewFormat);
	}
}

// ------------------------------------------------------------------------

void CMine::SaveColors (CColor *pc, int nColors, CFileManager& fp)
{
for (; nColors; nColors--, pc++)
	pc->Write (fp);
}

//--------------------------------------------------------------------------

int CMine::ReadColorMap (CFileManager& fp)
{
LoadColors (TexColors (), MAX_TEXTURES_D2, 0, 0, fp);
return 0;
}

//--------------------------------------------------------------------------

int CMine::WriteColorMap (CFileManager& fp)
{
SaveColors (TexColors (), MAX_TEXTURES_D2, fp);
return 0;
}

//--------------------------------------------------------------------------
//eof mine.cpp