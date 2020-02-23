// Segment.cpp

#include "stdafx.h"
#include "dle-xp-res.h"

#include < math.h>
#include "define.h"
#include "types.h"
#include "global.h"
#include "mine.h"
#include "matrix.h"
#include "cfile.h"
#include "TriggerManager.h"
#include "WallManager.h"
#include "SegmentManager.h"
#include "TextureManager.h"
#include "palette.h"
#include "dle-xp.h"
#include "robot.h"
#include "cfile.h"

CSegmentManager segmentManager;

// ----------------------------------------------------------------------------- 
// LinkSegments()
//
//  Action - checks 2 Segment () and 2 sides to see if the vertices are identical
//           If they are, then the segment sides are linked and the vertices
//           are removed (nSide1 is the extra side).
//
//  Change - no longer links if segment already has a child
//           no longer links Segment () if vert numbers are not in the right order
//
// ----------------------------------------------------------------------------- 

bool CSegmentManager::Link (short nSegment1, short nSide1, short nSegment2, short nSide2, double margin)
{
	CSegment		* seg1, * seg2; 
	short			i, j; 
	CVertex		v1 [4], v2 [4]; 
	short			fail;
	tVertMatch	match [4]; 

	seg1 = Segment (nSegment1); 
	seg2 = Segment (nSegment2); 

// don't link to a segment which already has a child
if (seg1->Child (nSide1) !=-1 || seg2->Child (nSide2) != -1)
	return FALSE; 

// copy vertices for comparison later (makes code more readable)
for (i = 0; i < 4; i++) {
	v1 [i] = *vertexManager.Vertex (seg1->m_info.verts [sideVertTable [nSide1][i]]);
	v2 [i] = *vertexManager.Vertex (seg2->m_info.verts [sideVertTable [nSide2][i]]);
	match [i].i = -1; 
}

// check to see if all 4 vertices match exactly one of each of the 4 other cube's vertices
fail = 0;   // assume test will pass for now
for (i = 0; i < 4; i++)
	for (j = 0; j < 4; j++)
		if (fabs (v1 [i].v.x - v2 [j].v.x) < margin &&
			 fabs (v1 [i].v.y - v2 [j].v.y) < margin &&
			 fabs (v1 [i].v.z - v2 [j].v.z) < margin)
			if (match [j].i != -1) // if this vertex already matched another vertex then abort
				return false; 
			else
				match [j].i = i;  // remember which vertex it matched
if (match [0].i == -1)
	return FALSE;

static int matches [][4] = {{0,3,2,1},{1,0,3,2},{2,1,0,3},{3,2,1,0}};

for (i = 1; i < 4; i++)
	if (match [i].i != matches [match [0].i][i])
		return FALSE;
// make sure verts match in the correct order
// if not failed and match found for each
LinkSides (nSegment1, nSide1, nSegment2, nSide2, match); 
return TRUE; 
}


// ----------------------------------------------------------------------------- 
// LinkSides()
// ----------------------------------------------------------------------------- 

void CSegmentManager::LinkSides (short nSegment1, short nSide1, short nSegment2, short nSide2, tVertMatch match [4]) 
{
	CSegment*	seg1 = Segment (nSegment1); 
	CSegment*	seg2 = Segment (nSegment2); 
	short			nSegment, nVertex, oldVertex, newVertex; 
	int			i; 

seg1->SetChild (nSide1, nSegment2); 
seg1->m_sides [nSide1].m_info.nBaseTex = 0; 
seg1->m_sides [nSide1].m_info.nOvlTex = 0; 
for (i = 0; i < 4; i++) 
	seg1->m_sides [nSide1].m_info.uvls [i].Clear (); 
seg2->SetChild (nSide2, nSegment1); 
seg2->m_sides [nSide2].m_info.nBaseTex = 0; 
seg2->m_sides [nSide2].m_info.nOvlTex = 0; 
for (i = 0; i < 4; i++) 
	seg2->m_sides [nSide2].m_info.uvls [i].Clear (); 

// merge vertices
for (i = 0; i < 4; i++) {
	oldVertex = seg1->m_info.verts [sideVertTable [nSide1][i]]; 
	newVertex = seg2->m_info.verts [sideVertTable [nSide2][match [i].i]]; 

	// if either vert was marked, then mark the new vert
	vertexManager.Status (newVertex) |= (vertexManager.Status (oldVertex) & MARKED_MASK); 

	// update all Segment () that use this vertex
	if (oldVertex != newVertex) {
		CSegment *segP = Segment (0);
		for (nSegment = 0; nSegment < Count (); nSegment++, segP++)
			for (nVertex = 0; nVertex < 8; nVertex++)
				if (segP->m_info.verts [nVertex] == oldVertex)
					segP->m_info.verts [nVertex] = newVertex; 
		// then delete the vertex
		DeleteVertex (oldVertex); 
		}
	}
}

// ----------------------------------------------------------------------------- 
// unlink_child()
//
// Action - unlinks current cube's children which don't share all four points
//
// Note: 2nd parameter "nSide" is ignored
// ----------------------------------------------------------------------------- 

