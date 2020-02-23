#include <assert.h>

#include "stdafx.h"
#include "textures.h"
#include "FileManager.h"
#include "dle-xp-res.h"
#include "global.h"
#include "PaletteManager.h"
#include "TextureManager.h"
#include "HogManager.h"
#include "dle-xp.h"

extern short nDbgTexture;

//-----------------------------------------------------------------------------------
// ReadPog ()
//-----------------------------------------------------------------------------------

int CTextureManager::ReadPog (CFileManager& fp, long nFileSize) 
{
	CPigHeader		pigFileInfo (1);
	CPigTexture		pigTexInfo (1);

	uint*				textureCount = 0;
	ushort*			xlatTbl = null;
	uint				offset, hdrOffset, bmpOffset, hdrSize, xlatSize;
	ushort			nCustomTextures, nUnknownTextures, nMissingTextures;
	CTexture*		pTexture;
	int				fileType = DLE.FileType ();

// make sure this is descent 2 fp
if (DLE.IsD1File ()) {
	INFOMSG (" Descent 1 does not support custom textures.");
	return 1;
	}

uint startOffset = fp.Tell ();
// read file header
pigFileInfo.Read (fp);
if (pigFileInfo.nId != 0x474f5044L) {  // 'DPOG'
	ErrorMsg ("Invalid pog file - reading from hog file");
	return 1;
	}
//Release ();
sprintf_s (message, sizeof (message), " Pog manager: Reading %d custom textures", pigFileInfo.nTextures);
DEBUGMSG (message);
if (!(xlatTbl = new ushort [pigFileInfo.nTextures]))
	return 5;
xlatSize = pigFileInfo.nTextures * sizeof (ushort);
offset = fp.Tell ();
fp.Read (xlatTbl, xlatSize, 1);
// loop for each custom texture
nCustomTextures = 0;
nUnknownTextures = 0;
nMissingTextures = 0;
hdrOffset = offset + xlatSize;
hdrSize = xlatSize + pigFileInfo.nTextures * sizeof (PIG_TEXTURE_D2);
bmpOffset = offset + hdrSize;

DLE.MainFrame ()->InitProgress (pigFileInfo.nTextures);

for (int i = 0; i < pigFileInfo.nTextures; i++) {
	DLE.MainFrame ()->Progress ().StepIt ();
#ifdef _DEBUG
	if (i == nDbgTexture)
		nDbgTexture = nDbgTexture;
#endif
	// read texture index
	ushort nIndex = xlatTbl [i];

	// get texture data offset from texture header
	fp.Seek (hdrOffset + i * sizeof (PIG_TEXTURE_D2), SEEK_SET);
	pigTexInfo.Read (&fp);
	if ((long) (hdrSize + pigTexInfo.offset) >= nFileSize) {
		nMissingTextures++;
		continue;
		}
	
	fp.Seek (bmpOffset + pigTexInfo.offset, SEEK_SET);
	pTexture = OverrideTexture (nIndex - 1, null, 1);
	pTexture->LoadFromPog (fp, pigTexInfo);
	nCustomTextures++;
	}
if (nUnknownTextures) {
	sprintf_s (message, sizeof (message), " Pog manager: %d unknown textures found.", nUnknownTextures);
	DEBUGMSG (message);
	}
if (nMissingTextures) {
	sprintf_s (message, sizeof (message), " Pog manager: %d textures missing (Pog file probably damaged).", nMissingTextures);
	DEBUGMSG (message);
	}
// Textures shouldn't be marked modified on first load. Assuming here that POGs are only read while
// opening a level, otherwise we'll have to unmark each texture individually
CommitTextureChanges ();

DLE.MainFrame ()->Progress ().DestroyWindow ();

if (xlatTbl)
	delete xlatTbl;
return 0;
}

//-----------------------------------------------------------------------------------

