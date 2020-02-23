#ifndef __mineinfo_h
#define __mineinfo_h

#include "define.h"

# pragma pack(push, packing)
# pragma pack(1)

// -----------------------------------------------------------------------------

class CMineFileInfo {
public:
	ushort  signature;
	ushort  version;
	int		size;

	void Read (CFileManager* fp);
	void Write (CFileManager* fp);
};

// -----------------------------------------------------------------------------

class CPlayerItemInfo {
public:
	int	 offset;
	int  size;

	CPlayerItemInfo () { offset = -1, size = 0; }
	void Read (CFileManager* fp);
	void Write (CFileManager* fp);
};

// -----------------------------------------------------------------------------

class CMineItemInfo {
public:
	int	offset;
	int	count;
	int	size;

	CMineItemInfo () { Reset (); }

	void Reset (void) { offset = -1, count = size = 0; } 
	bool Setup (CFileManager* fp);
	bool Restore (CFileManager* fp);
	void Read (CFileManager* fp);
	void Write (CFileManager* fp);
};

// -----------------------------------------------------------------------------

class CMineInfo {
public:
	CMineFileInfo		fileInfo;
	char					mineFilename [15];
	int					level;
	CPlayerItemInfo	player;

	void Read (CFileManager* fp, bool bIsD2XLevel);
	void Write (CFileManager* fp, bool bIsD2XLevel);
};

// -----------------------------------------------------------------------------

class CMineData {
	public:
		CMineInfo			m_info;
};

// -----------------------------------------------------------------------------

#endif // __mineinfo_h

