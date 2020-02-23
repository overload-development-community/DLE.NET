#include <math.h>
#include <stdlib.h>

#include "mine.h"
#include "dle-xp.h"
#include "rendermodel.h"
#include "modelmanager.h"
#include "glew.h"

using namespace RenderModel;

//------------------------------------------------------------------------------

void CModel::Init (void)
{
memset (m_teamTextures, 0, sizeof (m_teamTextures));
memset (m_nGunSubModels, 0xFF, sizeof (m_nGunSubModels));
m_nModel = -1;
m_fScale = 0;
m_nType = 0; //-1: custom mode, 0: default model, 1: alternative model, 2: hires model
m_nFaces = 0;
m_iFace = 0;
m_nVerts = 0;
m_nFaceVerts = 0;
m_iFaceVert = 0;
m_nSubModels = 0;
m_nTextures = 0;
m_iSubModel = 0;
m_bHasTransparency = 0;
m_bValid = 0;
m_bRendered = 0;
m_bBullets = 0;
m_vBullets.Clear ();
m_vboDataHandle = 0;
m_vboIndexHandle = 0;
}

//------------------------------------------------------------------------------

bool CModel::Create (void)
{
m_vertices.Create (m_nVerts);
m_color.Create (m_nVerts);
m_vertBuf [0].Create (m_nFaceVerts);
m_faceVerts.Create (m_nFaceVerts);
m_vertNorms.Create (m_nFaceVerts);
m_subModels.Create (m_nSubModels);
m_faces.Create (m_nFaces);
m_index [0].Create (m_nFaceVerts);
m_sortedVerts.Create (m_nFaceVerts);

m_vertices.Clear (0);
m_color.Clear (0xff);
m_vertBuf [0].Clear (0);
m_faceVerts.Clear (0);
m_vertNorms.Clear (0);
m_subModels.Clear (0);
m_faces.Clear (0);
m_index [0].Clear (0);
m_sortedVerts.Clear (0);

for (ushort i = 0; i < m_nSubModels; i++)
	m_subModels [i].m_nSubModel = i;

glGetError ();
glGenBuffersARB (1, &m_vboDataHandle);
int i = glGetError ();
if (i) {
	Destroy ();
	return false;
	}
m_vboIndexHandle = 0;
glGenBuffersARB (1, &m_vboIndexHandle);
return true;
}

//------------------------------------------------------------------------------

void CModel::Destroy (void)
{
if (m_vboDataHandle)
	glDeleteBuffersARB (1, &m_vboDataHandle);
if (m_vboIndexHandle)
	glDeleteBuffersARB (1, &m_vboIndexHandle);
m_textures = null;
m_sortedVerts.Destroy ();
m_index [0].Destroy ();
m_faces.Destroy ();
m_subModels.Destroy ();
m_vertNorms.Destroy ();
m_faceVerts.Destroy ();
m_vertBuf [0].Destroy ();
m_vertBuf [1].SetBuffer (0);	//avoid trying to delete memory allocated by the graphics driver
m_color.Destroy ();
m_vertices.Destroy ();
Init ();
}

//------------------------------------------------------------------------------

int CFace::Compare (const CFace* pf, const CFace* pm)
{
if (pf == pm)
	return 0;
if (pf->m_bBillboard < pm->m_bBillboard)
	return -1;
if (pf->m_bBillboard > pm->m_bBillboard)
	return 1;
if (pf->m_nTexture < pm->m_nTexture)
	return -1;
if (pf->m_nTexture > pm->m_nTexture)
	return 1;
if (pf->m_nVerts < pm->m_nVerts)
	return -1;
if (pf->m_nVerts > pm->m_nVerts)
	return 1;
return 0;
}

//------------------------------------------------------------------------------

const bool CFace::operator!= (CFace& other)
{
if (m_nTexture < other.m_nTexture)
	return true;
if (m_nTexture > other.m_nTexture)
	return true;
if (m_nVerts < other.m_nVerts)
	return true;
if (m_nVerts > other.m_nVerts)
	return true;
return false;
}

//------------------------------------------------------------------------------

void CFace::SetTexture (CTexture* textureP)
{
m_textureP = (textureP && (m_nTexture >= 0)) ? textureP + m_nTexture : null;
}

//------------------------------------------------------------------------------