int CTextureManager::ReadDtx (CFileManager& fp, long nFileSize)
{
	CPigHeader pigFileInfo (0);
	CPigTexture pigTexInfo (0);
	uint hdrOffset, bmpOffset, hdrSize;
	ushort nUnknownTextures, nMissingTextures;
	CTexture* pTexture;

if (DLE.IsD2File ()) {
	INFOMSG (" DTX patches are not supported for Descent 2 levels. Use a POG file instead.");
	return 1;
	}

// read file header
pigFileInfo.Read (fp);
sprintf_s (message, sizeof (message), " Pog manager: Reading %d custom textures", pigFileInfo.nTextures);
DEBUGMSG (message);
hdrOffset = fp.Tell ();
hdrSize = pigFileInfo.nTextures * sizeof (PIG_TEXTURE_D1) + pigFileInfo.nSounds * sizeof (PIG_SOUND);
bmpOffset = hdrOffset + hdrSize;
nUnknownTextures = 0;
nMissingTextures = 0;

DLE.MainFrame ()->InitProgress (pigFileInfo.nTextures);

for (int i = 0; i < pigFileInfo.nTextures; i++) {
	DLE.MainFrame ()->Progress ().StepIt ();

	// get texture data offset from texture header
	fp.Seek (hdrOffset + i * sizeof (PIG_TEXTURE_D1), SEEK_SET);
	pigTexInfo.Read (&fp);
	if ((long) (hdrSize + pigTexInfo.offset) >= nFileSize) {
		nMissingTextures++;
		continue;
		}

	// find the texture index - this requires a name lookup
	int nTexture;
	bool bFound = false;
	for (int nMatchTexture = 0; nMatchTexture < GlobalTextureCount (); nMatchTexture++) {
		if (strnicmp (textureManager.TextureByIndex (nMatchTexture)->Name (), pigTexInfo.name, ARRAYSIZE (pigTexInfo.name)) == 0 &&
			((pigTexInfo.dflags & BM_DFLAG_ANIMATED) == 0 ||
			(pigTexInfo.dflags & BM_DFLAG_ANIMFRAME_MASK) == textureManager.TextureByIndex (nMatchTexture)->FrameNumber ())
			) {
			nTexture = nMatchTexture;
			bFound = true;
			break;
			}
		}
	if (!bFound) {
		nUnknownTextures++;
		continue;
		}
	
	fp.Seek (bmpOffset + pigTexInfo.offset, SEEK_SET);
	pTexture = OverrideTexture (nTexture, null);
	pTexture->LoadFromPog (fp, pigTexInfo);
	}
if (nUnknownTextures) {
	sprintf_s (message, sizeof (message), " Pog manager: Unable to resolve %d textures in the DTX file.", nUnknownTextures);
	DEBUGMSG (message);
	}
if (nMissingTextures) {
	sprintf_s (message, sizeof (message), " Pog manager: %d textures missing (DTX file probably damaged).", nMissingTextures);
	DEBUGMSG (message);
	}
// Textures shouldn't be marked modified on first load
CommitTextureChanges ();

DLE.MainFrame ()->Progress ().DestroyWindow ();

return 0;
}

//-----------------------------------------------------------------------------------

uint CTextureManager::WriteCustomTextureHeader (CFileManager& fp, const CTexture *pTexture, uint nId, uint nOffset)
{
	CPigTexture pigTexInfo (Version ());
	ubyte *palIndex, *pSrc;

memset (pigTexInfo.name, 0, sizeof (pigTexInfo.name));
if (*pTexture->Name ())
	// Texture name in pigTexInfo is not null-terminated
	memcpy_s (pigTexInfo.name, sizeof (pigTexInfo.name), pTexture->Name (), strlen (pTexture->Name ()));
else {
	char name [9];
	sprintf_s (name, sizeof (name), "POG%04d", nId);
	memcpy (pigTexInfo.name, name, sizeof (pigTexInfo.name));
	}
pigTexInfo.Setup (Version (), pTexture->Width (), pTexture->Height (), (pTexture->Format () == TGA) ? 0x80 : 0, nOffset);
if (pTexture->MaybeAnimated ()) {
	pigTexInfo.dflags |= BM_DFLAG_ANIMATED | (pTexture->FrameNumber () & BM_DFLAG_ANIMFRAME_MASK);
	}

// check for transparency and super transparency
if (pTexture->Format () == BMP)
	try {
	if (palIndex = pSrc = pTexture->ToBitmap ()) {
		for (uint j = 0, h = pTexture->Size (); j < h; j++, pSrc++) {
			if (*pSrc == 255) 
				pigTexInfo.flags |= BM_FLAG_TRANSPARENT;
			if (*pSrc == 254) 
				pigTexInfo.flags |= BM_FLAG_SUPER_TRANSPARENT;
			if ((pigTexInfo.flags & (BM_FLAG_TRANSPARENT | BM_FLAG_SUPER_TRANSPARENT)) == (BM_FLAG_TRANSPARENT | BM_FLAG_SUPER_TRANSPARENT))
				break;
			}
		delete [] palIndex;
		}
	}
	catch(...) {
#if DBG_ARRAYS
		ArrayError ("invalid buffer size\n");
#endif
		}
pigTexInfo.Write (&fp);
return nOffset + ((pTexture->Format () == TGA) ? pTexture->Size () * sizeof (CBGRA) : pTexture->Size ());
}

