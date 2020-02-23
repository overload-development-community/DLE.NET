#include "mine.h"
#include "dle-xp.h"

// ----------------------------------------------------------------------------- 

bool CSegmentManager::Full (void) 
{ 
return Count () >= SEGMENT_LIMIT; 
}

// ----------------------------------------------------------------------------- 

short CSegmentManager::Add (void) 
{ 
#if USE_FREELIST

if (m_free.Empty ())
	return -1;
int nSegment = --m_free;
m_segments [nSegment].Clear ();
return (short) nSegment; 

#else //USE_FREELIST

if (Count () >= SEGMENT_LIMIT)
	return -1;
m_segments [Count ()].Reset ();
return Count ()++;

#endif //USE_FREELIST
}

// ----------------------------------------------------------------------------- 

void CSegmentManager::Remove (short nDelSeg)
{
#if USE_FREELIST
m_free += nDelSeg;
Count ()--;
#else
if (nDelSeg < Count ()) {
	if (nDelSeg < --Count ()) {
		// move the last segment in the segment list to the deleted segment's position
		if (current->SegmentId () == Count ())
			current->SetSegmentId (nDelSeg);
		if (other->m_nSegment == Count ())
			other->SetSegmentId (nDelSeg);
		*Segment (nDelSeg) = *Segment (Count ());
		// update all trigger targets pointing at the moved segment
		triggerManager.UpdateTargets (Count (), nDelSeg);
		objectManager.UpdateSegments (Count (), nDelSeg);
		// update all walls inside the moved segment
		CSide* pSide = Segment (nDelSeg)->Side (0);
		for (int i = 0; i < 6; i++, pSide++) {
			CSegment* pSegment = pSide->Child ();
			if (pSegment != null)
				pSegment->UpdateChildren (Count (), nDelSeg);
			CWall* pWall = pSide->Wall ();
			if (pWall != null)
				pWall->m_nSegment = nDelSeg;
			}
		}
	}
#endif
}

// ----------------------------------------------------------------------------- 

void CSegmentManager::AddSegments (void)
{
if (theMine->SelectMode () == BLOCK_MODE && segmentManager.TaggedSideCount () > 50) {
	if (Query2Msg ("You are about to insert a large number of cubes.\n"
		"Are you sure you want to do this?", MB_YESNO) != IDYES)
		return;
	}

undoManager.Begin (__FUNCTION__, udSegments); 
short nSegment = Create (*current);
if (nSegment < 0) {
	undoManager.End (__FUNCTION__);
	return;
	}
current->SetSegmentId (nSegment);
// In block mode, also insert cubes from all marked sides
if (theMine->SelectMode () == BLOCK_MODE) {
	CSegment* pSegment = Segment (0);
	for (short nSegment = 0, nSegments = Count (); nSegment < nSegments; nSegment++, pSegment++) 
		for (short nSide = 0; nSide < 6; nSide++)
			if (pSegment->IsTagged (nSide) && !pSegment->HasChild (nSide)) 
				if (0 > Create (CSideKey (nSegment, nSide)))
					break;
	}
undoManager.End (__FUNCTION__); 
}

// ----------------------------------------------------------------------------- 

