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
    &missionData };

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

void GlobalData::ResetMineViewDistance()
{
    DLE.MineView()->SetViewDistIndex(0);
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

void GlobalData::Trace(TraceLevel level, std::string message)
{
    switch (level)
    {
    case Error:
        ErrorMsg(message.c_str());
        break;
    case Warning:
        if (!DLE.ExpertMode())
        {
            ErrorMsg(message.c_str());
        }
        break;
    case Info:
        INFOMSG(message.c_str());
        break;
    case Verbose:
        break;
    default:
        break;
    }
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

const char* GlobalData::GetMissionFolder()
{
    return missionFolder;
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

std::vector<byte> GlobalData::LoadPofNames()
{
    return ::LoadResourceAsBlob(DLE.IsD1File() ? IDR_POF_NAMES1 : IDR_POF_NAMES2);
}

std::vector<byte> GlobalData::LoadNewLevelBlob(int gameVersion)
{
    int nResource = (gameVersion == 0) ? IDR_NEW_RDL : IDR_NEW_RL2;
    return ::LoadResourceAsBlob(nResource);
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
    LoadTextureFromResource(&texture, IDR_ARROW_TEXTURE);
}

void GlobalData::LoadIconTexture(int iconNumber, CTexture& texture)
{
    LoadTextureFromResource(&texture, IDR_SMOKE_ICON + iconNumber);
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

void GlobalData::SetDocumentModifiedFlag(bool modified)
{
    DLE.GetDocument()->SetModifiedFlag(modified);
}

void GlobalData::ResetSelections()
{
    DLE.MainFrame()->SetSelectMode(SelectMode::Side);
    current->Reset();
    other->Reset();
}

//------------------------------------------------------------------------------


