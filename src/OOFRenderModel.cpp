#include <math.h>
#include <stdlib.h>

#include "mine.h"
#include "dle-xp.h"
#include "OOFModel.h"
#include "RenderModel.h"

using namespace RenderModel;

//------------------------------------------------------------------------------

void CModel::CountOOFModelItems (OOF::CModel& oofModel)
{
	OOF::CSubModel*	pso;
	OOF::CFace*			pf;
	int					i, j;

i = oofModel.m_nSubModels;
m_nSubModels = i;
m_nFaces = 0;
m_nVerts = 0;
m_nFaceVerts = 0;
for (pso = oofModel.m_subModels.Buffer (); i; i--, pso++) {
	j = pso->m_faces.m_nFaces;
	m_nFaces += j;
	m_nVerts += pso->m_nVerts;
	for (pf = pso->m_faces.m_list.Buffer (); j; j--, pf++)
		m_nFaceVerts += pf->m_nVerts;
	}
}

//------------------------------------------------------------------------------

void CModel::GetOOFModelItems (int nModel, OOF::CModel& oofModel, float fScale)
{
	OOF::CSubModel*	pso;
	OOF::CFace*			pof;
	OOF::CFaceVert*	pfv;
	CSubModel*			psm;
	CFloatVector*		pvn = m_vertNorms.Buffer (), vNormal;
	CVertex*				pmv = m_faceVerts.Buffer ();
	CFace*				pmf = m_faces.Buffer ();
	int					h, i, j, n, nIndex = 0;

for (i = oofModel.m_nSubModels, pso = oofModel.m_subModels.Buffer (), psm = m_subModels.Buffer (); i; i--, pso++, psm++) {
	psm->m_nParent = pso->m_nParent;
	if (psm->m_nParent < 0)
		m_iSubModel = (short) (psm - m_subModels);
	psm->m_vOffset.v.x = F2X (pso->m_vOffset.v.x * fScale);
	psm->m_vOffset.v.y = F2X (pso->m_vOffset.v.y * fScale);
	psm->m_vOffset.v.z = F2X (pso->m_vOffset.v.z * fScale);
	psm->m_nAngles = 0;
	psm->m_nBomb = -1;
	psm->m_nMissile = -1;
	psm->m_nGun = -1;
	psm->m_nGunPoint = -1;
	psm->m_bBullets = 0;
	psm->m_bThruster = 0;
	psm->m_bGlow = 0;
	psm->m_bRender = 1;
	j = pso->m_faces.m_nFaces;
	psm->m_nIndex = nIndex;
	psm->m_nFaces = j;
	psm->m_faces = pmf;
	psm->InitMinMax ();
	for (pof = pso->m_faces.m_list.Buffer (); j; j--, pof++, pmf++) {
		pmf->m_nIndex = nIndex;
		pmf->m_bThruster = 0;
		pmf->m_bGlow = 0;
		n = pof->m_nVerts;
		pmf->m_nVerts = n;
		if (pof->m_bTextured)
			pmf->m_nTexture = pof->m_texProps.nTexId;
		else
			pmf->m_nTexture = -1;
		pfv = pof->m_vertices;
		h = pfv->m_nIndex;
		if (nModel > 200) {
			vNormal = Normal (pso->m_vertices [pfv [0].m_nIndex], pso->m_vertices [pfv [1].m_nIndex], pso->m_vertices [pfv [2].m_nIndex]);
			}
		else
			vNormal = pof->m_vNormal;
		for (; n; n--, pfv++, pmv++, pvn++) {
			h = pfv->m_nIndex;
			pmv->m_nIndex = h;
#if DBG
			if (h >= int (m_vertices.Length ()))
				continue;
#endif
			pmv->m_texCoord.u = pfv->m_fu;
			pmv->m_texCoord.v = pfv->m_fv;
			pmv->m_normal = vNormal;
			m_vertices [h] = pso->m_vertices [h];
			m_vertices [h] *= fScale;
			pmv->m_vertex = m_vertices [h];
			psm->SetMinMax (&pmv->m_vertex);
			*pvn = vNormal;
			if ((pmv->m_bTextured = pof->m_bTextured))
				pmv->m_baseColor.r =
				pmv->m_baseColor.g =
				pmv->m_baseColor.b = 1.0f;
			else {
				pmv->m_baseColor.r = (float) pof->m_texProps.color.r / 255.0f;
				pmv->m_baseColor.g = (float) pof->m_texProps.color.g / 255.0f;
				pmv->m_baseColor.b = (float) pof->m_texProps.color.b / 255.0f;
				}
			pmv->m_baseColor.a = 1.0f;
			nIndex++;
			}
		}
	}
}

//------------------------------------------------------------------------------

int CModel::BuildFromOOF (OOF::CModel& oofModel)
{
CountOOFModelItems (oofModel);
if (!Create ())
	return 0;
GetOOFModelItems (m_nModel = oofModel.m_nModel, oofModel, 1.0f);
m_textures = oofModel.m_textures.Textures ();
m_nTextures = oofModel.m_textures.Count ();
memset (m_teamTextures, 0xFF, sizeof (m_teamTextures));
m_nType = -1;
Setup (1, 1);
return -1;
}

//------------------------------------------------------------------------------
//eof
