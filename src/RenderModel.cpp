#include <math.h>
#include <stdlib.h>

#include "mine.h"
#include "dle-xp.h"
#include "RenderModel.h"
#include "glew.h"

//------------------------------------------------------------------------------

static int bCenterGuns [] = {0, 1, 1, 0, 0, 0, 1, 1, 0, 1};

int RenderModel::CSubModel::Filter (int nGunId, int nBombId, int nMissileId, int nMissiles)
{
if (!m_bRender)
	return 0;
if (m_bFlare)
	return 1;
if (m_nGunPoint >= 0)
	return 1;
if (m_bBullets)
	return 1;
if (m_bThruster && ((m_bThruster & (REAR_THRUSTER | FRONTAL_THRUSTER)) != (REAR_THRUSTER | FRONTAL_THRUSTER)))
	return 1;
if (m_bHeadlight)
	return 0;
if (m_bBombMount)
	return (nBombId == 0);

if (m_bWeapon) {
	int bLasers = 1;
	int bSuperLasers = 1;
	int bQuadLasers = 1;
	int bCenterGun = bCenterGuns [nGunId];
	int nWingtip = bQuadLasers ? bSuperLasers : 2; //gameOpts->render.ship.nWingtip;

	if (nWingtip == 0)
		nWingtip = bLasers && bSuperLasers && bQuadLasers;
	else if (nWingtip == 1)
		nWingtip = !bLasers || bSuperLasers;
	
	if (m_nGun == nGunId + 1) {
		if (bLasers) {
			if ((m_nWeaponPos > 2) && !bQuadLasers && (nWingtip != bSuperLasers))
				return 1;
			}
		}
	else if (m_nGun == LASER_INDEX + 1) {
		if (nWingtip)
			return 1;
		return !bCenterGun && (m_nWeaponPos < 3);
		}
	else if (m_nGun == SUPER_LASER_INDEX + 1) {
		if (nWingtip != 1)
			return 1;
		return !bCenterGun && (m_nWeaponPos < 3);
		}
	else if (!m_nGun) {
		if (bLasers && bQuadLasers)
			return 1;
		if (m_nType != nWingtip)
			return 1;
		return 0;
		}
	else 
		return 1;
	}
return 0;
}

//------------------------------------------------------------------------------

static inline int PlayerColor (int nObject)
{
return 1;
}

//------------------------------------------------------------------------------

static int nLevel = 0;

void RenderModel::CSubModel::Render (RenderModel::CModel& model, short nSubModel, int nGunId, int nBombId, int nMissileId, int nMissiles)
{
	RenderModel::CFace*	pFace;
	int						i;
	short						nTexture = -1;
	ushort					nFaceVerts, nVerts, nIndex;

if (Filter (nGunId, nBombId, nMissileId, nMissiles))
	return;

if ((0 != m_vOffset.v.x) || (0 != m_vOffset.v.y) || (0 != m_vOffset.v.z))
	glTranslatef (X2F (m_vOffset.v.x), X2F (m_vOffset.v.y), X2F (m_vOffset.v.z));

// render any dependent submodels
for (int i = 0; i < model.m_nSubModels; i++) {
	if (model.m_subModels [i].m_nParent == nSubModel)
		model.m_subModels [i].Render (model, i, nGunId, nBombId, nMissileId, nMissiles);
	}

if (m_bBillboard) {
	float modelView [16];
	glGetFloatv (GL_MODELVIEW_MATRIX, modelView);
	for (int i = 0; i < 3; i++) 
		 for (int j = 0; j < 3; j++)
			modelView [i * 4 + j] = (i == j) ? 1.0f : 0.0f;
	glLoadMatrixf (modelView);
	}

glDisable (GL_TEXTURE_2D);

for (i = m_nFaces, pFace = m_faces; i; ) {
	if (nTexture != pFace->m_nTexture) {
		if (0 > (nTexture = pFace->m_nTexture))
			glDisable (GL_TEXTURE_2D);
		else {
			glEnable (GL_TEXTURE_2D);
			if (!(*model.m_textures) [nTexture].GLBind (GL_TEXTURE0, GL_MODULATE))
				continue;
			}
		}
	nIndex = pFace->m_nIndex;
	if ((nFaceVerts = pFace->m_nVerts) > 4) {
		nVerts = nFaceVerts;
		pFace++;
		i--;
		}
	else {
		short nId = pFace->m_nId;
		nVerts = 0;
		do {
			nVerts += nFaceVerts;
			pFace++;
			i--;
			} while (i && (pFace->m_nId == nId));
		}
	glDrawRangeElements ((nFaceVerts == 3) ? GL_TRIANGLES : (nFaceVerts == 4) ? GL_QUADS : GL_TRIANGLE_FAN,
								0, model.m_nFaceVerts - 1, nVerts, GL_UNSIGNED_SHORT,
								VBO_OFFSET (nIndex * sizeof (short)));
	}

if ((0 != m_vOffset.v.x) || (0 != m_vOffset.v.y) || (0 != m_vOffset.v.z))
	glTranslatef (-X2F (m_vOffset.v.x), -X2F (m_vOffset.v.y), -X2F (m_vOffset.v.z));
}

