//***************************************************************************
//
//  NAME:    GLOBAL.CPP
//  
//	PURPOSE:
//	
//	  BY:      Bryan Aamot
//	  
//		REV:
//		
//***************************************************************************

#include "stdafx.h"
#include "DrawHelpers.h"
#include "FileManager.h"

char message [4096];
char descentFolder [2][256] = {"\\programs\\d2\\data", "\\programs\\d2\\data"};
char missionFolder [256]= "\\programs\\d2\\missions";
char modFolder [256]= "\\programs\\d2\\mods";
char szPlayerProfile [20] = "";

CMissionData missionData;

GlobalData g_data{ &lightManager, &modelManager, &objectManager,
    &paletteManager, &robotManager, &segmentManager, &textureManager,
    &triggerManager, &wallManager, &vertexManager, &undoManager,
    current, other, &missionData };

bool GlobalData::IsD1File()
{
    return DLE.IsD1File();
}

bool GlobalData::IsD2File()
{
    return DLE.IsD2File();
}

int GlobalData::FileType()
{
    return DLE.FileType();
}

int GlobalData::FileVersion()
{
    return DLE.FileVersion();
}

int GlobalData::LevelType()
{
    return DLE.LevelType();
}

int GlobalData::LevelVersion()
{
    return DLE.LevelVersion();
}

bool GlobalData::IsStdLevel()
{
    return DLE.IsStdLevel();
}

bool GlobalData::IsD2XLevel()
{
    return LevelVersion() >= 9;
}

void GlobalData::RefreshMineView(bool all)
{
    DLE.MineView()->Refresh(all);
}

void GlobalData::DelayMineViewRefresh(bool addDelay)
{
    // This function more or less manages a refcount. It's also probably a code smell.
    DLE.MineView()->DelayRefresh(addDelay);
}

void GlobalData::ResetMineView()
{
    DLE.MineView()->ResetView(true);
}

void GlobalData::AdvanceMineViewSide()
{
    DLE.MineView()->NextSide();
}

void GlobalData::RefreshToolView()
{
    DLE.ToolView()->Refresh();
}

void GlobalData::EnsureValidSelection()
{
    // wrap back then forward to make sure segment is valid
    Wrap(current->m_nSegment, -1, 0, ::segmentManager.Count() - 1);
    Wrap(current->m_nSegment, 1, 0, ::segmentManager.Count() - 1);
    Wrap(other->m_nSegment, -1, 0, ::segmentManager.Count() - 1);
    Wrap(other->m_nSegment, 1, 0, ::segmentManager.Count() - 1);
}

void GlobalData::DoInfoMsg(const char* msg)
{
    INFOMSG(msg);
}

void GlobalData::DoErrorMsg(const char* msg)
{
    ErrorMsg(msg);
}

void GlobalData::DoStatusMsg(const char* msg)
{
    STATUSMSG(msg);
}

int GlobalData::DoQueryMsg(const char* msg)
{
    return QueryMsg(msg);
}

int GlobalData::DoQuery2Msg(const char* msg, uint type)
{
    return Query2Msg(msg, type);
}

int GlobalData::ExpertMode()
{
    return DLE.ExpertMode();
}

bool GlobalData::DoInputDialog(const char* title, const char* prompt, char* buffer, size_t bufferSize)
{
    CInputDialog dlg(DLE.MainFrame(), title, prompt, buffer, bufferSize);
    return dlg.DoModal() == IDOK;
}

void GlobalData::InitProgress(int maxPosition)
{
    DLE.MainFrame()->InitProgress(maxPosition);
}

void GlobalData::UpdateProgress(int position)
{
    DLE.MainFrame()->Progress().SetPos(position);
}

void GlobalData::StepProgress()
{
    DLE.MainFrame()->Progress().StepIt();
}

void GlobalData::CleanupProgress()
{
    DLE.MainFrame()->Progress().DestroyWindow();
}

IRenderer* GlobalData::GetRenderer()
{
    return &DLE.MineView()->Renderer();
}

IFileManager* GlobalData::CreateFileManager()
{
    return new CFileManager();
}

const char* GlobalData::GetD1Path()
{
    return descentFolder[0];
}

const char* GlobalData::GetD2Path()
{
    return descentFolder[1];
}

void GlobalData::SetD2Path(const char* newPath)
{
    strcpy_s(descentFolder[1], newPath);
}

const char* GlobalData::GetAppFolder()
{
    return DLE.AppFolder();
}

double GlobalData::GetMineMoveRate()
{
    return DLE.MineView()->MineMoveRate();
}

