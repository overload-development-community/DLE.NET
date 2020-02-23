#include <math.h>
#include <stdlib.h>

#include "mine.h"
#include "dle-xp.h"
#include "rendermodel.h"
#include "buildmodel.h"

using namespace RenderModel;

//------------------------------------------------------------------------------

void CModel::CountASEModelItems (ASE::CModel& aseModel)
{
m_nFaces = aseModel.m_nFaces;
m_nSubModels = aseModel.m_nSubModels;
m_nVerts = aseModel.m_nVerts;
m_nFaceVerts = aseModel.m_nFaces * 3;
}

//------------------------------------------------------------------------------

void CModel::GetASEModelItems (int nModel, ASE::CModel& aseModel, float fScale)
{
	ASE::CSubModel*	pAseSubModel;
	ASE::CFace*			pAseFace;
	CSubModel*			pRenderSubModel;
	CFace*				pRenderFace = m_faces.Buffer ();
	CVertex*				pRenderVertex = m_faceVerts.Buffer ();
	int					h, i, nFaces, iFace, nVerts = 0, nIndex = 0;
	bool					bTextured;

for (pAseSubModel = aseModel.m_subModels; pAseSubModel; pAseSubModel = pAseSubModel->m_next) {
	pRenderSubModel = m_subModels + pAseSubModel->m_nSubModel;
#if DBG
	strcpy (pRenderSubModel->m_szName, pAseSubModel->m_szName);
#endif
	pRenderSubModel->m_nSubModel = pAseSubModel->m_nSubModel;
	pRenderSubModel->m_nParent = pAseSubModel->m_nParent;
	pRenderSubModel->m_faces = pRenderFace;
	pRenderSubModel->m_nFaces = nFaces = pAseSubModel->m_nFaces;
	pRenderSubModel->m_bGlow = pAseSubModel->m_bGlow;
	pRenderSubModel->m_bFlare = pAseSubModel->m_bFlare;
	pRenderSubModel->m_bBillboard = pAseSubModel->m_bBillboard;
	pRenderSubModel->m_bRender = pAseSubModel->m_bRender;
	pRenderSubModel->m_bThruster = pAseSubModel->m_bThruster;
	pRenderSubModel->m_bWeapon = pAseSubModel->m_bWeapon;
	pRenderSubModel->m_bHeadlight = pAseSubModel->m_bHeadlight;
	pRenderSubModel->m_bBombMount = pAseSubModel->m_bBombMount;
	pRenderSubModel->m_nGun = pAseSubModel->m_nGun;
	pRenderSubModel->m_nBomb = pAseSubModel->m_nBomb;
	pRenderSubModel->m_nMissile = pAseSubModel->m_nMissile;
	pRenderSubModel->m_nType = pAseSubModel->m_nType;
	pRenderSubModel->m_nWeaponPos = pAseSubModel->m_nWeaponPos;
	pRenderSubModel->m_nGunPoint = pAseSubModel->m_nGunPoint;
	pRenderSubModel->m_bBullets = (pAseSubModel->m_nBullets > 0);
	pRenderSubModel->m_nIndex = nIndex;
	pRenderSubModel->m_iFrame = 0;
	pRenderSubModel->m_tFrame = 0;
	pRenderSubModel->m_nFrames = (pAseSubModel->m_bBarrel || pAseSubModel->m_bThruster) ? 32 : 0;
	pRenderSubModel->m_vOffset = pAseSubModel->m_vOffset;
	pRenderSubModel->InitMinMax ();
	for (pAseFace = pAseSubModel->m_faces.Buffer (), iFace = 0; iFace < nFaces; iFace++, pAseFace++, pRenderFace++) {
		pRenderFace->m_nIndex = nIndex;
		i = pAseSubModel->m_nTexture;
		CTexture& tex = aseModel.m_textures.Texture (i);
		bTextured = !tex.Flat ();
		pRenderFace->m_nTexture = bTextured ? i : -1;
		pRenderFace->m_nVerts = 3;
		pRenderFace->m_nId = iFace;
		for (i = 0; i < 3; i++, pRenderVertex++) {
			h = pAseFace->m_nVerts [i];
			if ((pRenderVertex->m_bTextured = bTextured))
				pRenderVertex->m_baseColor.r =
				pRenderVertex->m_baseColor.g =
				pRenderVertex->m_baseColor.b = 1.0f;
			else 
				tex.GetAverageColor (pRenderVertex->m_baseColor);
			pRenderVertex->m_baseColor.a = 1.0f;
			pRenderVertex->m_renderColor = pRenderVertex->m_baseColor;
			pRenderVertex->m_normal = pAseSubModel->m_vertices [h].m_normal;
			pRenderVertex->m_vertex = pAseSubModel->m_vertices [h].m_vertex * fScale;
			if (pAseSubModel->m_texCoord.Buffer ())
				pRenderVertex->m_texCoord = pAseSubModel->m_texCoord [pAseFace->m_nTexCoord [i]];
			h += nVerts;
			m_vertices [h] = pRenderVertex->m_vertex;
			m_vertNorms [h] = pRenderVertex->m_normal;
			pRenderVertex->m_nIndex = h;
			pRenderSubModel->SetMinMax (&pRenderVertex->m_vertex);
			nIndex++;
			}
#if 1
		if (pRenderSubModel->m_bThruster) {
			CFloatVector n = Normal ((pRenderVertex - 3)->m_vertex, (pRenderVertex - 2)->m_vertex, (pRenderVertex - 1)->m_vertex);
			pRenderFace->m_vNormal = n;
			}
		else
#endif
			pRenderFace->m_vNormal = pAseFace->m_vNormal;
		}
	nVerts += pAseSubModel->m_nVerts;
	}
}

//------------------------------------------------------------------------------

int CModel::BuildFromASE (ASE::CModel& aseModel)
{
if (aseModel.m_nModel < 0)
	return 0;
CountASEModelItems (aseModel);
if (!Create ()) 
	return 0;
GetASEModelItems (m_nModel = aseModel.m_nModel, aseModel, 1.0f); 
m_textures = aseModel.m_textures.Textures ();
m_nTextures = aseModel.m_textures.Count ();
memset (m_teamTextures, 0xFF, sizeof (m_teamTextures));
for (int i = 0; i < m_nTextures; i++) {
	int j = (int) aseModel.m_textures.Team (i);
	if (j)
		m_teamTextures [j - 1] = i;
	}
m_nType = 2;
Setup (1, 0);
return -1;
}

//------------------------------------------------------------------------------
//eof
