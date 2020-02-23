// Segment.cpp

#include "types.h"
#include "mine.h"
#include "dle-xp-res.h"
#include "dle-xp.h"

extern short nDbgSeg, nDbgSide;
extern int nDbgVertex;

// -----------------------------------------------------------------------------
// define points for a given side 
ubyte sideVertexTable [6][4] = {
	{7,6,2,3},
	{0,4,7,3},
	{0,1,5,4},
	{2,6,5,1},
	{4,5,6,7},
	{3,2,1,0} 
};

// define opposite side of a given side 
ubyte oppSideTable [6] = {2,3,0,1,5,4};

// define points for the opposite side of a given side 
ubyte oppSideVertexTable [6][4] = {
	{4,5,1,0},
	{1,5,6,2},
	{3,2,6,7},
	{3,7,4,0},
	{0,1,2,3},
	{7,6,5,4} 
};

// each side has 4 children (ordered by side's line number)
int sideChildTable[6][4] = {
  {4,3,5,1},
  {2,4,0,5},
  {5,3,4,1},
  {0,4,2,5},
  {2,3,0,1},
  {0,3,2,1}
};

// define 2 points for a given line 
ubyte edgeVertexTable [12][2] = {
	{0,1},
	{1,2},
	{2,3},
	{3,0},
	{0,4},
	{4,5},
	{5,6},
	{6,7},
	{7,4},
	{1,5},
	{2,6},
	{3,7}
};

// edges connected to a vertex
ubyte vertexEdgeTable [8][3] = {
	{0,4,5},
	{0,1,9},
	{1,2,10},
	{2,3,11},
	{4,5,8},
	{5,6,9},
	{6,7,10},
	{7,8,11}
};

// edges for a given side 
ubyte sideEdgeTable [6][4] = {
	{7,10,2,11},
	{4,8,11,3},
	{0,9,5,4},
	{10,6,9,1},
	{5,6,7,8},
	{2,1,0,3} 
};

// sides adjacent to given edge
ubyte edgeSideTable [12][2] = { 
	{2,5},
	{3,5},
	{0,5},
	{1,5},
	{1,2},
	{2,4},
	{4,5},
	{0,4},
	{1,4},
	{2,3},
	{0,3},
	{0,1}
};

// the three adjacent points of a segment for each corner of the segment 
ubyte adjacentPointTable [8][3] = {
	{1,3,4},
	{2,0,5},
	{3,1,6},
	{0,2,7},
	{7,5,0},
	{4,6,1},
	{5,7,2},
	{6,4,3}
};

// side numbers for each point (3 sides touch each point)
ubyte adjacentSideTable [8][3] = {
    {1,2,5},
    {2,3,5},
    {0,3,5},
    {0,1,5},
    {1,2,4},
    {2,3,4},
    {0,3,4},
    {0,1,4}
};

// CUVL corner for adjacentSideTable above
ubyte pointCornerTable [8][3] = {
    {0,0,3},
    {1,3,2},
    {2,0,1},
    {3,3,0},
    {1,3,0},
    {2,2,1},
    {1,1,2},
    {0,2,3} 
};

// -----------------------------------------------------------------------------

static ubyte segFuncFromType [] = {
	SEGMENT_FUNC_NONE,
	SEGMENT_FUNC_PRODUCER,
	SEGMENT_FUNC_REPAIRCEN,
	SEGMENT_FUNC_REACTOR,
	SEGMENT_FUNC_ROBOTMAKER,
	SEGMENT_FUNC_GOAL_BLUE,
	SEGMENT_FUNC_GOAL_RED,
	SEGMENT_FUNC_NONE,
	SEGMENT_FUNC_NONE,
	SEGMENT_FUNC_TEAM_BLUE,
	SEGMENT_FUNC_TEAM_RED,
	SEGMENT_FUNC_SPEEDBOOST,
	SEGMENT_FUNC_NONE,
	SEGMENT_FUNC_NONE,
	SEGMENT_FUNC_SKYBOX,
	SEGMENT_FUNC_EQUIPMAKER,
	SEGMENT_FUNC_NONE
	};

static ubyte segPropsFromType [] = {
	SEGMENT_PROP_NONE,
	SEGMENT_PROP_NONE,
	SEGMENT_PROP_NONE,
	SEGMENT_PROP_NONE,
	SEGMENT_PROP_NONE,
	SEGMENT_PROP_NONE,
	SEGMENT_PROP_NONE,
	SEGMENT_PROP_WATER,
	SEGMENT_PROP_LAVA,
	SEGMENT_PROP_NONE,
	SEGMENT_PROP_NONE,
	SEGMENT_PROP_NONE,
	SEGMENT_PROP_BLOCKED,
	SEGMENT_PROP_NODAMAGE,
	SEGMENT_PROP_BLOCKED,
	SEGMENT_PROP_NONE,
	SEGMENT_PROP_SELF_ILLUMINATE
	};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void CSegment::Upgrade (void)
{
if ((m_info.function == 2) && DLE.LevelType () < 2) // standard level repair center type - not supported by standard Descent
	m_info.function = 0;
m_info.props = segPropsFromType [m_info.function];
#ifdef _DEBUG
if (m_info.props)
	m_info.props = m_info.props;
#endif
m_info.function = segFuncFromType [m_info.function];
m_info.damage [0] =
m_info.damage [1] = 0;
}

// -----------------------------------------------------------------------------

ubyte CSegment::ReadWalls (CFileManager* fp)
{
	ubyte wallFlags = ubyte (fp->ReadSByte ());
	int	i;

for (i = 0; i < MAX_SIDES_PER_SEGMENT; i++) 
	if (m_info.wallFlags & (1 << i)) 
		m_sides [i].m_info.nWall = (DLE.LevelVersion () >= 13) ? fp->ReadUInt16 () : (ushort) fp->ReadUByte ();
return wallFlags;
}

// -----------------------------------------------------------------------------

