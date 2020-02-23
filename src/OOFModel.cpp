#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include "mine.h"
#include "dle-xp.h"
#include "OOFModel.h"

#define MAXGAP	0.01f

using namespace OOF;

extern int nDbgModel;

//------------------------------------------------------------------------------

#define OOF_MEM_OPT	1

#ifdef __unix
#	ifndef _stricmp
#		define	_stricmp	strcasecmp
#	endif
#	ifndef _strnicmp
#		define	_strnicmp	strncasecmp
#	endif
#endif

//------------------------------------------------------------------------------

static int nIndent = 0;

//------------------------------------------------------------------------------

inline float fsqr (float x)
{
return x * x;
}

//------------------------------------------------------------------------------

float OOF_Centroid (CFloatVector *pvCentroid, CFloatVector vSrc[], int nv)
{
	CFloatVector	vNormal, vCenter;
	float				fTotalArea;
	int				i;

pvCentroid->Clear ();

// First figure out the total area of this polygon
vNormal = Perpendicular (vSrc [0], vSrc [1], vSrc [2]);
fTotalArea = vNormal.Mag () * 0.5f;
for (i = 2; i < nv - 1; i++) {
	vNormal = Perpendicular (vSrc [0], vSrc [i], vSrc [i + 1]);
	fTotalArea += vNormal.Mag () / 2;
	}
// Now figure out how much weight each triangle represents to the overall polygon
vNormal = Perpendicular (vSrc [0], vSrc [1], vSrc [2]);
float fArea = vNormal.Mag () * 0.5f;
// Get the center of the first polygon
vCenter = vSrc [0] + vSrc [1] + vSrc [2];
*pvCentroid += vCenter / (3.0f * fTotalArea / fArea);
// Now do the same for the rest
for (i = 2; i < nv - 1; i++) {
	vNormal = Perpendicular (vSrc [0], vSrc [i], vSrc [i + 1]);
	fArea = vNormal.Mag () * 0.5f;
	vCenter = vSrc [0] + vSrc [i] + vSrc [i + 1];
	*pvCentroid +=  vCenter / (3.0f * fTotalArea / fArea);
	}
return fTotalArea;
}

//------------------------------------------------------------------------------

static sbyte OOF_ReadByte (CFileManager& fp, const char *pszIdent)
{
sbyte b = fp.ReadUByte ();
return b;
}

//------------------------------------------------------------------------------

static int OOF_ReadInt (CFileManager& fp, const char *pszIdent)
{
int i = fp.ReadInt32 ();
return i;
}

//------------------------------------------------------------------------------

static float OOF_ReadFloat (CFileManager& fp, const char *pszIdent)
{
float f = fp.ReadFloat ();
return f;
}

//------------------------------------------------------------------------------

static void OOF_ReadVector (CFileManager& fp, CFloatVector *pv, const char *pszIdent)
{
pv->v.x = fp.ReadFloat ();
pv->v.y = fp.ReadFloat ();
pv->v.z = fp.ReadFloat ();
}

//------------------------------------------------------------------------------

static char *OOF_ReadString (CFileManager& fp, const char *pszIdent)
{
	char	*psz;
	int	l;

l = OOF_ReadInt (fp, "string length");
if (!(psz = new char [l + 1]))
	return null;
if (!l || fp.Read (psz, l, 1)) {
	psz [l] = '\0';
	return psz;
	}
delete[] psz;
return null;
}

//------------------------------------------------------------------------------

static int OOF_ReadIntList (CFileManager& fp, CDynamicArray<int>& list)
{
	uint	i;
	char	szId [20] = "";

list.Destroy ();
if (!(i = OOF_ReadInt (fp, "nList"))) 
	return 0;
if (!list.Create (i))
	return -1;
for (i = 0; i < list.Length (); i++) {
	list [i] = OOF_ReadInt (fp, szId);
	}
return list.Length ();
}

//------------------------------------------------------------------------------

static int ListType (char *pListId)
{
if (!strncmp (pListId, "TXTR", 4))
	return 0;
if (!strncmp (pListId, "OHDR", 4))
	return 1;
if (!strncmp (pListId, "SOBJ", 4))
	return 2;
if (!strncmp (pListId, "GPNT", 4))
	return 3;
if (!strncmp (pListId, "SPCL", 4))
	return 4;
if (!strncmp (pListId, "ATCH", 4))
	return 5;
if (!strncmp (pListId, "PANI", 4))
	return 6;
if (!strncmp (pListId, "RANI", 4))
	return 7;
if (!strncmp (pListId, "ANIM", 4))
	return 8;
if (!strncmp (pListId, "WBAT", 4))
	return 9;
if (!strncmp (pListId, "NATH", 4))
	return 10;
return -1;
}

//------------------------------------------------------------------------------

static void OOF_InitMinMax (CFloatVector *pvMin, CFloatVector *pvMax)
{
if (pvMin && pvMax) {
	(*pvMin).v.x =
	(*pvMin).v.y =
	(*pvMin).v.z = 1000000;
	(*pvMax).v.x =
	(*pvMax).v.y =
	(*pvMax).v.z = -1000000;
	}
}

//------------------------------------------------------------------------------

static void OOF_GetMinMax (CFloatVector *pv, CFloatVector *pvMin, CFloatVector *pvMax)
{
if (pvMin && pvMax) {
	if ((*pvMin).v.x > (*pv).v.x)
		(*pvMin).v.x = (*pv).v.x;
	if ((*pvMax).v.x < (*pv).v.x)
		(*pvMax).v.x = (*pv).v.x;
	if ((*pvMin).v.y > (*pv).v.y)
		(*pvMin).v.y = (*pv).v.y;
	if ((*pvMax).v.y < (*pv).v.y)
		(*pvMax).v.y = (*pv).v.y;
	if ((*pvMin).v.z > (*pv).v.z)
		(*pvMin).v.z = (*pv).v.z;
	if ((*pvMax).v.z < (*pv).v.z)
		(*pvMax).v.z = (*pv).v.z;
	}
}

//------------------------------------------------------------------------------

