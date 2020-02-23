#include "stdafx.h"
#include "dle-xp.h"
#include "PogDialog.h"
#include <stdarg.h>

#define ICONLIST_ICON_SIZE 16
#define ICONLIST_SIZE_INITIAL 200
#define ICONLIST_SIZE_INCREMENT 50

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CPogDialog, CDialog)
	ON_NOTIFY (LVN_ITEMCHANGED, IDC_POGMANAGER_LIST, OnTextureListSelectionChanged)
	ON_NOTIFY (NM_CLICK, IDC_POGMANAGER_LIST, OnTextureListClick)
	ON_BN_CLICKED (IDC_POGMANAGER_REVERT, OnRevert)
	ON_BN_CLICKED (IDC_POGMANAGER_EDIT, OnEdit)
	ON_BN_CLICKED (IDC_POGMANAGER_IMPORT, OnImport)
	ON_BN_CLICKED (IDC_POGMANAGER_EXPORT, OnExport)
	ON_BN_CLICKED (IDC_POGMANAGER_FILTER_CUSTOM, OnShowAll)
	ON_BN_CLICKED (IDC_POGMANAGER_FILTER_LEVEL, OnChangeFilter)
	ON_BN_CLICKED (IDC_POGMANAGER_FILTER_ROBOT, OnChangeFilter)
	ON_BN_CLICKED (IDC_POGMANAGER_FILTER_POWERUP, OnChangeFilter)
	ON_BN_CLICKED (IDC_POGMANAGER_FILTER_MISC, OnChangeFilter)
	ON_CBN_SELCHANGE (IDC_POGMANAGER_PALETTE, OnChangePalette)
	ON_BN_CLICKED (IDOK, OnOK)
	ON_BN_CLICKED (IDCANCEL, OnCancel)
	ON_WM_PAINT ()
END_MESSAGE_MAP ()

//------------------------------------------------------------------------------

CPogDialog::CPogDialog (CWnd *pParentWnd, bool bLevelLoaded, bool bPreselectTexture, uint uiSelectedTexture) :
	CDialog (IDD_POGMANAGER, pParentWnd),
	CDlgHelpers (this)
{
ZeroMemory (m_filters, sizeof (m_filters));
m_filters [TextureFilters_Level] = true;
m_bPaletteQueryDone = false;
m_bShowCustomOnly = true;
m_nTexPreviousFocused = -1;
m_nTexCurrentFocused = -1;
m_iSavedSelectedItem = -1;
m_bLevelLoaded = bLevelLoaded;
m_bPreselectTexture = bPreselectTexture;
m_uiPreselectedTexture = uiSelectedTexture;
if (m_bPreselectTexture && textureManager.Available ()) // always switch on the filter for the preselected texture
	m_filters [ClassifyTexture (textureManager.Textures (m_uiPreselectedTexture))] = true;
memset (m_szPreviousPigPath, 0, sizeof (m_szPreviousPigPath));
}

//------------------------------------------------------------------------------

CPogDialog::~CPogDialog (void)
{
}

//------------------------------------------------------------------------------

BOOL CPogDialog::OnInitDialog (void)
{
if (!textureManager.Available ()) {
	ErrorMsg ("A valid Descent 2 PIG file must be loaded before the custom texture\n"
	          "manager can be launched. Please select one from the Settings tool.");
	OnCancel ();
	return FALSE;
	}

CDialog::OnInitDialog ();
m_preview.SubclassDlgItem (IDC_POGMANAGER_PREVIEW, this);
int columnNum = 0;
TextureList ()->InsertColumn (columnNum++, "Texture", LVCFMT_LEFT, 100);
TextureList ()->InsertColumn (columnNum++, "Dimensions", LVCFMT_LEFT, 70, 1);
TextureList ()->InsertColumn (columnNum++, "Custom", LVCFMT_LEFT, 60, 2);
if (m_bLevelLoaded)
	TextureList ()->InsertColumn (columnNum++, "Used", LVCFMT_LEFT, 60, 3);
TextureList ()->InsertColumn (columnNum++, "Animated", LVCFMT_LEFT, 60, 4);
TextureList ()->InsertColumn (columnNum++, "Transparent", LVCFMT_LEFT, 60, 5);
TextureList ()->InsertColumn (columnNum++, "See-thru", LVCFMT_LEFT, 60, 6);
RebuildTextureList ();
if (m_bPreselectTexture)
	SetFocusedTexture (GetTextureListIndexFromId (textureManager.Index (m_uiPreselectedTexture)));

const char *pszCurrentPalette = null;
int nVersion = textureManager.Version ();
for (int i = 0; i < paletteManager.NumAvailablePalettes (nVersion); i++) {
	char szPaletteFileName [15];
	PaletteList ()->AddString (paletteManager.AvailablePaletteName (i, nVersion));
	sprintf_s (szPaletteFileName, sizeof (szPaletteFileName), "%s.pig", paletteManager.AvailablePaletteName (i, nVersion));
	if (_stricmp (szPaletteFileName, paletteManager.Name ()) == 0)
		pszCurrentPalette = paletteManager.AvailablePaletteName (i, nVersion);
	}
PaletteList ()->SelectString (-1, pszCurrentPalette);
UpdateData (FALSE);
return TRUE;
}

