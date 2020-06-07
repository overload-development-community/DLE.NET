#include "pch.h"
#include "resource.h"
#include "Delegates.h"
#include "FileManager.h"
#include "LightManager.h"
#include "ASEModel.h"
#include "ModelManager.h"
#include "ObjectManager.h"
#include "PaletteManager.h"
#include "RobotManager.h"
#include "TextureManager.h"
#include "WallManager.h"
#include "VertexManager.h"
#include "mine.h"

CMissionData missionData;

GlobalData g_data{ &lightManager, &modelManager, &objectManager,
    &paletteManager, &robotManager, &segmentManager, &textureManager,
    &triggerManager, &wallManager, &vertexManager, &undoManager,
    current, other, &missionData };

void GlobalData::RefreshMineView(bool all)
{
    //DLE.MineView()->Refresh(all);
}

void GlobalData::DelayMineViewRefresh(bool addDelay)
{
    // This function more or less manages a refcount. It's also probably a code smell.
    //DLE.MineView()->DelayRefresh(addDelay);
}

void GlobalData::ResetMineView()
{
    //DLE.MineView()->ResetView(true);
}

void GlobalData::ResetMineViewDistance()
{
    //DLE.MineView()->SetViewDistIndex(0);
}

void GlobalData::AdvanceMineViewSide()
{
    //DLE.MineView()->NextSide();
}

void GlobalData::RefreshToolView()
{
    //DLE.ToolView()->Refresh();
}

void GlobalData::EnsureValidSelection()
{
    // Called from block delete and undo. Will re-implement the undo part differently later.
    // UI can probably decide what to do about block delete.
}

void GlobalData::Trace(TraceLevel level, std::string message)
{
    auto clrMessage = msclr::interop::marshal_as<System::String^>(message);
    auto clrTraceLevel = static_cast<System::Diagnostics::TraceLevel>(level);
    g_proxyDelegateManager->OnTrace(clrTraceLevel, clrMessage);
}

void GlobalData::InitProgress(int maxPosition)
{
    auto tracker = g_proxyDelegateManager->GetProgressTracker();
    if (tracker)
    {
        tracker->InitializeProgress(maxPosition);
    }
}

void GlobalData::UpdateProgress(int position)
{
    auto tracker = g_proxyDelegateManager->GetProgressTracker();
    if (tracker)
    {
        tracker->SetProgress(position);
    }
}

void GlobalData::StepProgress()
{
    auto tracker = g_proxyDelegateManager->GetProgressTracker();
    if (tracker)
    {
        tracker->IncrementProgress();
    }
}

void GlobalData::CleanupProgress()
{
    auto tracker = g_proxyDelegateManager->GetProgressTracker();
    if (tracker)
    {
        tracker->CleanupProgress();
    }
}

IRenderer* GlobalData::GetRenderer()
{
    return &DLE.MineView()->Renderer();
}

const char* GlobalData::GetD1Path()
{
    // This is kind of ugly but we don't have anywhere safe to deallocate it
    static std::string d1Path;
    d1Path = msclr::interop::marshal_as<std::string>(g_proxyDelegateManager->GetD1PigPath());
    return d1Path.c_str();
}

const char* GlobalData::GetD2Path()
{
    static std::string d2Path;
    d2Path = msclr::interop::marshal_as<std::string>(g_proxyDelegateManager->GetD2PigPath());
    return d2Path.c_str();
}

void GlobalData::SetD2Path(const char* newPath)
{
    auto clrPath = gcnew System::String(newPath);
    g_proxyDelegateManager->ChangeD2PigPath(clrPath);
}

const char* GlobalData::GetAppFolder()
{
    static std::string appFolder;
    if (appFolder.length() == 0)
    {
        auto process = System::Diagnostics::Process::GetCurrentProcess();
        auto fullPath = process->MainModule->FileName;
        auto folder = System::IO::Path::GetDirectoryName(fullPath);
        appFolder = msclr::interop::marshal_as<std::string>(folder);
    }
    return appFolder.c_str();
}

const char* GlobalData::GetMissionFolder()
{
    static std::string missionFolder;
    missionFolder = msclr::interop::marshal_as<std::string>(g_proxyDelegateManager->GetMissionFolder());
    return missionFolder.c_str();
}

double GlobalData::GetMineMoveRate()
{
    return g_proxyDelegateManager->GetMineMoveRate();
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
    return ::LoadResourceAsBlob(IsD1File() ? IDR_POF_NAMES1 : IDR_POF_NAMES2);
}

std::vector<byte> GlobalData::LoadNewLevelBlob(int gameVersion)
{
    int nResource = (gameVersion == 0) ? IDR_NEW_RDL : IDR_NEW_RL2;
    return ::LoadResourceAsBlob(nResource);
}

std::vector<byte> GlobalData::GetDefaultLightTable()
{
    return ::LoadResourceAsBlob(IsD1File() ? IDR_LIGHT_D1 : IDR_LIGHT_D2);
}

std::vector<byte> GlobalData::GetDefaultColorTable()
{
    return ::LoadResourceAsBlob(IsD1File() ? IDR_COLOR_D1 : IDR_COLOR_D2);
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
    // Leaving this unimplemented for now, it's not MFC-dependent but the mod folder
    // code is messy and needs rewriting. Especially the magic folder numbers.
    //return DLE.MakeModFolders(subfolderName);
    return false;
}

const char* GlobalData::GetModFolder(int folderNumber)
{
    //return DLE.m_modFolders[folderNumber];
    return nullptr;
}

void GlobalData::ResetTextureView()
{
    // The editor should probably know to do this itself. We shouldn't need to tell it.
    //DLE.TextureView()->Setup();
    //DLE.TextureView()->Refresh();
}

void GlobalData::RefreshObjectTool()
{
    // As above.
    //DLE.ToolView()->ObjectTool()->Refresh();
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

    System::String^ pigPath;
    if (!pszName || strlen(pszName) == 0)
    {
        if (theMine && g_data.IsD1File())
        {
            return IDR_PALETTE_256;
        }
        pigPath = g_proxyDelegateManager->GetD2PigPath();
    }
    else
    {
        pigPath = gcnew System::String(pszName);
    }
    auto paletteName = System::IO::Path::GetFileNameWithoutExtension(pigPath)->ToLower();
    auto stdPaletteName = msclr::interop::marshal_as<std::string>(paletteName);

    if (palettes.count(stdPaletteName) != 0)
    {
        return palettes.at(stdPaletteName);
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
    //DLE.GetDocument()->SetModifiedFlag(modified);
}

void GlobalData::ResetSelections()
{
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
