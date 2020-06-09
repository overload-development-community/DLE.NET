// Copyright (c) 1998 Bryan Aamot, Brainware

#include "stdafx.h"
#include "MemoryFile.h"
#include "mine.h"
#include "HogManager.h"
#include "LightManager.h"
#include "PaletteManager.h"
#include "RobotManager.h"
#include "TextureManager.h"
#include "TunnelMaker.h"
#include "VertexManager.h"
#include "WallManager.h"

// -----------------------------------------------------------------------------

#undef szUndoId
#define szUndoId "CMine::LoadLevel"

short CMine::LoadLevel (CFileManager* fp, bool bLoadFromHog)
{
	CMemoryFile mf;
	CFileManager df;
	bool bCreate = false;

if (fp == null) {
	if (CreateNewLevel (mf)) 
		fp = &mf;
	else {
		CFileManager::SplitPath (IsD1File () ? g_data.GetD1Path() : g_data.GetMissionFolder(), m_startFolder, null, null);
		char filename [256];
		sprintf_s (filename, sizeof (filename), IsD1File () ? "%new.rdl" : "%snew.rl2", m_startFolder);
		if (!df.Open (filename, "rb")) {
			sprintf_s (message, sizeof (message),  "Error %d: Can't open file \"%s\".", GetLastError (), filename);
			g_data.Trace(Error, message);
			return -1;
			}
		fp = &df;
		bLoadFromHog = false;
		bCreate = true;
		}
	}
undoManager.Lock (szUndoId);
LoadMine (fp, bLoadFromHog, bCreate);
undoManager.Unlock (szUndoId);
return bCreate ? 1 : 0;
}

// -----------------------------------------------------------------------------

void CMine::Backup (const char* filename)
{
	CFileManager	fp;
	char				pszBackup1 [256], pszBackup2 [256];

strcpy_s (pszBackup1, filename);
char* ps = strrchr (pszBackup1, '.');
if (ps)
	*ps = '\0';
strcpy_s (pszBackup2, pszBackup1);
strcat_s (pszBackup1, ".bak.1");
strcat_s (pszBackup2, ".bak.2");
if (fp.Exist (pszBackup1)) {
	if (fp.Exist (pszBackup2)) 
		fp.Delete (pszBackup2);
	fp.Rename (pszBackup1, pszBackup2);
	}
fp.Copy (filename, pszBackup1);
}

// -----------------------------------------------------------------------------

short CMine::Load (const char* filename)
{
CMemoryFile	fp;
return fp.Open (filename, "rb") ? Load (&fp, false) : Load (null, false);
}

// -----------------------------------------------------------------------------
  
short CMine::Load (CFileManager* fp, bool bLoadFromHog)
{
	bool bCreate = false;

g_data.ResetMineViewDistance();
g_data.RefreshToolView();
undoManager.Reset ();
tunnelMaker.Destroy ();
segmentManager.InitFogInfo ();
// if no file passed, define a new level w/ 1 object
short i = LoadLevel (fp, bLoadFromHog);
if (i != 0)
	return i < 1;

if (LevelIsOutdated ()) {
	undoManager.Lock (szUndoId);
	if (LevelVersion () < 15) {
		segmentManager.UpdateWalls (MAX_WALLS_D2 + 1, WALL_LIMIT);
		triggerManager.ObjTriggerCount () = 0;
		}
	UpdateLevelVersion ();
	undoManager.Unlock (szUndoId);
	}

int errFlags = FixIndexValues ();
if (errFlags == 0)
	return 1;
sprintf_s(message, sizeof(message), "File contains corrupted data (error code %#04x).", errFlags);
g_data.Trace(Error, message);
return LoadLevel(null, false) < 1;
}

// -----------------------------------------------------------------------------

short CMine::LoadMineSigAndType (CFileManager* fp)
{
int sig = fp->ReadInt32 ();
if (sig != 'P'*0x1000000L + 'L'*0x10000L + 'V'*0x100 + 'L') {
	g_data.Trace(Error, "Signature value incorrect.");
	fp->Close ();
	return 1;
	}

// read version
SetLevelVersion (fp->ReadInt32 ());
if (LevelVersion () == 1) {
	SetFileType (RDL_FILE);
	}
else if ((LevelVersion () >= 6L) && (LevelVersion () <= LEVEL_VERSION)) {
	SetFileType (RL2_FILE);
	}
else {
	sprintf_s (message, sizeof (message),  "Version %d unknown. Cannot load this level.", LevelVersion ());
	g_data.Trace(Error, message);
	fp->Close ();
	return 1;
	}
return 0;
}