void CSegment::ReadExtras (CFileManager* fp, bool bExtras)
{
if (bExtras) {
	m_info.function = fp->ReadSByte ();
	if ((DLE.LevelType () == 2) && (DLE.LevelVersion () > 23)) {
		m_info.nProducer = fp->ReadInt16 ();
		m_info.value = fp->ReadInt16 ();
		}
	else {
		m_info.nProducer = fp->ReadSByte ();
		m_info.value = fp->ReadSByte ();
		}
	fp->ReadSByte ();
	}
else {
	m_info.function = 0;
	m_info.nProducer = -1;
	m_info.value = 0;
	}
if (DLE.LevelType ()) {
	if (DLE.LevelVersion () < 20)
		Upgrade ();
	else {
		m_info.props = fp->ReadSByte ();
		m_info.damage [0] = fp->ReadInt16 ();
		m_info.damage [1] = fp->ReadInt16 ();
		}
	}
m_info.staticLight = (DLE.LevelType () == 0) ? ((int) fp->ReadUInt16 ()) << 4 : fp->ReadInt32 ();
}

// -----------------------------------------------------------------------------

void CSegment::Read (CFileManager* fp)
{
	int	i;

if (DLE.LevelType () == 2) {
	m_info.owner = fp->ReadSByte ();
	m_info.group = fp->ReadSByte ();
	}
else {
	m_info.owner = -1;
	m_info.group = -1;
	}

for (i = 0; i < MAX_SIDES_PER_SEGMENT; i++)
	m_sides [i].Reset (i);
// read in child mask (1 ubyte)
m_info.childFlags = ubyte (fp->ReadSByte ());

// read 0 to 6 children (0 to 12 bytes)
for (i = 0; i < MAX_SIDES_PER_SEGMENT; i++)
	m_sides [i].m_info.nChild = (m_info.childFlags & (1 << i)) ? fp->ReadInt16 () : -1;

// read vertex numbers (16 bytes)
m_nShape = 0;
for (i = 0; i < MAX_VERTICES_PER_SEGMENT; i++)
	if (MAX_VERTEX < (m_info.vertexIds [i] = fp->ReadUInt16 ()))
		m_nShape++;

if (DLE.LevelType () == 0)
	ReadExtras (fp, (m_info.childFlags & (1 << MAX_SIDES_PER_SEGMENT)) != 0);

// read the wall bit mask
m_info.wallFlags = ubyte (fp->ReadSByte ());

// read in wall numbers (0 to 6 bytes)
for (i = 0; i < MAX_SIDES_PER_SEGMENT; i++) {
	m_sides [i].m_info.nWall = (m_info.wallFlags & (1 << i)) 
										? (DLE.LevelVersion () < 13) 
											? (ushort) fp->ReadUByte () 
											: fp->ReadUInt16 ()
										: NO_WALL;
	}

// read in textures and uvls (0 to 60 bytes)
m_nShape = 0;
for (i = 0; i < MAX_SIDES_PER_SEGMENT; i++) {
	m_sides [i].Read (fp, (ChildId (i) == -1) || ((m_info.wallFlags & (1 << i)) != 0));
	if (m_sides [i].Shape ())
		m_nShape++;
	}
}

// -----------------------------------------------------------------------------

ubyte CSegment::WriteWalls (CFileManager* fp)
{
	int	i;

m_info.wallFlags = 0;
for (i = 0; i < MAX_SIDES_PER_SEGMENT; i++) {
	if (m_sides [i].m_info.nWall != NO_WALL) 
		m_info.wallFlags |= (1 << i);
	}
fp->Write (m_info.wallFlags);

// write wall numbers
for (i = 0; i < MAX_SIDES_PER_SEGMENT; i++) {
	if (m_info.wallFlags & (1 << i)) {
		CWall _const_ * pWall = m_sides [i].Wall ();
		if (DLE.LevelVersion () >= 13)
			fp->WriteUInt16 ((ushort) const_cast<CWall*>(pWall)->Index ());
		else
			fp->WriteSByte ((sbyte) const_cast<CWall*>(pWall)->Index ());
		}
	}
return m_info.wallFlags;
}

// -----------------------------------------------------------------------------

void CSegment::WriteExtras (CFileManager* fp, bool bExtras)
{
if (bExtras) {
	fp->Write (m_info.function);
	if (DLE.LevelType () == 2) {
		fp->Write (m_info.nProducer);
		fp->Write (m_info.value);
		}
	else {
		sbyte h = (sbyte) m_info.nProducer;
		fp->Write (h);
		h = (sbyte) m_info.value;
		fp->Write (h);
		}
	fp->WriteByte (0); // s2Flags
	}
if (DLE.LevelType () == 2) {
	fp->Write (m_info.props);
	fp->Write (m_info.damage [0]);
	fp->Write (m_info.damage [1]);
	}
if (DLE.LevelType () == 0)
	fp->WriteUInt16 ((ushort) (m_info.staticLight >> 4));
else
	fp->Write (m_info.staticLight);
}

// -----------------------------------------------------------------------------

void CSegment::Write (CFileManager* fp)
{
	int	i;

if (DLE.LevelType () == 2) {
	fp->Write (m_info.owner);
	fp->Write (m_info.group);
	}

#if 1
m_info.childFlags = 0;
for (i = 0; i < MAX_SIDES_PER_SEGMENT; i++) {
	if (ChildId (i) != -1) {
		m_info.childFlags |= (1 << i);
		}
	}
if (DLE.LevelType () == 0) {
	if (m_info.function != 0) { // if this is a special segment
		m_info.childFlags |= (1 << MAX_SIDES_PER_SEGMENT);
		}
	}
#endif
fp->Write (m_info.childFlags);

// write children numbers (0 to 6 bytes)
for (i = 0; i < MAX_SIDES_PER_SEGMENT; i++) 
	if (m_info.childFlags & (1 << i)) 
		fp->WriteInt16 (ChildId (i));

// write vertex numbers (16 bytes)
for (i = 0; i < MAX_VERTICES_PER_SEGMENT; i++)
	fp->WriteUInt16 (m_info.vertexIds [i]);

// write special info (0 to 4 bytes)
if ((m_info.function == SEGMENT_FUNC_ROBOTMAKER) && (m_info.nProducer == -1)) {
	m_info.function = SEGMENT_FUNC_NONE;
	m_info.value = 0;
	m_info.childFlags &= ~(1 << MAX_SIDES_PER_SEGMENT);
	}
if (DLE.LevelType () == 0)
	WriteExtras (fp, (m_info.childFlags & (1 << MAX_SIDES_PER_SEGMENT)) != 0);

// calculate wall bit mask
WriteWalls (fp);
for (i = 0; i < MAX_SIDES_PER_SEGMENT; i++)  
	m_sides [i].Write (fp, (ChildId (i) == -1) || (m_info.wallFlags & (1 << i)));
}

