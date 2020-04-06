#include "stdafx.h"
#include "MemoryFile.h"
#include "HogManager.h"
#include "mine.h"
#include "LightManager.h"
#include "PaletteManager.h"
#include "RobotManager.h"
#include "TextureManager.h"
#include "TimeDate.h"
#include <io.h>

CHogManager* hogManager = null;

//------------------------------------------------------------------------------
// local prototypes
void DeleteSubFile (CFileManager& fp, long size, long offset, int numEntries, int deleteIndex);
bool ExportSubFile (const char *pszSrc, const char *pszDest, long offset, long size);

//------------------------------------------------------------------------------

static ubyte dataBuf [65536];
char tempStr[4096]; // a bunch of functions do string manipulation. STL would be better but this will be thrown out soon anyway

//------------------------------------------------------------------------------

CHogManager::CHogManager()
{
}

//------------------------------------------------------------------------------

long CHogManager::FindSubFile (CFileManager& fp, const char* pszFile, const char* pszSubFile, const char* pszExt)
{
	if (!pszSubFile)
		return 0;

strcpy_s (tempStr, sizeof (tempStr), pszSubFile);
if (pszExt) {
	char* p = strrchr (tempStr, '.');
	if (p == null) 
		strcat_s (tempStr, pszExt);
	else
		strcpy_s (p, 5, pszExt);
	}

long size, offset;
CLevelHeader lh;
FindFileData(pszFile, tempStr, lh, size, offset, TRUE, &fp);
if ((size <= 0) && (offset < 0))
	return 0;
fp.Seek (lh.Size () + offset, SEEK_SET);
return size;
}

//------------------------------------------------------------------------------
// CHogManager - load level
//------------------------------------------------------------------------------

bool CHogManager::LoadLevel (LPSTR pszFile, LPSTR pszSubFile) 
{
	CFileManager	fSrc;
	CLevelHeader	lh;
	long				size, offset;
	char				szPreviousPalette [256];
	char				szPreviousPigPath [256];

strcpy_s (szPreviousPalette, ARRAYSIZE (szPreviousPalette), paletteManager.Name ());
strcpy_s (szPreviousPigPath, ARRAYSIZE (szPreviousPigPath), g_data.GetD2Path());

if (!pszFile)
	pszFile = m_szFile;

if (!pszSubFile)
	return false;

if (!FindFileData(pszFile, pszSubFile, lh, size, offset))
	return false;

strcpy_s(m_levelFileName, pszSubFile);
if (!fSrc.Open (pszFile, "rb")) {
	g_data.DoErrorMsg ("Unable to load requested level.");
	return false;
	}

// set subfile name
fSrc.Seek (offset, SEEK_SET);

if (!lh.Read (&fSrc))
	return false;
// skip mineDataOffset and gameDataOffset
if (!theMine->LoadMineSigAndType (&fSrc)) {
	fSrc.ReadInt32 ();
	fSrc.ReadInt32 ();
	theMine->LoadPaletteName (&fSrc);
	fSrc.Seek (lh.Size () + offset, SEEK_SET);
	m_level.Load (&fSrc, size);
	lightManager.LoadDefaults ();
	if (0 < (size = FindSubFile (fSrc, pszFile, pszSubFile, ".pal")))
		paletteManager.LoadCustom (&fSrc, size);
	if (0 < (size = FindSubFile (fSrc, pszFile, pszSubFile, ".lgt")))
		lightManager.ReadLightMap (fSrc, size);
	if (0 < (size = FindSubFile (fSrc, pszFile, pszSubFile, ".clr")))
		lightManager.ReadColorMap (fSrc);
	paletteManager.Reload (paletteManager.Name ());
	if (!textureManager.LoadTextures ()) {
		sprintf_s (tempStr, sizeof (tempStr), "Unable to load palette \"%s\" - may be missing or corrupt.", paletteManager.Name ());
		g_data.DoErrorMsg (tempStr);
		// Let's try reverting to the previous palette;
		// at least we might be able to open the level
		paletteManager.SetName (szPreviousPalette);
		g_data.SetD2Path(szPreviousPigPath);
		paletteManager.Reload (paletteManager.Name ());
		textureManager.LoadTextures ();
		}
	// Custom textures - always prefer .pog if there is one
	if (0 < (size = FindSubFile (fSrc, pszFile, pszSubFile, ".pog")))
		textureManager.ReadPog (fSrc, size);
	else if (0 < (size = FindSubFile (fSrc, pszFile, pszSubFile, ".dtx")))
		textureManager.ReadDtx (fSrc, size);
	g_data.modelManager->Reset ();
	robotManager.ClearHXMData ();
	if (0 < (size = FindSubFile (fSrc, pszFile, pszSubFile, ".hxm"))) {
		robotManager.ReadHXM (fSrc, size);
		int nCustom = 0;
		for (int i = 0; i < (int) robotManager.Count (); i++)
			if (robotManager.RobotInfo (i)->Info ().bCustom)
				nCustom++;
		sprintf_s (tempStr, sizeof (tempStr)," Hog manager: %d custom robots read", nCustom);
		DEBUGMSG (tempStr);
		}
	return true;
	}
return false;
}

