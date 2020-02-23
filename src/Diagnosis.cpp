
#include "stdafx.h"
#include "afxpriv.h"
#include "dle-xp.h"

#include "dlcDoc.h"
#include "mineview.h"
#include "toolview.h"

#include "PaletteManager.h"
#include "textures.h"
#include "global.h"
#include "FileManager.h"

#include <math.h>

typedef struct tBugPos {
	int	nSegment;
	int	nSide;
	int	nLine;
	int	nPoint;
	int	nChild;
	int	nWall;
	int	nTrigger;
	int	nObject;
} tBugPos;

//------------------------------------------------------------------------------

void CDiagTool::ClearBugList ()
{
if (!(m_bInited && theMine && IsWindow (m_hWnd)))
	return;

	CListBox	*plb = LBBugs ();

if (!plb)
	return;

	int h = plb->GetCount ();
	char szText [256];

tBugPos *pbp;
int i;
for (i = 0; i < h; i++) {
	plb->GetText (i, szText);
	if (!strchr (szText, '['))
		if (pbp = (tBugPos *) plb->GetItemDataPtr (i)) {
			delete pbp;
			plb->SetItemDataPtr (i, null);
			}
	}
plb->ResetContent ();
m_nErrors [0] =
m_nErrors [1] = 0;
m_statsWidth = 0;
}

//------------------------------------------------------------------------------

bool CDiagTool::TagSegment (short nSegment) 
{
if ((nSegment < 0) || (nSegment >= segmentManager.Count ()))
	return false;
segmentManager.Segment (nSegment)->UnTag (TAGGED_MASK);
segmentManager.Segment (nSegment)->Tag (TAGGED_MASK);
return true;
}

//------------------------------------------------------------------------------

