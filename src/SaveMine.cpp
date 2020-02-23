// Copyright (c) 1998 Bryan Aamot, Brainware
#include "mine.h"
#include "dle-xp.h"
#include "quaternion.h"

#include "rapidjson\document.h"
#include "rapidjson\filewritestream.h"
#include "rapidjson\pointer.h"
#include "rapidjson\prettywriter.h"

// -----------------------------------------------------------------------------

short CMine::Save (const char * szFile)
{
if (segmentManager.Overflow ()) {
	if (vertexManager.Overflow ()) 
		ErrorMsg ("Error: Too many segments and vertices for this level version!\nThe level cannot be saved.");
	else
		ErrorMsg ("Error: Too many segments for this level version!\nThe level cannot be saved.");
	return 0;
	}
else if (vertexManager.Overflow ()) {
	ErrorMsg ("Error: Too many vertices for this level version!\nThe level cannot be saved.");
	return 0;
	}

	CFileManager	fp;
	char				filename [256];
	int				mineDataOffset, gameDataOffset, hostageTextOffset;
	int				mineErr, gameErr;

strcpy_s (filename, sizeof (filename), szFile);
if (!fp.Open (filename, "w+b"))
	return 0;

m_changesMade = 0;
// write file signature
fp.WriteInt32 ('P'*0x1000000L + 'L'*0x10000L + 'V'*0x100 + 'L'); // signature

// always save as version 7 or greater if its a D2 level
// otherwise, blinking lights will not work.
if (LevelVersion () < 7 && IsD2File ()) {
	SetLevelVersion (7);
	}
if (IsD2XLevel () && LevelIsOutdated ()) {
	UpdateLevelVersion ();
	segmentManager.UpdateWalls (MAX_WALLS_D2 + 1, WALL_LIMIT);
	}

// write version
fp.WriteInt32 (LevelVersion ());
fp.WriteInt32 (0); // mineDataOffset (temporary)
fp.WriteInt32 (0); // gameDataOffset (temporary)

if (IsD1File ())
	fp.WriteInt32 (0); // hostageTextOffset (temporary)
else if (IsD2File ()) {
	if (LevelVersion () >= 8) {
		fp.WriteInt16 (rand ());
		fp.WriteInt16 (rand ());
		fp.WriteInt16 (rand ());
		fp.WriteSByte ((sbyte) rand ());
		}
	// save palette name
	char *name = strrchr (descentFolder [1], '\\');
	if (!name) 
		name = descentFolder [1]; // point to 1st char if no slash found
	else
		name++;               // point to character after slash
	char paletteName [15];
	strncpy_s (paletteName, sizeof (paletteName), name, 12);
	paletteName [13] = null;  // null terminate just in case
	// replace extension with *.256
	if (strlen (paletteName) > 4)
		strcpy_s (&paletteName [strlen (paletteName) - 4], 5, ".256");
	else
		strcpy_s (paletteName, sizeof (paletteName), "groupa.256");
	//_strupr_s (paletteName, sizeof (paletteName));
	strcat_s (paletteName, sizeof (paletteName), "\n"); // add a return to the end
	fp.Write (paletteName, (int) strlen (paletteName), 1);
	// write reactor info
	fp.Write (ReactorTime ());
	fp.Write (ReactorStrength ());
	// variable light new for version 7
	lightManager.WriteVariableLights (&fp);
	// write secret segment number
	fp.Write (SecretSegment ());
	// write secret return segment orientation
	fp.Write (SecretOrient ());
	}
// save mine data
mineDataOffset = fp.Tell ();
if (0 > (mineErr = SaveMineGeometry (&fp))) {
	fp.Close ();
	ErrorMsg ("Error saving mine data");
	return -2;
	}

// save game data
gameDataOffset = fp.Tell ();
if (0 > (gameErr = SaveGameItems (&fp))) {
	fp.Close ();
	ErrorMsg ("Error saving game data");
	return -3;
	}

// save hostage data
hostageTextOffset = fp.Tell ();
// leave hostage text empty

// now and go back to beginning of file and save offsets
fp.Seek (2 * sizeof (int), SEEK_SET);
fp.Write (mineDataOffset);    // mineDataOffset
fp.Write (gameDataOffset);    // gameDataOffset
if (m_fileType == RDL_FILE) 
	fp.Write (hostageTextOffset); // hostageTextOffset
fp.Seek (0, SEEK_END);
fp.Close ();
return 1;
}

