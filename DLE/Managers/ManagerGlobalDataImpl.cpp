#include "stdafx.h"
#include "MemoryFile.h"
#include "mine.h"

bool GlobalData::IsD1File()
{
    return theMine->IsD1File();
}

bool GlobalData::IsD2File()
{
    return theMine->IsD2File();
}

int GlobalData::FileType()
{
    return theMine->FileType();
}

int GlobalData::FileVersion()
{
    return theMine->FileInfo().version;
}

int GlobalData::LevelType()
{
    return theMine->LevelType();
}

int GlobalData::LevelVersion()
{
    return theMine->LevelVersion();
}

bool GlobalData::IsStdLevel()
{
    return theMine->IsStdLevel();
}

bool GlobalData::IsD2XLevel()
{
    return LevelVersion() >= 9;
}

IFileManager* GlobalData::CreateFileManager()
{
    return new CFileManager();
}