void CSegmentManager::UnlinkChild (short nParentSeg, short nSide) 
{
  CSegment *parentSegP = Segment (nParentSeg); 

// loop on each side of the parent
//	int nSide;
//  for (nSide = 0; nSide < 6; nSide++) {
int nChildSeg = parentSegP->Child (nSide); 
// does this side have a child?
if (nChildSeg < 0 || nChildSeg >= Count ())
	return;
CSegment *child_seg = Segment (nChildSeg); 
// yes, see if child has a side which points to the parent
int nChildSide;
for (nChildSide = 0; nChildSide < 6; nChildSide++)
	if (child_seg->Child (nChildSide) == nParentSeg) break; 
// if we found the matching side
if (nChildSide < 6) {
// define vert numbers for comparison
	short pv [4], cv [4]; // (short names given for clarity)
	int i;
	for (i = 0; i < 4; i++) {
		pv [i] = parentSegP->m_info.verts [sideVertTable [nSide][i]]; // parent vert
		cv [i] = child_seg->m_info.verts [sideVertTable [nChildSide][i]]; // child vert
		}
	// if they share all four vertices..
	// note: assumes verts increase clockwise looking outward
	if ((pv [0]== cv [3] && pv [1]== cv [2] && pv [2]== cv [1] && pv [3]== cv [0]) ||
		 (pv [0]== cv [2] && pv [1]== cv [1] && pv [2]== cv [0] && pv [3]== cv [3]) ||
		 (pv [0]== cv [1] && pv [1]== cv [0] && pv [2]== cv [3] && pv [3]== cv [2]) ||
		 (pv [0]== cv [0] && pv [1]== cv [3] && pv [2]== cv [2] && pv [3]== cv [1]))
		; // they match, don't mess with them
	else {
		// otherwise, they don't share all four points correctly
		// so unlink the child from the parent
		// and unlink the parent from the child
		undoManager.SetModified (true); 
		undoManager.Lock ();
		ResetSide (nChildSeg, nChildSide); 
		ResetSide (nParentSeg, nSide); 
		undoManager.Unlock ();
		}
	}
else {
	// if the child does not point to the parent, 
	// then just unlink the parent from the child
	ResetSide (nParentSeg, nSide); 
	}
}

// ----------------------------------------------------------------------------- 

bool CSegmentManager::IsPointOfSide (CSegment *segP, int nSide, int nPoint)
{
for (int i = 0; i < 4; i++)
	if (sideVertTable [nSide][i] == nPoint)
		return true;
return false;
}

// ----------------------------------------------------------------------------- 

bool CSegmentManager::IsLineOfSide (CSegment *segP, int nSide, int nLine)
{
for (int i = 0; i < 2; i++)
	if (!IsPointOfSide (segP, nSide, lineVertTable [nLine][i]))
		return false;
return true;
}

// ----------------------------------------------------------------------------- 
//                          Splitpoints()
//
// Action - Splits one point shared between two cubes into two points.
//          New point is added to current cube, other cube is left alone.
//
// ----------------------------------------------------------------------------- 

void CSegmentManager::SplitPoints (void) 
{
CSegment *segP; 
short vert, nSegment, nVertex, nOppSeg, nOppSide; 
bool found; 

if (tunnelMaker.Active ()) {
	ErrorMsg (spline_error_message); 
	return; 
	}
if (VertCount () > (MAX_VERTICES - 1)) {
	ErrorMsg ("Cannot unjoin these points because the\n"
				"maximum number of points is reached."); 
	return; 
	}

segP = Segment (current.m_nSegment); 
vert = segP->m_info.verts [sideVertTable [current.m_nSide][current.m_nPoint]]; 

// check to see if current point is shared by any other cubes
found = FALSE; 
segP = Segment (0);
for (nSegment = 0; (nSegment < Count ()) && !found; nSegment++, segP++)
	if (nSegment != current.m_nSegment)
		for (nVertex = 0; nVertex < 8; nVertex++)
			if (segP->m_info.verts [nVertex] == vert) {
				found = TRUE; 
				break; 
				}
if (!found) {
	ErrorMsg ("This point is not joined with any other point."); 
	return; 
	}

if (QueryMsg("Are you sure you want to unjoin this point?") != IDYES) 
	return; 

undoManager.SetModified (true); 
undoManager.Lock ();
// create a new point (copy of other vertex)
memcpy (vertexManager.Vertex (VertCount ()), vertexManager.Vertex (vert), sizeof (*vertexManager.Vertex (0)));
/*
vertexManager.Vertex (VertCount ()).x = vertexManager.Vertex (vert).x; 
vertexManager.Vertex (VertCount ()).y = vertexManager.Vertex (vert).y; 
vertexManager.Vertex (VertCount ()).z = vertexManager.Vertex (vert).z; 
*/
// replace existing point with new point
segP = Segment (current.m_nSegment); 
segP->m_info.verts [sideVertTable [current.m_nSide][current.m_nPoint]] = VertCount (); 
segP->m_info.wallFlags &= ~MARKED_MASK; 

// update total number of vertices
vertexManager.Status (VertCount ()++) = 0; 

for (short nSide = 0; nSide < 6; nSide++) {
	if (IsPointOfSide (segP, nSide, segP->m_info.verts [sideVertTable [current.m_nSide][current.m_nPoint]]) &&
		 OppositeSide (nOppSeg, nOppSide, current.m_nSegment, nSide)) {
		UnlinkChild (segP->Child (nSide), oppSideTable [nSide]);
		UnlinkChild (current.m_nSegment, nSide); 
		}
	}	
UnlinkChild (current.m_nSegment, nSide); 

SetLinesToDraw (); 
undoManager.Unlock ();
DLE.MineView ()->Refresh ();
INFOMSG("A new point was made for the current point."); 
}