CMemoryFile* CHogManager::GetLevel()
{
	return &m_level;
}

const std::vector<tHogFileData>& CHogManager::GetFileList()
{
	return m_fileList;
}

tHogFileData CHogManager::GetFileInfoAtOffset(long fileOffset)
{
	tHogFileData data{};

	CFileManager fp;
	if (!fp.Open(m_szFile, "r+b"))
	{
		g_data.DoErrorMsg("Could not open HOG file.");
		return data;
	}

	CLevelHeader lh;
	fp.Seek(fileOffset, SEEK_SET);
	if (!lh.Read(&fp))
	{
		g_data.DoErrorMsg("Error reading HOG file");
		return data;
	}

	data.m_fileName = lh.Name();
	data.m_offs = fileOffset;
	data.m_size = lh.FileSize();

	fp.Close();
	return data;
}

//------------------------------------------------------------------------------
// CHogManager::RenameMsg()
//------------------------------------------------------------------------------

void CHogManager::RenameFile(const char* oldFileName, const char* newFileName)
{
	CFileManager fp;
	if (!fp.Open(m_szFile, "r+b"))
	{
		g_data.DoErrorMsg("Could not open HOG file.");
		return;
	}

	CLevelHeader lh;
	long fileSize, fileOffset;
	// Having FindFileData write the input parameter is pretty dumb... need to fix that
	strcpy_s(tempStr, sizeof(tempStr), oldFileName);
	if (!FindFileData(NULL, tempStr, lh, fileSize, fileOffset, FALSE, &fp))
	{
		sprintf_s(tempStr, "Could not find file \"%s\".", oldFileName);
		g_data.DoErrorMsg(tempStr);
		return;
	}

	// Normalize filename
	std::vector<char> nameBuffer(lh.NameSize());
	strcpy_s(nameBuffer.data(), nameBuffer.size(), newFileName);
	_strlwr_s(nameBuffer.data(), nameBuffer.size());
	nameBuffer[nameBuffer.size() - 1] = '\0';

	std::copy(nameBuffer.begin(), nameBuffer.end(), lh.Name());

	fp.Seek (fileOffset, SEEK_SET);
	if (!lh.Write (&fp, lh.Extended ()))
		g_data.DoErrorMsg ("Cannot write to HOG file");
}

//------------------------------------------------------------------------------
// CHogManager - read hog data
//------------------------------------------------------------------------------

int CHogManager::ReadSignature (CFileManager* fp, bool bVerbose)
{
	char sig [3];

fp->Read (sig, 3, 1); // verify signature "DHF"
if ((sig [0] == 'D') && (sig [1] == 'H') && (sig [2] == 'F'))
	return m_bExtended = 0;
else if ((sig [0] && 'D') && (sig [1] && '2') && (sig [2] && 'X')) 
	return m_bExtended = 1;
else {
	if (bVerbose)
		g_data.DoErrorMsg ("This is not a Descent HOG file");
	fp->Close ();
	return -1;
	}
}

//------------------------------------------------------------------------------

int CHogManager::ReadData (LPCSTR pszFile, bool bAllFiles, bool bOnlyLevels, bool bGetFileData) 
{
	ubyte index;
	CFileManager fp;

m_fileList.clear();
if (!fp.Open (pszFile, "rb")) {
	sprintf_s (tempStr, sizeof (tempStr), "Unable to open HOG file (%s)", pszFile);
	g_data.DoErrorMsg (tempStr);
	return -1;
	}
if (0 > ReadSignature (&fp))
	return -1;

CLevelHeader lh;
long position = 3;
int nFiles = 0;

while (!fp.EoF ()) {
	fp.Seek (position, SEEK_SET);
	if (!lh.Read (&fp)) {
		g_data.DoErrorMsg ("Error reading HOG file");
		fp.Close ();
		return -1;
		}
	if (bAllFiles 
		 || strstr (lh.Name (), ".rdl") || strstr (lh.Name (), ".rl2")
		 || (!bOnlyLevels && (strstr (lh.Name (), ".pog") || 
									 strstr (lh.Name (), ".hxm") || 
									 strstr (lh.Name (), ".lgt") || 
									 strstr (lh.Name (), ".clr") || 
									 strstr (lh.Name (), ".pal") || 
									 strstr (lh.Name (), ".dtx")))) {
		if (bGetFileData)
		{
			m_fileList.push_back(tHogFileData{ position, lh.FileSize(), lh.Name(), nFiles });
		}
		nFiles++;
		}
	position += lh.Size () + lh.FileSize ();
	if (position >= fp.Length ())
		break;
	}
fp.Close ();
return m_bExtended;
}

void CHogManager::SetMissionName(const char* newFileName)
{
	strcpy_s(m_szFile, newFileName);
}

//------------------------------------------------------------------------------
// CHogManager - read hog data
//------------------------------------------------------------------------------

