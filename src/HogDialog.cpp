#include "stdafx.h"
#include "dle-res.h"
#include "HogDialog.h"

CInputDialog::CInputDialog(CWnd* pParentWnd, LPSTR pszTitle, LPSTR pszPrompt, LPSTR pszBuf, size_t nBufSize)
	: CDialog(IDD_INPDLG, pParentWnd)
{
	m_pszTitle = pszTitle;
	m_pszPrompt = pszPrompt;
	m_pszBuf = pszBuf;
	m_nBufSize = nBufSize;
}

BOOL CInputDialog::OnInitDialog(void)
{
	CDialog::OnInitDialog();
	SetWindowText(m_pszTitle);
	return TRUE;
}

void CInputDialog::DoDataExchange(CDataExchange* pDX)
{
	// m_pszPrompt can point to a string literal, and writing to those causes crashes
	// using some compiler settings, so this is read-only
	if (!pDX->m_bSaveAndValidate)
		DDX_Text(pDX, IDC_INPDLG_PROMPT, m_pszPrompt, int(strlen(m_pszPrompt)));
	DDX_Text(pDX, IDC_INPDLG_BUF, m_pszBuf, int(m_nBufSize));
}

void CInputDialog::OnOK(void)
{
	UpdateData(TRUE);
	CDialog::OnOK();
}

bool BrowseForFile(BOOL bOpen, LPSTR pszDefExt, LPSTR pszFile, LPSTR pszFilter, DWORD nFlags, CWnd* pParentWnd)
{
	INT_PTR nResult;
	char szFile[256], szFolder[256], * ps;

	if (*pszFile)
		strcpy_s(szFolder, sizeof(szFolder), pszFile);
	else {
		strcpy_s(szFolder, sizeof(szFolder), missionFolder);
		if ((ps = strstr(szFolder, "\\data")))
			ps[1] = '\0';
		if (!*pszDefExt)
			strcat_s(szFolder, sizeof(szFolder), "missions\\*.*");
		else {
			strcat_s(szFolder, sizeof(szFolder), "missions\\*.");
			strcat_s(szFolder, sizeof(szFolder), pszDefExt);
			if ((ps = strchr(szFolder, ';')))
				*ps = '\0';
		}
	}
	if (!(ps = strrchr(szFolder, '\\')))
		strcpy_s(szFile, sizeof(szFile), szFolder);
	else {
		ps[0] = '\0';
		strcpy_s(szFile, sizeof(szFile), ps + 1);
	}
	CFileDialog d(bOpen, pszDefExt, szFile, nFlags, pszFilter, pParentWnd);
	//d.m_ofn.hInstance = AfxGetInstanceHandle ();
	//d.GetOFN ().lpstrInitialDir = szFolder;
	d.m_ofn.lpstrInitialDir = szFolder;
	if ((nResult = d.DoModal()) != IDOK)
		return false;
	strcpy_s(pszFile, 256, d.GetPathName());
	return true;
}

BEGIN_MESSAGE_MAP(HogDialog, CDialog)
	ON_BN_CLICKED(IDC_HOG_RENAME, OnRename)
	ON_BN_CLICKED(IDC_HOG_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_HOG_IMPORT, OnImport)
	ON_BN_CLICKED(IDC_HOG_EXPORT, OnExport)
	ON_BN_CLICKED(IDC_HOG_FILTER, OnFilter)
	ON_LBN_SELCHANGE(IDC_HOG_FILES, OnSelectedFilesChanged)
	ON_LBN_DBLCLK(IDC_HOG_FILES, OnOK)
END_MESSAGE_MAP()

HogDialog::HogDialog(CWnd* pParentWnd, LPCSTR pszFile, LPCSTR pszSubFile)
	: CDialog(IDD_HOGMANAGER, pParentWnd)
{
	Setup(pszFile, pszSubFile);
}

void HogDialog::Setup(LPCSTR pszFile, LPCSTR pszSubFile)
{
	m_bInited = false;
	if (pszFile)
		hogManager->SetMissionName(pszFile);
	if (pszSubFile)
		strcpy_s(m_szSubFile, sizeof(m_szSubFile), pszSubFile);
	else
		memset(m_szSubFile, 0, sizeof(m_szSubFile));
	m_bShowAll = false;
}

BOOL HogDialog::OnInitDialog(void)
{
	CDialog::OnInitDialog();
	EnableControls();
	if (ReadHogData()) {
		m_bInited = true;
		return TRUE;
	}
	EndDialog(0);
	return FALSE;
}

