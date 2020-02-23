
#include "types.h"
#include "mine.h"
#include "dle-xp-res.h"
#include "dle-xp.h"
#include "VertexManager.h"

tUVL defaultUVLs [4] = {
	{0.0, 0.0, (ushort) DEFAULT_LIGHTING},
	{0.0, 1.0, (ushort) DEFAULT_LIGHTING},
	{-1.0, 1.0, (ushort) DEFAULT_LIGHTING},
	{-1.0, 0.0, (ushort) DEFAULT_LIGHTING}
};

short CSide::vertexCounts [4] = {4, 3, 2, 1};
short CSide::faceCounts [4] = {2, 1, 0, 0};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void CUVL::Rotate (double angle)
{
if ((angle != 0.0) && (u != 0.0) || (v != 0.0)) {
	double radius = Mag ();
	// add rotation
	double a = atan3 (v, u) - angle;	
	// convert back to rectangular coordinates
	u = radius * cos (a);
	v = radius * sin (a);
	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void CSide::Read (CFileManager* fp, bool bTextured)
{
if (DLE.LevelVersion () > 24) {
	for (int i = 0; i < 4; i++)
		m_vertexIdIndex [i] = fp->ReadUByte ();
	}
if (bTextured) {
	m_info.nBaseTex = fp->ReadInt16 ();
	if (BaseTex () & 0x8000) {
		m_info.nOvlTex = fp->ReadInt16 ();
#if 1 //DBG
		if ((m_info.nOvlTex & TEXTURE_MASK) > textureManager.MaxTextures ())
			m_info.nOvlTex = 0;
#endif
		}
	else
		m_info.nOvlTex = 0;
	m_info.nBaseTex &= TEXTURE_MASK;
	if (m_info.nBaseTex > textureManager.MaxTextures ())
		m_info.nBaseTex = 0;
	for (int i = 0; i < 4; i++)
		m_info.uvls [i].Read (fp);
	}
else {
	ResetTextures ();
	}
DetectShape ();
}

// -----------------------------------------------------------------------------

void CSide::Write (CFileManager* fp, bool bTextured)
{
if (DLE.LevelVersion () > 24) {
	for (int i = 0; i < 4; i++)
		fp->Write (m_vertexIdIndex [i]);
	}
if (bTextured) {
	if (m_info.nOvlTex == 0)
		fp->Write (m_info.nBaseTex);
	else {
		fp->WriteInt16 (m_info.nBaseTex | 0x8000);
		fp->Write (m_info.nOvlTex);
		}
	for (int i = 0; i < 4; i++)
		m_info.uvls [i].Write (fp);
	}
}

// -----------------------------------------------------------------------------

void CSide::Setup (short nSide)
{
Reset (nSide);
}

// ----------------------------------------------------------------------------- 

void CSide::LoadTextures (void)
{
textureManager.Load (BaseTex (), OvlTex (0));
}

// -----------------------------------------------------------------------------

bool CSide::SetTextures (int nBaseTex, int nOvlTex)
{
	bool bChange = false;

if ((nBaseTex >= 0) && (nOvlTex & TEXTURE_MASK) == nBaseTex)
   nOvlTex = 0; 
if ((nBaseTex >= 0) && (nBaseTex != BaseTex ())) {
	m_info.nBaseTex = nBaseTex; 
	if (nBaseTex == (m_info.nOvlTex & TEXTURE_MASK)) {
		m_info.nOvlTex = 0; 
		}
	bChange = true; 
	}
if (nOvlTex >= 0) {
	if ((nOvlTex & TEXTURE_MASK) == m_info.nBaseTex)
		m_info.nOvlTex = 0; 
	else if (nOvlTex) {
		m_info.nOvlTex &= ALIGNMENT_MASK;	//preserve light settings
		m_info.nOvlTex |= nOvlTex; 
		}
	else
		m_info.nOvlTex = 0; 
	bChange = true; 
	}
if (bChange)
	LoadTextures ();
return bChange;
}

// -----------------------------------------------------------------------------

void CSide::GetTextures (short &nBaseTex, short &nOvlTex) _const_
{
nBaseTex = BaseTex ();
nOvlTex = OvlTex (0);
}

// -----------------------------------------------------------------------------

void CSide::InitUVL (short nTexture)
{
uint scale = (uint) textureManager.Scale (DLE.FileType (), nTexture);
for (int i = 0; i < 4; i++) {
	m_info.uvls [i].u = defaultUVLs [i].u / scale;
	m_info.uvls [i].v = defaultUVLs [i].v / scale;
	m_info.uvls [i].l = defaultUVLs [i].l;
	}
}

// -----------------------------------------------------------------------------

CSegment _const_ * CSide::Child (void) _const_
{ 
return ((short) m_info.nChild < 0) ? null: segmentManager.Segment (m_info.nChild); 
}

// -----------------------------------------------------------------------------

CWall _const_ * CSide::Wall (void) _const_
{ 
return wallManager.Wall (m_info.nWall); 
}

// -----------------------------------------------------------------------------

CTrigger _const_ * CSide::Trigger (void) _const_
{ 
CWall* pWall = wallManager.Wall (m_info.nWall); 
return (pWall == null) ? null : pWall->Trigger ();
}

// -----------------------------------------------------------------------------

bool CSide::IsVisible (void)
{
return (Shape () <= SIDE_SHAPE_TRIANGLE) && (((short) m_info.nChild == -1) || wallManager.IsVisible (m_info.nWall));
}

// -----------------------------------------------------------------------------

bool CSide::UpdateChild (short nOldChild, short nNewChild)
{
if (m_info.nChild != nOldChild)
	return false;
m_info.nChild = nNewChild;
return true;
}

// -----------------------------------------------------------------------------

void CSide::DeleteWall (void)
{
if ((m_info.nWall >= 0) && (m_info.nWall < NO_WALL)) {
	wallManager.Delete (m_info.nWall);
	m_info.nWall = NO_WALL;
	}
}

// -----------------------------------------------------------------------------

short CSide::FindVertexIdIndex (ubyte nIndex)
{
for (short i = 0, j = VertexCount (); i < j; i++)
	if (m_vertexIdIndex [i] == nIndex)
		return i;
return -1;
}

// -----------------------------------------------------------------------------

bool CSide::GetEdgeIndices (short nLine, short& i1, short& i2)
{
short n = VertexCount ();
if (n < 2)
	return false;
i1 = m_vertexIdIndex [nLine % n];
i2 = m_vertexIdIndex [(nLine + 1) % n];
return true;
}

// -----------------------------------------------------------------------------

bool CSide::HasVertex (ubyte nIndex)
{
for (int i = 0, j = VertexCount (); i < j; i++)
	if (m_vertexIdIndex [i] == nIndex)
		return true;
return false;
}

// -----------------------------------------------------------------------------

ubyte CSide::DetectShape (void)
{
int i;
for (i = 0; i < 4; i++)
	if (m_vertexIdIndex [i] == 0xff)
		break;
return m_nShape = 4 - i;
}

// -----------------------------------------------------------------------------

void CSide::ResetTextures (void)
{
m_info.nBaseTex = 0; 
m_info.nOvlTex = 0; 
CUVL* uvls = m_info.uvls;
double scale = textureManager.Scale (DLE.FileType (), BaseTex ());
for (int i = 0; i < 4; i++, uvls++) {
	uvls->u = defaultUVLs [i].u / scale; 
	uvls->v = defaultUVLs [i].v / scale; 
	uvls->l = (ushort) DEFAULT_LIGHTING; 
	}
memset (m_info.uvlDeltas, 0, sizeof (m_info.uvlDeltas));
}

// -----------------------------------------------------------------------------

void CSide::Reset (short nSide)
{
ResetTextures ();
m_nTag = 0;
m_nPoint = 0;
m_info.nChild = 0xFFFF;
m_info.nWall = NO_WALL;
if (nSide >= 0) {
	memcpy (m_vertexIdIndex, sideVertexTable [nSide], 4);
	m_nShape = SIDE_SHAPE_RECTANGLE;
	}
}

// -----------------------------------------------------------------------------

void CSide::UpdateTexCoords (ubyte nPoint, ubyte nOtherPoint, ubyte nLine, CVertex& v1, CVertex& v2)
{
double l0 = Distance (v1, v2);
double l1 = Distance (v1 + v1.m_delta, v2 + v2.m_delta);
if (l0 == l1)
	return;
CDoubleVector v = v1 - v2;
v.Normalize ();
double d = Dot (v, v1.Delta ())/* * v1.Delta ().Mag ()*/;
if (d == 0.0)
	return;
double scale = (l0 + d) / l0;
if (nLine & 1) {
	double d = (m_info.uvls [nPoint].u - m_info.uvls [nOtherPoint].u) * scale;
	m_info.uvlDeltas [nPoint].u = d + m_info.uvls [nOtherPoint].u - m_info.uvls [nPoint].u;
	}
else {
	double d = (m_info.uvls [nPoint].v - m_info.uvls [nOtherPoint].v) * scale;
	m_info.uvlDeltas [nPoint].v = d + m_info.uvls [nOtherPoint].v - m_info.uvls [nPoint].v;
	}
}

// -----------------------------------------------------------------------------

CVertex& CSide::Vertex (ushort* vertexIds, ubyte nIndex)
{
return vertexManager [vertexIds [m_vertexIdIndex [(nIndex + VertexCount ()) % VertexCount ()]]];
}

// -----------------------------------------------------------------------------

void CSide::UpdateTexCoords (ubyte nPoint, ushort* vertexIds)
{
CVertex& v = Vertex (vertexIds, nPoint);
ubyte n = ubyte (VertexCount ());
CDoubleVector vTurn = Vertex (vertexIds, nPoint + 1) - Vertex (vertexIds, nPoint - 1);
CDoubleVector vParallel = Vertex (vertexIds, nPoint + 2) - v;
vTurn.Normalize ();
double angle = Dot (vTurn, vParallel);
vParallel -= vTurn * angle;
vParallel.Normalize ();
ubyte nPredPoint = (nPoint - 1 + n) % n;
ubyte nSuccPoint = (nPoint + 1) % n;
CUVL uv1 = m_info.uvls [nSuccPoint] - m_info.uvls [nPredPoint];
uv1.Normalize ();
CUVL uv2 = CUVL (-uv1.v, uv1.u);
double a1 = Dot (vTurn, v.m_delta) / 20.0;
double a2 = Dot (vParallel, v.m_delta) / 20.0;
m_info.uvlDeltas [nPoint] = CUVL (a1 * uv1.u + a2 * uv2.u, a1 * uv1.v + a2 * uv2.v);
m_info.uvlDeltas [nPoint].l = 1;
}

// -----------------------------------------------------------------------------

void CSide::FindAndUpdateTexCoords (ubyte nIdIndex, ushort* nVertexIds, bool bMove)
{
ubyte h = ubyte (VertexCount ()); 
for (ubyte nPoint = 0; nPoint < h; nPoint++) {
	if (m_vertexIdIndex [nPoint] == nIdIndex) {
		if (bMove)
			MoveTexCoords ();
		else
			UpdateTexCoords (nPoint, nVertexIds);
		return;
		}
	}
}

// -----------------------------------------------------------------------------

void CSide::MoveTexCoords (void)
{
for (int i = 0; i < 4; i++) {
	if (m_info.uvlDeltas [i].l) {
		m_info.uvls [i].u += m_info.uvlDeltas [i].u;
		m_info.uvls [i].v += m_info.uvlDeltas [i].v;
		m_info.uvlDeltas [i].l = 0;
		}
	}
}

// -----------------------------------------------------------------------------

CDoubleVector& CSide::ComputeNormals (ushort* vertexIds, CDoubleVector& vCenter, bool bView)
{
if (Shape () > SIDE_SHAPE_TRIANGLE)
	m_vNormal [2].Clear ();
else if (bView) {
	CDoubleVector& v0 = vertexManager [vertexIds [m_vertexIdIndex [0]]].m_view;
	CDoubleVector v = vCenter - v0;
	::Normal (m_vNormal [0], v0, vertexManager [vertexIds [m_vertexIdIndex [1]]].m_view, vertexManager [vertexIds [m_vertexIdIndex [2]]].m_view);
	if (Dot (m_vNormal [0], v) < 0.0) // make sure normal points into the segment
		m_vNormal [0].Negate ();

	if (VertexCount () < 4) 
		m_vNormal [1] = m_vNormal [0];
	else {
		::Normal (m_vNormal [1], v0, vertexManager [vertexIds [m_vertexIdIndex [2]]].m_view, vertexManager [vertexIds [m_vertexIdIndex [3]]].m_view);
		if (Dot (m_vNormal [1], v) < 0.0)
			m_vNormal [1].Negate ();
		}
	}
else {
	CDoubleVector& v0 = vertexManager [vertexIds [m_vertexIdIndex [0]]];
	CDoubleVector v = vCenter - v0;
	::Normal (m_vNormal [0], v0, vertexManager [vertexIds [m_vertexIdIndex [1]]], vertexManager [vertexIds [m_vertexIdIndex [2]]]);
	if (Dot (m_vNormal [0], v) < 0.0) // make sure normal points into the segment
		m_vNormal [0].Negate ();

	if (VertexCount () < 4) 
		m_vNormal [1] = m_vNormal [0];
	else {
		::Normal (m_vNormal [1], v0, vertexManager [vertexIds [m_vertexIdIndex [2]]], vertexManager [vertexIds [m_vertexIdIndex [3]]]);
		if (Dot (m_vNormal [1], v) < 0.0)
			m_vNormal [1].Negate ();
		}
	}
m_vNormal [2] = Average (m_vNormal [0], m_vNormal [1]);
return m_vNormal [2];
}

// -----------------------------------------------------------------------------

double CSide::LineHitsFace (CVertex* p1, CVertex* p2, ushort* vertexIds, double maxDist, bool bView)
{
CDoubleVector* vertices [4];
short nVertices = VertexCount ();

if (bView) {
	for (short nVertex = 0; nVertex < nVertices; nVertex++) {
		vertices [nVertex] = &vertexManager [vertexIds [m_vertexIdIndex [nVertex]]].m_view;
		if (*p2 == *vertices [nVertex])
			return Distance (*p1, *p2);
		}
	}
else {
	for (short nVertex = 0; nVertex < nVertices; nVertex++) {
		vertices [nVertex] = &vertexManager [vertexIds [m_vertexIdIndex [nVertex]]];
		if (*p2 == *vertices [nVertex])
			return Distance (*p1, *p2);
		}
	}

CDoubleVector intersection;
// Does the ray hit the first triangle?
if ((0 < PlaneLineIntersection (intersection, vertices [0], &m_vNormal [0], p1, p2)) &&
	PointIsInTriangle3D (intersection, *vertices [0], *vertices [1], *vertices [2]))
	;
// Does it hit the second (if there is one)?
else if ((nVertices > 3) && (0 < PlaneLineIntersection (intersection, vertices [2], &m_vNormal [1], p1, p2)) &&
		 PointIsInTriangle3D (intersection, *vertices [0], *vertices [2], *vertices [3]))
	;
// If not, the line doesn't hit the face
else
	return -1.0;

double d = Distance (intersection, *p1);
if (d > maxDist)
	return -1.0;
return d;
}

//--------------------------------------------------------------------------

short CSide::IsSelected (CRect& viewport, long xMouse, long yMouse, ushort vertexIds []) 
{
if (Shape () > SIDE_SHAPE_TRIANGLE)
	return 0;

CLongVector mousePos;
mousePos.x = xMouse;
mousePos.y = yMouse;
mousePos.z = 0;

CVertex sideVerts [4];
int h = VertexCount ();
for (int j = 0; j < h; j++) {
	sideVerts [j] = vertexManager [vertexIds [m_vertexIdIndex [j]]];
	sideVerts [j].m_screen.z = 0;
	}
for (int j = 0; j < h; j++) {
	int x = sideVerts [j].m_screen.x;
	int y = sideVerts [j].m_screen.y;
	if ((x < viewport.left) || (x > viewport.right) || (y < viewport.top) || (y > viewport.bottom)) 
		sideVerts [j].m_screen.z = 0;
	else if (sideVerts [j].m_view.v.z < 0.0)
		sideVerts [j].m_screen.z = 0;
	}
if (PointIsInTriangle2D (mousePos, sideVerts [0].m_screen, sideVerts [1].m_screen, sideVerts [2].m_screen))
	return 1;
if ((h > 3) && PointIsInTriangle2D (mousePos, sideVerts [0].m_screen, sideVerts [2].m_screen, sideVerts [3].m_screen)) 
	return 1;
return 0;
}

// -----------------------------------------------------------------------------

short CSide::NearestEdge (CRect& viewport, long xMouse, long yMouse, ushort vertexIds [], double& minDist) 
{
CDoubleVector mousePos (double (xMouse), double (yMouse), 0.0);
short nNearestEdge = -1, nEdges = VertexCount ();
CVertex* v1, * v2 = &vertexManager [vertexIds [m_vertexIdIndex [0]]];
for (short nEdge = 1; nEdge <= nEdges; nEdge++) {
	v1 = v2;
	if (!v1->InRange (viewport.right, viewport.bottom, 1))
		continue;
	v2 = &vertexManager [vertexIds [m_vertexIdIndex [nEdge % nEdges]]];
	if (!v2->InRange (viewport.right, viewport.bottom, 1))
		continue;
	v1->m_proj.v.z = v2->m_proj.v.z = 0.0;
	double dist = PointLineDistance (mousePos, v1->m_proj, v2->m_proj);
	if (minDist > dist) {
		minDist = dist;
		nNearestEdge = nEdge - 1;
		}
	}
return nNearestEdge;
}

// -----------------------------------------------------------------------------

short CSide::Edge (short i1, short i2)
{
for (short i = 0, j = VertexCount (); i < j; i++) {
	if (m_vertexIdIndex [i] == i1) {
		if (m_vertexIdIndex [(i + 1) % j] == i2) 
			return i;
		short h = (i - 1 + j) % j;
		if (m_vertexIdIndex [h] == i2) 
			return h;
		return -1;
		}
	}
return -1;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//eof side.cpp