// ----------------------------------------------------------------------------- 
//                         Splitlines()
//
// Action - Splits common lines of two cubes into two lines.
//
// ----------------------------------------------------------------------------- 

void CSegmentManager::SplitLines (void) 
{
  CSegment *segP; 
  short vert [2], nSegment, nVertex, nLine, nOppSeg, nOppSide, i; 
  bool found [2]; 

if (tunnelMaker.Active ()) {
	ErrorMsg (spline_error_message); 
	return; 
	}
if (VertCount () > (MAX_VERTICES - 2)) {
	if (!bExpertMode)
		ErrorMsg ("Cannot unjoin these lines because\nthere are not enought points left."); 
	return; 
	}

segP = Segment (current.m_nSegment); 
for (i = 0; i < 2; i++) {
	nLine = sideLineTable [current.m_nSide][current.m_nLine]; 
	vert [i] = Segment (current.m_nSegment)->m_info.verts [lineVertTable [nLine][i]]; 
	// check to see if current points are shared by any other cubes
	found [i] = FALSE; 
	segP = Segment (0);
	for (nSegment = 0; (nSegment < Count ()) && !found [i]; nSegment++, segP++) {
		if (nSegment != current.m_nSegment) {
			for (nVertex = 0; nVertex < 8; nVertex++) {
				if (segP->m_info.verts [nVertex] == vert [i]) {
					found [i] = TRUE; 
					break; 
					}
				}
			}
		}
	}
if (!(found [0] && found [1])) {
	if (!bExpertMode)
		ErrorMsg ("One or both of these points are not joined with any other points."); 
	return; 
	}

if (QueryMsg ("Are you sure you want to unjoin this line?") != IDYES)
	return; 
undoManager.SetModified (true); 
undoManager.Lock ();
segP = Segment (current.m_nSegment); 
// create a new points (copy of other vertices)
for (i = 0; i < 2; i++)
	if (found [i]) {
		memcpy (vertexManager.Vertex (VertCount ()), vertexManager.Vertex (vert [i]), sizeof (*vertexManager.Vertex (0)));
		/*
		vertices [VertCount ()].x = vertices [vert [i]].x; 
		vertices [VertCount ()].y = vertices [vert [i]].y; 
		vertices [VertCount ()].z = vertices [vert [i]].z; 
		*/
		// replace existing points with new points
		nLine = sideLineTable [current.m_nSide][current.m_nLine]; 
		segP->m_info.verts [lineVertTable [nLine][i]] = VertCount (); 
		segP->m_info.wallFlags &= ~MARKED_MASK; 
		// update total number of vertices
		vertexManager.Status (VertCount ()++) = 0; 
		}
int nSide;
for (nSide = 0; nSide < 6; nSide++) {
	if (IsLineOfSide (segP, nSide, nLine) && 
		 OppositeSide (nOppSeg, nOppSide, current.m_nSegment, nSide)) {
		UnlinkChild (nOppSeg, nOppSide);
		UnlinkChild (current.m_nSegment, nSide); 
		}
	}
SetLinesToDraw(); 
undoManager.Unlock ();
DLE.MineView ()->Refresh ();
INFOMSG ("Two new points were made for the current line."); 
}

// ----------------------------------------------------------------------------- 
//                       Splitsegments()
//
// ACTION - Splits a cube from all other points which share its coordinates
//
//  Changes - Added option to make thin side
// If solidify == 1, the side will keep any points it has in common with other
// sides, unless one or more of its vertices are already solitaire, in which
// case the side needs to get disconnected from its child anyway because that 
// constitutes an error in the level structure.
// ----------------------------------------------------------------------------- 

