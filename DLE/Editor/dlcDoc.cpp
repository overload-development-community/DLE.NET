// dlcDoc.cpp : implementation of the CDlcDoc class
//

#include "stdafx.h"
#include <process.h>
#include <direct.h>
#include "parser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CSaveFileDlg : public CDialog {
	public:
		char	m_name [256];
		LPSTR	m_lpszName;
		int	m_type;
		int	*m_pType;

		CSaveFileDlg (CWnd *pParentWnd)
			: CDialog (IDD_SAVELEVEL, pParentWnd) {};
		afx_msg void OnNo ()
			{ EndDialog (IDNO); }
	
	DECLARE_MESSAGE_MAP ()
	};

BEGIN_MESSAGE_MAP (CSaveFileDlg, CDialog)
	ON_BN_CLICKED (IDNO, OnNo)
END_MESSAGE_MAP ()

/////////////////////////////////////////////////////////////////////////////
// CDlcDoc

IMPLEMENT_DYNCREATE(CDlcDoc, CDocument)

BEGIN_MESSAGE_MAP(CDlcDoc, CDocument)
	//{{AFX_MSG_MAP(CDlcDoc)
	ON_COMMAND(ID_OPENFILE, OnOpenFile)
	ON_COMMAND(ID_SAVEFILE, OnSaveFile)
	ON_COMMAND(ID_SAVEFILE_AS, OnSaveFileAs)
	ON_COMMAND(ID_RUN_LEVEL, OnRunLevel)
	ON_COMMAND(ID_INSERT_CUBE, OnInsertSegment)
	ON_COMMAND(ID_INSERT_CUBE_REACTOR, OnInsertSegReactor)
	ON_COMMAND(ID_INSERT_CUBE_ROBOTMAKER, OnInsertSegRobotMaker)
	ON_COMMAND(ID_INSERT_CUBE_PRODUCERTER, OnInsertSegProducer)
	ON_COMMAND(ID_INSERT_CUBE_REPAIRCENTER, OnInsertSegRepairCenter)
	ON_COMMAND(ID_INSERT_OBJECT_PLAYERCOPY, OnInsertObjectPlayerCopy)
	ON_COMMAND(ID_INSERT_OBJECT_PLAYER, OnInsertObjectPlayer)
	ON_COMMAND(ID_INSERT_OBJECT_ROBOT, OnInsertObjectRobot)
	ON_COMMAND(ID_INSERT_OBJECT_WEAPON, OnInsertObjectWeapon)
	ON_COMMAND(ID_INSERT_OBJECT_POWERUP, OnInsertObjectPowerup)
	ON_COMMAND(ID_INSERT_OBJECT_GUIDEBOT, OnInsertObjectGuideBot)
	ON_COMMAND(ID_INSERT_OBJECT_COOPPLAYER, OnInsertObjectCoopPlayer)
	ON_COMMAND(ID_INSERT_OBJECT_REACTOR, OnInsertObjectReactor)
	ON_COMMAND(ID_INSERT_DOOR_NORMAL, OnInsertDoorNormal)
	ON_COMMAND(ID_INSERT_DOOR_PRISON, OnInsertDoorPrison)
	ON_COMMAND(ID_INSERT_DOOR_SECRETEXIT, OnInsertDoorSecretExit)
	ON_COMMAND(ID_INSERT_DOOR_EXIT, OnInsertDoorExit)
	ON_COMMAND(ID_INSERT_DOOR_GUIDEBOTDOOR, OnInsertDoorGuideBot)
	ON_COMMAND(ID_INSERT_TRIGGER_OPENDOOR, OnInsertTriggerOpenDoor)
	ON_COMMAND(ID_INSERT_TRIGGER_ROBOTMAKER, OnInsertTriggerRobotMaker)
	ON_COMMAND(ID_INSERT_TRIGGER_SHIELDDRAIN, OnInsertTriggerShieldDrain)
	ON_COMMAND(ID_INSERT_TRIGGER_ENERGYDRAIN, OnInsertTriggerEnergyDrain)
	ON_COMMAND(ID_INSERT_TRIGGER_CONTROLPANEL, OnInsertTriggerControlPanel)
	ON_COMMAND(ID_INSERT_WALL_FUELCELLS, OnInsertWallFuelCells)
	ON_COMMAND(ID_INSERT_WALL_ILLUSION, OnInsertWallIllusion)
	ON_COMMAND(ID_INSERT_WALL_FORCEFIELD, OnInsertWallForceField)
	ON_COMMAND(ID_INSERT_WALL_FAN, OnInsertWallFan)
	ON_COMMAND(ID_INSERT_WALL_GRATE, OnInsertWallGrate)
	ON_COMMAND(ID_INSERT_WALL_WATERFALL, OnInsertWallWaterfall)
	ON_COMMAND(ID_INSERT_WALL_LAVAFALL, OnInsertWallLavafall)
	ON_COMMAND(ID_DELETE_CUBE, OnDeleteSegment)
	ON_COMMAND(ID_DELETE_OBJECT, OnDeleteObject)
	ON_COMMAND(ID_DELETE_WALL, OnDeleteWall)
	ON_COMMAND(ID_DELETE_TRIGGER, OnDeleteTrigger)
