

#include "Mine.h"
#include "dle-xp.h"

CBlockManager blockManager;

//------------------------------------------------------------------------------

#define CURRENT_POINT(a) ((current->Point () + (a)) % current->Side ()->VertexCount ())

//------------------------------------------------------------------------------

char *BLOCKOP_HINT =
	"The block of cubes will be saved relative to the current segment.\n"
	"Later, when you paste the block, it will be placed relative to\n"
	"the current segment at that time.  You can change the current side\n"
	"and the current point to affect the relative direction and\n"
	"rotation of the block.\n"
	"\n"
	"Would you like to proceed?";

static char* textureIds [2][2] = {{"    tmap_num %hd\n", "    tmap_num2 %hd\n"}, {"    BaseTex %hd\n", "    OvlTex %hd\n"}};
static char* vertexIds [2] = {"  vms_vector %hu %d %d %d\n", "  Vertex %hu %d %d %d\n"};

//------------------------------------------------------------------------------

void CBlockManager::SetupTransformation (CDoubleMatrix& t, CDoubleVector& o)
{
CSegment* pSegment = current->Segment ();
o = *pSegment->Vertex (current->SideId (), current->Point ());
// set x'
t.m.rVec = *pSegment->Vertex (current->SideId (), current->Point () + 1);
t.m.rVec -= o;
// calculate y'
CDoubleVector v = *pSegment->Vertex (current->SideId (), current->Point () + current->Side ()->VertexCount () - 1);
v -= o;
t.m.uVec = CrossProduct (t.m.rVec, v);
t.m.fVec = CrossProduct (t.m.rVec, t.m.uVec);
t.m.rVec.Normalize ();
t.m.uVec.Normalize ();
t.m.fVec.Normalize ();
}

//------------------------------------------------------------------------------

bool CBlockManager::Error (int argsFound, int argsNeeded, char* msg, int nSegment, char* szFunction)
{
if (argsNeeded == argsFound)
	return false;
undoManager.Unroll (szFunction);
if (nSegment < 0)
	ErrorMsg (msg);
else {
	char s [200];
	sprintf_s (s, sizeof (s), "%s (segment %d)", msg, nSegment);
	ErrorMsg (s);
	}
return true;
}

//------------------------------------------------------------------------------
// Read ()
//
// ACTION - Reads a segment's information in text form from a file.  Adds
//          new vertices if non-identical one does not exist.  Aborts if
//	    MAX_VERTICES is hit.
//
// Change - Now reads verts relative to current side
//------------------------------------------------------------------------------

