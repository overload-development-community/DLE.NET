// dlcView.cpp : implementation of the CMineView class
//

#include "stdafx.h"
#include "afxpriv.h"
#include "dle-xp.h"

#include "dlcDoc.h"
#include "mineview.h"
#include "toolview.h"

#include "PaletteManager.h"
#include "TextureManager.h"
#include "global.h"
#include "FileManager.h"
#include "ObjectManager.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	MAX_VIEWDIST	30

//------------------------------------------------------------------------------

CAppSettings appSettings;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CSettingsTabDlg::UpdateSettings (void) 
{ 
DLE.ToolView ()->SettingsTool ()->Update (); 
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CSettingsTool, CToolDlg)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

CSettingsTool::CSettingsTool (CPropertySheet *pParent)
	: CToolDlg (IDD_SETTINGS_HORZ + nLayout, pParent)
{
appSettings.Setup ();
m_bNoRefresh = false;
m_bInvalid = false;
}

//------------------------------------------------------------------------------

CSettingsTool::~CSettingsTool ()
{
appSettings.Save ();
}

//------------------------------------------------------------------------------

BOOL CSettingsTool::OnInitDialog ()
{
if (!CToolDlg::OnInitDialog ())
   return FALSE;

m_settingsTools.Init (this);
if (nLayout) {
	m_settingsTools.Add (new CFileSettingsTool (IDD_PREFS_FILES_VERT, this), IDD_PREFS_FILES_VERT, "Files");
	m_settingsTools.Add (new CLayoutSettingsTool (IDD_PREFS_LAYOUT_VERT, this), IDD_PREFS_LAYOUT_VERT, "Layout");
	m_settingsTools.Add (new CRendererSettingsTool (IDD_PREFS_RENDERER_VERT, this), IDD_PREFS_RENDERER_VERT, "Renderer");
	m_settingsTools.Add (new CEditorSettingsTool (IDD_PREFS_EDITOR_VERT, this), IDD_PREFS_EDITOR_VERT, "Editor");
	m_settingsTools.Add (new CVisibilitySettingsTool (IDD_PREFS_VISIBILITY_VERT, this), IDD_PREFS_VISIBILITY_VERT, "Visibility");
	m_settingsTools.Add (new CMiscSettingsTool (IDD_PREFS_MISC_VERT, this), IDD_PREFS_MISC_VERT, "Misc");
	}
else {
	m_settingsTools.Add (new CFileSettingsTool (IDD_PREFS_FILES_HORZ, this), IDD_PREFS_FILES_HORZ, "Files");
	m_settingsTools.Add (new CLayoutSettingsTool (IDD_PREFS_LAYOUT_HORZ, this), IDD_PREFS_LAYOUT_HORZ, "Layout");
	m_settingsTools.Add (new CRendererSettingsTool (IDD_PREFS_RENDERER_HORZ, this), IDD_PREFS_RENDERER_HORZ, "Renderer");
	m_settingsTools.Add (new CEditorSettingsTool (IDD_PREFS_EDITOR_HORZ, this), IDD_PREFS_EDITOR_HORZ, "Editor");
	m_settingsTools.Add (new CVisibilitySettingsTool (IDD_PREFS_VISIBILITY_HORZ, this), IDD_PREFS_VISIBILITY_HORZ, "Visibility");
	m_settingsTools.Add (new CMiscSettingsTool (IDD_PREFS_MISC_HORZ, this), IDD_PREFS_MISC_VERT, "Misc");
	}
m_settingsTools.Setup ();

appSettings.Get (false);
appSettings.Load (false);

m_bInvalid = false;
appSettings.Set ();
m_bInited = true;
return TRUE;
}

//------------------------------------------------------------------------------

void CSettingsTool::DoDataExchange (CDataExchange *pDX)
{
DDX_Control (pDX, IDC_TOOL_TAB, m_settingsTools);
if (!HaveData (pDX)) 
	return;
}

//------------------------------------------------------------------------------

void CSettingsTool::Refresh (void)
{
if (!m_bNoRefresh) {
	appSettings.Get ();
	if (::IsWindow (m_hWnd)) {
		UpdateData (FALSE);
		Current ()->UpdateData (FALSE);
		}
	}
}