short CSegmentManager::Create (CSideKey key, int addMode)
{
	int i;

if (tunnelMaker.Active ())
	return -1; 

if (Full ()) {
	ErrorMsg ("Cannot add a new segment because\nthe maximum number of segments has been reached."); 
	return -1;
	}
if (vertexManager.Full ()) {
	ErrorMsg ("Cannot add a new segment because\nthe maximum number of vertices has been reached."); 
	return -1;
	}
if ((Count () == MAX_SEGMENTS - 1) && (QueryMsg ("Adding more segments will exceed\nthe maximum segment count for this level type.\nAre you sure you want to continue?") != IDYES)) {
	return -1;
	}

current->Get (key);
CSegment* pSegment = Segment (key.m_nSegment); 

short	nCurSide = key.m_nSide; 
if (pSegment->ChildId (nCurSide) >= 0) {
	ErrorMsg ("Cannot add a new segment to a side\nwhich already has a segment attached."); 
	return -1;
	}

if (Side (key)->VertexCount () < 3) {
	ErrorMsg ("Cannot add a new segment to this side."); 
	//undoManager.Unroll (__FUNCTION__);
	return -1;
	}

m_bCreating = true;
undoManager.Begin (__FUNCTION__, udSegments | udVertices); 
// get new segment

if (addMode < 0)
	addMode = m_nAddMode;

short nSides [2] = {key.m_nSide, oppSideTable [key.m_nSide]};

if ((addMode == MIRROR) && (pSegment->Side (nSides [0])->Shape () != pSegment->Side (nSides [1])->Shape ())) // mirror not possible!
	addMode = EXTEND;

short nNewSeg = Add (); 
CSegment* pNewSeg = Segment (nNewSeg); 

ushort newVerts [4]; 
vertexManager.Add (newVerts, Side (key)->VertexCount ());

short nVertices = ComputeVertices (key, newVerts, addMode); 

CSide* pOrgSide = pSegment->Side (nSides [0]);
CSide* pFarSide = pNewSeg->Side (nSides [0]);
CSide* pOppSide = pNewSeg->Side (nSides [1]);
short nOppVertices = pOppSide->VertexCount ();

pNewSeg->Setup ();

if (nVertices == 3) { //create a wedge shaped segment when adding to a triangular segment side
	pNewSeg->SetShape (SEGMENT_SHAPE_WEDGE);
	short nSide = 0;
	for (; nSide < 6; nSide++)
		if ((nSide != nSides [0]) && (nSide != nSides [1]) && pSegment->Side (nSide)->Shape ())
			break;
	CSide* pSide = pNewSeg->Side (nSide);
	short nEdge = 0, i = pSide->m_vertexIdIndex [0], v1, v2 = pNewSeg->m_info.vertexIds [i];
	for (++nEdge; nEdge <= 4; nEdge++) {
		v1 = v2;
		i = pSide->m_vertexIdIndex [nEdge % 4];
		v2 = pNewSeg->m_info.vertexIds [i];
		if (pNewSeg->HasEdge (nSides [0], v1, v2) || pNewSeg->HasEdge (nSides [1], v1, v2))
			break;
		}
	--nEdge;
	segmentManager.CollapseEdge (nNewSeg, nSide, nEdge % pSide->VertexCount (), false);
	if (nEdge < pSide->VertexCount ())
		nEdge++;
	segmentManager.CollapseEdge (nNewSeg, nSide, nEdge % pSide->VertexCount (), false);
	}

#ifdef _DEBUG
for (i = 0; i < 8; i++)
	if (pNewSeg->m_info.vertexIds [i] < MAX_VERTEX)
		pNewSeg->m_info.vertexIds [i] = MAX_VERTEX;
#endif

// Definitions:
//   origin segment: The segment to which a new segment is added
//   origin side: The origin segment's side from which the new segment shall extend
//   opposite side: The side of new segment that forms the back side of the origin side
//   far side: The side of the new segment opposing the base side
//   origin vertices: Vertices of the origin side
// This function assigns the origin vertices to the base side and derives
// the far side's vertices from these (by adding an offset to them)
// The following code assigns vertices to the new segment by looking at the edges of 
// the segment connecting opp side and far side

ubyte oppVertexIdIndex [4], * vertexIdIndex = pFarSide->m_vertexIdIndex;
pNewSeg->CreateOppVertexIndex (nSides [0], oppVertexIdIndex);

#if 0

for (short i = 0; i < nVertices; i++) {
	// define vertex numbers for new side
	ubyte h = pFarSide->m_vertexIdIndex [i];
	pNewSeg->m_info.vertexIds [h] = newVerts [i]; 

	// look for an edge that connects opp and far side and that shares the point we just assigned a vertex to. 
	// The other point of that edge is the corresponding point of origin side.
	ubyte v, v1, v2, s1, s2;
	for (short j = 0; j < nEdges; j++) {
		edgeList.Get (j, s1, s2, v1, v2);
		if (v1 == h)
			v = v2;
		else if (v2 == h)
			v = v1;
		else
			continue;
		if ((s1 == nSides [0]) || (s1 == nSides [1]) || (s2 == nSides [0]) || (s2 == nSides [1]))
			continue;
		pNewSeg->m_info.vertexIds [v] = pSegment->m_info.vertexIds [pOrgSide->m_vertexIdIndex [i]]; 
		break;
		}
	}

#else

for (int i = 0; i < nVertices; i++) {
	// define vertex numbers for new side
	pNewSeg->m_info.vertexIds [vertexIdIndex [i]] = newVerts [i]; 
	pNewSeg->m_info.vertexIds [oppVertexIdIndex [i]] = pSegment->m_info.vertexIds [pOrgSide->m_vertexIdIndex [i]]; 
	}

#endif

// define children and special child
pNewSeg->m_info.childFlags = 1 << oppSideTable [nCurSide]; // only opposite side connects to current segment
for (int i = 0; i < MAX_SIDES_PER_SEGMENT; i++) { // no remaining children
	pNewSeg->SetChild (i, (pNewSeg->m_info.childFlags & (1 << i)) ? key.m_nSegment : -1);
	pNewSeg->Side (i)->DetectShape ();
	}

// define textures
for (short nSide = 0; nSide < MAX_SIDES_PER_SEGMENT; nSide++) {
	if (pNewSeg->ChildId (nSide) < 0) {
		// if other segment does not have a child (therefore it has a texture)
		if ((pSegment->ChildId (nSide) < 0) && (pSegment->m_info.function == SEGMENT_FUNC_NONE)) {
			pNewSeg->m_sides [nSide].SetTextures (pSegment->m_sides [nSide].BaseTex (), pSegment->m_sides [nSide].OvlTex ());
			for (i = 0; i < nVertices; i++) {
				if (addMode != ORTHOGONAL)
					memcpy (pNewSeg->m_sides [nSide].m_info.uvls, pSegment->m_sides [nSide].m_info.uvls, sizeof (pNewSeg->m_sides [nSide].m_info.uvls));
				else
					pNewSeg->m_sides [nSide].m_info.uvls [i].l = pSegment->m_sides [nSide].m_info.uvls [i].l; 
				}
			} 
		}
	else {
		pNewSeg->m_sides [nSide].ResetTextures ();
		}
	}

// define static light
pNewSeg->m_info.staticLight = pSegment->m_info.staticLight; 

// delete variable light if it exists
lightManager.DeleteVariableLight (CSideKey (key.m_nSegment, nCurSide)); 

// update current segment
pSegment->SetChild (nCurSide, nNewSeg); 
pOrgSide->SetTextures (0, 0);
memset (pOrgSide->m_info.uvls, 0, sizeof (pOrgSide->m_info.uvls));
 
// link the new segment with any touching Segment ()
#if 1
CVertex *vNewSeg = vertexManager.Vertex (pNewSeg->m_info.vertexIds [0]);
int nSegments = Count ();
pSegment = Segment (0); 
for (int i = 0; i < nSegments; i++, pSegment++) {
	if (i != nNewSeg) {
		// first check to see if Segment () are any where near each other
		// use x, y, and z coordinate of first point of each segment for comparison
		CVertex *vSeg = vertexManager.Vertex (pSegment->m_info.vertexIds [0]);
		if (fabs (vNewSeg->v.x - vSeg->v.x) < 160.0 &&
			 fabs (vNewSeg->v.y - vSeg->v.y) < 160.0 &&
			 fabs (vNewSeg->v.z - vSeg->v.z) < 160.0)
			for (short nNewSide = 0; nNewSide < 6; nNewSide++)
				for (short nSide = 0; nSide < 6; nSide++)
					Link (nNewSeg, nNewSide, i, nSide, 3);
		}
	}
#endif
// auto align textures new segment
for (short nNewSide = 0; nNewSide < 6; nNewSide++)
	AlignSideTextures (key.m_nSegment, nNewSide, nNewSeg, nNewSide, true, true); 
// set current segment to new segment
key.m_nSegment = nNewSeg; 
Segment (key)->Backup (opAdd);
//		SetLinesToDraw(); 
DLE.MineView ()->Refresh (false); 
DLE.ToolView ()->Refresh (); 
undoManager.End (__FUNCTION__);
m_bCreating = false;
return nNewSeg; 
}

