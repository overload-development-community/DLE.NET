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

void GlobalData::RefreshMineView()
{
    DLE.MineView()->Refresh();
}

void GlobalData::DelayMineViewRefresh(bool addDelay)
{
    // This function more or less manages a refcount. It's also probably a code smell.
    DLE.MineView()->DelayRefresh(addDelay);
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