void HogDialog::DoDataExchange(CDataExchange* pDX)
{
	long size, offset;
	CString blankText("");
	CString multipleOffset("(multiple)");

	if (LBFiles()->GetSelCount() == 0) {
		DDX_Text(pDX, IDC_HOG_SIZE, blankText);
		DDX_Text(pDX, IDC_HOG_OFFSET, blankText);
	}
	else if (LBFiles()->GetSelCount() > 1) {
		CArray <int> selItems;
		long thisSize;
		size = 0;
		GetSelectedIndices(selItems);
		for (int i = 0; i < selItems.GetCount(); i++) {
			GetFileData(selItems[i], &thisSize, &offset);
			size += thisSize;
		}
		DDX_Text(pDX, IDC_HOG_SIZE, size);
		DDX_Text(pDX, IDC_HOG_OFFSET, multipleOffset);
	}
	else {
		GetFileData(-1, &size, &offset);
		DDX_Text(pDX, IDC_HOG_SIZE, size);
		DDX_Text(pDX, IDC_HOG_OFFSET, offset);
	}
	DDX_Check(pDX, IDC_HOG_FILTER, m_bShowAll);
}

void HogDialog::EndDialog(int nResult)
{
	if (m_bInited)
		ClearFileList();
	CDialog::EndDialog(nResult);
}

void HogDialog::OnOK()
{
	if (LBFiles()->GetSelCount() != 1)
		return;
	LBFiles()->GetText(GetSingleSelectedIndex(), m_szSubFile);
	char* pszExt = strrchr((char*)m_szSubFile, '.');
	if (pszExt && _strcmpi(pszExt, ".rdl") && _strcmpi(pszExt, ".rl2")) {
		ErrorMsg("DLE cannot process this file. To change the file,\n\n"
			"export it and process it with the appropriate application.\n"
			"To incorporate the changes in the HOG file,\n"
			"import the modified file back into the HOG file.");
		return;
	}

	hogManager->LoadLevel(nullptr, m_szSubFile);
	CDialog::OnOK();
}

void HogDialog::OnCancel(void)
{
	CDialog::OnCancel();
}

void HogDialog::OnRename(void)
{
	char szOldName[256], szNewName[256];
	int index = GetSingleSelectedIndex();

	if (index < 0)
		return;
	LBFiles()->GetText(index, szOldName);
	strcpy_s(szNewName, sizeof(szNewName), szOldName);
	CInputDialog dlg(this, "Rename file", "Enter new name:", (char*)szNewName, sizeof(szNewName));
	if (dlg.DoModal() != IDOK)
		return;
	if (FindFilename(szNewName) >= 0) {
		ErrorMsg("A file with that name already exists\nin the HOG file.");
		return;
	}

	hogManager->RenameFile(szOldName, szNewName);
	UpdateListBoxItem(index);

	// if renamed file was a level file, rename all auxiliary files belonging to that level, too
	char* p = strstr(szOldName, ".rl2");
	if (p != null) {
		static char* subFileExts[] = { ".pal", ".lgt", ".clr", ".pog", ".hxm" };
		CLevelHeader lh(DLE.IsD2XFile());
		int nameSize = lh.NameSize() - 5;

		for (int i = 0; i < ARRAYSIZE(subFileExts); i++) {
			std::string subFileName(szOldName, p - szOldName);
			subFileName.append(subFileExts[i]);
			if (DoesSubFileExist(MissionName(), subFileName.c_str())) {
				index = LBFiles()->FindStringExact(-1, subFileName.c_str());
				if (index >= 0) {
					if ((p = strchr(szNewName, '.'))) {
						if (p - szNewName > nameSize)
							p = szNewName + nameSize;
						memcpy(p, subFileExts[i], sizeof(subFileExts[i]));
						hogManager->RenameFile(subFileName.c_str(), szNewName);
						UpdateListBoxItem(index);
					}
				}
			}
		}
		p = strchr(szNewName, '.');
		memcpy(p, ".rl2", 5);
		index = LBFiles()->FindStringExact(-1, szNewName);
		if (index >= 0)
			LBFiles()->SetCurSel(index);
	}
}

//------------------------------------------------------------------------------
// CHogManager - ImportMsg
//
// Adds fp to the end of the list
//------------------------------------------------------------------------------