// ----------------------------------------------------------------------------- 

short CSegmentManager::Create (short nSegment, bool bCreate, ubyte nFunction, short nTexture, char* szError)
{
if ((szError != null) && DLE.IsD1File ()) {
	if (!DLE.ExpertMode ())
		ErrorMsg (szError);
	return 0;
	}

if (bCreate) {
	if (current->ChildId () >= 0)
		return -1;
	undoManager.Begin (__FUNCTION__, udSegments);
	nSegment = Create (*current, -1);
	if (nSegment < 0) {
		Remove (nSegment);
		undoManager.End (__FUNCTION__);
		return -1; 
		}
	}	
else
	undoManager.Begin (__FUNCTION__, udSegments);

DLE.MineView ()->DelayRefresh (true);
m_bCreating = true;
if (!Define (nSegment, nFunction, -1)) {
	if (bCreate)
		Remove (nSegment);
	undoManager.End (__FUNCTION__);
	DLE.MineView ()->DelayRefresh (false);
	m_bCreating = false;
	return -1; 
	}	
Segment (nSegment)->Backup ();
m_bCreating = false;
undoManager.End (__FUNCTION__);
DLE.MineView ()->DelayRefresh (false);
DLE.MineView ()->Refresh ();
return nSegment;
}

// ----------------------------------------------------------------------------- 