void CSegmentManager::SplitSegments (int solidify, int nSide) 
{
  CSegment *segP; 
  int vert [4], nSegment, nVertex, i, nFound = 0; 
  bool found [4]; 

if (tunnelMaker.Active ()) {
	ErrorMsg (spline_error_message); 
	return; 
	}

segP = current.Segment (); 
if (nSide < 0)
	nSide = current.m_nSide;
int nChildSeg = segP->Child (nSide); 
if (nChildSeg == -1) {
	ErrorMsg ("The current side is not connected to another cube"); 
	return; 
	}

for (i = 0; i < 4; i++)
	vert [i] = segP->m_info.verts [sideVertTable [nSide][i]]; 
	// check to see if current points are shared by any other cubes
for (nSegment = 0, segP = Segment (0); nSegment < Count (); nSegment++, segP++)
	if (nSegment != current.m_nSegment)
		for (i = 0, nFound = 0; i < 4; i++) {
			found [i] = FALSE;
			for (nVertex = 0; nVertex < 8; nVertex++)
				if (segP->m_info.verts [nVertex] == vert [i]) {
					found [i] = TRUE;
					if (++nFound == 4)
						goto found;
					}
			}
// If a side has one or more solitary points but has a child, there is 
// something wrong. However, nothing speaks against completely unjoining it.
// In fact, this does even cure the problem. So, no error message.
//	ErrorMsg ("One or more of these points are not joined with any other points."); 
//	return; 

found:

if (!solidify && (VertCount () > (MAX_VERTICES - nFound))) {
	ErrorMsg ("Cannot unjoin this side because\nthere are not enough vertices left."); 
	return; 
	}

if (QueryMsg ("Are you sure you want to unjoin this side?") != IDYES)
	return; 

undoManager.SetModified (true); 
undoManager.Lock ();
segP = Segment (current.m_nSegment); 
if (nFound < 4)
	solidify = 0;
if (!solidify) {
	// create new points (copy of other vertices)
	for (i = 0; i < 4; i++) {
		if (found [i]) {
			memcpy (vertexManager.Vertex (VertCount ()), vertexManager.Vertex (vert [i]), sizeof (*vertexManager.Vertex (0)));
			/*
			vertices [VertCount ()].x = vertices [vert [i]].x; 
			vertices [VertCount ()].y = vertices [vert [i]].y; 
			vertices [VertCount ()].z = vertices [vert [i]].z; 
			*/
			// replace existing points with new points
			segP->m_info.verts [sideVertTable [nSide][i]] = VertCount (); 
			segP->m_info.wallFlags &= ~MARKED_MASK; 

			// update total number of vertices
			vertexManager.Status (VertCount ()++) = 0; 
			}
		}
	int nSide;
	for (nSide = 0; nSide < 6; nSide++)
		if (nSide != oppSideTable [nSide])
			UnlinkChild (current.m_nSegment, nSide); 
	SetLinesToDraw(); 
	INFOMSG (" Four new points were made for the current side."); 
	}
else {
	// does this side have a child?
	CSegment *childSegP = Segment (nChildSeg); 
	// yes, see if child has a side which points to the parent
	int nChildSide;
	for (nChildSide = 0; nChildSide < 6; nChildSide++)
		if (childSegP->Child (nChildSide) == current.m_nSegment) 
			break; 
	// if we found the matching side
	if (nChildSide < 6)
		ResetSide (nChildSeg, nChildSide); 
	ResetSide (current.m_nSegment, current.m_nSide); 
	SetLinesToDraw(); 
	}
undoManager.Unlock ();
DLE.MineView ()->Refresh ();
}

// ----------------------------------------------------------------------------- 
// Mine - Joinpoints
// ----------------------------------------------------------------------------- 

void CSegmentManager::JoinPoints (void) 
{
  CSegment *seg1, *seg2; 
 double distance; //v1x, v1y, v1z, v2x, v2y, v2z; 
  int vert1, vert2; 
  CSelection *cur1, *cur2; 

if (tunnelMaker.Active ()) {
	ErrorMsg (spline_error_message); 
	return; 
	}
if (selections [0].m_nSegment== selections [1].m_nSegment) {
	ErrorMsg ("You cannot joint two points on the same cube.\n\n"
				"Hint: The two golden circles represent the current point, \n"
				"and the 'other' cube's current point.  Press 'P' to change the\n"
				"Current () point or press the space bar to switch to the other cube."); 
	return;
	}

if (Current () == &Current1 ()) {
	seg1 = Segment (selections [0].m_nSegment); 
	seg2 = Segment (selections [1].m_nSegment); 
	cur1 = &Current1 (); 
	cur2 = &Current2 (); 
	}
else {
	seg1 = Segment (selections [1].m_nSegment); 
	seg2 = Segment (selections [0].m_nSegment); 
	cur1 = &Current2 (); 
	cur2 = &Current1 (); 
	}
vert1 = seg1->m_info.verts [sideVertTable [cur1->nSide][cur1->nPoint]]; 
vert2 = seg2->m_info.verts [sideVertTable [cur2->nSide][cur2->nPoint]]; 
// make sure verts are different
if (vert1== vert2) {
	ErrorMsg ("These points are already joined."); 
	return; 
	}
// make sure there are distances are close enough
distance = Distance (*vertexManager.Vertex (vert1), *vertexManager.Vertex (vert2)); 
if (distance > JOIN_DISTANCE) {
	ErrorMsg ("Points are too far apart to join"); 
	return; 
	}
if (QueryMsg("Are you sure you want to join the current point\n"
				 "with the 'other' cube's current point?") != IDYES)
	return; 
undoManager.SetModified (true); 
undoManager.Lock ();
// define vert numbers
seg1->m_info.verts [sideVertTable [cur1->nSide][cur1->nPoint]] = vert2; 
// delete any unused vertices
//  delete_unused_vertices(); 
FixChildren(); 
SetLinesToDraw(); 
DLE.MineView ()->Refresh ();
undoManager.Unlock ();
}

// ----------------------------------------------------------------------------- 
// Mine - Joinlines
// ----------------------------------------------------------------------------- 

