// Copyright (C) 1997 Bryan Aamot
#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "stophere.h"
#include "define.h"
#include "types.h"
#include "dle-xp.h"
#include "mine.h"
#include "global.h"
#include "toolview.h"
#include "cfile.h"
#include "hogmanager.h"

#define CURRENT_POINT(a) ((current.m_nPoint + (a))&0x03)

char *BLOCKOP_HINT =
	"The block of cubes will be saved relative to the current cube.\n"
	"Later, when you paste the block, it will be placed relative to\n"
	"the current cube at that time.  You can change the current side\n"
	"and the current point to affect the relative direction and\n"
	"rotation of the block.\n"
	"\n"
	"Would you like to proceed?";

//---------------------------------------------------------------------------
// ReadSegmentInfo()
//
// ACTION - Reads a segment's information in text form from a file.  Adds
//          new vertices if non-identical one does not exist.  Aborts if
//	    MAX_VERTICES is hit.
//
// Change - Now reads verts relative to current side
//---------------------------------------------------------------------------

short CMine::ReadSegmentInfo (CFileManager& fp) 
{
	CSegment			*segP;
	CSide				*sideP;
	short				nSegment, nSide, nVertex;
	short				i, j, test;
	short				origVertCount, k;
	CVertex			origin, vVertex;
	CDoubleVector	xPrime, yPrime, zPrime, v;
	CDoubleVector	xAxis, yAxis, zAxis;
	short				nNewSegs = 0, nNewWalls = 0, nNewTriggers = 0, nNewObjects = 0;
	short				xlatSegNum [SEGMENT_LIMIT];
	int				byteBuf;

// remember number of vertices for later
origVertCount = VertCount ();

// set origin
segP = current.Segment ();
nSide = current.m_nSide;
origin = *Vertices (segP->m_info.verts [sideVertTable [nSide][CURRENT_POINT(0)]]);
// set x'
xPrime = *Vertices (segP->m_info.verts [sideVertTable [nSide][CURRENT_POINT(1)]]) - origin;
// calculate y'
vVertex = *Vertices (segP->m_info.verts [sideVertTable [nSide][CURRENT_POINT(3)]]) - origin;
yPrime = CrossProduct (xPrime, CDoubleVector (vVertex));
zPrime = CrossProduct (xPrime, yPrime);
xPrime.Normalize ();
yPrime.Normalize ();
zPrime.Normalize ();

// now take the determinant
xAxis.Set (yPrime.v.y * zPrime.v.z - zPrime.v.y * yPrime.v.z, 
			  zPrime.v.y * xPrime.v.z - xPrime.v.y * zPrime.v.z, 
			  xPrime.v.y * yPrime.v.z - yPrime.v.y * xPrime.v.z);
yAxis.Set (zPrime.v.x * yPrime.v.z - yPrime.v.x * zPrime.v.z, 
			  xPrime.v.x * zPrime.v.z - zPrime.v.x * xPrime.v.z,
			  yPrime.v.x * xPrime.v.z - xPrime.v.x * yPrime.v.z);
zAxis.Set (yPrime.v.x * zPrime.v.y - zPrime.v.x * yPrime.v.y,
			  zPrime.v.x * xPrime.v.y - xPrime.v.x * zPrime.v.y,
			  xPrime.v.x * yPrime.v.y - yPrime.v.x * xPrime.v.y);

nNewSegs = 0;
memset (xlatSegNum, 0xff, sizeof (xlatSegNum));
while (!fp.EoF ()) {
	DLE.MainFrame ()->Progress ().SetPos (fp.Tell ());
	if (SegCount () >= MAX_SEGMENTS) {
		ErrorMsg ("No more free segments");
		return (nNewSegs);
		}
// abort if there are not at least 8 vertices free
	if (MAX_VERTICES - VertCount () < 8) {
		ErrorMsg ("No more free vertices");
		return(nNewSegs);
		}
	nSegment = SegCount ();
	segP = GetSegment (nSegment);
	segP->m_info.owner = -1;
	segP->m_info.group = -1;
	fscanf_s (fp.File (), "segment %hd\n", &segP->m_info.nIndex);
	xlatSegNum [segP->m_info.nIndex] = nSegment;
	// invert segment number so its children can be children can be fixed later
	segP->m_info.nIndex = ~segP->m_info.nIndex;

	// read in side information 
	sideP = segP->m_sides;
	for (int nSide = 0; nSide < MAX_SIDES_PER_SEGMENT; nSide++, sideP++) {
		fscanf_s (fp.File (), "  side %hd\n", &test);
		if (test != nSide) {
			ErrorMsg ("Invalid side number read");
			return (0);
			}
		sideP->m_info.nWall = NO_WALL;
		fscanf_s (fp.File (), "    tmap_num %hd\n",&sideP->m_info.nBaseTex);
		fscanf_s (fp.File (), "    tmap_num2 %hd\n",&sideP->m_info.nOvlTex);
		for (j = 0; j < 4; j++)
			fscanf_s (fp.File (), "    uvls %hd %hd %hd\n",
						&sideP->m_info.uvls [j].u,
						&sideP->m_info.uvls [j].v,
						&sideP->m_info.uvls [j].l);
		if (bExtBlkFmt) {
			fscanf_s (fp.File (), "    nWall %d\n",&byteBuf);
			sideP->m_info.nWall = (ushort) byteBuf;
			if (sideP->m_info.nWall != NO_WALL) {
				CWall w;
				CTrigger t;
				w.Clear ();
				t.Clear ();
				fscanf_s (fp.File (), "        segment %ld\n", &w.m_nSegment);
				fscanf_s (fp.File (), "        side %ld\n", &w.m_nSide);
				fscanf_s (fp.File (), "        hps %ld\n", &w.m_info.hps);
				fscanf_s (fp.File (), "        type %d\n", &byteBuf);
				w.m_info.type = byteBuf;
				fscanf_s (fp.File (), "        flags %d\n", &byteBuf);
				w.m_info.flags = byteBuf;
				fscanf_s (fp.File (), "        state %d\n", &byteBuf);
				w.m_info.state = byteBuf;
				fscanf_s (fp.File (), "        nClip %d\n", &byteBuf);
				w.m_info.nClip = byteBuf;
				fscanf_s (fp.File (), "        keys %d\n", &byteBuf);
				w.m_info.keys = byteBuf;
				fscanf_s (fp.File (), "        cloak %d\n", &byteBuf);
				w.m_info.cloakValue = byteBuf;
				fscanf_s (fp.File (), "        trigger %d\n", &byteBuf);
				w.m_info.nTrigger = byteBuf;
				if ((w.m_info.nTrigger >= 0) && (w.m_info.nTrigger < MAX_TRIGGERS)) {
					fscanf_s (fp.File (), "			    type %d\n", &byteBuf);
					t.m_info.type = byteBuf;
					fscanf_s (fp.File (), "			    flags %hd\n", &t.m_info.flags);
					fscanf_s (fp.File (), "			    value %ld\n", &t.m_info.value);
					fscanf_s (fp.File (), "			    timer %d\n", &t.m_info.time);
					fscanf_s (fp.File (), "			    count %hd\n", &t.m_count);
					int iTarget;
					for (iTarget = 0; iTarget < t.m_count; iTarget++) {
						fscanf_s (fp.File (), "			        segP %hd\n", &t [iTarget].m_nSegment);
						fscanf_s (fp.File (), "			        side %hd\n", &t [iTarget].m_nSide);
						}
					}
				if (MineInfo ().walls.count < MAX_WALLS) {
					if ((w.m_info.nTrigger >= 0) && (w.m_info.nTrigger < MAX_TRIGGERS)) {
						if (MineInfo ().triggers.count >= MAX_TRIGGERS)
							w.m_info.nTrigger = NO_TRIGGER;
						else {
							w.m_info.nTrigger = MineInfo ().triggers.count++;
							++nNewTriggers;
							*Triggers (w.m_info.nTrigger) = t;
							}
						}
					nNewWalls++;
					sideP->m_info.nWall = MineInfo ().walls.count++;
					w.m_nSegment = nSegment;
					*Walls (sideP->m_info.nWall) = w;
					}
				}
#if 0
			fscanf_s (fp.File (), "    object_num %hd\n",&segObjCount);
			while (segObjCount) {
				CGameObject o;
				memset (&o, 0, sizeof (o));
				fscanf_s (fp.File (), "            signature %hd\n", &o.signature);
				fscanf_s (fp.File (), "            type %d\n", &byteBuf);
				o.type = (char) byteBuf;
				fscanf_s (fp.File (), "            id %d\n", &byteBuf);
				o.id = (char) byteBuf;
				fscanf_s (fp.File (), "            controlType %d\n", &byteBuf);
				o.controlType = (byte) byteBuf;
				fscanf_s (fp.File (), "            movementType %d\n", &byteBuf);
				o.movementType = (byte) byteBuf;
				fscanf_s (fp.File (), "            renderType %d\n", &byteBuf);
				o.renderType = (byte) byteBuf;
				fscanf_s (fp.File (), "            flags %d\n", &byteBuf);
				o.flags = (byte) byteBuf;
				o.nSegment = nSegment;
				fscanf_s (fp.File (), "            pos %ld %ld %ld\n", &o.pos.x, &o.pos.y, &o.pos.z);
				memcpy (&o.lastPos, &o.pos, sizeof (o.pos));
				fscanf_s (fp.File (), "            orient %ld %ld %ld %ld %ld %ld %ld %ld %ld\n", 
													&o.orient.rVec.x, &o.orient.rVec.y, &o.orient.rVec.z,
													&o.orient.uVec.x, &o.orient.uVec.y, &o.orient.uVec.z,
													&o.orient.fVec.x, &o.orient.fVec.y, &o.orient.fVec.z);
				fscanf_s (fp.File (), "            nSegment %hd\n", &o.nSegment);
				fscanf_s (fp.File (), "            size %ld\n", &o.size);
				fscanf_s (fp.File (), "            shields %ld\n", &o.shields);
				fscanf_s (fp.File (), "            contents.type %d\n", &byteBuf);
				o.contents.type = (char) byteBuf;
				fscanf_s (fp.File (), "            contents.id %d\n", &byteBuf);
				o.contents.id = (char) byteBuf;
				fscanf_s (fp.File (), "            contents.count %d\n", &byteBuf);
				o.contents.count = (char) byteBuf;
				switch (o.type) {
					case OBJ_POWERUP:
					case OBJ_HOSTAGE:
						// has vclip
						break;
					case OBJ_PLAYER:
					case OBJ_COOP:
					case OBJ_ROBOT:
					case OBJ_WEAPON:
					case OBJ_CNTRLCEN:
						// has poly model;
						break;
					}
				switch (o.controlType) {
					case :
					}
				switch (o.movementType) {
					case MT_PHYSICS:
						fscanf_s (fp.File (), "            velocity %ld %ld %ld\n", 
								  &o.physInfo.velocity.x, &o.physInfo.velocity.y, &o.physInfo.velocity.z);
						fscanf_s (fp.File (), "            thrust %ld %ld %ld\n", 
								  &o.physInfo.thrust.x, &o.physInfo.thrust.y, &o.physInfo.thrust.z);
						fscanf_s (fp.File (), "            mass %ld\n", &o.physInfo.mass);
						fscanf_s (fp.File (), "            drag %ld\n", &o.physInfo.drag);
						fscanf_s (fp.File (), "            brakes %ld\n", &o.physInfo.brakes);
						fscanf_s (fp.File (), "            rotvel %ld %ld %ld\n", 
								  &o.physInfo.rotvel.x, &o.physInfo.rotvel.y, &o.physInfo.rotvel.z);
						fscanf_s (fp.File (), "            rotthrust %ld %ld %ld\n", 
								  &o.physInfo.rotthrust.x, &o.physInfo.rotthrust.y, &o.physInfo.rotthrust.z);
						fscanf_s (fp.File (), "            turnroll %hd\n", &o.physInfo.turnroll);
						fscanf_s (fp.File (), "            flags %hd\n", &o.physInfo.flags);
						break;
					case MT_SPIN:
						fscanf_s (fp.File (), "            spinrate %ld %ld %ld\n", 
								  &o.spin_rate.x, &o.spin_rate.y, &o.spin_rate.z);
						break;
					}
				switch (o.renderType) {
					case :
						break;
					}
				}
#endif
			}
		}
	short children [6];
	fscanf_s (fp.File (), "  children %hd %hd %hd %hd %hd %hd\n", 
				 children + 0, children + 1, children + 2, children + 3, children + 4, children + 5, children + 6);
	for (i = 0; i < 6; i++)
		segP->SetChild (i, children [i]);
	// read in vertices
	for (i = 0; i < 8; i++) {
		int x, y, z;
		fscanf_s (fp.File (), "  vms_vector %hd %ld %ld %ld\n", &test, &x, &y, &z);
		if (test != i) {
			ErrorMsg ("Invalid vertex number read");
			return (0);
			}
		// each vertex relative to the origin has a x', y', and z' component
		// adjust vertices relative to origin
		vVertex.Set (x, y, z);
		v.Set (vVertex ^ xAxis, vVertex ^ xAxis, vVertex ^ xAxis);
		v += origin;
		// add a new vertex
		// if this is the same as another vertex, then use that vertex number instead
		CVertex* vertP = Vertices (origVertCount);
		for (k = origVertCount; k < VertCount (); k++, vertP++)
			if (*vertP == v) {
				segP->m_info.verts [i] = k;
				break;
				}
		// else make a new vertex
		if (k == VertCount ()) {
			nVertex = VertCount ();
			vertexManager.Status (nVertex) |= NEW_MASK;
			segP->m_info.verts [i] = nVertex;
			*Vertices (nVertex) = v;
			VertCount ()++;
			}
		}
	// mark vertices
	for (i = 0; i < 8; i++)
		vertexManager.Status (segP->m_info.verts [i]) |= MARKED_MASK;
	fscanf_s (fp.File (), "  staticLight %ld\n", &segP->m_info.staticLight);
	if (bExtBlkFmt) {
		fscanf_s (fp.File (), "  special %d\n", &byteBuf);
		segP->m_info.function = byteBuf;
		fscanf_s (fp.File (), "  nMatCen %d\n", &byteBuf);
		segP->m_info.nMatCen = byteBuf;
		fscanf_s (fp.File (), "  value %d\n", &byteBuf);
		segP->m_info.value = byteBuf;
		fscanf_s (fp.File (), "  childFlags %d\n", &byteBuf);
		segP->m_info.childFlags = byteBuf;
		fscanf_s (fp.File (), "  wallFlags %d\n", &byteBuf);
		segP->m_info.wallFlags = byteBuf;
		switch (segP->m_info.function) {
			case SEGMENT_FUNC_FUELCEN:
				if (!AddFuelCenter (nSegment, SEGMENT_FUNC_FUELCEN, false, false))
					segP->m_info.function = 0;
				break;
			case SEGMENT_FUNC_REPAIRCEN:
				if (!AddFuelCenter (nSegment, SEGMENT_FUNC_REPAIRCEN, false, false))
					segP->m_info.function = 0;
				break;
			case SEGMENT_FUNC_ROBOTMAKER:
				if (!AddRobotMaker (nSegment, false, false))
					segP->m_info.function = 0;
				break;
			case SEGMENT_FUNC_EQUIPMAKER:
				if (!AddEquipMaker (nSegment, false, false))
					segP->m_info.function = 0;
				break;
			case SEGMENT_FUNC_CONTROLCEN:
				if (!AddReactor (nSegment, false, false))
					segP->m_info.function = 0;
				break;
			default:
				break;
			}
		}
	else {
		segP->m_info.function = 0;
		segP->m_info.nMatCen = -1;
		segP->m_info.value = -1;
		}
	segP->m_info.wallFlags = MARKED_MASK; // no other bits
	// calculate childFlags
	segP->m_info.childFlags = 0;
	for (i = 0; i < MAX_SIDES_PER_SEGMENT; i++)
		if (segP->GetChild (i) >= 0)
		segP->m_info.childFlags |= (1 << i);
	SegCount ()++;
	nNewSegs++;
	}

CTrigger *trigger = Triggers (MineInfo ().triggers.count);
for (i = nNewTriggers; i; i--) {
	trigger--;
	for (j = 0; j < trigger->m_count; j++) {
		if (trigger->Segment (j) >= 0)
			trigger->Segment (j) = xlatSegNum [trigger->Segment (j)];
		else if (trigger->m_count == 1) {
			DeleteTrigger (short (trigger - Triggers (0)));
			i--;
			}
		else {
			trigger->Delete (j);
			}
		}
	}

sprintf_s (message, sizeof (message),
			" Block tool: %d blocks, %d walls, %d triggers pasted.", 
			nNewSegs, nNewWalls, nNewTriggers);
DEBUGMSG (message);
return (nNewSegs);
}