short CBlockManager::Read (CFileManager& fp) 
{
	int				i, j, scanRes;
	short				origVertCount;
	CDoubleMatrix	t, r;
	CDoubleVector	/*xAxis, yAxis, zAxis,*/ origin;
	short				nNewSegs = 0, nNewWalls = 0, nNewTriggers = 0;
	int				byteBuf; // needed for scanning ubyte values
	CTrigger			* newTriggers = null;
	
m_oldSegments = m_newSegments = null;
// remember number of vertices for later
origVertCount = vertexManager.Count ();

// set origin
SetupTransformation (t, origin);
// now take the determinant
r.m.rVec.Set (t.m.uVec.v.y * t.m.fVec.v.z - t.m.fVec.v.y * t.m.uVec.v.z, 
			     t.m.fVec.v.y * t.m.rVec.v.z - t.m.rVec.v.y * t.m.fVec.v.z, 
			     t.m.rVec.v.y * t.m.uVec.v.z - t.m.uVec.v.y * t.m.rVec.v.z);
r.m.uVec.Set (t.m.fVec.v.x * t.m.uVec.v.z - t.m.uVec.v.x * t.m.fVec.v.z, 
			     t.m.rVec.v.x * t.m.fVec.v.z - t.m.fVec.v.x * t.m.rVec.v.z,
			     t.m.uVec.v.x * t.m.rVec.v.z - t.m.rVec.v.x * t.m.uVec.v.z);
r.m.fVec.Set (t.m.uVec.v.x * t.m.fVec.v.y - t.m.fVec.v.x * t.m.uVec.v.y,
			     t.m.fVec.v.x * t.m.rVec.v.y - t.m.rVec.v.x * t.m.fVec.v.y,
			     t.m.rVec.v.x * t.m.uVec.v.y - t.m.uVec.v.x * t.m.rVec.v.y);

nNewSegs = 0;
memset (m_xlatSegNum, 0xff, sizeof (m_xlatSegNum));

for (i = 0, j = segmentManager.Count (); i < j; i++) {
	segmentManager.Segment (i)->SetLink (m_oldSegments);
	m_oldSegments = segmentManager.Segment (i);
	}	

undoManager.Begin (__FUNCTION__, udAll);
while (!fp.EoF ()) {
	DLE.MainFrame ()->Progress ().SetPos (fp.Tell ());
// abort if there are not at least 8 vertices free
	if (VERTEX_LIMIT - vertexManager.Count () < 8) {
		undoManager.End (__FUNCTION__);
		ErrorMsg ("No more free vertices");
		return nNewSegs;
		}
	short nSegment = segmentManager.Add ();
	if (nSegment < 0) {
		undoManager.End (__FUNCTION__);
		ErrorMsg ("No more free segments");
		return nNewSegs;
		}
	CSegment* pSegment = segmentManager.Segment (nSegment);
	pSegment->SetLink (m_newSegments);
	m_newSegments = pSegment;
	pSegment->m_info.owner = -1;
	pSegment->m_info.group = -1;
	short nSegIdx = -1;
	if (Error (fscanf_s (fp.File (), "segment %hd\n", &nSegIdx), 1, "Invalid segment id", nSegIdx, __FUNCTION__))
		return 0;
	m_xlatSegNum [nSegIdx] = nSegment;
	// invert segment number so its children can be children can be fixed later
	pSegment->Index () = -nSegIdx - 1;

	// read in side information 
	CSide* pSide = pSegment->m_sides;
	for (short nSide = 0; nSide < MAX_SIDES_PER_SEGMENT; nSide++, pSide++) {
		short test;
		if (Error (fscanf_s (fp.File (), "  side %hd\n", &test), 1, "Invalid side id", nSegIdx, __FUNCTION__))
			return 0;
		int bVariableLight = m_bExtended && (test < 0);
		if (bVariableLight)
			test = -test;
		if (test != nSide) {
			undoManager.End (__FUNCTION__);
			ErrorMsg ("Invalid side number read");
			return 0;
			}
		pSide->m_info.nWall = NO_WALL;

		if (Error (fscanf_s (fp.File (), textureIds [m_bExtended][0], &pSide->m_info.nBaseTex), 1, "Invalid base texture id", nSegIdx, __FUNCTION__))
			return 0;
		if (Error (fscanf_s (fp.File (), textureIds [m_bExtended][1], &pSide->m_info.nOvlTex), 1,  "Invalid overlay texture id", nSegIdx, __FUNCTION__))
			return 0;
		for (i = 0; i < 4; i++) {
			int u, v;
			if (Error (fscanf_s (fp.File (), "    uvls %d %d %d\n", &u, &v, &pSide->m_info.uvls [i].l), 3, "Invalid UVL information", nSegIdx, __FUNCTION__))
				return 0;
			pSide->m_info.uvls [i].u = double (u) / 2048.0;
			pSide->m_info.uvls [i].v = double (v) / 2048.0;
			}
		if (m_bExtended) {
			ushort vertexIdIndex [4];
			if (Error (fscanf_s (fp.File (), "    vertex ids %hu %hu %hu %hu\n", 
						  &vertexIdIndex [0], &vertexIdIndex [1], &vertexIdIndex [2], &vertexIdIndex [3]),
						  4, "Invalid vertex id index", nSegIdx, __FUNCTION__))
				return 0;
			for (int j = 0; j < 4; j++)
				pSide->m_vertexIdIndex [j] = ubyte (vertexIdIndex [j]);
			pSide->DetectShape ();
			if (bVariableLight) {
				tVariableLight vl;
				if (Error (fscanf_s (fp.File (), "    variable light %u %d %d\n", &vl.mask, &vl.timer, &vl.delay), 3, "Invalid variable light data", nSegIdx, __FUNCTION__))
					return 0;
				lightManager.AddVariableLight (CSideKey (nSegment, nSide), vl.mask, vl.timer);
				}
			if (Error (fscanf_s (fp.File (), "    wall %hd\n", &pSide->m_info.nWall), 1, "Invalid wall id", nSegIdx, __FUNCTION__))
				return 0;
			if (pSide->m_info.nWall != NO_WALL) {
				CWall w;
				CTrigger t;
				w.Clear ();
				t.Clear ();
				scanRes = fscanf_s (fp.File (), "        segment %hd\n", &w.m_nSegment);
				scanRes += fscanf_s (fp.File (), "        side %hd\n", &w.m_nSide);
				scanRes += fscanf_s (fp.File (), "        hps %d\n", &w.Info ().hps);
				scanRes += fscanf_s (fp.File (), "        type %d\n", &byteBuf);
				w.Info ().type = (ubyte) byteBuf;
				scanRes += fscanf_s (fp.File (), "        flags %hd\n", &w.Info ().flags);
				scanRes += fscanf_s (fp.File (), "        state %d\n", &byteBuf);
				w.Info ().state = (ubyte) byteBuf;
				scanRes += fscanf_s (fp.File (), "        clip %d\n", &byteBuf);
				w.Info ().nClip = (ubyte) byteBuf;
				scanRes += fscanf_s (fp.File (), "        keys %d\n", &byteBuf);
				w.Info ().keys = (ubyte) byteBuf;
				scanRes += fscanf_s (fp.File (), "        cloak %d\n", &byteBuf);
				w.Info ().cloakValue = (ubyte) byteBuf;
				scanRes += fscanf_s (fp.File (), "        trigger %d\n", &byteBuf);
				if (Error (scanRes, 10, "Invalid wall information", nSegIdx, __FUNCTION__))
					return 0;
				w.Info ().nTrigger = (ubyte) byteBuf;
				if ((w.Info ().nTrigger >= 0) && (w.Info ().nTrigger < MAX_TRIGGERS)) {
					scanRes = fscanf_s (fp.File (), "			    type %d\n", &byteBuf);
					t.Info ().type = (ubyte) byteBuf;
					scanRes += fscanf_s (fp.File (), "			    flags %hd\n", &t.Info ().flags);
					scanRes += fscanf_s (fp.File (), "			    value %d\n", &t.Info ().value);
					scanRes += fscanf_s (fp.File (), "			    timer %d\n", &t.Info ().time);
					scanRes += fscanf_s (fp.File (), "			    count %hd\n", &t.Count ());
					for (i = 0; i < t.Count (); i++) {
						scanRes += fscanf_s (fp.File (), "			        segment %hd\n", &t [i].m_nSegment);
						scanRes += fscanf_s (fp.File (), "			        side %hd\n", &t [i].m_nSide);
						}
					if (Error (scanRes, 5 + 2 * t.Count (), "Invalid trigger information", nSegIdx, __FUNCTION__))
						return 0;
					}
				if (wallManager.HaveResources ()) {
					if ((w.Info ().nTrigger >= 0) && (w.Info ().nTrigger < MAX_TRIGGERS)) {
						if (!triggerManager.HaveResources ())
							w.Info ().nTrigger = NO_TRIGGER;
						else {
							w.Info ().nTrigger = (ubyte) triggerManager.Add ();
							CTrigger* pTrigger = triggerManager.Trigger (w.Info ().nTrigger);
							*pTrigger = t;
							pTrigger->Backup (opAdd);
							pTrigger->SetLink (newTriggers);
							newTriggers = pTrigger;
							++nNewTriggers;
							}
						}
					pSide->m_info.nWall = wallManager.Add (false);
					w.m_nSegment = nSegment;
					CWall* pWall = wallManager.Wall (pSide->m_info.nWall);
					*pWall = w;
					pWall->Backup (opAdd);
					++nNewWalls;
					}
				}
			}
		}

	short children [6];
	if (Error (fscanf_s (fp.File (), "  children %hd %hd %hd %hd %hd %hd\n", 
				  children + 0, children + 1, children + 2, children + 3, children + 4, children + 5), 
				  6, "Invalid child information", nSegIdx, __FUNCTION__))
		return 0;
	for (i = 0; i < 6; i++)
		pSegment->SetChild (i, children [i]);
	// read in vertices
	ubyte bShared = 0;
	pSegment->m_nShape = 0;
	for (i = 0; i < 8; i++) {
		int x, y, z;
		ushort index;
		if (Error (fscanf_s (fp.File (), vertexIds [m_bExtended], &index, &x, &y, &z), 4, "Invalid vertex information", nSegIdx, __FUNCTION__))
			return 0;
		if (m_bExtended) {
			pSegment->m_info.vertexIds [i] = index;
			if (index > MAX_VERTEX) {
				pSegment->m_nShape++;
				continue;
				}
			}
		else if (index != i) {
			undoManager.End (__FUNCTION__);
			ErrorMsg ("Invalid vertex number read");
			return 0;
			}
		// each vertex relative to the origin has a x', y', and z' component
		// adjust vertices relative to origin
		CDoubleVector v;
		v.Set (X2D (x), X2D (y), X2D (z));
		//v.Set (Dot (v, xAxis), Dot (v, yAxis), Dot (v, zAxis));
		v = r * v;
		v += origin;
		// add a new vertex
		// if this is the same as another vertex, then use that vertex number instead
		CVertex* pVertex = vertexManager.Find (v);
		ushort nVertex;
		if (pVertex != null) {
			nVertex = pSegment->m_info.vertexIds [i] = vertexManager.Index (pVertex);
			bShared |= 1 << i;
			}
		// else make a new vertex
		else  {
			nVertex;
			vertexManager.Add (&nVertex);
			vertexManager.Status (nVertex) |= NEW_MASK;
			pSegment->m_info.vertexIds [i] = nVertex;
			*vertexManager.Vertex (nVertex) = v;
			vertexManager.Vertex (nVertex)->Backup ();
			}
		vertexManager.Status (nVertex) |= TAGGED_MASK;
		}

#if 0
	// check each side whether it shares all four vertices with another side
	// if so, make the segment owning that side a child
	for (short nSide = 0; nSide < 6; nSide++) {
		if (pSegment->ChildId (nSide) >= 0) // has a child in the block
			continue;
		ushort nVertex;
		ushort sharedVerts [4];
		for (nVertex = 0; nVertex < 4; nVertex++) {
			ushort h = sideVertexTable [nSide][nVertex];
			if ((bShared & (1 << h)) == 0)
				break;
			sharedVerts [nVertex] = pSegment->m_info.vertexIds [h];
			}
		if (nVertex == 4) {
			for (nVertex = 0; nVertex < 4; nVertex++)
				vertexManager.Status (sharedVerts [nVertex]) |= SHARED_MASK;
			for (CSegment* pChildSeg = m_oldSegments; pChildSeg != null; pChildSeg = dynamic_cast<CSegment*>(pChildSeg->GetLink ())) {
				short nChildSide;
				for (nChildSide = 0; nChildSide < 6; nChildSide++) {
					if (pChildSeg->ChildId (nChildSide) != -1)
						continue;
					short nChildVert;
					for (nChildVert = 0; nChildVert < 4; nChildVert++) {
						if (!pChildSeg->Vertex (sideVertexTable [nChildSide][nChildVert])->IsTagged (SHARED_MASK))
							break;
						}
					if (nChildVert == 4) {
						pChildSeg->SetChild (nChildSide, nSegment);
						short nChildSeg = segmentManager.Index (pChildSeg);
						pSegment->SetChild (nSide, nChildSeg);
						segmentManager.SetTextures (CSideKey (nSegment, nSide), 0, 0); 
						segmentManager.SetTextures (CSideKey (nChildSeg, nChildSide), 0, 0); 
						pChildSeg = null; // break out of outer loop
						break;
						}	
					}
				if (pChildSeg == null)
					break;
				}
			for (nVertex = 0; nVertex < 4; nVertex++)
				vertexManager.Status (sharedVerts [nVertex]) &= ~SHARED_MASK;
			}
		}
#endif
	// mark vertices
	if (Error (fscanf_s (fp.File (), "  static_light %d\n", &pSegment->m_info.staticLight), 1, "Invalid static light information", nSegIdx, __FUNCTION__))
		return 0;
	if (m_bExtended) {
		scanRes = fscanf_s (fp.File (), "  special %hhd\n", &pSegment->m_info.function);
		scanRes += fscanf_s (fp.File (), "  matcen_num %d\n", &byteBuf);
		pSegment->m_info.nProducer = (ubyte) byteBuf;
		scanRes += fscanf_s (fp.File (), "  value %d\n", &byteBuf);
		pSegment->m_info.value = (ubyte) byteBuf;
		scanRes += fscanf_s (fp.File (), "  child_bitmask %d\n", &byteBuf);
		pSegment->m_info.childFlags = (ubyte) byteBuf;
		scanRes += fscanf_s (fp.File (), "  wall_bitmask %d\n", &byteBuf);
		if (Error (scanRes, 5, "Invalid extended segment information", nSegIdx, __FUNCTION__))
			return 0;
		pSegment->m_info.wallFlags = (ubyte) byteBuf;
		switch (pSegment->m_info.function) {
			case SEGMENT_FUNC_PRODUCER:
				if (!segmentManager.CreateProducer (nSegment, SEGMENT_FUNC_PRODUCER, false, false))
					pSegment->m_info.function = 0;
				break;
			case SEGMENT_FUNC_REPAIRCEN:
				if (!segmentManager.CreateProducer (nSegment, SEGMENT_FUNC_REPAIRCEN, false, false))
					pSegment->m_info.function = 0;
				break;
			case SEGMENT_FUNC_ROBOTMAKER:
				if (!segmentManager.CreateRobotMaker (nSegment, false, false))
					pSegment->m_info.function = 0;
				break;
			case SEGMENT_FUNC_EQUIPMAKER:
				if (!segmentManager.CreateEquipMaker (nSegment, false, false))
					pSegment->m_info.function = 0;
				break;
			case SEGMENT_FUNC_REACTOR:
				if (!segmentManager.CreateReactor (nSegment, false, false))
					pSegment->m_info.function = 0;
				break;
			default:
				break;
			}
		}
	else {
		pSegment->m_info.function = 0;
		pSegment->m_info.nProducer = -1;
		pSegment->m_info.value = -1;
		}
	pSegment->Tag (TAGGED_MASK); // no other bits
	// calculate childFlags
	pSegment->m_info.childFlags = 0;
	for (i = 0; i < MAX_SIDES_PER_SEGMENT; i++) {
		if (pSegment->ChildId (i) >= 0)
			pSegment->m_info.childFlags |= (1 << i);
		}
	pSegment->Backup ();
	nNewSegs++;
	}

#if 0
while (m_newSegments != null) {
	short nSegment = segmentManager.Index (m_newSegments);
	for (int nSide = 0; nSide < 6; nSide++) {
		if (m_newSegments->ChildId (nSide) < 0)
			segmentManager.Join (CSideKey (nSegment, nSide), true);
		}
	m_newSegments = dynamic_cast<CSegment*> (m_newSegments->GetLink ());
	}
#endif

while (newTriggers != null) {
	CTrigger* pTrigger = newTriggers;
	newTriggers = dynamic_cast<CTrigger*> (pTrigger->GetLink ());
	for (j = 0; j < pTrigger->Count (); j++) {
		if (pTrigger->Segment (j) >= 0)
			pTrigger->Segment (j) = m_xlatSegNum [pTrigger->Segment (j)];
		else if (pTrigger->Count () == 1) {
			triggerManager.Delete (triggerManager.Index (pTrigger));
			i--;
			}
		else {
			pTrigger->Delete (j);
			}
		}
	}

undoManager.End (__FUNCTION__);
sprintf_s (message, sizeof (message),
			  " Block tool: %d blocks, %d walls, %d triggers pasted.", 
			  nNewSegs, nNewWalls, nNewTriggers);
DEBUGMSG (message);
return (nNewSegs);
}