static bool OOF_ReadVertList (CFileManager& fp, CDynamicArray<CFloatVector>& list, int nVerts, CFloatVector *pvMin, CFloatVector *pvMax)
{
	char	szId [20] = "";

OOF_InitMinMax (pvMin, pvMax);
if (!list.Create (nVerts))
	return false;

for (int i = 0; i < nVerts; i++) {
	OOF_ReadVector (fp, list + i, szId);
	OOF_GetMinMax (list + i, pvMin, pvMax);
	}
return true;
}

//------------------------------------------------------------------------------

void OOF::CFrameInfo::Init (void)
{
m_nFrames = 0;
m_nFirstFrame = 0;
m_nLastFrame = 0;
}

//------------------------------------------------------------------------------

int OOF::CFrameInfo::Read (CFileManager& fp, CModel* po, int bTimed)
{
nIndent += 2;
if (bTimed) {
	m_nFrames = OOF_ReadInt (fp, "nFrames");
	m_nFirstFrame = OOF_ReadInt (fp, "nFirstFrame");
	m_nLastFrame = OOF_ReadInt (fp, "nLastFrame");
	if (po->m_frameInfo.m_nFirstFrame > m_nFirstFrame)
		po->m_frameInfo.m_nFirstFrame = m_nFirstFrame;
	if (po->m_frameInfo.m_nLastFrame < m_nLastFrame)
		po->m_frameInfo.m_nLastFrame = m_nLastFrame;
	}
else
	m_nFrames = po->m_frameInfo.m_nFrames;
nIndent -= 2;
return 1;
}

//------------------------------------------------------------------------------