//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------

void CMine::WriteSegmentInfo (CFileManager& fp, short /*nSegment*/) 
{
	short				nSegment;
	CSegment			*segP;
	CSide				*sideP;
	CWall				*wallP;
	short				i,j;
	CVertex			origin;
	CDoubleVector	xPrime, yPrime, zPrime, vVertex;
	short				nVertex;

// set origin
segP = current.Segment ();
origin = *Vertices (segP->m_info.verts[sideVertTable[current.m_nSide][CURRENT_POINT(0)]]);
// set x'
xPrime = *Vertices (segP->m_info.verts[sideVertTable[current.m_nSide][CURRENT_POINT(1)]]) - origin;
// calculate y'
vVertex = *Vertices (segP->m_info.verts[sideVertTable[current.m_nSide][CURRENT_POINT(3)]]) - origin;
yPrime = CrossProduct (xPrime, vVertex);
zPrime = CrossProduct (xPrime, yPrime);
xPrime.Normalize ();
yPrime.Normalize ();
zPrime.Normalize ();

segP = Segments (0);
for (nSegment = 0; nSegment < SegCount (); nSegment++, segP++) {
	DLE.MainFrame ()->Progress ().StepIt ();
	if (segP->m_info.wallFlags & MARKED_MASK) {
		fprintf (fp.File (), "segment %d\n",nSegment);
		sideP = segP->m_sides;
		for (i = 0; i < MAX_SIDES_PER_SEGMENT; i++, sideP++) {
			fprintf (fp.File (), "  side %d\n",i);
			fprintf (fp.File (), "    tmap_num %d\n",sideP->m_info.nBaseTex);
			fprintf (fp.File (), "    tmap_num2 %d\n",sideP->m_info.nOvlTex);
			for (j = 0; j < 4; j++) {
				fprintf (fp.File (), "    uvls %d %d %d\n",
				sideP->m_info.uvls [j].u,
				sideP->m_info.uvls [j].v,
				sideP->m_info.uvls [j].l);
				}
			if (bExtBlkFmt) {
				fprintf (fp.File (), "    nWall %d\n", 
							(sideP->m_info.nWall < MineInfo ().walls.count) ? sideP->m_info.nWall : NO_WALL);
				if (sideP->m_info.nWall < MineInfo ().walls.count) {
					wallP = Walls (sideP->m_info.nWall);
					fprintf (fp.File (), "        segment %d\n", wallP->m_nSegment);
					fprintf (fp.File (), "        side %d\n", wallP->m_nSide);
					fprintf (fp.File (), "        hps %d\n", wallP->m_info.hps);
					fprintf (fp.File (), "        type %d\n", wallP->m_info.type);
					fprintf (fp.File (), "        flags %d\n", wallP->m_info.flags);
					fprintf (fp.File (), "        state %d\n", wallP->m_info.state);
					fprintf (fp.File (), "        nClip %d\n", wallP->m_info.nClip);
					fprintf (fp.File (), "        keys %d\n", wallP->m_info.keys);
					fprintf (fp.File (), "        cloak %d\n", wallP->m_info.cloakValue);
					if ((wallP->m_info.nTrigger < 0) || (wallP->m_info.nTrigger >= MineInfo ().triggers.count))
						fprintf (fp.File (), "        trigger %u\n", NO_TRIGGER);
					else {
						CTrigger *trigger = Triggers (wallP->m_info.nTrigger);
						int iTarget;
						int count = 0;
						// count trigger targets in marked area
						for (iTarget = 0; iTarget < trigger->m_count; iTarget++)
							if (Segments (trigger->Segment (iTarget))->m_info.wallFlags & MARKED_MASK)
								count++;
						fprintf (fp.File (), "        trigger %d\n", wallP->m_info.nTrigger);
						fprintf (fp.File (), "			    type %d\n", trigger->m_info.type);
						fprintf (fp.File (), "			    flags %ld\n", trigger->m_info.flags);
						fprintf (fp.File (), "			    value %ld\n", trigger->m_info.value);
						fprintf (fp.File (), "			    timer %d\n", trigger->m_info.time);
						fprintf (fp.File (), "			    count %d\n", count);
						for (iTarget = 0; iTarget < trigger->m_count; iTarget++)
							if (Segments (trigger->Segment (iTarget))->m_info.wallFlags & MARKED_MASK) {
								fprintf (fp.File (), "			        segP %d\n", trigger->Segment (iTarget));
								fprintf (fp.File (), "			        side %d\n", trigger->Side (iTarget));
								}
						}
					}
				}
			}
		fprintf (fp.File (), "  children");
		for (i = 0; i < 6; i++)
			fprintf (fp.File (), " %d", segP->GetChild (i));
		fprintf (fp.File (), "\n");
		// save vertices
		for (i = 0; i < 8; i++) {
			// each vertex relative to the origin has a x', y', and z' component
			// which is a constant (k) times the axis
			// k = (B*A)/(A*A) where B is the vertex relative to the origin
			//                       A is the axis unit vVertexor (always 1)
			nVertex = segP->m_info.verts [i];
			vVertex = *Vertices (nVertex) - origin;
			fprintf (fp.File (), "  vms_vector %d %ld %ld %ld\n", i, D2X (vVertex ^ xPrime), D2X (vVertex ^ yPrime), D2X (vVertex ^ zPrime));
			}
		fprintf (fp.File (), "  staticLight %ld\n",segP->m_info.staticLight);
		if (bExtBlkFmt) {
			fprintf (fp.File (), "  special %d\n",segP->m_info.function);
			fprintf (fp.File (), "  nMatCen %d\n",segP->m_info.nMatCen);
			fprintf (fp.File (), "  value %d\n",segP->m_info.value);
			fprintf (fp.File (), "  childFlags %d\n",segP->m_info.childFlags);
			fprintf (fp.File (), "  wallFlags %d\n",segP->m_info.wallFlags);
			}
		}
	}
}

