// dlcView.cpp : implementation of the CMineView class
//

#include "stdafx.h"
#include "afxpriv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolView

BEGIN_MESSAGE_MAP(CMissionTool, CToolDlg)
	ON_BN_CLICKED (IDC_MISSION_ADD, OnAdd)
	ON_BN_CLICKED (IDC_MISSION_DELETE, OnDelete)
	ON_BN_CLICKED (IDC_MISSION_UP, OnUp)
	ON_BN_CLICKED (IDC_MISSION_DOWN, OnDown)
	ON_BN_CLICKED (IDC_MISSION_RENAME, OnRename)
	ON_BN_CLICKED (IDC_MISSION_FROMHOG, OnFromHog)
	ON_EN_KILLFOCUS (IDC_MISSION_ADD, OnAdd)
	ON_EN_KILLFOCUS (IDC_LEVEL_NAME, OnSetLevelName)
	ON_LBN_DBLCLK (IDC_MISSION_LEVELLIST, OnLoadLevel)
/*
	ON_BN_CLICKED (IDC_MISSION_, On)
	ON_EN_KILLFOCUS (IDC_MISSION_, On)
	ON_EN_UPDATE (IDC_MISSION, OnOK)
*/
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

CMissionTool::CMissionTool (CPropertySheet *pParent)
	: CToolDlg (IDD_MISSIONDATA_HORZ + nLayout, pParent)
{
*m_szLevel = '\0';
m_missionData.numLevels =
m_missionData.numSecrets = 0;
}

//------------------------------------------------------------------------------

BOOL CMissionTool::OnInitDialog ()
{
if (!CToolDlg::OnInitDialog ())
   return FALSE;
BuildLevelList ();
m_bInited = true;
Refresh ();
return TRUE;
}

//------------------------------------------------------------------------------


void CMissionTool::Refresh (void)
{
if (::IsWindow (m_hWnd) && m_bInited) {
	memcpy (&m_missionData, &missionData, sizeof (missionData));
	strcpy_s(m_levelName, theMine->LevelName());
	UpdateData (FALSE);
	}
}

								/*--------------------------*/

BOOL CMissionTool::OnSetActive ()
{
Refresh ();
return CToolDlg::OnSetActive ();
}

//------------------------------------------------------------------------------

void CMissionTool::BuildLevelList (void)
{
	CListBox *plb = LBLevels ();

plb->ResetContent ();
int i;
for (i = 0; i < m_missionData.numLevels; i++)
	plb->AddString (m_missionData.levelList [i]);
int j;
for (j = 0; j < m_missionData.numSecrets; i++, j++)
	plb->AddString (m_missionData.levelList [i]);
}

//------------------------------------------------------------------------------