bool FindFileData (const char* pszFile, const char* pszSubFile, CLevelHeader& lh, long& nSize, long& nPos, BOOL bVerbose, CFileManager* fp)
{
	CFileManager _fp;
	char errorBuffer[512];

nSize = -1;
nPos = -1;
if (fp != null)
	fp->Seek (0);
else {
	fp = &_fp;
	if (!fp->Open (pszFile, "rb")) {
		if (bVerbose) {
			sprintf_s (tempStr, sizeof (tempStr), "Unable to open HOG file (%s)\n(%s)", pszFile, strerror_s (errorBuffer, errno));
			g_data.DoErrorMsg (tempStr);
			}
		return false;
		}
	}

if (0 > hogManager->ReadSignature (fp))
	return false;

long position = 3;
int nFiles = 0;

while (!fp->EoF ()) {
	fp->Seek (position, SEEK_SET);
	if (!lh.Read (fp)) {
		if (bVerbose)
			g_data.DoErrorMsg ("Error reading HOG file");
		break;
		}
	if ((strcmp(pszSubFile, "*") == 0) || !_strcmpi(lh.Name(), pszSubFile)) {
		nSize = lh.FileSize ();
		nPos = position;
		if (fp == &_fp)
			fp->Close ();
		return true;
		}
	position += lh.Size () + lh.FileSize ();
	if (position >= fp->Length ())
		break;
	}
if (fp == &_fp)
	fp->Close ();
return false;
}

//------------------------------------------------------------------------------
// extract_from_hog()
//------------------------------------------------------------------------------

bool ExportSubFile (const char *pszSrc, const char *pszDest, long offset, long size) 
{
CFileManager fSrc;
if (!fSrc.Open (pszSrc, "rb")) {
	g_data.DoErrorMsg ("Could not open HOG file.");
	return false;
	}
CFileManager fDest;
if (!fDest.Open (pszDest, "wb")) {
	g_data.DoErrorMsg ("Could not create export file.");
	return false;
	}
// seek to item's offset in HOG file
fSrc.Seek (offset, SEEK_SET);

CLevelHeader lh;
if (!lh.Read (&fSrc)) {
	g_data.DoErrorMsg ("Could not read HOG file.");
	return false;
	}

// create file (from HOG to file)
while (size > 0) {
	size_t nBytes, n;
	n = (size > sizeof (dataBuf)) ? sizeof (dataBuf) : size_t (size);
	nBytes = fSrc.Read (dataBuf, 1, n);
	fDest.Write (dataBuf, 1, (int) nBytes);
	size -= long (nBytes);
	if (nBytes != n)
		break;
	}
fDest.Close ();
fSrc.Close ();
return (size == 0);
}


//----------------------------------------------------------------------------------
// delete_sub_file()
//----------------------------------------------------------------------------------

void DeleteSubFile (CFileManager& fp, long size, long offset, int numEntries, int deleteIndex) 
{
	int nBytes;
// as long as we are not deleting the last item
if (deleteIndex < numEntries - 1) {
	// get size of chunk to remove from the fp, then move everything
	// down by that amount.
	do {
		fp.Seek (offset + size, SEEK_SET);
		nBytes = (int) fp.Read (dataBuf, 1, (int) sizeof (dataBuf));
		if (nBytes <= 0)
			break;
		fp.Seek (offset, SEEK_SET);
		fp.Write (dataBuf, 1, nBytes);
		offset += nBytes;
		} while (nBytes > 0);
	}
// set the new size of the fp
_chsize (_fileno (fp.File ()), offset);
}

//--------------------------------------------------------------------------------
// strip_extension
//--------------------------------------------------------------------------------

void strip_extension(char *str) 
{
char *ext = strrchr(str,'.');
if (ext)
	*ext = '\0';
}

//--------------------------------------------------------------------------------
// DeleteLevelSubFiles()
//
// deletes sub-files with same base name from hog
//--------------------------------------------------------------------------------

#define MAX_REGIONS 8

void DeleteLevelSubFiles (CFileManager& fp, const char *base, const char *ext = null) 
{
struct region {
	int index;
	int offset;
	int size;
	int files;
	};
	int nRegion = 0;
	int deleteIndex = 0;
	int numEntries = 0;
	region regions [MAX_REGIONS] = {{-1,0,0,0},{-1,0,0,0},{-1,0,0,0},{-1,0,0,0},{-1,0,0,0},{-1,0,0,0},{-1,0,0,0},{-1,0,0,0}};

// figure out regions of the fp to delete (3 regions max)
long offset = 3;
long size;
deleteIndex = -1;
CLevelHeader lh;
fp.Seek (0, SEEK_SET); // fp might have been previously set to end
while(!fp.EoF ()) {
	fp.Seek (offset, SEEK_SET); // skip "HOG"
	if (!lh.Read (&fp)) 
		break;
	size = lh.FileSize () + lh.Size ();
	if (nRegion < MAX_REGIONS) {
		LPSTR thisExt = strrchr (lh.Name (), '.');
		if (!thisExt)
			goto nextSubFile;
		if (_strcmpi (thisExt, ".rdl") &&
		    _strcmpi (thisExt, ".rl2") &&
		    _strcmpi (thisExt, ".hxm") &&
		    _strcmpi (thisExt, ".pog") &&
		    _strcmpi (thisExt, ".dtx") &&
		    _strcmpi (thisExt, ".pal") &&
		    _strcmpi (thisExt, ".lgt") &&
		    _strcmpi (thisExt, ".clr"))
			goto nextSubFile;
		if (ext && _strcmpi (ext, thisExt))
			goto nextSubFile;
		*thisExt = '\0';
		if (_strcmpi (base, lh.Name ()))
			goto nextSubFile;
		// try to merge this with the last region
		if ((nRegion > 0) && (deleteIndex == numEntries - 1)) {
			regions [nRegion-1].size += size;
			regions [nRegion-1].files++;
			numEntries--; // pretend that there is one less entry
			}
		else {
			regions [nRegion].index = numEntries;
			regions [nRegion].offset = offset;
			regions [nRegion].size = size;
			regions [nRegion].files++;
			nRegion++;
			}
		deleteIndex = numEntries;
		}
nextSubFile:
	offset += size;
	numEntries++;
	}
// now delete matching regions
while (nRegion > 0) {
	nRegion--;
	DeleteSubFile (fp, regions [nRegion].size, regions [nRegion].offset, numEntries, regions [nRegion].index);
	numEntries -= regions [nRegion].files;
	}
}