//==========================================================================
// MENU - Cut
//==========================================================================

void CMine::CutBlock (void)
{
  CFileManager fp;
  short nSegment;
  short count;
  char szFile [256] = "\0";

if (m_bSplineActive) {
	ErrorMsg (spline_error_message);
	return;
	}

  // make sure some cubes are marked
count = MarkedSegmentCount ();
if (count==0) {
	ErrorMsg ("No block marked.\n\n""Use 'M' or shift left mouse button\n""to mark one or more cubes.");
	return;
	}

//  if (disable_saves) {
//    ErrorMsg ("Saves disabled, contact Interplay for your security number.");
//    return;
//  }

if (!bExpertMode && Query2Msg(BLOCKOP_HINT,MB_YESNO) != IDYES)
	return;
if (!BrowseForFile (FALSE, 
	                 bExtBlkFmt ? "blx" : "blk", szFile, 
						  "Block file|*.blk|"
						  "Extended block file|*.blx|"
						  "All Files|*.*||",
						  OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT,
						  DLE.MainFrame ()))
	return;
_strlwr_s (szFile, sizeof (szFile));
bExtBlkFmt = strstr (szFile, ".blx") != null;
if (fp.Open (szFile, "w")) {
	ErrorMsg ("Unable to open block file");
	return;
	}
//undoManager.UpdateBuffer(0);
strcpy_s (m_szBlockFile, sizeof (m_szBlockFile), szFile); // remember file for quick paste
fprintf (fp.File (), bExtBlkFmt ? "DMB_EXT_BLOCK_FILE\n" : "DMB_BLOCK_FILE\n");
DLE.MainFrame ()->InitProgress (SegCount ());
WriteSegmentInfo (fp, 0);
DLE.MainFrame ()->Progress ().DestroyWindow ();
// delete Segments () from last to first because SegCount ()
// is effected for each deletion.  When all Segments () are marked
// the SegCount () will be decremented for each nSegment in loop.
undoManager.SetModified (true);
undoManager.Lock ();
DLE.MainFrame ()->InitProgress (SegCount ());
CSegment *segP = Segments (SegCount ());
for (nSegment = SegCount () - 1; nSegment; nSegment--) {
	DLE.MainFrame ()->Progress ().StepIt ();
    if ((--segP)->m_info.wallFlags & MARKED_MASK) {
		if (SegCount () <= 1)
			break;
		DeleteSegment (nSegment); // delete segP w/o asking "are you sure"
		}
	}
DLE.MainFrame ()->Progress ().DestroyWindow ();
undoManager.Unlock ();
fp.Close ();
sprintf_s (message, sizeof (message), " Block tool: %d blocks cut to '%s' relative to current side.", count, szFile);
DEBUGMSG (message);
  // wrap back then forward to make sure segment is valid
wrap(&Current1 ().nSegment,-1,0,SegCount () - 1);
wrap(&Current2 ().nSegment,1,0,SegCount () - 1);
wrap(&Current2 ().nSegment,-1,0,SegCount () - 1);
wrap(&Current2 ().nSegment,1,0,SegCount () - 1);
SetLinesToDraw ();
DLE.MineView ()->Refresh ();
}