int CRotFrame::Read (CFileManager& fp, int bTimed)
{
	float	fMag;

nIndent += 2;
if (bTimed)
	m_nStartTime = OOF_ReadInt (fp, "nStartTime");
OOF_ReadVector (fp, &m_vAxis, "vAxis");
if (0 < (fMag = m_vAxis.Mag ()))
	m_vAxis /= fMag;
m_nAngle = OOF_ReadInt (fp, "nAngle");
nIndent -= 2;
return 1;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CAnim::Init (void)
{
m_nTicks = 0;
}

//------------------------------------------------------------------------------

void CAnim::Destroy (void)
{
m_remapTicks.Destroy ();
Init ();
}

//------------------------------------------------------------------------------

void CRotAnim::Init (void)
{
CAnim::Init ();
}

//------------------------------------------------------------------------------

void CRotAnim::Destroy (void)
{
m_frames.Destroy ();
CAnim::Destroy ();
}

//------------------------------------------------------------------------------

int CRotAnim::Read (CFileManager& fp, CModel* po, int bTimed)
{
if (!this->CFrameInfo::Read (fp, po, bTimed))
	return 0;
if (!m_frames.Create (m_nFrames))
	return 0;
m_frames.Clear (0);
if (bTimed &&
	 (m_nTicks = abs (m_nLastFrame - m_nFirstFrame) + 1) &&
	 !m_remapTicks.Create (m_nTicks)) {
	 Destroy ();
	return 0;
	}
if (m_nTicks)
	for (int i = 0; i < m_nFrames; i++)
		if (!m_frames [i].Read (fp, bTimed)) {
			Destroy ();
			return 0;
			}
return 1;
}

//------------------------------------------------------------------------------

void CRotAnim::BuildAngleMatrix (CDoubleMatrix *pm, int a, CFloatVector *pAxis)
{
float x = (*pAxis).v.x;
float y = (*pAxis).v.y;
float z = (*pAxis).v.z;
float s = (float) sin ((float) a);
float c = (float) cos ((float) a);
float t = 1.0f - c;
float i = t * x;
float j = s * z;
//pm->m_r.x = t * x * x + c;
pm->m.rVec.v.x = i * x + c;
i *= y;
//pm->m.v.r.v.c.y = t * x * y + s * z;
//pm->m.v.u.v.c.x = t * x * y - s * z;
pm->m.rVec.v.y = i + j;
pm->m.uVec.v.x = i - j;
i = t * z;
//pm->m.v.f.v.c.z = t * z * z + c;
pm->m.fVec.v.z = i * z + c;
i *= x;
j = s * y;
//pm->m.v.r.v.c.z = t * x * z - s * y;
//pm->m.v.f.v.c.x = t * x * z + s * y;
pm->m.rVec.v.z = i - j;
pm->m.fVec.v.x = i + j;
i = t * y;
//pm->m.v.u.v.c.y = t * y * y + c;
pm->m.uVec.v.y = i * y + c;
i *= z;
j = s * x;
//pm->m.v.u.v.c.z = t * y * z + s * x;	
//pm->m.v.f.v.c.y = t * y * z - s * x;
pm->m.uVec.v.z = i + j;
pm->m.fVec.v.y = i - j;
}

//------------------------------------------------------------------------------

void CRotAnim::BuildMatrices (void)
{
	CDoubleMatrix	mBase, mTemp;
	CRotFrame*		pf;
	int				i;

mBase.Clear ();
for (i = m_frames.Length (), pf = m_frames.Buffer (); i; i--, pf++) {
	BuildAngleMatrix (&mTemp, pf->m_nAngle, &pf->m_vAxis);
	pf->m_mMat = mTemp * mBase;
	mBase = pf->m_mMat;
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

int CPosFrame::Read (CFileManager& fp, int bTimed)
{
nIndent += 2;
if (bTimed)
	m_nStartTime = OOF_ReadInt (fp, "nStartTime");
OOF_ReadVector (fp, &m_vPos, "vPos");
nIndent -= 2;
return 1;
}

//------------------------------------------------------------------------------

void CPosAnim::Destroy (void)
{
m_frames.Destroy ();
CAnim::Destroy ();
}

//------------------------------------------------------------------------------

int CPosAnim::Read (CFileManager& fp, CModel* po, int bTimed)
{
if (!this->CFrameInfo::Read (fp, po, bTimed))
	return 0;
if (bTimed &&
	 (m_nTicks = m_nLastFrame - m_nFirstFrame) &&
	 !m_remapTicks.Create (m_nTicks)) {
	 Destroy ();
	 return 0;
	}
if (!m_frames.Create (m_nFrames)) {
	Destroy ();
	return 0;
	}
m_frames.Clear (0);
for (int i = 0; i < m_nFrames; i++)
	if (!m_frames [i].Read (fp, bTimed)) {	
		Destroy ();
		return 0;
		}
return 1;
}

//------------------------------------------------------------------------------

void CSpecialPoint::Init (void)
{
memset (this, 0, sizeof (*this));
}

//------------------------------------------------------------------------------

void CSpecialPoint::Destroy (void)
{
delete[] m_pszName;
m_pszName = null;
delete[] m_pszProps;
m_pszProps = null;
}

//------------------------------------------------------------------------------

int CSpecialPoint::Read (CFileManager& fp)
{
Init ();
nIndent += 2;
if (!(m_pszName = OOF_ReadString (fp, "pszName"))) {
	nIndent -= 2;
	return 0;
	}
if (!(m_pszProps = OOF_ReadString (fp, "pszProps"))) {
	nIndent -= 2;
	return 0;
	}
OOF_ReadVector (fp, &m_vPos, "vPos");
m_fRadius = OOF_ReadFloat (fp, "fRadius");
nIndent -= 2;
return 1;
}

//------------------------------------------------------------------------------

int CSpecialList::Read (CFileManager& fp)
{
	uint	i;

i = OOF_ReadInt (fp, "nVerts");
if (!i)
	return 1;
if (!Create (i))
	return 0;
for (i = 0; i < Length (); i++)
	(*this) [i].Read (fp);
return 1;
}

//------------------------------------------------------------------------------

int OOF::CPoint::Read (CFileManager& fp, int bParent)
{
nIndent += 2;
m_nParent = bParent ? OOF_ReadInt (fp, "nParent") : 0;
OOF_ReadVector (fp, &m_vPos, "vPos");
OOF_ReadVector (fp, &m_vDir, "vDir");
nIndent -= 2;
return 1;
}

//------------------------------------------------------------------------------

int CPointList::Read (CFileManager& fp, int bParent, int nSize)
{
	int	i;

nIndent += 2;
i = OOF_ReadInt (fp, "nPoints");
if (nSize < i)
	nSize = i;
if (!Create (i)) {
	nIndent -= 2;
	return 0;
	}
for (i = 0; i < static_cast<int> (Length ()); i++)
	if (!(*this) [i].Read (fp, bParent)) {
		Destroy ();
		nIndent -= 2;
		return 0;
		}
nIndent -= 2;
return 1;
}

//------------------------------------------------------------------------------

int CAttachList::Read (CFileManager& fp)
{
	int	i;

nIndent += 2;
i = OOF_ReadInt (fp, "nPoints");
if (!Create (i)) {
	nIndent -= 2;
	return 0;
	}
for (i = 0; i < static_cast<int> (Length ()); i++)
	if (!(*this) [i].CPoint::Read (fp, 1)) {
		Destroy ();
		nIndent -= 2;
		return 0;
		}
nIndent -= 2;
return 1;
}

//------------------------------------------------------------------------------

int CAttachPoint::Read (CFileManager& fp)
{
OOF_ReadVector (fp, &m_vu, "vu");	//actually ignored
OOF_ReadVector (fp, &m_vu, "vu");
m_bu = 1;
return 1;
}

//------------------------------------------------------------------------------

int CAttachList::ReadNormals (CFileManager& fp)
{
	uint	i;

nIndent += 2;
i = OOF_ReadInt (fp, "nPoints");
if (i != Length ()) {
	nIndent -= 2;
	return 0;
	}
for (i = 0; i < Length (); i++)
	(*this).Read (fp);
nIndent -= 2;
return 1;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CBattery::Init (void)
{
m_nVerts = 0;
m_nTurrets = 0;
}

//------------------------------------------------------------------------------

void CBattery::Destroy (void)
{
m_vertIndex.Destroy ();
m_turretIndex.Destroy ();
}

//------------------------------------------------------------------------------

int CBattery::Read (CFileManager& fp)
{
nIndent += 2;
if (0 > (m_nVerts = OOF_ReadIntList (fp, m_vertIndex))) {
	nIndent -= 2;
	Destroy ();
	return 0;
	}
if (0 > (m_nTurrets = OOF_ReadIntList (fp, m_turretIndex))) {
	nIndent -= 2;
	Destroy ();
	return 0;
	}
nIndent -= 2;
return 1;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

int CArmament::Read (CFileManager& fp)
{
	int	l;

nIndent += 2;
if (!(l = OOF_ReadInt (fp, "nBatts"))) {
	nIndent -= 2;
	return 1;
	}
if (!Create (l)) {
	Destroy ();
	nIndent -= 2;
	return 0;
	}
for (int i = 0; i < l; i++)
	if (!m_data.buffer [i].Read (fp)) {
		Destroy ();
		nIndent -= 2;
		return 0;
		}
nIndent -= 2;
return 1;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

int CFaceVert::Read (CFileManager& fp, int bFlipV)
{
nIndent += 2;
m_nIndex = OOF_ReadInt (fp, "nIndex");
m_fu = OOF_ReadFloat (fp, "fu");
m_fv = OOF_ReadFloat (fp, "fv");
if (bFlipV)
	m_fv = -m_fv;
nIndent -= 2;
return 1;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

inline CFloatVector* CFace::CalcCenter (CSubModel *pso)
{
	CFaceVert		*pfv = m_vertices;
	CFloatVector	*pv = pso->m_vertices.Buffer ();
	int				i;

m_vCenter.Clear ();
for (i = m_nVerts; i; i--, pfv++)
	m_vCenter += pv [pfv->m_nIndex];
m_vCenter /= (float) m_nVerts;
return &m_vCenter;
}

//------------------------------------------------------------------------------

inline CFloatVector* CFace::CalcNormal (CSubModel *pso)
{
	CFloatVector	*pv = pso->m_rotVerts.Buffer ();
	CFaceVert		*pfv = m_vertices;

m_vRotNormal = Normal (pv [pfv [0].m_nIndex], pv [pfv [1].m_nIndex], pv [pfv [2].m_nIndex]);
return &m_vRotNormal;
}

//------------------------------------------------------------------------------

int CFace::Read (CFileManager& fp, CSubModel *pso, CFaceVert *pfv, int bFlipV)
{
	int	i, v0 = 0;
	CEdge	e;

nIndent += 2;
OOF_ReadVector (fp, &m_vNormal, "vNormal");
#if 0
m_vNormal.x = -m_vNormal.x;
m_vNormal.y = -m_vNormal.y;
m_vNormal.z = -m_vNormal.z;
#endif
m_nVerts = OOF_ReadInt (fp, "nVerts");
m_bTextured = OOF_ReadInt (fp, "bTextured");
if (m_bTextured) {
	m_texProps.nTexId = OOF_ReadInt (fp, "texProps.nTexId");
	}
else {
	m_texProps.color.r = OOF_ReadByte (fp, "texProps.color.r");
	m_texProps.color.g = OOF_ReadByte (fp, "texProps.color.g");
	m_texProps.color.b = OOF_ReadByte (fp, "texProps.color.b");
	}
#if OOF_MEM_OPT
if (pfv) {
	m_vertices = pfv;
#else
	if (!(m_vertices= new CFaceVert [m_nVerts])) {
		nIndent -= 2;
		return OOF_FreeFace (&f);
		}
#endif
	OOF_InitMinMax (&m_vMin, &m_vMax);
	e.m_v1 [0] = -1;
	for (i = 0; i < m_nVerts; i++)
		if (!m_vertices [i].Read (fp, bFlipV)) {
			nIndent -= 2;
			return 0;
			}
		else {
			e.m_v0 [0] = e.m_v1 [0];
			e.m_v1 [0] = m_vertices [i].m_nIndex;
			OOF_GetMinMax (pso->m_vertices + e.m_v1 [0], &m_vMin, &m_vMax);
			if (i)
				pso->AddEdge (this, e.m_v0 [0], e.m_v1 [0]);
			else
				v0 = e.m_v1 [0];
			}
	pso->AddEdge (this, e.m_v1 [0], v0);
	CalcCenter (pso);
#if OOF_MEM_OPT
	}
else
	fp.Seek (m_nVerts * sizeof (CFaceVert), SEEK_CUR);
#endif
m_fBoundingLength = OOF_ReadFloat (fp, "fBoundingLength");
m_fBoundingWidth = OOF_ReadFloat (fp, "fBoundingWidth");
nIndent -= 2;
return m_nVerts;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CFaceList::Init (void)
{
m_nFaces = 0;
}

//------------------------------------------------------------------------------

void CFaceList::Destroy (void)
{
m_list.Destroy ();
m_vertices.Destroy ();
Init ();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void OOF::CEdgeList::Init (void)
{
m_nEdges = m_nContourEdges = 0;
}

//------------------------------------------------------------------------------

void OOF::CEdgeList::Destroy (void)
{
m_list.Destroy ();
Init ();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CSubModel::Init (void)
{
m_nIndex = 0;
m_nParent = 0;
m_nFlags = 0;
m_nTreeOffset = 0;
m_nDataOffset = 0;
m_nMovementType = 0;
m_nMovementAxis = 0;
m_nFSLists = 0;
m_nVerts = 0;
m_nChildren = 0;
m_fd = 0;
m_fRadius = 0;
m_fFOV = 0;
m_fRPS = 0;
m_fUpdate = 0;
memset (m_children, 0, sizeof (m_children));
m_pszName = null;
m_pszProps = null;
m_faces.Init ();
m_edges.Init ();
m_glowInfo.Init ();
m_vNormal.Clear ();
m_vPlaneVert.Clear ();
m_vOffset.Clear ();
m_vCenter.Clear ();
m_vMin.Clear ();
m_vMax.Clear ();
m_vMod.Clear ();
m_aMod.Clear ();
m_mMod.Clear ();	
}

//------------------------------------------------------------------------------

void CSubModel::Destroy (void)
{
#if !OOF_MEM_OPT
	int	i;
#endif

delete[] m_pszName;
m_pszName = null;
delete[] m_pszProps;
m_pszProps = null;
m_vertices.Destroy ();
m_rotVerts.Destroy ();
m_vertColors.Destroy ();
m_normals.Destroy ();
m_pfAlpha.Destroy ();
m_posAnim.Destroy ();
m_rotAnim.Destroy ();
m_faces.Destroy ();
m_edges.Destroy ();
}

//------------------------------------------------------------------------------

int CSubModel::FindVertex (int i)
{
	CFloatVector	v, *pv;
	int				j;

pv = m_vertices.Buffer ();
v = pv [i];
for (j = 0; j < i; j++, pv++)
	if ((v.v.x == (*pv).v.x) && (v.v.y == (*pv).v.y) && (v.v.z == (*pv).v.z))
		return j;
return i;
}

//------------------------------------------------------------------------------

int CSubModel::FindEdge (int i0, int i1)
{
	int				i;
	CEdge				h;
	CFloatVector	v0, v1, hv0, hv1;

for (i = 0; i < m_edges.m_nEdges; i++) {
	h = m_edges.m_list [i];
	if (((h.m_v0 [0] == i0) && (h.m_v1 [0] == i1)) || ((h.m_v0 [0] == i1) && (h.m_v1 [0] == i0)))
		return i;
	}
v0 = m_vertices [i0]; 
v1 = m_vertices [i1]; 
for (i = 0; i < m_edges.m_nEdges; i++) {
	h = m_edges.m_list [i];
	hv0 = m_vertices [h.m_v0 [0]]; 
	hv1 = m_vertices [h.m_v1 [0]]; 
	if ((hv0.v.x == v0.v.x) && (hv0.v.y == v0.v.y) && (hv0.v.z == v0.v.z) &&
		 (hv1.v.x == v1.v.x) && (hv1.v.y == v1.v.y) && (hv1.v.z == v1.v.z))
		return i;
	if ((hv1.v.x == v0.v.x) && (hv1.v.y == v0.v.y) && (hv1.v.z == v0.v.z) &&
		 (hv0.v.x == v1.v.x) && (hv0.v.y == v1.v.y) && (hv0.v.z == v1.v.z))
		return i;
	}
for (i = 0; i < m_edges.m_nEdges; i++) {
	h = m_edges.m_list [i];
	hv0 = m_vertices [h.m_v0 [0]] - v0;
	hv1 = m_vertices [h.m_v1 [0]] - v1;
	if ((hv0.Mag () < MAXGAP) && (hv1.Mag () < MAXGAP))
		return i;
	hv0 = m_vertices [h.m_v0 [0]] - v1;
	hv1 = m_vertices [h.m_v1 [0]] - v0;
	if ((hv0.Mag () < MAXGAP) && (hv1.Mag () < MAXGAP))
		return i;
	}
return -1;
}

//------------------------------------------------------------------------------

int CSubModel::AddEdge (CFace *pf, int v0, int v1)
{
	int	i = FindEdge (v0, v1);
	CEdge	*pe;

if (m_nFlags & (OOF_SOF_GLOW | OOF_SOF_THRUSTER))
	return -1;
if (i < 0)
	i = m_edges.m_nEdges++;
pe = m_edges.m_list + i;
if (i < 0) {
	}
if (pe->m_faces [0]) {
	pe->m_faces [1] = pf;
	if (pf->m_bReverse) {
		pe->m_v0 [1] = v1;
		pe->m_v1 [1] = v0;
		}
	else {
		pe->m_v0 [1] = v0;
		pe->m_v1 [1] = v1;
		}
	}
else {
	pe->m_faces [0] = pf;
	if (pf->m_bReverse) {
		pe->m_v0 [0] = v1;
		pe->m_v1 [0] = v0;
		}
	else {
		pe->m_v0 [0] = v0;
		pe->m_v1 [0] = v1;
		}
	}
return i;
}

//------------------------------------------------------------------------------

void CSubModel::SetProps (char *pszProps)
{
	// first, extract the command

	int l;
	char command [200], data [200], *psz;

if (3 > (l = (int) strlen (pszProps)))
	return;
if ((psz = strchr (pszProps, '=')))
	l = (int) (psz - pszProps + 1);
memcpy (command, pszProps, l);
command [l] = '\0';
if (psz)
	strcpy (data, psz + 1);
else
	*data = '\0';

// now act on the command/data pair

if (!_stricmp (command, "$rotate=")) { // constant rotation for a subobject
	float spinrate = (float) atof( data);
	if ((spinrate <= 0) || (spinrate > 20))
		return;		// bad data
	m_nFlags |= OOF_SOF_ROTATE;
	m_fRPS = 1.0f / spinrate;
	return;
	}

if (!_strnicmp (command, "$jitter", 7)) {	// this subobject is a jittery CObject
	m_nFlags |= OOF_SOF_JITTER;
	return;
	}

if (!_strnicmp (command, "$shell", 6)) { // this subobject is a door shell
	m_nFlags |= OOF_SOF_SHELL;
	return;
	}

if (!_strnicmp (command, "$facing", 7)) { // this subobject always faces you
	m_nFlags |= OOF_SOF_FACING;
	return;
	}

if (!_strnicmp (command, "$frontface", 10)) { // this subobject is a door front
	m_nFlags |= OOF_SOF_FRONTFACE;
	return;
	}

if (!_stricmp (command, "$glow=")) {
	float r,g,b;
	float size;

	sscanf (data, " %f, %f, %f, %f", &r, &g, &b, &size);
	m_nFlags |= OOF_SOF_GLOW;
	//m_glowInfo = new CGlowInfo;
	m_glowInfo.m_color.r = r;
	m_glowInfo.m_color.g = g;
	m_glowInfo.m_color.b = b;
	m_glowInfo.m_fSize = size;
	return;
	}

if (!_stricmp (command, "$thruster=")) {
	float r,g,b;
	float size;

	sscanf(data, " %f, %f, %f, %f", &r,&g,&b,&size);
	m_nFlags |= OOF_SOF_THRUSTER;
	//m_glowInfo = new CGlowInfo;
	m_glowInfo.m_color.r = r;
	m_glowInfo.m_color.g = g;
	m_glowInfo.m_color.b = b;
	m_glowInfo.m_fSize = size;
	return;
	}

if (!_stricmp (command, "$fov=")) {
	float fov_angle;
	float turret_spr;
	float reactionTime;

	sscanf(data, " %f, %f, %f", &fov_angle, &turret_spr, &reactionTime);
	if (fov_angle < 0.0f || fov_angle > 360.0f) { // Bad data
		fov_angle = 1.0;
		}
	// .4 is really fast and really arbitrary
	if (turret_spr < 0.0f || turret_spr > 60.0f) { // Bad data
		turret_spr = 1.0f;
		}
	// 10 seconds is really slow and really arbitrary
	if (reactionTime < 0.0f || reactionTime > 10.0f) { // Bad data
		reactionTime = 10.0;
		}
	m_nFlags |= OOF_SOF_TURRET;
	m_fFOV = fov_angle/720.0f; // 720 = 360 * 2 and we want to make fov the amount we can move in either direction
	                             // it has a minimum value of (0.0) to [0.5]
	m_fRPS = 1.0f / turret_spr;  // convert spr to rps (rotations per second)
	m_fUpdate = reactionTime;
	return;
	}

if (!_stricmp (command, "$monitor01")) { // this subobject is a monitor
	m_nFlags |= OOF_SOF_MONITOR1;
	return;
	}

if (!_stricmp (command, "$monitor02")) { // this subobject is a 2nd monitor
	m_nFlags |= OOF_SOF_MONITOR2;
	return;
	}

if (!_stricmp (command, "$monitor03")) { // this subobject is a 3rd monitor
	m_nFlags |= OOF_SOF_MONITOR3;
	return;
	}

if (!_stricmp (command, "$monitor04")) { // this subobject is a 4th monitor
	m_nFlags |= OOF_SOF_MONITOR4;
	return;
	}

if (!_stricmp (command, "$monitor05")) { // this subobject is a 4th monitor
	m_nFlags |= OOF_SOF_MONITOR5;
	return;
	}

if (!_stricmp (command, "$monitor06")) { // this subobject is a 4th monitor
	m_nFlags |= OOF_SOF_MONITOR6;
	return;
	}

if (!_stricmp (command, "$monitor07")) { // this subobject is a 4th monitor
	m_nFlags |= OOF_SOF_MONITOR7;
	return;
	}

if (!_stricmp (command, "$monitor08")) { // this subobject is a 4th monitor
	m_nFlags |= OOF_SOF_MONITOR8;
	return;
	}

if (!_stricmp (command, "$viewer")) { // this subobject is a viewer
	m_nFlags |= OOF_SOF_VIEWER;
	return;
	}

if (!_stricmp (command, "$layer")) { // this subobject is a layer to be drawn after original CObject.
	m_nFlags |= OOF_SOF_LAYER;
	return;
	}

if (!_stricmp (command, "$custom")) { // this subobject has custom textures/colors
	m_nFlags |= OOF_SOF_CUSTOM;
	return;
	}
}

//------------------------------------------------------------------------------

int CSubModel::Read (CFileManager& fp, CModel* po, int bFlipV)
{
	int				h, i;
#if OOF_MEM_OPT
	int				bReadData, nPos, nFaceVerts = 0;
#endif
	char				szId [20] = "";

nIndent += 2;
Init ();
m_nIndex = OOF_ReadInt (fp, "nIndex");
if (m_nIndex >= OOF_MAX_SUBOBJECTS) {
	nIndent -= 2;
	return 0;
	}
m_nParent = OOF_ReadInt (fp, "nParent");
OOF_ReadVector (fp, &m_vNormal, "vNormal");
m_fd = OOF_ReadFloat (fp, "fd");
OOF_ReadVector (fp, &m_vPlaneVert, "vPlaneVert");
OOF_ReadVector (fp, &m_vOffset, "vOffset");
m_fRadius = OOF_ReadFloat (fp, "fRadius");
m_nTreeOffset = OOF_ReadInt (fp, "nTreeOffset");
m_nDataOffset = OOF_ReadInt (fp, "nDataOffset");
if (po->m_nVersion > 1805)
	OOF_ReadVector (fp, &m_vCenter, "vCenter");
if (!(m_pszName = OOF_ReadString (fp, "pszName"))) {
	Destroy ();
	return 0;
	}
if (!(m_pszProps = OOF_ReadString (fp, "pszProps"))) {
	Destroy ();
	return 0;
	}
SetProps (m_pszProps);
m_nMovementType = OOF_ReadInt (fp, "nMovementType");
m_nMovementAxis = OOF_ReadInt (fp, "nMovementAxis");
m_fsLists = null;
if ((m_nFSLists = OOF_ReadInt (fp, "nFSLists")))
	fp.Seek (m_nFSLists * sizeof (int), SEEK_CUR);
m_nVerts = OOF_ReadInt (fp, "nVerts");
if (m_nVerts) {
	if (!OOF_ReadVertList (fp, m_vertices, m_nVerts, &m_vMin, &m_vMax)) {
		Destroy ();
		nIndent -= 2;
		return 0;
		}
	m_vCenter = (m_vMin + m_vMax) / 2.0f;
	if (!m_rotVerts.Create (m_nVerts)) {
		Destroy ();
		nIndent -= 2;
		return 0;
		}
	if (!m_vertColors.Create (m_nVerts)) {
		Destroy ();
		nIndent -= 2;
		return 0;
		}
	m_vertColors.Clear (0);
	if (!(OOF_ReadVertList (fp, m_normals, m_nVerts, null, null))) {
		nIndent -= 2;
		Destroy ();
		return 0;
		}
	if (!m_pfAlpha.Create (m_nVerts)) {
		Destroy ();
		nIndent -= 2;
		return 0;
		}
	for (i = 0; i < m_nVerts; i++)
		if (po->m_nVersion < 2300) 
			m_pfAlpha [i] = 1.0f;
		else {
			m_pfAlpha [i] = OOF_ReadFloat (fp, szId);
			if	(m_pfAlpha [i] < 0.99)
				po->m_nFlags |= OOF_PMF_ALPHA;
			}
	}
m_faces.m_nFaces = OOF_ReadInt (fp, "nFaces");
if (!m_faces.m_list.Create (m_faces.m_nFaces)) {
	Destroy ();
	nIndent -= 2;
	return 0;
	}
#if OOF_MEM_OPT
nPos = (int) fp.Tell ();
m_edges.m_nEdges = 0;
for (bReadData = 0; bReadData < 2; bReadData++) {
	fp.Seek (nPos, SEEK_SET);
	if (bReadData) {
		if (!m_faces.m_vertices.Create (nFaceVerts)) {
			Destroy ();
			nIndent -= 2;
			return 0;
			}
		if (!m_edges.m_list.Create (nFaceVerts)) {
			Destroy ();
			nIndent -= 2;
			return 0;
			}
		m_edges.m_list.Clear ();
		m_edges.m_nEdges = 0;
		}
	for (i = 0, nFaceVerts = 0; i < m_faces.m_nFaces; i++) {
		if (!(h = m_faces.m_list [i].Read (fp, this, bReadData ? m_faces.m_vertices + nFaceVerts : null, bFlipV))) {
			Destroy ();
			nIndent -= 2;
			return 0;
			}
		nFaceVerts += h;
		}
	}
#else
for (i = 0; i < m_faces.m_nFaces; i++)
	if (!OOF_ReadFace (fp, &so, m_faces.faces + i, null, bFlipV)) {
		nIndent -= 2;
		Destroy ();
		return 0;
		}
#endif
nIndent -= 2;
return 1;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

int CModel::ReleaseTextures (void)
{
m_textures.Release ();
return 0;
}

//------------------------------------------------------------------------------

int CModel::ReloadTextures (void)
{
return m_textures.Bind (m_bCustom);
}

//------------------------------------------------------------------------------

int CModel::FreeTextures (void)
{
m_textures.Destroy ();
return 0;
}

//------------------------------------------------------------------------------

int CModel::ReadTextures (CFileManager& fp)
{
	int			i;
	char*			pszName;

nIndent += 2;
int nBitmaps = OOF_ReadInt (fp, "nBitmaps");
if (!(m_textures.Create (nBitmaps))) {
	nIndent -= 2;
	FreeTextures ();
	return 0;
	}
for (i = 0; i < m_textures.Count (); i++) {
	if (!(pszName = OOF_ReadString (fp, "texture"))) {
		nIndent -= 2;
		FreeTextures ();
		return 0;
		}
	char szFile [256];
	sprintf (szFile, "%s\\%s", m_folder, pszName);
	if (!strrchr (szFile, '.'))
		strcat (szFile, ".tga");
	if (!m_textures.Texture (i).LoadFromFile (szFile)) {
		nIndent -= 2;
		FreeTextures ();
		return 0;
		}
	m_textures.Name (i).SetBuffer (pszName, 0, uint (strlen (pszName) + 1));
	}
return 1;
}

//------------------------------------------------------------------------------

void CModel::Init (void)
{
m_nModel = -1;
m_nVersion = 0;
m_nFlags = 0;
m_nDetailLevels = 0;
m_nSubModels = 0;
m_bCloaked = 0;
m_nCloakPulse = 0;
m_nCloakChangedTime = 0;
m_fMaxRadius = 0;
m_fAlpha = 0;
m_vMin.Clear ();
m_vMax.Clear ();
m_textures.Init ();
m_frameInfo.Init ();
}

//------------------------------------------------------------------------------

void CModel::Destroy (void)
{
FreeTextures ();
m_subModels.Destroy ();
m_gunPoints.Destroy ();
m_attachPoints.Destroy ();
m_specialPoints.Destroy ();
m_armament.Destroy ();
m_nModel = -1;
}

//------------------------------------------------------------------------------

int CModel::ReadInfo (CFileManager& fp)
{
nIndent += 2;
m_nSubModels = OOF_ReadInt (fp, "nSubModels");
if (m_nSubModels >= OOF_MAX_SUBOBJECTS) {
	nIndent -= 2;
	return 0;
	}
m_fMaxRadius = OOF_ReadFloat (fp, "fMaxRadius");
OOF_ReadVector (fp, &m_vMin, "vMin");
OOF_ReadVector (fp, &m_vMax, "vMax");
m_nDetailLevels = OOF_ReadInt (fp, "nDetailLevels");
nIndent -= 2;
fp.Seek (m_nDetailLevels * sizeof (int), SEEK_CUR);
if (!m_subModels.Create (m_nSubModels))
	return 0;
return 1;
}

//------------------------------------------------------------------------------

void CModel::BuildAnimMatrices (void)
{
for (int i = 0; i < m_nSubModels; i++)
	m_subModels [i].m_rotAnim.BuildMatrices ();
}

//------------------------------------------------------------------------------

void CModel::AssignChildren (void)
{
	CSubModel *pso, *pParent;
	int					i;

for (i = 0, pso = m_subModels.Buffer (); i < m_nSubModels; i++, pso++) {
	int nParent = pso->m_nParent;
	if (nParent == i)
		pso->m_nParent = -1;
	else if (nParent != -1) {
		pParent = m_subModels + nParent;
		pParent->m_children [pParent->m_nChildren++] = i;
		}
	}
}

//------------------------------------------------------------------------------

inline void CModel::LinkSubModelBatteries (int iObject, int iBatt)
{
	CSubModel	*pso = m_subModels + iObject;
	int			i, nFlags = iBatt << OOF_WB_INDEX_SHIFT;

pso->m_nFlags |= nFlags | OOF_SOF_WB;	
for (i = 0; i < pso->m_nChildren; i++)
	LinkSubModelBatteries (pso->m_children [i], iBatt);
}

//------------------------------------------------------------------------------

void CModel::LinkBatteries (void)
{
	CSubModel*	pso;
	CBattery*	pb;
	int*			pti;
	int			i, j, k;

for (i = 0, pso = m_subModels.Buffer (); i < m_nSubModels; i++, pso++) {
	if (!(pso->m_nFlags & OOF_SOF_TURRET))
		continue;
	for (j = 0, pb = m_armament.Buffer (); j < static_cast<int> (m_armament.Length ()); j++, pb++)
		for (k = pb->m_nTurrets, pti = pb->m_turretIndex.Buffer (); k; k--, pti++)
			if (*pti == i) {
				LinkSubModelBatteries (i, j);
				j = m_armament.Length ();
				break;
				}
	}
}

//------------------------------------------------------------------------------

void CModel::BuildPosTickRemapList (void)
{
	int			i, j, k, t, nTicks;
	CSubModel	*pso;

for (i = m_nSubModels, pso = m_subModels.Buffer (); i; i--, pso++) {
	nTicks = pso->m_posAnim.m_nLastFrame - pso->m_posAnim.m_nFirstFrame;
	for (j = 0, t = pso->m_posAnim.m_nFirstFrame; j < nTicks; j++, t++)
		if ((k = pso->m_posAnim.m_nFrames - 1))
			for (; k >= 0; k--)
				if (t >= pso->m_posAnim.m_frames [k].m_nStartTime) {
					pso->m_posAnim.m_remapTicks [j] = k;
					break;
					}
	}
}

//------------------------------------------------------------------------------

void CModel::BuildRotTickRemapList (void)
{
	int				i, j, k, t, nTicks;
	CSubModel	*pso;

for (i = m_nSubModels, pso = m_subModels.Buffer (); i; i--, pso++) {
	nTicks = pso->m_rotAnim.m_nLastFrame - pso->m_rotAnim.m_nFirstFrame;
	for (j = 0, t = pso->m_rotAnim.m_nFirstFrame; j < nTicks; j++, t++)
		if ((k = pso->m_rotAnim.m_nFrames - 1))
			for (; k >= 0; k--)
				if (t >= pso->m_rotAnim.m_frames [k].m_nStartTime) {
					pso->m_rotAnim.m_remapTicks [j] = k;
					break;
					}
	}
}

//------------------------------------------------------------------------------

void CModel::ConfigureSubModels (void)
{
	int			i, j;
	CSubModel	*pso;

for (i = m_nSubModels, pso = m_subModels.Buffer (); i; i--, pso++) {
	if (!pso->m_rotAnim.m_nFrames)
		pso->m_nFlags &= ~(OOF_SOF_ROTATE | OOF_SOF_TURRET);

	if (pso->m_nFlags & OOF_SOF_FACING) {
		CFloatVector v [30], avg;
		CFace	*pf = pso->m_faces.m_list.Buffer ();

		for (j = 0; j < pf->m_nVerts; j++)
			v [j] = pso->m_vertices [pf->m_vertices [j].m_nIndex];
	
		pso->m_fRadius = (float) (sqrt (OOF_Centroid (&avg, v, pf->m_nVerts)) / 2);
		m_nFlags |= OOF_PMF_FACING;

		}

	if (pso->m_nFlags & (OOF_SOF_GLOW | OOF_SOF_THRUSTER)) {
		CFloatVector v [30];
		CFace	*pf = pso->m_faces.m_list.Buffer ();

		for (j = 0; j < pf->m_nVerts; j++)
			v [j] = pso->m_vertices [pf->m_vertices [j].m_nIndex];
		pso->m_glowInfo.m_vNormal = Normal (v [0], v [1], v [2]);
		m_nFlags |= OOF_PMF_FACING;	// Set this so we know when to draw
		}
	}
}

//------------------------------------------------------------------------------

void CModel::GetSubModelBounds (CSubModel *pso, CFloatVector vo)
{
	int	i;
	float	h;

vo += pso->m_vOffset;
if (m_vMax.v.x < (h = pso->m_vMax.v.x + vo.v.x))
	 m_vMax.v.x = h;
if (m_vMax.v.y < (h = pso->m_vMax.v.y + vo.v.y))
	 m_vMax.v.y = h;
if (m_vMax.v.z < (h = pso->m_vMax.v.z + vo.v.z))
	 m_vMax.v.z = h;
if (m_vMin.v.x > (h = pso->m_vMin.v.x + vo.v.x))
	 m_vMin.v.x = h;
if (m_vMin.v.y > (h = pso->m_vMin.v.y + vo.v.y))
	 m_vMin.v.y = h;
if (m_vMin.v.z > (h = pso->m_vMin.v.z + vo.v.z))
	 m_vMin.v.z = h;
for (i = 0; i < pso->m_nChildren; i++)
	GetSubModelBounds (m_subModels + pso->m_children [i], vo);
}

//------------------------------------------------------------------------------

void CModel::GetBounds (void)
{
	CSubModel*		pso;
	CFloatVector	vo;
	int				i;

vo.Clear ();
OOF_InitMinMax (&m_vMin, &m_vMax);
for (i = 0, pso = m_subModels.Buffer (); i < m_nSubModels; i++, pso++)
	if (pso->m_nParent == -1)
		GetSubModelBounds (pso, vo);
}

//------------------------------------------------------------------------------

int CModel::Read (const char* pszFolder, const short nModel)
{
if (m_nModel >= 0)
	return 0;

	CFileManager	fp;
	char				fileId [4];
	int				i, nLength, nFrames, nSubModels, bTimed = 0;

char szFile [256];
if (nModel == 108)
	sprintf (szFile, "%s\\pyrogl.oof", pszFolder);
else if (nModel == 109)
	sprintf (szFile, "%s\\wolf.oof", pszFolder);
else if (nModel == 110)
	sprintf (szFile, "%s\\phantomxl.oof", pszFolder);
else
	sprintf (szFile, "%s\\model%d.oof", pszFolder, nModel);

if (!fp.Open (szFile, "rb")) 
	return 0;
	
m_folder = pszFolder;

nIndent = 0;

if (!fp.Read (fileId, sizeof (fileId), 1)) {
	fp.Close ();
	return 0;
	}
if (strncmp (fileId, "PSPO", 4)) {
	fp.Close ();
	return 0;
	}
Init ();

m_nModel = nModel;
#ifdef _DEBUG
if (nModel == nDbgModel)
	nDbgModel = nDbgModel;
#endif

m_nVersion = OOF_ReadInt (fp, "nVersion");
if (m_nVersion >= 2100)
	m_nFlags |= OOF_PMF_LIGHTMAP_RES;
if (m_nVersion >= 22) {
	bTimed = 1;
	m_nFlags |= OOF_PMF_TIMED;
	m_frameInfo.m_nFirstFrame = 0;
	m_frameInfo.m_nLastFrame = 0;
	}
nSubModels = 0;

while (!fp.EoF ()) {
	char chunkID [4];

	if (!fp.Read (chunkID, sizeof (chunkID), 1)) {
		fp.Close ();
		return 0;
		}
	nLength = OOF_ReadInt (fp, "nLength");
	switch (ListType (chunkID)) {
		case 0:
			if (!ReadTextures (fp)) {
				Destroy ();
				return 0;
				}
			break;

		case 1:
			if (!ReadInfo (fp)) {
				Destroy ();
				return 0;
				}
			break;

		case 2:
			if (!m_subModels [nSubModels].Read (fp, this, 1)) {
				Destroy ();
				return 0;
				}
			nSubModels++;
			break;

		case 3:
			if (!m_gunPoints.Read (fp, m_nVersion >= 1908, MAX_GUNS)) {
				Destroy ();
				return 0;
				}
			break;

		case 4:
			if (!m_specialPoints.Read (fp)) {
				Destroy ();
				return 0;
				}
			break;

		case 5:
			if (!m_attachPoints.Read (fp)) {
				Destroy ();
				return 0;
				}
			break;

		case 6:
			nFrames = m_frameInfo.m_nFrames;
			if (!bTimed)
				m_frameInfo.m_nFrames = OOF_ReadInt (fp, "nFrames");
			for (i = 0; i < m_nSubModels; i++)
				if (!m_subModels [i].m_posAnim.Read (fp, this, bTimed)) {
					Destroy ();
					return 0;
					}
			if (m_frameInfo.m_nFrames < nFrames)
				m_frameInfo.m_nFrames = nFrames;
			break;

		case 7:
		case 8:
			nFrames = m_frameInfo.m_nFrames;
			if (!bTimed)
				m_frameInfo.m_nFrames = OOF_ReadInt (fp, "nFrames");
			for (i = 0; i < m_nSubModels; i++)
				if (!m_subModels [i].m_rotAnim.Read (fp, this, bTimed)) {
					Destroy ();
					return 0;
					}
			if (m_frameInfo.m_nFrames < nFrames)
				m_frameInfo.m_nFrames = nFrames;
			break;

		case 9:
			if (!m_armament.Read (fp)) {
				Destroy ();
				return 0;
				}
			break;

		case 10:
			if (!m_attachPoints.ReadNormals (fp)) {
				Destroy ();
				return 0;
				}
			break;

		default:
			fp.Seek (nLength, SEEK_CUR);
			break;
		}
	}
fp.Close ();
ConfigureSubModels ();
BuildAnimMatrices ();
AssignChildren ();
LinkBatteries ();
BuildPosTickRemapList ();
BuildRotTickRemapList ();
return 1;
}

//------------------------------------------------------------------------------
//eof