std::vector<byte> GlobalData::LoadResourceAsBlob(const char* resourceName)
{
    CResource res;
    if (!res.Load(resourceName))
    {
        return std::vector<byte>();
    }
    return std::vector<byte>(res.Data(), res.Data() + res.Size());
}

std::vector<byte> LoadResourceAsBlob(int resourceId)
{
    CResource res;
    if (!res.Load(resourceId))
    {
        return std::vector<byte>();
    }
    return std::vector<byte>(res.Data(), res.Data() + res.Size());
}

std::vector<byte> GlobalData::GetDefaultLightTable()
{
    return ::LoadResourceAsBlob(DLE.IsD1File() ? IDR_LIGHT_D1 : IDR_LIGHT_D2);
}

std::vector<byte> GlobalData::GetDefaultColorTable()
{
    return ::LoadResourceAsBlob(DLE.IsD1File() ? IDR_COLOR_D1 : IDR_COLOR_D2);
}

std::vector<std::string> GlobalData::LoadTextureNames(int gameVersion)
{
    std::vector<std::string> textureNames;
    int nResource = (gameVersion == 0) ? TEXTURE_STRING_TABLE_D1 : TEXTURE_STRING_TABLE_D2;
    int textureCount = textureManager->MaxTextures(gameVersion);

    for (int i = 0; i < textureCount; i++)
    {
        CStringResource res;
        res.Load(nResource + i);
        textureNames.push_back(std::string(res.Value()));
    }

    return std::move(textureNames);
}

std::vector<byte> GlobalData::LoadTextureIndex(int gameVersion)
{
    return ::LoadResourceAsBlob(gameVersion ? IDR_TEXTURE2_DAT : IDR_TEXTURE_DAT);
}

void GlobalData::LoadArrowTexture(CTexture& texture)
{
    DrawHelpers::LoadTextureFromResource(&texture, IDR_ARROW_TEXTURE);
}

void GlobalData::LoadIconTexture(int iconNumber, CTexture& texture)
{
    DrawHelpers::LoadTextureFromResource(&texture, IDR_SMOKE_ICON + iconNumber);
}

bool GlobalData::MakeModFolders(const char* subfolderName)
{
    return DLE.MakeModFolders(subfolderName);
}

const char* GlobalData::GetModFolder(int folderNumber)
{
	return DLE.m_modFolders[folderNumber];
}

void GlobalData::ResetTextureView()
{
    DLE.TextureView()->Setup();
    DLE.TextureView()->Refresh();
}

void GlobalData::RefreshObjectTool()
{
    DLE.ToolView()->ObjectTool()->Refresh();
}

int GetPaletteResourceId(const char* pszName)
{
    const std::map<std::string, int> palettes{
        {"groupa", IDR_GROUPA_256},
        {"alien1", IDR_ALIEN1_256},
        {"alien2", IDR_ALIEN2_256},
        {"fire", IDR_FIRE_256},
        {"water", IDR_WATER_256},
        {"ice", IDR_ICE_256},
        {"descent", IDR_PALETTE_256},
        {"", 0}
    };

    char szName[256]{};
    if (!pszName || !*pszName)
    {
        if (theMine && DLE.IsD1File())
        {
            return IDR_PALETTE_256;
        }
        CFileManager::SplitPath(descentFolder[1], nullptr, szName, nullptr);
    }
    else
    {
        CFileManager::SplitPath(pszName, nullptr, szName, nullptr);
    }
    _strlwr(szName);

    if (palettes.count(std::string(szName)) != 0)
    {
        return palettes.at(std::string(szName));
    }
    return IDR_GROUPA_256;
}

std::vector<byte> GlobalData::LoadPaletteData(const char* paletteName)
{
    auto resourceId = GetPaletteResourceId(paletteName);
    return ::LoadResourceAsBlob(resourceId);
}

UINT GlobalData::GetPaletteEntries(UINT nStartIndex, UINT nNumEntries, LPPALETTEENTRY lpPaletteColors)
{
    return RenderCurrentPalette()->GetPaletteEntries(nStartIndex, nNumEntries, lpPaletteColors);
}

void GlobalData::SetDocumentModifiedFlag(bool modified)
{
    DLE.GetDocument()->SetModifiedFlag(modified);
}

bool GlobalData::GLCreateTexture(CTexture* texture, bool bForce)
{
    return DrawHelpers::GLCreateTexture(texture, bForce);
}

GLuint GlobalData::GLBindTexture(const CTexture* texture, GLuint nTMU, GLuint nMode)
{
    return DrawHelpers::GLBindTexture(texture, nTMU, nMode);
}

void GlobalData::GLReleaseTexture(CTexture* texture)
{
    return DrawHelpers::GLReleaseTexture(texture);
}

//------------------------------------------------------------------------------


