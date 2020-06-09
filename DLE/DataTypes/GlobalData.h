#pragma once

class ILightManager
{
public:
    virtual int GetCount() = 0;
    virtual CVariableLight* VariableLight(int i) = 0;
    virtual short VariableLight(CSideKey key) = 0;
    virtual CVariableLight* AddVariableLight(CSide* side, short index = -1) = 0;
    virtual void DeleteVariableLight(short index, bool bUndo = false) = 0;

    virtual void ReadLightDeltaInfo(IFileManager* fp) = 0;
    virtual void WriteLightDeltaInfo(IFileManager* fp) = 0;
};

class IModelManager
{
public:
    virtual void Reset() = 0;
    virtual tAnimationInfo* Animation(int i) = 0;
    virtual int ScreenRad() = 0;
    virtual void Render(ubyte* p, int nStage, int bCustom, ushort firstTexture) = 0;
};

class IObjectManager
{
public:
    virtual short Add(bool bUndo = true) = 0;
    virtual void Delete(short nDelObj = -1, bool bUndo = true) = 0;

    virtual void ReadInfo(IFileManager* fp) = 0;
    virtual void WriteInfo(IFileManager* fp) = 0;
};

class IPaletteManager
{
public:
    virtual UINT GetPaletteEntries(UINT nStartIndex, UINT nNumEntries, LPPALETTEENTRY lpPaletteColors) = 0;
    virtual CBGR* Current(int i = 0, char* pszName = null) = 0;
    virtual BITMAPINFO* BMI() = 0;
    virtual ubyte ClosestColor(CBGR& color, bool bAllowTransp = true) = 0;
    virtual CBGR SuperTranspKey() = 0;
    virtual bool SuperTransp(const CBGR& color) = 0;
};

class IRobotManager
{
public:
    virtual CRobotInfo* RobotInfo(int i, int j = 0) = 0;
};

class ISegmentManager
{
public:
    virtual short Index(CSegment _const_* pSegment) = 0;
    virtual CSegment* Segment(int i) = 0;
    virtual CSide* Side(CSideKey key) = 0;
    virtual short Add() = 0;
    virtual void Remove(short nDelSeg) = 0;
    virtual bool SetTextures(CSideKey key, int nBaseTex, int nOvlTex) = 0;
    virtual CVertex& CalcCenter(CVertex& pos, short nSegment) = 0;

    virtual void ReadInfo(IFileManager* fp) = 0;
    virtual void WriteInfo(IFileManager* fp) = 0;
    virtual void ReadRobotMakerInfo(IFileManager* fp) = 0;
    virtual void WriteRobotMakerInfo(IFileManager* fp) = 0;
    virtual void ReadEquipMakerInfo(IFileManager* fp) = 0;
    virtual void WriteEquipMakerInfo(IFileManager* fp) = 0;
    virtual void ReadFogInfo(IFileManager* fp) = 0;
    virtual void WriteFogInfo(IFileManager* fp) = 0;
};

class ITextureManager
{
public:
    virtual bool Available(int nVersion = -1) = 0;
    virtual CTexture* Textures(int nTexture, int nVersion = -1) = 0;
    virtual CTexture* TextureByIndex(uint nIndex, int nVersion = -1) = 0;
    virtual int TexIdFromIndex(uint nIndex, int nVersion = -1) = 0;
    virtual bool HaveAnimationInfo(int nVersion = -1) = 0;
    virtual bool IsAnimationRoot(int nTexture, int nVersion = -1) = 0;
    virtual int ScrollSpeed(UINT16 texture, int* x, int* y) = 0;
    virtual CPigTexture* Info(int nVersion, uint nIndex) = 0;
    virtual uint TexDataOffset(int nVersion = -1) = 0;
    virtual int MaxTextures(int nVersion = -1) = 0;
    virtual int GlobalTextureCount(int nVersion = -1) = 0;
    virtual const char* Name(int nVersion, short nTexture) = 0;
    virtual void Load(ushort nBaseTex, ushort nOvlTex) = 0;
    virtual double Scale(int nVersion, short nTexture) = 0;
    virtual const CTexture* BaseTextures(uint nIndex, int nVersion = -1) = 0;
    virtual CTexture& Icon(int i) = 0;
    virtual int WriteCustomTexture(IFileManager& fp, const CTexture* pTexture, bool bUseBMPFileFormat = false) = 0;
};

class ITriggerManager
{
public:
    virtual CTrigger* Trigger(int i, int nClass = 0) = 0;

    virtual void ReadInfo(IFileManager* fp) = 0;
    virtual void WriteInfo(IFileManager* fp) = 0;
    virtual void ReadReactorInfo(IFileManager* fp) = 0;
    virtual void WriteReactorInfo(IFileManager* fp) = 0;
};

class IWallManager
{
public:
    virtual CWall* Wall(int i) = 0;
    virtual short Add(CSideKey key, bool bUndo = false) = 0;
    virtual void Remove(short nDelWall) = 0;
    virtual void Delete(CSideKey key, short nDelWall = -1) = 0;
    virtual bool IsVisible(short nWall) = 0;