#undef MAX_REGNUM

//------------------------------------------------------------------------------
// write_sub_file()

int WriteSubFile (CFileManager& fDest, char *szSrc, char *szLevel) 
{
	CFileManager	fSrc;
	size_t			nBytes;

if (!fSrc.Open (szSrc, "rb")) {
	sprintf_s (tempStr, sizeof (tempStr), "Unable to open temporary file:\n%s",szSrc);
	g_data.DoErrorMsg (tempStr);
	return 0;
	}
// write szLevel (13 chars, null filled)
CLevelHeader lh (g_data.IsD2XLevel ());

int nameSize = lh.NameSize ();
strncpy_s (lh.Name (), nameSize, szLevel, nameSize);
_strlwr_s (lh.Name (), nameSize);
// calculate then write size
lh.SetFileSize (fSrc.Size ());
//fclose (fSrc);
//fSrc = fopen (szSrc,"rb");
lh.Write (&fDest);
// write data
for (int l = fSrc.Size (); l > 0; l -= sizeof (dataBuf)) {
	nBytes = fSrc.Read (dataBuf, 1, min (l, sizeof (dataBuf)));
	if (nBytes > 0)
		fDest.Write (dataBuf, 1, (int) nBytes);
	}
fSrc.Close ();
return lh.FileSize () + lh.Size ();
}

//------------------------------------------------------------------------------

bool DoesSubFileExist (const char *pszFile, const char *pszSubFile)
{
    CLevelHeader lh;
    long nSize, nPos;

	std::vector<char> temp(strlen(pszSubFile) + 1);
	strcpy_s(temp.data(), temp.size(), pszSubFile);
	return FindFileData(pszFile, temp.data(), lh, nSize, nPos, FALSE);
}

//------------------------------------------------------------------------------

bool DoesSubFileExist (CFileManager &fp, char *pszSubFile)
{
    CLevelHeader lh;
    long nSize, nPos;

return FindFileData (NULL, pszSubFile, lh, nSize, nPos, FALSE, &fp);
}

//------------------------------------------------------------------------------

const char *GetCustomFileExtension (const int nType)
{
	static const char* extensions [] = {".lgt", ".clr", ".pal", ".pog", ".hxm", ".dtx"};

if (nType < 0 || nType >= ARRAYSIZE (extensions))
	return null;
return extensions [nType];
}

//------------------------------------------------------------------------------
// make_hog()
//
// Action - makes a HOG file which includes three files
//             1. the rdl fp to include (only one)
//             2. a briefing fp (called brief.txb)
//             3. an ending sequence (same name as hog w/ .txb extension)
//          also makes a mission file for this HOG file
//
// Changes - now saves rl2 files
//------------------------------------------------------------------------------

typedef int (* subFileWriter) (CFileManager&);

//--------------------------------------------------------------------------------