//-----------------------------------------------------------------------------------

int CTextureManager::WriteCustomTexture (CFileManager& fp, const CTexture *pTexture, bool bUseBMPFileFormat)
{
if (pTexture->Format () == TGA && DLE.IsD2XLevel ()) {
	tRGBA rgba [16384];
	uint h = 0;
	const CBGRA* pBuffer = pTexture->Buffer (pTexture->Width () * (pTexture->Height () - 1));
	for (uint i = pTexture->Height (); i; i--) {
		for (uint j = pTexture->Width (); j; j--, pBuffer++) {
			rgba [h].r = pBuffer->r;
			rgba [h].g = pBuffer->g;
			rgba [h].b = pBuffer->b;
			rgba [h].a = pBuffer->a;
			if (++h == sizeofa (rgba)) {
				fp.Write (rgba, sizeof (tRGBA), h);
				h = 0;
				}
			}
		pBuffer -= 2 * pTexture->Width ();
		}
	if (h > 0)
		fp.Write (rgba, sizeof (tRGBA), h);
	}
else {
	ubyte* palIndex = pTexture->ToBitmap ();

	if (bUseBMPFileFormat) {
		// The row length of a bitmap when written to file must be a multiple of 4 bytes,
		// but in memory it isn't always, so we need to do this line-by-line
		ubyte* pRow = palIndex;
		ubyte pad [4] = { 0 };
		uint padLength = -(int)pTexture->Width () & 3;
		for (uint i = pTexture->Height (); i; i--) {
			fp.Write (pRow, 1, pTexture->Width ());
			if (padLength > 0)
				fp.Write (pad, 1, padLength);
			pRow += pTexture->Width ();
			}
		}
	else {
		// Writing to POG etc - we can just write the unaligned blob in this case.
		fp.Write (palIndex, 1, pTexture->Size ());
		}

	delete [] palIndex;
	}
return 1;
}

//-----------------------------------------------------------------------------------
// CreatePog ()
//
// Action - Creates a POG fp from all the changed textures
//
// Format:
//   Pig Header
//   Texture Indicies (N total * ushort)
//   Texture Header 0
//     ...
//   Texture Header N
//   Texture 0
//     ...
//   Texture N
//
// where N is the number of textures.
//
// Uncommpressed Texture data:
//   ubyte  data[dataSize];    // raw data
//
// Commpressed Texture data:
//   uint totalSize;      // including this long word and everything below
//   ubyte  lineSizes[64];  // size of each line (in bytes)
//   ubyte  data[dataSize]; // run length encoded (RLE) data
//
// RLE definition:
//
//   If upper 3 bits of ubyte are set, lower 5 bytes represents how many times
//   to repeat the following ubyte.  The last ubyte of the data must be set
//   to 0xE0, which means 0 bytes to follow.  Note: The last ubyte may be
//   omitted since the xsize array defines the length of each line???
//
//-----------------------------------------------------------------------------------