#if 0
	ON_COMMAND(ID_FILE_TEST, OnFileTest)
#endif
	ON_COMMAND(ID_EDIT_CUT, OnCutBlock)
	ON_COMMAND(ID_EDIT_COPY, OnCopyBlock)
	ON_COMMAND(ID_EDIT_QUICKCOPY, OnQuickCopyBlock)
	ON_COMMAND(ID_EDIT_PASTE, OnPasteBlock)
	ON_COMMAND(ID_EDIT_QUICKPASTE, OnQuickPasteBlock)
	ON_COMMAND(ID_EDIT_DELETEBLOCK, OnDeleteBlock)
	ON_COMMAND(ID_EDIT_COPYOTHERCUBESTEXTURES, OnCopyOtherCube)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CDlcDoc, CDocument)
	//{{AFX_DISPATCH_MAP(CAutoDoc)
	DISP_PROPERTY(CDlcDoc, "MemberLong1", Member1, VT_I4)
	DISP_FUNCTION(CDlcDoc, "TestLong", Test, VT_I4, VTS_NONE)
	DISP_FUNCTION(CDlcDoc, "SetMyText", SetMyText, VT_EMPTY, VTS_BSTR)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IAuto to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {3F315844-67AC-11d2-AE2A-00C0F03014A5}
static const IID IID_Idlc =
{ 0x3f315844, 0x67ac, 0x11d2, { 0xae, 0x2a, 0x0, 0xc0, 0xf0, 0x30, 0x14, 0xa5 } };

BEGIN_INTERFACE_MAP(CDlcDoc, CDocument)
	INTERFACE_PART(CDlcDoc, IID_Idlc, Dispatch)
END_INTERFACE_MAP()

//------------------------------------------------------------------------------

class CNewFileDlg : public CDialog {
	public:
		char	m_name [256];
		LPSTR	m_lpszName;
		int	m_type;
		int	*m_pType;

		CNewFileDlg (CWnd *pParentWnd, LPSTR lpszName, int *pType)
			: CDialog (IDD_NEWLEVEL, pParentWnd) {
			strcpy_s (m_name, sizeof (m_name), m_lpszName = lpszName);
			m_type = *(m_pType = pType);
			}
      virtual BOOL OnInitDialog () {
			CDialog::OnInitDialog ();
			if (!theMine->IsVertigo())
				GetDlgItem (IDC_D2VLEVEL)->EnableWindow (FALSE);
			return TRUE;
			}
		virtual void DoDataExchange (CDataExchange * pDX) { 
			DDX_Text (pDX, IDC_LEVELNAME, m_name, sizeof (m_name)); 
			DDX_Radio (pDX, IDC_D1LEVEL, m_type);
			}
		void OnOK (void) {
			UpdateData (TRUE);
			strcpy_s (m_lpszName, 256, m_name); 
			*m_pType = m_type;
			EndDialog (IDOK);
			}
	};

/////////////////////////////////////////////////////////////////////////////
// CDlcDoc construction/destruction

CDlcDoc::CDlcDoc()
{
//theMine = new CMine;
//theMine->Initialize ();
//theMine->Default ();
m_bInitDocument = true;
*m_szFile = '\0';
*m_szSubFile = '\0';
memset (&missionData, 0, sizeof (missionData));
}

CDlcDoc::~CDlcDoc()
{
}

/////////////////////////////////////////////////////////////////////////////
// CDlcDoc serialization

void CDlcDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlcDoc diagnostics

#ifdef _DEBUG
void CDlcDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDlcDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDlcDoc commands

BOOL CDlcDoc::OnNewDocument()
{
if (!CDocument::OnNewDocument())
	return FALSE;
*m_szFile = '\0';
*m_szSubFile = '\0';

if (theMine) {
#if 0
	if (m_bInitDocument) {
		m_bInitDocument = false;
		theMine->Load ();
		}
	else
#endif
		CreateNewLevel ();
	}
return TRUE;
}

//------------------------------------------------------------------------------

