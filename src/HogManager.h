#ifndef __hogman_h
#define __hogman_h

#include "dle-xp-res.h"
#include "MemoryFile.h"

#define MAX_HOGFILES	1000

//------------------------------------------------------------------------

class CInputDialog : public CDialog {
	public:
		LPSTR		m_pszTitle;
		LPSTR		m_pszPrompt;
		LPSTR		m_pszBuf;
		size_t	m_nBufSize;

		CInputDialog (CWnd *pParentWnd = null, LPSTR pszTitle = null, LPSTR pszPrompt = null, LPSTR pszBuf = null, size_t nBufSize = 0);
		virtual BOOL OnInitDialog (void);
		virtual void DoDataExchange (CDataExchange * pDX);
		void OnOK (void);
};

//------------------------------------------------------------------------

typedef struct tHogFileData {
	long	m_offs;
	long	m_size;
	int	m_fileno;
} tHogFileData;

class CHogManager : public CDialog {
	public:
		bool				m_bInited;
		char				m_name [256];
		char				m_szFile [256];
		char				m_szSubFile [256];
		int				m_bExtended;
		//int				m_type;
		//int				*m_pType;
		int				m_bShowAll;
		CMemoryFile		m_level;
		//CLevelHeaderFactory	m_headerFactory;
//		tHogFileData	m_fileData;

		CHogManager (CWnd *pParentWnd = null, LPCSTR pszFile = null, LPCSTR pszSubFile = null);
		void Setup (LPCSTR pszFile, LPCSTR pszSubFile);
		virtual BOOL OnInitDialog (void);
		virtual void DoDataExchange (CDataExchange * pDX);
		void EndDialog (int nResult);
		int FindFilename (LPSTR pszName);
		void ClearFileList (void);
		int AddFile (LPSTR pszName, long length, long size, long offset, int fileno);
		int DeleteFile (int index);
		int GetFileData (int index = -1, long *size = null, long *offset = null);
		int AddFileData (int index, long size, long offset, int fileno);
		int GetSingleSelectedIndex (void);
		void GetSelectedIndices (CArray <int> &selectedIndices);
		bool LoadLevel (LPSTR pszFile = null, LPSTR pszSubFile = null);
		void OnOK (void);
		void OnCancel (void);
		bool ReadHogData (void);
		void Reset (void);

		long FindSubFile (CFileManager& fp, const char* pszFile, const char* pszSubFile, const char* pszExt);
		int ReadSignature (CFileManager* fp, bool bVerbose = true);
		int ReadData (LPSTR pszFile, CListBox *plb, bool bAllFiles, bool bOnlyLevels, bool bGetFileData = true);

		inline char* MissionName (void) { return m_szFile; }
		inline char* LevelName (void) { return m_szSubFile; }

		//inline CBaseLevelHeader* CreateHeader (int nType = -1) { return m_headerFactory.Create ((nType < 0) ? m_nType : nType); }

		afx_msg void OnRename ();
		afx_msg void OnDelete ();
		afx_msg void OnImport ();
		afx_msg void OnExport ();
		afx_msg void OnFilter ();
		afx_msg void OnSelectedFilesChanged ();
		inline CListBox *LBFiles () { return (CListBox *) GetDlgItem (IDC_HOG_FILES); }

	private:
		void Rename (CFileManager& fp, int index, char* szNewName);
		void EnableControls (void);

	DECLARE_MESSAGE_MAP ()
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

bool BrowseForFile (BOOL bOpen, LPSTR pszDefExt, LPSTR pszFile, LPSTR pszFilter, DWORD nFlags = 0, CWnd *pParentWnd = null);
int SaveToHog (LPSTR szHogFile, LPSTR szSubFile, bool bSaveAs);
bool FindFileData (const char* pszFile, char* pszSubFile, CLevelHeader& lh, long& nSize, long& nPos, BOOL bVerbose = TRUE, CFileManager* fp = null);
bool ExportSubFile (const char *pszSrc, const char *pszDest, long offset, long size);
int ReadMissionFile (char *pszFile);
int WriteMissionFile (char *pszFile, int levelVersion, bool bSaveAs = true);
int MakeMissionFile (char *pszFile, char *pszSubFile, int bCustomTextures, int bCustomRobots, bool bSaveAs = true);
bool DoesSubFileExist (const char* pszFile, char* pszSubFile);
const char *GetCustomFileExtension (const int nType);
int WriteCustomFile (const char* pszFile, char* pszSubFile, const int nType);

//------------------------------------------------------------------------

#endif //__hogman_h