// -----------------------------------------------------------------------------

void CSegment::Setup (void)
{
Reset ();
}

// -----------------------------------------------------------------------------

#if 0

void CSegment::SetUV (short nSide, double x, double y)
{
	CDoubleVector	A [4], G [4]; 
	// triangle 1 - (0, 1, 2)
	CDoubleVector	B1 [3], C1 [3], D1 [3], E1 [3]; 
	// triangle 2 - (0, 2, 3)
	CDoubleVector	B2 [3], C2 [3], D2 [3], E2 [3], F2 [3]; 
	int				i; 
	double			angle1, sinAngle1, cosAngle1; 
	double			angle2, sinAngle2, cosAngle2; 
	CSide*			pSide = Side (nSide);

// for testing, x is used to tell how far to convert vector
// 0, 1, 2, 3 represent B, C, D, E coordinate transformations

// copy side's four points into A

for (i = 0; i < 4; i++)
	A [i] = CDoubleVector (*vertexManager.Vertex (m_info.vertexIds [pSide->VertexIdIndex (i)])); 

// subtract point 0 from all points in A to form B points
for (i = 0; i < 3; i++) {
	B1 [i] = A [i] - A [0]; 
	B2 [i] = A [i ? i + 1 : i] - A [0];
	}

// calculate angle to put point 1 in x - y plane by spinning on x - axis
// then rotate B points on x - axis to form C points.
// check to see if on x - axis already
angle1 = atan3 (B1 [1].v.z, B1 [1].v.y); 
sinAngle1 = sin (angle1);
cosAngle1 = cos (angle1);
angle2 = atan3 (B2 [1].v.z, B2 [1].v.y); 
sinAngle2 = sin (angle2);
cosAngle2 = cos (angle2);
for (i = 0; i < 3; i++) {
	C1 [i].Set (B1 [i].v.x, B1 [i].v.y * cosAngle1 + B1 [i].v.z * sinAngle1, -B1 [i].v.y * sinAngle1 + B1 [i].v.z * cosAngle1); 
	C2 [i].Set (B2 [i].v.x, B2 [i].v.y * cosAngle2 + B2 [i].v.z * sinAngle2, -B2 [i].v.y * sinAngle2 + B2 [i].v.z * cosAngle2); 
	}

#if UV_DEBUG
if (abs((int)C [1].z) != 0) {
	sprintf_s (message, sizeof (message),  "SetUV: point 1 not in x/y plane\n(%f); angle = %f", (float)C [1].z, (float)angle); 
	DEBUGMSG (message); 
	}
#endif

// calculate angle to put point 1 on x axis by spinning on z - axis
// then rotate C points on z - axis to form D points
// check to see if on z - axis already
angle1 = atan3 (C1 [1].v.y, C1 [1].v.x); 
sinAngle1 = sin (angle1);
cosAngle1 = cos (angle1);
angle2 = atan3 (C2 [1].v.y, C2 [1].v.x); 
sinAngle2 = sin (angle2);
cosAngle2 = cos (angle2);
for (i = 0; i < 3; i++) {
	D1 [i].Set (C1 [i].v.x * cosAngle1 + C1 [i].v.y * sinAngle1, -C1 [i].v.x * sinAngle1 + C1 [i].v.y * cosAngle1, C1 [i].v.z); 
	D2 [i].Set (C2 [i].v.x * cosAngle2 + C2 [i].v.y * sinAngle2, -C2 [i].v.x * sinAngle2 + C2 [i].v.y * cosAngle2, C2 [i].v.z); 
	}
#if UV_DEBUG
if (abs((int)D [1].y) != 0) {
	DEBUGMSG (" SetUV: Point 1 not in x axis"); 
	}
#endif

// calculate angle to put point 2 in x - y plane by spinning on x - axis
// the rotate D points on x - axis to form E points
// check to see if on x - axis already
angle1 = atan3 (D1 [2].v.z, D1 [2].v.y); 
sinAngle1 = sin (angle1);
cosAngle1 = cos (angle1);
angle2 = atan3 (D2 [2].v.z, D2 [2].v.y); 
sinAngle2 = sin (angle2);
cosAngle2 = cos (angle2);
for (i = 0; i < 3; i++) {
	E1 [i].Set (D1 [i].v.x, D1 [i].v.y * cosAngle1 + D1 [i].v.z * sinAngle1, -D1 [i].v.y * sinAngle1 + D1 [i].v.z * cosAngle1); 
	E2 [i].Set (D2 [i].v.x, D2 [i].v.y * cosAngle2 + D2 [i].v.z * sinAngle2, -D2 [i].v.y * sinAngle2 + D2 [i].v.z * cosAngle2); 
	}

// calculate angle to rotate triangle 2 0->2 vector (initially aligned with the x - axis) onto triangle 1 0->2 vector
// then rotate E2 points on z - axis to form F2 points
angle1 = -atan3 (E1 [2].v.y, E1 [2].v.x); 
sinAngle2 = sin (angle1);
cosAngle2 = cos (angle1);
for (i = 0; i < 3; i++) {
	F2 [i].Set (E2 [i].v.x * cosAngle2 + E2 [i].v.y * sinAngle2, -E2 [i].v.x * sinAngle2 + E2 [i].v.y * cosAngle2, E2 [i].v.z); 
	}

// finally, combine triangles and store in G
for (i = 0; i < 3; i++) {
	G [i] = E1 [i];
	}
G [3] = F2 [2];

// now points 0, 1, and 2 are in x - y plane and point 3 is close enough.
// set v to x axis and u to negative u axis to match default (u, v)
// (remember to scale by dividing by 640)
CUVL *uvls = m_sides [nSide].m_info.uvls;
#if UV_DEBUG
switch (x) {
	case 0:
		for (i = 0; i < 4; i++) {
			uvls [i].v = (B [i].x/640); 
			uvls [i].u = - (B [i].y/640); 
			}
		break; 
	case 1:
		for (i = 0; i < 4; i++) {
			uvls [i].v = (C [i].x/640); 
			uvls [i].u = 0x400/10 - (C [i].y/640); 
			}
		break; 
	case 2:
		for (i = 0; i < 4; i++) {
			uvls [i].v = (D [i].x/640); 
			uvls [i].u = 2*0x400/10 - (D [i].y/640); 
			}
		break; 
	case 3:
		for (i = 0; i < 4; i++) {
			uvls [i].v = (E [i].x/640); 
			uvls [i].u = 3*0x400/10 - (E [i].y/640); 
			}
	break; 
	}
#else
undoManager.Begin (__FUNCTION__, udSegments); 
m_sides [nSide].LoadTextures ();
for (i = 0; i < 4; i++, uvls++) {
	uvls->v = (y + G [i].v.x) / 20.0; 
	uvls->u = (x - G [i].v.y) / 20.0; 
	}
undoManager.End (__FUNCTION__);
#endif
}