// ------------------------------------------------------------------------
// SaveMineGeometry()
//
// ACTION - Writes a mine data portion of RDL file.
// ------------------------------------------------------------------------

short CMine::SaveMineGeometry (CFileManager* fp)
{
// write version (1 ubyte)
fp->WriteByte (COMPILED_MINE_VERSION);
// write no. of vertices (2 bytes)
fp->WriteUInt16 (vertexManager.Count ());
// write number of Segments () (2 bytes)
fp->WriteInt16 (segmentManager.Count ());
// write all vertices
vertexManager.SetIndex ();
segmentManager.SetIndex ();
triggerManager.SetIndex ();
wallManager.SetIndex ();

vertexManager.Write (fp);
// write segment information
segmentManager.WriteSegments (fp);
// for Descent 2, save special info here
if (LevelVersion () >= 9)
	lightManager.WriteColors (*fp);
return 1;
}

// ------------------------------------------------------------------------
// SaveGameItems()
//
//  ACTION - Saves the player, object, wall, door, trigger, and
//           material generator data from an RDL file.
// ------------------------------------------------------------------------

short CMine::SaveGameItems (CFileManager* fp)
{
if (IsD1File ()) {
	Info ().fileInfo.signature = 0x6705;
	Info ().fileInfo.version = 25;
	Info ().fileInfo.size = 119;
	Info ().level = 0;
	}
else {
	Info ().fileInfo.signature = 0x6705;
	Info ().fileInfo.version = (LevelVersion () < 13) ? 31 : 40;
	Info ().fileInfo.size = (LevelVersion () < 13) ? 143 : -1; // same as sizeof (Info ())
	Info ().level = 0;
	}

int startOffset = fp->Tell ();
Info ().Write (fp, IsD2XLevel ());
if (Info ().fileInfo.version >= 14) // save mine file name
	fp->Write (m_currentLevelName, sizeof (char), (int) strlen (m_currentLevelName));
if (IsD2File ())
	fp->Write ("\n", 1, 1); // write an end - of - line
else
	fp->Write ("", 1, 1);   // write a null

// write pof names from resource file
short	nSavePofNames;

if (IsD2File ()) {
	nSavePofNames = 166;
	fp->WriteUInt16 (166);   // write # of POF names
	}
else {
	nSavePofNames = 78;
	fp->WriteInt16 (25);	// Don't know exactly what this value is for or why it is 25?
	}

CResource res;
ubyte* savePofNames = res.Load (IsD1File () ? IDR_POF_NAMES1 : IDR_POF_NAMES2);
if (savePofNames == null)
	return 0;
fp->Write (savePofNames, nSavePofNames, 13); // 13 characters each

Info ().player.offset = fp->Tell ();
char* str = "Made with Descent Level Editor\0\0\0\0\0\0\0";
fp->Write (str, (int) strlen (str) + 1, 1);

segmentManager.RenumberProducers ();
objectManager.Write (fp);
wallManager.Write (fp);
triggerManager.Write (fp);
triggerManager.WriteReactor (fp);
segmentManager.WriteRobotMakers (fp);
if (IsD2File ()) {
	segmentManager.WriteEquipMakers (fp);
	lightManager.WriteLightDeltas (fp);
	}

fp->Seek (startOffset, SEEK_SET);
Info ().Write (fp, IsD2XLevel ());
fp->Seek (0, SEEK_END);
return 1;
}

// -----------------------------------------------------------------------------

using namespace rapidjson;