//------------------------------------------------------------------------------
// dump_seg_info()
//
// ACTION - Writes a segment's information in text form to a fp.  Uses
//          actual coordinate information instead of vertex number.  Only
//          saves segment information (no Walls (), Objects (), or Triggers ()).
//
// Change - Now saves verts relative to the current side (point 0)
//          Uses x',y',z' axis where:
//          y' is in the neg dirction of line0
//          z' is in the neg direction orthogonal to line0 & line3
//          x' is in the direction orghogonal to x' and y'
//
//------------------------------------------------------------------------------

void CBlockManager::Write (CFileManager& fp) 
{
	short				j;
	CVertex			origin;
	CDoubleMatrix	m;
	CDoubleVector	v;
	short				nVertex;

// set origin
SetupTransformation (m, origin);

CSegment* pSegment = segmentManager.Segment (0);
int nSegments = segmentManager.Count ();
for (short nSegment = 0; nSegment < nSegments; nSegment++) {
	DLE.MainFrame ()->Progress ().StepIt ();
	CSegment* pSegment = segmentManager.Segment (nSegment);
	if (pSegment->IsTagged ()) {
		fprintf (fp.File (), "segment %d\n", nSegment);
		CSide* pSide = pSegment->m_sides;
		for (short nSide = 0; nSide < MAX_SIDES_PER_SEGMENT; nSide++, pSide++) {
			CVariableLight* pLight = lightManager.VariableLight (lightManager.VariableLight (CSideKey (nSegment, nSide)));
			fprintf (fp.File (), "  side %d\n", (pLight == null || !m_bExtended) ? nSide : -nSide);
			fprintf (fp.File (), "    %s %d\n", m_bExtended ? "BaseTex" : "tmap_num", pSide->BaseTex ());
			fprintf (fp.File (), "    %s %d\n", m_bExtended ? "OvlTex" : "tmap_num2", pSide->m_info.nOvlTex);
			for (j = 0; j < 4; j++) {
				fprintf (fp.File (), "    uvls %d %d %d\n", (int) Round (pSide->m_info.uvls [j].u * 2048.0), (int) Round (pSide->m_info.uvls [j].v * 2048.0), pSide->m_info.uvls [j].l);
				}
			if (m_bExtended) {
				fprintf (fp.File (), "    vertex ids %hu %hu %hu %hu\n",
							pSide->m_vertexIdIndex [0], pSide->m_vertexIdIndex [1], pSide->m_vertexIdIndex [2], pSide->m_vertexIdIndex [3]);
				if (pLight) 
					fprintf (fp.File (), "    variable light %u %d %d\n", pLight->m_info.mask, pLight->m_info.timer, pLight->m_info.delay);
				fprintf (fp.File (), "    wall %d\n", pSide->m_info.nWall);
				if (pSide->m_info.nWall != NO_WALL) {
					CWall* pWall = pSide->Wall ();
					fprintf (fp.File (), "        segment %d\n", pWall->m_nSegment);
					fprintf (fp.File (), "        side %d\n", pWall->m_nSide);
					fprintf (fp.File (), "        hps %d\n", pWall->Info ().hps);
					fprintf (fp.File (), "        type %d\n", pWall->Type ());
					fprintf (fp.File (), "        flags %d\n", pWall->Info ().flags);
					fprintf (fp.File (), "        state %d\n", pWall->Info ().state);
					fprintf (fp.File (), "        clip %d\n", pWall->Info ().nClip);
					fprintf (fp.File (), "        keys %d\n", pWall->Info ().keys);
					fprintf (fp.File (), "        cloak %d\n", pWall->Info ().cloakValue);
					if (pWall->Info ().nTrigger == NO_TRIGGER)
						fprintf (fp.File (), "        trigger %u\n", NO_TRIGGER);
					else {
						CTrigger *pTrigger = pWall->Trigger ();
						int iTarget, count = 0;
						// count trigger targets in marked area
						for (iTarget = 0; iTarget < pTrigger->Count (); iTarget++)
							if (segmentManager.Segment (pTrigger->Segment (iTarget))->IsTagged ())
								count++;
						fprintf (fp.File (), "        trigger %d\n", pWall->Info ().nTrigger);
						fprintf (fp.File (), "			    type %d\n", pTrigger->Type ());
						fprintf (fp.File (), "			    flags %d\n", pTrigger->Info ().flags);
						fprintf (fp.File (), "			    value %d\n", pTrigger->Info ().value);
						fprintf (fp.File (), "			    timer %d\n", pTrigger->Info ().time);
						fprintf (fp.File (), "			    count %d\n", count);
						for (iTarget = 0; iTarget < pTrigger->Count (); iTarget++)
							if (segmentManager.Segment (pTrigger->Segment (iTarget))->IsTagged ()) {
								fprintf (fp.File (), "			        segment %d\n", pTrigger->Segment (iTarget));
								fprintf (fp.File (), "			        side %d\n", pTrigger->Side (iTarget));
								}
						}
					}
				}
			}
		fprintf (fp.File (), "  children");
		for (short nSide = 0; nSide < 6; nSide++) {
			short nChild = pSegment->ChildId (nSide);
			fprintf (fp.File (), " %d", ((nChild < 0) || !segmentManager.Segment (nChild)->IsTagged ()) ? -1 : nChild);
			}
		fprintf (fp.File (), "\n");
		// save vertices
		for (short i = 0; i < 8; i++) {
			// each vertex relative to the origin has a x', y', and z' component
			// which is a constant (k) times the axis
			// k = (B*A)/(A*A) where B is the vertex relative to the origin
			//                       A is the axis unit vertex (always 1)
			nVertex = pSegment->m_info.vertexIds [i];
			CVertex v = *vertexManager.Vertex (nVertex) - origin;
			if (m_bExtended) {
				if (pSegment->m_info.vertexIds [i] > MAX_VERTEX)
					fprintf (fp.File (), "  Vertex %hu 0 0 0 \n", pSegment->m_info.vertexIds [i]);
				else {
					v = m * v;
					fprintf (fp.File (), "  Vertex %hu %d %d %d\n", 
								pSegment->m_info.vertexIds [i], D2X (v.v.x), D2X (v.v.y), D2X (v.v.z));
					}
				}
			else {
			fprintf (fp.File (), "  vms_vector %hu %d %d %d\n", 
						ushort (i), D2X (v ^ m.m.rVec), D2X (v ^ m.m.uVec), D2X (v ^ m.m.fVec));
				}
			}
		fprintf (fp.File (), "  static_light %d\n",pSegment->m_info.staticLight);
		if (m_bExtended) {
			fprintf (fp.File (), "  special %d\n", pSegment->m_info.function);
			fprintf (fp.File (), "  matcen_num %d\n", pSegment->m_info.nProducer);
			fprintf (fp.File (), "  value %d\n", pSegment->m_info.value);
			fprintf (fp.File (), "  child_bitmask %d\n", pSegment->m_info.childFlags);
			fprintf (fp.File (), "  wall_bitmask %d\n", pSegment->m_info.wallFlags);
			}
		}
	}
}