//------------------------------------------------------------------------------

BOOL CPogDialog::DestroyWindow (void)
{
for (int i = 0; i < TextureList ()->GetItemCount (); i++) {
	TextureListItemData *pData = GetTextureListItemData (i);
	if (pData != NULL)
		delete pData;
	}
return CDialog::DestroyWindow ();
}

//------------------------------------------------------------------------------

void CPogDialog::DoDataExchange (CDataExchange *pDX)
{
CDialog::DoDataExchange (pDX);
DDX_Check (pDX, IDC_POGMANAGER_FILTER_CUSTOM, m_bShowCustomOnly);
DDX_Check (pDX, IDC_POGMANAGER_FILTER_LEVEL, m_filters [TextureFilters_Level]);
DDX_Check (pDX, IDC_POGMANAGER_FILTER_ROBOT, m_filters [TextureFilters_Robot]);
DDX_Check (pDX, IDC_POGMANAGER_FILTER_POWERUP, m_filters [TextureFilters_Powerup]);
DDX_Check (pDX, IDC_POGMANAGER_FILTER_MISC, m_filters [TextureFilters_Misc]);
if (!pDX->m_bSaveAndValidate) {
	int bTransparent = (GetFocusedTextureIndex () >= 0) ? GetFocusedTexture ()->IsTransparent () : false;
	int bSeeThru = (GetFocusedTextureIndex () >= 0) ? GetFocusedTexture ()->IsSuperTransparent () : false;
	DDX_Check (pDX, IDC_POGMANAGER_TRANSP_CHECK, bTransparent);
	DDX_Check (pDX, IDC_POGMANAGER_SEETHRU_CHECK, bSeeThru);
	UpdateTexturePreviewAndControls ();
	}
}

//------------------------------------------------------------------------------

void CPogDialog::OnPaint (void)
{
CDialog::OnPaint ();
// This is mainly needed to draw preselected textures on dialog init
const CTexture *pFocusTexture = GetFocusedTexture ();
if (pFocusTexture && pFocusTexture->IsAnimated ())
	m_preview.StartAnimation (pFocusTexture);
else
	PaintTexture (&m_preview, IMG_BKCOLOR, pFocusTexture);
}

//------------------------------------------------------------------------------

void CPogDialog::RebuildTextureList (void)
{
	bool expandAnim = false;
	uint nExpandAnimTextureIndex = 0;
	const CTexture *pFocusTexture = GetFocusedTexture ();

// If the parent (or first frame due to the behavior of UpdateTextureListFrameExpansion) is selected
// it will be expanded when we restore focus; subframes are the only case that needs special treatment
if (pFocusTexture && pFocusTexture->FrameNumber () > 0) {
	expandAnim = true;
	nExpandAnimTextureIndex = pFocusTexture->GetParent ()->Index ();
	}

if (m_bLevelLoaded)
	textureManager.TagUsedTextures ();

TextureList ()->SetRedraw (FALSE);
for (int i = 0; i < TextureList ()->GetItemCount (); i++) {
	TextureListItemData *pData = GetTextureListItemData (i);
	if (pData != NULL)
		delete pData;
	}
TextureList ()->DeleteAllItems ();
m_customTextureIcons.DeleteImageList ();
m_customTextureIcons.Create (ICONLIST_ICON_SIZE, ICONLIST_ICON_SIZE, ILC_COLOR32, ICONLIST_SIZE_INITIAL, ICONLIST_SIZE_INCREMENT);
TextureList ()->SetImageList (&m_customTextureIcons, LVSIL_SMALL);

int nListItem = 0;
for (uint nIndex = 0; nIndex < (uint) textureManager.GlobalTextureCount (); nIndex++) {
	const CTexture *pTexture = textureManager.TextureByIndex (nIndex);
#if DBG
	if (!strcmp (pTexture->Name (), "exp03"))
		nIndex = nIndex;
#endif
	if (pTexture->IsAnimated ()) {
		// Don't add individual frames to the list, we'll do that later
		nIndex += pTexture->GetFrameCount () - 1;
		}
	if (!IsTextureIncluded (pTexture))
		continue;

	AddTextureListItem (nListItem, pTexture, false);

	// Only increment this when we actually process an item
	nListItem++;
	}

if (expandAnim)
	AddTextureListFrames (textureManager.TextureByIndex (nExpandAnimTextureIndex));

TextureList ()->SortItems (&CPogDialog::CompareTextures, 0);
m_nTexPreviousFocused = -1;
m_nTexCurrentFocused = -1;
if (pFocusTexture) {
	int nFocusTexture = GetTextureListIndexFromId (pFocusTexture->Index ());
	if (nFocusTexture >= 0)
		// Focused texture still exists, restore state
		SetFocusedTexture ((uint)nFocusTexture);
	}
TextureList ()->SetRedraw (TRUE);
}