// -----------------------------------------------------------------------------

#else

void CSegment::SetUV (short nSide, double x, double y)
{
	CDoubleVector	A [4], C1 [3], C2 [3], D [4];
	CDoubleVector*	R;
	int				i; 
	double			angle; 
	CSide*			pSide = Side (nSide);

for (i = 0; i < pSide->VertexCount(); i++)
	A [i] = CDoubleVector (*vertexManager.Vertex (m_info.vertexIds [pSide->VertexIdIndex (i)])); 

switch (pSide->Shape ()) {
	case SIDE_SHAPE_TRIANGLE:
		{
			CDoubleVector B1 [] = {A [0], A [1], A [2]};
			GetTriangleUVs (C1, B1);
		}
		R = C1;
		break;

	case SIDE_SHAPE_RECTANGLE:
		if (Dot (A [3] - A [0], Normal (A [0], A [1], A [2])) > 0) {
			// Split on 0,2
			CDoubleVector B1 [] = {A [0], A [1], A [2]};
			CDoubleVector B2 [] = {A [0], A [2], A [3]};
			GetTriangleUVs (C1, B1);
			GetTriangleUVs (C2, B2);

			// Rotate triangle C2 UVs onto C1 UVs so points 0 and 2 match (only need to calculate last point)
			angle = -atan3 (-C1 [2].v.x, C1 [2].v.y);
			C2 [2].Rotate (C2 [0], CDoubleVector (0, 0, 1), angle);

			// Combine triangles
			D [0] = C1 [0], D [1] = C1 [1], D [2] = C1 [2], D [3] = C2 [2];
			}
		else {
			// Split on 1,3
			CDoubleVector B1 [] = {A [0], A [1], A [3]};
			CDoubleVector B2 [] = {A [3], A [1], A [2]};
			GetTriangleUVs (C1, B1);
			GetTriangleUVs (C2, B2);

			// Rotate and translate triangle C2 UVs onto C1 UVs so points 1 and 3 match
			angle = atan3 (C1 [1].v.x - C1 [2].v.x, C1 [1].v.y - C1 [2].v.y);
			C2 [2].Rotate (C2 [0], CDoubleVector (0, 0, 1), angle);
			C2 [2] += C1 [2];

			// Combine triangles
			D [0] = C1 [0], D [1] = C1 [1], D [2] = C2 [2], D [3] = C1 [2];
			}
		R = D;
		break;

	default:
		// No reset operation defined, just return
		return;
	}

// Scale, translate, store UV values
CUVL *uvls = m_sides [nSide].m_info.uvls;
undoManager.Begin (__FUNCTION__, udSegments); 
m_sides [nSide].LoadTextures ();
for (i = 0; i < pSide->VertexCount (); i++, uvls++) {
	uvls->u = (x + R [i].v.x) / 20.0; 
	uvls->v = (y + R [i].v.y) / 20.0; 
	}
undoManager.End (__FUNCTION__);
}

// -----------------------------------------------------------------------------

void CSegment::GetTriangleUVs (CDoubleVector (&triangleUVs) [3], const CDoubleVector (&triangleVecs) [3])
{
	CDoubleVector vec1, vec2, proj, rej;
	double projSF;

vec1 = triangleVecs [1] - triangleVecs [0];
vec2 = triangleVecs [2] - triangleVecs [0];
proj = vec1;
if (proj.Mag () == 0) // degenerate face but let's still handle it
	proj = CDoubleVector (0, 1, 0);
proj.Normalize ();
projSF = Dot (proj, vec2);
proj *= projSF;
rej = vec2 - proj;
triangleUVs [0] = CDoubleVector (0, 0, 0);
triangleUVs [1] = CDoubleVector (0, vec1.Mag (), 0);
triangleUVs [2] = CDoubleVector (-rej.Mag (), projSF, 0);
}

#endif

// -----------------------------------------------------------------------------

short CSegment::SetChild (short nSide, short nSegment) 
{
m_sides [nSide].m_info.nChild = nSegment;
if (nSegment == -1)
	m_info.childFlags &= ~(1 << nSide);
else
	m_info.childFlags |= (1 << nSide);
return nSegment;
}

// -----------------------------------------------------------------------------

bool CSegment::ReplaceChild (short nOldSeg, short nNewSeg) 
{
for (short nSide = 0; nSide < 6; nSide++) {
	if (m_sides [nSide].m_info.nChild == nOldSeg) {
		SetChild (nSide, nNewSeg);
		return true;
		}
	}
return false;
}

// -----------------------------------------------------------------------------

short CSegment::CommonVertices (short nOtherSeg, short nMaxVertices, ushort* vertices)
{
	CSegment* pOther = segmentManager.Segment (nOtherSeg);
	short nCommon = 0;

for (short i = 0; i < 8; i++) {
	ushort nVertex = VertexId (i);
	for (short j = 0; j < 8; j++) {
		if (nVertex == pOther->VertexId (j)) {
			if (vertices)
				vertices [nCommon] = nVertex;
			if (++nCommon == nMaxVertices)
				return nCommon;
			break;
			}
		}
	}
return nCommon;
}

// -----------------------------------------------------------------------------