//------------------------------------------------------------------------------

void CBlockManager::Copy (char* filename, bool bDelete)
{
if (tunnelMaker.Active ()) 
	return;

  // make sure some cubes are marked
short count = segmentManager.TaggedCount ();
if (count == 0) {
	ErrorMsg ("No block marked.\n\nUse 'M' or shift left mouse button\nto mark one or more cubes.");
	return;
	}

if (!DLE.ExpertMode () && Query2Msg (BLOCKOP_HINT, MB_YESNO) != IDYES)
	return;

char szFile [256];
if (filename && *filename) {
	if (strcmp (filename, "dle_temp.blx"))
		strcpy_s (szFile, sizeof (szFile), filename);
	else
		sprintf (szFile, "%sdle_temp.blx", DLE.AppFolder ());
	}
else {
	szFile [0] = '\0';
	if (!BrowseForFile (FALSE, 
							  m_bExtended ? "blx" : "blk", szFile, 
							  "Block file|*.blk|"
							  "Extended block file|*.blx|"
							  "All Files|*.*||",
							  OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT,
							  DLE.MainFrame ()))
		return;
	}	
_strlwr_s (szFile, sizeof (szFile));
// Check if the user has specified .blk. We default to .blx otherwise
char* pszExt = strstr (szFile, ".blk");
m_bExtended = DLE.IsD2XLevel () || !pszExt;
// Has the user specified any (recognized) extension at all? If not append one
if (!pszExt)
	pszExt = strstr (szFile, ".blx");
if (!pszExt)
	strcat (szFile, m_bExtended ? ".blx" : ".blk");

CFileManager fp;
if (!fp.Open (szFile, "w")) {
	ErrorMsg ("Unable to open block file");
	return;
	}
//undoManager.UpdateBuffer(0);
strcpy_s (m_filename, sizeof (m_filename), szFile); // remember file for quick paste
fprintf (fp.File (), m_bExtended ? "DMB_EXT_BLOCK_FILE\n" : "DMB_BLOCK_FILE\n");
DLE.MainFrame ()->InitProgress (segmentManager.Count ());
Write (fp);
DLE.MainFrame ()->Progress ().DestroyWindow ();
fp.Close ();

if (bDelete)
	Delete ();
else {
	sprintf_s (message, sizeof (message), " Block tool: %d blocks copied to '%s' relative to current side.", count, szFile);
	DEBUGMSG (message);
  // wrap back then forward to make sure segment is valid
	segmentManager.SetLinesToDraw ();
	DLE.MineView ()->Refresh ();
	}
}