void CDlcDoc::CreateNewLevel ()
{
char	newLevelName [256];
int	newFileType = 1;
strcpy_s (newLevelName, sizeof (newLevelName), "(untitled)");

CNewFileDlg	d (DLE.MainFrame (), newLevelName, &newFileType);
if (d.DoModal () == IDOK) {
	theMine->Default ();
	DLE.MineView ()->Reset ();
	DLE.TextureView ()->Reset ();
	DLE.ToolView ()->Reset ();
//		InitRobotData();

	*m_szFile = '\0';
	theMine->SetFileType (newFileType);
	switch (newFileType) {
		case 0:
			theMine->SetFileType (0);
			break;
		case 1:
		case 2:
		case 3:
			theMine->SetFileType (1);
			break;
		}
	theMine->Load ();
	switch (newFileType) {
		case 0:
			theMine->SetLevelVersion (1);
			break;
		case 1:
			theMine->SetLevelVersion (7);
			break;
		case 2:
			theMine->SetLevelVersion (8);
			break;
		case 3:
			theMine->UpdateLevelVersion ();
			segmentManager.UpdateWalls (MAX_WALLS_D2, WALL_LIMIT);
		}
	*m_szSubFile = '\0';
	theMine->SetLevelName(newLevelName);
	theMine->Reset ();
	ResetSelections();
	segmentManager.SetLinesToDraw ();
	DLE.MineView ()->ResetView (true);
	DLE.MineView ()->FitToView ();
	memset (&missionData, 0, sizeof (missionData));
	lightManager.CreateLightMap ();
	DLE.TextureView ()->Setup ();
	DLE.ToolView ()->TextureTool ()->LoadTextureListBoxes ();
	DLE.ToolView ()->MissionTool ()->Refresh ();
	}
}

//------------------------------------------------------------------------------

bool CDlcDoc::BrowseForFile (LPSTR pszFile, BOOL bOpen)
{
return ::BrowseForFile (bOpen, "hog;rl2;rdl", pszFile, 
								"all levels (*.hog;*.rl2;*.rdl)|*.hog;*.rl2;*.rdl"
								"|mission files (*.hog)|*.hog|"
								"|Descent 2 level (*.rl2)|*.rl2|"
								"Descent 1 level (*.rdl)|*.rdl||", 
								0, DLE.MainFrame ());
}

//------------------------------------------------------------------------------

void CDlcDoc::UpdateCaption ()
{
SetPathName (m_szFile);
}

void CDlcDoc::ResetSelections()
{
	selections[0].Index() = 0;
	selections[1].Index() = 1;
	current = &selections[0];
	other = &selections[1];
	selections[0].SetSegmentId(DEFAULT_SEGMENT);
	selections[0].SetPoint(DEFAULT_POINT);
	selections[0].SetEdge(DEFAULT_EDGE);
	selections[0].SetSideId(DEFAULT_SIDE);
	selections[0].SetObjectId(DEFAULT_OBJECT);
	selections[1].SetSegmentId(DEFAULT_SEGMENT);
	selections[1].SetPoint(DEFAULT_POINT);
	selections[1].SetEdge(DEFAULT_EDGE);
	selections[1].SetSideId(DEFAULT_SIDE);
	selections[1].SetObjectId(DEFAULT_OBJECT);
}

//------------------------------------------------------------------------------

bool CDlcDoc::SaveIfModified (void)
{
	INT_PTR	nAction;

if (!IsModified ())
	return true;

CSaveFileDlg d (DLE.MainFrame ());

nAction = d.DoModal (); //AfxMessageBox ("\nThe mine has been modified.\n\nClick 'Yes' to load another mine and loose all changes,\n'No' to save changes before loading another mine,\nor 'Cancel' to keep this mine and return without saving.", MB_YESNOCANCEL | MB_ICONEXCLAMATION);
if (nAction == IDCANCEL)
	return false;
undoManager.SetModified (false);
// buggy for new mine; int required
if (nAction == IDNO) 
	SaveFile (*GetPathName () == '\0');
return true;
}

//------------------------------------------------------------------------------