//==========================================================================
// MENU - Copy
//==========================================================================

void CMine::CopyBlock (char *pszBlockFile)
{
  CFileManager fp;
  char szFile [256] = "\0";
  short count = MarkedSegmentCount ();

// make sure some cubes are marked
if (count == 0) {
	ErrorMsg ("No block marked.\n\n""Use 'M' or shift left mouse button\n""to mark one or more cubes.");
	return;
	}

//  if (disable_saves) {
//    ErrorMsg ("Saves disabled, contact Interplay for your security number.");
//    return;
//  }

if (!bExpertMode && Query2Msg(BLOCKOP_HINT,MB_YESNO) != IDYES)
	return;
if (pszBlockFile && *pszBlockFile)
	strcpy_s (szFile, sizeof (szFile), pszBlockFile);
else {
	strcpy_s (szFile, sizeof (szFile), m_szBlockFile);
	if (!BrowseForFile (FALSE, 
	                 bExtBlkFmt ? "blx" : "blk", szFile, 
						  "Block file|*.blk|"
						  "Extended block file|*.blx|"
						  "All Files|*.*||",
						  OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT,
						  DLE.MainFrame ()))
		return;
	}
_strlwr_s (szFile, sizeof (szFile));
bExtBlkFmt = strstr (szFile, ".blx") != null;
if (fp.Open (szFile, "w")) {
	sprintf_s (message, sizeof (message), "Unable to open block file '%s'", szFile);
	ErrorMsg (message);
	return;
	}
//  undoManager.UpdateBuffer(0);
strcpy_s (m_szBlockFile, sizeof (m_szBlockFile), szFile); // remember fp for quick paste
fprintf (fp.File (), bExtBlkFmt ? "DMB_EXT_BLOCK_FILE\n" : "DMB_BLOCK_FILE\n");
WriteSegmentInfo (fp, 0);
fp.Close ();
sprintf_s (message, sizeof (message), " Block tool: %d blocks copied to '%s' relative to current side.", count, szFile);
DEBUGMSG (message);
SetLinesToDraw ();
DLE.MineView ()->Refresh ();
}

