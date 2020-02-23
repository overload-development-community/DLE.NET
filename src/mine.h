#ifndef __mine_h
#define __mine_h

#include "define.h"
#include "global.h"
#include "MemoryFile.h"
#include "carray.h"
#include "Types.h"
#include "GameItem.h"
#include "Light.h"
#include "Selection.h"
#include "Textures.h"
#include "MineInfo.h"
#include "TriggerManager.h"
#include "WallManager.h"
#include "segmentManager.h"
#include "VertexManager.h"
#include "RobotManager.h"
#include "ObjectManager.h"
#include "TunnelMaker.h"
#include "BlockManager.h"
#include "LightManager.h"
#include "TextureManager.h"
#include "HogManager.h"
#include "UndoManager.h"
#include "ResourceManager.h"
#include "PaletteManager.h"
#include "TextureProjector.h"

#ifdef _DEBUG

#define CLEAR(_b) memset ((_b).Buffer (), 0, (_b).Size ())
#define ASSIGN(_a,_b) (_a) = (_b)
#define DATA(_b) (_b).Buffer (void)

#else

#define CLEAR(_b)	memset ((_b), 0, sizeof (_b))
#define ASSIGN(_a,_b) memcpy (_a, _b, sizeof (_a))
#define DATA(_b) (_b)

#endif

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

class CMine {
	public:
		// level info
		int				m_fileType;
		int				m_levelVersion;
		char				m_currentLevelName [256];	
		CMineData		m_data;

		HPALETTE			m_paletteHandle;
		
		// strings
		char				message [4096];
		char				m_startFolder [256];
		short				m_selectMode;
		int				m_changesMade;
		int				m_nNoLightDeltas;
		char				m_szBlockFile [256];
		bool				m_bVertigo;
		char*				m_pHxmExtraData;
		int				m_nHxmExtraDataSize;

		double			m_rotateRate;
	// Constructor/Desctuctor
	public:
		CMine ();
		~CMine ();
		void Initialize (void);
		void Reset (void);
		void Default (void);
		
	public:
		inline CMineData& Data (void) { return m_data; }

		inline int LevelVersion (void) { return m_levelVersion; }
		inline void SetLevelVersion (int levelVersion) { m_levelVersion = levelVersion; }
		inline bool IsD2XLevel (void) { return LevelVersion () >= 9; }
		inline bool IsStdLevel (void) { return LevelVersion () < 9; }
		inline bool LevelIsOutdated (void) { return IsD2XLevel () && (LevelVersion () < LEVEL_VERSION); }
		inline void UpdateLevelVersion (void) { SetLevelVersion (LEVEL_VERSION); }

		inline int FileType (void) { return m_fileType; }
		inline void SetFileType (int fileType) { m_fileType = fileType; }
		inline bool IsD1File (void) { return m_fileType == RDL_FILE; }
		inline bool IsD2File (void) { return m_fileType != RDL_FILE; }
		inline bool IsD2XFile (void) { return IsD2File () && IsD2XLevel (); }

		inline int LevelType (void) { return IsD2XLevel () ? 2 : IsD2File () ? 1 : 0; }

		inline CMineInfo& Info (void)	{ return Data ().m_info; }

		inline CMineFileInfo& FileInfo (void) { return Data ().m_info.fileInfo; }

		long TotalSize (CMineItemInfo& gii) { return (int) gii.size * (int) gii.count; }

		inline int& ReactorTime (void) { return triggerManager.ReactorTime (); }

		inline int& ReactorStrength (void) { return triggerManager.ReactorStrength (); }

		inline int& SecretSegment (void) { return objectManager.SecretSegment (); }

		inline CDoubleMatrix& SecretOrient (void) { return objectManager.SecretOrient (); }

		void Backup (const char *filename);

		short Load (const char *filename = "");

		short Load (CFileManager* fp, bool bLoadFromHog);

		short Save (const char *filename);

		short ExportOverload (const char *filename);

		void UpdateCenter (void);

		void SetCenter (const CDoubleVector &center);

		inline LPSTR LevelName (void) { return m_currentLevelName; }

		inline int LevelNameSize (void) { return sizeof m_currentLevelName; }

		inline void SetSelectMode (short mode) { m_selectMode = mode; }

		inline short SelectMode (void) { return m_selectMode; }

		int ScrollSpeed (ushort texture,int *x,int *y);

		bool EditGeoFwd (void);
		bool EditGeoBack (void);
		bool EditGeoUp (void);
		bool EditGeoDown (void); 
		bool EditGeoLeft (void); 
		bool EditGeoRight (void); 
		bool EditGeoRotLeft (void); 
		bool EditGeoRotRight (void); 
		bool EditGeoGrow (void); 
		bool EditGeoShrink (void); 
		bool RotateSelection(double angle, bool perpendicular); 
		bool ResizeItem (double delta); 
		bool MovePoints (CDoubleVector vDelta); 
		bool MovePoints (int pt0, int pt1); 
		bool ResizeLine (CSegment *pSegment, int point0, int point1, double delta, int nStage); 
		bool MoveElements (CDoubleVector delta); 
		bool SpinSelection (double angle); 
		void LoadSideTextures (short nSegment, short nSide);

		inline double& RotateRate (void) { return m_rotateRate; }

		short LoadMineSigAndType (CFileManager* fp);
		void LoadPaletteName (CFileManager* fp, bool bNewMine = false);

		void ConvertD1toD2 (short* textureMap); 
		void ConvertD2toD1 (void); 

	private:
		short CreateNewLevel (CMemoryFile& fp);
		int FixIndexValues (void) { return segmentManager.Fix () | wallManager.Fix (); }

		short LoadLevel (CFileManager* fp, bool bLoadFromHog);
		short LoadMine (CFileManager* fp, bool bLoadFromHog, bool bCreate);
		short LoadMineGeometry (CFileManager* fp, bool bNewMine);
		short LoadGameItems (CFileManager* fp, bool bNewMine);
		short SaveMineGeometry (CFileManager* fp);
		short SaveGameItems (CFileManager* fp);
		void ClearMineData (void);
	};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define MAX_LIGHT_DELTA_INDICES ((theMine == null) ? MAX_LIGHT_DELTA_INDICES_STD : (DLE.IsD1File () || DLE.IsStdLevel ()) ? MAX_LIGHT_DELTA_INDICES_STD : MAX_LIGHT_DELTA_INDICES_D2X)
#define MAX_LIGHT_DELTA_VALUES ((theMine == null) ? MAX_LIGHT_DELTA_VALUES_STD : (DLE.IsD1File () || DLE.IsStdLevel ()) ? MAX_LIGHT_DELTA_VALUES_STD : MAX_LIGHT_DELTA_VALUES_D2X)

extern CMine* theMine;

#define CHECKMINE			if (theMine == null) return;
#define CHECKMINEV(_v)	if (theMine == null) return (_v);
#define CHECKMINEF		CHECKMINE(FALSE);

#endif //__mine_h