// -----------------------------------------------------------------------------

void CMine::LoadPaletteName (CFileManager* fp, bool bCreate)
{
	if (IsD2File()) {
		if (LevelVersion() >= 8) {
			fp->ReadInt16();
			fp->ReadInt16();
			fp->ReadInt16();
			fp->ReadSByte();
		}
		// read palette file name
		paletteManager.LoadName(fp);
		// try to find new pig file in same directory as Current () pig file
		// 1) cut off old name
		if (!bCreate) {
			if (strlen(g_data.GetD2Path()) > 0)
			{
				std::string path(g_data.GetD2Path());
				size_t pos = path.rfind('\\');
				if (pos != std::string::npos)
				{
					path = path.substr(0, pos + 1);
				}
				else
				{
					path = std::string("");
				}
				path += paletteManager.Name();
				std::transform(path.begin(), path.end(), path.begin(), ::tolower);

				g_data.SetD2Path(path.c_str());
			}
		}
	}
	else
		paletteManager.SetName("descent.pig");
}

// -----------------------------------------------------------------------------

short CMine::LoadMine (CFileManager* fp, bool bLoadFromHog, bool bCreate)
{
m_changesMade = 0;

//	CFileManager fp;
//if (fp->Open (filename, "rb")) {
//	sprintf_s (message, sizeof (message),  "Error %d: Can't open file \"%s\".", GetLastError (), filename);
//	g_data.Trace(Error, message);
//	return -1;
//	}

if (LoadMineSigAndType (fp))
	return -1;
textureManager.Reload (textureManager.Version (), false);
// If we are loading from .HOG, the light manager will have already been initialized
if (!bLoadFromHog)
	lightManager.CreateLightMap ();
ClearMineData ();
// read mine data offset
int mineDataOffset = fp->ReadInt32 ();
// read game data offset
int gameDataOffset = fp->ReadInt32 ();
LoadPaletteName (fp, bCreate);

// read descent 2 reactor information
if (IsD2File ()) {
	ReactorTime () = fp->ReadInt32 (); // base control center explosion time
	ReactorStrength () = fp->ReadInt32 (); // reactor strength
	lightManager.ReadVariableLights (fp);
	// read secret segment number
	SecretSegment () = fp->ReadInt32 ();
	// read secret segment orientation?
	fp->Read (SecretOrient ());
	}

fp->Seek (mineDataOffset, SEEK_SET);
if (LoadMineGeometry (fp, bCreate) != 0) {
	g_data.Trace(Error, "Error loading mine data");
	fp->Close ();
	return(2);
	}

fp->Seek (gameDataOffset, SEEK_SET);
if (LoadGameItems (fp, bCreate) != 0) {
	g_data.Trace(Error, "Error loading game data");
	// reset "howmany"
	objectManager.ResetInfo ();
	wallManager.ResetInfo ();
	triggerManager.ResetInfo ();
	segmentManager.ResetInfo ();
	lightManager.ResetInfo ();
	fp->Close ();
	return 3;
	}

if (!(bLoadFromHog || bCreate)) {
	// try to load the Descent 2 textures for Descent 1 levels as well to allow rendering of textured ship and robot models and of powerup sprites
	if (!textureManager.Version ())
		textureManager.Reload (1, true); 
	paletteManager.Reload (paletteManager.Name ());
	textureManager.Reload (textureManager.Version (), true);
	if (IsD2File ()) {
		char filename [256];
		strcpy_s (filename, sizeof (filename), fp->Name ());
		char* ps = strstr (filename, ".");
		if (ps)
			strcpy_s (ps, 256 - (ps - filename), ".pog");
		else
			strcat_s (filename, 256, ".pog");
		if (fp->Open (filename, "rb")) {
			textureManager.ReadPog (*fp, fp->Size ());
			fp->Close ();
			}
		robotManager.ReadHAM (null);
		robotManager.ClearHXMData ();
		if (IsD2File ()) {
			char szHogFile [256], szHamFile [256], *p;
			long nSize, nOffset;
			CFileManager hfp;
			CLevelHeader lh;

			CFileManager::SplitPath (g_data.GetD2Path(), szHogFile, null, null);
			if (p = strstr (szHogFile, "data"))
				*p = '\0';
			strcat_s (szHogFile, sizeof (szHogFile), "missions\\d2x.hog");
			if (FindFileData (szHogFile, "d2x.ham", lh, nSize, nOffset, FALSE)) {
				CFileManager::SplitPath (g_data.GetD2Path(), szHamFile, null, null);
				if (p = strstr (szHamFile, "data"))
					*p = '\0';
				strcat_s (szHamFile, sizeof (szHamFile), "missions\\d2x.ham");
				if (!hfp.Open (szHogFile, "rb"))
					g_data.Trace(Error, "Could not open HOG file.");
				else {
					if (0 < hfp.Seek (nOffset + lh.Size (), SEEK_SET))
						m_bVertigo = robotManager.ReadHAM (&hfp, EXTENDED_HAM) != 0;
					hfp.Close ();
					}
				}
			}
		ps = strstr (filename, ".");
		if (ps)
			strcpy_s (filename, 256 - (ps - filename), ".hxm");
		else
			strcat_s (filename, 256, ".hxm");
		if (fp->Open (filename, "rb")) {
			robotManager.ReadHXM (*fp, -1);
			fp->Close ();
			}
		}
	else {
		// Load DTX patch for D1 levels if there is any
		char filename [256];
		strcpy_s (filename, sizeof (filename), fp->Name ());
		char* ps = strstr (filename, ".");
		if (ps)
			strcpy_s (ps, 256 - (ps - filename), ".dtx");
		else
			strcat_s (filename, 256, ".dtx");
		if (fp->Open (filename, "rb")) {
			textureManager.ReadDtx (*fp, fp->Size ());
			fp->Close ();
			}
		}
	}
objectManager.Sort ();
g_data.ResetSelections();
return 0;
}

