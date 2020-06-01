#ifndef __hogman_h
#define __hogman_h

#define MAX_HOGFILES	1000

//------------------------------------------------------------------------

typedef struct tHogFileData {
	long m_offs;
	long m_size;
	std::string m_fileName;
	int m_fileno; // this is the index in m_fileList, which may be filtered - don't use it for writing HOGs
} tHogFileData;

class CHogManager {
	public:
		CHogManager ();

		bool LoadLevel(LPSTR pszFile = null, LPSTR pszSubFile = null);
		CMemoryFile* GetLevel();
		const std::vector<tHogFileData>& GetFileList();
		tHogFileData GetFileInfoAtOffset(long fileOffset);
		long FindSubFile(CFileManager& fp, const char* pszFile, const char* pszSubFile, const char* pszExt);
		int ReadSignature(CFileManager* fp, bool bVerbose = true);
		int ReadData(LPCSTR pszFile, bool bAllFiles, bool bOnlyLevels, bool bGetFileData = true);
		void WriteHogHeader(CFileManager& fp);
		void RenameFile(const char* oldFileName, const char* newFileName);

		inline char* MissionName(void) { return m_szFile; }
		void SetMissionName(const char* newFileName);
		inline char* LevelFileName(void) { return m_levelFileName; }

		static bool ContainsSubFile(const char* fileName, const char* subFileName)
		{
			// TODO Implement properly
			return true;
		}

	private:
		char m_szFile[256];
		int m_bExtended;
		CMemoryFile m_level;
		char m_levelFileName[256];
		std::vector<tHogFileData> m_fileList;
	};

extern CHogManager* hogManager;

//------------------------------------------------------------------------

#define CUSTOM_FILETYPE_LIGHTMAP 0
#define CUSTOM_FILETYPE_COLORMAP 1
#define CUSTOM_FILETYPE_PALETTE  2
#define CUSTOM_FILETYPE_POG      3
#define CUSTOM_FILETYPE_HXM      4
#define CUSTOM_FILETYPE_DTX      5
#define NUM_CUSTOM_FILETYPES (1+CUSTOM_FILETYPE_DTX)

int SaveToHog (LPSTR szHogFile, LPSTR szSubFile, bool newLevel, bool bSaveAs, bool overwrite);
bool FindFileData (const char* pszFile, const char* pszSubFile, CLevelHeader& lh, long& nSize, long& nPos, BOOL bVerbose = TRUE, CFileManager* fp = null);
bool ExportSubFile (const char *pszSrc, const char *pszDest, long offset, long size);
int ReadMissionFile (char *pszFile);
int WriteMissionFile (char *pszFile, int levelVersion);
int MakeMissionFile (char *pszFile, char *pszSubFile, int bCustomTextures, int bCustomRobots, bool bSaveAs = true);
bool DoesSubFileExist (const char* pszFile, const char* pszSubFile);
const char *GetCustomFileExtension (const int nType);
int WriteCustomFile (const char* pszFile, char* pszSubFile, const int nType);
void DeleteSubFile(CFileManager& fp, long size, long offset, int numEntries, int deleteIndex);

//------------------------------------------------------------------------

#endif //__hogman_h