short CSegment::FindSide (short nSide, short nVertices, ushort* vertices)
{
for (; nSide < 6; nSide++) {
	short nCommon = 0;
	for (int i = 0; i < nVertices; i++) {
		for (int j = 0; j < 4; j++) {
			if (VertexId (Side (nSide)->VertexIdIndex (j)) == vertices [i]) {
				if (++nCommon == nVertices)
					return nSide;
				}
			}
		}
	}		 
return -1;
}

// -----------------------------------------------------------------------------

short CSegment::CommonSide (short nSide, ushort* vertices)
{
ubyte* vertexIdIndex = Side (nSide)->m_vertexIdIndex;
short nCommon = 0;
for (short i = 0; i < 4; i++) {
	ushort nVertex = vertices [i];
	short j = 0;
	for (; j < 4; j++) {
		if (m_info.vertexIds [vertexIdIndex [j]] == nVertex) {
			if (++nCommon == 4)
				return 1;
			break;
			}
		}
	if (j == 4)
		return 0;
	}	
return 0;
}

// -----------------------------------------------------------------------------

short CSegment::CommonSides (short nOtherSeg, short& nOtherSide)
{
ushort vertices [4];
if (CommonVertices (nOtherSeg, 4, vertices) < 4)
	return -1;

short nSide = 0;
for (; nSide < 6; nSide++) 
	if (CommonSide (nSide, vertices))
		break;
if (nSide == 6)
	return -1;

CSegment* pOther = segmentManager.Segment (nOtherSeg);

for (nOtherSide = 0; nOtherSide < 6; nOtherSide++)
	if (pOther->CommonSide (nOtherSide, vertices))
		break;
if (nOtherSide == 6)
	return -1;
return nSide;
}

// -----------------------------------------------------------------------------
// return the index of the vertex index nIndex in segments's vertex index table

short CSegment::VertexIndex (short nVertexId)
{
for (int i = 0; i < 8; i++)
	if (m_info.vertexIds [i] == nVertexId)
		return i;
return -1;
}

// -----------------------------------------------------------------------------
// return the index of the vertex index nIndex in side's nSide vertex index index

short CSegment::SideVertexIndex (short nSide, ubyte nPoint)
{
return m_sides [nSide].FindVertexIdIndex (nPoint);
}

// -----------------------------------------------------------------------------

CVertex _const_ * CSegment::Vertex (ushort nVertex) _const_
{
return vertexManager.Vertex (m_info.vertexIds [nVertex]);
}

// -----------------------------------------------------------------------------

CVertex _const_ * CSegment::Vertex (ushort nSide, short nIndex) _const_
{
return vertexManager.Vertex (VertexId (nSide, nIndex));
}

// -----------------------------------------------------------------------------

CSegment* CSegment::Child (short nSide) _const_ 
{ 
return (short (m_sides [nSide].m_info.nChild) < 0) ? null : segmentManager.Segment (m_sides [nSide].m_info.nChild); 
}

// -----------------------------------------------------------------------------

int CSegment::UpdateVertexId (ushort nOldId, ushort nNewId)
{
for (int i = 0; i < 8; i++)
	if (m_info.vertexIds [i] == nOldId) {
		m_info.vertexIds [i] = nNewId;
		return i;
		}
return -1;
}

// -----------------------------------------------------------------------------

void CSegment::UpdateChildren (short nOldChild, short nNewChild)
{
for (short nSide = 0; nSide < 6; nSide++)
	if (m_sides [nSide].UpdateChild (nOldChild, nNewChild))	// no two sides can have the same child
		return;
}

// -----------------------------------------------------------------------------

void CSegment::Reset (short nSide)
{
if (nSide >= 0) {
	SetChild (nSide, -1); 
	m_info.childFlags &= ~(1 << nSide); 
	m_sides [nSide].Reset (-1);
	}
else {
	m_info.staticLight = 0; 
	m_info.group = -1;
	m_info.owner = -1;
	m_info.function = 0;
	m_info.value = -1;
	m_info.nProducer = -1;
	m_info.childFlags = 0;
	m_info.wallFlags = 0;
	m_info.flags = 0;
	m_info.bTunnel = 0;
	for (int i = 0; i < 8; i++)
		m_info.vertexIds [i] = i;
	m_nShape = SIDE_SHAPE_RECTANGLE;
	for (nSide = 0; nSide < 6; nSide++) {
		SetChild (nSide, -1); 
		m_info.childFlags &= ~(1 << nSide); 
		m_sides [nSide].Reset (nSide);
		}
	}	
}

// -----------------------------------------------------------------------------

bool CSegment::HasVertex (ushort nVertex)
{
for (int i = 0; i < 8; i++)
	if (m_info.vertexIds [i] == nVertex)
		return true;
return false;
}

// -----------------------------------------------------------------------------

bool CSegment::HasVertex (short nSide, ubyte nIndex)
{
return m_sides [nSide].HasVertex (nIndex);
}

// -----------------------------------------------------------------------------

bool CSegment::HasEdge (short nSide, ushort nVertex1, ushort nVertex2)
{
	CSide*	pSide = Side (nSide);
	int		nVertices = pSide->VertexCount ();

if (nVertices < 2)
	return false;

if (nVertices == 2) {
	ushort v1 = m_info.vertexIds [pSide->VertexIdIndex (0)],
			 v2 = m_info.vertexIds [pSide->m_vertexIdIndex [1]];
	return ((v1 == nVertex1) && (v2 == nVertex2)) || ((v2 == nVertex1) && (v1 == nVertex2));
	}

ushort v1, v2 = m_info.vertexIds [pSide->VertexIdIndex (0)];
for (int i = 1; i <= nVertices; i++) {
	v1 = v2;
	v2 = m_info.vertexIds [pSide->m_vertexIdIndex [i % nVertices]];
	if (((v1 == nVertex1) && (v2 == nVertex2)) || ((v2 == nVertex1) && (v1 == nVertex2)))
		return true;
	}
return false;
}

// -----------------------------------------------------------------------------

CGameItem* CSegment::Copy (CGameItem* pDest)
{
if (pDest != null)
	*dynamic_cast<CSegment*> (pDest) = *this;
return pDest;
}

// -----------------------------------------------------------------------------

CGameItem* CSegment::Clone (void)
{
return Copy (new CSegment);	// only make a copy if modified
}

// -----------------------------------------------------------------------------

void CSegment::Backup (eEditType editType)
{
Id () = undoManager.Backup (this, editType);
}