void CSegmentManager::JoinLines (void) 
{
  CSegment *seg1, *seg2; 
  double v1x [2], v1y [2], v1z [2], v2x [2], v2y [2], v2z [2]; 
  double distance, min_radius; 
  int v1, v2, vert1 [2], vert2 [2]; 
  short match [2]; 
  short i, j, nLine; 
  bool fail; 
  CSelection *cur1, *cur2; 

if (tunnelMaker.Active ()) {
	ErrorMsg (spline_error_message); 
	return; 
	}

if (selections [0].m_nSegment == selections [1].m_nSegment) {
	ErrorMsg ("You cannot joint two lines on the same cube.\n\n"
				"Hint: The two green lines represent the current line, \n"
				"and the 'other' cube's current line.  Press 'L' to change\n"
				"the current line or press the space bar to switch to the other cube."); 
	return;
	}

if (Current ()== &Current1 ()) {
	seg1 = Segment (selections [0].m_nSegment); 
	seg2 = Segment (selections [1].m_nSegment); 
	cur1 = &Current1 (); 
	cur2 = &Current2 (); 
	} 
else {
	seg1 = Segment (selections [1].m_nSegment); 
	seg2 = Segment (selections [0].m_nSegment); 
	cur1 = &Current2 (); 
	cur2 = &Current1 (); 
	}

for (i = 0; i < 2; i++) {
	nLine = sideLineTable [cur1->nSide][cur1->nLine]; 
	v1 = vert1 [i] = seg1->m_info.verts [lineVertTable [nLine][i]]; 
	nLine = sideLineTable [cur2->nSide][cur2->nLine]; 
	v2 = vert2 [i] = seg2->m_info.verts [lineVertTable [nLine][i]]; 
	v1x [i] = vertexManager.Vertex (v1)->v.x; 
	v1y [i] = vertexManager.Vertex (v1)->v.y; 
	v1z [i] = vertexManager.Vertex (v1)->v.z; 
	v2x [i] = vertexManager.Vertex (v2)->v.x; 
	v2y [i] = vertexManager.Vertex (v2)->v.y; 
	v2z [i] = vertexManager.Vertex (v2)->v.z; 
	match [i] =-1; 
	}

// make sure verts are different
if (vert1 [0]== vert2 [0] || vert1 [0]== vert2 [1] ||
	 vert1 [1]== vert2 [0] || vert1 [1]== vert2 [1]) {
	ErrorMsg ("Some or all of these points are already joined."); 
	return; 
	}

// find closest for each point for each corner
for (i = 0; i < 2; i++) {
	min_radius = JOIN_DISTANCE; 
	for (j = 0; j < 2; j++) {
		distance = sqrt((v1x [i] - v2x [j]) * (v1x [i] - v2x [j])
					+ (v1y [i] - v2y [j]) * (v1y [i] - v2y [j])
					+ (v1z [i] - v2z [j]) * (v1z [i] - v2z [j])); 
		if (distance < min_radius) {
			min_radius = distance; 
			match [i] = j;  // remember which vertex it matched
			}
		}
	}

// make sure there are distances are close enough
if (min_radius == JOIN_DISTANCE) {
	ErrorMsg ("Lines are too far apart to join"); 
	return; 
	}

if (QueryMsg("Are you sure you want to join the current line\n"
				 "with the 'other' cube's current line?") != IDYES)
	return; 
fail = FALSE; 
// make sure there are matches for each and they are unique
fail = (match [0] == match [1]);
if (fail) {
	match [0] = 1; 
	match [1] = 0; 
	}
undoManager.SetModified (true); 
undoManager.Lock ();
// define vert numbers
for (i = 0; i < 2; i++) {
	nLine = sideLineTable [cur1->nSide][cur1->nLine]; 
	seg1->m_info.verts [lineVertTable [nLine][i]] = vert2 [match [i]]; 
	}
FixChildren(); 
SetLinesToDraw(); 
DLE.MineView ()->Refresh ();
undoManager.Unlock ();
}

// ----------------------------------------------------------------------------- 
// FixChildren()
//
// Action - Updates linkage between current segment and all other Segment ()
// ----------------------------------------------------------------------------- 

void CSegmentManager::FixChildren (void)
{
short nNewSide, nSide, nSegment, nNewSeg; 

nNewSeg = current.m_nSegment; 
nNewSide = current.m_nSide; 
CSegment *segP = Segment (0),
			*newSegP = Segment (nNewSeg);
CVertex	*vSeg, 
			*vNewSeg = vertexManager.Vertex (newSegP->m_info.verts [0]);
for (nSegment = 0; nSegment < Count (); nSegment++, segP) {
	if (nSegment != nNewSeg) {
		// first check to see if Segment () are any where near each other
		// use x, y, and z coordinate of first point of each segment for comparison
		vSeg = vertexManager.Vertex (segP->m_info.verts [0]);
		if (fabs ((double) (vNewSeg->v.x - vSeg->v.x)) < 10.0 &&
		    fabs ((double) (vNewSeg->v.y - vSeg->v.y)) < 10.0 &&
		    fabs ((double) (vNewSeg->v.z - vSeg->v.z)) < 10.0) {
			for (nSide = 0; nSide < 6; nSide++) {
				if (!Link (nNewSeg, nNewSide, nSegment, nSide, 3)) {
					// if these Segment () were linked, then unlink them
					if ((newSegP->Child (nNewSide) == nSegment) && (segP->Child (nSide) == nNewSeg)) {
						newSegP->SetChild (nNewSide, -1); 
						segP->SetChild (nSide, -1); 
						}
					}
				}
			}
		}
	}
}

// ----------------------------------------------------------------------------- 
//		       Joinsegments()
//
//  ACTION - Joins sides of current Segment ().  Finds closest corners.
//	     If sides use vertices with the same coordinates, these vertices
//	     are merged and the cube's are connected together.  Otherwise, a
//           new cube is added added.
//
//  Changes - Added option to solidifyally figure out "other cube"
// ----------------------------------------------------------------------------- 