//------------------------------------------------------------------------------

void CPogDialog::AddTextureListFrames (const CTexture *pTexture)
{
if (pTexture->GetFrameCount () <= 1)
	return;

// Find the list item index for this texture
int nListItem = GetTextureListIndexFromId (pTexture->Index ());
if (nListItem < 0)
	return;

// Add all frames from animated texture
for (uint nFrame = 0; nFrame < pTexture->GetFrameCount (); nFrame++) {
	const CTexture *pFrame = pTexture->GetFrame (nFrame);
	char szLabel [40] = {0};
	int columnNum = 1;
	nListItem++;

	AddTextureListItem (nListItem, pFrame, true);

	if (nFrame == 0 && pFrame->IsSingleImageAnimation ())
		// Skipping the unused frames to (hopefully) avoid confusion
		break;
	}
}

//------------------------------------------------------------------------------

uint CPogDialog::RemoveTextureListFrames (const CTexture *pTexture)
{
	uint nFramesRemoved = 0;

if (pTexture->GetFrameCount () <= 1)
	return nFramesRemoved;

// Find the list item index for this texture
int nListItem = GetTextureListIndexFromId (pTexture->Index ());
if (nListItem < 0)
	return nFramesRemoved;

// Remove all frames from animated texture
nListItem++; // Not the root itself though
TextureListItemData *pData = GetTextureListItemData (nListItem);
while (pData != NULL && pData->bIsFrame && pData->uiParentTextureIndex == pTexture->Index ()) {
	nFramesRemoved++;
	delete pData;
	TextureList ()->DeleteItem (nListItem);
	pData = GetTextureListItemData (nListItem);
	}

return nFramesRemoved;
}

void CPogDialog::UpdateTextureListItem (int nListItem)
{
	LVITEM item = {0};
	char szLabel [40] = {0};
	int columnNum = 1;

if (nListItem < 0)
	return;

const CTexture *pTexture = GetTextureAtIndex ((uint)nListItem);

// Replace item in image list (if any)
item.iItem = nListItem;
item.mask = LVIF_IMAGE;
TextureList ()->GetItem (&item);
if (item.iImage != -1) {
	CBitmap *pbmImage = null;
	pTexture->CreateBitmap (&pbmImage, true, ICONLIST_ICON_SIZE);
	m_customTextureIcons.Replace (item.iImage, pbmImage, (CBitmap *)null);
	delete pbmImage;
	}

// Dimensions
sprintf_s (szLabel, ARRAYSIZE (szLabel), "%ux%u", pTexture->Width (), pTexture->Height ());
TextureList ()->SetItemText (nListItem, columnNum++, szLabel);
// Custom
sprintf_s (szLabel, ARRAYSIZE (szLabel), pTexture->IsCustom () ? "Yes" : "No");
TextureList ()->SetItemText (nListItem, columnNum++, szLabel);
// Used currently does not change while the dialog is open so we won't update it here
// Animated currently does not change so we won't update it here
// Transparent
sprintf_s (szLabel, ARRAYSIZE (szLabel), pTexture->IsTransparent () ? "Yes" : "No");
TextureList ()->SetItemText (nListItem, columnNum++, szLabel);
// See-thru
sprintf_s (szLabel, ARRAYSIZE (szLabel), pTexture->IsSuperTransparent () ? "Yes" : "No");
TextureList ()->SetItemText (nListItem, columnNum++, szLabel);

// Display changes immediately
TextureList ()->Update (nListItem);
}

//------------------------------------------------------------------------------