void CMissionTool::DoDataExchange (CDataExchange * pDX)
{
if (!HaveData (pDX)) 
	return;

	int	h, i, j;
	char	szSecret [13], szError [80];
	char	*psz;
	int	nSecret;

DDX_Text (pDX, IDC_MISSION_NAME, m_missionData.missionName, sizeof (m_missionData.missionName));
DDX_Text (pDX, IDC_LEVEL_NAME, m_levelName, sizeof(m_levelName));
DDX_Radio (pDX, IDC_MISSION_MULTIPLAYER, m_missionData.missionType);
for (i = 0; i < 8; i++)
	DDX_Text (pDX, IDC_MISSION_EDITORS + i, m_missionData.missionInfo [i], sizeof (m_missionData.missionInfo [i]));
for (i = 0; i < 6; i++)
	DDX_Check (pDX, IDC_MISSION_NORMAL + i, m_missionData.missionFlags [i]);
for (i = 0; i < 2; i++)
	DDX_Check (pDX, IDC_MISSION_MULTIAUTHOR + i, m_missionData.authorFlags [i]);
for (i = 0; i < 3; i++)
	DDX_Check (pDX, IDC_MISSION_POG + i, m_missionData.customFlags [i]);
CListBox *plb = LBLevels ();
if (pDX->m_bSaveAndValidate) {
	theMine->SetLevelName(m_levelName);
	m_missionData.numLevels =
	m_missionData.numSecrets = 0;
	nSecret = -1;
	for (i = 0, h = plb->GetCount (); i < h; i++) {
		plb->GetText (i, m_missionData.levelList [i]);
		if (psz = strchr (m_missionData.levelList [i], ',')) {
			nSecret = atoi (psz + 1);
			if (nSecret > m_missionData.numLevels) {
				sprintf_s (szError, sizeof (szError), "%s: Invalid secret level source number.", m_missionData.levelList [i]);
				ErrorMsg (szError);
				continue;
				}
			else if (!nSecret) {
				*psz = '\0';
				strncpy_s (szSecret, sizeof (szSecret), psz + 1, sizeof (szSecret));
				szSecret [sizeof (szSecret) - 1] = '\0';
				for (nSecret = j = 0; nSecret < m_missionData.numLevels; nSecret++) {
					if (strchr (m_missionData.levelList [nSecret], ','))
						j++;	//count secret level info mixed with level info - not a good idea to mix though
					else if (!strcmp (szSecret, m_missionData.levelList [nSecret])) {
						sprintf_s (psz + 1, sizeof (m_missionData.levelList [i]) - ((psz + 1) - m_missionData.levelList [i]), ",%d", nSecret - j + 1);
						break;
						}
					}
				}
			}
		if ((nSecret >= 0) && (nSecret < m_missionData.numLevels))
			m_missionData.numSecrets++;
		else
			m_missionData.numLevels++;
		}
	}
else
{
	BuildLevelList();
}
DDX_Text (pDX, IDC_MISSION_LEVELEDIT, m_szLevel, sizeof (m_szLevel));
DDX_Text (pDX, IDC_MISSION_COMMENT, m_missionData.comment, sizeof (m_missionData.comment));
}

//------------------------------------------------------------------------------

void CMissionTool::OnOK (void)
{
UpdateData (TRUE);
memcpy (&missionData, &m_missionData, sizeof (missionData));
WriteMissionFile (DLE.GetDocument ()->File (), DLE.LevelVersion ());
}

//------------------------------------------------------------------------------

void CMissionTool::OnCancel (void)
{
memcpy (&m_missionData, &missionData, sizeof (missionData));
UpdateData (FALSE);
}

//------------------------------------------------------------------------------

LPSTR CMissionTool::CopyLevelName (LPSTR pszDest, LPSTR pszSrc)
{
strncpy_s (pszDest, 13, pszSrc, 12);
pszDest [12] = '\0';
_strlwr_s (pszDest, 13);
return pszDest;
}

//------------------------------------------------------------------------------

LPSTR CMissionTool::FixLevelName (LPSTR pszName)
{
	char *psz;

if (psz = strchr (pszName, '.'))
	*psz = '\0';
int l = int (strlen (pszName));
if (l > 8)
	pszName [8] = '\0';
strcat_s (pszName, 13, DLE.IsD1File () ? ".rdl" : ".rl2");
return pszName;
}

//------------------------------------------------------------------------------

void CMissionTool::OnSetLevelName ()
{
UpdateData (TRUE);
}

//------------------------------------------------------------------------------

void CMissionTool::OnAdd ()
{
int i = LBLevels ()->GetCurSel ();

UpdateData (TRUE);
if (*m_szLevel) {
	char szLevel [13], szSecret [13];
	char *psz = strchr (m_szLevel, ',');
	if (psz) {
		*psz = '\0';
		FixLevelName (CopyLevelName (szLevel, m_szLevel));
		FixLevelName (CopyLevelName (szSecret, psz + 1));
		int j = LBLevels ()->FindStringExact (-1, szLevel);
		if (j != LB_ERR) {
			sprintf_s (m_szLevel, sizeof (m_szLevel), "%s,%d", szSecret, j + 1);
			j = LBLevels ()->FindString (-1, szSecret);
			}
		else {
			j = atoi (szSecret);
			if (j <= 0) {
				STATUSMSG ("invalid secret level");
				return;
				}
			sprintf_s (m_szLevel, sizeof (m_szLevel), "%s,%d", szLevel, j);
			j = LBLevels ()->FindString (-1, szLevel);
			}
		if (j != LB_ERR)
			LBLevels ()->DeleteString (j);
		}
	else
		FixLevelName (m_szLevel);
	LBLevels ()->InsertString (i, m_szLevel);
	*m_szLevel = '\0';
   AfxSetWindowText (((CWnd *) GetDlgItem (IDC_MISSION_LEVELEDIT))->GetSafeHwnd (), "");
	}
}