void CSegmentManager::Join (int solidify)
{
	CSegment *segP; 
	CSegment *seg1, *seg2; 
	short h, i, j, nSide, nNewSeg, nSegment; 
	CVertex v1 [4], v2 [4]; 
	double radius, min_radius, max_radius, totalRad, minTotalRad; 
	tVertMatch match [4]; 
	bool fail; 
	CSelection *cur1, *cur2, my_cube; 

if (tunnelMaker.Active ()) {
	ErrorMsg (spline_error_message); 
	return; 
	}

// figure out "other' cube
if (solidify) {
	if (Segment (current.m_nSegment)->Child (current.m_nSide) != -1) {
		if (!bExpertMode)
			ErrorMsg ("The current side is already joined to another cube"); 
		return; 
		}
	cur1 = Current (); 
	cur2 = &my_cube; 
	my_cube.nSegment = -1;
	// find first cube (other than this cube) which shares all 4 points
	// of the current side (points must be < 5.0 away)
	seg1 = Segment (cur1->nSegment); 
	for (i = 0; i < 4; i++) {
		memcpy (&v1 [i], vertexManager.Vertex (seg1->m_info.verts [sideVertTable [cur1->nSide][i]]), sizeof (CVertex));
		}
	minTotalRad = 1e300;
	for (nSegment = 0, seg2 = Segment (0); nSegment < Count (); nSegment++, seg2++) {
		if (nSegment== cur1->nSegment)
			continue; 
		for (nSide = 0; nSide < 6; nSide++) {
			fail = FALSE; 
			for (i = 0; i < 4; i++) {
				memcpy (&v2 [i], vertexManager.Vertex (seg2->m_info.verts[sideVertTable[nSide][i]]), sizeof (CVertex));
				}
			for (i = 0; i < 4; i++)
				match [i].b = 0; 
			for (i = 0; i < 4; i++) {
				match [i].i = -1; 
				match [i].d = 1e300;
				for (j = 0, h = -1; j < 4; j++) {
					if (match [j].b)
						continue;
					radius = Distance (v1 [i], v2 [j]);
					if ((radius <= 10.0) && (radius < match [i].d)) {
						h = j;  // remember which vertex it matched
						match [i].d = radius;
						}
					}
				if (h < 0) {
					fail = TRUE;
					break;
					}
				match [i].i = h;
				match [h].b = i;
				}
			if (fail)
				continue;
			totalRad = 0;
			for (i = 0; i < 4; i++)
				totalRad += match [i].d;
			if (minTotalRad > totalRad) {
				minTotalRad = totalRad;
				my_cube.nSegment = nSegment; 
				my_cube.nSide = nSide; 
				my_cube.nPoint = 0; // should not be used
			// force break from loops
				if (minTotalRad == 0) {
					nSide = 6; 
					nSegment = Count (); 
					}
				}
			}
		}
	if (my_cube.nSegment < 0) {
		if (!bExpertMode)
			ErrorMsg ("Could not find another cube whose side is within\n"
						"10.0 units from the current side"); 
		return; 
		}
	}
else
	if (Current ()== &Current1 ()) {
		cur1 = &Current1 (); 
		cur2 = &Current2 (); 
		}
	else {
		cur1 = &Current2 (); 
		cur2 = &Current1 (); 
		}

if (cur1->nSegment == cur2->nSegment) {
	if (!bExpertMode)
		ErrorMsg ("You cannot joint two sides on the same cube.\n\n"
					"Hint: The two red squares represent the current side, \n"
					"and the 'other' cube's current side.  Press 'S' to change\n"
					"the current side or press the space bar to switch to the other cube."); 
	return; 
	}

seg1 = Segment (cur1->nSegment); 
seg2 = Segment (cur2->nSegment); 

// figure out matching corners to join to.
// get coordinates for calulaction and set match = none
for (i = 0; i < 4; i++) {
	memcpy (&v1 [i], vertexManager.Vertex (seg1->m_info.verts [sideVertTable [cur1->nSide][i]]), sizeof (CVertex)); 
	memcpy (&v2 [i], vertexManager.Vertex (seg2->m_info.verts [sideVertTable [cur2->nSide][i]]), sizeof (CVertex)); 
	match [i].i = -1; 
	}

// find closest for each point for each corner
for (i = 0; i < 4; i++) {
	min_radius = JOIN_DISTANCE; 
	for (j = 0; j < 4; j++) {
		radius = Distance (v1 [i], v2 [j]);
		if (radius < min_radius) {
			min_radius = radius; 
			match [i].i = j;  // remember which vertex it matched
			}
		}
	}

fail = false; 
for (i = 0; i < 4; i++)
	if (match [i].i == -1) {
		fail = true; 
		break; 
	}

// make sure there are matches for each and they are unique
if (!fail)
	fail = (match [0].i == match [1].i) ||
			 (match [0].i == match [2].i) ||
			 (match [0].i == match [3].i) ||
			 (match [1].i == match [2].i) ||
			 (match [1].i == match [3].i) ||
			 (match [2].i == match [3].i);

if (fail) {
	int offset = (4 + cur1->nPoint - (3 - cur2->nPoint)) % 4; 
	match [0].i = (offset + 3) % 4; 
	match [1].i = (offset + 2) % 4; 
	match [2].i = (offset + 1) % 4; 
	match [3].i = (offset + 0) % 4; 
	}

// determine min and max distances
min_radius = JOIN_DISTANCE; 
max_radius = 0; 
for (i = 0; i < 4; i++) {
	j = match [i].i; 
	radius = Distance (v1 [i], v2 [j]);
	minRadius = min (min_radius, radius); 
	maxRadius = max (max_radius, radius); 
	}

// make sure there are distances are close enough
if (max_radius >= JOIN_DISTANCE) {
	if (!bExpertMode)
		ErrorMsg ("Sides are too far apart to join.\n\n"
					 "Hint: Cubes should not exceed 200 in any dimension\n"
					 "or they will distort when viewed from close up."); 
	return; 
	}

// if Segment () are too close to put a new segment between them, 
// then solidifyally link them together without asking
if (min_radius <= 5) {
	undoManager.SetModified (true); 
	undoManager.Lock ();
	LinkSides (cur1->nSegment, cur1->nSide, cur2->nSegment, cur2->nSide, match); 
	SetLinesToDraw(); 
	undoManager.Unlock ();
	DLE.MineView ()->Refresh ();
	return; 
	}

if (QueryMsg ("Are you sure you want to create a new segment which\n"
				  "connects the current side with the 'other' side?\n\n"
				  "Hint: Make sure you have the current point of each segment\n"
				  "on the corners you to connected.\n"
				  "(the 'P' key selects the current point)") != IDYES)
	return; 

nNewSeg = Count (); 
if (!(Count () < MAX_SEGMENTS)) {
	if (!bExpertMode)
		ErrorMsg ("The maximum number of Segment () has been reached.\n"
					"Cannot add any more Segment ()."); 
	return; 
	}
segP = Segment (nNewSeg); 

undoManager.SetModified (true); 
undoManager.Lock ();
// define children and special child
// first clear all sides
segP->m_info.childFlags = 0; 
for (i = 0; i < MAX_SIDES_PER_SEGMENT; i++)  /* no remaining children */
	segP->SetChild (i, -1); 

// now define two sides:
// near side has opposite side number cube 1
segP->SetChild (oppSideTable [cur1->nSide], cur1->nSegment); 
// far side has same side number as cube 1
segP->SetChild (cur1->nSide, cur2->nSegment); 
segP->m_info.owner = -1;
segP->m_info.group = -1;
segP->m_info.function = 0; 
segP->m_info.nMatCen =-1; 
segP->m_info.value =-1; 

// define vert numbers
for (i = 0; i < 4; i++) {
	segP->m_info.verts [oppSideVertTable [cur1->nSide][i]] = seg1->m_info.verts [sideVertTable [cur1->nSide][i]]; 
	segP->m_info.verts [sideVertTable [cur1->nSide][i]] = seg2->m_info.verts [sideVertTable [cur2->nSide][match [i].i]]; 
	}

// define Walls ()
segP->m_info.wallFlags = 0; // unmarked
for (nSide = 0; nSide < MAX_SIDES_PER_SEGMENT; nSide++)
	segP->m_sides [nSide].m_info.nWall = NO_WALL; 

// define sides
for (nSide = 0; nSide < MAX_SIDES_PER_SEGMENT; nSide++) {
	if (segP->Child (nSide) == -1) {
		SetTextures (CSideKey (nNewSeg, nSide), seg1->m_sides [cur1->nSide].m_info.nBaseTex, seg1->m_sides [cur1->nSide].m_info.nOvlTex); 
		Segment (nNewSeg)->SetUV (nSide, 0, 0); 
		}
	else {
		SetTextures (CSideKey (nNewSeg), nSide, 0, 0); 
		for (i = 0; i < 4; i++) 
			segP->m_sides [nSide].m_info.uvls [i].Clear (); 
		}
	}

// define static light
segP->m_info.staticLight = seg1->m_info.staticLight; 

// update cur segment
seg1->SetChild (cur1->nSide, nNewSeg); 
SetTextures (cur1->nSegment, cur1->nSide, 0, 0); 
for (i = 0; i < 4; i++) 
	seg1->m_sides [cur1->nSide].m_info.uvls [i].Clear (); 
seg2->SetChild (cur2->nSide, nNewSeg); 
SetTextures (CSideKey (cur2->nSegment, cur2->nSide), 0, 0); 
for (i = 0; i < 4; i++) 
	seg2->m_sides [cur2->nSide].m_info.uvls [i].Clear (); 

// update number of Segment () and vertices
Count ()++; 
undoManager.Unlock ();
SetLinesToDraw(); 
DLE.MineView ()->Refresh ();
}