void HogDialog::OnImport(void)
{
	long offset;
	CLevelHeader lh(DLE.IsD2XFile());
	char szOfnFileBuf[512] = { 0 }; // buffer for ofn dialog, can be multiple files
	char szFilePath[256] = { 0 }; // buffer for fp name
	char szShortFileName[13] = { 0 }; // buffer for 8.3 filename
	char* pszFile = null;
	OPENFILENAME ofn;

	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = GetSafeHwnd();
	ofn.lpstrFilter = "Descent Level\0*.rdl\0"
		"Descent 2 Level\0*.rl2\0"
		"Texture file\0*.pog\0"
		"Robot file\0*.hxm\0"
		"Lightmap file\0*.lgt\0"
		"Color file\0*.clr\0"
		"Palette file\0*.pal\0"
		"All Files\0*.*\0";
	if (DLE.IsD1File()) {
		ofn.nFilterIndex = 1;
		ofn.lpstrDefExt = "rdl";
	}
	else {
		ofn.nFilterIndex = 2;
		ofn.lpstrDefExt = "rl2";
	}
	ofn.lpstrFile = szOfnFileBuf;
	ofn.nMaxFile = sizeof(szOfnFileBuf);
	ofn.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;
	if (!GetOpenFileName(&ofn))
		return;

	if (!(GetFileAttributes(szOfnFileBuf) & FILE_ATTRIBUTE_DIRECTORY))
		// only one file selected, direct copy
		strncpy_s(szFilePath, sizeof(szFilePath), szOfnFileBuf, _TRUNCATE);

	CFileManager fDest;
	if (!fDest.Open(MissionName(), "ab")) {
		ErrorMsg("Could not open destination HOG file for import.");
		return;
	}

	CFileManager fSrc;
	do {
		// If we didn't already get a filename, we had multiple files selected.
		// OpenFileDialog crams these into the same buffer (szOfnFileBuf) separated
		// by nulls - directory first, then each filename. We have to parse this
		// to extract all the filenames.
		if (!pszFile && !*szFilePath)
			// get first filename
			pszFile = szOfnFileBuf + strlen(szOfnFileBuf) + 1;
		if (pszFile >= szOfnFileBuf + sizeof(szOfnFileBuf)) {
			ErrorMsg("Source file directory path too long.");
			return;
		}
		if (!*szFilePath)
			// multiple files, have to construct the next path
			sprintf_s(szFilePath, sizeof(szFilePath), "%s\\%s", szOfnFileBuf, pszFile);

		if (!fSrc.Open(szFilePath, "rb")) {
			ErrorMsg("Could not open source file for import.");
			return;
		}
		fDest.Seek(0, SEEK_END);
		offset = fDest.Tell();
		// shorten filename to 8.3 if necessary - this will be the name used in the hog
		DWORD dwResult = GetShortPathName(PathFindFileName(szFilePath),
			szShortFileName, ARRAYSIZE(szShortFileName));
		if (!dwResult || dwResult > ARRAYSIZE(szShortFileName)) {
			// couldn't create the short filename using API - just truncate it
			strncpy_s(szShortFileName, ARRAYSIZE(szShortFileName), PathFindFileName(szFilePath), _TRUNCATE);
		}
		// notify user if the filename was shortened
		if (strcmp(szShortFileName, PathFindFileName(szFilePath)) != 0)
			ErrorMsg("Files imported into HOG files must have filenames\n"
				"in 8.3 format. The filename will be shortened.");
		// write header
		lh.SetFileSize(fSrc.Length());
		strcpy_s(lh.Name(), lh.NameSize(), szShortFileName);
		_strlwr_s(lh.Name(), lh.NameSize());
		lh.Write(&fDest);

		// write data (from source to HOG)
		while (!fSrc.EoF()) {
			std::vector<ubyte> dataBuf(65536);
			size_t nBytes = fSrc.Read(dataBuf.data(), 1, dataBuf.size());
			if (nBytes <= 0)
				break;
			fDest.Write(dataBuf.data(), 1, (int)nBytes);
		}
		fSrc.Close();

		// update list boxes
		AddFile(lh.Name(), lh.FileSize(), offset, LBFiles()->GetCount());

		// get next file path if any
		if (pszFile) {
			pszFile += strlen(pszFile) + 1;
			if (pszFile >= szOfnFileBuf + sizeof(szOfnFileBuf))
				pszFile = null;
			memset(szFilePath, 0, sizeof(szFilePath));
		}
	} while (pszFile && *pszFile);

	fDest.Close();
}

//------------------------------------------------------------------------------
// CHogManager - ExportMsg
//
// Exports selected item to a fp
//------------------------------------------------------------------------------