int WriteCustomFile (const char* pszFile, char* pszSubFile, const int nType)
{
	int nResult = 1;
	CFileManager fp, fTmp;
	char szFolder [256], szTmp [256], szDest [256], szBase [256];
	long size = 0;

// determine base name
CFileManager::SplitPath (pszFile, szFolder, null, null);
CFileManager::SplitPath (pszSubFile, null, szBase, null);
sprintf_s (szDest, sizeof (szDest), "%s%s", szBase, GetCustomFileExtension (nType));
sprintf_s (szTmp, sizeof (szTmp), "%sdle_temp%s", szFolder, GetCustomFileExtension (nType));

if (!fp.Open (pszFile, "r+b")) {
	g_data.DoErrorMsg ("Destination HOG file not found or inaccessible.");
	return 0;
	}

// Save sounds from D1 DTX patches so we can write them back later
CDtxSoundList soundList;
if (nType == CUSTOM_FILETYPE_DTX && (0 < (size = hogManager->FindSubFile (fp, pszFile, szDest, null))))
	soundList.Load (fp, size);

// If target HOG already contains a file with this name, delete it so we can overwrite it
if (DoesSubFileExist (fp, szDest))
	DeleteLevelSubFiles (fp, szBase, GetCustomFileExtension (nType));

bool bHasContent = false;
switch (nType) {
	case CUSTOM_FILETYPE_LIGHTMAP:
		bHasContent = lightManager.HasCustomLightMap ();
		break;
	case CUSTOM_FILETYPE_COLORMAP:
		bHasContent = lightManager.HasCustomLightColors ();
		break;
	case CUSTOM_FILETYPE_PALETTE:
		bHasContent = paletteManager.Custom () != null;
		break;
	case CUSTOM_FILETYPE_POG:
		bHasContent = textureManager.HasCustomTextures ();
		break;
	case CUSTOM_FILETYPE_HXM:
		bHasContent = robotManager.HasCustomRobots ();
		break;
	case CUSTOM_FILETYPE_DTX:
		bHasContent = textureManager.HasCustomTextures () || (soundList.Count () > 0);
		break;
	default:
		break;
	}

// Write the new file, but only if there's anything to write to it
if (bHasContent) {
	fp.Seek (0, SEEK_END);
	if (!fTmp.Open (szTmp, "wb"))
		return 0;
	switch (nType) {
		case CUSTOM_FILETYPE_LIGHTMAP:
			lightManager.WriteLightMap (fTmp);
			break;
		case CUSTOM_FILETYPE_COLORMAP:
			lightManager.WriteColorMap (fTmp);
			break;
		case CUSTOM_FILETYPE_PALETTE:
			nResult = paletteManager.SaveCustom (&fTmp);
			break;
		case CUSTOM_FILETYPE_POG:
			nResult = textureManager.CreatePog (fTmp);
			break;
		case CUSTOM_FILETYPE_HXM:
			nResult = robotManager.WriteHXM (fTmp);
			break;
		case CUSTOM_FILETYPE_DTX:
			nResult = textureManager.CreateDtx (fTmp, soundList);
			break;
		default:
			nResult = 0;
			break;
		}
	fTmp.Close ();
	if (nResult) {
		nResult = WriteSubFile (fp, szTmp, szDest) > 0;
		}
	CFileManager::Delete (szTmp);
	}

fp.Close ();
return nResult;
}

//--------------------------------------------------------------------------------

void CHogManager::WriteHogHeader (CFileManager& fp) 
{
if ((m_bExtended = (g_data.IsD2XLevel () ? 1 : 0)))
	fp.Write ("D2X", 1, 3); // starts with Descent Hog File
else
	fp.Write ("DHF", 1, 3); // starts with Descent Hog File
}

//--------------------------------------------------------------------------------

int CreateHogFile (char* rdlFilename, char* hogFilename, char* szSubFile, bool bSaveAs) 
{
	CFileManager	fp, fTmp;
	char				*pszNameStart, *pszNameEnd, *pszExtStart;
	char				szFolder [256], szFile [256], szExt [256], szTmp [256];
	int				custom_robots = 0;
	int				custom_textures = 0;

// create HOG file which contains szTmp.rdl, szTmp.txb, and dlebrief.txb");
if (!fp.Open (hogFilename, "wb")) {
	sprintf_s (tempStr, sizeof (tempStr), "Unable to create HOG file:\n%s", hogFilename);
	g_data.DoErrorMsg (tempStr);
	return 0;
	}
// write fp type
hogManager->WriteHogHeader (fp);
// get base szTmp w/o extension and w/o path

CLevelHeader lh (g_data.IsD2XLevel ());
int nameSize = lh.NameSize ();

memset (szFile, 0, nameSize);
CFileManager::SplitPath (hogFilename, szFolder, szFile, szExt);
szFile [nameSize - 1] = 0;
pszNameStart = strrchr (hogFilename,'\\');
if (pszNameStart == null)
	pszNameStart = hogFilename;
else
	pszNameStart++; // move to just pass the backslash
strncpy_s (szFile, sizeof (szFile), pszNameStart, nameSize - 1);
szFile [nameSize - 1] = null; // make sure it is null terminated
pszNameEnd = strrchr ((char *)szFile,'.');
if (!pszNameEnd)
	pszNameEnd = szFile + strlen ((char *)szFile);
memset (pszNameEnd, 0, nameSize - 1 - int (pszNameEnd - szFile));
// write rdl file
if (*szSubFile) {
	CFileManager::SplitPath (szSubFile, null, szFile, null);
	for (pszExtStart = szSubFile; *pszExtStart && (*pszExtStart != '.'); pszExtStart++)
		;
	strncpy_s (szFile, sizeof (szFile), szSubFile, pszExtStart - szSubFile);
	szFile [pszExtStart - szSubFile] = '\0';
	}

sprintf_s (szTmp, sizeof (szTmp), g_data.IsD1File () ? "%s.rdl" : "%s.rl2", szFile);
WriteSubFile (fp, rdlFilename, szTmp);
CFileManager::Delete (szTmp);

fp.Close ();
MakeMissionFile (hogFilename, szSubFile, custom_textures, custom_robots, bSaveAs);
return 1;
}

//==========================================================================
// MENU - Save
//==========================================================================