// -----------------------------------------------------------------------------
// LoadMineGeometry()
//
// ACTION - Reads a mine data portion of RDL file.
// -----------------------------------------------------------------------------

short CMine::LoadMineGeometry (CFileManager* fp, bool bCreate)
{
// read version (1 ubyte)
ubyte version = fp->ReadUByte ();

// read number of vertices (2 bytes)
ushort nVertices = fp->ReadUInt16 ();
int nOverflow = vertexManager.Overflow (nVertices);
if (nOverflow > 0)
	g_data.Trace(Error, "Warning: Too many vertices for this level version");
else if (nOverflow < 0) {
	sprintf_s (message, sizeof (message),  "Too many vertices (%d)", nVertices);
	g_data.Trace(Error, message);
	return 1;
	}

// read number of Segments () (2 bytes)
ushort nSegments = fp->ReadUInt16 ();
nOverflow = segmentManager.Overflow (nSegments);
if (nOverflow > 0)
	g_data.Trace(Error, "Warning: Too many segments for this level version");
else if (nOverflow < 0) {
	sprintf_s (message, sizeof (message), "Too many segments (%d)", nSegments);
	g_data.Trace(Error, message);
	return 2;
	}

objectManager.ResetInfo ();
wallManager.ResetInfo ();
triggerManager.ResetInfo ();
segmentManager.ResetInfo ();
lightManager.ResetInfo ();

vertexManager.Count () = nVertices;
vertexManager.FileOffset () = fp->Tell ();
vertexManager.Read (fp);

segmentManager.Count () = nSegments;
segmentManager.FileOffset () = fp->Tell ();
segmentManager.ReadSegments (fp);

lightManager.ReadColors (*fp);

if (objectManager.Count () > MAX_OBJECTS) {
	sprintf_s (message, sizeof (message),  "Warning: Max number of objects for this level version exceeded (%ld/%d)", 
			     objectManager.Count (), MAX_OBJECTS);
	g_data.Trace(Error, message);
	}
return 0;
}

// -----------------------------------------------------------------------------
// LoadGameItems()
//
// ACTION - Loads the player, object, wall, door, trigger, and
//          materialization center data from an RDL file.
// -----------------------------------------------------------------------------

short CMine::LoadGameItems (CFileManager* fp, bool bCreate) 
{
// Check signature
Info ().Read (fp, IsD2XLevel ());
if (FileInfo ().signature != 0x6705) {
	g_data.Trace(Error, "Game data signature incorrect");
	return -1;
	}
if (Info ().fileInfo.version < 14) 
	m_currentLevelName [0] = 0;
else {  /*load mine filename */
	for (char *p = m_currentLevelName; ; p++) {
		*p = fp->ReadChar ();
		if (*p== '\n')
			*p = 0;
		if (*p == 0)
			break;
		}
	}

objectManager.Read (fp);
wallManager.Read (fp);
triggerManager.Read (fp);
triggerManager.ReadReactor (fp);
segmentManager.ReadRobotMakers (fp);
if (!g_data.IsD1File ()) {
	segmentManager.ReadEquipMakers (fp);
	lightManager.ReadLightDeltas (fp);
	}
segmentManager.RenumberProducers (true);
return 0;
}

// -------------------------------------------------------------------------------
//eof mine.cpp