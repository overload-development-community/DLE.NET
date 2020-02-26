#ifndef __mineinfo_h
#define __mineinfo_h

#pragma pack(push, packing)
#pragma pack(1)

// -----------------------------------------------------------------------------

class CMineFileInfo {
public:
	ushort  signature;
	ushort  version;
	int		size;

	void Read (IFileManager* fp);
	void Write (IFileManager* fp);
};

// -----------------------------------------------------------------------------

class CPlayerItemInfo {
public:
	int	 offset;
	int  size;

	CPlayerItemInfo () { offset = -1, size = 0; }
	void Read (IFileManager* fp);
	void Write (IFileManager* fp);
};

// -----------------------------------------------------------------------------

class CMineItemInfo {
public:
	int	offset;
	int	count;
	int	size;

	CMineItemInfo () { Reset (); }

	void Reset (void) { offset = -1, count = size = 0; } 
	bool Setup (IFileManager* fp);
	bool Restore (IFileManager* fp);
	void Read (IFileManager* fp);
	void Write (IFileManager* fp);
};

// -----------------------------------------------------------------------------

class CMineInfo {
public:
	CMineFileInfo		fileInfo;
	char					mineFilename [15];
	int					level;
	CPlayerItemInfo	player;

	void Read (IFileManager* fp, bool bIsD2XLevel);
	void Write (IFileManager* fp, bool bIsD2XLevel);
};

// -----------------------------------------------------------------------------

class CMineData {
	public:
		CMineInfo			m_info;
};

// -----------------------------------------------------------------------------

class CFogInfo {
public:
	rgbColor			m_color;
	ubyte				m_density;

	void Init(int nType);
	void Read(IFileManager* fp);
	void Write(IFileManager* fp);
};

// -----------------------------------------------------------------------------

#pragma pack(pop, packing)

#endif // __mineinfo_h