int SaveToHog (LPSTR szHogFile, LPSTR szSubFile, bool bSaveAs) 
{
	CFileManager	fTmp;
	char				szTmp [256], szBase [MAX_PATH];
	char*				psz;

_strlwr_s (szHogFile, 256);
psz = strstr (szHogFile, "new.");
if (!*szSubFile || psz) { 
	CLevelHeader lh (g_data.IsD2XLevel ());
	if (!g_data.DoInputDialog("Name mine", "Enter file name:", szSubFile, lh.NameSize() - 4))
		return 0;
	LPSTR ext = strrchr (szSubFile, '.');
	if (ext)
		*ext = '\0';
	psz = strrchr (szSubFile, '.');
	if (psz)
		*psz = '\0';
	psz = strstr (szHogFile, "new.");
	if (psz) {
		int l = int (psz - szHogFile);
		strcpy_s (psz, 256 - l, szSubFile);
		l += (int) strlen (szSubFile);
		strcat_s (psz, 256 - l, ".hog");
		}
	strcpy_s (szBase, sizeof (szBase), szSubFile);
	strcat_s (szSubFile, 256, (g_data.IsD1File ()) ? ".rdl" : ".rl2");
	}
else {
	LPSTR ext = strrchr (szSubFile, '.');
	if (ext)
		*ext = '\0';
	strcpy_s (szBase, sizeof (szBase), szSubFile);
	strcat_s (szSubFile, 256, (g_data.IsD1File ()) ? ".rdl" : ".rl2");
	}
sprintf_s(szTmp, sizeof(szTmp), "%s\\dle_temp.rdl", g_data.GetAppFolder());

CFileManager fp;
if (!fp.Open (szHogFile, "r+b")) {
	// HOG file doesn't exist, so just create it and return
	theMine->Save (szTmp);
	int nRes = CreateHogFile (szTmp, szHogFile, szSubFile, true);
	CFileManager::Delete (szTmp);
	return nRes;
	}

// Make sure we can successfully write the level (to the temporary file) first.
// Otherwise modifying the hog file is probably not a good idea
if (theMine->Save (szTmp) <= 0)
	return 0;

hogManager->WriteHogHeader (fp); // make sure the hog header is "D2X" if the hog file contains extended level headers (-> long level filenames)
fp.Seek (3, SEEK_SET);

// See if another level with the same name exists
// and see if there are any other files here (ignore hxm and pog files)
bool bIdenticalLevelFound = false;
CLevelHeader lh;
while (!fp.EoF ()) {
	if (!lh.Read (&fp))
		break;
	_strlwr_s (lh.Name (), lh.NameSize ());
	lh.Name () [lh.NameSize () - 1] = null; // null terminate
	if (!_strcmpi ((char *) lh.Name (), szSubFile)) // same level
		bIdenticalLevelFound = true;
	fp.Seek (lh.FileSize (), SEEK_CUR);
	}
if (bIdenticalLevelFound) {
	if (g_data.DoQueryMsg ("Overwrite old level with same name?") != IDYES) {
		CFileManager::Delete (szTmp);
		fp.Close ();
		return 0;
		}
	DeleteLevelSubFiles (fp, szBase, (g_data.IsD1File ()) ? ".rdl" : ".rl2");
	}
else if (!g_data.ExpertMode ())
	g_data.DoErrorMsg ("Don't forget to add this level's name to the mission file.");
fp.Close ();

// now append sub-file to the end of the HOG file
if (!fp.Open (szHogFile, "ab")) {
	g_data.DoErrorMsg ("Could not open destination HOG file for save.");
	return 0;
	}
fp.Seek (0, SEEK_END);
WriteSubFile (fp, szTmp, szSubFile);
CFileManager::Delete (szTmp);

fp.Close ();
return 1;
}

//------------------------------------------------------------------------------
// write_mission_file()

static LPCSTR szMissionName [] = {"name", "zname", "d2x-name", null};
static LPCSTR szMissionInfo [] = {"editor", "build_time", "date", "revision", "author", "email", "web_site", "briefing", null};
static LPCSTR szMissionType [] = {"type", null};
static LPCSTR szMissionTypes [] = {"anarchy", "normal", null};
static LPCSTR szMissionFlags [] = {"normal", "anarchy", "robo_anarchy", "coop", "capture_flag", "hoard", null};
static LPCSTR szCustomFlags [] = {"custom_textures", "custom_robots", "custom_music", null};
static LPCSTR szAuthorFlags [] = {"multi_author", "want_feedback", null};
static LPCSTR szNumLevels [] = {"num_levels", null};
static LPCSTR szNumSecrets [] = {"num_secrets", null};
static LPCSTR szBool [] = {"no", "yes", null};

static LPCSTR *szTags [] = {szMissionName, szMissionInfo, szMissionType, szMissionFlags, szCustomFlags, szAuthorFlags, szNumLevels, szNumSecrets};

//------------------------------------------------------------------------------

int atob (LPSTR psz, size_t nSize)
{
_strlwr_s (psz, nSize);
int i;
for (i = 0; i < 2; i++)
	if (!strcmp (psz, szBool [i]))
		return i;
return 0;
}

//------------------------------------------------------------------------------