//------------------------------------------------------------------------------

void CMissionTool::OnDelete ()
{
	int i = LBLevels ()->GetCurSel ();

if ((i >= 0) && (i < LBLevels ()->GetCount ()))
	LBLevels ()->DeleteString (i);
}

//------------------------------------------------------------------------------

void CMissionTool::OnRename ()
{
	int i = LBLevels ()->GetCurSel ();

if ((i >= 0) && (i < LBLevels ()->GetCount ())) {
	UpdateData (TRUE);
	if (*m_szLevel) {
		FixLevelName (m_szLevel);
		LBLevels ()->DeleteString (i);
		_strlwr_s (m_szLevel, sizeof (m_szLevel));
		LBLevels ()->InsertString (i, m_szLevel);
		LBLevels ()->SetCurSel (i);
		strncpy_s (m_missionData.levelList [i], sizeof (m_missionData.levelList [i]), m_szLevel, sizeof (m_missionData.levelList [i]));
		m_missionData.levelList [i] [sizeof (m_missionData.levelList [i]) - 1] = '\0';
		*m_szLevel = '\0';
	   AfxSetWindowText (((CWnd *) GetDlgItem (IDC_MISSION_LEVELEDIT))->GetSafeHwnd (), "");
		}
	}
}

//------------------------------------------------------------------------------

void CMissionTool::OnUp ()
{
	int i = LBLevels ()->GetCurSel ();

if (i > 0) {
	char	szLevel [26];
	LBLevels ()->GetText (i, szLevel);
	LBLevels ()->DeleteString (i);
	LBLevels ()->InsertString (--i, szLevel);
	LBLevels ()->SetCurSel (i);
	}	
}

//------------------------------------------------------------------------------

void CMissionTool::OnDown ()
{
	int h = LBLevels ()->GetCount ();
	int i = LBLevels ()->GetCurSel ();

if ((i >= 0) && (i < h - 1)) {
	char	szLevel [26];
	LBLevels ()->GetText (i, szLevel);
	LBLevels ()->DeleteString (i);
	LBLevels ()->InsertString (++i, szLevel);
	LBLevels ()->SetCurSel (i);
	}	
}

//------------------------------------------------------------------------------

void CMissionTool::OnFromHog ()
{
	if (!hogManager->ReadData(DLE.GetDocument()->File(), false, true, false))
	{
		::RefreshHogFileList(LBLevels());
		UpdateData(TRUE);
	}
}

//------------------------------------------------------------------------------

void CMissionTool::OnLoadLevel (void)
{
	char	szNewFile [256];

int i = LBLevels ()->GetCurSel ();
if (i >= LBLevels ()->GetCount ())
	return;
LBLevels ()->GetText (i, szNewFile);
char *psz = strchr (szNewFile, ',');
if (psz)
	*psz = '\0';
if (!DLE.GetDocument ()->OpenFile (false, null, szNewFile))
	ErrorMsg ("Couldn't load requested level.");
}

//------------------------------------------------------------------------------

int CMissionTool::LevelNumber (char* pszLevel)
{
	int l = int (strlen (pszLevel));

for (int i = 0, j = missionData.numLevels + missionData.numSecrets; i < j; i++) {
	if (!_strnicmp (missionData.levelList [i], pszLevel, l))
		return (i < missionData.numLevels) ? i + 1 : -i - 1;
	}
return 0;
}

//------------------------------------------------------------------------------

//eof prefsdlg.cpp