void CPogDialog::AddTextureListItem (int nListItem, const CTexture *pTexture, bool asFrame)
{
	int nTexList = -1;
	char szLabel [40] = { 0 };
	int columnNum = 1;
	TextureListItemData *pData = new TextureListItemData;
	pData->uiIndex = pTexture->Index ();
	pData->bIsFrame = asFrame;

// For non-frame items, we add a thumbnail to the list. For frames we don't,
// because they're not particularly helpful for identification
if (!asFrame) {
	// Scale down texture to 16x16 and add to image list
	CBitmap *pbmImage = null;
	pTexture->CreateBitmap (&pbmImage, true, ICONLIST_ICON_SIZE);
	// Mask not used, second parameter ignored
	m_customTextureIcons.Add (pbmImage, (CBitmap *)null);
	delete pbmImage;

	// nTexList is probably equal to nListItem but don't want to assume that
	nTexList = m_customTextureIcons.GetImageCount () - 1;
	sprintf_s (szLabel, ARRAYSIZE (szLabel), pTexture->Name ());
	pData->uiParentTextureIndex = pData->uiIndex;
	}
else {
	if (pTexture->IsSingleImageAnimation ()) {
		sprintf_s (szLabel, ARRAYSIZE (szLabel), "Frames 1-%u", pTexture->GetFrameCount ());
		pData->uiParentTextureIndex = pData->uiIndex;
		}
	else {
		sprintf_s (szLabel, ARRAYSIZE (szLabel), "Frame %u", pTexture->FrameNumber () + 1);
		pData->uiParentTextureIndex = pTexture->GetParent ()->Index ();
		}
	}

// nTexList of -1 prevents an icon being shown
TextureList ()->InsertItem (nListItem, szLabel, nTexList);
TextureList ()->SetItemData (nListItem, (DWORD_PTR)pData);
if (asFrame)
	TextureList ()->SetItem (nListItem, 0, LVIF_INDENT, null, 0, 0, 0, null, 1);

// Dimensions
sprintf_s (szLabel, ARRAYSIZE (szLabel), "%ux%u", pTexture->Width (), asFrame ? pTexture->Height () : pTexture->FrameHeight ());
TextureList ()->SetItemText (nListItem, columnNum++, szLabel);
// Custom
sprintf_s (szLabel, ARRAYSIZE (szLabel), pTexture->IsCustom () ? "Yes" : "No");
TextureList ()->SetItemText (nListItem, columnNum++, szLabel);
// Used
if (m_bLevelLoaded) {
	uint uiNumInstances = textureManager.UsedCount (pTexture);
	if (asFrame)
		// Door frames can be used individually, but most frames can't
		sprintf_s (szLabel, ARRAYSIZE (szLabel), uiNumInstances > 0 ? "Yes (%u)" : "", uiNumInstances);
	else
		sprintf_s (szLabel, ARRAYSIZE (szLabel), uiNumInstances > 0 ? "Yes (%u)" : "No", uiNumInstances);
	TextureList ()->SetItemText (nListItem, columnNum++, szLabel);
	}
// Animated
if (asFrame)
	sprintf_s (szLabel, ARRAYSIZE (szLabel), "");
else
	sprintf_s (szLabel, ARRAYSIZE (szLabel), pTexture->IsAnimated () ? "Yes (%u frames)" : "No", pTexture->GetFrameCount ());
TextureList ()->SetItemText (nListItem, columnNum++, szLabel);
// Transparent
sprintf_s (szLabel, ARRAYSIZE (szLabel), pTexture->IsTransparent () ? "Yes" : "No");
TextureList ()->SetItemText (nListItem, columnNum++, szLabel);
// See-thru
sprintf_s (szLabel, ARRAYSIZE (szLabel), pTexture->IsSuperTransparent () ? "Yes" : "No");
TextureList ()->SetItemText (nListItem, columnNum++, szLabel);
}

//------------------------------------------------------------------------------
// Check the texture against the filters - return true if we should still show it
bool CPogDialog::IsTextureIncluded (const CTexture *pTexture)
{
	bool shouldInclude;

// Exclude invalid textures
if (!pTexture || strlen (pTexture->Name ()) == 0)
	return false;

if (m_bShowCustomOnly) {
	shouldInclude = pTexture->IsCustom ();
	// We want to include animated textures if ANY frame is custom
	if (!shouldInclude && pTexture->IsAnimated ())
		for (uint j = pTexture->Index (); j < pTexture->Index () + pTexture->GetFrameCount (); j++)
			if (pTexture->IsCustom ()) {
				shouldInclude = true;
				break;
				}
	}

// Preselected textures show up even if they aren't custom
if (m_bPreselectTexture) {
	// We actually need to compare the index, not level ID, because some level texture IDs
	// point to the same base texture (e.g. scrolling variants)
	if (pTexture->Index () == textureManager.Textures (m_uiPreselectedTexture)->Index ())
		shouldInclude = true;
	}

if (!m_filters [ClassifyTexture (pTexture)])
	shouldInclude = false;

return shouldInclude;
}

//------------------------------------------------------------------------------

CPogDialog::TextureFilters CPogDialog::ClassifyTexture (const CTexture *pTexture)
{
	char *robotTextures [] = {
		"rbot*", "eye*", "glow*", "boss*", "metl*", "ctrl*", "react*", "rmap*", "ship*",
		"energy01", "flare", "marker", "missile", "missiles", "missback", "water07"
	};
	char *powerupTextures [] = {
		"aftrbrnr", "allmap", "ammorack", "cloak", "cmissil*", "convert", "erthshkr",
		"flag01", "flag02", "fusion", "gauss", "headlite", "helix", "hmissil*", "hostage",
		"invuln", "key01", "key02", "key03", "laser", "life01", "merc*", "mmissile",
		"omega", "pbombs", "phoenix", "plasma", "quad", "spbombs", "spread", "suprlasr",
		"vammo", "vulcan"
	};

int nTexture = textureManager.TexIdFromIndex (pTexture->Index ());
if (nTexture >= 0)
	return TextureFilters_Level;

for (int i = 0; i < ARRAYSIZE(robotTextures); i++)
	if (strchr (robotTextures [i], '*')) {
		int count = strchr (robotTextures [i], '*') - robotTextures [i];
		if (_strnicmp (pTexture->Name (), robotTextures [i], count) == 0)
			return TextureFilters_Robot;
		}
	else if (_stricmp (pTexture->Name (), robotTextures [i]) == 0)
		return TextureFilters_Robot;

for (int i = 0; i < ARRAYSIZE(powerupTextures); i++)
	if (strchr (powerupTextures [i], '*')) {
		int count = strchr (powerupTextures [i], '*') - powerupTextures [i];
		if (_strnicmp (pTexture->Name (), powerupTextures [i], count) == 0)
			return TextureFilters_Powerup;
		}
	else if (_stricmp (pTexture->Name (), powerupTextures [i]) == 0)
		return TextureFilters_Powerup;

return TextureFilters_Misc;
}