bool CSegmentManager::CreateProducer (short nSegment, bool bCreate, ubyte nType, bool bSetDefTextures, 
												  CObjectProducer* producers, CMineItemInfo& info, char* szError) 
{
if (info.count >= MAX_MATCENS) {
	ErrorMsg (szError);
	return false;
	}
undoManager.Begin (__FUNCTION__, udSegments);
if (0 > (nSegment = Create (nSegment, bCreate, nType))) {
	undoManager.End (__FUNCTION__);
	return false;
	}
producers [info.count].Setup (nSegment, info.count, 0);
Segment (nSegment)->m_info.value = 
Segment (nSegment)->m_info.nProducer = info.count++;
Segment (nSegment)->Backup (); // overwrite backup
undoManager.End (__FUNCTION__);
return true;
}

// ----------------------------------------------------------------------------- 

bool CSegmentManager::CreateEquipMaker (short nSegment, bool bCreate, bool bSetDefTextures) 
{
if (!DLE.IsD2XFile ()) {
	ErrorMsg ("Equipment makers are only available in D2X-XL levels.");
	return false;
	}
return CreateProducer (nSegment, bCreate, SEGMENT_FUNC_EQUIPMAKER, bSetDefTextures, EquipMaker (0), m_producerInfo [1], 
							"Maximum number of equipment makers reached");
}

// ----------------------------------------------------------------------------- 

bool CSegmentManager::CreateRobotMaker (short nSegment, bool bCreate, bool bSetDefTextures) 
{
return CreateProducer (nSegment, bCreate, SEGMENT_FUNC_ROBOTMAKER, bSetDefTextures, RobotMaker (0), m_producerInfo [0], 
							"Maximum number of robot makers reached");
}

// ----------------------------------------------------------------------------- 

bool CSegmentManager::CreateReactor (short nSegment, bool bCreate, bool bSetDefTextures) 
{
return 0 <= Create (nSegment, bCreate, SEGMENT_FUNC_REACTOR, bSetDefTextures ? DLE.IsD1File () ? 10 : 357 : -1);
}

// ----------------------------------------------------------------------------- 

bool CSegmentManager::CreateGoal (short nSegment, bool bCreate, bool bSetDefTextures, ubyte nType, short nTexture) 
{
return 0 <= Create (nSegment, bCreate, nType, bSetDefTextures ? nTexture : -1, "Flag goals are not available in Descent 1.");
}

// ----------------------------------------------------------------------------- 

bool CSegmentManager::CreateTeam (short nSegment, bool bCreate, bool bSetDefTextures, ubyte nType, short nTexture) 
{
return 0 <= Create (nSegment, bCreate, nType, bSetDefTextures ? nTexture : -1, "Team start positions are not available in Descent 1.");
}

// ----------------------------------------------------------------------------- 

bool CSegmentManager::CreateSkybox (short nSegment, bool bCreate) 
{
return 0 <= Create (nSegment, bCreate, SEGMENT_FUNC_SKYBOX, -1, "Skyboxes are not available in Descent 1.");
}

// ----------------------------------------------------------------------------- 

bool CSegmentManager::CreateSpeedBoost (short nSegment, bool bCreate) 
{
return 0 <= Create (nSegment, bCreate, SEGMENT_FUNC_SPEEDBOOST, -1, "Speed boost segments are not available in Descent 1.");
}

// ----------------------------------------------------------------------------- 

int CSegmentManager::ProducerCount (void)
{
int nProducers = 0;
for (CSegmentIterator si; si; si++) 
	if ((si->m_info.function == SEGMENT_FUNC_PRODUCER) || (si->m_info.function == SEGMENT_FUNC_REPAIRCEN))
		nProducers++;
return nProducers;
}

// ----------------------------------------------------------------------------- 

short CSegmentManager::CreateProducer (short nSegment, ubyte nType, bool bCreate, bool bSetDefTextures) 
{
// count number of fuel centers
int nProducer = ProducerCount ();
if (nProducer >= MAX_NUM_RECHARGERS) {
	ErrorMsg ("Maximum number of fuel centers reached.");
	return 0;
	}

CSegment *pSegment = Segment (0);

undoManager.Begin (__FUNCTION__, udSegments);
if (nType == SEGMENT_FUNC_REPAIRCEN)
	nSegment = Create (nSegment, bCreate, nType, bSetDefTextures ? 433 : -1, "Repair centers are not available in Descent 1.");
else {
	short nLastSeg = current->SegmentId ();
	nSegment = Create (nSegment, bCreate, nType, bSetDefTextures ? DLE.IsD1File () ? 322 : 333 : -1);
	if (nSegment < 0) {
		undoManager.End (__FUNCTION__);
		return -1;
		}
	if (bSetDefTextures) { // add energy spark walls to fuel center sides
		current->SetSegmentId (nLastSeg);
		if (wallManager.Create (*current, WALL_ILLUSION, 0, KEY_NONE, -1, -1) != null) {
			CSideKey back;
			if (BackSide (back))
				wallManager.Create (back, WALL_ILLUSION, 0, KEY_NONE, -1, -1);
			}
		Segment (nSegment)->Backup ();
		current->SetSegmentId (nSegment);
		}
	}
undoManager.End (__FUNCTION__);
return nSegment;
}

