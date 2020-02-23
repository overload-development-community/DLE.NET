// Copyright (c) 1997 Bryan Aamot
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <string.h>

#include "mine.h"
#include "dle-xp.h"
#include "ModelManager.h"
#include "glew.h"

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

void CPolyModel::Render (ubyte *p, int nStage) 
{
while (W (p) != OP_EOF) {
	switch (W (p)) {
		// Point Definitions with Start Offset:
		// 2  ushort      modelManager.m_nPolyPoints       number of points
		// 4  ushort      start_point    starting point
		// 6  ushort      unknown
		// 8  CFixVector  pts[modelManager.m_nPolyPoints]  x, y, z data
		case OP_DEFP_START: {
			m_pt0 = W (p+4);
			if (nStage <= 0) {
				modelManager.m_nPolyPoints = W (p+2);
				modelManager.m_data.nPoints += modelManager.m_nPolyPoints;
				for (m_pt = 0; m_pt < modelManager.m_nPolyPoints; m_pt++) 
					modelManager.m_data.points [m_pt+m_pt0] = modelManager.m_offset + CDoubleVector (VP (p+8)[m_pt]);
				modelManager.SetPoints (m_pt0, m_pt0 + modelManager.m_nPolyPoints);
				}
			p += W (p+2)*sizeof (CFixVector) + 8;
			break;
			}
		// Flat Shaded Polygon:
		// 2  ushort     nVerts
		// 4  CFixVector vector1
		// 16 CFixVector vector2
		// 28 ushort     color
		// 30 ushort     verts[nVerts]
		case OP_FLATPOLY: {
			modelManager.m_face = &modelManager.m_data.polys [modelManager.m_data.nPolys];
			modelManager.m_face->nVerts = W (p+2);
			if (nStage >= 0) {
				modelManager.m_face->offset = *VP (p+4);
				modelManager.m_face->normal = *VP (p+16);
				ushort c = W (p+28);
				modelManager.m_face->color.r = PAL2RGBA ((c >> 10) & 31);
				modelManager.m_face->color.g = PAL2RGBA ((c >> 5) & 31);
				modelManager.m_face->color.b = PAL2RGBA (c & 31);
				modelManager.m_face->nTexture = 0;
				p += 30;
				for (m_pt = 0; m_pt < modelManager.m_face->nVerts; m_pt++) 
					modelManager.m_face->index [m_pt] = WP (p)[m_pt];
				modelManager.m_face->Draw ();
				}
			else
				p += 30;
			p += (modelManager.m_face->nVerts | 1) * 2;
			break;
			}
		// Texture Mapped Polygon:
		// 2  ushort     nVerts
		// 4  CFixVector vector1
		// 16 CFixVector vector2
		// 28 ushort     nBaseTex
		// 30 ushort     verts[nVerts]
		// -- UVL        uvls[nVerts]
		case OP_TMAPPOLY: {
			modelManager.m_face = &modelManager.m_data.polys [modelManager.m_data.nPolys];
			modelManager.m_face->nVerts = W (p+2);
			if (nStage >= 0) {
				modelManager.m_face->offset = *VP (p+4);
				modelManager.m_face->normal = *VP (p+16);
				modelManager.m_face->color.r = 
				modelManager.m_face->color.g = 
				modelManager.m_face->color.b = 1.0;
				modelManager.m_face->nTexture = -modelManager.m_textureIndex [m_bCustom][modelManager.m_modelTextureIndex [m_bCustom][m_info.firstTexture + W (p+28)]]; // texture index is based on 1!
				modelManager.m_face->nGlow = modelManager.m_nGlow;
				p += 30;
				tIntUVL* uvls = (tIntUVL*) (p + (modelManager.m_face->nVerts | 1) * 2);
				for (m_pt = 0; m_pt < modelManager.m_face->nVerts; m_pt++) {
					modelManager.m_face->texCoords [m_pt].u = X2D (uvls [m_pt].u);
					modelManager.m_face->texCoords [m_pt].v = -X2D (uvls [m_pt].v);
					modelManager.m_face->index [m_pt] = WP (p)[m_pt];
					}
				modelManager.m_face->Draw ();
				}
			else
				p += 30;
			p += (modelManager.m_face->nVerts | 1) * 2;
			p += modelManager.m_face->nVerts * 12;
			break;
			}
		// Sort by Normal
		// 2  ushort      unknown
		// 4  CFixVector  Front Model normal
		// 16 CFixVector  Back Model normal
		// 28 ushort      Front Model Offset
		// 30 ushort      Back Model Offset
		case OP_SORTNORM: {
			/* = W (p+2); */
			/* = W (p+4); */
			/* = W (p+16); */
			if (modelManager.CheckNormal (*VP (p+4), *VP (p+16))) {
			  Render (p + W (p+28), nStage);
			  Render (p + W (p+30), nStage);
				}
			else {
			  Render (p + W (p+30), nStage);
			  Render (p + W (p+28), nStage);
				}
			p += 32;
			break;
			}
		// Call a Sub Object
		// 2  ushort     n_anims
		// 4  CFixVector offset
		// 16 ushort     model offset
		case OP_SUBCALL: {
			modelManager.m_offset += *VP (p+4);
			Render (p + W (p+16), nStage);
			modelManager.m_offset -= *VP (p+4);
			p += 20;
			break;
			}
		// Glow Number for Next Poly
		// 2 UINTW  Glow_Value
		case OP_GLOW: {
			modelManager.m_nGlow = W (p+2);
			p += 4;
			break;
			}
		default: 
			assert (0);
		}
	}
return;
}

//------------------------------------------------------------------------------

void CPolyModel::Read (CFileManager* fp, bool bRenderData) 
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