//==========================================================================
// MENU - Paste
//==========================================================================

void CMine::PasteBlock() 
{
if (m_bSplineActive) {
	ErrorMsg (spline_error_message);
	return;
	}
// Initialize data for fp open dialog
  char szFile [256] = "\0";

if (!BrowseForFile (TRUE, 
	                 bExtBlkFmt ? "blx" : "blk", szFile, 
						  "Block file|*.blk|"
						  "Extended block file|*.blx|"
						  "All Files|*.*||",
						  OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST,
						  DLE.MainFrame ()))
	return;
if (!ReadBlock (szFile, 0))
	DLE.MineView ()->SetSelectMode (BLOCK_MODE);
}

//==========================================================================
// read_block_file()
//
// returns 0 on success
// if option == 1, then "x blocks pasted" message is suppressed
//==========================================================================

int CMine::ReadBlock (char *pszBlockFile,int option) 
{
	CSegment *segP,*seg2;
	short nSegment,seg_offset;
	short count,child;
	short nVertex;
	CFileManager fp;

_strlwr_s (pszBlockFile, 256);
if (fp.Open (pszBlockFile, "r")) {
	ErrorMsg ("Unable to open block file");
	return 1;
	}	

fscanf_s (fp.File (), "%s\n", &message, sizeof (message));
if (!strncmp (message, "DMB_BLOCK_FILE", 14))
	bExtBlkFmt = false;
else if (!strncmp (message, "DMB_EXT_BLOCK_FILE", 18))
	bExtBlkFmt = true;
else {
	ErrorMsg ("This is not a block file.");
	fp.Close ();
	return 2;
	}

strcpy_s (m_szBlockFile, sizeof (m_szBlockFile), pszBlockFile); // remember file for quick paste

// unmark all Segments ()
// set up all seg_numbers (makes sure there are no negative seg_numbers)
undoManager.SetModified (true);
undoManager.Lock ();
DLE.MineView ()->DelayRefresh (true);
segP = Segments (0);
for (nSegment = 0; nSegment < MAX_SEGMENTS; nSegment++, segP++) {
	segP->m_info.nIndex = nSegment;
	segP->m_info.wallFlags &= ~MARKED_MASK;
	}

// unmark all vertices
for (nVertex = 0; nVertex < MAX_VERTICES; nVertex++) {
	vertexManager.Status (nVertex) &= ~MARKED_MASK;
	vertexManager.Status (nVertex) &= ~NEW_MASK;
	}

DLE.MainFrame ()->InitProgress (fp.Length ());
count = ReadSegmentInfo (fp);
DLE.MainFrame ()->Progress ().DestroyWindow ();

// int up the new Segments () children
segP = Segments (0);
for (nSegment = 0; nSegment < SegCount (); nSegment++, segP++) {
	if (segP->m_info.nIndex < 0) {  // if segment was just inserted
		// if child has a segment number that was just inserted, set it to the
		//  segment's offset number, otherwise set it to -1
		for (child = 0; child < MAX_SIDES_PER_SEGMENT; child++) {
			if (segP->m_info.childFlags & (1 << child)) {
				seg2 = Segments (0);
				for (seg_offset = 0; seg_offset < SegCount (); seg_offset++, seg2++) {
					if (segP->GetChild (child) == ~seg2->m_info.nIndex) {
						segP->SetChild (child, seg_offset);
						break;
						}
					}
				if (seg_offset == SegCount ()) { // no child found
					ResetSide (nSegment,child);
					// auto link the new segment with any touching Segments ()
					seg2 = Segments (0);
					int segnum2, sidenum2;
					for (segnum2 = 0; segnum2 < SegCount (); segnum2++, seg2++) {
						if (nSegment != segnum2) {
							// first check to see if Segments () are any where near each other
							// use x, y, and z coordinate of first point of each segment for comparison
							CVertex* v1 = Vertices (segP ->m_info.verts [0]);
							CVertex* v2 = Vertices (seg2->m_info.verts [0]);
							if (fabs (v1->v.x - v2->v.x) < 10.0 &&
								 fabs (v1->v.y - v2->v.y) < 10.0 &&
								 fabs (v1->v.z - v2->v.z) < 10.0) {
								for (sidenum2 = 0; sidenum2 < 6; sidenum2++) {
									LinkSegments (nSegment, child, segnum2, sidenum2, 3 * F1_0);
									}
								}
							}
						}
					}
				} 
			else {
				segP->SetChild (child, -1); // force child to agree with bitmask
				}
			}
		}
	}
// clear all new vertices as such
for (nVertex = 0; nVertex < MAX_VERTICES; nVertex++)
	vertexManager.Status (nVertex) &= ~NEW_MASK;
// now set all seg_numbers
segP = Segments (0);
for (nSegment = 0; nSegment < SegCount (); nSegment++, segP++)
	segP->m_info.nIndex = nSegment;
/*
if (option != 1) {
	sprintf_s (message, sizeof (message)," Block tool: %d blocks pasted.",count);
	DEBUGMSG (message);
	}
*/
fp.Close ();
//DLE.MineView ()->Refresh ();
undoManager.Unlock ();
DLE.MineView ()->DelayRefresh (false);
DLE.MineView ()->Refresh ();
return 0;
}