int ReadMissionFile (char *pszFile) 
{
	FILE	*fMsn;
	char  szMsn [256];
	LPSTR	psz;
	LPCSTR* ppsz;
	char	szTag [256], szValue [256], szBuf [256];
	int	i, j, l;

strcpy_s (szMsn, sizeof (szMsn), pszFile);
char *pExt = strrchr (szMsn, '.');
if (pExt)
	*pExt = '\0';
strcat_s (szMsn, sizeof (szMsn), (g_data.IsD1File ()) ? ".msn" : ".mn2");
fopen_s (&fMsn, szMsn, "rt");
if (!fMsn) {
	DEBUGMSG (" Hog manager: Mission fp not found.");
	return -1;
	}
*g_data.missionData = {};
l = 0;
while (fgets (szBuf, sizeof (szBuf), fMsn)) {
	if ((psz = strstr (szBuf, "\r\n")) || (psz = strchr (szBuf, '\n')))	//replace cr/lf
		*psz = '\0';
	if (*szBuf == ';') {	//comment
		if (l) {
			strncpy_s (g_data.missionData->comment + l, sizeof (g_data.missionData->comment) - l, "\r\n", sizeof (g_data.missionData->comment) - l);
			l += 2;
			}
		strncpy_s (g_data.missionData->comment + l, sizeof (g_data.missionData->comment) - l, szBuf + 1, sizeof (g_data.missionData->comment) - l);
		l += int (strlen (szBuf + 1));
		continue;
		}
	if (!(psz = strchr (szBuf, '=')))	// otherwise need <tag> '=' <value> format
		continue;
	if (psz - szBuf >= sizeof (szTag))	// invalid keyword
		continue;
	*szTag = '\0';
	for (i = -1; psz + i > szBuf; i--)	// remove blanks around '='
		if (psz [i] != ' ') {
			psz [++i] = '\0';
			strncpy_s (szTag, sizeof (szTag), szBuf, sizeof (szTag));
			szTag [sizeof (szTag) - 1] = '\0';
			break;
			}
	*szValue = '\0';
	for (i = 1; psz [i]; i++)
		if (psz [i] != ' ') {
			strncpy_s (szValue, psz + i, sizeof (szValue));
			szValue [sizeof (szValue) - 1] = '\0';
			break;
			}
	if (!(*szTag && *szValue))	// mustn't be empty
		continue;
	_strlwr_s (szTag, sizeof (szTag));	// find valid tag
	for (i = 0, j = -1; i < 8; i++)
		for (ppsz = szTags [i]; *ppsz; ppsz++)
			if (!strcmp (*ppsz, szTag)) {
				j = int (ppsz - szTags [i]);
				goto tagFound;
				}
	continue;
tagFound:
	switch (i) {
		case 0:
			strcpy_s (g_data.missionData->missionName, sizeof (g_data.missionData->missionName), szValue);
			break;
		case 1:
			strcpy_s (g_data.missionData->missionInfo [j], sizeof (g_data.missionData->missionInfo [j]), szValue);
			break;
		case 2:
			_strlwr_s (szValue, sizeof (szValue));
			for (j = 0; j < 2; j++)
				if (!strcmp (szValue, szMissionTypes [j]))
					g_data.missionData->missionType = j;
			break;
		case 3:
			g_data.missionData->missionFlags [j] = atob (szValue, sizeof (szValue));
			break;
		case 4:
			g_data.missionData->customFlags [j] = atob (szValue, sizeof (szValue));
			break;
		case 5:
			g_data.missionData->authorFlags [j] = atob (szValue, sizeof (szValue));
			break;
		case 6:
			g_data.missionData->numLevels = atol (szValue);
			for (i = 0; i < g_data.missionData->numLevels; i++) {
				fgets (g_data.missionData->levelList [i], sizeof (g_data.missionData->levelList [i]), fMsn);
				for (j = int (strlen (g_data.missionData->levelList [i])); --j; )
					if ((g_data.missionData->levelList [i][j] != '\r') &&
						 (g_data.missionData->levelList [i][j] != '\n'))
						break;
				g_data.missionData->levelList [i][j+1] = '\0';
				_strlwr_s (g_data.missionData->levelList [i], sizeof (g_data.missionData->levelList [i]));
				}
			break;
		case 7:
			g_data.missionData->numSecrets = atol (szValue);
			for (i = 0, j = g_data.missionData->numLevels; i < g_data.missionData->numSecrets; i++, j++) {
				fscanf_s (fMsn, "%s", g_data.missionData->levelList [j], sizeof (g_data.missionData->levelList [j]));
				_strlwr_s (g_data.missionData->levelList [i], sizeof (g_data.missionData->levelList [i]));
				}
			break;
		default:
			continue;
		}
	}
fclose (fMsn);
return 0;
}

//------------------------------------------------------------------------------

