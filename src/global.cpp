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

char message [4096];
char descentFolder [2][256] = {"\\programs\\d2\\data", "\\programs\\d2\\data"};
char missionFolder [256]= "\\programs\\d2\\missions";
char modFolder [256]= "\\programs\\d2\\mods";
char szPlayerProfile [20] = "";

CMissionData missionData;

GlobalData g_data{ &lightManager, &modelManager, &objectManager,
    &paletteManager, &robotManager, &segmentManager, &textureManager,
    &triggerManager, &wallManager, &vertexManager, &undoManager,
    current };

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

void GlobalData::RefreshMineView()
{
    DLE.MineView()->Refresh();
}

void GlobalData::DoInfoMsg(const char* msg)
{
    INFOMSG(msg);
}

void GlobalData::DoErrorMsg(const char* msg)
{
    ErrorMsg(msg);
}

int GlobalData::DoQuery2Msg(const char* msg, uint type)
{
    return Query2Msg(msg, type);
}

int GlobalData::ExpertMode()
{
    return DLE.ExpertMode();
}

IRenderer* GlobalData::GetRenderer()
{
    return &DLE.MineView()->Renderer();
}

IFileManager* GlobalData::CreateFileManager()
{
    return new CFileManager();
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