//------------------------------------------------------------------------------

void CBlockManager::Cut (void)
{
Copy (null, true);
}

//------------------------------------------------------------------------------

void CBlockManager::Paste (void) 
{
if (tunnelMaker.Active ()) 
	return;
// Initialize data for fp open dialog
  char szFile [256] = "\0";

if (!BrowseForFile (TRUE, 
	                 m_bExtended ? "blx" : "blk", szFile, 
						  "Block file|*.blk|"
						  "Extended block file|*.blx|"
						  "All Files|*.*||",
						  OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST,
						  DLE.MainFrame ()))
	return;
if (!Read (szFile))
	DLE.MineView ()->SetSelectMode (BLOCK_MODE);
}

//------------------------------------------------------------------------------

int CBlockManager::Read (char *filename) 
{
	CFileManager fp;

_strlwr_s (filename, 256);
if (!fp.Open (filename, "r")) {
	ErrorMsg ("Unable to open block file");
	return 1;
	}	

fscanf_s (fp.File (), "%s\n", &message, sizeof (message));
if (!strncmp (message, "DMB_BLOCK_FILE", 14))
	m_bExtended = false;
else if (!strncmp (message, "DMB_EXT_BLOCK_FILE", 18))
	m_bExtended = true;
else {
	ErrorMsg ("This is not a block file.");
	fp.Close ();
	return 2;
	}

strcpy_s (m_filename, sizeof (m_filename), filename); // remember file for quick paste

// untag all segmentManager.Segment ()
// set up all seg_numbers (makes sure there are no negative seg_numbers)
undoManager.Begin (__FUNCTION__, udAll);
DLE.MineView ()->DelayRefresh (true);
CSegment* pSegment = segmentManager.Segment (0);
for (short nSegment = 0; nSegment < SEGMENT_LIMIT; nSegment++, pSegment++) {
	pSegment->Index () = nSegment;
	pSegment->UnTag ();
	}

// untag all vertices
vertexManager.UnTagAll (TAGGED_MASK | NEW_MASK);

DLE.MainFrame ()->InitProgress (fp.Length ());
short count = Read (fp);
DLE.MainFrame ()->Progress ().DestroyWindow ();

// int up the new segmentManager.Segment () children
for (CSegment* pNewSeg = m_newSegments; pNewSeg != null; pNewSeg = dynamic_cast<CSegment*>(pNewSeg->GetLink ())) {
	// if child has a segment number that was just inserted, set it to the
	//  segment's offset number, otherwise set it to -1
	for (short nSide = 0; nSide < MAX_SIDES_PER_SEGMENT; nSide++) {
		if (pNewSeg->HasChild (nSide)) // has a child in the block
			pNewSeg->SetChild (nSide, m_xlatSegNum [pNewSeg->ChildId (nSide)]);
		else {
			CVertex* v1 = vertexManager.Vertex (pNewSeg->m_info.vertexIds [pNewSeg->Side (nSide)->VertexIdIndex (0)]);
			for (CSegment* pOldSeg = m_oldSegments; pOldSeg != null; pOldSeg = dynamic_cast<CSegment*>(pOldSeg->GetLink ())) {
				CSide* pSide = pOldSeg->Side (0);
				for (short nChildSide = 0; nChildSide < 6; nChildSide++, pSide++) {
					for (short nChildVertex = 0; nChildVertex < 4; nChildVertex++) {
						CVertex* v2 = vertexManager.Vertex (pOldSeg->m_info.vertexIds [pSide->VertexIdIndex (nChildVertex)]);
						if ((fabs (v1->v.x - v2->v.x) < 160.0) && (fabs (v1->v.y - v2->v.y) < 160.0) && (fabs (v1->v.z - v2->v.z) < 160.0)) {
							segmentManager.Link (segmentManager.Index (pNewSeg), nSide, segmentManager.Index (pOldSeg), nChildSide, 3.0);
							break;
							}
						}
					} 
				}
			}
		}
	}
// clear all new vertices as such
vertexManager.UnTagAll (NEW_MASK);
// now set all seg_numbers
pSegment = segmentManager.Segment (0);
for (short nSegment = 0; nSegment < segmentManager.Count (); nSegment++, pSegment++)
	pSegment->Index () = nSegment;
fp.Close ();
DLE.MineView ()->Refresh ();
undoManager.End (__FUNCTION__);
DLE.MineView ()->DelayRefresh (false);
DLE.MineView ()->Refresh ();
return 0;
}