// ----------------------------------------------------------------------------- 

short CSegmentManager::ComputeVerticesOrtho (CSideKey key, ushort newVerts [4])
{
	CSegment*		pSegment = segmentManager.Segment (key); 
	CSide*			pSide = pSegment->Side (key.m_nSide);
	short				nVertices = pSide->VertexCount ();

	CDoubleVector	A [4]; 
	short				i, nPoint = pSide->Point (), points [4]; 

for (i = 0; i < 4; i++)
	points [i] = (nPoint + i) % 4;
CDoubleVector center = CalcSideCenter (key); 
CDoubleVector oppCenter = CalcSideCenter (CSideKey (key.m_nSegment, oppSideTable [key.m_nSide])); 
CDoubleVector vNormal = CalcSideNormal (key); 
// set the length of the new segment to be one standard segment length
// scale the vector
vNormal *= 20.0; // 20 is edge length of a standard segment
// figure out new center
CDoubleVector newCenter = center + vNormal; 
// new method: extend points 0 and 1 with vNormal, then move point 0 toward point 1.
// point 0
ushort i1, i2;
pSegment->GetEdgeVertices (key.m_nSide, nPoint, i1, i2);
CDoubleVector a = vNormal + *vertexManager.Vertex (i1); 
// point 1
CDoubleVector b = vNormal + *vertexManager.Vertex (i2); 
// center
CDoubleVector c = Average (a, b);
// vector from center to point0 and its length
CDoubleVector d = a - c; 
double length = d.Mag (); 
// factor to mul
double factor = (length > 0) ? 10.0 / length : 1.0; 
// set point 0
d *= factor;
A [points [0]] = c + d; 
// set point 1
A [points [1]] = c - d; 
// point 2 is orthogonal to the vector 01 and the vNormal vector
c = -CrossProduct (A [points [0]] - A [points [1]], vNormal);
c.Normalize ();
// normalize the vector
A [points [2]] = A [points [1]] + (c * 20); 
A [points [3]] = A [points [0]] + (c * 20); 
if (nVertices == 3) {
	A [points [2]] += A [points [3]]; 
	A [points [2]] *= 0.5;
	}
a = A [0];
for (i = 1; i < nVertices; i++)
	a += A [i]; 
a /= double (nVertices);
// now center the side along about the newCenter
for (i = 0; i < nVertices; i++)
	A [i] += (newCenter - a); 
// set the new vertices
for (i = 0; i < nVertices; i++) {
	ushort nVertex = newVerts [i];
	*vertexManager.Vertex (nVertex) = A [i]; 
	vertexManager.Vertex (nVertex)->Backup (); // update the newly added vertex instead of creating a new backup
	}
return nVertices;
}

// ----------------------------------------------------------------------------- 

short CSegmentManager::ComputeVerticesExtend (CSideKey key, ushort newVerts [4])
{
	CSegment*		pSegment = segmentManager.Segment (key); 
	CSide*			pSide = pSegment->Side (key.m_nSide);
	short				nVertices = pSide->VertexCount ();

CDoubleVector center = CalcSideCenter (key); 
CDoubleVector oppCenter = CalcSideCenter (CSideKey (key.m_nSegment, oppSideTable [key.m_nSide])); 
CDoubleVector vNormal = CalcSideNormal (key); 
// calculate the length of the new segment
vNormal *= Distance (center, oppCenter); 
// set the new vertices
for (int i = 0; i < nVertices; i++) {
	ushort nVertex = newVerts [i];
	*vertexManager.Vertex (nVertex) = *pSegment->Vertex (pSide->m_vertexIdIndex [i]) + vNormal; 
	vertexManager.Vertex (nVertex)->Backup (); // do not create a new backup right after adding this vertex
	}
return nVertices;
}

// ----------------------------------------------------------------------------- 