    virtual void ReadInfo(IFileManager* fp) = 0;
    virtual void WriteInfo(IFileManager* fp) = 0;
};

class IVertexManager
{
public:
    virtual ushort Add(ushort* nVertices, ushort count = 1, bool bUndo = true) = 0;
    virtual void Delete(ushort nDelVert, bool bUndo = true) = 0;
    virtual CVertex& operator[] (int i) = 0;
    virtual CVertex* Vertex(int i) = 0;
};

typedef enum {
    udVertices = 1,
    udSegments = 2,
    udProducers = 4,
    udWalls = 8,
    udDoors = 16,
    udTriggers = 32,
    udObjects = 64,
    udRobots = 128,
    udVariableLights = 256,
    udStaticLight = 512,
    udDynamicLight = 1024,
    udLight = 0x700,
    udAll = 0x7FF
} eUndoFlags;

class IUndoManager
{
public:
    virtual int Backup(CGameItem* parent, eEditType editType) = 0;
    virtual void Begin(const char* szFunction, int dataFlags, bool bAccumulate = false) = 0;
    virtual void End(const char* szFunction) = 0;
    virtual void Unroll(const char* szFunction, bool bRestore = true) = 0;
};

class ISelection
{
public:
    virtual short SegmentId() = 0;
    virtual short SideId() = 0;
    virtual short Edge() = 0;
    virtual short Point() = 0;
    virtual CSegment* Segment() = 0;
    virtual CSide* Side() = 0;
    virtual CVertex* Vertex(short vertexNum = 0) = 0;
    virtual operator CSideKey() = 0;
};

enum TraceLevel
{
    Error = 1,
    Warning = 2,
    Info = 3,
    Verbose = 4
};

struct GlobalData
{
public:
    HMODULE hModule;
    ILightManager* lightManager;
    IModelManager* modelManager;
    IObjectManager* objectManager;
    IPaletteManager* paletteManager;
    IRobotManager* robotManager;
    ISegmentManager* segmentManager;
    ITextureManager* textureManager;
    ITriggerManager* triggerManager;
    IWallManager* wallManager;
    IVertexManager* vertexManager;
    IUndoManager* undoManager;
    CMissionData* missionData;

    bool IsD1File();
    bool IsD2File();
    int FileType();
    int FileVersion();
    int LevelType();
    int LevelVersion();
    bool IsStdLevel();
    bool IsD2XLevel();
    void RefreshMineView(bool all = true);
    void DelayMineViewRefresh(bool addDelay);
    void ResetMineView();
    void ResetMineViewDistance();
    void AdvanceMineViewSide();
    void RefreshToolView();
    void EnsureValidSelection();
    void Trace(TraceLevel level, std::string message);
    void InitProgress(int maxPosition);
    void UpdateProgress(int position);
    void StepProgress();
    void CleanupProgress();
    IFileManager* CreateFileManager();
    const char* GetD1Path();
    const char* GetD2Path();
    void SetD2Path(const char* newPath); // for palette switching
    const char* GetAppFolder();
    const char* GetMissionFolder();
    double GetMineMoveRate();
    std::vector<byte> LoadResourceAsBlob(const char* resourceName);
    std::vector<byte> LoadPofNames();
    std::vector<byte> LoadNewLevelBlob(int gameVersion);
    std::vector<byte> GetDefaultLightTable();
    std::vector<byte> GetDefaultColorTable();
    std::vector<std::string> LoadTextureNames(int gameVersion);
    std::vector<byte> LoadTextureIndex(int gameVersion);
    void LoadArrowTexture(CTexture& texture);
    void LoadIconTexture(int iconNumber, CTexture& texture);
    bool MakeModFolders(const char* subfolderName);
    const char* GetModFolder(int folderNumber);
    void ResetTextureView();
    void RefreshObjectTool();
    std::vector<byte> LoadPaletteData(const char* paletteName);
    void SetDocumentModifiedFlag(bool modified);
    void ResetSelections();

    GlobalData(ILightManager* lightManager,
        IModelManager* modelManager,
        IObjectManager* objectManager,
        IPaletteManager* paletteManager,
        IRobotManager* robotManager,
        ISegmentManager* segmentManager,
        ITextureManager* textureManager,
        ITriggerManager* triggerManager,
        IWallManager* wallManager,
        IVertexManager* vertexManager,
        IUndoManager* undoManager,
        CMissionData* missionData)
        :
        lightManager(lightManager),
        modelManager(modelManager),
        objectManager(objectManager),
        paletteManager(paletteManager),
        robotManager(robotManager),
        segmentManager(segmentManager),
        textureManager(textureManager),
        triggerManager(triggerManager),
        wallManager(wallManager),
        vertexManager(vertexManager),
        undoManager(undoManager),
        missionData(missionData)
    {}
};
extern GlobalData g_data;
void _cdecl PrintLog(const int nIndent, const char* fmt, ...);