int CTextureManager::CreatePog (CFileManager& fp) 
{
if (!textureManager.Available ())
	return 1;

	CPigHeader		pigFileInfo (1);
	uint				textureCount = 0, nOffset = 0;
	int				nVersion = DLE.FileType ();
	int				nId, i, h = m_header [nVersion].nTextures;
	const CTexture*	pTexture;

if (DLE.IsD1File ()) {
	ErrorMsg ("Descent 1 does not support custom textures.");
	return 0;
	}

paletteManager.ResetCLUT ();
textureCount = m_nTextures [1];

sprintf_s (message, sizeof (message), "%s\\dle_temp.pog", DLE.AppFolder ());

// write file header
pigFileInfo.nId = 0x474f5044L; /* 'DPOG' */
pigFileInfo.nVersion = 1;
pigFileInfo.nTextures = 0;

for (i = 0; i < h; i++) {
	pTexture = TextureByIndex (i);
	if (pTexture->IsCustom ())
		pigFileInfo.nTextures++;
	}
pigFileInfo.Write (fp);

// write list of textures
for (i = 0; i < h; i++) {
	pTexture = TextureByIndex (i);
	if (pTexture->IsCustom ())
		fp.WriteUInt16 (ushort (i + 1));
	}

// write texture headers
nId = 0;
for (i = 0; i < h; i++) {
	pTexture = TextureByIndex (i);
	if (pTexture->IsCustom ())
		nOffset = WriteCustomTextureHeader (fp, pTexture, nId++, nOffset);
	}

sprintf_s (message, sizeof (message)," Pog manager: Saving %d custom textures", pigFileInfo.nTextures);
DEBUGMSG (message);

for (i = 0; i < h; i++) {
	pTexture = TextureByIndex (i);
	if (pTexture->IsCustom ())
		WriteCustomTexture (fp, pTexture);
	}

return 1;
}

//------------------------------------------------------------------------------
// CreateDtx ()
//
// Creates a DTX patch from all the changed textures
//
// Format:
//   PIG Header (D1 format)
//   Texture Header 1 (D1 format)
//     ...
//   Texture Header N (D1 format)
//   Sound Header 1
//     ...
//   Sound Header M
//   Texture/Sound 1
//     ...
//   Texture/Sound N+M
//
// where N is the number of textures and M is the number of sound clips. DTX patches
// do not store texture indices, and instead use texture names (as written in the
// texture header) to determine which texture to override. Sounds are handled in the
// same way - although presently we do not edit these and just carry them over from
// the old file if we found any.
//
// Texture data is stored in the same way as in a POG file. While the DTX file format
// appears to allow textures and sounds to be interleaved, we write textures first,
// then sounds.
//
//-----------------------------------------------------------------------------------

int CTextureManager::CreateDtx (CFileManager & fp, CDtxSoundList & soundList)
{
if (!textureManager.Available ())
	return 1;

	CPigHeader		pigFileInfo (0);
	uint				nOffset = 0;
	int				nVersion = DLE.FileType ();
	int				nId, i, h = m_header [nVersion].nTextures;
	const CTexture*	pTexture;

if (DLE.IsD2File ()) {
	ErrorMsg ("DLE does not support saving DTX patches for Descent 2 levels. Use a POG file instead.");
	return 0;
	}

paletteManager.ResetCLUT ();

sprintf_s (message, sizeof (message), "%s\\dle_temp.dtx", DLE.AppFolder ());

// write file header
pigFileInfo.nTextures = 0;
for (i = 0; i < h; i++) {
	pTexture = TextureByIndex (i);
	if (pTexture->IsCustom ())
		pigFileInfo.nTextures++;
	}
pigFileInfo.nSounds = soundList.Count ();
pigFileInfo.Write (fp);

sprintf_s (message, sizeof (message)," Pog manager: Saving %d custom textures", pigFileInfo.nTextures);
DEBUGMSG (message);

// write texture headers
nId = 0;
for (i = 0; i < h; i++) {
	pTexture = TextureByIndex (i);
	if (pTexture->IsCustom ())
		nOffset = WriteCustomTextureHeader (fp, pTexture, nId++, nOffset);
	}

// write sound headers
for (i = 0; i < soundList.Count (); i++)
	nOffset = soundList.WriteHeader (fp, i, nOffset);

// write texture data
for (i = 0; i < h; i++) {
	pTexture = TextureByIndex (i);
	if (pTexture->IsCustom ())
		WriteCustomTexture (fp, pTexture);
	}

// write sound data
for (i = 0; i < soundList.Count (); i++)
	soundList.WriteSoundClip (fp, i);

return 1;
}