void CDiagTool::OnShowBug (void)
{
CHECKMINE;

	bool bCurSeg;
	CWall *pWall = null;
	int nWall;

int i = LBBugs ()->GetCurSel ();
if ((i < 0) || (i >= LBBugs ()->GetCount ()))
	return;
tBugPos *pbp = (tBugPos *) LBBugs ()->GetItemDataPtr (i);
if (!pbp)
	return;
segmentManager.UnTagAll ();
if (bCurSeg = TagSegment (pbp->nSegment))
	current->SetSegmentId (pbp->nSegment);
TagSegment (pbp->nChild);
if ((pbp->nSide >= 0) && (pbp->nSide < MAX_SIDES_PER_SEGMENT))
	current->SetSideId (pbp->nSide);
if ((pbp->nLine >= 0) && (pbp->nLine < 4))
	current->SetEdge (pbp->nLine);
if ((pbp->nPoint >= 0) && (pbp->nPoint < 8))
	current->SetPoint (pbp->nPoint);
if ((pbp->nWall >= 0) && (pbp->nWall < wallManager.WallCount ()))
	nWall = pbp->nWall;
else if ((pbp->nTrigger >= 0) && (pbp->nTrigger < triggerManager.WallTriggerCount ()))
	pWall = wallManager.FindByTrigger (pbp->nTrigger);
if ((pWall != null) && TagSegment (pWall->m_nSegment))
	if (bCurSeg) {
		other->m_nSegment = pWall->m_nSegment;
		other->m_nSide = pWall->m_nSide;
		}
	else {
		current->SetSegmentId (pWall->m_nSegment);
		current->SetSideId (pWall->m_nSide);
		}
if ((pbp->nObject >= 0) && (pbp->nObject < objectManager.Count ()))
	current->SetObjectId (pbp->nObject);
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------
// CalcFlatnessRatio ()
//
// 1) calculates the average length lines 0 and 2
// 2) calculates the midpoints of lines 1 and 3
// 3) calculates the length between these two midpoints
// 4) calculates the ratio of the midpoint length over the line lengths
// 5) repeats this for lines 1 and 3 and midpoints 0 and 2
// 6) returns the minimum of the two ratios 
//------------------------------------------------------------------------

double CDiagTool::CalcFlatnessRatio (short nSegment, short nSide) 
{
	CVertex		midpoint1, midpoint2;
	double		length1, length2, ave_length, mid_length;
	double		ratio;
	CVertex		vert [4];
  // copy vertnums into an array
	CSegment*	pSegment = segmentManager.Segment (nSegment);
	CSide*		pSide = pSegment->Side (nSide);
	short			i, n = pSide->VertexCount ();

if (n < 3)
	return 1.0;

for (i = 0; i < n; i++)
	vert [i] = *pSegment->Vertex (nSide, i);

length1 = CalcDistance (vert + 0, vert + 1, vert + 2);
length2 = (n < 4) ? 0.0 : CalcDistance (vert + 0, vert + 1, vert + 3);
ave_length = (length1 + length2) / 2;
midpoint1 = Average (vert [0], vert [1]);
midpoint2 = (n < 4) ? midpoint1 : Average (vert [2], vert [3]);
mid_length = Distance (midpoint1, midpoint2);
ratio = mid_length / ave_length;

length1 = CalcDistance (vert + 1, vert + 2, vert + 3);
length2 = (n < 4) ? 0.0 : CalcDistance (vert + 1, vert + 2, vert + 0);
ave_length = (length1 + length2) / 2;
midpoint1 = Average (vert [1], vert [2]);
midpoint2 = (n < 4) ? midpoint1 : Average (vert [3], vert [0]);
mid_length = Distance (midpoint1, midpoint2);
return min (ratio, mid_length / ave_length);
}

//--------------------------------------------------------------------------
// CalcDistance
//
// Action - calculates the distance from a line to a point
//--------------------------------------------------------------------------

double CDiagTool::CalcDistance (CVertex* v1, CVertex* v2, CVertex* v3)
{
// normalize all points to vector 1
CDoubleVector A = *v2 - *v1;
CDoubleVector B = *v3 - *v1;

// use formula from page 505 of "Calculase and Analytical Geometry" Fifth Addition
// by Tommas/Finney, Addison-Wesley Publishing Company, June 1981
//          B * A
// B2 = B - ----- A
//          A * A

double a2 = A ^ A;
double c = (a2 != 0) ? (B ^ A) / a2 : 0;
CDoubleVector C = B - (A * c);
return C.Mag ();
}

//--------------------------------------------------------------------------
// CalcAngle ()
//--------------------------------------------------------------------------

double CDiagTool::CalcAngle (short vert0, short vert1, short vert2, short vert3) 
{
  CDoubleVector line1, line2, line3, orthog;
  double ratio;
  double dot_product, magnitude1, magnitude2, angle;
  CVertex* v0 = vertexManager.Vertex (vert0);
  CVertex* v1 = vertexManager.Vertex (vert1);
  CVertex* v2 = vertexManager.Vertex (vert2);
  CVertex* v3 = vertexManager.Vertex (vert3);
      // define lines
line1 = *v1 - *v0;
line2 = *v2 - *v0;
line3 = *v3 - *v0;
// use cross product to calcluate orthogonal vector
orthog = -CrossProduct (line1, line2);
// use dot product to determine angle A dot B = |A|*|B| * cos (angle)
// therfore: angle = acos (A dot B / |A|*|B|)
dot_product = line3 ^ orthog;
magnitude1 = line3.Mag ();
magnitude2 = orthog.Mag ();
if (dot_product == 0 || magnitude1 == 0 || magnitude2 == 0)
	angle = (200.0 * M_PI)/180.0; 
else {
	ratio = dot_product/ (magnitude1*magnitude2);
	ratio = ( (double) ( (int) (ratio*1000.0))) / 1000.0; // bug int 9/21/96
	if (ratio < -1.0 || ratio > (double)1.0) 
		angle = (199.0 * M_PI)/180.0;
	else
		angle = acos (ratio);
	}
return fabs (angle);  // angle should be positive since acos returns 0 to PI but why not be sure
}

//--------------------------------------------------------------------------
//  CheckId (local subroutine)
//
//  ACTION - Checks the range of the ID number of an object of a particular
//           type.
//
//  RETURN - Returns TRUE if ID is out of range.  Otherwise, FALSE.
//--------------------------------------------------------------------------

int CDiagTool::CheckId (ubyte &type, ubyte &id) 
{
switch (type) {
	case OBJ_ROBOT: /* an evil enemy */
		if (id < 0 || id > (DLE.IsD1File () ? MAX_ROBOT_ID_D1 : MAX_ROBOT_ID_D2)) {
			return 1;
		}
		break;

	case OBJ_HOSTAGE: /* a hostage you need to rescue */
#if 0
		if (id< 0 || id >1) {
			return 1;
		}
#endif
		break;

	case OBJ_PLAYER: {
		int bError  = (id < 0) || (id >= MAX_PLAYERS);
		if (!m_bAutoFixBugs)
			return bError;
		id = m_playerId++;
		return 2 * bError;
		}
		break;

	case OBJ_POWERUP: /* a powerup you can pick up */
		if (id < 0 || id > MAX_POWERUP_IDS) {
			return 1;
		}
		break;

	case OBJ_REACTOR: /* the control center */
		if (DLE.IsD1File ()) {
			if (id >= 0 || id <= 25) {
				return 0;
			}
		} else {
			if (id >= 1 || id <= 6) {
				return 0;
			}
		}
		if (!m_bAutoFixBugs)
			return 1;
		id = DLE.IsD1File () ? 1 : 2;
		return 2;
		break;

	case OBJ_COOP: {
		int bError = (id < MAX_PLAYERS) || (id >= MAX_PLAYERS + MAX_COOP_PLAYERS);
		if (!m_bAutoFixBugs)
			return bError;
		id = m_coopId++;
		return 2 * bError;
		}
		break;

	case OBJ_WEAPON: // (d2 only)
		if (id != SMALLMINE_ID) {
			if (!m_bAutoFixBugs)
				return 1;
			id = SMALLMINE_ID;
			return 2;
		}
	}
	return 0;
}

//--------------------------------------------------------------------------
// CDiagTool - WMTimer
//
// Action - Fills list box with data about the theMine->  This
//          routine is called once after the dialog is opened.
//
//--------------------------------------------------------------------------

void CDiagTool::OnCheckMine (void)
{
CHECKMINE;

UpdateData (TRUE);
ClearBugList ();
m_bCheckMsgs = true;
m_playerId = 0;
m_coopId = MAX_PLAYERS;
if (m_bAutoFixBugs)
	undoManager.Begin (__FUNCTION__, udAll);
  // set mode to BLOCK mode to make errors appear in red
DLE.MineView ()->SetSelectMode (BLOCK_MODE);

// now do actual checking
DLE.MainFrame ()->InitProgress (segmentManager.Count () * 3 + 
										  vertexManager.Count () +
										  wallManager.WallCount () * 2 +
										  triggerManager.WallTriggerCount () * 3 +
										  objectManager.Count () * 2 +
										  triggerManager.ObjTriggerCount ());
//if (!CheckBotGens ())
//	if (!CheckEquipGens ())
		if (!CheckSegments ())
			if (!CheckSegTypes ())
				if (!CheckWalls ())
					if (!CheckTriggers ())
						if (!CheckObjects ())
							if (!CheckVertices ())
								CheckLights ()
								;
DLE.MainFrame ()->Progress ().DestroyWindow ();
LBBugs ()->SetCurSel (0);
if (m_bAutoFixBugs)
	undoManager.End (__FUNCTION__);
}

//--------------------------------------------------------------------------
// CDiagTool - UpdateStats ()
//
// Action  - Updates error and warning counts.
//
// Returns - TRUE if 1000 warnings or errors have occured 
//
// Parameters - error_msg = null terminated string w/ error message
//              error     = set to a 1 if this is an error
//              warning   = set to a 1 if this is a warning 
//--------------------------------------------------------------------------

void CDiagTool::UpdateStatsWidth (char* s)
{
	CClientDC dc(this);

CFont * f = LBBugs ()->GetFont();
dc.SelectObject(f);

CSize sz = dc.GetTextExtent (s, (int) _tcslen (s));
sz.cx += 3 * ::GetSystemMetrics(SM_CXBORDER);
if (sz.cx > m_statsWidth) 
	LBBugs ()->SetHorizontalExtent (m_statsWidth = sz.cx);
}

//--------------------------------------------------------------------------

bool CDiagTool::UpdateStats (char *szError, int nErrorLevel, 
									  int nSegment, int nSide, int nLine, int nPoint, 
									  int nChild, int nWall, int nTrigger, int objnum)
{
if (!(szError && *szError))
	return false;
if (!(m_bShowWarnings || !strstr (szError, "WARNING:")))
	return true;
int h = AddMessage (szError, -1, true);
UpdateStatsWidth (szError);
if (h >= 0) {
	tBugPos *pbp = new tBugPos;
	if (!pbp)
		return false;
	pbp->nSegment = nSegment;
	pbp->nSide = nSide;
	pbp->nLine = nLine;
	pbp->nPoint = nPoint;
	pbp->nWall = nWall;
	pbp->nTrigger = nTrigger;
	pbp->nObject = objnum;
	pbp->nChild = nChild;
	LBBugs ()->SetItemDataPtr (h, (void *) pbp);
	}
*szError = '\0';
m_nErrors [nErrorLevel]++;
if (m_nErrors [0] < 1000 && m_nErrors [1] < 1000)
	return false;
LBBugs ()->AddString ("Maximum number of errors/warnings reached");
UpdateStatsWidth (szError);
return true;
}

//--------------------------------------------------------------------------

bool CDiagTool::CheckSegTypes (void) 
{
if (theMine == null) 
	return false;

	short	i, nBotGens = 0, nEquipGens = 0, nProducers = 0;
	CSegment	*pSegment = segmentManager.Segment (0);

for (i = segmentManager.Count (); i; i--, pSegment++)
	switch (pSegment->m_info.function) {
		case SEGMENT_FUNC_ROBOTMAKER:
			nBotGens++;
			break;
		case SEGMENT_FUNC_EQUIPMAKER:
			nEquipGens++;
			break;
		case SEGMENT_FUNC_PRODUCER:
			nProducers++;
			break;
		default:
			break;
	}
if (nBotGens != segmentManager.RobotMakerCount ()) {
	if (m_bAutoFixBugs) {
		sprintf_s (message, sizeof (message), "FIXED: Invalid robot maker count");
		segmentManager.RobotMakerCount () = nBotGens;
		}
	else
		sprintf_s (message, sizeof (message), "ERROR: Invalid robot maker count");
	if (UpdateStats (message, 1, -1, -1, -1, -1, -1, -1, -1, -1))
		return true;
	}
return false;
}

//--------------------------------------------------------------------------
// CDiagTool  LBBugs ()()
//
//  Checks for:
//    out of range data
//    valid segments geometry
//--------------------------------------------------------------------------

bool CDiagTool::CheckSegments (void) 
{
if (theMine == null) 
	return false;

  short nSegment, nSide, nChild, nSide2;
  short i, j;
  CSegment *pSegment = segmentManager.Segment (0);

  // check Segments ()
  //--------------------------------------------------------------
short sub_errors = m_nErrors [0];
short sub_warnings = m_nErrors [1];
LBBugs ()->AddString ("[Segments]");

for (nSegment = 0; nSegment < segmentManager.Count (); nSegment++, pSegment++) {
	DLE.MainFrame ()->Progress ().StepIt ();
// check geometry of segment
// 	Given that each point has 3 lines (called L1, L2, and L3), 
//	and an orthogonal vector of L1 and L2 (called V1), the angle
//	between L3 and V1 must be less than PI/2.
//
	if (pSegment->m_info.function == SEGMENT_FUNC_ROBOTMAKER) {
		if ((pSegment->m_info.nProducer >= segmentManager.RobotMakerCount ()) || (segmentManager.RobotMaker (pSegment->m_info.nProducer)->m_info.nSegment != nSegment)) {
	 		sprintf_s (message, sizeof (message), "%s: Segment has invalid type (segment=%d))", m_bAutoFixBugs ? "FIXED" : "ERROR", nSegment);
			if (m_bAutoFixBugs)
				segmentManager.Undefine (nSegment);
			}
		}
	if (pSegment->m_info.function == SEGMENT_FUNC_EQUIPMAKER) {
		if ((pSegment->m_info.nProducer >= segmentManager.EquipMakerCount ()) || (segmentManager.EquipMaker (pSegment->m_info.nProducer)->m_info.nSegment != nSegment)) {
	 		sprintf_s (message, sizeof (message), "%s: Segment has invalid type (segment=%d))", m_bAutoFixBugs ? "FIXED" : "ERROR", nSegment);
			if (m_bAutoFixBugs)
				segmentManager.Undefine (nSegment);
			}
		}

	if (!pSegment->Shape ()) {
		short nPoint;
		for (nPoint = 0; nPoint < 8; nPoint++) {
	// define vert numbers
			short vert0 = pSegment->m_info.vertexIds [nPoint];
			short vert1 = pSegment->m_info.vertexIds [adjacentPointTable [nPoint][0]];
			short vert2 = pSegment->m_info.vertexIds [adjacentPointTable [nPoint][1]];
			short vert3 = pSegment->m_info.vertexIds [adjacentPointTable [nPoint][2]];
			double angle = CalcAngle (vert0, vert1, vert2, vert3);
			double a = CalcAngle (vert0, vert2, vert3, vert1);
			angle = max (angle, a);
			a = CalcAngle (vert0, vert3, vert1, vert2);
			angle = max (angle, a);
			if (angle > M_PI_2) {
				sprintf_s (message, sizeof (message), "WARNING: Illegal segment geometry (segment=%d, point=%d, angle=%d)", nSegment, nPoint, (int) ( (angle*180.0)/M_PI));
				if (UpdateStats (message, 0, nSegment, -1, -1, nPoint))
					return true;
				break; // from for loop
				}
			}
		if (nPoint == 8) { // angles must be ok from last test
	// now test for flat sides
			for (nSide = 0; nSide < 6; nSide++) {
				double flatness = CalcFlatnessRatio (nSegment, nSide);
				if (flatness < 0.80) {
					sprintf_s (message, sizeof (message), "ERROR: Illegal segment geometry (segment=%d, side=%d, flatness=%d%%)", nSegment, nSide, (int) (flatness*100));
					if (UpdateStats (message, 1, nSegment, nSide))
						return true;
					break; // from for loop
					}
				} 
			}
		}
// Check length of each line.
	for (nSide = 0; nSide < MAX_SIDES_PER_SEGMENT; nSide++) {
		nChild = segmentManager.Segment (nSegment)->ChildId (nSide);
// check nChild range 
		if ((nChild != -1) && (nChild != -2)) {
			if (nChild < -2) {
				sprintf_s (message, sizeof (message), "ERROR: Illegal child number %d (segment=%d, side=%d)", nChild, nSegment, nSide);
				if (UpdateStats (message, 1, nSegment, nSide))
					return true;
				}
			else if (nChild >= segmentManager.Count ()) {
				sprintf_s (message, sizeof (message), "ERROR: Child out of range %d (segment=%d, side=%d)", nChild, nSegment, nSide);
				if (UpdateStats (message, 1, nSegment, nSide)) 
					return true;
				}
			else if (pSegment->Side (nSide)->Shape () > SIDE_SHAPE_TRIANGLE) {
				if (m_bAutoFixBugs) {
					pSegment->ReplaceChild (nChild, -1);
					segmentManager.Segment (nChild)->ReplaceChild (nSegment, -1);
					sprintf_s (message, sizeof (message), "FIXED: Collapsed side has child segment %d (segment=%d, side=%d)", nChild, nSegment, nSide);
					}
				else
					sprintf_s (message, sizeof (message), "ERROR: Collapsed side has child segment %d (segment=%d, side=%d)", nChild, nSegment, nSide);
				if (UpdateStats (message, 1, nSegment, nSide)) 
					return true;
				}
			else {
			// make sure nChild segment has a nChild from this segment
			// and that it shares the same vertices as this segment
				for (nSide2 = 0; nSide2 < MAX_SIDES_PER_SEGMENT; nSide2++) {
					if (segmentManager.Segment (nChild)->ChildId (nSide2) == nSegment) 
						break;
					}					
				if (nSide2 < MAX_SIDES_PER_SEGMENT) {
					short match [4] = { 0, 0, 0, 0 };
					CSegment * pSegment = segmentManager.Segment (nSegment), 
								* pChildSeg = segmentManager.Segment (nChild);
					int n = pSegment->Side (nSide)->VertexCount ();
					for (i = 0; i < n; i++) {
						ushort v = pSegment->m_info.vertexIds [pSegment->Side (nSide)->VertexIdIndex (i)];
						for (j = 0; j < n; j++)
							if (v == pChildSeg->m_info.vertexIds [pChildSeg->Side (nSide2)->VertexIdIndex (j)])
								match[i]++;
						}
					for ( ; i < 4; i++)
						match [i] = 1;
					if ((match [0] != 1) || (match [1] != 1) || (match [2] != 1) || (match [3] != 1)) {
						sprintf_s (message, sizeof (message), "WARNING:Child segment does not share points of segment (segment=%d, side=%d, child=%d)", nSegment, nSide, nChild);
						if (UpdateStats (message, 0, nSegment, -1, -1, -1, nChild))
							return true;
						}
					}
				else {
					sprintf_s (message, sizeof (message), "WARNING:Child segment does not connect to this segment (segment=%d, side=%d, child=%d)", nSegment, nSide, nChild);
					if (UpdateStats (message, 1, nSegment, -1, -1, -1, nChild))
						return false;
					}
				}
			}  
		}
	}
if (sub_errors == m_nErrors [0] && sub_warnings == m_nErrors [1]) {
	LBBugs ()->DeleteString (LBBugs ()->GetCount ()-1);
	LBBugs ()->AddString ("[Segments] (no errors)");
	}
return false;
}

//--------------------------------------------------------------------------

bool CDiagTool::CheckAndFixPlayer (int nMin, int nMax, int nObject, int* players)
{
if (theMine == null) 
	return false;

int id= objectManager.Object (nObject)->m_info.id;
if ((id < nMin) || (id > nMax))
	sprintf_s (message, sizeof (message), "WARNING: Invalid player id (Object %d)", id, nObject);
else if (players [id])
	sprintf_s (message, sizeof (message), "WARNING: Duplicate player #%d (found %d)", id, players [id]);
else {
	players [id]++;
	return false;
	}
if (m_bAutoFixBugs) {
	for (id= nMin; (id < nMax) && players [id]; id++)
		;
	objectManager.Object (nObject)->m_info.id= (id < nMax) ? id : nMax - 1;
	players [objectManager.Object (nObject)->m_info.id]++;
	}
return true;
}

//--------------------------------------------------------------------------
// CDiagTool - check_objects ()
//
// ACTION - Checks object's: segment number, type, id, position, container.
//          Makes sure the correct number of players coop-players, and
//          control centers (robots) are used.
//          Control center belongs to segment with special = SEGMENT_FUNC_REACTOR
//--------------------------------------------------------------------------

bool CDiagTool::CheckObjects (void) 
{
if (theMine == null) 
	return false;

	int				h, nObject, type, id, count, players [16 + MAX_COOP_PLAYERS], nSegment, flags, corner, nPlayers [2], bFix;
	CVertex			center;
	double			radius, maxRadius, objectDistance;
	CGameObject*	pObject = objectManager.Object (0);
	CGameObject*	playerP = null;
	int				objCount = objectManager.Count ();
	CSegment*		pSegment;
	ubyte				nMaxRobotId = (DLE.IsD2XFile () || theMine->m_bVertigo) ? MAX_ROBOT_TYPES : DLE.IsD2File () ? N_ROBOT_TYPES_D2 : N_ROBOT_TYPES_D1;

short sub_errors = m_nErrors [0];
short sub_warnings = m_nErrors [1];
LBBugs ()->AddString ("[Objects]");
for (nObject = 0; nObject < objCount ; nObject++, pObject++) {
	DLE.MainFrame ()->Progress ().StepIt ();
	// check segment range
	nSegment = pObject->m_info.nSegment;
	if (nSegment < 0 || nSegment >= segmentManager.Count ()) {
		if (m_bAutoFixBugs) {
			pObject->m_info.nSegment = nSegment = 0;
			sprintf_s (message, sizeof (message), "FIXED: Bad segment number (object=%d, nSegment=%d)", nObject, nSegment);
			}
		else
			sprintf_s (message, sizeof (message), "ERROR: Bad segment number (object=%d, nSegment=%d)", nObject, nSegment);
		if (UpdateStats (message, 1, -1, -1, -1, -1, -1, -1, -1, nObject))
			return true;
		}

	if (nSegment < 0 || nSegment >= segmentManager.Count ()) {
		if (m_bAutoFixBugs) {
			pObject->m_info.nSegment = 0;
			sprintf_s (message, sizeof (message), "FIXED: Bad segment number (object=%d, nSegment=%d)", nObject, nSegment);
			}
		else
			sprintf_s (message, sizeof (message), "ERROR: Bad segment number (object=%d, nSegment=%d)", nObject, nSegment);
		if (UpdateStats (message, 1, -1, -1, -1, -1, -1, -1, -1, nObject))
			return true;
		}

	pSegment = segmentManager.Segment (nSegment);
    // make sure object is within its segment
    // find center of segment then find maximum distance
	// of corner to center.  Calculate Objects () distance
    // from center and make sure it is less than max corner.
   center = pSegment->ComputeCenter ();
   maxRadius = 0;
	for (corner = 0; corner < 8; corner++) {
		if (pSegment->m_info.vertexIds [corner] <= MAX_VERTEX) {
			radius = Distance (*vertexManager.Vertex (pSegment->m_info.vertexIds [corner]), center);
			maxRadius = max (maxRadius, radius);
			}
		}
	objectDistance = Distance (pObject->Position (), center);
   if ((objectDistance > maxRadius) && (pObject->Type () != OBJ_EFFECT)) {
		int nNewSegment = objectManager.FindSegment (pObject);
		if (m_bAutoFixBugs && (nNewSegment > -1) && (nNewSegment != pObject->m_info.nSegment)) {
			//objectManager.Move (pObject, nNewSegment);
			pObject->Info ().nSegment = nNewSegment;
	      sprintf_s (message, sizeof (message), "FIXED: Object is outside of segment (object=%d, segment=%d, moved to %d)", nObject, nSegment, nNewSegment);
		   if (UpdateStats (message, 1, nSegment, -1, -1, -1, -1, -1, -1, nObject))
				return true;
			}
		else {
	      sprintf_s (message, sizeof (message), "ERROR: Object is outside of segment (object=%d, segment=%d)", nObject, nSegment);
		   if (UpdateStats (message, 1, nSegment, -1, -1, -1, -1, -1, -1, nObject))
				return true;
			}
    }

    // check for non-zero flags (I don't know what these flags are for)
   flags = pObject->m_info.flags;
	if (flags != 0) {
		if (m_bAutoFixBugs) {
			pObject->m_info.flags = 0;
			sprintf_s (message, sizeof (message), "FIXED: Flags for object non-zero (object=%d, flags=%d)", nObject, flags);
			}
		else
			sprintf_s (message, sizeof (message), "ERROR: Flags for object non-zero (object=%d, flags=%d)", nObject, flags);
      if (UpdateStats (message, 1, nSegment, -1, -1, -1, -1, -1, -1, nObject)) 
			return true;
    }

    // check type range
	 if ((pObject->Id () < 0) || (pObject->Id () > 255)) {
		 if (m_bAutoFixBugs) {
			sprintf_s (message, sizeof (message), "FIXED: Illegal object id (object=%d, id=%d)", nObject, pObject->Id ());
			pObject->Id () = 0;
			}
		 else
			sprintf_s (message, sizeof (message), "WARNING: Illegal object id (object=%d, id=%d)", nObject, pObject->Id ());
		 if (UpdateStats (message, 0, nSegment, -1, -1, -1, -1, -1, -1, nObject))
			return true;
		}

	type = pObject->Type ();
    switch (type) {
	  case OBJ_PLAYER:
		  if (!playerP)
			  playerP = pObject;
			if (pObject->Id () >= MAX_PLAYERS) {
				if (m_bAutoFixBugs) {
					sprintf_s (message, sizeof (message), "FIXED: Illegal player id (object=%d, id=%d)", nObject, pObject->Id ());
				pObject->Id () = MAX_PLAYERS - 1;
				}
			else
				sprintf_s (message, sizeof (message), "WARNING: Illegal player id (object=%d, id=%d)", nObject, pObject->Id ());
			if (UpdateStats (message, 0, nSegment, -1, -1, -1, -1, -1, -1, nObject))
				return true;
			}

	  case OBJ_COOP:
		  if (pObject->Id () >= MAX_PLAYERS + MAX_COOP_PLAYERS) {
			if (m_bAutoFixBugs) {
				sprintf_s (message, sizeof (message), "FIXED: Illegal coop player id (object=%d, id=%d)", nObject, pObject->Id ());
				pObject->Id () = MAX_PLAYERS + pObject->Id () % MAX_COOP_PLAYERS;
				}
			else
				sprintf_s (message, sizeof (message), "WARNING: Illegal coop player id (object=%d, id=%d)", nObject, pObject->Id ());
			if (UpdateStats (message, 0, nSegment, -1, -1, -1, -1, -1, -1, nObject))
				return true;
			}
			break;

	  case OBJ_EFFECT:
			if (pObject->Id () > MAX_EFFECT_ID) {
				if (m_bAutoFixBugs) {
					sprintf_s (message, sizeof (message), "FIXED: effect id (object=%d, id=%d)", nObject, pObject->Id ());
					pObject->Id () = MAX_EFFECT_ID;
					}
				else
					sprintf_s (message, sizeof (message), "WARNING: Illegal effect id (object=%d, id=%d)", nObject, pObject->Id ());
				if (UpdateStats (message, 0, nSegment, -1, -1, -1, -1, -1, -1, nObject))
					return true;
				}
			else if (!pObject->CheckEffectType (m_bAutoFixBugs == TRUE)) {
				sprintf_s (message, sizeof (message), "%s: Illegal effect id (object=%d, id=%d)", m_bAutoFixBugs ? "FIXED" : "WARNING", nObject, pObject->Id ());
				if (UpdateStats (message, 0, nSegment, -1, -1, -1, -1, -1, -1, nObject))
					return true;
				}
			break;

	  case OBJ_ROBOT:
		  if (pObject->Id () > nMaxRobotId) {
			if (m_bAutoFixBugs) {
				sprintf_s (message, sizeof (message), "FIXED: robot id (object=%d, id=%d)", nObject, pObject->Id ());
				pObject->Id () = nMaxRobotId;
				}
			else
				sprintf_s (message, sizeof (message), "WARNING: Illegal robot id (object=%d, id=%d)", nObject, pObject->Id ());
			if (UpdateStats (message, 0, nSegment, -1, -1, -1, -1, -1, -1, nObject))
				return true;
			}
			break;

	  case OBJ_HOSTAGE:
	  case OBJ_POWERUP:
     case OBJ_REACTOR:
     case OBJ_WEAPON:
		  break;

	  case OBJ_CAMBOT:
	  case OBJ_SMOKE:
	  case OBJ_MONSTERBALL:
	  case OBJ_EXPLOSION:
			if (DLE.IsD2File ()) 
				break;

	  default:
		 if (m_bAutoFixBugs) {
			 objectManager.Delete (nObject);
			sprintf_s (message, sizeof (message), "FIXED: Illegal object type (object=%d, type=%d)", nObject, type);
			}
		 else
			sprintf_s (message, sizeof (message), "WARNING: Illegal object type (object=%d, type=%d)", nObject, type);
		if (UpdateStats (message, 0, nSegment, -1, -1, -1, -1, -1, -1, nObject))
			return true;
		}

    id = pObject->Id ();

    // check id range
    if (h = CheckId (pObject->Type (), pObject->Id ())) {
		 if (h == 2)
	      sprintf_s (message, sizeof (message), "FIXED: Illegal object id (object=%d, id=%d)", nObject, id);
		else
	      sprintf_s (message, sizeof (message), "WARNING: Illegal object id (object=%d, id=%d)", nObject, id);
      if (UpdateStats (message, 0, nSegment, -1, -1, -1, -1, -1, -1, nObject)) 
			return true;
    }

	// check contains count range
	 if (pObject->Type () == OBJ_ROBOT) {
		count = pObject->m_info.contents.count;
		if (count < -1) {
			if (m_bAutoFixBugs) {
				pObject->m_info.contents.count = 0;
			  sprintf_s (message, sizeof (message), "FIXED: Spawn count must be >= -1 (object=%d, count=%d)", nObject, count);
				}
			else
			  sprintf_s (message, sizeof (message), "WARNING: Spawn count must be >= -1 (object=%d, count=%d)", nObject, count);
			if (UpdateStats (message, 0, nSegment, -1, -1, -1, -1, -1, -1, nObject)) 
				return true;
		 }

		// check container type range
		if (count > 0) {
			type = pObject->m_info.contents.type;
			if ((type != OBJ_ROBOT) && (type != OBJ_POWERUP)) {
				if (m_bAutoFixBugs) {
					pObject->m_info.contents.type = 0;
					pObject->m_info.contents.count = 0;
					sprintf_s (message, sizeof (message), "FIXED: Illegal contained type (object=%d, contains=%d)", nObject, type);
					}
				else
					sprintf_s (message, sizeof (message), "WARNING: Illegal contained type (object=%d, contains=%d)", nObject, type);
				if (UpdateStats (message, 0, nSegment, -1, -1, -1, -1, -1, -1, nObject)) return true;
				count = 0;
				}
			}
		if (count > 0) {
			id = pObject->m_info.contents.id;
			// check contains id range
			if (h = CheckId (pObject->m_info.contents.type, pObject->m_info.contents.id)) {
				if (h == 2)
					sprintf_s (message, sizeof (message), "FIXED: Illegal contains id (object=%d, contains id=%d)", nObject, id);
				else
					sprintf_s (message, sizeof (message), "WARNING: Illegal contains id (object=%d, contains id=%d)", nObject, id);
				if (UpdateStats (message, 0, 1)) return true;
				}
			}
		}
	 }

  // make sure object 0 is player 0; if not int it
if (objectManager.Object (0)->m_info.type != OBJ_PLAYER || objectManager.Object (0)->m_info.id != 0) {
	if (m_bAutoFixBugs && playerP) {
		CGameObject h;
		memcpy (&h, playerP, sizeof (CGameObject));
		memcpy (playerP, objectManager.Object (0), sizeof (CGameObject));
		memcpy (objectManager.Object (0), playerP, sizeof (CGameObject));
		strcpy_s (message, sizeof (message), "FIXED: Object 0 was not Player 0.");
		if (UpdateStats (message, 0, nSegment, -1, -1, -1, -1, -1, -1, nObject))
			return true;
		}
	else {
		strcpy_s (message, sizeof (message), "WARNING: Object 0 is not Player 0.");
		if (UpdateStats (message, 0, nSegment, -1, -1, -1, -1, -1, -1, nObject))
			return true;
		}
	}

	// make sure there is the proper number of players and coops
	// reset count to zero
	memset (players, 0, sizeof (players));
	memset (nPlayers, 0, sizeof (nPlayers));
	bFix = 0;
	// count each
	pObject = objectManager.Object (0);
	for (nObject = 0; nObject < objCount; nObject++, pObject++) {
		if (pObject->Type () == OBJ_PLAYER) {
			nPlayers [0]++;
			if (CheckAndFixPlayer (0, MAX_PLAYERS, nObject, players))
				bFix |= 1;
			}
		else if (pObject->Type () == OBJ_COOP) {
			nPlayers [1]++;
			if (CheckAndFixPlayer (MAX_PLAYERS, MAX_PLAYERS + MAX_COOP_PLAYERS, nObject, players))
				bFix |= 2;
			}
		}
// resequence player and coop ids
if (m_bAutoFixBugs) {
	int i;
	if (bFix & 1) {
		for (i = id= 0; id < MAX_PLAYERS; id++)
			if (players [id] != 0) 
				players [id] = ++i;
		}
	if (bFix & 2) {
		for (id= MAX_PLAYERS; id < MAX_PLAYERS + MAX_COOP_PLAYERS; id++)
			if (players [id] != 0) 
				players [id] = id;
		}
	pObject = objectManager.Object (0);
	for (nObject = 0; nObject < objCount; nObject++, pObject++) {
		if (pObject->Type () == OBJ_PLAYER) {
			if ((bFix & 1) && (pObject->Id () >= 0) && (pObject->Id () < MAX_PLAYERS))
				pObject->Id () = players [pObject->Id ()] - 1;
			}
		else if (pObject->Type () == OBJ_COOP) {
			if ((bFix & 2) && (pObject->Id () >= MAX_PLAYERS) && (pObject->Id () < MAX_PLAYERS + MAX_COOP_PLAYERS))
				pObject->Id () = players [pObject->Id ()] - 1;
			}
		}
	}

for (id= 0; id < MAX_PLAYERS; id++) {
	if (players [id] == 0) {
		sprintf_s (message, sizeof (message), "INFO: No player %d", id + 1);
		if (UpdateStats (message, 0)) 
			return true;
		}
	}
if (nPlayers [0] > MAX_PLAYERS) {
	sprintf_s (message, sizeof (message), "WARNING: too many players found (found %d, max. allowed %d)", nPlayers [0], MAX_PLAYERS);
	if (UpdateStats (message, 0)) 
		return true;
	}
if (nPlayers [1] < 3) {
	sprintf_s (message, sizeof (message), "INFO: %d coop players found (3 allowed)", nPlayers [1]);
	if (UpdateStats (message, 0)) 
		return true;
	}
else if (nPlayers [1] > 3) {
	sprintf_s (message, sizeof (message), "WARNING: too many coop players found (found %d, max. allowed 3)", nPlayers [1]);
	if (UpdateStats (message, 0)) 
		return true;
	}

  // make sure there is only one control center
count = 0;
pObject = objectManager.Object (0);
for (nObject = 0; nObject < objCount; nObject++, pObject++) {
	DLE.MainFrame ()->Progress ().StepIt ();
	type = pObject->Type ();
	if (type == OBJ_REACTOR) {
		if (segmentManager.Segment (pObject->m_info.nSegment)->m_info.function != SEGMENT_FUNC_REACTOR) {
			if (m_bAutoFixBugs && segmentManager.CreateReactor (pObject->m_info.nSegment, false, false))
				sprintf_s (message, sizeof (message), "FIXED: Reactor belongs to a segment of wrong type (pObject=%d, pSegment=%d)", nObject, pObject->m_info.nSegment);
			else
				sprintf_s (message, sizeof (message), "WARNING: Reactor belongs to a segment of wrong type (pObject=%d, pSegment=%d)", nObject, pObject->m_info.nSegment);
			if (UpdateStats (message, 0, nSegment, -1, -1, -1, -1, -1, -1, nObject))
				return true;
			}
		count++;
		if (count > 1) {
			sprintf_s (message, sizeof (message), "WARNING: More than one Reactor found (object=%d)", nObject);
			if (UpdateStats (message, 0, nSegment, -1, -1, -1, -1, -1, -1, nObject))
				return true;
			}
		}
	}
if (count < 1) {
	sprintf_s (message, sizeof (message), "WARNING: No Reactors found (note: ok if boss robot used)");
	if (UpdateStats (message, 0)) 
		return true;
	}

if (sub_errors == m_nErrors [0] && sub_warnings == m_nErrors [1]) {
	LBBugs ()->DeleteString (LBBugs ()->GetCount ()-1);
	LBBugs ()->AddString ("[Objects] (no errors)");
	}
return false;
}

//--------------------------------------------------------------------------
// CDiagTool - check_triggers ()
//
//  x unlinked or overlinked Triggers ()
//
//  o key exists for all locked doors
//  o exit (normal and special)
//  o center exists
//  o non-keyed doors have triggers
//  o Triggers () point to something
//--------------------------------------------------------------------------

bool CDiagTool::CheckTriggers (void)
 {
if (theMine == null) 
	return false;

	int count, nTrigger, nDelTrigger, nWall, i;
	int nSegment, nSide, nTarget;

	short sub_errors = m_nErrors [0];
	short sub_warnings = m_nErrors [1];
	LBBugs ()->AddString ("[Triggers]");
	int segCount = segmentManager.Count ();
	int trigCount = triggerManager.WallTriggerCount ();
	CTrigger *pTrigger = triggerManager.Trigger (0);
	int wallCount = wallManager.WallCount ();
	CWall *pWall;
	CReactorTrigger *reactorTrigger = triggerManager.ReactorTrigger (0);

	// make sure pTrigger is linked to exactly one pWall
for (i = 0; i < reactorTrigger->Count (); i++)
	if ((reactorTrigger->Segment (i) >= segCount) ||
		(segmentManager.Segment (reactorTrigger->Segment (i))->m_sides [reactorTrigger->Side (i)].m_info.nWall >= wallCount)) {
		if (m_bAutoFixBugs) {
			reactorTrigger->Delete (i);
			strcpy_s (message, sizeof (message), "FIXED: Reactor has invalid trigger target.");
			if (UpdateStats (message, 0))
				return true;
			}
		else {
			strcpy_s (message, sizeof (message), "WARNING: Reactor has invalid trigger target.");
			if (UpdateStats (message, 0))
				return true;
			}
		}
for (nTrigger = nDelTrigger = 0; nTrigger < trigCount; nTrigger++, pTrigger++) {
	DLE.MainFrame ()->Progress ().StepIt ();
	count = 0;
	CWall* pWall = null;
	for (nWall = 0; pWall = wallManager.FindByTrigger (nTrigger, nWall); nWall++) {
		nWall = wallManager.Index (pWall);
		if (++count > 1) {
			sprintf_s (message, sizeof (message), "WARNING: Trigger belongs to more than one wall (trigger=%d, wall=%d)", nTrigger, nWall);
			if (UpdateStats (message, 0, pWall->m_nSegment, pWall->m_nSide, -1, -1, -1, nWall)) return true;
			}
		// if exit, make sure it is linked to CReactorTrigger
		bool bExit = pTrigger->IsExit (false);
		bool bFound = (reactorTrigger->Find (*pWall) >= 0);
		if (bExit != bFound) {
			if (m_bAutoFixBugs) {
				if (bExit) {
					triggerManager.ReactorTrigger ()->Add (*pWall);
					sprintf_s (message, sizeof (message), "FIXED: Exit not linked to reactor (segment=%d, side=%d)", pWall->m_nSegment, pWall->m_nSide);
					}
				else {
					triggerManager.ReactorTrigger ()->Delete (*pWall);
					sprintf_s (message, sizeof (message), "FIXED: Reactor linked to non-exit (segment=%d, side=%d)", pWall->m_nSegment, pWall->m_nSide);
					}
				}
			else {
				if (bExit)
					sprintf_s (message, sizeof (message), "WARNING: Exit not linked to reactor (segment=%d, side=%d)", pWall->m_nSegment, pWall->m_nSide);
				else
					sprintf_s (message, sizeof (message), "WARNING: Reactor linked to non-exit (segment=%d, side=%d)", pWall->m_nSegment, pWall->m_nSide);
				}
			if (UpdateStats (message, 1, pWall->m_nSegment, pWall->m_nSide, -1, -1, -1, nWall))
				return true;
			}
		}

	if (count < 1) {
		if (m_bAutoFixBugs) {
			triggerManager.Delete (nTrigger);
			nTrigger--;
			pTrigger--;
			trigCount--;
			sprintf_s (message, sizeof (message), "FIXED: Unused trigger (trigger=%d)", nTrigger + nDelTrigger);
			nDelTrigger++;
			}
		else
			sprintf_s (message, sizeof (message), "WARNING: Unused trigger (trigger=%d)", nTrigger);
		if (UpdateStats (message, 0, 1)) return true;
		}
	}

short trigSeg, trigSide;
pTrigger = triggerManager.Trigger (0);
for (nTrigger = 0; nTrigger < trigCount; nTrigger++, pTrigger++) {
	DLE.MainFrame ()->Progress ().StepIt ();
	pWall = wallManager.FindByTrigger (nTrigger);
	if (pWall != null) {
		trigSeg = pWall->m_nSegment;
		trigSide = pWall->m_nSide;
		}
	else
		trigSeg = trigSide = -1;
	// check number of links of pTrigger (only for
	int tt = pTrigger->Type ();
	int tf = pTrigger->Info ().flags;
	if (pTrigger->Count () == 0) {
		if (DLE.IsD1File ()
			 ? tf & (TRIGGER_CONTROL_DOORS | TRIGGER_ON | TRIGGER_ONE_SHOT | TRIGGER_MATCEN | TRIGGER_ILLUSION_OFF | TRIGGER_ILLUSION_ON) 
			 : (tt != TT_EXIT) && (tt != TT_SECRET_EXIT) && (tt != TT_MESSAGE) && (tt != TT_SOUND) && 
			   (tt != TT_SPEEDBOOST) && (tt != TT_SHIELD_DAMAGE_D2) && (tt != TT_ENERGY_DRAIN_D2)
			) {
			sprintf_s (message, sizeof (message), "WARNING: Trigger has no targets (trigger=%d)", nTrigger);
			if (UpdateStats (message, 0, -1, -1, -1, -1, -1, -1, nTrigger))
				return true;
			}
		}
	else {
		// check range of links
		for (nTarget = 0; nTarget < pTrigger->Count (); nTarget++) {
			if (nTarget >= MAX_TRIGGER_TARGETS) {
				if (m_bAutoFixBugs) {
					pTrigger->Count () = MAX_TRIGGER_TARGETS;
					sprintf_s (message, sizeof (message), "FIXED: Trigger has too many targets (trigger=%d, number of links=%d)", nTrigger, nTarget);
					}
				else
					sprintf_s (message, sizeof (message), "WARNING: Trigger has too many targets (trigger=%d, number of links=%d)", nTrigger, nTarget);
				if (UpdateStats (message, 0, trigSeg, trigSide, -1, -1, -1, -1, nTrigger)) 
					return true;
				break;
				}
			// check segment range
			nSegment = pTrigger->Segment (nTarget);
			nSide = pTrigger->Side (nTarget);
			if ((nSegment < 0) || ((nSide < 0) ? (nSegment >= objectManager.Count ()) : (nSegment >= segmentManager.Count ()))) {
				if (m_bAutoFixBugs) {
					if (pTrigger->Delete (nTarget))
						nTarget--;
					else { // => pTrigger deleted
						nTarget = MAX_TRIGGER_TARGETS;	// take care of the loops
						pTrigger--;
						}
					sprintf_s (message, sizeof (message), "FIXED: Trigger points to non-existant %s (trigger=%d, segment=%d)", 
								  (nSide < 0) ? "object" : "segment", nTrigger, nSegment);
					}
				else
					sprintf_s (message, sizeof (message), "ERROR: Trigger points to non-existant %s (triggger=%d, segment=%d)", 
								  (nSide < 0) ? "object" : "segment", nTrigger, nSegment);
				if (UpdateStats (message, 1, trigSeg, trigSide, -1, -1, -1, -1, nTrigger)) 
					return true;
				}
			else {
				// check side range
				if (nSide < -1 || nSide > 5) {
					if (m_bAutoFixBugs) {
						if (nSide < 0)
							nSide = 0;
						else if (nSide > 5)
							nSide = 5;
						sprintf_s (message, sizeof (message), "FIXED: Trigger points to non-existant side (trigger=%d, side=%d)", nTrigger, nSide);
						}
					else
						sprintf_s (message, sizeof (message), "ERROR: Trigger points to non-existant side (trigger=%d, side=%d)", nTrigger, nSide);
					if (UpdateStats (message, 1, trigSeg, trigSide, -1, -1, -1, -1, nTrigger)) 
						return true;
				} else {
					CSegment *pSegment = segmentManager.Segment (nSegment);
					// check door opening pTrigger
//						if (pTrigger->Info ().flags == TRIGGER_CONTROL_DOORS) {
					if (DLE.IsD1File ()
						 ? tf & TRIGGER_CONTROL_DOORS 
						 : tt==TT_OPEN_DOOR || tt==TT_CLOSE_DOOR || tt==TT_LOCK_DOOR || tt==TT_UNLOCK_DOOR) {
						// make sure pTrigger points to a pWall if it controls doors
						if (pSegment->m_sides[nSide].m_info.nWall >= wallCount) {
							if (m_bAutoFixBugs) {
								if (pTrigger->Delete (nTarget))
									nTarget--;
								else {
									nTarget = MAX_TRIGGER_TARGETS;
									pTrigger--;
									}
								sprintf_s (message, sizeof (message), "FIXED: Trigger does not target a door (trigger=%d, link= (%d, %d))", nTrigger, nSegment, nSide);
								}
							else
								sprintf_s (message, sizeof (message), "WARNING: Trigger does not target a door (trigger=%d, link= (%d, %d))", nTrigger, nSegment, nSide);
							if (UpdateStats (message, 0, trigSeg, trigSide, -1, -1, -1, -1, nTrigger)) return true;
						}

						// make sure oposite segment/side has a pWall too
						CSideKey opp;
						CSide* pOppSide = segmentManager.BackSide (CSideKey (nSegment, nSide), opp);
						if (pOppSide == null) {
							sprintf_s (message, sizeof (message), "WARNING: Trigger opens a single sided door (trigger=%d, link= (%d, %d))", nTrigger, nSegment, nSide);
							if (UpdateStats (message, 0, trigSeg, trigSide, -1, -1, -1, -1, nTrigger)) return true;
							}
						else {
							if (pOppSide->Wall () == null) {
								sprintf_s (message, sizeof (message), "WARNING: Trigger opens a single sided door (trigger=%d, link= (%d, %d))", nTrigger, nSegment, nSide);
								if (UpdateStats (message, 1, trigSeg, trigSide, -1, -1, -1, -1, nTrigger)) return true;
								}
							}
						}
					else if (DLE.IsD1File () 
								? tf & (TRIGGER_ILLUSION_OFF | TRIGGER_ILLUSION_ON) 
								: tt == TT_ILLUSION_OFF || tt == TT_ILLUSION_ON || tt == TT_OPEN_WALL || tt == TT_CLOSE_WALL || tt == TT_ILLUSORY_WALL
							  ) {
						// make sure pTrigger points to a pWall if it controls doors
						if (pSegment->m_sides [nSide].m_info.nWall >= wallCount) {
							if (m_bAutoFixBugs) {
								if (pTrigger->Delete (nTarget))
									nTarget--;
								else {
									nTarget = MAX_TRIGGER_TARGETS;
									pTrigger--;
									}
								sprintf_s (message, sizeof (message), "FIXED: Trigger target does not exist (trigger=%d, link= (%d, %d))", nTrigger, nSegment, nSide);
								}
							else
								sprintf_s (message, sizeof (message), "ERROR: Trigger target does not exist (trigger=%d, link= (%d, %d))", nTrigger, nSegment, nSide);
							if (UpdateStats (message, 0, trigSeg, trigSide, -1, -1, -1, -1, nTrigger)) return true;
							}
						}
//						if (pTrigger->Info ().flags == TRIGGER_MATCEN) {
					else if (DLE.IsD1File () ? tf & TRIGGER_MATCEN : tt == TT_MATCEN) {
						if ((pSegment->m_info.function != SEGMENT_FUNC_ROBOTMAKER) && (pSegment->m_info.function != SEGMENT_FUNC_EQUIPMAKER)) {
							sprintf_s (message, sizeof (message), "WARNING: Trigger does not target a robot or equipment maker (trigger=%d, link= (%d, %d))", nTrigger, nSegment, nSide);
							if (UpdateStats (message, 0, trigSeg, trigSide, -1, -1, -1, -1, nTrigger)) return true;
							}
						}
					}
				}
			}
		}
	}

// make sure there is exactly one exit and its linked to the CReactorTrigger
count = 0;
pTrigger = triggerManager.Trigger (0);
for (nTrigger = 0; nTrigger < trigCount; nTrigger++, pTrigger++) {
	DLE.MainFrame ()->Progress ().StepIt ();
	pWall = wallManager.FindByTrigger (nTrigger);
	if (pWall != null) {
		trigSeg = pWall->m_nSegment;
		trigSide = pWall->m_nSide;
		}
	else
		trigSeg = trigSide = -1;
	int tt = pTrigger->Type ();
	int tf = pTrigger->Info ().flags;
	if (DLE.IsD1File () ? tf & TRIGGER_EXIT : tt == TT_EXIT) {
		count++;
		if (count >1) {
			sprintf_s (message, sizeof (message), "WARNING: More than one exit found (trigger=%d)", nTrigger);
			if (UpdateStats (message, 0, trigSeg, trigSide, -1, -1, -1, -1, nTrigger)) return true;
			}
		}
	}

trigCount = triggerManager.ObjTriggerCount ();
for (nTrigger = 0; nTrigger < trigCount; nTrigger++) {
	DLE.MainFrame ()->Progress ().StepIt ();
	pTrigger = triggerManager.ObjTrigger (nTrigger);
	if ((pTrigger->Type () != TT_MESSAGE) && (pTrigger->Type () != TT_SOUND) && (pTrigger->Type () != TT_COUNTDOWN) && !pTrigger->Count ()) {
		sprintf_s (message, sizeof (message), "ERROR: Object trigger has no targets (trigger=%d, object=%d))", nTrigger, pTrigger->Info ().nObject);
		if (UpdateStats (message, 0, nTrigger, pTrigger->Info ().nObject, -1, -1, -1, -1, nTrigger)) return true;
		}
	}
if (count < 1) {
	sprintf_s (message, sizeof (message), "WARNING: No exit found");
	if (UpdateStats (message, 0)) return true;
	}

if (sub_errors == m_nErrors [0] && sub_warnings == m_nErrors [1]) {
	LBBugs ()->DeleteString (LBBugs ()->GetCount () - 1);
	LBBugs ()->AddString ("[Triggers] (no errors)");
	}
return false;
}

//------------------------------------------------------------------------

char CDiagTool::FindProducer (CObjectProducer* producerP, short nSegment, short* refList)
{
	char	h = -1, i, j = char (segmentManager.RobotMakerCount ());

if (refList) {
	for (i = 0; i < j; i++) {
		if (refList [i] >= 0) {
			if (producerP [i].m_info.objFlags [0] || producerP [i].m_info.objFlags [1])
				return i;
			h = i;
			}
		}
	}
else {
	for (i = 0; i < j; i++) {
		if (producerP [i].m_info.nSegment == nSegment)
			return i;
		}
	}
return h;
}

//------------------------------------------------------------------------

void CDiagTool::CountProducerRefs (int nSpecialType, short* refList, CObjectProducer* producerP, short nProducers)
{
	CSegment*		pSegment = segmentManager.Segment (0);
	short				n, h, i, j = segmentManager.Count ();

memset (refList, 0, sizeof (*refList) * MAX_NUM_MATCENS_D2);
for (h = i = 0; i < j; i++, pSegment++) {
	if (pSegment->m_info.function == ubyte (nSpecialType)) {
		n = pSegment->m_info.nProducer;
		if ((n >= 0) && (n < nProducers) && (refList [n] >= 0)) {
			if (producerP [n].m_info.nSegment == i)
				refList [n] = -1;
			else
				refList [n]++;
			}
		}
	}
}

//------------------------------------------------------------------------

short CDiagTool::FixProducers (int nSpecialType, short* segList, short* refList, CObjectProducer* producerP, short nProducers, char* pszType)
{
	CSegment*	pSegment = segmentManager.Segment (0);
	short			n, h, i, j = segmentManager.Count ();

for (h = i = 0; i < j; i++, pSegment++) {
	if (pSegment->m_info.function != ubyte (nSpecialType))
		continue;
	n = pSegment->m_info.nProducer;
	if ((n < 0) || (n >= nProducers)) {
		if (m_bAutoFixBugs)
			n = pSegment->m_info.nProducer = char (FindProducer (producerP, i));
		sprintf_s (message, sizeof (message), "%s: %s maker list corrupted (segment=%d)", m_bAutoFixBugs ? "FIXED" : "ERROR", pszType, i);
		if (UpdateStats (message, i)) return true;
		}
	else if (producerP [n].m_info.nSegment != i) {
		sprintf_s (message, sizeof (message), "%s: %s maker list corrupted (segment=%d)", m_bAutoFixBugs ? "FIXED" : "ERROR", pszType, i);
		if (UpdateStats (message, i)) return true;
		if (m_bAutoFixBugs) {
			n = char (FindProducer (producerP, i));
			if (n >= 0) {
				pSegment->m_info.nProducer = n;
				refList [n] = -1;
				}
			else {
				n = pSegment->m_info.nProducer;
				if (refList [n] >= 0) {
					producerP [n].m_info.nSegment = i;
					refList [n] = -1;
					}
				else
					pSegment->m_info.nProducer = -1;
				}
			}
		}
	if (h < MAX_NUM_MATCENS_D2)
		segList [h++] = i;
	else {
		sprintf_s (message, sizeof (message), "%s: Too many %s makers", m_bAutoFixBugs ? "FIXED" : "ERROR", pszType, i);
		if (m_bAutoFixBugs) 
			segmentManager.Undefine (i);
		}
	}
return h;
}

//------------------------------------------------------------------------

short CDiagTool::AssignProducers (int nSpecialType, short* segList, short* refList, CObjectProducer* producerP, short nProducers)
{
if (!m_bAutoFixBugs)
	return nProducers;

	CSegment*	pSegment = segmentManager.Segment (0);
	short			h, i, j = segmentManager.Count ();

for (h = i = 0; i < j; i++, pSegment++) {
	if (pSegment->m_info.function != ubyte (nSpecialType))
		continue;
	short n = pSegment->m_info.nProducer;
	if (n >= 0)
		continue;
	n = FindProducer (producerP, i, refList);
	if (n >= 0) {
		pSegment->m_info.nProducer = n;
		producerP [n].m_info.nSegment = i;
		refList [n] = -1;
		}
	else if (m_bAutoFixBugs)
		segmentManager.Undefine (i);
	}
return h;
}

//------------------------------------------------------------------------

short CDiagTool::CleanupProducers (short* refList, CObjectProducer* producerP, short nProducers)
{
if (!m_bAutoFixBugs)
	return nProducers;

	CSegment*	pSegment = segmentManager.Segment (0);
	
for (int i = 0; i < nProducers; i) {
	if (refList [i] < 0) 
		i++;
	else {
		if (i < --nProducers) {
			producerP [i] = producerP [nProducers];
			producerP [i].m_info.nProducer =
			pSegment [producerP [i].m_info.nSegment].m_info.nProducer = i;
			refList [i] = refList [nProducers];
			}
		}
	}
return nProducers;
}

//------------------------------------------------------------------------

bool CDiagTool::CheckBotGens (void)
{
if (theMine == null) 
	return false;

	short					h = segmentManager.Count (), i, nSegment = 0;
	bool					bOk = true;
	short					nProducerSegs, nProducers = short (segmentManager.RobotMakerCount ());
	CSegment*			pSegment = segmentManager.Segment (0);
	CObjectProducer*	producerP = segmentManager.RobotMaker (0);
	short					segList [MAX_NUM_MATCENS_D2];
	short					refList [MAX_NUM_MATCENS_D2];

  short sub_errors = m_nErrors [0];
  short sub_warnings = m_nErrors [1];
  LBBugs ()->AddString ("[Robot Generators]");

for (i = 0; i < nProducers; i++)
	producerP [i].m_info.nProducer = i;
CountProducerRefs (SEGMENT_FUNC_ROBOTMAKER, refList, producerP, nProducers);
nProducerSegs = FixProducers (SEGMENT_FUNC_ROBOTMAKER, segList, refList, producerP, nProducers, "Robot");
AssignProducers (SEGMENT_FUNC_ROBOTMAKER, segList, refList, producerP, nProducers);
segmentManager.RobotMakerCount () = CleanupProducers (refList, producerP, nProducers);
if (!bOk) {
	sprintf_s (message, sizeof (message), "%s: Robot maker list corrupted (segment=%d))", m_bAutoFixBugs ? "FIXED" : "ERROR", nSegment);
	if (UpdateStats (message, m_bAutoFixBugs ? 1 : 0, nSegment)) return true;
	}
if (sub_errors == m_nErrors [0] && sub_warnings == m_nErrors [1]) {
	LBBugs ()->DeleteString (LBBugs ()->GetCount ()-1);
	LBBugs ()->AddString ("[Robot Generators] (no errors)");
	}
return false;
}

//------------------------------------------------------------------------

bool CDiagTool::CheckEquipGens (void)
{
	short					i, nSegment = 0;
	bool					bOk = true;
	int					nProducerSegs, nProducers = int (segmentManager.EquipMakerCount ());
	CObjectProducer*	producerP = segmentManager.EquipMaker (0);
	short					segList [MAX_NUM_MATCENS_D2];
	short					refList [MAX_NUM_MATCENS_D2];

  short sub_errors = m_nErrors [0];
  short sub_warnings = m_nErrors [1];
  LBBugs ()->AddString ("[Equipment Generators]");

for (i = 0; i < nProducers; i++)
	producerP [i].m_info.nProducer = i;
CountProducerRefs (SEGMENT_FUNC_EQUIPMAKER, refList, producerP, nProducers);
nProducerSegs = FixProducers (SEGMENT_FUNC_EQUIPMAKER, segList, refList, producerP, nProducers, "Equipment");
AssignProducers (SEGMENT_FUNC_EQUIPMAKER, segList, refList, producerP, nProducers);
segmentManager.EquipMakerCount () = CleanupProducers (refList, producerP, nProducers);
if (!bOk) {
	sprintf_s (message, sizeof (message), "%s: Equipment maker list corrupted (segment=%d))", m_bAutoFixBugs ? "FIXED" : "ERROR", nSegment);
	if (UpdateStats (message, m_bAutoFixBugs ? 1 : 0, nSegment)) return true;
	}
if (sub_errors == m_nErrors [0] && sub_warnings == m_nErrors [1]) {
	LBBugs ()->DeleteString (LBBugs ()->GetCount ()-1);
	LBBugs ()->AddString ("[Equipment Generators] (no errors)");
	}
return false;
}

//------------------------------------------------------------------------------

bool CDiagTool::CheckLights (void)
{
if (DLE.LevelVersion () < 7)
	return true;

  short sub_errors = m_nErrors [0];
  short sub_warnings = m_nErrors [1];
  LBBugs ()->AddString ("[Lights]");

for (int i = 0, j = lightManager.Count (); i < j; i++) {
	CVariableLight* pLight = lightManager.VariableLight (i);
	CSide* pSide = segmentManager.Segment (pLight->m_nSegment)->Side (pLight->m_nSide);
	if (!(lightManager.IsLight (pSide->BaseTex ()) || lightManager.IsLight (pSide->OvlTex (0)))) {
		sprintf_s (message, sizeof (message), "%s: Variable light doesn't refer to a light source (segment=%d, side=%d)", 
					  m_bAutoFixBugs ? "FIXED" : "ERROR", pLight->m_nSegment, pLight->m_nSide);
		if (UpdateStats (message, m_bAutoFixBugs ? 1 : 0, pLight->m_nSegment, pLight->m_nSide)) return true;
		if (m_bAutoFixBugs) {
			lightManager.DeleteVariableLight (*pLight);
			--i;
			}
		}
	else if (!pLight->m_info.delay) {
		if (m_bAutoFixBugs) 
			pLight->m_info.delay = 100;
		sprintf_s (message, sizeof (message), "%s: Variable light has no delay (segment=%d, side=%d)", 
					  m_bAutoFixBugs ? "FIXED" : "ERROR", pLight->m_nSegment, pLight->m_nSide);
		if (UpdateStats (message, m_bAutoFixBugs ? 1 : 0, pLight->m_nSegment, pLight->m_nSide)) return true;
		}
	}
if (sub_errors == m_nErrors [0] && sub_warnings == m_nErrors [1]) {
	LBBugs ()->DeleteString (LBBugs ()->GetCount ()-1);
	LBBugs ()->AddString ("[Lights] (no errors)");
	}
return false;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

CWall *CDiagTool::OppWall (ushort nSegment, ushort nSide)
{
	CSideKey	opp;
	CSide*	pSide = segmentManager.BackSide (CSideKey (nSegment, nSide), opp);

return (pSide == null) ? null : pSide->Wall ();
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

bool CDiagTool::CheckWalls (void) 
{
if (theMine == null) 
	return false;

	short nSegment, nSide;
	ushort nWall, wallCount = wallManager.WallCount (), 
			 maxWalls = MAX_WALLS;
	CSegment *pSegment;
	CSide *pSide;
	CWall *w, *ow;
	int segCount = segmentManager.Count ();
	ubyte wallFixed [MAX_WALLS_D2];

	short sub_errors = m_nErrors [0];
	short sub_warnings = m_nErrors [1];
	LBBugs ()->AddString ("[Walls]");

memset (wallFixed, 0, sizeof (wallFixed));
*message = '\0';
for (nSegment = 0, pSegment = segmentManager.Segment (0); nSegment < segCount; nSegment++, pSegment++) {
	for (nSide = 0, pSide = pSegment->m_sides; nSide < 6; nSide++, pSide++) {
		nWall = pSide->m_info.nWall;
		if ((nWall < 0) || (nWall >= wallCount) || (nWall >= maxWalls)) {
			if (nWall != NO_WALL)
				pSide->m_info.nWall = NO_WALL;
			continue;
			}
		w = wallManager.Wall (nWall);
		if (w->m_nSegment != nSegment) {
			if (m_bAutoFixBugs) {
				sprintf_s (message, sizeof (message), 
							"FIXED: Wall sits in wrong segment (segment=%d, wall=%d, parent=%d)", 
							nSegment, nWall, w->m_nSegment);
				if (wallFixed [nWall])
					pSide->m_info.nWall = NO_WALL;
				else {
					if (segmentManager.Segment (w->m_nSegment)->m_sides [w->m_nSide].m_info.nWall == nWall)
						pSide->m_info.nWall = NO_WALL;
					else {
						w->m_nSegment = nSegment;
						w->m_nSide = nSide;
						}
					wallFixed [nWall] = 1;
					}
				}
			else
				sprintf_s (message, sizeof (message), 
							"ERROR: Wall sits in wrong segment (segment=%d, wall=%d, parent=%d)", 
							nSegment, nWall, w->m_nSegment);
			if (UpdateStats (message, 1, nSegment, nSide, -1, -1, -1, pSide->m_info.nWall)) return true;
			} 
		else if (w->m_nSide != nSide) {
			if (m_bAutoFixBugs) {
				sprintf_s (message, sizeof (message), 
							"FIXED: Wall sits at wrong side (segment=%d, side=%d, wall=%d, parent=%d)", 
							nSegment, nSide, nWall, w->m_nSegment);
				if (wallFixed [nWall])
					pSide->m_info.nWall = NO_WALL;
				else {
					ow = OppWall (nSegment, nSide);
					if (ow && (ow->Type () == w->Type ())) {
						pSegment->m_sides [w->m_nSide].m_info.nWall = NO_WALL;
						w->m_nSide = nSide;
						}
					else if (pSegment->m_sides [w->m_nSide].m_info.nWall == nWall)
						pSide->m_info.nWall = NO_WALL;
					else
						w->m_nSide = nSide;
					wallFixed [nWall] = 1;
					}
				}
			else
				sprintf_s (message, sizeof (message), 
							"ERROR: Wall sits at wrong side (segment=%d, side=%d, wall=%d, parent=%d)", 
							nSegment, nSide, nWall, w->m_nSegment);
			if (UpdateStats (message, 1, -1, -1, -1, -1, -1, pSide->m_info.nWall)) return true;
			}
		} 
	}

CWall *pWall = wallManager.Wall (0);
for (nWall = 0; nWall < wallCount; nWall++, pWall++) {
	DLE.MainFrame ()->Progress ().StepIt ();
	// check wall range type
	if (pWall->Type () > (DLE.IsD1File () ? WALL_CLOSED : DLE.IsStdLevel () ? WALL_CLOAKED : WALL_COLORED)) {
		sprintf_s (message, sizeof (message), 
					"ERROR: Wall type out of range (wall=%d, type=%d)", 
					nWall, pWall->Type ());
		if (UpdateStats (message, 1, pWall->m_nSegment, pWall->m_nSide, -1, -1, -1, nWall)) return true;
		}
		// check range of segment number that the wall points to
	if (pWall->m_nSegment >= segmentManager.Count ()) {
		sprintf_s (message, sizeof (message), 
					"ERROR: Wall sits in non-existant segment (wall=%d, segment=%d)", 
					nWall, pWall->m_nSegment);
		if (UpdateStats (message, 1, -1, -1, -1, -1, -1, nWall)) return true;
		} 
	else if (pWall->m_nSide >= 6) {
		// check range of side number that the wall points to
		sprintf_s (message, sizeof (message), 
					"ERROR: Wall sits on side which is out of range (wall=%d, side=%d)", 
					nWall, pWall->m_nSide);
		if (UpdateStats (message, 1, -1 -1, -1, -1, -1, nWall)) return true;
		}
	else {
		// check to make sure segment points back to wall
		pSide = segmentManager.Segment (pWall->m_nSegment)->m_sides + pWall->m_nSide;
		if (pSide->m_info.nWall != nWall) {
			w = wallManager.Wall (pSide->m_info.nWall);
			if ((pSide->m_info.nWall < wallCount) && (w->m_nSegment == pWall->m_nSegment) && (w->m_nSide == pWall->m_nSide)) {
				if (m_bAutoFixBugs) {
					sprintf_s (message, sizeof (message), 
								"FIXED: Duplicate wall found (wall=%d, segment=%d)", nWall, pWall->m_nSegment);
					wallManager.Delete (nWall);
					nWall--;
					pWall--;
					wallCount--;
					continue;
					}
				else 
					sprintf_s (message, sizeof (message), 
								"ERROR: Duplicate wall found (wall=%d, segment=%d)", nWall, pWall->m_nSegment);
				if (UpdateStats (message, 1, pWall->m_nSegment, pWall->m_nSide, -1, -1, -1, nWall)) return true;
				}
			else {
				if (m_bAutoFixBugs) {
					pSide->m_info.nWall = nWall;
					sprintf_s (message, sizeof (message), 
								"FIXED: Segment does not reference wall which sits in it (wall=%d, segment=%d)", 
								nWall, pWall->m_nSegment);
					}
				else 
					sprintf_s (message, sizeof (message), 
								"ERROR: Segment does not reference wall which sits in it (wall=%d, segment=%d)", 
								nWall, pWall->m_nSegment);
				if (UpdateStats (message, 1, pWall->m_nSegment, pWall->m_nSide, -1, -1, -1, nWall)) return true;
				}
			}
			// make sure trigger number of wall is in range
		if ((pWall->Info ().nTrigger != NO_TRIGGER) && (pWall->Info ().nTrigger >= triggerManager.WallTriggerCount ())) {
			if (m_bAutoFixBugs) {
				sprintf_s (message, sizeof (message), 
							"FIXED: Wall has invalid trigger (wall=%d, trigger=%d)", 
							nWall, pWall->Info ().nTrigger);
				pWall->Info ().nTrigger = NO_TRIGGER;
				}
			else
				sprintf_s (message, sizeof (message), 
							"ERROR: Wall has invalid trigger (wall=%d, trigger=%d)", 
							nWall, pWall->Info ().nTrigger);
			if (UpdateStats (message, 1, pWall->m_nSegment, pWall->m_nSide, -1, -1, -1, nWall)) return true;
			}
#if 1 // linked walls not supported in DLE and D2X-XL
		if (pWall->Info ().linkedWall != -1) {
			short invLinkedWall = pWall->Info ().linkedWall;
			if (m_bAutoFixBugs) {
				pWall->Info ().linkedWall = -1;
				sprintf_s (message, sizeof (message), 
							  "FIXED: Wall has invalid linked wall (wall=%d, linked wall=%d [%d])", 
							  nWall, invLinkedWall, pWall->Info ().linkedWall);
				}
			else
				sprintf_s (message, sizeof (message), 
							  "ERROR: Wall has invalid linked wall (wall=%d, linked wall=%d [%d])", 
							  nWall, invLinkedWall, pWall->Info ().linkedWall);
			}
#else
		if ((pWall->Info ().linkedWall < -1) || (pWall->Info ().linkedWall >= wallCount)) {
			if (m_bAutoFixBugs) {
				short	oppSeg, oppSide, invLinkedWall = pWall->Info ().linkedWall;
				if (theMine->OppositeSide (oppSeg, oppSide, pWall->m_nSegment, pWall->m_nSide)) {
					pWall->Info ().linkedWall = segmentManager.Segment (oppSeg)->m_sides [oppSide].m_info.nWall;
					if ((pWall->Info ().linkedWall < -1) || (pWall->Info ().linkedWall >= wallCount))
						pWall->Info ().linkedWall = -1;
					sprintf_s (message, sizeof (message), 
						"FIXED: Wall has invalid linked wall (wall=%d, linked wall=%d [%d])", 
						nWall, invLinkedWall, pWall->Info ().linkedWall);
					}
				}
			else
				sprintf_s (message, sizeof (message), 
					"ERROR: Wall has invalid linked wall (wall=%d, linked wall=%d)", 
					nWall, pWall->Info ().linkedWall);
			}
		else if (pWall->Info ().linkedWall >= 0) {
			short	oppSeg, oppSide;
			if (theMine->OppositeSide (oppSeg, oppSide, pWall->m_nSegment, pWall->m_nSide)) {
				short oppWall = segmentManager.Segment (oppSeg)->m_sides [oppSide].m_info.nWall;
				if ((oppWall < 0) || (oppWall >= wallCount)) {
					sprintf_s (message, sizeof (message), 
						"%s: Wall links to non-existant wall (wall=%d, linked side=%d, %d)", 
						m_bAutoFixBugs ? "FIXED" : "ERROR", 
						nWall, wallManager.Wall (pWall->Info ().linkedWall)->m_info.nSegment, wallManager.Wall (pWall->Info ().linkedWall)->nSide);
						if (m_bAutoFixBugs)
							pWall->Info ().linkedWall = -1;
					}
				else if (pWall->Info ().linkedWall != oppWall) {
					sprintf_s (message, sizeof (message), 
						"%s: Wall links to wrong opposite wall (wall=%d, linked side=%d, %d)", 
						m_bAutoFixBugs ? "FIXED" : "ERROR", 
						nWall, wallManager.Wall (pWall->Info ().linkedWall)->m_info.nSegment, wallManager.Wall (pWall->Info ().linkedWall)->nSide);
						if (m_bAutoFixBugs)
							pWall->Info ().linkedWall = oppWall;
					}
				}
			else {
				sprintf_s (message, sizeof (message), 
					"%s: Wall links to non-existant side (wall=%d, linked side=%d, %d)", 
					m_bAutoFixBugs ? "FIXED" : "ERROR", 
					nWall, wallManager.Wall (pWall->Info ().linkedWall)->m_info.nSegment, wallManager.Wall (pWall->Info ().linkedWall)->nSide);
				if (m_bAutoFixBugs)
					pWall->Info ().linkedWall = -1;
				}
			}
#endif
		if (UpdateStats (message, 1, pWall->m_nSegment, pWall->m_nSide, -1, -1, -1, nWall)) return true;
		// check wall nClip
		if ((pWall->Type () == WALL_CLOAKED) && (pWall->Info ().cloakValue > 31)) {
			if (m_bAutoFixBugs) {
				pWall->Info ().cloakValue = 31;
				sprintf_s (message, sizeof (message), "FIXED: Wall has invalid cloak value (wall=%d)", nWall);
				}
			else
				sprintf_s (message, sizeof (message), "ERROR: Wall has invalid cloak value (wall=%d)", nWall);
			}
		if (((pWall->Type () == WALL_BLASTABLE) || (pWall->Type () == WALL_DOOR)) &&
			 (   (pWall->Clip () < 0)
			  || (pWall->Clip () == 2)
			  || (pWall->Clip () == 8)
			  || (DLE.IsD1File () && pWall->Clip () > MAX_DOOR_ANIMS_D1)
			  || (DLE.IsD2File () && pWall->Clip () > MAX_DOOR_ANIMS_D2))) {
			if (m_bAutoFixBugs) {
				sprintf_s (message, sizeof (message), 
							  "FIXED: Illegal wall clip number (wall=%d, clip number=%d)", 
							  nWall, pWall->Clip ());
				pWall->Clip () = ((pWall->Clip () - 1) % ((DLE.IsD1File () ? MAX_DOOR_ANIMS_D1 : MAX_DOOR_ANIMS_D2) - 1)) + 1;
				}
			else
				sprintf_s (message, sizeof (message), 
							  "ERROR: Illegal wall clip number (wall=%d, clip number=%d)", 
							  nWall, pWall->Clip ());
			if (UpdateStats (message, 1, pWall->m_nSegment, pWall->m_nSide, -1, -1, -1, nWall)) return true;
			}
			// Make sure there is a child to the segment
		if (pWall->Type () != WALL_OVERLAY) {
			if (!(segmentManager.Segment (pWall->m_nSegment)->m_info.childFlags & (1<< pWall->m_nSide))) {
				sprintf_s (message, sizeof (message), 
							"ERROR: No adjacent segment for this door (wall=%d, segment=%d)", 
							nWall, pWall->m_nSegment);
				if (UpdateStats (message, 1, pWall->m_nSegment, pWall->m_nSide, -1, -1, -1, nWall)) return true;
				}
			else {
				nSegment = segmentManager.Segment (pWall->m_nSegment)->ChildId (pWall->m_nSide);
				CSegment *pSegment = segmentManager.Segment (nSegment);
				if ((nSegment >= 0 && nSegment < segmentManager.Count ()) &&
					 (pWall->Type () == WALL_DOOR || pWall->Type () == WALL_ILLUSION)) {
					// find segment's child side
					for (nSide = 0; nSide < 6; nSide++)
						if (pSegment->ChildId (nSide) == pWall->m_nSegment)
							break;
					if (nSide != 6) {  // if child's side found
						if (pSegment->m_sides[nSide].m_info.nWall >= wallManager.WallCount ()) {
							sprintf_s (message, sizeof (message), 
										"WARNING: No matching wall for this wall (wall=%d, segment=%d)", 
										nWall, nSegment);
							if (UpdateStats (message, 0, pWall->m_nSegment, pWall->m_nSide, -1, -1, -1, nWall)) return true;
							} 
						else {
							ushort wallnum2 = pSegment->m_sides[nSide].m_info.nWall;
							if ((wallnum2 < wallCount) &&
								 ((pWall->Clip () != wallManager.Wall (wallnum2)->Info ().nClip ||
									pWall->Type () != wallManager.Wall (wallnum2)->Type ()))) {
								sprintf_s (message, sizeof (message), 
											"WARNING: Matching wall for this wall is of different type or clip no. (wall=%d, segment=%d)", 
											nWall, nSegment);
								if (UpdateStats (message, 0, pWall->m_nSegment, pWall->m_nSide, -1, -1, -1, nWall)) return true;
								}
							}
						}
					}
				}
			}
		}
	}

	// make sure pSegment's wall points back to the segment
pSegment = segmentManager.Segment (0);
for (nSegment = 0; nSegment < segCount; nSegment++, pSegment++) {
	DLE.MainFrame ()->Progress ().StepIt ();
	pSide = pSegment->m_sides;
	for (nSide = 0; nSide < 6; nSide++, pSide++) {
		if (pSide->m_info.nWall <	wallCount) {
			nWall = pSide->m_info.nWall;
			if (nWall >= wallCount) {
				if (m_bAutoFixBugs) {
					pSide->m_info.nWall = wallCount;
					sprintf_s (message, sizeof (message), "FIXED: Segment has an invalid wall number (wall=%d, segment=%d)", nWall, nSegment);
					}
				else
					sprintf_s (message, sizeof (message), "ERROR: Segment has an invalid wall number (wall=%d, segment=%d)", nWall, nSegment);
				if (UpdateStats (message, 1, nSegment, nSide)) return true;
			} else {
				if (wallManager.Wall (nWall)->m_nSegment != nSegment) {
					if (m_bAutoFixBugs) {
						wallManager.Wall (nWall)->m_nSegment = nSegment;
						sprintf_s (message, sizeof (message), "FIXED: Segment's wall does not sit in segment (wall=%d, segment=%d)", nWall, nSegment);
						}
					else
						sprintf_s (message, sizeof (message), "ERROR: Segment's wall does not sit in segment (wall=%d, segment=%d)", nWall, nSegment);
					if (UpdateStats (message, 1, nSegment, pWall->m_nSide, -1, -1, -1, nWall)) return true;
					}
				}
			}
		}
	}

if (sub_errors == m_nErrors [0] && sub_warnings == m_nErrors [1]) {
	LBBugs ()->DeleteString (LBBugs ()->GetCount ()-1);
	LBBugs ()->AddString ("[Walls] (no errors)");
	}
return false;
}

//--------------------------------------------------------------------------
// unused verticies
// make sure the mine has an exit
//--------------------------------------------------------------------------

bool CDiagTool::CheckVertices (void) 
{
if (theMine == null) 
	return false;

//  bool found;
  int nSegment, nVertex, point;
  int nUnused = 0;

  short sub_errors = m_nErrors [0];
  short sub_warnings = m_nErrors [1];
  LBBugs ()->AddString ("[Vertices]");

for (nVertex = vertexManager.Count (); nVertex > 0; )
	vertexManager.Status (--nVertex) &= ~NEW_MASK;

// mark all used verts
CSegment *pSegment = segmentManager.Segment (0);
for (nSegment = segmentManager.Count (); nSegment; nSegment--, pSegment++)
	for (point = 0; point < 8; point++)
		if (pSegment->m_info.vertexIds [point] <= MAX_VERTEX)
			vertexManager.Status (pSegment->m_info.vertexIds [point]) |= NEW_MASK;

for (nVertex = vertexManager.Count (); nVertex > 0; ) {
	DLE.MainFrame ()->Progress ().StepIt ();
	if (!(vertexManager.Status (--nVertex) & NEW_MASK)) {
		nUnused++;
		if (m_bAutoFixBugs) {
			if (nVertex < --vertexManager.Count ())
				memcpy (vertexManager.Vertex (nVertex), vertexManager.Vertex (nVertex + 1), (vertexManager.Count () - nVertex) * sizeof (*vertexManager.Vertex (0)));
			CSegment *pSegment = segmentManager.Segment (0);
			for (nSegment = segmentManager.Count (); nSegment; nSegment--, pSegment++)
				for (point = 0; point < 8; point++)
					if (pSegment->m_info.vertexIds [point] >= nVertex)
						pSegment->m_info.vertexIds [point]--;
			}
		}
	}
for (nVertex = vertexManager.Count (); nVertex > 0; )
	vertexManager.Status (--nVertex) &= ~NEW_MASK;
if (nUnused) {
	if (m_bAutoFixBugs)
		sprintf_s (message, sizeof (message), "FIXED: %d unused vertices found", nUnused);
	else
		sprintf_s (message, sizeof (message), "WARNING: %d unused vertices found", nUnused);
	if (UpdateStats (message, 0)) return true;
	}

if (sub_errors == m_nErrors [0] && sub_warnings == m_nErrors [1]) {
	LBBugs ()->DeleteString (LBBugs ()->GetCount ()-1);
	LBBugs ()->AddString ("[Vertices] (no errors)");
	}
return false;
}

//eof check.cpp