// -----------------------------------------------------------------------------

void CSegment::Undo (void)
{
switch (EditType ()) {
	case opAdd:
		segmentManager.Remove (Index ());
		break;
	case opDelete:
		segmentManager.Add ();
		// fall through
	case opModify:
		*GetParent () = *this;
		break;
	}
}

// -----------------------------------------------------------------------------

void CSegment::Redo (void)
{
switch (EditType ()) {
	case opDelete:
		segmentManager.Remove (Index ());
		break;
	case opAdd:
		segmentManager.Add ();
		// fall through
	case opModify:
		*GetParent () = *this;
		break;
	}
}

// -----------------------------------------------------------------------------

short CSegment::AdjacentSide (short nIgnoreSide, ushort* nEdgeVerts)
{
for (short nSide = 0; nSide < 6; nSide++)
	if ((nSide != nIgnoreSide) && (m_sides [nSide].m_nShape <= SIDE_SHAPE_TRIANGLE) && HasEdge (nSide, nEdgeVerts [0], nEdgeVerts [1]))
		return nSide;
return -1;
}

// -----------------------------------------------------------------------------

void CSegment::TagVertices (ubyte mask, short nSide)
{
if (nSide < 0) {
	for (int i = 0; i < 8; i++)
		if (m_info.vertexIds [i] <= MAX_VERTEX)
			vertexManager [m_info.vertexIds [i]].Tag (mask);
	}
else {
	CSide* pSide = Side (nSide);
	for (int i = 0, j = pSide->VertexCount (); i < j; i++)
		vertexManager [m_info.vertexIds [pSide->VertexIdIndex (i)]].Tag (mask);
	}
}

// -----------------------------------------------------------------------------

void CSegment::UnTagVertices (ubyte mask, short nSide)
{
if (nSide < 0) {
	for (int i = 0; i < 8; i++)
		if (m_info.vertexIds [i] <= MAX_VERTEX)
			vertexManager [m_info.vertexIds [i]].UnTag (mask);
	}
else {
	CSide* pSide = Side (nSide);
	for (int i = 0, j = pSide->VertexCount (); i < j; i++)
		vertexManager [m_info.vertexIds [pSide->VertexIdIndex (i)]].UnTag (mask);
	}
}

// ------------------------------------------------------------------------ 

bool CSegment::HasTaggedVertices (ubyte mask, short nSide)
{
if (nSide < 0) {
	for (int i = 0; i < 8; i++)
		if ((m_info.vertexIds [i] <= MAX_VERTEX) && vertexManager [m_info.vertexIds [i]].IsTagged (mask))
			return true;
	}
else {
	CSide* pSide = Side (nSide);
	for (int i = 0, j = pSide->VertexCount (); i < j; i++)
		if (vertexManager [m_info.vertexIds [pSide->VertexIdIndex (i)]].IsTagged (mask))
			return true;
	}
return false;
}

// -----------------------------------------------------------------------------

void CSegment::Tag (ubyte mask)
{
m_info.flags |= mask;
for (int i = 0; i < 6; i++)
	Side (i)->Tag (mask);
#if 0
for (int i = 0; i < 8; i++)
	if (VertexId (i) <= MAX_VERTEX)
		Vertex (i)->Tag (mask);
#endif
}

// -----------------------------------------------------------------------------

void CSegment::UnTag (ubyte mask)
{
m_info.flags &= ~mask;
for (int i = 0; i < 6; i++)
	Side (i)->UnTag (mask);
#if 0
for (int i = 0; i < 8; i++)
	if (VertexId (i) <= MAX_VERTEX)
		Vertex (i)->UnTag (mask);
#endif
}

// -----------------------------------------------------------------------------

void CSegment::ToggleTag (ubyte mask)
{
m_info.flags ^= mask;
for (int i = 0; i < 6; i++)
	Side (i)->ToggleTag (mask);
#if 0
for (int i = 0; i < 8; i++)
	if (VertexId (i) <= MAX_VERTEX)
		Vertex (i)->ToggleTag (mask);
#endif
}

// -----------------------------------------------------------------------------

void CSegment::Tag (short nSide, ubyte mask)
{
Side (nSide)->Tag (mask);
#if 0
for (int i = 0, j = Side (nSide)->VertexCount (); i < j; i++)
	Vertex (nSide, i)->Tag (mask);
#endif
}

// -----------------------------------------------------------------------------

void CSegment::UnTag (short nSide, ubyte mask)
{
Side (nSide)->UnTag (mask);
#if 0
for (int i = 0, j = Side (nSide)->VertexCount (); i < j; i++)
	Vertex (nSide, i)->UnTag (mask);
#endif
}

// -----------------------------------------------------------------------------

void CSegment::ToggleTag (short nSide, ubyte mask)
{
Side (nSide)->ToggleTag (mask);
for (int i = 0, j = Side (nSide)->VertexCount (); i < j; i++)
	Vertex (nSide, i)->ToggleTag (mask);
}

// -----------------------------------------------------------------------------

bool CSegment::IsTagged (short nSide, ubyte mask)
{
if (nSide >= 0)
	return Side (nSide)->IsTagged (mask);
for (nSide = 0; nSide < 6; nSide++)
	if (Side (nSide)->IsTagged (mask))
		return true;
return false;
}

// -----------------------------------------------------------------------------

bool CSegment::GetEdgeVertices (short nSide, short nLine, ushort& v1, ushort& v2)
{
	short i1, i2;

if (!Side (nSide)->GetEdgeIndices (nLine, i1, i2))
	return false;
v1 = m_info.vertexIds [i1];
v2 = m_info.vertexIds [i2];
return true;
}

// -----------------------------------------------------------------------------

ubyte CSegment::OppSideVertex (short nSide, short nIndex)
{
for (int i = 0; i < 4; i++) {
	if (sideVertexTable [nSide][i] == nIndex) 
		return oppSideVertexTable [nSide][i];
	}
return 0xff;
}

// -----------------------------------------------------------------------------

#if 0