// -----------------------------------------------------------------------------

bool CSegmentManager::SplitSegment (void)
{
	CSegment*	centerSegP = current.Segment (), *segP, *childSegP;
	short			nCenterSeg = short (centerSegP - Segment (0));
	short			nSegment, nChildSeg;
	short			nSide, nOppSide, nChildSide;
	short			vertNum, nWall;
	CVertex		segCenter, *segVert, *centerSegVert;
	bool			bVertDone [8], bUndo;
	int			h, i, j, k;
	short			oppSides [6] = {2,3,0,1,5,4};

if (Count () >= MAX_SEGMENTS - 6) {
	ErrorMsg ("Cannot split this cube because\nthe maximum number of cubes would be exceeded."); 
	return false;
	}
bUndo = undoManager.SetModified (true); 
undoManager.Lock ();
//h = VertCount ();
// compute segment center
vertexManager.Add (8);
segCenter = CalcCenter (Index (centerSegP));
// add center segment
// compute center segment vertices
memset (bVertDone, 0, sizeof (bVertDone));
for (nSide = 0; nSide < 6; nSide++) {
	for (vertNum = 0; vertNum < 4; vertNum++) {
		j = sideVertTable [nSide][vertNum];
		if (bVertDone [j])
			continue;
		bVertDone [j] = true;
		centerSegVert = vertexManager.Vertex (centerSegP->m_info.verts [j]);
		segVert = vertexManager.Vertex (h + j);
		*segVert = Average (*centerSegVert, segCenter);
		//centerSegP->m_info.verts [j] = h + j;
		}
	}

#if 1
// create the surrounding segments
for (nSegment = Count (), nSide = 0; nSide < 6; nSegment++, nSide++) {
	segP = Segment (nSegment);
	nOppSide = oppSides [nSide];
	for (vertNum = 0; vertNum < 4; vertNum++) {
		i = sideVertTable [nSide][vertNum];
		segP->m_info.verts [i] = centerSegP->m_info.verts [i];
		if ((nSide & 1) || (nSide >= 4)) {
			i = lineVertTable [sideLineTable [nSide][0]][0];
			j = lineVertTable [sideLineTable [nOppSide][2]][0];
			segP->m_info.verts [j] = h + i;
			i = lineVertTable [sideLineTable [nSide][0]][1];
			j = lineVertTable [sideLineTable [nOppSide][2]][1];
			segP->m_info.verts [j] = h + i;
			i = lineVertTable [sideLineTable [nSide][2]][0];
			j = lineVertTable [sideLineTable [nOppSide][0]][0];
			segP->m_info.verts [j] = h + i;
			i = lineVertTable [sideLineTable [nSide][2]][1];
			j = lineVertTable [sideLineTable [nOppSide][0]][1];
			segP->m_info.verts [j] = h + i;
			}
		else {
			i = lineVertTable [sideLineTable [nSide][0]][0];
			j = lineVertTable [sideLineTable [nOppSide][2]][1];
			segP->m_info.verts [j] = h + i;
			i = lineVertTable [sideLineTable [nSide][0]][1];
			j = lineVertTable [sideLineTable [nOppSide][2]][0];
			segP->m_info.verts [j] = h + i;
			i = lineVertTable [sideLineTable [nSide][2]][0];
			j = lineVertTable [sideLineTable [nOppSide][0]][1];
			segP->m_info.verts [j] = h + i;
			i = lineVertTable [sideLineTable [nSide][2]][1];
			j = lineVertTable [sideLineTable [nOppSide][0]][0];
			segP->m_info.verts [j] = h + i;
			}
		}
	InitSegment (nSegment);
	if ((segP->SetChild (nSide, centerSegP->Child (nSide))) > -1) {
		for (childSegP = Segment (segP->Child (nSide)), nChildSide = 0; nChildSide < 6; nChildSide++)
			if (childSegP->Child (nChildSide) == nCenterSeg) {
				childSegP->SetChild (nChildSide, nSegment);
				break;
				}
			}
	segP->SetChild (nOppSide, nCenterSeg);
	centerSegP->SetChild (nSide, nSegment);
	nWall = centerSegP->m_sides [nSide].m_info.nWall;
	segP->m_sides [nSide].m_info.nWall = nWall;
	if ((nWall >= 0) && (nWall != NO_WALL)) {
		Walls (nWall)->m_nSegment = nSegment;
		centerSegP->m_sides [nSide].m_info.nWall = NO_WALL;
		}
	}
// relocate center segment vertex indices
memset (bVertDone, 0, sizeof (bVertDone));
for (nSide = 0; nSide < 6; nSide++) {
	for (vertNum = 0; vertNum < 4; vertNum++) {
		j = sideVertTable [nSide][vertNum];
		if (bVertDone [j])
			continue;
		bVertDone [j] = true;
		centerSegP->m_info.verts [j] = h + j;
		}
	}
// join adjacent sides of the segments surrounding the center segment
#if 1
for (nSegment = 0, segP = Segment (Count ()); nSegment < 5; nSegment++, segP++) {
	for (nSide = 0; nSide < 6; nSide++) {
		if (segP->Child (nSide) >= 0)
			continue;
		for (nChildSeg = nSegment + 1, childSegP = Segment (Count () + nChildSeg); 
			  nChildSeg < 6; 
			  nChildSeg++, childSegP++) {
			for (nChildSide = 0; nChildSide < 6; nChildSide++) {
				if (childSegP->Child (nChildSide)  >= 0)
					continue;
				h = 0;
				for (i = 0; i < 4; i++) {
					k = segP->m_info.verts [sideVertTable [nSide][i]];
					for (j = 0; j < 4; j++) {
						if (k == childSegP->m_info.verts [sideVertTable [nChildSide][j]]) {
							h++;
							break;
							}
						}
					}
				if (h == 4) {
					segP->SetChild (nSide, Count () + nChildSeg);
					childSegP->SetChild (nChildSide, Count () + nSegment);
					break;
					}
				}
			}
		}
	}
#endif
Count () += 6;
#endif
undoManager.Unlock ();
DLE.MineView ()->Refresh ();
return true;
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
//eof segmentmanager.cpp