BOOL CDlcDoc::OpenFile (bool bBrowseForFile, LPCSTR pszFile, LPCSTR pszSubFile) 
{
if (theMine == null)
	return true;

	int err = 0;
	char szFile [256] = {0};
	char szSubFile [256] = {0};

if (DLE.MineView ()->RenderVariableLights ())
	DLE.ToolView ()->LightTool ()->OnShowVariableLights ();
if (!SaveIfModified ())
	return FALSE;
if (bBrowseForFile && !BrowseForFile (szFile, TRUE))
	return FALSE;
if (DLE.ToolView () && DLE.ToolView ()->DiagTool ())
	DLE.ToolView ()->DiagTool ()->Reset ();
if (!*szFile)
	strcpy_s (szFile, sizeof (szFile), pszFile ? pszFile : m_szFile);
_strlwr_s (szFile, 256);
if (pszSubFile)
	strcpy_s (szSubFile, sizeof (szSubFile), pszSubFile);
if (strstr (szFile, ".hog")) {
	if (*szSubFile) {
		if (!hogManager->LoadLevel (szFile, szSubFile))
			return FALSE;
		}
	else {
		HogDialog hogDialog(DLE.MainFrame(), szFile, szSubFile);
		if (hogDialog.DoModal () != IDOK)
			return FALSE;
		strcpy_s (szSubFile, sizeof (szSubFile), hogDialog.LevelName ());
		}
	theMine->Backup (szFile);
	err = !theMine->Load (hogManager->GetLevel(), true);
	memset (&missionData, 0, sizeof (missionData));
	ReadMissionFile (szFile);
	textureManager.LoadMod ();
	modelManager.LoadMod ();
	}
else {
	char szExt [256];
	err = !theMine->Load (szFile);
	CFileManager::SplitPath (szFile, null, szSubFile, szExt);
	strcat_s (szSubFile, 256, szExt);
	}
theMine->Reset ();
ResetSelections();
DLE.TextureView ()->Setup ();
DLE.MineView ()->DelayRefresh (true);
DLE.MineView ()->Reset ();
DLE.MineView ()->FitToView ();
DLE.TextureView ()->Refresh ();
DLE.ToolView ()->MissionTool ()->Refresh ();
DLE.MineView ()->DelayRefresh (false);
DLE.MineView ()->ResetView (true);
DLE.MainFrame ()->UpdateSelectButtons (DLE.MineView ()->GetSelectMode ());
//UpdateAllViews (null);
if (!err) {
	if (m_szFile != szFile)
		strcpy_s (m_szFile, sizeof (m_szFile), szFile);
	strcpy_s (m_szSubFile, sizeof (m_szSubFile), szSubFile);
	UpdateCaption ();
	AfxGetApp ()->AddToRecentFileList (m_szFile);
	}
//textureManager.CountCustomTextures ();
DLE.ToolView ()->TextureTool ()->LoadTextureListBoxes ();
DLE.ToolView ()->SettingsTool ()->Refresh ();
return (err == 0);
}

//------------------------------------------------------------------------------

bool CDlcDoc::SaveFile (bool bSaveAs) 
{
	int result = 0;

DLE.ToolView ()->Refresh ();
//textureManager.CountCustomTextures ();
if (DLE.MineView ()->RenderVariableLights ())
	DLE.ToolView ()->LightTool ()->OnShowVariableLights ();
if (!*m_szFile) {
	char	szMissions [256];
	CFileManager::SplitPath ((DLE.IsD1File ()) ? descentFolder [0] : missionFolder, szMissions, null, null);
//	strcpy_s (m_szFile, sizeof (m_szFile), (DLE.IsD1File ()) ? "new.rdl" : "new.rl2");
	sprintf_s (m_szFile, sizeof (m_szFile), "%s\\%s.hog", szMissions, *m_szSubFile ? m_szSubFile : "new");
	}
if (bSaveAs && !BrowseForFile (m_szFile, FALSE))
	return false;
bool bSaveToHog = strstr (m_szFile, ".hog") != null;
if (bSaveToHog)
{
	if (bSaveAs)
	{
		// This is a bit silly, should be checking .msn/.mn2, but do that in the new UI
		FILE* hogFile;
		fopen_s(&hogFile, m_szFile, "rt");
		if (hogFile)
		{
			fclose(hogFile);
			if (Query2Msg("A mission file with that name already exists.\nOverwrite mission file?", MB_YESNO) != IDYES)
				return false;
		}
	}

	bool overwrite = false;
	if (CHogManager::ContainsSubFile(m_szFile, m_szSubFile))
	{
		overwrite = QueryMsg("Overwrite old level with same name?") == IDYES;
	}

	_strlwr_s(m_szFile, 256);
	bool newLevel = strlen(m_szSubFile) == 0 || strstr(m_szFile, "new.");
	if (newLevel)
	{
		CLevelHeader lh(g_data.IsD2XLevel());
		CInputDialog dlg(DLE.MainFrame(), "Name mine", "Enter file name:", m_szSubFile, lh.NameSize() - 4);
		if (dlg.DoModal() != IDOK)
			return 0;

		char szBaseName[256]{};
		CFileManager::SplitPath(m_szSubFile, null, szBaseName, null);
		if (!*g_data.missionData->missionName)
			strcpy_s(g_data.missionData->missionName, sizeof(g_data.missionData->missionName), szBaseName);
		if (bSaveAs || !*g_data.missionData->missionName)
			do {
				CInputDialog dlg(DLE.MainFrame(), "Mission title", "Enter mission title:",
					g_data.missionData->missionName, sizeof(g_data.missionData->missionName));
				if (dlg.DoModal() != IDOK)
					return -1;
			} while (!*g_data.missionData->missionName);
	}

	result = SaveToHog(m_szFile, m_szSubFile, newLevel, bSaveAs, overwrite);
}
else
	result = theMine->Save (m_szFile);
if (result)
	result = SaveCustomFiles (bSaveToHog);
SetModifiedFlag (result == 0);
if (result) {
	UpdateCaption ();
	AfxGetApp ()->AddToRecentFileList (m_szFile);
	}
return result != 0;
}