int CFace::GatherVertices (CDynamicArray<RenderModel::CVertex>& source, CDynamicArray<RenderModel::CVertex>& dest, int nIndex)
{
#if DBG
if (uint (m_nIndex + m_nVerts) > source.Length ())
	return 0;
if (uint (nIndex + m_nVerts) > dest.Length ())
	return 0;
#endif
memcpy (dest + nIndex, source + m_nIndex, m_nVerts * sizeof (CVertex));
m_nIndex = nIndex;	//set this face's index of its first vertex in the model's vertex buffer
return m_nVerts;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CSubModel::InitMinMax (void)
{
m_vMin.Set (1e30f, 1e30f, 1e30f);
m_vMax.Set (-1e30f, -1e30f, -1e30f);
}

//------------------------------------------------------------------------------

void CSubModel::SetMinMax (CFloatVector *pVertex)
{
	CFloatVector v = *pVertex;

if (m_vMin.v.x > v.v.x)
	m_vMin.v.x = v.v.x;
if (m_vMin.v.y > v.v.y)
	m_vMin.v.y = v.v.y;
if (m_vMin.v.z > v.v.z)
	m_vMin.v.z = v.v.z;
if (m_vMax.v.x < v.v.x)
	m_vMax.v.x = v.v.x;
if (m_vMax.v.y < v.v.y)
	m_vMax.v.y = v.v.y;
if (m_vMax.v.z < v.v.z)
	m_vMax.v.z = v.v.z;
}

//------------------------------------------------------------------------------

void CSubModel::SortFaces (CTexture* textureP)
{
	CQuickSort<CFace>	qs;

for (int i = 0; i < m_nFaces; i++)
	m_faces [i].SetTexture (textureP);
if (m_nFaces > 1)
	qs.SortAscending (m_faces, 0, static_cast<uint> (m_nFaces - 1), &RenderModel::CFace::Compare);
}

//------------------------------------------------------------------------------

void CSubModel::GatherVertices (CDynamicArray<RenderModel::CVertex>& source, CDynamicArray<RenderModel::CVertex>& dest)
{
	int nIndex = m_nIndex;	//this submodels vertices start at m_nIndex in the models vertex buffer

// copy vertices face by face
for (int i = 0; i < m_nFaces; i++)
	nIndex += m_faces [i].GatherVertices (source, dest, nIndex);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CModel::Setup (int bHires, int bSort)
{
	CSubModel*		psm;
	CFace*			pfi, * pfj;
	CVertex*			pmv;
	CFloatVector*	pv, * pn;
	tTexCoord2d*	pt;
	rgbaColorf*		pc;
	CTexture*		textureP = bHires ? m_textures->Buffer () : null;
	int				i, j;
	ushort			nId;

m_fScale = 1;
for (i = 0, j = m_nFaceVerts; i < j; i++)
	m_index [0][i] = i;
//sort each submodel's faces

for (i = 0; i < m_nSubModels; i++) {
	psm = &m_subModels [i];
	if (psm->m_nFaces) {
		if (bSort) {
			psm->SortFaces (textureP);
			psm->GatherVertices (m_faceVerts, m_sortedVerts);
			}
		pfi = psm->m_faces;
		pfi->SetTexture (textureP);
		for (nId = 0, j = psm->m_nFaces - 1; j; j--) {
			pfi->m_nId = nId;
			pfj = pfi++;
			pfi->SetTexture (textureP);
			if (*pfi != *pfj)
				nId++;
			if (textureP && (textureP [pfi->m_nTexture].Transparent ()))
			m_bHasTransparency = 1;
		}
		pfi->m_nId = nId;
		}
	}

m_vbVerts.SetBuffer (reinterpret_cast<CFloatVector*> (m_vertBuf [0].Buffer ()), 1, m_vertBuf [0].Length ());
m_vbNormals.SetBuffer (m_vbVerts.Buffer () + m_nFaceVerts, true, m_vertBuf [0].Length ());
m_vbColor.SetBuffer (reinterpret_cast<rgbaColorf*> (m_vbNormals.Buffer () + m_nFaceVerts), 1, m_vertBuf [0].Length ());
m_vbTexCoord.SetBuffer (reinterpret_cast<tTexCoord2d*> (m_vbColor.Buffer () + m_nFaceVerts), 1, m_vertBuf [0].Length ());
pv = m_vbVerts.Buffer ();
pn = m_vbNormals.Buffer ();
pt = m_vbTexCoord.Buffer ();
pc = m_vbColor.Buffer ();
pmv = bSort ? m_sortedVerts.Buffer () : m_faceVerts.Buffer ();
for (i = 0, j = m_nFaceVerts; i < j; i++, pmv++) {
	pv [i] = pmv->m_vertex;
	pn [i] = pmv->m_normal;
	pc [i] = pmv->m_baseColor;
	pt [i] = pmv->m_texCoord;
	}

if (m_vertBuf [0].Buffer ()) { // points to graphics driver buffer for VBO based rendering
	glBindBufferARB (GL_ARRAY_BUFFER_ARB, m_vboDataHandle);
	glEnableClientState (GL_VERTEX_ARRAY);
	glBufferDataARB (GL_ARRAY_BUFFER, m_nFaceVerts * sizeof (CRenderVertex), reinterpret_cast<void*> (m_vertBuf [0].Buffer ()), GL_STATIC_DRAW_ARB);
	}

if (m_index [0].Buffer ()) { // points to graphics driver buffer for VBO based rendering
	glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, m_vboIndexHandle);
	glEnableClientState (GL_VERTEX_ARRAY);
	glBufferDataARB (GL_ELEMENT_ARRAY_BUFFER_ARB, m_nFaceVerts * sizeof (short), reinterpret_cast<void*> (m_index [0].Buffer ()), GL_STATIC_DRAW_ARB);
	}
if (bSort)
	memcpy (m_faceVerts.Buffer (), m_sortedVerts.Buffer (), m_faceVerts.Size ());
else
	memcpy (m_sortedVerts.Buffer (), m_faceVerts.Buffer (), m_sortedVerts.Size ());
m_bValid = 1;
glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0);
glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
m_sortedVerts.Destroy ();
}

//------------------------------------------------------------------------------
//eof