//------------------------------------------------------------------------------

void CSettingsTool::Update (bool bRefresh)
{
UpdateData (TRUE);
Current ()->UpdateData (TRUE);
m_bNoRefresh = true;
appSettings.Set (-1);
m_bNoRefresh = false;
if (bRefresh)
	Refresh ();
}

//------------------------------------------------------------------------------

BOOL CSettingsTool::OnSetActive ()
{
Refresh ();
return CToolDlg::OnSetActive ();
}

//------------------------------------------------------------------------------

void CSettingsTool::OnOK (void)
{
Update ();
}

//------------------------------------------------------------------------------

void CSettingsTool::OnCancel (void)
{
appSettings.Get ();
UpdateData (FALSE);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// copy local settings to application

void CAppSettings::Setup (void)
{
appSettings.m_depthPerception = 2;

appSettings.m_rotateRates [8] = (double) PI / 4.0; // 45 deg
appSettings.m_rotateRates [7] = (double) PI / 6.0; // 30 deg
appSettings.m_rotateRates [6] = (double) PI / 8.0;
appSettings.m_rotateRates [5] = (double) PI / 12.0; // 15 deg
appSettings.m_rotateRates [4] = (double) PI / 16.0;
appSettings.m_rotateRates [3] = (double) PI / 24.0; // 7.5 deg
appSettings.m_rotateRates [2] = (double) PI / 32.0;
appSettings.m_rotateRates [1] = (double) PI / 64.0;
appSettings.m_rotateRates [0] = (double) PI / 128.0;
appSettings.m_iRotateRate = 4;
appSettings.m_bDepthTest = 1;
appSettings.m_nRenderer = -1;
appSettings.m_nPerspective = 0;

Load (true);

DLE.ExpertMode () = appSettings.m_bExpertMode;
DLE.SplashScreen () = appSettings.m_bSplashScreen;
appSettings.m_bApplyFaceLightSettingsGlobally = 1; // on by default; changes only valid per session and not persistent!

appSettings.m_bInvalid = false;
}

//------------------------------------------------------------------------------

void CAppSettings::Set (int bUpdate)
{
CHECKMINE;
if (appSettings.m_bInvalid)
	return;

_strlwr_s (appSettings.m_d1Folder, sizeof (appSettings.m_d1Folder));
if (strcmp (descentFolder [0], appSettings.m_d1Folder)) {
	strcpy_s (descentFolder [0], sizeof (descentFolder [0]), appSettings.m_d1Folder);
	WritePrivateProfileString ("DLE", "DescentDirectory", descentFolder [0], DLE.IniFile ());
	ReloadTextures (0);
	}

_strlwr_s (appSettings.m_d2Folder, sizeof (appSettings.m_d2Folder));
if (strcmp (descentFolder [1], appSettings.m_d2Folder) && (bUpdate < 0)) {
	bool	bChangePig = true;
	if (textureManager.HasCustomTextures () &&
		 (QueryMsg ("Changing the pig file will affect the custom textures\n"
						"in this level because of the change in palette.\n"
						"(Reload the level to int custom texture appeareance.)\n\n"
						"Are you sure you want to do this?") != IDYES))
		bChangePig = false;
	if (bChangePig) {
		strcpy_s (descentFolder [1], sizeof (descentFolder [1]), appSettings.m_d2Folder);
		WritePrivateProfileString ("DLE", "Descent2Directory", descentFolder [1], DLE.IniFile ());
		ReloadTextures (1);
		}
	}

_strlwr_s (appSettings.m_missionFolder, sizeof (appSettings.m_missionFolder));
if (strcmp (missionFolder, appSettings.m_missionFolder)) {
	strcpy_s (missionFolder, sizeof (missionFolder), appSettings.m_missionFolder);
	WritePrivateProfileString ("DLE", "levelsDirectory", missionFolder, DLE.IniFile ());
	if (*missionFolder)
		::SetCurrentDirectory (missionFolder);
	}

if (!bUpdate)
	DLE.MineView ()->DelayRefresh (true);
else if (bUpdate < 0) {
	DLE.MineView ()->SetRenderer (appSettings.m_nRenderer);
	appSettings.m_nRenderer = abs (appSettings.m_nRenderer);
	DLE.MineView ()->SetPerspective (appSettings.m_nPerspective);
	DLE.MineView ()->SetViewDistIndex (appSettings.m_nViewDist);
	DLE.MineView ()->SetDepthTest (appSettings.m_bDepthTest != 0);
	DLE.MineView ()->SetViewMineFlags (appSettings.m_mineViewFlags);
	DLE.MineView ()->SetViewObjectFlags (appSettings.m_objViewFlags);
	DLE.TextureView ()->SetViewFlags (appSettings.m_texViewFlags);
	DLE.MineView ()->SetDepthScale (appSettings.m_depthPerception);
	DLE.MineView ()->MineCenter () = appSettings.m_nMineCenter;
	}
if (!bUpdate) {
	DLE.MineView ()->DelayRefresh (false);
	DLE.MineView ()->Refresh (false);
	}
if (theMine)
	theMine->RotateRate () = appSettings.m_rotateRates [appSettings.m_iRotateRate];
DLE.MineView ()->SetDepthScale (appSettings.m_depthPerception);
DLE.MineView ()->SetMoveRates (appSettings.m_moveRate);
DLE.MineView ()->SetInputSettings ();
DLE.ExpertMode () = appSettings.m_bExpertMode;
lightManager.ApplyFaceLightSettingsGlobally () = appSettings.m_bApplyFaceLightSettingsGlobally;
DLE.SplashScreen () = appSettings.m_bSplashScreen;
objectManager.SortObjects () = appSettings.m_bSortObjects;
objectManager.BumpObjects () = !appSettings.m_bAllowObjectOverlap;
objectManager.BumpIncrement () = appSettings.m_bumpIncrement;
undoManager.SetMaxSize (appSettings.m_nMaxUndo);
segmentManager.UpdateAlignmentOnEdit () = appSettings.m_bUpdateTexAlign;
if (bUpdate < 1)
	Save (false);
#ifdef _DEBUG
if (appSettings.m_bInited)
	DLE.ToolView ()->SetActive (11);
#endif
}

//------------------------------------------------------------------------------

void CAppSettings::WritePrivateProfileInt (LPSTR szKey, int nValue)
{
	char	szValue [20];

sprintf_s (szValue, sizeof (szValue), "%d", nValue);
WritePrivateProfileString ("DLE", szKey, szValue, DLE.IniFile ());
}

//------------------------------------------------------------------------------

void CAppSettings::WritePrivateProfileDouble (LPSTR szKey, double nValue)
{
	char	szValue [100];

sprintf_s (szValue, sizeof (szValue), "%1.3f", nValue);
WritePrivateProfileString ("DLE", szKey, szValue, DLE.IniFile ());
}

//------------------------------------------------------------------------------
// copy application settings to local storage

void CAppSettings::Get (bool bGetFolders)
{
if (bGetFolders) {
	strcpy_s (appSettings.m_d1Folder, sizeof (appSettings.m_d1Folder), descentFolder [0]);
	strcpy_s (appSettings.m_d2Folder, sizeof (appSettings.m_d2Folder), descentFolder [1]);
	strcpy_s (appSettings.m_missionFolder, sizeof (appSettings.m_missionFolder), missionFolder);
	}

appSettings.m_bDepthTest = DLE.MineView ()->DepthTest ();
appSettings.m_mineViewFlags = DLE.MineView ()->GetMineViewFlags ();
appSettings.m_objViewFlags = DLE.MineView ()->GetObjectViewFlags ();
appSettings.m_texViewFlags = DLE.TextureView ()->GetViewFlags ();
appSettings.m_nMaxUndo = undoManager.MaxSize ();
appSettings.m_depthPerception = DLE.MineView ()->DepthPerception ();
appSettings.m_iRotateRate = 4;
if (theMine) {
	int i = 8;
	for (; i > 0; i--)
		if (theMine->RotateRate () >= appSettings.m_rotateRates [i]) {
			break;
			}
	appSettings.m_iRotateRate = i;
	}
DLE.MineView ()->GetMoveRates (appSettings.m_moveRate);
appSettings.m_nViewDist = DLE.MineView ()->ViewDistIndex ();
appSettings.m_bApplyFaceLightSettingsGlobally = lightManager.ApplyFaceLightSettingsGlobally ();
appSettings.m_bSortObjects = objectManager.SortObjects ();
appSettings.m_bExpertMode = DLE.ExpertMode ();
appSettings.m_bSplashScreen = DLE.SplashScreen ();
appSettings.m_bAllowObjectOverlap = !objectManager.BumpObjects ();
appSettings.m_bUpdateTexAlign = segmentManager.UpdateAlignmentOnEdit ();
}

//------------------------------------------------------------------------------

void CAppSettings::Load (bool bInitialize)
{
if (bInitialize) {
	GetPrivateProfileString ("DLE", "DescentDirectory", descentFolder [0], descentFolder [0], sizeof (descentFolder [0]), DLE.IniFile ());
	strcpy_s (appSettings.m_d1Folder, sizeof (appSettings.m_d1Folder), descentFolder [0]);
	CompletePath (appSettings.m_d1Folder, "descent.pig", ".pig");
	GetPrivateProfileString ("DLE", "Descent2Directory", descentFolder [1], descentFolder [1], sizeof (descentFolder [1]), DLE.IniFile ());
	strcpy_s (appSettings.m_d2Folder, sizeof (appSettings.m_d2Folder), descentFolder [1]);
	CompletePath (appSettings.m_d2Folder, "groupa.pig", ".pig");
	GetPrivateProfileString ("DLE", "LevelsDirectory", missionFolder, missionFolder, sizeof (missionFolder), DLE.IniFile ());
	char* ps = strrchr (missionFolder, '\\');
	if (ps)
		*ps = '\0';
	strcpy_s (appSettings.m_missionFolder, sizeof (appSettings.m_missionFolder), missionFolder);
	//CompletePath (appSettings.m_missionFolder, "descent2.hog", ".hog");
	GetPrivateProfileString ("DLE", "PlayerProfile", szPlayerProfile, szPlayerProfile, sizeof (szPlayerProfile), DLE.IniFile ());
	CFileManager::SplitPath (descentFolder [1], modFolder, null, null);
	ps = strstr (modFolder, "data");
	if (ps)
		strcpy (ps, "mods");
	else
		strcat (modFolder, "mods");

	appSettings.m_depthPerception = GetPrivateProfileInt ("DLE", "DepthPerception", 2, DLE.IniFile ());
	appSettings.m_iRotateRate = GetPrivateProfileInt ("DLE", "RotateRate", 4, DLE.IniFile ());

	char	szMoveRate [100];

	GetPrivateProfileString ("DLE", "MoveRate", "1", szMoveRate, sizeof (szMoveRate), DLE.IniFile ());
	appSettings.m_moveRate [0] = Clamp ((double) atof (szMoveRate), 0.001, 1000.0);
	GetPrivateProfileString ("DLE", "ViewMoveRate", "1", szMoveRate, sizeof (szMoveRate), DLE.IniFile ());
	appSettings.m_moveRate [1] = Clamp ((double) atof (szMoveRate), 0.001, 1000.0);
	}

#if 0
*descentFolder [0] =
*descentFolder [1] = '\0';
#endif

appSettings.m_movementMode = (eMovementModes)GetPrivateProfileInt ("DLE", "MovementMode", 0, DLE.IniFile ());
char szSpeed [100];
GetPrivateProfileString ("DLE", "MoveSpeed", "50", szSpeed, sizeof (szSpeed), DLE.IniFile ());
appSettings.m_kbMoveScale = Clamp ((double) atof (szSpeed), 0.001, 1000.0);
GetPrivateProfileString ("DLE", "TurnSpeed", "1", szSpeed, sizeof (szSpeed), DLE.IniFile ());
appSettings.m_kbRotateScale = Clamp ((double) atof (szSpeed), 0.001, 1000.0);
appSettings.m_bFpInputLock = GetPrivateProfileInt ("DLE", "ForceFirstPersonOnInputLock", 1, DLE.IniFile ());
appSettings.m_bExpertMode = GetPrivateProfileInt ("DLE", "ExpertMode", 1, DLE.IniFile ());
appSettings.m_bSplashScreen = GetPrivateProfileInt ("DLE", "SplashScreen", 1, DLE.IniFile ());
// Object overlap is flipped due to the name change. Keeping .ini setting as it was for backward compatibility.
appSettings.m_bAllowObjectOverlap = !GetPrivateProfileInt ("DLE", "BumpObjects", 1, DLE.IniFile ());
char szBumpIncrement [100];
GetPrivateProfileString ("DLE", "BumpIncrement", "3", szBumpIncrement, sizeof (szBumpIncrement), DLE.IniFile ());
appSettings.m_bumpIncrement = Clamp ((double) atof (szBumpIncrement), 0.001, 1000.0);
appSettings.m_bUpdateTexAlign = GetPrivateProfileInt ("DLE", "UpdateTexAlign", 1, DLE.IniFile ());
appSettings.m_mineViewFlags = GetPrivateProfileInt ("DLE", "MineViewFlags", appSettings.m_mineViewFlags, DLE.IniFile ());
appSettings.m_objViewFlags = GetPrivateProfileInt ("DLE", "ObjViewFlags", appSettings.m_objViewFlags, DLE.IniFile ());
appSettings.m_texViewFlags = GetPrivateProfileInt ("DLE", "TexViewFlags", appSettings.m_texViewFlags, DLE.IniFile ());
//appSettings.m_bApplyFaceLightSettingsGlobally = GetPrivateProfileInt ("DLE", "ApplyFaceLightSettingsGlobally", appSettings.m_bApplyFaceLightSettingsGlobally, DLE.IniFile ());
appSettings.m_bSortObjects = GetPrivateProfileInt ("DLE", "SortObjects", appSettings.m_bSortObjects, DLE.IniFile ());
appSettings.m_bDepthTest = GetPrivateProfileInt ("DLE", "DepthTest", appSettings.m_bDepthTest, DLE.IniFile ());
appSettings.m_nViewDist = GetPrivateProfileInt ("DLE", "ViewDistance", 0, DLE.IniFile ());
//appSettings.m_nRenderer = GetPrivateProfileInt ("DLE", "Renderer", 0, DLE.IniFile ());
appSettings.m_nPerspective = GetPrivateProfileInt ("DLE", "Perspective", 0, DLE.IniFile ());
appSettings.m_nMineCenter = GetPrivateProfileInt ("DLE", "MineCenter", 0, DLE.IniFile ());
appSettings.m_nMaxUndo = GetPrivateProfileInt ("DLE", "MaxUndo", DLE_MAX_UNDOS, DLE.IniFile ());
}

//------------------------------------------------------------------------------

void CAppSettings::Save (bool bSaveFolders)
{
	char	szMoveRate [100];

Get (bSaveFolders);
if (bSaveFolders) {
	WritePrivateProfileString ("DLE", "DescentDirectory", descentFolder [0], DLE.IniFile ());
	WritePrivateProfileString ("DLE", "Descent2Directory", descentFolder [1], DLE.IniFile ());
	WritePrivateProfileString ("DLE", "levelsDirectory", missionFolder, DLE.IniFile ());
	}
WritePrivateProfileString ("DLE", "PlayerProfile", szPlayerProfile, DLE.IniFile ());
//WritePrivateProfileInt ("Renderer", appSettings.m_nRenderer);
WritePrivateProfileInt ("Perspective", appSettings.m_nPerspective);
WritePrivateProfileInt ("DepthPerception", appSettings.m_depthPerception);
WritePrivateProfileInt ("RotateRate", appSettings.m_iRotateRate);
sprintf_s (szMoveRate, sizeof (szMoveRate), "%1.3f", appSettings.m_moveRate [0]);
WritePrivateProfileDouble ("MoveRate", appSettings.m_moveRate [0]);
sprintf_s (szMoveRate, sizeof (szMoveRate), "%1.3f", appSettings.m_moveRate [1]);
WritePrivateProfileDouble ("ViewMoveRate", appSettings.m_moveRate [1]);
WritePrivateProfileInt ("MovementMode", appSettings.m_movementMode);
WritePrivateProfileDouble ("MoveSpeed", appSettings.m_kbMoveScale);
WritePrivateProfileDouble ("TurnSpeed", appSettings.m_kbRotateScale);
WritePrivateProfileInt ("ForceFirstPersonOnInputLock", appSettings.m_bFpInputLock);
WritePrivateProfileInt ("ExpertMode", appSettings.m_bExpertMode);
WritePrivateProfileInt ("SplashScreen", appSettings.m_bSplashScreen);
WritePrivateProfileInt ("BumpObjects", !appSettings.m_bAllowObjectOverlap);
WritePrivateProfileDouble ("BumpIncrement", appSettings.m_bumpIncrement);
WritePrivateProfileInt ("UpdateTexAlign", appSettings.m_bUpdateTexAlign);
WritePrivateProfileInt ("DepthTest", appSettings.m_bDepthTest);
WritePrivateProfileInt ("MineViewFlags", appSettings.m_mineViewFlags);
WritePrivateProfileInt ("ObjViewFlags", appSettings.m_objViewFlags);
WritePrivateProfileInt ("TexViewFlags", appSettings.m_texViewFlags);
//WritePrivateProfileInt ("ApplyFaceLightSettingsGlobally", appSettings.m_bApplyFaceLightSettingsGlobally);
WritePrivateProfileInt ("SortObjects", appSettings.m_bSortObjects);
WritePrivateProfileInt ("ViewDistance", appSettings.m_nViewDist);
WritePrivateProfileInt ("MineCenter", appSettings.m_nMineCenter);
WritePrivateProfileInt ("MaxUndo", appSettings.m_nMaxUndo);
WritePrivateProfileInt ("TextureFilter", DLE.TextureView ()->TextureFilter ());
WritePrivateProfileInt ("EnableQuickSelection", DLE.MineView ()->EnableQuickSelection ());
WritePrivateProfileInt ("ShowSelectionCandidates", DLE.MineView ()->ShowSelectionCandidates ());
WritePrivateProfileInt ("ElementMovementReference", DLE.MineView ()->GetElementMovementReference ());
}

//------------------------------------------------------------------------------

void CAppSettings::ReloadTextures (int nVersion)
{
	bool bSucceeded = false;

if (textureManager.Available (nVersion))
	bSucceeded = textureManager.ChangePigFile (descentFolder [nVersion], nVersion);
else {
	paletteManager.Reload ();
	bSucceeded = textureManager.Reload (nVersion);
	}
if (bSucceeded) {
	if (appSettings.m_bInited) {
		DLE.TextureView ()->Setup ();
		DLE.TextureView ()->Refresh ();
		}
	}
else if (appSettings.m_bInited)
	DLE.ToolView ()->SetActive (11);
if (appSettings.m_bInited)
	DLE.MineView ()->ResetView (true);
}

//------------------------------------------------------------------------------

void CAppSettings::SetPerspective (int nPerspective) 
{ 
appSettings.m_nPerspective = nPerspective; 
DLE.MineView ()->SetPerspective (appSettings.m_nPerspective);
}

//------------------------------------------------------------------------------

void CAppSettings::TogglePerspective (void)
{
SetPerspective (!appSettings.m_nPerspective);
}

//------------------------------------------------------------------------------

void CAppSettings::SetLayout (int nLayout)
{
if (nLayout == 3)
	WritePrivateProfileInt ("Layout", 1);
else
	WritePrivateProfileInt ("Layout", nLayout);
WritePrivateProfileInt ("SingleToolPane", nLayout == 3);
}

//------------------------------------------------------------------------------

void CAppSettings::SetRenderer (int nRenderer)
{
DLE.MineView ()->SetRenderer (appSettings.m_nRenderer = nRenderer);
DLE.ToolView ()->SettingsTool ()->UpdateData (FALSE);
DLE.ToolView ()->SettingsTool ()->Update ();
}

//------------------------------------------------------------------------------

void CAppSettings::CompletePath (LPSTR pszPath, LPSTR pszFile, LPSTR pszExt)
{
_strlwr_s (pszPath, 256);
if (*pszPath && !strstr (pszPath, pszExt)) {
	if (pszPath [strlen (pszPath) - 1] != '\\')
		strcat_s (pszPath, 256, "\\");
	strcat_s (pszPath, 256, pszFile);
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//eof prefsdlg.cpp