bool CDlcDoc::SaveCustomFiles (bool bSaveToHog)
{
	bool bSucceeded = true;

if (bSaveToHog) {
	for (int nType = 0; nType < NUM_CUSTOM_FILETYPES; nType++)
		if (PromptShouldWriteCustomFile (nType)) {
			bSucceeded &= WriteCustomFile (m_szFile, m_szSubFile, nType) > 0;
			}
	}
else {
	if (textureManager.HasCustomTextures ())
		if (DLE.IsD2File ())
			bSucceeded &= WriteExternalCustomFile (CUSTOM_FILETYPE_POG);
		else
			bSucceeded &= WriteExternalCustomFile (CUSTOM_FILETYPE_DTX);
	
	if (robotManager.HasModifiedRobots ())
		bSucceeded &= WriteExternalCustomFile (CUSTOM_FILETYPE_HXM);
	}

return bSucceeded;
}

//--------------------------------------------------------------------------------

bool CDlcDoc::PromptShouldWriteCustomFile (const int nType)
{
	bool bShouldModify = false;
	static const char* szPogQuery = "This level contains custom textures.\nWould you like to save these textures into the HOG file?\n\n"
		"Note: You must use version 1.2 or higher of Descent 2 to see\nthe textures when you play the game.";
	char szOverwritePogQuery [200] = { 0 };
	static const char* szHxmQuery = "This level contains custom robot settings.\nWould you like to save these changes into the HOG file?\n\n"
		"Note: You must use version 1.2 or higher of Descent 2 for\nthe changes to take effect.";
	static const char* szOverwriteHxmQuery = "Would you like to save changes to custom robot behavior for this level?";
	static const char* szDtxQuery = "This level contains custom textures.\nWould you like to save these textures into the HOG file?\n\n"
		"Note: In Descent 1 levels this uses a DTX patch, which will only be loaded automatically by D1X 1.00 or later.\n"
		"For older versions of the game, you will need to download DTX and use the included DTXPATCH utility to apply the custom textures.";
	char szSubFileName [24] = { 0 };

strcpy_s (szSubFileName, sizeof (szSubFileName), m_szSubFile);
LPSTR ext = strrchr (szSubFileName, '.');
if (ext)
	*ext = '\0';
strcat_s (szSubFileName, sizeof (szSubFileName), GetCustomFileExtension (nType));

switch (nType) {
	case CUSTOM_FILETYPE_LIGHTMAP:
		bShouldModify = true;
		break;
	case CUSTOM_FILETYPE_COLORMAP:
		bShouldModify = true;
		break;
	case CUSTOM_FILETYPE_PALETTE:
		bShouldModify = !DLE.IsD1File ();
		break;
	case CUSTOM_FILETYPE_POG:
		if (DLE.IsD1File ())
			break;
		sprintf_s (szOverwritePogQuery, ARRAYSIZE (szOverwritePogQuery),
			"This level contains %d new or changed custom textures.\nWould you like to save these changes?",
			textureManager.CountModifiedTextures ());
		// For new levels, we can have custom textures that aren't tagged "modified" so need to check differently
		if (DoesSubFileExist (m_szFile, szSubFileName))
			bShouldModify = textureManager.CountModifiedTextures () > 0 && (QueryMsg (szOverwritePogQuery) == IDYES);
		else
			bShouldModify = textureManager.CountCustomTextures () > 0 && (QueryMsg (szPogQuery) == IDYES);
		break;
	case CUSTOM_FILETYPE_DTX:
		if (!DLE.IsD1File ())
			break;
		sprintf_s (szOverwritePogQuery, ARRAYSIZE (szOverwritePogQuery),
			"This level contains %d new or changed custom textures.\nWould you like to save these changes?",
			textureManager.CountModifiedTextures ());
		if (DoesSubFileExist (m_szFile, szSubFileName))
			bShouldModify = textureManager.CountModifiedTextures () > 0 && (QueryMsg (szOverwritePogQuery) == IDYES);
		else
			bShouldModify = textureManager.CountCustomTextures () > 0 && (QueryMsg (szDtxQuery) == IDYES);
		break;
	case CUSTOM_FILETYPE_HXM:
		if (DLE.IsD1File ())
			break;
		if (DoesSubFileExist (m_szFile, szSubFileName))
			bShouldModify = robotManager.HasModifiedRobots () && (QueryMsg (szOverwriteHxmQuery) == IDYES);
		else
			bShouldModify = robotManager.HasCustomRobots () && (QueryMsg (szHxmQuery) == IDYES);
		break;
	default:
		break;
	}

return bShouldModify;
}