//------------------------------------------------------------------------------

void CBlockManager::QuickCopy (void)
{
Copy ("dle_temp.blx");
}

//------------------------------------------------------------------------------

void CBlockManager::QuickPaste (void)
{
if (!*m_filename) {
	Paste ();
	return;
	}

if (tunnelMaker.Active ()) 
	return;
if (!Read (m_filename))
	DLE.MineView ()->SetSelectMode (BLOCK_MODE);
}

//------------------------------------------------------------------------------

void CBlockManager::Delete (void)
{
short nSegment, count;

if (tunnelMaker.Active ()) 
	return;
// make sure some cubes are marked
count = segmentManager.TaggedCount ();
if (!count) {
	ErrorMsg ("No block marked.\n\n"
				 "Use 'M' or shift left mouse button\n"
				 "to mark one or more cubes.");
	return;
	}

// delete segmentManager.Segment () from last to first because segmentManager.Count ()
// is effected for each deletion.  When all segmentManager.Segment () are marked
// the segmentManager.Count () will be decremented for each nSegment in loop.
if (QueryMsg ("Are you sure you want to delete the marked cubes?") != IDYES)
	return;

undoManager.Begin (__FUNCTION__, udAll);
DLE.MineView ()->DelayRefresh (true);

DLE.MainFrame ()->InitProgress (segmentManager.Count ());
for (nSegment = segmentManager.Count () - 1; nSegment >= 0; nSegment--) {
	DLE.MainFrame ()->Progress ().StepIt ();
	if (segmentManager.Segment (nSegment)->IsTagged ()) {
		if (segmentManager.Count () <= 1)
			break;
		if (objectManager.Object (0)->m_info.nSegment == nSegment) {
			short nNewSeg = (nSegment < segmentManager.Count() - 1) ? nSegment + 1 : (nSegment > 0) ? nSegment - 1 : -1;
			if (nNewSeg < 0)
				continue;
			objectManager.Move (objectManager.Object (0), nNewSeg);
			}
		segmentManager.Delete (nSegment, false); // delete pSegment w/o asking "are you sure"
		}
	}
vertexManager.DeleteUnused ();
DLE.MainFrame ()->Progress ().DestroyWindow ();
// wrap back then forward to make sure segment is valid
Wrap (selections [0].m_nSegment, -1, 0, segmentManager.Count () - 1);
Wrap (selections [0].m_nSegment, 1, 0, segmentManager.Count () - 1);
Wrap (selections [1].m_nSegment, -1, 0, segmentManager.Count () - 1);
Wrap (selections [1].m_nSegment, 1, 0, segmentManager.Count () - 1);
undoManager.End (__FUNCTION__);
segmentManager.SetLinesToDraw ();
DLE.MineView ()->DelayRefresh (false);
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------------
//eof block.cpp