int WriteMissionFile (char *pszFile, int levelVersion, bool bSaveAs) 
{
	FILE	*fMsn;
	char  szMsn [256], szLevel [256];
	int	i, j;

strcpy_s (szMsn, sizeof (szMsn), pszFile);
char *pExt = strrchr (szMsn, '.');
if (pExt)
	*pExt = '\0';
strcat_s (szMsn, sizeof (szMsn), (g_data.IsD1File ()) ? ".msn" : ".mn2");
if (bSaveAs) {
	fopen_s (&fMsn, szMsn, "rt");
	if (fMsn) {
		fclose (fMsn);
		if (g_data.DoQuery2Msg("A mission file with that name already exists.\nOverwrite mission file?", MB_YESNO) != IDYES)
			return -1;
		}
	}
// create mission file
fopen_s (&fMsn, szMsn, "wt");
if (!fMsn)
	return -1;
if (levelVersion >= 9)
	fprintf (fMsn, "d2x-name = %s\n", g_data.missionData->missionName);
else if (levelVersion >= 8)
	fprintf (fMsn, "zname = %s\n", g_data.missionData->missionName);
else
	fprintf (fMsn, "name = %s\n", g_data.missionData->missionName);
fprintf (fMsn, "type = %s\n", szMissionTypes [g_data.missionData->missionType]);
fprintf (fMsn, "num_levels = %d\n", g_data.missionData->numLevels);
for (i = 0; i < g_data.missionData->numLevels; i++)
	fprintf (fMsn, "%s\n", g_data.missionData->levelList [i]);
if (g_data.missionData->numSecrets) {
	fprintf (fMsn, "num_secrets = %d\n", g_data.missionData->numSecrets);
	for (j = 0; j < g_data.missionData->numSecrets; i++, j++) {
		strcpy_s (szLevel, sizeof (szLevel), g_data.missionData->levelList [i]);
		char* ext = strrchr (szLevel, '.');
		if (ext)
			*ext = '\0';
		strcat_s (szLevel, sizeof (szLevel), g_data.IsD1File () ? ".rdl" : ".rl2");
		fprintf (fMsn, "%s\n", szLevel);
		}
	}
for (i = 0; i < 8; i++)
	if (*g_data.missionData->missionInfo [i])
		fprintf (fMsn, "%s = %s\n", szMissionInfo [i], g_data.missionData->missionInfo [i]);
for (i = 0; i < 3; i++)
	fprintf (fMsn, "%s = %s\n", szCustomFlags [i], szBool [g_data.missionData->customFlags [i]]);
for (i = 0; i < 6; i++)
	fprintf (fMsn, "%s = %s\n", szMissionFlags [i], szBool [g_data.missionData->missionFlags [i]]);
for (i = 0; i < 2; i++)
	fprintf (fMsn, "%s = %s\n", szAuthorFlags [i], szBool [g_data.missionData->authorFlags [i]]);
if (*g_data.missionData->comment) {
	char *pi, *pj;
	for (pi = pj = g_data.missionData->comment; ; pj++)
		if (!*pj) {
			fprintf (fMsn, ";%s\n", pi);
			break;
			}
		else if ((pj [0] == '\r') && (pj [1] == '\n')) {
			*pj = '\0';
			fprintf (fMsn, ";%s\n", pi);
			*pj++ = '\r';
			pi = pj + 1;
			}
	}
fclose (fMsn);
return 0;
}

//------------------------------------------------------------------------------
// make_mission_file()

int MakeMissionFile (char *pszFile, char *pszSubFile, int bCustomTextures, int bCustomRobots, bool bSaveAs) 
{
	char	szBaseName [256];
	char	szTime [20];

//memset (&missionData, 0, sizeof (missionData));
CFileManager::SplitPath (pszSubFile, null, szBaseName, null);
if (!*g_data.missionData->missionName)
	strcpy_s (g_data.missionData->missionName, sizeof (g_data.missionData->missionName), szBaseName);
if (bSaveAs || !*g_data.missionData->missionName)
	do {
		if (!g_data.DoInputDialog("Mission title", "Enter mission title:", g_data.missionData->missionName, sizeof(g_data.missionData->missionName)))
			return -1;
	} while (!*g_data.missionData->missionName);
g_data.missionData->missionType = 1;
g_data.missionData->numLevels = 1;

CLevelHeader lh (g_data.IsD2XLevel());
char *pszExt = strchr (pszSubFile, '.');
int l = int (pszExt ? pszExt - pszSubFile : strlen (pszSubFile));
if (l > lh.NameSize () - 4) {
	pszSubFile [lh.NameSize () - 4] = '\0';
	if (pszExt)
		strcat_s (pszSubFile, lh.NameSize (), pszExt);
	}
strcpy_s (g_data.missionData->levelList [0], sizeof (g_data.missionData->levelList [0]), pszSubFile);
if (!strchr (pszSubFile, '.'))
	strcat_s (g_data.missionData->levelList [0], sizeof (g_data.missionData->levelList [0]), g_data.IsD2File () ? ".rl2" : ".rdl");
g_data.missionData->numSecrets = 0;
memset (g_data.missionData->missionInfo, 0, sizeof (g_data.missionData->missionInfo));
strcpy_s (g_data.missionData->missionInfo [0], sizeof (g_data.missionData->missionInfo [0]), "DLE");
strcpy_s (g_data.missionData->missionInfo [2], sizeof (g_data.missionData->missionInfo [2]), DateStr (szTime, sizeof (szTime), true));
if (bSaveAs)
	strcpy_s (g_data.missionData->missionInfo [3], sizeof (g_data.missionData->missionInfo [3]), "1.0");
g_data.missionData->customFlags [0] = bCustomTextures;
g_data.missionData->customFlags [1] = bCustomRobots;
return WriteMissionFile (pszFile, g_data.LevelVersion (), bSaveAs);
}

//------------------------------------------------------------------------------
// eof fp.cpp