//--------------------------------------------------------------------------------

bool CDlcDoc::SaveCustomFile (const int nType)
{
bool bSaveToHog = strstr (m_szFile, ".hog") != null;
if (bSaveToHog)
	return WriteCustomFile (m_szFile, m_szSubFile, nType) > 0;
else
	return WriteExternalCustomFile (nType);
}

//------------------------------------------------------------------------------

bool CDlcDoc::WriteExternalCustomFile (const int nType)
{
	bool bSucceeded = true;
	CFileManager fp;
	char filename [MAX_PATH] = { 0 };
	strcpy_s (filename, sizeof (filename), m_szFile);
	char* ps = strstr (filename, ".");

switch (nType) {
	case CUSTOM_FILETYPE_POG:
		if (ps)
			strcpy_s (ps, sizeof (filename) - (ps - filename), ".pog");
		else
			strcat_s (filename, sizeof (filename), ".pog");
		if (fp.Open (filename, "wb")) {
			bSucceeded &= textureManager.CreatePog (fp) > 0;
			fp.Close ();
			}
		break;

	case CUSTOM_FILETYPE_DTX:
		{
		if (ps)
			strcpy_s (ps, sizeof (filename) - (ps - filename), ".dtx");
		else
			strcat_s (filename, sizeof (filename), ".dtx");
		CDtxSoundList soundList;
		if (fp.Exist (filename) && fp.Open (filename, "rb")) {
			soundList.Load (fp, fp.Size ());
			fp.Close ();
			}
		if (fp.Open (filename, "wb")) {
			bSucceeded &= textureManager.CreateDtx (fp, soundList) > 0;
			fp.Close ();
			}
		}
		break;

	case CUSTOM_FILETYPE_HXM:
		if (ps)
			strcpy_s (ps, sizeof (filename) - (ps - filename), ".hxm");
		else
			strcat_s (filename, sizeof (filename), ".hxm");
		if (fp.Open (filename, "wb"))
			bSucceeded &= robotManager.WriteHXM (fp) > 0;
		break;

	default:
		break;
	}

return bSucceeded;
}

//------------------------------------------------------------------------------

BOOL CDlcDoc::OnOpenDocument (LPCTSTR lpszPathName) 
{
return OpenFile (false, lpszPathName);
}

//------------------------------------------------------------------------------

BOOL CDlcDoc::OnSaveDocument (LPCTSTR lpszPathName) 
{
strcpy_s (m_szFile, sizeof (m_szFile), lpszPathName);
return SaveFile (false);
}

//------------------------------------------------------------------------------

void CDlcDoc::OnOpenFile () 
{
OpenFile ();
}

//------------------------------------------------------------------------------

void CDlcDoc::OnSaveFile () 
{
SaveFile (false);
}

//------------------------------------------------------------------------------

void CDlcDoc::OnSaveFileAs () 
{
SaveFile (true);
}

//------------------------------------------------------------------------------

void CDlcDoc::OnRunLevel () 
{
SaveFile (false);
char *h, *p = strstr (m_szFile, "missions\\");
if (p) {
	char	szProg [255], szHogFile [255], szMission [255];

	strcpy_s (szProg, sizeof (szProg), descentFolder [1]);
	if (h = strstr (szProg, "data"))
		*h = '\0';
	int i;
	for (i = int (strlen (szProg)); i && szProg [i - 1] != '\\'; i--)
		;
	szProg [i] = '\0';
	_chdir (szProg);
#ifdef _DEBUG
	strcat_s (szProg, sizeof (szProg), "d2x-xl-dbg.exe");
#else
	strcat_s (szProg, sizeof (szProg), "d2x-xl.exe");
#endif
	sprintf_s (szHogFile, sizeof (szHogFile), "\"%s\"", p + strlen ("missions\\"));
	sprintf_s (szMission, sizeof (szMission), "\"%s\"", m_szSubFile);
	intptr_t j = _spawnl (_P_WAIT, szProg, szProg, 
								 *szPlayerProfile ? "-player" : "", szPlayerProfile, 
								 "-auto_hogfile", szHogFile, 
								 "-auto_mission", szMission, 
								 null);
	if (j < 0)
		j = errno;
	}
}