short CSegmentManager::ComputeVerticesMirror (CSideKey key, ushort newVerts [4])
{
	CDoubleVector	A [8], B [8], C [8], D [8], E [8]; 
	short				i; 
	CDoubleVector	center, oppCenter, newCenter, vNormal; 

	CSegment*		pSegment = segmentManager.Segment (key); 
	CSide*			pSide = pSegment->Side (key.m_nSide);
	CSide*			pOppSide = segmentManager.Side (CSideKey (key.m_nSegment, oppSideTable [key.m_nSide]));
	short				nVertices = pSide->VertexCount ();
	ubyte				oppVertexIdIndex [4];

	pSegment->CreateOppVertexIndex (key.m_nSide, oppVertexIdIndex);

// copy side's four points into A
short nSide = key.m_nSide;
for (i = 0; i < nVertices; i++) {
	A [i] = *pSegment->Vertex (pSide->m_vertexIdIndex [i]); 
	A [i + nVertices] = *pSegment->Vertex (oppVertexIdIndex [i]); 
	}

// subtract point 0 from all points in A to form B points
for (i = 0; i < 2 * nVertices; i++)
	B [i] = A [i] - A [0]; 

// calculate angle to put point 1 in x - y plane by spinning on x - axis
// then rotate B points on x - axis to form C points.
// check to see if on x - axis already
double angle1 = atan3 (B [1].v.z, B [1].v.y); 
for (i = 0; i < 2 * nVertices; i++)
	C [i].Set (B [i].v.x, B [i].v.y * cos (angle1) + B [i].v.z * sin (angle1), B [i].v.z * cos (angle1) - B [i].v.y * sin (angle1)); 
// calculate angle to put point 1 on x axis by spinning on z - axis
// then rotate C points on z - axis to form D points
// check to see if on z - axis already
double angle2 = atan3 (C [1].v.y, C [1].v.x); 
for (i = 0; i < 2 * nVertices; i++)
	D [i].Set (C [i].v.x * cos (angle2) + C [i].v.y * sin (angle2), C [i].v.y * cos (angle2) - C [i].v.x * sin (angle2), C [i].v.z); 

// calculate angle to put point 2 in x - y plane by spinning on x - axis
// the rotate D points on x - axis to form E points
// check to see if on x - axis already
double angle3 = atan3 (D [2].v.z, D [2].v.y); 
for (i = 0; i < 2 * nVertices; i++) 
	E [i].Set (D [i].v.x, D [i].v.y * cos (angle3) + D [i].v.z * sin (angle3), D [i].v.z * cos (angle3) - D [i].v.y * sin (angle3)); 

// now points 0, 1, and 2 are in x - y plane and point 3 is close enough.
// mirror new points on z axis
for (i = nVertices; i < 2 * nVertices; i++)
	E [i].v.z = -E [i].v.z; 
// now reverse rotations
angle3 = -angle3;
for (i = nVertices; i < 2 * nVertices; i++) 
	D [i].Set (E [i].v.x, E [i].v.y * cos (angle3) + E [i].v.z * sin (angle3), E [i].v.z * cos (angle3) - E [i].v.y * sin (angle3)); 
angle2 = -angle2;
for (i = nVertices; i < 2 * nVertices; i++) 
	C [i].Set (D [i].v.x * cos (angle2) + D [i].v.y * sin (angle2), D [i].v.y * cos (angle2) - D [i].v.x * sin (angle2), D [i].v.z); 
angle1 = -angle1;
for (i = nVertices; i < 2 * nVertices; i++) 
	B [i].Set (C [i].v.x, C [i].v.y * cos (angle1) + C [i].v.z * sin (angle1), C [i].v.z * cos (angle1) - C [i].v.y * sin (angle1)); 

// and translate back
ushort nVertex = pSegment->m_info.vertexIds [pSide->m_vertexIdIndex [0]]; 
for (i = nVertices; i < 2 * nVertices; i++) 
	A [i] = B [i] + *vertexManager.Vertex (nVertex); 

for (i = 0; i < nVertices; i++) {
	nVertex = newVerts [i];
	*vertexManager.Vertex (nVertex) = A [i + nVertices]; 
	vertexManager.Vertex (nVertex)->Backup ();
	}
return nVertices;
}

// ----------------------------------------------------------------------------- 
// Calculate vertices when adding a new segment.

short CSegmentManager::ComputeVertices (CSideKey key, ushort newVerts [4], int addMode)
{
	short	nVertices = segmentManager.Side (key)->VertexCount ();

if (nVertices < 3)
	return 0;

if (addMode < 0)
	addMode = m_nAddMode;
if (addMode == ORTHOGONAL)
	nVertices = ComputeVerticesOrtho (key, newVerts);
else if (addMode == EXTEND)
	nVertices = ComputeVerticesExtend (key, newVerts);
else if (addMode == MIRROR)
	nVertices = ComputeVerticesMirror (key, newVerts);
else
	return 0;
for (int i = nVertices; i < 4; i++) 
	newVerts [i] = 0xffff;
return nVertices;
}