//------------------------------------------------------------------------------

int CALLBACK CPogDialog::CompareTextures (LPARAM lParam1, LPARAM lParam2, LPARAM /*lParamSort*/)
{
	uint nIndex1 = ((TextureListItemData *)lParam1)->uiIndex;
	uint nIndex2 = ((TextureListItemData *)lParam2)->uiIndex;

return _stricmp (textureManager.TextureByIndex (nIndex1)->Name (), textureManager.TextureByIndex (nIndex2)->Name ());
}

//------------------------------------------------------------------------------

bool CPogDialog::ParentTexturesAreEqual (const CTexture *pTexture, const CTexture *pOtherTexture)
{
if (pTexture == pOtherTexture)
	return true;
if (!pTexture || !pOtherTexture)
	return false;
if (pTexture->IsAnimated () && pOtherTexture->IsAnimated () && pTexture->GetParent () == pOtherTexture->GetParent ())
	return true;
return false;
}

//------------------------------------------------------------------------------

void CPogDialog::UpdateTextureListFrameExpansion (void)
{
	const CTexture *pCurrentFocusedTexture = GetTextureFromId (m_nTexCurrentFocused);
	const CTexture *pPreviousFocusedTexture = GetTextureFromId (m_nTexPreviousFocused);

	// If previous and new selected textures are the same or share the same root, do nothing
#if 1
if (textureManager.ShareAnimation (-m_nTexCurrentFocused - 1, -m_nTexPreviousFocused - 1))
#else
if (ParentTexturesAreEqual (pCurrentFocusedTexture, pPreviousFocusedTexture))
#endif
	return;

if (pPreviousFocusedTexture && textureManager.IsAnimationFrame (-m_nTexPreviousFocused - 1)) {
	uint nFramesRemoved = RemoveTextureListFrames (pPreviousFocusedTexture->GetParent ());
	if (m_iSavedSelectedItem > GetTextureListIndexFromId (pPreviousFocusedTexture->GetParent ()->Index ()))
		m_iSavedSelectedItem -= nFramesRemoved;
	}

if (pCurrentFocusedTexture && pCurrentFocusedTexture->IsAnimated ())
	AddTextureListFrames (pCurrentFocusedTexture);
}

//------------------------------------------------------------------------------

void CPogDialog::UpdateTexturePreviewAndControls (void)
{
	uint uiSelectedCount = TextureList ()->GetSelectedCount ();
	const CTexture *pTexture = GetFocusedTexture ();
	bool isAnimatedTexture = IsAnimatedTextureRoot (GetFocusedTextureIndex ());
	bool is64x64Texture = pTexture && pTexture->Width () == 64 && pTexture->Height () == 64;

if (!m_preview)
	return;

// Currently the texture edit dialog is only designed to work on 64x64 textures - we'll disable it for the rest
EnableControls (IDC_POGMANAGER_EDIT, IDC_POGMANAGER_EDIT, uiSelectedCount == 1 && !isAnimatedTexture && is64x64Texture);
EnableControls (IDC_POGMANAGER_IMPORT, IDC_POGMANAGER_IMPORT, uiSelectedCount == 1 /*&& !isAnimatedTexture*/);
EnableControls (IDC_POGMANAGER_REVERT, IDC_POGMANAGER_EXPORT, uiSelectedCount > 0);

m_preview.StopAnimation ();

if (pTexture) {
	char szLabel [40] = {0};

	if (isAnimatedTexture)
		m_preview.StartAnimation (pTexture);
	else
		// For this one we want the entire image for a TGA animation (if this is one)
		PaintTexture (&m_preview, IMG_BKCOLOR, pTexture, null, 0, 0, 0, false);
	sprintf_s (szLabel, ARRAYSIZE (szLabel), "%ux%u", pTexture->Width (), pTexture->Height ());
	GetDlgItem (IDC_POGMANAGER_PREVIEW_SIZE)->SetWindowText (szLabel);
	}
else {
	PaintTexture (&m_preview, IMG_BKCOLOR, -1);
	GetDlgItem (IDC_POGMANAGER_PREVIEW_SIZE)->SetWindowText ("");
	}
}

//------------------------------------------------------------------------------

