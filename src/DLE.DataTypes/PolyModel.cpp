// Copyright (c) 1997 Bryan Aamot
#include "stdafx.h"
#include "PolyModel.h"

//------------------------------------------------------------------------------
// CONSTANTS
//------------------------------------------------------------------------------

#define OP_EOF        0	/* eof                                 */
#define OP_DEFPOINTS  1	/* defpoints                (not used) */
#define OP_FLATPOLY   2	/* flat-shaded polygon                 */
#define OP_TMAPPOLY   3	/* texture-mapped polygon              */
#define OP_SORTNORM   4	/* sort by normal                      */
#define OP_RODBM      5	/* rod bitmap               (not used) */
#define OP_SUBCALL    6	/* call a subobject                    */
#define OP_DEFP_START 7	/* defpoints with start                */
#define OP_GLOW       8	/* m_info.glow value for next poly            */

#define MAX_INTERP_COLORS 100
#define MAX_POINTS_PER_POLY 25

//------------------------------------------------------------------------------
// MACROS
//------------------------------------------------------------------------------

#define W(p)   (*((ushort *) (p)))
#define WP(p)  ((ushort *) (p))
#define VP(p)  ((CFixVector*) (p))

#define calcNormal(a, b)
#define glNormal3fv(a)
#define glColor3ub(a, b, c)
#define glBegin(a)
#define glVertex3i(x, y, z)
#define glEnd()
#define glTexCoord2fv(a)

#define UINTW int

//------------------------------------------------------------------------------

#define PAL2RGBA(_c)	(((_c) >= 31) ? 1.0 : double (_c) / 31.0)

typedef struct tIntUVL {
	int u, v, l;
} tIntUVL;

//------------------------------------------------------------------------------

void CPolyModel::Read (IFileManager* fp, bool bRenderData) 
{
if (bRenderData) {
	Release ();
	if ((m_info.renderData = new ubyte [m_info.dataSize]))
		fp->Read (m_info.renderData, m_info.dataSize, 1);
	}
else {
	m_info.nModels = fp->ReadInt32 ();
	m_info.dataSize = fp->ReadInt32 ();
	fp->ReadInt32 ();
	Release ();
	for (int i = 0; i < MAX_SUBMODELS; i++)
		m_info.subModels [i].ptr = fp->ReadInt32 ();
	for (int i = 0; i < MAX_SUBMODELS; i++)
		fp->Read (m_info.subModels [i].offset);
	for (int i = 0; i < MAX_SUBMODELS; i++)
		fp->Read (m_info.subModels [i].norm);
	for (int i = 0; i < MAX_SUBMODELS; i++)
		fp->Read (m_info.subModels [i].pnt);
	for (int i = 0; i < MAX_SUBMODELS; i++)
		m_info.subModels [i].rad = fp->ReadInt32 ();
	for (int i = 0; i < MAX_SUBMODELS; i++)
		m_info.subModels [i].parent = (ubyte) fp->ReadSByte ();
	for (int i = 0; i < MAX_SUBMODELS; i++)
		fp->Read (m_info.subModels [i].vMin);
	for (int i = 0; i < MAX_SUBMODELS; i++)
		fp->Read (m_info.subModels [i].vMax);
	fp->Read (m_info.vMin);
	fp->Read (m_info.vMax);
	m_info.rad = fp->ReadInt32 ();
	m_info.textureCount = fp->ReadUByte ();
	m_info.firstTexture = fp->ReadUInt16 ();
	m_info.simplerModel = fp->ReadUByte ();
	}
}

//------------------------------------------------------------------------------
//eof polymodel.cpp