// ----------------------------------------------------------------------------- 

bool CSegmentManager::SetDefaultTexture (short nTexture)
{
if (nTexture < 0)
	return true;

short nSegment = current->SegmentId ();
CSegment *pSegment = Segment (nSegment);

if (!m_bCreating)
	pSegment->Backup ();
double scale = textureManager.Scale (DLE.FileType (), nTexture);

undoManager.Begin (__FUNCTION__, udSegments);
pSegment->m_info.childFlags |= (1 << MAX_SIDES_PER_SEGMENT);
// set textures
CSide *pSide = pSegment->m_sides;
for (short nSide = 0; nSide < 6; nSide++, pSide++) {
	if (pSegment->ChildId (nSide) == -1) {
		SetTextures (CSideKey (nSegment, nSide), nTexture, 0);
		for (int i = 0; i < 4; i++) {
			pSide->m_info.uvls [i].u = (short) ((double) defaultUVLs [i].u / scale);
			pSide->m_info.uvls [i].v = (short) ((double) defaultUVLs [i].v / scale);
			pSide->m_info.uvls [i].l = defaultUVLs [i].l;
			}
		Segment (nSegment)->SetUV (nSide, 0, 0);
		}
	}
undoManager.End (__FUNCTION__);
return true;
}

// ----------------------------------------------------------------------------- 

bool CSegmentManager::Define (short nSegment, ubyte nFunction, short nTexture)
{
undoManager.Begin (__FUNCTION__, udSegments);
CSegment *pSegment = (nSegment < 0) ? current->Segment () : Segment (nSegment);
if (!m_bCreating)
	pSegment->Backup ();
Undefine (Index (pSegment));
pSegment->m_info.function = nFunction;
pSegment->m_info.childFlags |= (1 << MAX_SIDES_PER_SEGMENT);
SetDefaultTexture (nTexture);
undoManager.End (__FUNCTION__);
DLE.MineView ()->Refresh ();
return true;
}

// ----------------------------------------------------------------------------- 

void CSegmentManager::RemoveProducer (CSegment* pSegment, CObjectProducer* producers, CMineItemInfo& info, int nFunction)
{
if (info.count > 0) {
	// fill in deleted matcen
	int nDelProducer = pSegment->m_info.nProducer;
	if (nDelProducer >= 0) {
		// copy last producer in list to deleted producer's position
		undoManager.Begin (__FUNCTION__, udSegments | udProducers);
		pSegment->m_info.nProducer = -1;
		pSegment->m_info.value = -1;
		if (nDelProducer < --info.count) {
			memcpy (&producers [nDelProducer], &producers [info.count], sizeof (producers [0]));
			producers [nDelProducer].m_info.nProducer = nDelProducer;
			// point owner of relocated producer to that producer's new position
			for (CSegmentIterator si; si; si++) {
				CSegment *pSegment = &(*si);
				if ((pSegment->m_info.function == nFunction) && (pSegment->m_info.nProducer == info.count)) {
					pSegment->Backup ();
					pSegment->m_info.nProducer = nDelProducer;
					break;
					}
				}
			}
		// remove producer from all robot maker triggers targetting it
		CSideKey key = (-Index (pSegment) - 1, 0); 
		for (int nClass = 0; nClass < 2; nClass++) {
			CTrigger* pTrigger = triggerManager.Trigger (0, nClass);
			for (CTriggerIterator ti (nClass); ti; ti++) {
				if (ti->Info ().type == TT_MATCEN)
					ti->Delete (key);
				}
			}
		undoManager.End (__FUNCTION__);
		}
	}
pSegment->m_info.nProducer = -1;
}

// ----------------------------------------------------------------------------- 