//------------------------------------------------------------------------------

void CTextureManager::ReadMod (char* pszFolder)
{
	char szFile [256];
	int  h = MaxTextures ();

for (int i = 0; i < h; i++) {
	DLE.MainFrame ()->Progress ().StepIt ();
	const CTexture* pTexture = Textures (i);
#ifdef _DEBUG
	if (i == nDbgTexture)
		nDbgTexture = nDbgTexture;
#endif
	if (pTexture->IsCustom ())
		continue;
	sprintf (szFile, "%s\\%s%s.tga", pszFolder, pTexture->Name (), pTexture->IsAnimated () ? "#0" : "");
	CTexture *pNewTexture = new CTexture ();
	if (!pNewTexture)
		ErrorMsg ("Not enough memory for texture.");
	else if (pNewTexture->Copy (*pTexture) && pNewTexture->LoadFromFile (szFile, false)) {
		OverrideTexture (pTexture->Index (), pNewTexture);
		pNewTexture->SetCustom ();
		}
	else
		delete pNewTexture;
	}
}

//------------------------------------------------------------------------------

void CTextureManager::LoadMod (void)
{
if (DLE.MakeModFolders ("textures")) {
	DLE.MainFrame ()->InitProgress (2 * MaxTextures ());
	// first read the level specific textures
	ReadMod (DLE.m_modFolders [0]);
	// then read the mission wide textures
	ReadMod (DLE.m_modFolders [1]);
	DLE.MainFrame ()->Progress ().DestroyWindow ();
	}
}

//------------------------------------------------------------------------------

CDtxSoundList::~CDtxSoundList ()
{
for (uint i = 0; i < m_sounds.Length (); i++)
	free (m_sounds [i]);
}

//------------------------------------------------------------------------------

void CDtxSoundList::Load (CFileManager& fp, long nFileSize)
{
	CPigHeader pigFileInfo (0);
	uint hdrOffset, dataOffset, hdrSize;

// read file header
pigFileInfo.Read (fp);
hdrOffset = fp.Tell ();
hdrSize = pigFileInfo.nTextures * sizeof (PIG_TEXTURE_D1) + pigFileInfo.nSounds * sizeof (PIG_SOUND);
dataOffset = hdrOffset + hdrSize;

m_headers.Resize (pigFileInfo.nSounds, false);
m_sounds.Resize (pigFileInfo.nSounds, false);

// read sound headers
fp.Seek (hdrOffset + pigFileInfo.nTextures * sizeof (PIG_TEXTURE_D1), SEEK_SET);
for (int i = 0; i < pigFileInfo.nSounds; i++) {
	// get texture data offset from texture header
	fp.ReadBytes (&m_headers [i], sizeof (PIG_SOUND));
	}

// read sound data
for (int i = 0; i < pigFileInfo.nSounds; i++) {
	if ((long) (hdrSize + m_headers [i].offset + m_headers [i].length) > nFileSize)
		continue;
	
	m_sounds [i] = malloc (m_headers [i].length);
	fp.Seek (dataOffset + m_headers [i].offset, SEEK_SET);
	fp.ReadBytes (m_sounds [i], m_headers [i].length);
	}
}

//------------------------------------------------------------------------------

uint CDtxSoundList::WriteHeader (CFileManager& fp, uint nItem, uint nOffset)
{
// update clip offset, it might have moved
m_headers [nItem].offset = nOffset;

fp.WriteBytes (&m_headers [nItem], sizeof (PIG_SOUND));
return nOffset + m_headers [nItem].length;
}

//------------------------------------------------------------------------------

void CDtxSoundList::WriteSoundClip (CFileManager& fp, uint nItem)
{
fp.WriteBytes (m_sounds [nItem], m_headers [nItem].length);
}

//------------------------------------------------------------------------------
//eof texturemanager.custom.cpp