//==========================================================================
// MENU - Quick Paste
//==========================================================================

void CMine::QuickPasteBlock ()
{
if (!*m_szBlockFile) {
	PasteBlock ();
//	ErrorMsg ("You must first use one of the cut or paste commands\n"
//				"before you use the Quick Paste command");
	return;
	}

if (m_bSplineActive) {
	ErrorMsg (spline_error_message);
	return;
	}

//undoManager.UpdateBuffer(0);

if (!ReadBlock (m_szBlockFile, 1))
	DLE.MineView ()->SetSelectMode (BLOCK_MODE);
}

//==========================================================================
// MENU - Delete Block
//==========================================================================

void CMine::DeleteBlock (void)
{

short nSegment, count;

if (m_bSplineActive) {
	ErrorMsg (spline_error_message);
	return;
	}
// make sure some cubes are marked
count = MarkedSegmentCount ();
if (!count) {
	ErrorMsg ("No block marked.\n\n"
				"Use 'M' or shift left mouse button\n"
				"to mark one or more cubes.");
	return;
	}

undoManager.SetModified (true);
undoManager.Lock ();
DLE.MineView ()->DelayRefresh (true);

// delete Segments () from last to first because SegCount ()
// is effected for each deletion.  When all Segments () are marked
// the SegCount () will be decremented for each nSegment in loop.
if (QueryMsg ("Are you sure you want to delete the marked cubes?") != IDYES)
	return;

DLE.MainFrame ()->InitProgress (SegCount ());
for (nSegment = SegCount () - 1; nSegment >= 0; nSegment--) {
		DLE.MainFrame ()->Progress ().StepIt ();
		if (GetSegment (nSegment)->m_info.wallFlags & MARKED_MASK) {
		if (SegCount () <= 1)
			break;
		if (Objects (0)->m_info.nSegment != nSegment)
			DeleteSegment (nSegment); // delete segP w/o asking "are you sure"
		}
	}
DLE.MainFrame ()->Progress ().DestroyWindow ();
// wrap back then forward to make sure segment is valid
wrap(&Current1 ().nSegment,-1,0,SegCount () - 1);
wrap(&Current2 ().nSegment,1,0,SegCount () - 1);
wrap(&Current2 ().nSegment,-1,0,SegCount () - 1);
wrap(&Current2 ().nSegment,1,0,SegCount () - 1);
undoManager.Unlock ();
DLE.MineView ()->DelayRefresh (false);
DLE.MineView ()->Refresh ();
}

//eof block.cpp