// dlcView.cpp: implementation of the CMineView class
//

#include "stdafx.h"
#include "winuser.h"
#include "dle-xp.h"

#include "dlcDoc.h"
#include "mineview.h"

#include "PaletteManager.h"
#include "TextureManager.h"
#include "global.h"
#include "FileManager.h"

#include <math.h>
#include <time.h>

short nDbgSeg = -1, nDbgSide = -1;
int nDbgVertex = -1;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CRenderer::Reset (void)
{
Center ().Clear ();
Translation ().Clear ();
Scale ().Set (1.0, 1.0, 1.0);
Rotation ().Set (M_PI / 4.0, M_PI / 4.0, 0.0);
}

//------------------------------------------------------------------------------

bool CRenderer::InitViewDimensions (void)
{
	CRect	rc;

m_pParent->GetClientRect (rc);
int width = (rc.Width () + 3) & ~3; // long word align
int height = rc.Height ();
if ((ViewWidth () == width) && (ViewHeight () == height))
	return false;
ViewWidth () = width;
ViewHeight () = height;
return true;
}

//------------------------------------------------------------------------------

void CRenderer::ComputeViewLimits (CRect* pRC)
{
	CRect		rc (LONG_MAX, LONG_MAX, -LONG_MAX, -LONG_MAX);

MinViewPoint ().m_screen.z = LONG_MAX;
MaxViewPoint ().m_screen.z = -LONG_MAX;
for (int h, i = 0, j = vertexManager.Count (); i < j; i++) {
	CVertex& v = vertexManager [i];
	if (v.Status () == 255)
		continue;
	h = v.m_screen.x;
	if (rc.left > h) {
		rc.left = h;
		MinViewPoint ().v.x = h;
		}
	if (rc.right < h) {
		rc.right = h;
		MaxViewPoint ().v.x = h;
		}
	h = v.m_screen.y;
	if (rc.top > h) {
		rc.top = h;
		MinViewPoint ().v.y = h;
		}
	if (rc.bottom < h) {
		rc.bottom = h;
		MaxViewPoint ().v.y = h;
		}
	h = v.m_screen.z;
	if (MinViewPoint ().m_screen.z > h) {
		MinViewPoint ().v.z = h;
		MinViewPoint ().m_screen.z = h;
		}
	if (MaxViewPoint ().m_screen.z < h) {
		MaxViewPoint ().v.z = h;
		MaxViewPoint ().m_screen.z = h;
		}
	}
MinViewPoint ().m_screen.x = rc.left;
MaxViewPoint ().m_screen.x = rc.right;
MinViewPoint ().m_screen.y = rc.bottom;
MaxViewPoint ().m_screen.y = rc.top;

if (pRC)
	*pRC = rc;
}

//------------------------------------------------------------------------------

void CRenderer::ComputeBrightness (CFaceListEntry& face, ushort brightness [4], int bVariableLights)
{
#ifdef _DEBUG
if ((face.m_nSegment == nDbgSeg) && ((nDbgSide < 0) || (face.m_nSide == nDbgSide)))
	nDbgSeg = nDbgSeg;
#endif

	CSegment*	pSegment = segmentManager.Segment (face.m_nSegment);
	CSide*		pSide = pSegment->Side (face.m_nSide);
	short			nVertices = pSide->VertexCount ();
	int			b [4];

for (int i = 0; i < nVertices; i++) {
	b [i] = (int) pSide->m_info.uvls [i].l;
	// clip brightness
	//if (b [i] & 0x8000) {
	//	b [i] = 0x7fff;
	//	}
	}

#ifdef _DEBUG
if ((face.m_nSegment == nDbgSeg) && ((nDbgSide < 0) || (face.m_nSide == nDbgSide)))
	nDbgSeg = nDbgSeg;
#endif

if (bVariableLights /*&& (lightManager.LightIsOn (face) < 1)*/) {
	// check each variable light whether it affects side face
	// search delta light index to see if current side has light
	int indexCount = lightManager.DeltaIndexCount ();
//#pragma omp parallel for if (indexCount > 15)
	for (int i = 0; i < indexCount; i++) {
		CLightDeltaIndex* pIndex = lightManager.LightDeltaIndex (i);
#ifdef _DEBUG
		if ((pIndex->m_nSegment == nDbgSeg) && ((nDbgSide < 0) || (pIndex->m_nSide == nDbgSide)))
			nDbgSeg = nDbgSeg;
#endif
		if (lightManager.LightIsOn (*pIndex))
			continue; // light is on or there's no variable light for this delta light index (e.g. only blastable)
		if (*pIndex == face) {
			int lightDelta = (int) lightManager.Brightness (face);
			for (short k = 0; k < nVertices; k++) 
//#pragma omp atomic
				b [k] -= lightDelta;
			}
		else {
			CLightDeltaValue* pLightDelta = lightManager.LightDeltaValue (pIndex->m_info.index);
			ushort nSides = pIndex->m_info.count;
			for (ushort j = 0; j < nSides; j++, pLightDelta++) {
				if (*pLightDelta == face) {
					for (short k = 0; k < nVertices; k++) {
						//short lightDelta = ;
						//if (lightDelta >= 0x20)
						//	lightDelta = 0x7fff;
						//else
						//	lightDelta <<= 10;
//#pragma omp atomic
						b [k] -= pLightDelta->m_info.vertLight [k];
						}
					break;
					}
				}
			}
		}
	}

for (int i = 0; i < nVertices; i++) 
	brightness [i] = (b [i] < 0) ? 0 : (b [i] > 32767) ? 32767 : b [i];

#ifdef _DEBUG
if ((face.m_nSegment == nDbgSeg) && ((nDbgSide < 0) || (face.m_nSide == nDbgSide)))
	nDbgSeg = nDbgSeg;
#endif
}

//------------------------------------------------------------------------------
//eof