double CSegment::MinEdgeLength (void)
{
	CEdgeList edgeList;

	ubyte		v1, v2, s1, s2;
	double	l, minLength = 1e30;
	int		nEdges = BuildEdgeList (edgeList);

for (int i = 0; i < nEdges; i++) {
	edgeList.Get (i, s1, s2, v1, v2);
	l = Distance (Vertex (VertexId (v1)), Vertex (VertexId (v2)));
	if (minLength > l)
		minLength = l;
	}
return minLength;
}

#endif

// -----------------------------------------------------------------------------

void CSegment::CreateOppVertexIndex (short nSide, ubyte* oppVertexIndex)
{
	CEdgeList edgeList;

	int	nEdges = BuildEdgeList (edgeList);
	int	nVertices = m_sides [nSide].VertexCount ();
	short	nOppSide = oppSideTable [nSide];
	ubyte* vertexIndex = m_sides [nSide].m_vertexIdIndex;
	int	i, j;
	ubyte	h, v, v1, v2, s1, s2;

for (i = 0; i < nVertices; i++) {
	oppVertexIndex [i] = 0xff;
	h = vertexIndex [i];
	// look for an edge that connects opp and far side and that shares the point we just assigned a vertex to. 
	// The other point of that edge is the corresponding point of origin side.
	for (j = 0; j < nEdges; j++) {
		edgeList.Get (j, s1, s2, v1, v2);
		if (h == v1)
			v = v2;
		else if (h == v2)
			v = v1;
		else
			continue;
		if ((s1 == nSide) || (s1 == nOppSide) || (s2 == nSide) || (s2 == nOppSide))
			continue;
		oppVertexIndex [i] = v; 
		break;
		}
	if (j == nEdges) {
		for (j = 0; j < 6; j++) {
			if (m_sides [j].m_nShape > SIDE_SHAPE_EDGE) {
				oppVertexIndex [i] = m_sides [j].m_vertexIdIndex [0];
				break;
				}
			}
		}
	}
}

// -----------------------------------------------------------------------------

void CSegment::ShiftVertices (short nSide)
{
	CSide& side = m_sides [nSide];
	ushort vertices [4];
	int j = side.VertexCount ();

for (int i = 0; i < j; i++)
	vertices [(i + 1) % j] = m_info.vertexIds [side.m_vertexIdIndex [i]];
for (int i = 0; i < j; i++)
	m_info.vertexIds [side.m_vertexIdIndex [i]] = vertices [i];
}

// -----------------------------------------------------------------------------
// all vertex id table entries with index > nIndex have been moved down once in
// the table. Update the sides' vertex id indices accordingly.

void CSegment::UpdateVertexIdIndex (ubyte nIndex)
{
CSide* pSide = Side (0);
for (int i = 0; i < 6; i++, pSide++) {
	int h = pSide->VertexCount ();
	for (int j = 0; j < h; j++) {
		if (pSide->m_vertexIdIndex [j] > nIndex)
			pSide->m_vertexIdIndex [j]--;
		}
	}
}

// -----------------------------------------------------------------------------

int CSegment::BuildEdgeList (CEdgeList& edgeList, ubyte nSide, bool bSparse)
{
	CSide* pSide = Side (nSide);

if (bSparse && !pSide->IsVisible ())
	return 0; // only gather edges that are not shared with another segment
short nVertices = pSide->VertexCount ();
if (nVertices < 2)
	return 0;
if (nVertices == 2)
	edgeList.Add (nSide, pSide->VertexIdIndex (0), pSide->VertexIdIndex (1));
else {
	ubyte v1, v2 = pSide->VertexIdIndex (0);
	for (short nVertex = 1; nVertex <= nVertices; nVertex++) {
		v1 = v2;
		v2 = pSide->VertexIdIndex (nVertex);
		edgeList.Add (nSide, v1, v2);
		}
	}
return 1;
}

// -----------------------------------------------------------------------------

int CSegment::BuildEdgeList (CEdgeList& edgeList, bool bSparse)
{
edgeList.Reset ();
for (ubyte nSide = 0; nSide < 6; nSide++) 
	BuildEdgeList (edgeList, nSide, bSparse);
return edgeList.Count ();
}

// -----------------------------------------------------------------------------

CVertex& CSegment::ComputeCenter (bool bView)
{
int nVertices = 0;
m_vCenter.Clear ();
for (int i = 0; i < 8; i++) {
	if (m_info.vertexIds [i] <= MAX_VERTEX) {
		nVertices++;
		m_vCenter += bView ? vertexManager [m_info.vertexIds [i]].m_view : vertexManager [m_info.vertexIds [i]];
		}
	}
m_vCenter /= double (nVertices);
return m_vCenter;
}

// -----------------------------------------------------------------------------

CVertex& CSegment::ComputeCenter (short nSide)
{
CSide* pSide = Side (nSide);
CVertex& vCenter = pSide->m_vCenter;
vCenter.Clear ();
vCenter.m_view.Clear ();
vCenter.m_screen.x = vCenter.m_screen.y = vCenter.m_screen.z = 0;
short j = pSide->VertexCount ();
for (short i = 0; i < j; i++) {
	CVertex* v = Vertex (nSide, i);
	vCenter += *v;
	vCenter.m_view += v->m_view;
	vCenter.m_screen += v->m_screen;
	}
vCenter /= double (j);
vCenter.m_view /= double (j);
vCenter.m_screen /= j;
return vCenter;
}

// -----------------------------------------------------------------------------

void CSegment::ComputeNormals (short nSide, bool bView)
{
if (nSide >= 0)
	Side (nSide)->ComputeNormals (m_info.vertexIds, ComputeCenter (bView));
else {
	ComputeCenter (bView);
	for (nSide = 0; nSide < 6; nSide++)
		Side (nSide)->ComputeNormals (m_info.vertexIds, m_vCenter);
	}
}

// -----------------------------------------------------------------------------

void CSegment::UpdateTexCoords (ushort nVertexId, bool bMove, short nIgnoreSide)
{
for (ubyte i = 0; i < 8; i++) {
	if (m_info.vertexIds [i] == nVertexId) {
		for (short nSide = 0; nSide < 6; nSide++) {
#ifdef _DEBUG
			if (nSide == nDbgSide)
				nDbgSide = nDbgSide;
#endif
			if (nSide != nIgnoreSide)
				m_sides [nSide].FindAndUpdateTexCoords (i, m_info.vertexIds, bMove);
			}
		return;
		}
	}
}

