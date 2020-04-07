#pragma once

class HogDialog : public CDialog
{
public:
	HogDialog(CWnd* pParentWnd = null, LPCSTR pszFile = null, LPCSTR pszSubFile = null);
	void Setup(LPCSTR pszFile, LPCSTR pszSubFile);

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	void EndDialog(int nResult);
	void OnOK();
	void OnCancel();
	afx_msg void OnRename();
	afx_msg void OnDelete();
	afx_msg void OnImport();
	afx_msg void OnExport();
	afx_msg void OnFilter();
	afx_msg void OnSelectedFilesChanged();

	inline CListBox* LBFiles() { return (CListBox*)GetDlgItem(IDC_HOG_FILES); }
	const char* MissionName();
	inline char* LevelName() { return m_szSubFile; }

	int FindFilename(LPSTR pszName);
	void ClearFileList(void);
	int AddFile(LPCSTR pszName, long size, long offset, int fileno);
	int DeleteFile(int index);
	int GetFileData(int index = -1, long* size = null, long* offset = null);
	int AddFileData(int index, long size, long offset, int fileno);
	void UpdateListBoxItem(int index);
	int GetSingleSelectedIndex(void);
	void GetSelectedIndices(CArray <int>& selectedIndices);
	bool ReadHogData(void);

private:
	bool m_bInited;
	char m_name[256];
	char m_szSubFile[256];
	int m_bShowAll;

	void EnableControls();

	DECLARE_MESSAGE_MAP()
};

class CInputDialog : public CDialog
{
public:
	LPCSTR m_pszTitle;
	LPCSTR m_pszPrompt;
	LPSTR m_pszBuf;
	size_t m_nBufSize;

	CInputDialog(CWnd* pParentWnd = null, LPCSTR pszTitle = null, LPCSTR pszPrompt = null, LPSTR pszBuf = null, size_t nBufSize = 0);
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	void OnOK();
};

bool BrowseForFile(BOOL bOpen, LPSTR pszDefExt, LPSTR pszFile, LPSTR pszFilter, DWORD nFlags = 0, CWnd* pParentWnd = null);
void RefreshHogFileList(CListBox* plb);
