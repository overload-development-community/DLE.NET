
#include "mine.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void CMineFileInfo::Read (CFileManager* fp) 
{
signature = fp->ReadInt16 ();
version = fp->ReadInt16 ();
size = fp->ReadInt32 ();
}

// -----------------------------------------------------------------------------

void CMineFileInfo::Write (CFileManager* fp) 
{
fp->Write (signature);
fp->Write (version);
fp->Write (size);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void CPlayerItemInfo::Read (CFileManager* fp) 
{
offset = fp->ReadInt32 ();
size  = fp->ReadInt32 ();
}

// -----------------------------------------------------------------------------

void CPlayerItemInfo::Write (CFileManager* fp) 
{
fp->Write (offset);
fp->Write (size);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void CMineItemInfo::Read (CFileManager* fp) 
{
offset = fp->ReadInt32 ();
count = fp->ReadInt32 ();
size  = fp->ReadInt32 ();
}

// -----------------------------------------------------------------------------

void CMineItemInfo::Write (CFileManager* fp) 
{
if (count == 0)
	offset = -1;
fp->Write (offset);
fp->Write (count);
fp->Write (size);
}

// -----------------------------------------------------------------------------

bool CMineItemInfo::Setup (CFileManager* fp) 
{
offset = (count == 0) ? -1 : fp->Tell ();
return offset >= 0;
}

// -----------------------------------------------------------------------------

bool CMineItemInfo::Restore (CFileManager* fp) 
{
if (offset < 0) {
	count = 0;
	return false;
	}
fp->Seek (offset);
return true;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void CMineInfo::Read (CFileManager* fp, bool bIsD2XLevel) 
{
fileInfo.Read (fp);
fp->Read (mineFilename, 1, sizeof (mineFilename));
level = fp->ReadInt32 ();
player.Read (fp);
objectManager.ReadInfo (fp);
wallManager.ReadInfo (fp);
triggerManager.ReadInfo (fp);
CMineItemInfo links; // unused
links.Read (fp);
triggerManager.ReadReactorInfo (fp);
segmentManager.ReadRobotMakerInfo (fp);
if (fileInfo.version >= 29)
	lightManager.ReadLightDeltaInfo (fp);
if (bIsD2XLevel) {
	if (theMine->LevelVersion () > 15)
		segmentManager.ReadEquipMakerInfo (fp);
	if (theMine->LevelVersion () > 26)
		segmentManager.ReadFogInfo (fp);
	}
}

// -----------------------------------------------------------------------------

void CMineInfo::Write (CFileManager* fp, bool bIsD2XLevel) 
{
	long startPos = fp->Tell ();

fileInfo.Write (fp);
fp->Write (mineFilename, 1, sizeof (mineFilename));
fp->Write (level);
player.Write (fp);
objectManager.WriteInfo (fp);
wallManager.WriteInfo (fp);
triggerManager.WriteInfo (fp);
CMineItemInfo links; // unused
links.Write (fp);
triggerManager.WriteReactorInfo (fp);
segmentManager.WriteRobotMakerInfo (fp);
if (fileInfo.version >= 29)
	lightManager.WriteLightDeltaInfo (fp);
if (bIsD2XLevel) {
	segmentManager.WriteEquipMakerInfo (fp);
	segmentManager.WriteFogInfo (fp);
	}
if (fileInfo.size < 0) {
	fileInfo.size = fp->Tell () - startPos;
	long endPos = fp->Tell ();
	fp->Seek (startPos);
	fileInfo.Write (fp);
	fp->Seek (endPos);
	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void CFogInfo::Init (int nType)
{
if (nType == 0) {
	m_color.r = ubyte (255.0f * 0.2f);
	m_color.g = ubyte (255.0f * 0.4f);
	m_color.b = ubyte (255.0f * 0.6f);
	m_density = 11;
	}
else if (nType == 1) {
	m_color.r = ubyte (255.0f * 0.8f);
	m_color.g = ubyte (255.0f * 0.4f);
	m_color.b = ubyte (255.0f * 0.0f);
	m_density = 2;
	}
else {
	m_color.r = m_color.g = m_color.b = ubyte (255.0f * 0.7f);
	m_density = (nType == 3) ? 4 : 11;
	}
}

// -----------------------------------------------------------------------------

void CFogInfo::Read (CFileManager *fp)
{
m_color.r = (ubyte) fp->ReadChar ();
m_color.g = (ubyte) fp->ReadChar ();
m_color.b = (ubyte) fp->ReadChar ();
m_density = (ubyte) fp->ReadChar ();
}

// -----------------------------------------------------------------------------

void CFogInfo::Write (CFileManager *fp)
{
fp->WriteChar (char (m_color.r));
fp->WriteChar (char (m_color.g));
fp->WriteChar (char (m_color.b));
fp->WriteChar (char (m_density));
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------