//--------------------------------------------------------------------------

short CSegment::IsSelected (CRect& viewport, long xMouse, long yMouse, short nSide, bool bSegments) 
{
ComputeCenter ();
CViewMatrix* viewMatrix = DLE.MineView ()->ViewMatrix ();
if (bSegments) {
	CVertex& center = Center ();
	center.Transform (viewMatrix);
	center.Project (viewMatrix);
	if ((center.m_screen.x  < 0) || (center.m_screen.y < 0) || (center.m_screen.x >= viewport.right) || (center.m_screen.y >= viewport.bottom) || (center.m_view.v.z < 0.0)) 
		return -1;
	}
short nSegment = segmentManager.Index (this);
#ifdef _DEBUG
if (nSegment == nDbgSeg)
	nDbgSeg = nDbgSeg;
#endif
CFrustum* frustum = DLE.MineView ()->Renderer ().Frustum ();
CSide* pSide = Side (nSide);
for (; nSide < 6; nSide++, pSide++) {
	if (frustum && !frustum->Contains (nSegment, nSide))
		continue;
	pSide->SetParent (nSegment);
	if (!pSide->IsSelected (viewport, xMouse, yMouse, m_info.vertexIds))
		continue;
	if (!bSegments) {
		ComputeCenter (nSide);
		CVertex& center = pSide->Center ();
		if (!pSide->IsVisible ()) {
			pSide->ComputeNormals (m_info.vertexIds, Center ());
			CVertex normal;
			center += pSide->Normal (2) * 2.0;
			center.Transform (viewMatrix);
			center.Project (viewMatrix);
			}
		if ((center.m_screen.x  < 0) || (center.m_screen.y < 0) || (center.m_screen.x >= viewport.right) || (center.m_screen.y >= viewport.bottom) || (center.m_view.v.z < 0.0)) 
			continue;
		}
	return nSide;
	}
return -1;
}

// -----------------------------------------------------------------------------

short CSegment::Edge (short nSide, ushort v1, ushort v2)
{
for (short i = 0; i < 8; i++) {
	if (m_info.vertexIds [i] == v1) {
		short j = (i + 1) % 8;
		if (m_info.vertexIds [j] != v2) {
			j = (i - 1 + 8) % 8;
			if (m_info.vertexIds [j] != v2)
				return -1;
			}
		return Side (nSide)->Edge (i, j);
		}
	}
return -1;
}

// -----------------------------------------------------------------------------

void CSegment::MakeCoplanar (short nSide)
{
	CSide* pSide = Side (nSide);
	short j = pSide->VertexCount ();

if (j < 4)
	return;

pSide->ComputeNormals (VertexIds (), ComputeCenter ());
if (pSide->Normal (0) == pSide->Normal (1))
	return;
CDoubleVector v0 = *Vertex (nSide, pSide->m_nPoint);
CDoubleVector n = Normal (v0, *Vertex (nSide, pSide->m_nPoint + 1), *Vertex (nSide, pSide->m_nPoint - 1));
CDoubleVector v1 = pSide->Center () - v0;
if (Dot (v1, n) < 0.0)
	n.Negate ();
for (short i = 0; i < j; i++) {
	CDoubleVector v = *Vertex (nSide, i) - v0;
	double d = Dot (v, n);
	*Vertex (nSide, i) -= n * d;
	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void CObjectProducer::Read (CFileManager* fp, bool bFlag)
{
m_info.objFlags [0] = fp->ReadInt32 ();
if (DLE.IsD2File ())
	m_info.objFlags [1] = fp->ReadInt32 ();
m_info.hitPoints = fp->ReadInt32 ();
m_info.interval = fp->ReadInt32 ();
m_info.nSegment = fp->ReadInt16 ();
m_info.nProducer = fp->ReadInt16 ();
}

// -----------------------------------------------------------------------------

void CObjectProducer::Write (CFileManager* fp, bool bFlag)
{
fp->Write (m_info.objFlags [0]);
if (DLE.IsD2File ())
	fp->Write (m_info.objFlags [1]);
fp->Write (m_info.hitPoints);
fp->Write (m_info.interval);
fp->Write (m_info.nSegment);
fp->Write (m_info.nProducer);
}

// -----------------------------------------------------------------------------
// make a copy of this segment for the undo manager
// if segment was modified, make a copy of the current segment
// if segment was added or deleted, just make a new CGameItem instance and 
// mark the operation there

CGameItem* CObjectProducer::Clone (void)
{
CObjectProducer* cloneP = new CObjectProducer;	// only make a copy if modified
if (cloneP != null)
	*cloneP = *this;
return cloneP;
}

// -----------------------------------------------------------------------------

CGameItem* CObjectProducer::Copy (CGameItem* pDest)
{
if (pDest != null)
	*dynamic_cast<CObjectProducer*> (pDest) = *this;
return pDest;
}

// -----------------------------------------------------------------------------

void CObjectProducer::Backup (eEditType editType)
{
Id () = undoManager.Backup (this, editType);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

int CEdgeList::Add (ubyte nSide, ubyte v1, ubyte v2)
{
ushort nEdge = Key (v1, v2);
for (int i = 0; i < m_nEdges; i++)
	if (m_edgeList [i].m_nEdge == nEdge) {
		if (m_edgeList [i].m_nSides == 2)
			return -1;
		if (m_edgeList [i].m_sides [0] != nSide) {
			m_edgeList [i].m_sides [1] = nSide;
			m_edgeList [i].m_nSides = 2;
			}
		return i;
		}
if (m_nEdges == 12)
	return -1;
m_edgeList [m_nEdges].m_nEdge = nEdge;
m_edgeList [m_nEdges].m_sides [0] = nSide;
m_edgeList [m_nEdges].m_nSides = 1;
return ++m_nEdges;
}

// -----------------------------------------------------------------------------

int CEdgeList::Find (int i, ubyte& side1, ubyte& side2, ubyte v1, ubyte v2)
{ 
ushort nEdge = Key (v1, v2);
for (; i < m_nEdges; i++) {
	if (m_edgeList [i].m_nEdge == nEdge) {
		side1 = m_edgeList [i].m_sides [0];
		side2 = m_edgeList [i].m_sides [1];
		return i;
		}
	}
return -1;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//eof segment.cpp