void HogDialog::OnExport(void)
{
	char szFile[256] = "\0"; // buffer for fp name
	CArray <int> indices;
	CLevelHeader lh;
	long size, offset;

	// make sure there is an item selected
	if (LBFiles()->GetSelCount() < 1) {
		ErrorMsg("Please select a file to export.");
		return;
	}

	GetSelectedIndices(indices);
	if (indices.GetCount() > 1) {
		// Save all selected files to a directory, keeping their current names
		char szPath[MAX_PATH] = { 0 };
		BROWSEINFO bi = { 0 };
		bi.hwndOwner = m_hWnd;
		bi.pszDisplayName = szPath;
		bi.lpszTitle = "Choose a location to export the files to:";
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
		PIDLIST_ABSOLUTE pidlFolder = SHBrowseForFolder(&bi);
		if (!pidlFolder)
			return;
		BOOL bSuccess = SHGetPathFromIDList(pidlFolder, szPath);
		CoTaskMemFree(pidlFolder);
		if (!bSuccess)
			return;

		for (int i = 0; i < indices.GetCount(); i++) {
			LBFiles()->GetText(indices[i], lh.Name());
			GetFileData(indices[i], &size, &offset);
			sprintf_s(szFile, sizeof(szFile), "%s\\%s", szPath, lh.Name());
			ExportSubFile(MissionName(), szFile, offset, size);
		}
	}
	else {
		// get item name, size, and offset
		LBFiles()->GetText(indices[0], lh.Name());
		GetFileData(indices[0], &size, &offset);
		strcpy_s(szFile, sizeof(szFile), lh.Name());
		if (!BrowseForFile(FALSE, "", szFile, "All Files|*.*||",
			OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT,
			DLE.MainFrame()))
			return;
		ExportSubFile(MissionName(), szFile, offset, size);
	}
}

//------------------------------------------------------------------------------
// CHogManager - DeleteMsg
//
// Deletes an item from a HOG file using the following steps:
//
// 1) Creates a new HOG file which does not contain the fp selected
// 2) Deletes original HOG file
// 3) Renames new fp to original fp's name
//
//------------------------------------------------------------------------------

void HogDialog::OnDelete(void)
{
	CListBox* plb = LBFiles();
	CArray <int> deleteIndices;
	long size;
	long offset;
	int fileno;

	// make sure there is an item selected
	GetSelectedIndices(deleteIndices);
	if (deleteIndices.GetCount() < 1) {
		ErrorMsg("Please choose a file to delete.");
		return;
	}

	if (!DLE.ExpertMode()) {
		if (deleteIndices.GetCount() == 1) {
			char levelName[256];
			plb->GetText(deleteIndices[0], levelName);
			sprintf_s(message, sizeof(message), "Are you sure you want to delete '%s'", levelName);
		}
		else
			strcpy_s(message, sizeof(message), "Are you sure you want to delete these files");
		strcat_s(message, sizeof(message), "?\n(This operation will change the HOG file immediately)");
		if (QueryMsg(message) != IDYES)
			return;
	}

	// The code below really should be in HogManager.cpp but it would be a nuisance to test...
	// so nevermind, we're going to redo it soon anyway
	int nDeletedFiles = 0;
	for (int i = 0; i < deleteIndices.GetCount(); i++) {
		// We have to adjust indices as files are deleted. Relies on deleteIndices being sorted
		fileno = GetFileData(deleteIndices[i] - nDeletedFiles, &size, &offset);
		int nFiles = plb->GetCount();

		// Reopen HOG file for each delete - this is really ugly but we need to refresh the
		// file offsets afterwards which requires calling ReadHogData which also opens the
		// file. Will fix this in a future version, it's a large change
		CFileManager fp;
		CLevelHeader lh;
		if (!fp.Open(MissionName(), "r+b")) {
			ErrorMsg("Could not open hog file.");
			return;
		}
		fp.Seek(offset);
		lh.Read(&fp);
		DeleteSubFile(fp, size + lh.Size(), offset, nFiles, fileno);
		fp.Close();
		nDeletedFiles++;

		ReadHogData();
	}
	LBFiles()->SetCurSel(-1);
}

void HogDialog::OnFilter(void)
{
	m_bShowAll = ((CButton*)GetDlgItem(IDC_HOG_FILTER))->GetCheck();
	if (!ReadHogData())
		EndDialog(0);
}

void HogDialog::OnSelectedFilesChanged(void)
{
	EnableControls();
	UpdateData(FALSE);
}

const char* HogDialog::MissionName()
{
	return hogManager->MissionName();
}