short CMine::ExportOverload (const char * filename)
{
	CFileManager fp;

if (!fp.Open (filename, "w+b"))
	return 0;
else {
	Document document;
	document.SetObject ();
	auto& allocator = document.GetAllocator ();
	char path [128] = { 0 };
	int numEntities = 0;
	CDynamicArray <uint> doorWallIds;

	document.AddMember ("properties", Value ().SetObject (), allocator);
	document.AddMember ("global_data", Value ().SetObject (), allocator);
	document.AddMember ("challenge_data_string", "", allocator);
	document.AddMember ("verts", Value ().SetObject (), allocator);
	document.AddMember ("segments", Value ().SetObject (), allocator);
	document.AddMember ("entities", Value ().SetObject (), allocator);

	// Level properties
	Pointer ("/properties/next_segment").Set (document, segmentManager.Count ());
	Pointer ("/properties/next_vertex").Set (document, vertexManager.Count ());
	Pointer ("/properties/next_entity").Set (document, 0);
	Pointer ("/properties/selected_segment").Set (document, 0);
	Pointer ("/properties/selected_side").Set (document, 1);
	Pointer ("/properties/selected_vertex").Set (document, 0);
	Pointer ("/properties/selected_entity").Set (document, -1);
	Pointer ("/properties/num_segments").Set (document, segmentManager.Count ());
	Pointer ("/properties/num_vertices").Set (document, vertexManager.Count ());
	Pointer ("/properties/num_entities").Set (document, 0);
	Pointer ("/properties/num_marked_segments").Set (document, 0);
	Pointer ("/properties/num_marked_sides").Set (document, 0);
	Pointer ("/properties/num_marked_vertices").Set (document, 0);
	Pointer ("/properties/num_marked_entities").Set (document, 0);
	Pointer ("/properties/texture_set").Set (document, "Titan - Bronze");

	// Global data - just use default settings
	Pointer ("/global_data/grid_size").Set (document, 8);
	Pointer ("/global_data/pre_smooth").Set (document, 3);
	Pointer ("/global_data/post_smooth").Set (document, 0);
	Pointer ("/global_data/simplify_strength").Set (document, 0.0);
	Pointer ("/global_data/deform_presets0").Set (document, "PLAIN_NOISE");
	Pointer ("/global_data/deform_presets1").Set (document, "NONE");
	Pointer ("/global_data/deform_presets2").Set (document, "NONE");
	Pointer ("/global_data/deform_presets3").Set (document, "NONE");

	// Vertices
	for (int nVertex = 0; nVertex < vertexManager.Count (); nVertex++) {
		CVertex& vertex = vertexManager [nVertex];
		sprintf_s (path, "/verts/%d/marked", nVertex);
		Pointer (path).Set (document, false);
		sprintf_s (path, "/verts/%d/x", nVertex);
		Pointer (path).Set (document, vertex.v.x / 5);
		sprintf_s (path, "/verts/%d/y", nVertex);
		Pointer (path).Set (document, vertex.v.y / 5);
		sprintf_s (path, "/verts/%d/z", nVertex);
		Pointer (path).Set (document, vertex.v.z / 5);
		}

	// Segments
	for (int nSegment = 0; nSegment < segmentManager.Count (); nSegment++) {
		CSegment* pSegment = segmentManager.Segment (nSegment);
		sprintf_s (path, "/segments/%d/marked", nSegment);
		Pointer (path).Set (document, false);
		sprintf_s (path, "/segments/%d/pathfinding", nSegment);
		Pointer (path).Set (document, "All");
		sprintf_s (path, "/segments/%d/exitsegment", nSegment);
		Pointer (path).Set (document, "None");
		sprintf_s (path, "/segments/%d/dark", nSegment);
		Pointer (path).Set (document, false);

		// Vertex IDs
		sprintf_s (path, "/segments/%d/verts", nSegment);
		Value& verts = Pointer (path).Create (document).SetArray ();
		for (int nVert = 0; nVert < MAX_VERTICES_PER_SEGMENT; nVert++)
			verts.PushBack (pSegment->VertexId (nVert), allocator);

		// Sides
		sprintf_s (path, "/segments/%d/sides", nSegment);
		Value& sides = Pointer (path).Create (document).SetArray ();
		for (int nSide = 0; nSide < MAX_SIDES_PER_SEGMENT; nSide++) {
			CSide *pSide = pSegment->Side (nSide);
			Value side (kObjectType);
			Pointer ("/marked").Set (side, false, allocator);
			Pointer ("/chunk_plane_order").Set (side, -1, allocator);
			Pointer ("/tex_name").Set (side, textureManager.Textures (pSide->BaseTex ())->Name (), allocator);
			Pointer ("/deformation_preset").Set (side, 0, allocator);
			Pointer ("/deformation_height").Set (side, 0.0, allocator);

			Value& sideVerts = Pointer ("/verts").Create (side, allocator).SetArray ();
			Value& sideUVs = Pointer ("/uvs").Create (side, allocator).SetArray ();
			for (int nSideVert = 0; nSideVert < pSide->VertexCount (); nSideVert++) {
				sideVerts.PushBack (pSegment->VertexId (nSide, nSideVert), allocator);
				Value uv (kObjectType);
				uv.AddMember ("u", pSide->Uvls (nSideVert)->u, allocator);
				uv.AddMember ("v", pSide->Uvls (nSideVert)->v, allocator);
				sideUVs.PushBack (uv, allocator);
				}

			Value& decals = Pointer ("/decals").Create (side, allocator).SetArray ();
			// Fill out basic decal data...
			Value decal (kObjectType);
			Pointer ("/mesh_name").Set (decal, "", allocator);
			Pointer ("/align").Set (decal, "CENTER", allocator);
			Pointer ("/mirror").Set (decal, "OFF", allocator);
			Pointer ("/rotation").Set (decal, 0, allocator);
			Pointer ("/repeat_u").Set (decal, 1, allocator);
			Pointer ("/repeat_v").Set (decal, 1, allocator);
			Pointer ("/offset_u").Set (decal, 0, allocator);
			Pointer ("/offset_v").Set (decal, 0, allocator);
			Pointer ("/hidden").Set (decal, false, allocator);
			Value& clips = Pointer ("/clips").Create (decal, allocator).SetArray ();
			clips.PushBack ("NONE", allocator);
			clips.PushBack ("NONE", allocator);
			clips.PushBack ("NONE", allocator);
			clips.PushBack ("NONE", allocator);
			Value& caps = Pointer ("/caps").Create (decal, allocator).SetArray ();
			caps.PushBack ("NONE", allocator);
			caps.PushBack ("NONE", allocator);
			caps.PushBack ("NONE", allocator);
			caps.PushBack ("NONE", allocator);

			// Two decal slots per side. Set up the first one
			if (pSegment->ChildId (nSide) == -1) {
				// This side is not connected and is therefore a solid surface.
				// Use the secondary texture (if any) as a decal.
				if (pSide->OvlTex (0)) {
					Pointer ("/mesh_name").Set (decal, textureManager.Textures (pSide->OvlTex (0))->Name (), allocator);
					Pointer ("/rotation").Set (decal, pSide->OvlAlignment () * 2, allocator);
					}
				}
			else if (pSegment->ChildId (nSide) >= 0 && pSide->Wall () &&
				(pSide->Wall ()->Type () == WALL_NORMAL || pSide->Wall ()->Type () == WALL_CLOSED)) {
				// This is a solid wall. Use the primary texture for the first decal.
				Pointer ("/mesh_name").Set (decal, textureManager.Textures (pSide->BaseTex ())->Name (), allocator);
				}
			decals.PushBack (Value ().CopyFrom (decal, allocator), allocator);

			// Second decal. We only use this for the secondary texture of walls
			if (pSegment->ChildId (nSide) >= 0 && pSide->Wall () &&
				(pSide->Wall ()->Type () == WALL_NORMAL || pSide->Wall ()->Type () == WALL_CLOSED) &&
				pSide->OvlTex (0)) {
				// This is a solid wall. Use the secondary texture for the second decal.
				Pointer ("/mesh_name").Set (decal, textureManager.Textures (pSide->OvlTex (0))->Name (), allocator);
				Pointer ("/rotation").Set (decal, pSide->OvlAlignment () * 2, allocator);
				}
			else {
				// Blank out fields for the second decal
				Pointer ("/mesh_name").Set (decal, "", allocator);
				Pointer ("/rotation").Set (decal, 0, allocator);
				}
			decals.PushBack (decal, allocator);

			int doorNum = -1;
			if (pSide->Wall () && pSide->Wall ()->IsDoor ()) {
				CWall* pWall = pSide->Wall ();
				CWall* pOppositeWall = wallManager.OppositeWall (*pWall);

				// Check that we haven't already added this door from the other side
				if (!pOppositeWall || doorWallIds.Find (wallManager.Index (pOppositeWall)) == -1) {
					// In Overload levels, doors are entities, and the /door field contains the entity number.
					// We don't have a corresponding entity in the Descent level so we'll have to create one.
					doorNum = numEntities;
					numEntities++;
					doorWallIds.Resize (doorWallIds.Length () + 1);
					*doorWallIds.End () = wallManager.Index (pWall);
					}
				}
			Pointer ("/door").Set (side, doorNum, allocator);
			sides.PushBack (side, allocator);
			}

		// Neighboring segments
		sprintf_s (path, "/segments/%d/neighbors", nSegment);
		Value& neighbors = Pointer (path).Create (document).SetArray ();
		for (int nSide = 0; nSide < MAX_SIDES_PER_SEGMENT; nSide++) {
			short nChildId = pSegment->ChildId (nSide);
			if (nChildId == -2)
				nChildId = -1; // -2 is not supported by the Overload format
			neighbors.PushBack (nChildId, allocator);
			}
		}

	// Entities
	// First write the doors, since we already pointed segment data to them
	for (size_t nDoor = 0; nDoor < doorWallIds.Length (); nDoor++) {
		CWall* pWall = wallManager.Wall (doorWallIds [nDoor]);

		// We have to generate a GUID for each entity
		GUID guid;
		CoCreateGuid (&guid);
		char guidString [GUIDSTRING_MAX];
		sprintf_s (guidString, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
			guid.Data1, guid.Data2, guid.Data3, guid.Data4 [0], guid.Data4 [1],
			guid.Data4 [2], guid.Data4 [3], guid.Data4 [4], guid.Data4 [5], guid.Data4 [6], guid.Data4 [7]);
		sprintf_s (path, "/entities/%d/guid", nDoor);
		Pointer (path).Set (document, guidString);

		sprintf_s (path, "/entities/%d/ref_guid", nDoor);
		Pointer (path).Set (document, "00000000-0000-0000-0000-000000000000");

		// Position
		sprintf_s (path, "/entities/%d/position", nDoor);
		Value& position = Pointer (path).Create (document).SetArray ();
		for (int i = 0; i < 3; i++) {
			segmentManager.Segment (pWall->m_nSegment)->ComputeCenter (pWall->Side ());
			position.PushBack (pWall->Side ()->Center () [i] / 5, allocator);
			}

		// Rotation (4x4 matrix; DLE uses 3x3 so we have to pad it)
		// CTunnelBase happens to calculate the orientation of a side already, so we're repurposing that
		sprintf_s (path, "/entities/%d/rotation", nDoor);
		Value& rotation = Pointer (path).Create (document).SetArray ();
		CTunnelBase sideOrientation;
		CSelection wallSide (*pWall);
		sideOrientation.Setup (&wallSide, -1.0, true);
		for (size_t i = 0; i < 4; i++)
			for (size_t j = 0; j < 4; j++) {
				if (i == 3 && j == 3)
					rotation.PushBack (1.0f, allocator);
				else if (i == 3 || j == 3)
					rotation.PushBack (0.0f, allocator);
				else
					rotation.PushBack (sideOrientation.m_rotation [i] [j], allocator);
				}

		// ...and the other stuff
		sprintf_s (path, "/entities/%d/segnum", nDoor);
		Pointer (path).Set (document, pWall->m_nSegment, allocator);
		sprintf_s (path, "/entities/%d/type", nDoor);
		Pointer (path).Set (document, "DOOR", allocator);
		sprintf_s (path, "/entities/%d/sub_type", nDoor);
		Pointer (path).Set (document, "OM1", allocator); // just a reasonable default - can't remap
		sprintf_s (path, "/entities/%d/mp_team", nDoor);
		Pointer (path).Set (document, 0, allocator);
		sprintf_s (path, "/entities/%d/properties/door_lock", nDoor);
		Pointer (path).Set (document, (pWall->Info ().flags & WALL_DOOR_LOCKED) ? 1 : 0, allocator);
		sprintf_s (path, "/entities/%d/properties/robot_access", nDoor);
		Pointer (path).Set (document, "False", allocator);
		sprintf_s (path, "/entities/%d/properties/no_chunk", nDoor);
		Pointer (path).Set (document, "False", allocator);
		}

	int nEntity = doorWallIds.Length ();

	// Now write anything else applicable
	for (int nObject = 0; nObject < objectManager.Count (); nObject++) {
		CGameObject* pObject = objectManager.Object (nObject);

		switch (pObject->Type ()) {
			case OBJ_ROBOT:
			case OBJ_HOSTAGE:
			case OBJ_PLAYER:
			case OBJ_POWERUP:
			case OBJ_REACTOR:
				break;

			default:
				// Not supported in Overload levels
				continue;
			}

		// GUIDs
		GUID guid;
		CoCreateGuid (&guid);
		char guidString [GUIDSTRING_MAX];
		sprintf_s (guidString, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
			guid.Data1, guid.Data2, guid.Data3, guid.Data4 [0], guid.Data4 [1],
			guid.Data4 [2], guid.Data4 [3], guid.Data4 [4], guid.Data4 [5], guid.Data4 [6], guid.Data4 [7]);
		sprintf_s (path, "/entities/%d/guid", nEntity);
		Pointer (path).Set (document, guidString);

		sprintf_s (path, "/entities/%d/ref_guid", nEntity);
		Pointer (path).Set (document, "00000000-0000-0000-0000-000000000000");

		// Position
		sprintf_s (path, "/entities/%d/position", nEntity);
		Value& position = Pointer (path).Create (document).SetArray ();
		for (int i = 0; i < 3; i++)
			position.PushBack (pObject->Position () [i] / 5, allocator);

		// Rotation (4x4 matrix; DLE uses 3x3 so we have to pad it)
		sprintf_s (path, "/entities/%d/rotation", nEntity);
		Value& rotation = Pointer (path).Create (document).SetArray ();
		for (size_t i = 0; i < 4; i++)
			for (size_t j = 0; j < 4; j++) {
				if (i == 3 && j == 3)
					rotation.PushBack (1.0f, allocator);
				else if (i == 3 || j == 3)
					rotation.PushBack (0.0f, allocator);
				else
					rotation.PushBack (pObject->Orient () [i] [j], allocator);
				}

		// Shared properties
		sprintf_s (path, "/entities/%d/segnum", nEntity);
		Pointer (path).Set (document, pObject->Info ().nSegment, allocator);
		sprintf_s (path, "/entities/%d/mp_team", nEntity);
		Pointer (path).Set (document, 0, allocator);

		// Type-specific properties
		switch (pObject->Type ()) {
			case OBJ_ROBOT:
				sprintf_s (path, "/entities/%d/type", nEntity);
				Pointer (path).Set (document, "ENEMY", allocator);
				sprintf_s (path, "/entities/%d/sub_type", nEntity);
				Pointer (path).Set (document, "GRUNTA", allocator);
				break;

			case OBJ_HOSTAGE:
				sprintf_s (path, "/entities/%d/type", nEntity);
				Pointer (path).Set (document, "PROP", allocator);
				sprintf_s (path, "/entities/%d/sub_type", nEntity);
				Pointer (path).Set (document, "CRYOTUBE", allocator);
				break;

			case OBJ_PLAYER:
				sprintf_s (path, "/entities/%d/type", nEntity);
				Pointer (path).Set (document, "SPECIAL", allocator);
				sprintf_s (path, "/entities/%d/sub_type", nEntity);
				Pointer (path).Set (document, "PLAYER_START", allocator);
				break;

			case OBJ_POWERUP:
				sprintf_s (path, "/entities/%d/type", nEntity);
				Pointer (path).Set (document, "ITEM", allocator);
				sprintf_s (path, "/entities/%d/sub_type", nEntity);
				Pointer (path).Set (document, "CM_SPAWN", allocator);
				break;

			case OBJ_REACTOR:
				sprintf_s (path, "/entities/%d/type", nEntity);
				Pointer (path).Set (document, "PROP", allocator);
				sprintf_s (path, "/entities/%d/sub_type", nEntity);
				Pointer (path).Set (document, "REACTOR_OM", allocator);
				break;
			}

		nEntity++;
		}

	char writeBuffer [65536] = { 0 };
	FileWriteStream stream (fp.File (), writeBuffer, sizeof (writeBuffer));
	PrettyWriter <FileWriteStream> writer (stream);
	writer.SetIndent (' ', 2);
	document.Accept (writer);
	}

fp.Close ();
char msg [MAX_PATH + 30];
sprintf_s (msg, _countof (msg), "Level exported to %s", filename);
INFOMSG (msg);
return 1;
}

// --------------------------------------------------------------------------
//eof mine.cpp