bool CPogDialog::IsAnimatedTextureRoot (uint uiTextureListIndex)
{
	const CTexture *pTexture = GetTextureAtIndex (uiTextureListIndex);

if (pTexture && pTexture->IsAnimated ()) {
	// First frame of the texture points to the same object as the animated heading - we don't
	// want to animate that. We can use the bIsFrame field in the list item data for this
	TextureListItemData *pData = GetTextureListItemData ((int) uiTextureListIndex);
	if (!pData->bIsFrame)
		return true;
	}
return false;
}

//------------------------------------------------------------------------------

void CPogDialog::ScrollTextureList (uint uiTextureListIndex)
{
if (uiTextureListIndex >= (uint)TextureList ()->GetItemCount ())
	return;
uint uiNumFrames = GetTextureAtIndex (uiTextureListIndex)->GetFrameCount ();
if (uiNumFrames > 1)
	// Animated texture, try to get the frames in view too
	TextureList ()->EnsureVisible ((int)uiTextureListIndex + uiNumFrames, FALSE);
TextureList ()->EnsureVisible ((int)uiTextureListIndex, FALSE);
}

//------------------------------------------------------------------------------

void CPogDialog::SetFocusedTexture (uint uiTextureListIndex)
{
if (uiTextureListIndex >= (uint)TextureList ()->GetItemCount ())
	return;
TextureList ()->SetItemState ((int)uiTextureListIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

//------------------------------------------------------------------------------

int CPogDialog::GetFocusedTextureIndex (void)
{
for (int i = 0; i < TextureList ()->GetItemCount (); i++) {
	if (TextureList ()->GetItemState (i, LVIS_FOCUSED) > 0)
		return i;
	}
return -1;
}

//------------------------------------------------------------------------------

int CPogDialog::GetTextureListIndexFromId (uint nIndex)
{
for (int i = 0; i < TextureList ()->GetItemCount (); i++) {
	if (GetTextureListItemData (i)->uiIndex == nIndex)
		return i;
	}
return -1;
}

//------------------------------------------------------------------------------

const CTexture *CPogDialog::GetFocusedTexture (bool bGetRootTexture)
{
int nFocused = GetFocusedTextureIndex ();
if (nFocused < 0) 
	return null;
if (!bGetRootTexture)
	return GetTextureAtIndex ((uint) nFocused);
int nIndex = textureManager.AnimationRoot (nFocused);
return GetTextureAtIndex ((uint) ((nIndex < 0) ? nFocused : nIndex));
}

//------------------------------------------------------------------------------

const CTexture *CPogDialog::GetTextureAtIndex (uint uiTextureListIndex)
{
if (uiTextureListIndex >= (uint)TextureList ()->GetItemCount ())
	return null;
uint nIndex = GetTextureListItemData (uiTextureListIndex)->uiIndex;
return textureManager.TextureByIndex (nIndex);
}

//------------------------------------------------------------------------------

bool CPogDialog::IsSingleTextureSelected (void)
{
int numSelectedTextures = 0;
for (int i = 0; i < TextureList ()->GetItemCount (); i++) {
	if (TextureList ()->GetItemState (i, LVIS_SELECTED) > 0) {
		numSelectedTextures++;
		}
	}
return numSelectedTextures == 1;
}

//------------------------------------------------------------------------------

bool CPogDialog::ExecuteOnSelectedTextures (ExecuteOnSelectedTexturesCallback callback, ...)
{
va_list vl;
va_start (vl, callback);
bool bExecuted = false;
for (int i = 0; i < TextureList ()->GetItemCount (); i++) {
	if (TextureList ()->GetItemState (i, LVIS_SELECTED) > 0) {
		va_list args;
#ifdef va_copy
		va_copy (args, vl);
#else
		args = vl;
#endif
		(this->*callback) (GetTextureAtIndex (i), args);
		va_end (args);
		bExecuted = true;
		}
	}
va_end (vl);
return bExecuted;
}

//------------------------------------------------------------------------------

void CPogDialog::OnTextureListSelectionChanged (NMHDR *pNMHDR, LRESULT *pResult)
{
NMLISTVIEW *pData = (NMLISTVIEW *)pNMHDR;
if ((pData->uNewState & (LVIS_FOCUSED | LVIS_SELECTED))
	  != (pData->uOldState & (LVIS_FOCUSED | LVIS_SELECTED))) {
	// Selection or focus has changed; controls will need updating
	if (pData->uNewState & LVIS_FOCUSED) {
		// Selection can get messed up by the mouse button up event after this, so we'll save it
		m_iSavedSelectedItem = pData->iItem;
		m_nTexPreviousFocused = m_nTexCurrentFocused;
		m_nTexCurrentFocused = GetTextureAtIndex (pData->iItem)->Index ();
		UpdateTextureListFrameExpansion ();
		// If we've just "opened" a new animated texture we want to scroll the list to fit it in view
#if 1
		if (!textureManager.ShareAnimation (-m_nTexCurrentFocused - 1, -m_nTexPreviousFocused - 1))
#else
		if (!ParentTexturesAreEqual (GetTextureFromId (m_nTexCurrentFocused), GetTextureFromId (m_nTexPreviousFocused)))
#endif
			ScrollTextureList ((uint) m_iSavedSelectedItem);
		}
	UpdateData (FALSE);
	}
}

//------------------------------------------------------------------------------

void CPogDialog::OnTextureListClick (NMHDR *pNMHDR, LRESULT *pResult)
{
NMITEMACTIVATE *pData = (NMITEMACTIVATE *)pNMHDR;
if (m_iSavedSelectedItem >= 0) {
	SetFocusedTexture (m_iSavedSelectedItem);
	m_iSavedSelectedItem = -1;
	}
}

//------------------------------------------------------------------------------

void CPogDialog::OnRevert (void)
{
if ((QueryMsg("Are you sure you want to revert these custom textures?") == IDYES)) {
	ExecuteOnSelectedTextures (&CPogDialog::RevertTexture);
	RebuildTextureList ();
	UpdateData (FALSE);
	}
}

//------------------------------------------------------------------------------

void CPogDialog::RevertTexture (const CTexture *pTexture, va_list args)
{
if (pTexture) {
	// If the texture still showed up with the custom texture filter on that might be confusing
	if (m_bPreselectTexture && pTexture->Id () == m_uiPreselectedTexture)
		m_bPreselectTexture = false;
	textureManager.RevertTexture (pTexture->Index ());
	}
}

//------------------------------------------------------------------------------

void CPogDialog::OnEdit (void)
{
if (!IsSingleTextureSelected ())
	return;

	CTextureEdit e (GetFocusedTexture (), this);

if (e.DoModal () == IDOK) { // was the texture actually changed?
	if (GetFocusedTexture ()->IsSingleImageAnimation ())
		RebuildTextureList ();
	else
		UpdateTextureListItem (GetFocusedTextureIndex ());
	UpdateData (FALSE);
	}
}

//------------------------------------------------------------------------------

void CPogDialog::OnImport (void)
{
if (!IsSingleTextureSelected ())
	return;

char szFile [MAX_PATH] = {0};
CFileManager::tFileFilter filters [] = {
		{ "Bitmap files", "bmp" },
		{ "TGA files", "tga" }
		};

if (!CFileManager::RunOpenFileDialog (szFile, ARRAYSIZE (szFile), filters, ARRAYSIZE (filters), m_hWnd))
	return;

const CTexture *pTexture = GetFocusedTexture (strstr (::_strlwr (szFile), ".tga") != null);

CTexture *pNewTexture = textureManager.OverrideTexture (pTexture->Index ());
if (pNewTexture->LoadFromFile (szFile)) {
	if (pNewTexture->IsSingleImageAnimation ())
		RebuildTextureList ();
	else
		UpdateTextureListItem (GetFocusedTextureIndex ());
	UpdateData (FALSE);
	}
}

//------------------------------------------------------------------------------

void CPogDialog::OnExport (void)
{
if (IsSingleTextureSelected ()) {
	char szFile [MAX_PATH] = {0};
	const CTexture *pTexture = GetFocusedTexture ();
	bool bGotFilename = false;
	switch (pTexture->Format ()) {
		case BMP:
			sprintf_s (szFile, ARRAYSIZE (szFile), "%s.bmp", pTexture->Name ());
			bGotFilename = CFileManager::RunSaveFileDialog (szFile, ARRAYSIZE (szFile), "256 color Bitmap Files", "bmp", m_hWnd);
			break;
		case TGA:
			sprintf_s (szFile, ARRAYSIZE (szFile), "%s.tga", pTexture->Name ());
			bGotFilename = CFileManager::RunSaveFileDialog (szFile, ARRAYSIZE (szFile), "Truevision Targa", "tga", m_hWnd);
			break;
		}
	if (bGotFilename)
		pTexture->Save (szFile);
	}
else {
	char szPath [MAX_PATH] = {0};
	if (CFileManager::RunMultiSaveDialog (szPath, ARRAYSIZE (szPath), "Choose a location to export the textures to:", m_hWnd))
		ExecuteOnSelectedTextures (&CPogDialog::ExportTexture, szPath);
	}
}

//------------------------------------------------------------------------------

void CPogDialog::ExportTexture (const CTexture *pTexture, va_list args)
{
char szFile [MAX_PATH] = {0};
char *szPath;
szPath = va_arg (args, char *);
const char *szExt;
switch (pTexture->Format ()) {
	case BMP:
		szExt = "bmp";
		break;
	case TGA:
		szExt = "tga";
		break;
	default:
		break;
	}
if (pTexture->FrameNumber () > 0)
	sprintf_s (szFile, ARRAYSIZE (szFile), "%s\\%s_%02d.%s", szPath, pTexture->Name (), pTexture->FrameNumber (), szExt);
else
	sprintf_s (szFile, ARRAYSIZE (szFile), "%s\\%s.%s", szPath, pTexture->Name (), szExt);
pTexture->Save (szFile);
}

//------------------------------------------------------------------------------

void CPogDialog::OnShowAll (void)
{
UpdateData (TRUE);
RebuildTextureList ();
UpdateData (FALSE); // selected texture might have changed
}

//------------------------------------------------------------------------------

void CPogDialog::OnChangeFilter (void)
{
	UpdateData (TRUE);
	RebuildTextureList ();
	UpdateData (FALSE); // selected texture might have changed
}

//------------------------------------------------------------------------------

void CPogDialog::OnChangePalette (void)
{
CString pigFileName;

if (textureManager.HasCustomTextures () && (m_bPaletteQueryDone ||
	   (QueryMsg ("Changing the palette will affect the existing custom textures in this POG file.\n"
	            "They will be re-indexed but some color detail is likely to be lost.\n\n"
	            "Are you sure you want to do this?") != IDYES))) {
	// Revert selection
	for (int i = 0; i < paletteManager.NumAvailablePalettes (textureManager.Version ()); i++) {
		PaletteList ()->GetLBText (i, pigFileName);
		pigFileName.Append (".pig");
		if (_stricmp (pigFileName.GetBuffer (), paletteManager.Name ()) == 0)
			PaletteList ()->SetCurSel (i);
		}
	return;
	}
// Only prompt once per dialog instance, we'll presume the user knows the risks afterward
m_bPaletteQueryDone = true;

// Construct the path of the new PIG file
char szPigPath [256];
PaletteList ()->GetLBText (PaletteList ()->GetCurSel (), pigFileName);
pigFileName.Append (".pig");
CFileManager::SplitPath (descentFolder [1], szPigPath, null, null);
// Save pre-edit location so we can revert if need be
if (!*m_szPreviousPigPath)
	sprintf_s (m_szPreviousPigPath, ARRAYSIZE (m_szPreviousPigPath), "%s%s", szPigPath, paletteManager.Name ());
strcat_s (szPigPath, sizeof (szPigPath), pigFileName.GetBuffer ());
textureManager.ChangePigFile (szPigPath);

// Reload images, colors might have changed
RebuildTextureList ();
UpdateData (FALSE);
}

//------------------------------------------------------------------------------

void CPogDialog::OnOK (void)
{
CDlcDoc *pDocument = DLE.GetDocument ();
ASSERT (pDocument); // we don't support stand-alone POGs yet
char szOverwritePogQuery [400] = { 0 };
if (DLE.IsD2File ()) {
	sprintf_s (szOverwritePogQuery, ARRAYSIZE (szOverwritePogQuery),
		"%d textures have been modified. Do you want to save these changes?\n\n"
		"(Select \'No\' to revert changes and exit, or \'Cancel\' to return to the texture editor.)",
		textureManager.CountModifiedTextures ());
	}
else {
	sprintf_s (szOverwritePogQuery, ARRAYSIZE (szOverwritePogQuery),
		"%d textures have been modified. Do you want to save these changes?\n"
		"Because this is a Descent 1 level, a DTX patch will be used. You will "
		"need the DTXPATCH utility to apply the patch, or alternatively it can "
		"automatically be loaded by D1X 1.00 or later.\n\n"
		"(Select \'No\' to revert changes and exit, or \'Cancel\' to return to the texture editor.)",
		textureManager.CountModifiedTextures ());
	}

// If this is a new level we won't save right now - we don't know where it's going. We do need
// to commit textures though, so that subsequent visits to this dialog can't wipe them just by
// clicking "cancel".
if (textureManager.CountModifiedTextures () > 0 && (*pDocument->File () == '\0'))
	textureManager.CommitTextureChanges ();
else if (textureManager.CountModifiedTextures () > 0) {
	switch (Query2Msg (szOverwritePogQuery, MB_YESNOCANCEL)) {
		case IDYES:
			textureManager.CommitTextureChanges ();
			if (DLE.IsD2File ()) {
				pDocument->SaveCustomFile (CUSTOM_FILETYPE_POG);
				}
			else {
				pDocument->SaveCustomFile (CUSTOM_FILETYPE_DTX);
				}
			break;
		case IDNO:
			textureManager.UndoTextureChanges ();
			break;
		case IDCANCEL:
			// Don't close dialog - bypass overridden method
			return;
		default:
			break;
		}
	}

CDialog::OnOK ();
}

//------------------------------------------------------------------------------

void CPogDialog::OnCancel (void)
{
static const char *szQuery = "There are unsaved texture changes in the POG file.\nDo you want to abandon these changes?";

if (textureManager.CountModifiedTextures() > 0 || *m_szPreviousPigPath) {
	if (QueryMsg (szQuery) == IDYES) {
		if (*m_szPreviousPigPath)
			textureManager.ChangePigFile (m_szPreviousPigPath);
		textureManager.UndoTextureChanges ();
		}
	else
		return;
	}
CDialog::OnCancel ();
}

//------------------------------------------------------------------------------