void HogDialog::EnableControls(void)
{
	GetDlgItem(IDOK)->EnableWindow(LBFiles()->GetSelCount() == 1);
	GetDlgItem(IDC_HOG_RENAME)->EnableWindow(LBFiles()->GetSelCount() == 1);
	GetDlgItem(IDC_HOG_EXPORT)->EnableWindow(LBFiles()->GetSelCount() > 0);
	GetDlgItem(IDC_HOG_DELETE)->EnableWindow(LBFiles()->GetSelCount() > 0);
}

void ClearFileList(CListBox* plb)
{
	int i, h = plb->GetCount();
	tHogFileData* pfd;

	for (i = 0; i < h; i++)
		if (pfd = (tHogFileData*)plb->GetItemDataPtr(i))
			delete pfd;
	plb->ResetContent();
}

void HogDialog::ClearFileList(void)
{
	::ClearFileList(LBFiles());
}

int HogDialog::DeleteFile(int index)
{
	if (index == -1)
		return -1;
	tHogFileData* pfd = (tHogFileData*)LBFiles()->GetItemDataPtr(index);
	if (pfd)
		delete pfd;
	LBFiles()->DeleteString(index);
	return 0;
}

int HogDialog::AddFile(LPCSTR pszName, long size, long offset, int fileno)
{
	int index = LBFiles()->AddString(pszName);

	if (0 > AddFileData(index, size, offset, fileno))
		return -1;
	LBFiles()->SetCurSel(index);
	return 0;
}

int HogDialog::GetFileData(int index, long* size, long* offset)
{
	if (index == -1)
		index = GetSingleSelectedIndex();
	if (index == -1)
		return -1;
	tHogFileData* pfd = (tHogFileData*)LBFiles()->GetItemDataPtr(index);
	if (!pfd)
		return -1;
	if (size)
		*size = pfd->m_size;
	if (offset)
		*offset = pfd->m_offs;
	return pfd->m_fileno;
}

int AddFileData(CListBox* plb, int index, int size, int offset, int fileno)
{
	if (index == -1)
		return -1;
	tHogFileData* pfd = new tHogFileData;
	if (!pfd)
		return -1;
	pfd->m_size = size;
	pfd->m_offs = offset;
	pfd->m_fileno = fileno;
	plb->SetItemDataPtr(index, (void*)pfd);
	return index;
}

int HogDialog::AddFileData(int index, long size, long offset, int fileno)
{
	return ::AddFileData(LBFiles(), index, size, offset, fileno);
}

void HogDialog::UpdateListBoxItem(int index)
{
	auto oldFileData = *reinterpret_cast<tHogFileData*>(LBFiles()->GetItemDataPtr(index));
	DeleteFile(index);
	auto newFileData = hogManager->GetFileInfoAtOffset(oldFileData.m_offs);
	AddFile(newFileData.m_fileName.c_str(), newFileData.m_size, newFileData.m_offs, oldFileData.m_fileno);
}

int HogDialog::GetSingleSelectedIndex(void)
{
	if (LBFiles()->GetSelCount() != 1)
		return -1;
	CArray <int> items;
	GetSelectedIndices(items);
	return items[0];
}

void HogDialog::GetSelectedIndices(CArray <int>& selectedIndices)
{
	int nItems = LBFiles()->GetSelCount();
	selectedIndices.SetSize(nItems);
	LBFiles()->GetSelItems(nItems, selectedIndices.GetData());
}

int HogDialog::FindFilename(LPSTR pszName)
{
	CListBox* plb = LBFiles();
	char szName[256];

	int h, i;
	for (h = plb->GetCount(), i = 0; i < h; i++) {
		plb->GetText(i, szName);
		if (!_strcmpi(szName, pszName))
			return i;
	}
	return -1;
}

void RefreshHogFileList(CListBox* plb)
{
	::ClearFileList(plb);
	int selIndex = -1;
	auto files = hogManager->GetFileList();
	for (const auto& file : files)
	{
		int i = plb->AddString(file.m_fileName.c_str());
		if ((0 > ::AddFileData(plb, i, file.m_size, file.m_offs, file.m_fileno))) {
			ErrorMsg("Too many files in HOG file.");
			return;
		}
		if (selIndex < 0)
		{
			if (file.m_fileName.rfind(".rdl") != std::string::npos ||
				file.m_fileName.rfind(".rl2") != std::string::npos)
			{
				selIndex = i;
			}
		}
	}
	plb->SetCurSel(selIndex);
}

bool HogDialog::ReadHogData(void)
{
	if (0 > hogManager->ReadData(MissionName(), m_bShowAll == 1, false))
		return false;
	::RefreshHogFileList(LBFiles());
	UpdateData(FALSE);
	return true;
}