void CSegmentManager::Undefine (short nSegment)
{
	CSegment *pSegment = (nSegment < 0) ? current->Segment () : Segment (nSegment);

pSegment->Backup ();
nSegment = Index (pSegment);
if (pSegment->m_info.function == SEGMENT_FUNC_ROBOTMAKER)
	RemoveProducer (pSegment, RobotMaker (0), m_producerInfo [0], SEGMENT_FUNC_ROBOTMAKER);
else if (pSegment->m_info.function == SEGMENT_FUNC_EQUIPMAKER) 
	RemoveProducer (pSegment, EquipMaker (0), m_producerInfo [1], SEGMENT_FUNC_EQUIPMAKER);
else if (pSegment->m_info.function == SEGMENT_FUNC_PRODUCER) { //remove all fuel cell walls
	undoManager.Begin (__FUNCTION__, udSegments);
	CSide *pSide = pSegment->m_sides;
	for (short nSide = 0; nSide < 6; nSide++, pSide++) {
		if (pSegment->ChildId (nSide) < 0)	// assume no wall if no child segment at the current side
			continue;
		CSegment* pChildSeg = Segment (pSegment->ChildId (nSide));
		if (pChildSeg->m_info.function == SEGMENT_FUNC_PRODUCER)	// don't delete if child segment is fuel center
			continue;
		// if there is a wall and it's a fuel cell delete it
		CSideKey key (nSegment, nSide);
		CWall *pWall = Wall (key);
		if ((pWall != null) && (pWall->Type () == WALL_ILLUSION) && (pSide->BaseTex () == (DLE.IsD1File () ? 322 : 333)))
			wallManager.Delete (pSide->m_info.nWall);
		// if there is a wall at the opposite side and it's a fuel cell delete it
		CSideKey back;
		if (BackSide (key, back)) {
			pWall = Wall (back);
			if ((pWall != null) && (pWall->Type () == WALL_ILLUSION)) {
				CSide* pOppSide = Side (back);
				if (pOppSide->BaseTex () == (DLE.IsD1File () ? 322 : 333))
					wallManager.Delete (pOppSide->m_info.nWall);
				}
			}
		}
	undoManager.End (__FUNCTION__);
	}
pSegment->m_info.childFlags &= ~(1 << MAX_SIDES_PER_SEGMENT);
pSegment->m_info.function = SEGMENT_FUNC_NONE;
}

// ----------------------------------------------------------------------------- 

void CSegmentManager::Delete (short nDelSeg, bool bDeleteVerts)
{
if (Count () < 2)
	return; 
if (nDelSeg < 0)
	nDelSeg = current->SegmentId (); 
if (nDelSeg < 0 || nDelSeg >= Count ()) 
	return; 
if (tunnelMaker.Active ())
	return;

undoManager.Begin (__FUNCTION__, udSegments);
CSegment* pDelSeg = Segment (nDelSeg); 
current->Fix (nDelSeg);
other->Fix (nDelSeg);
pDelSeg->Backup (opDelete);
Undefine (nDelSeg);

// delete any variable lights that use this segment
for (int nSide = 0; nSide < 6; nSide++) {
	CSideKey key (nDelSeg, nSide);
	triggerManager.DeleteTargets (key); 
	lightManager.DeleteVariableLight (key);
	}

// delete any Walls () within segment (if defined)
DeleteWalls (nDelSeg); 

// delete any walls from adjacent segments' sides connecting to this segment
for (short i = 0; i < MAX_SIDES_PER_SEGMENT; i++) {
	if (pDelSeg->ChildId (i) >= 0) {
		CSideKey back, key (nDelSeg, i);
		if (BackSide (key, back))
			wallManager.Delete (Side (back)->m_info.nWall); 
		}
	}

	// delete any Objects () within segment
objectManager.DeleteSegmentObjects (nDelSeg);

// delete any control pSegment with this segment
for (short i = triggerManager.ReactorTriggerCount (); i > 0; )
	triggerManager.ReactorTrigger (--i)->Delete (CSideKey (-nDelSeg - 1));

	// update secret segment number if out of range now
	short nSegment = (short) objectManager.SecretSegment (); 
	if (nSegment >= Count () || nSegment == nDelSeg)
		objectManager.SecretSegment () = 0; 

	// update segment flags
	pDelSeg->UnTag (); 

	// unlink any children with this segment number
	int nSegments = segmentManager.Count ();
	for (int si = 0; si < nSegments; si++) {
		CSegment* pSegment = segmentManager.Segment (si);
		for (short nChild = 0; nChild < MAX_SIDES_PER_SEGMENT; nChild++) {
			if (pSegment->ChildId (nChild) == nDelSeg) {
				// remove nChild number and update nChild bitmask
				pSegment->SetChild (nChild, -1); 

				// define textures, (u, v) and light
				CSide *pSide = pDelSeg->m_sides + nChild;
				SetTextures (CSideKey (si, nChild), pSide->BaseTex (), pSide->OvlTex ()); 
				pSegment->SetUV (nChild, 0, 0); 
				for (short j = 0; j < 4; j++) {
					pSegment->Uvls (nChild) [j].l = pDelSeg->Uvls (nChild) [j].l; 
				}
			}
		}
	}
Remove (nDelSeg);

// delete all unused vertices
vertexManager.DeleteUnused (); 

// make sure current segment numbers are valid
DLE.MineView ()->Refresh (false); 
DLE.ToolView ()->Refresh (); 
undoManager.End (__FUNCTION__);
}

// ----------------------------------------------------------------------------- 