//------------------------------------------------------------------------------

int RenderModel::CModel::Render (CViewMatrix* view, CGameObject* pObject, int nGunId, int nBombId, int nMissileId, int nMissiles)
{
#if 0
	return 0;
#else
if (m_nModel < 0)
	return 0;
glActiveTexture (GL_TEXTURE0);
glClientActiveTexture (GL_TEXTURE0);
glEnable (GL_TEXTURE_2D);
glEnableClientState (GL_COLOR_ARRAY);
glEnableClientState (GL_TEXTURE_COORD_ARRAY);
glEnableClientState (GL_NORMAL_ARRAY);
glEnableClientState (GL_VERTEX_ARRAY);
glBindBufferARB (GL_ARRAY_BUFFER_ARB, m_vboDataHandle);
glNormalPointer (GL_FLOAT, 0, VBO_OFFSET (m_nFaceVerts * sizeof (CFloatVector)));
glColorPointer (4, GL_FLOAT, 0, VBO_OFFSET (m_nFaceVerts * 2 * sizeof (CFloatVector)));
glTexCoordPointer (2, GL_DOUBLE, 0, VBO_OFFSET (m_nFaceVerts * (2 * sizeof (CFloatVector) + sizeof (rgbaColorf))));
glVertexPointer (3, GL_FLOAT, 0, VBO_OFFSET (0));
glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, m_vboIndexHandle);

if (m_nType < 0)
	glCullFace (GL_BACK);
glEnable (GL_BLEND);
glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
glDepthMask (1);
glColor3f (1.0f, 1.0f, 1.0f);

view->SetupOpenGL ();
view->SetupOpenGL (&pObject->Orient (), &pObject->Position ());

for (int i = 0; i < m_nSubModels; i++) {
	if (m_subModels [i].m_nParent == -1)
		m_subModels [i].Render (*this, i, nGunId, nBombId, nMissileId, nMissiles);
	}

for (; nLevel > 0; nLevel--) 
	glPopMatrix ();
	
view->ResetOpenGL ();
view->ResetOpenGL ();

if (m_nType < 0)
	glCullFace (GL_FRONT);
m_bRendered = 1;
glCullFace (GL_FRONT);
glDisableClientState (GL_COLOR_ARRAY);
glDisableClientState (GL_NORMAL_ARRAY);
glDisableClientState (GL_TEXTURE_COORD_ARRAY);
glDisableClientState (GL_VERTEX_ARRAY);
glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0);
glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
glDisable (GL_TEXTURE_2D);
return 1;
#endif
}

//------------------------------------------------------------------------------
//eof