//------------------------------------------------------------------------------

void CDlcDoc::OnInsertSegment() 
{
	if (theMine->SelectMode() == BLOCK_MODE && segmentManager.TaggedSideCount() > 50)
	{
		if (Query2Msg("You are about to insert a large number of cubes.\n"
			"Are you sure you want to do this?", MB_YESNO) != IDYES)
			return;
	}

	auto nSegment = segmentManager.AddSegments(current);
	current->SetSegmentId(nSegment);
}

void CDlcDoc::OnDeleteSegment() 
{
    auto segmentId = current->SegmentId();
    segmentManager.Delete(segmentId);
    current->Fix(segmentId);
    other->Fix(segmentId);
    current->FixObject();
    other->FixObject();
}

void CDlcDoc::OnInsertSegReactor()
{
    segmentManager.CreateReactor(*current);
}

void CDlcDoc::OnInsertSegRobotMaker()
{
    segmentManager.CreateRobotMaker(*current);
}

void CDlcDoc::OnInsertSegProducer()
{
    segmentManager.CreateProducer(*current);
}

void CDlcDoc::OnInsertSegRepairCenter()
{
    segmentManager.CreateProducer(*current, -1, SEGMENT_FUNC_REPAIRCEN);
}

void CDlcDoc::OnInsertDoorNormal()
{
    wallManager.CreateAutoDoor(*current);
}

void CDlcDoc::OnInsertDoorPrison()
{
    wallManager.CreatePrisonDoor(*current);
}

void CDlcDoc::OnInsertDoorGuideBot()
{
    wallManager.CreateGuideBotDoor(*current);
}

void CDlcDoc::OnInsertDoorExit()
{
    wallManager.CreateNormalExit(*current);
}

void CDlcDoc::OnInsertDoorSecretExit()
{
    wallManager.CreateSecretExit(*current);
}

void CDlcDoc::OnInsertTriggerOpenDoor()
{
    triggerManager.AddOpenDoor(*current, *other);
}

void CDlcDoc::OnInsertTriggerRobotMaker()
{
    triggerManager.AddRobotMaker(*current, *other);
}

void CDlcDoc::OnInsertTriggerShieldDrain()
{
    triggerManager.AddShieldDrain(*current, *other);
}

void CDlcDoc::OnInsertTriggerEnergyDrain()
{
    triggerManager.AddEnergyDrain(*current, *other);
}

void CDlcDoc::OnInsertTriggerControlPanel()
{
    triggerManager.AddUnlock(*current, *other);
}

void CDlcDoc::OnInsertWallFuelCells()
{
    wallManager.CreateFuelCell(*current);
}

void CDlcDoc::OnInsertWallIllusion()
{
    wallManager.CreateIllusion(*current);
}

void CDlcDoc::OnInsertWallForceField()
{
    wallManager.CreateForceField(*current);
}

void CDlcDoc::OnInsertWallFan()
{
    wallManager.CreateFan(*current);
}

void CDlcDoc::OnInsertWallGrate()
{
    wallManager.CreateGrate(*current);
}

void CDlcDoc::OnInsertWallWaterfall()
{
    wallManager.CreateWaterFall(*current);
}

void CDlcDoc::OnInsertWallLavafall()
{
    wallManager.CreateLavaFall(*current);
}

void CDlcDoc::OnInsertObjectPlayer()
{
    objectManager.Create(OBJ_PLAYER, current->Object(), current->SegmentId());
}

void CDlcDoc::OnInsertObjectCoopPlayer()
{
    objectManager.Create(OBJ_COOP, current->Object(), current->SegmentId());
}

void CDlcDoc::OnInsertObjectPlayerCopy()
{
    objectManager.Create(OBJ_NONE, current->Object(), current->SegmentId());
}

void CDlcDoc::OnInsertObjectRobot()
{
    if (objectManager.Create(OBJ_ROBOT, current->Object(), current->SegmentId())) {
        current->Object()->Id() = 3; // class 1 drone
        current->Object()->Setup(current->Object()->Type());
    }
}

void CDlcDoc::OnInsertObjectWeapon()
{
    if (objectManager.Create(OBJ_POWERUP, current->Object(), current->SegmentId())) {
        current->Object()->Id() = 3; // laser
        current->Object()->Setup(current->Object()->Type());
    }
}

void CDlcDoc::OnInsertObjectPowerup()
{
    if (objectManager.Create(OBJ_POWERUP, current->Object(), current->SegmentId())) {
        current->Object()->Id() = 1; // energy boost
        current->Object()->Setup(current->Object()->Type());
    }
}

void CDlcDoc::OnInsertObjectGuideBot()
{
    if (objectManager.Create(OBJ_ROBOT, current->Object(), current->SegmentId())) {
        current->Object()->Id() = 33; // guide bot
        current->Object()->Setup(current->Object()->Type());
    }
}

void CDlcDoc::OnInsertObjectReactor()
{
    if (objectManager.Create(OBJ_REACTOR, current->Object(), current->SegmentId())) {
        current->Object()->Id() = 2; // standard reactor
        current->Object()->Setup(current->Object()->Type());
    }
}

void CDlcDoc::OnDeleteObject()
{
    if ((QueryMsg("Are you sure you want to delete this object?") == IDYES))
        objectManager.Delete(current->ObjectId());
}

void CDlcDoc::OnDeleteWall()
{
    wallManager.Delete(*current);
    DLE.MineView()->Refresh();
}

void CDlcDoc::OnDeleteTrigger()
{
    triggerManager.DeleteFromWall(*current);
}

const char* BLOCKOP_HINT =
    "The block of cubes will be saved relative to the current segment.\n"
    "Later, when you paste the block, it will be placed relative to\n"
    "the current segment at that time.  You can change the current side\n"
    "and the current point to affect the relative direction and\n"
    "rotation of the block.\n"
    "\n"
    "Would you like to proceed?";

void CDlcDoc::OnCutBlock()
{
    if (QueryMsg(BLOCKOP_HINT) != IDYES)
        return;

    char szFile[256]{};
    if (!::BrowseForFile(FALSE,
        blockManager.Extended() ? "blx" : "blk", szFile,
        "Block file|*.blk|"
        "Extended block file|*.blx|"
        "All Files|*.*||",
        OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT,
        DLE.MainFrame()
    ))
        return;

    blockManager.Cut(current, szFile);
}

void CDlcDoc::OnCopyBlock()
{
    if (QueryMsg(BLOCKOP_HINT) != IDYES)
        return;

    char szFile[256]{};
    if (!::BrowseForFile(FALSE,
        blockManager.Extended() ? "blx" : "blk", szFile,
        "Block file|*.blk|"
        "Extended block file|*.blx|"
        "All Files|*.*||",
        OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT,
        DLE.MainFrame()
    ))
        return;

    blockManager.Copy(current, szFile);
}

void CDlcDoc::OnQuickCopyBlock()
{
    blockManager.QuickCopy(current);
}

void CDlcDoc::OnPasteBlock()
{
    if (tunnelMaker.Active())
        return;
    // Initialize data for fp open dialog
    char szFile[256] = "\0";

    if (!::BrowseForFile(TRUE, blockManager.Extended() ? "blx" : "blk", szFile,
        "Block file|*.blk|"
        "Extended block file|*.blx|"
        "All Files|*.*||",
        OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST,
        DLE.MainFrame()
    ))
        return;
    if (!blockManager.Read(current, szFile))
        DLE.MineView()->SetSelectMode(BLOCK_MODE);
}

void CDlcDoc::OnQuickPasteBlock()
{
    if (tunnelMaker.Active())
        return;

    if (blockManager.HasRememberedFilename())
    {
        if (!blockManager.Read(current, nullptr))
            DLE.MineView()->SetSelectMode(BLOCK_MODE);
    }
    else
    {
        OnPasteBlock();
    }
}

void CDlcDoc::OnDeleteBlock()
{
    if (QueryMsg("Are you sure you want to delete the marked cubes?") != IDYES)
        return;

    blockManager.Delete();
    current->FixObject();
    other->FixObject();
}

void CDlcDoc::OnCopyOtherCube()
{
    segmentManager.CopyOtherSegment(other->SegmentId(), current->SegmentId());
}


long CDlcDoc::Test() 
{
	// TODO: Add your dispatch handler code here

	return 0;
}

void CDlcDoc::SetMyText(LPCTSTR string) 
{
//	 m_Text.Format(string);
}

#if 0
void CDlcDoc::OnFileTest() 
{
	CParser parser();
	
	parser.RunScript();
	
}
#endif
