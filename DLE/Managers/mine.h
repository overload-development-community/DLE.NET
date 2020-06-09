#ifndef __mine_h
#define __mine_h

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
		HPALETTE			m_paletteHandle;
		
		// strings
		char				message [4096];
		char				m_startFolder [256];
		short				m_selectMode;
		int				m_changesMade;
		int				m_nNoLightDeltas;
		char				m_szBlockFile [256];

		double			m_rotateRate;
	// Constructor/Desctuctor
	public:
		CMine ();
		~CMine ();
		void Initialize (void);
		void Reset (void);
		void Default (void);
		
	public:
		CMineData& Data(void);

		int LevelVersion(void);
		void SetLevelVersion(int levelVersion);
		inline bool IsD2XLevel (void) { return LevelVersion () >= 9; }
		inline bool IsStdLevel (void) { return LevelVersion () < 9; }
		inline bool LevelIsOutdated (void) { return IsD2XLevel () && (LevelVersion () < LEVEL_VERSION); }
		inline void UpdateLevelVersion (void) { SetLevelVersion (LEVEL_VERSION); }

		int FileType(void);
		void SetFileType(int fileType);
		bool IsD1File(void);
		bool IsD2File(void);
		inline bool IsD2XFile (void) { return IsD2File () && IsD2XLevel (); }
		bool IsVertigo();

		inline int LevelType (void) { return IsD2XLevel () ? 2 : IsD2File () ? 1 : 0; }

		CMineInfo& Info(void);

		CMineFileInfo& FileInfo(void);

		long TotalSize (CMineItemInfo& gii) { return (int) gii.size * (int) gii.count; }

		int& ReactorTime();

		int& ReactorStrength();

		int& SecretSegment();

		CDoubleMatrix& SecretOrient();

		void Backup (const char *filename);

		short Load (const char *filename = "");

		short Load (CFileManager* fp, bool bLoadFromHog);

		short Save (const char *filename);

		short ExportOverload (const char *filename);

		void UpdateCenter (void);

		void SetCenter (const CDoubleVector &center);

		LPCSTR LevelName(void);
		void SetLevelName(LPCSTR levelName);

		inline void SetSelectMode (short mode) { m_selectMode = mode; }

		inline short SelectMode (void) { return m_selectMode; }

		int ScrollSpeed (ushort texture,int *x,int *y);

        bool EditGeoFwd(ISelection* atSide, CDoubleVector* forwardVector);
        bool EditGeoBack(ISelection* atSide, CDoubleVector* forwardVector);
        bool EditGeoUp(ISelection* atSide, CDoubleVector* upVector);
        bool EditGeoDown(ISelection* atSide, CDoubleVector* upVector);
        bool EditGeoLeft(ISelection* atSide, CDoubleVector* leftVector);
        bool EditGeoRight(ISelection* atSide, CDoubleVector* leftVector);
        bool EditGeoRotLeft(ISelection* atSide, CDoubleVector* forwardVector);
        bool EditGeoRotRight(ISelection* atSide, CDoubleVector* forwardVector);
        bool EditGeoGrow(ISelection* atSide);
        bool EditGeoShrink(ISelection* atSide);
        bool RotateSelection(ISelection* atSide, double angle, bool perpendicular);
        bool ResizeItem(ISelection* atSide, double delta);
        bool MoveElements(ISelection* atSide, CDoubleVector vDelta, int objectNum = -1);
        bool MovePoints(ISelection* atSide, int pt0, int pt1);
        bool ResizeLine(CSegment* pSegment, int point0, int point1, double delta, int nStage);
        bool MoveElements2(ISelection* atSide, CDoubleVector delta, int objectNum = -1);
        bool SpinSelection(ISelection* atSide, double angle, CDoubleVector* forwardVector, int objectNum = -1);

		inline double& RotateRate (void) { return m_rotateRate; }

		short LoadMineSigAndType (CFileManager* fp);
		void LoadPaletteName (CFileManager* fp, bool bNewMine = false);

		void ConvertD1toD2 (short* textureMap); 
		void ConvertD2toD1 (void); 

	private:
		// level info
		int m_fileType;
		int m_levelVersion;
		char m_currentLevelName[256];
		CMineData m_data;

		bool m_bVertigo;
		char* m_pHxmExtraData;
		int m_nHxmExtraDataSize;

		short CreateNewLevel (CMemoryFile& fp);
		int FixIndexValues();

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

#define MAX_LIGHT_DELTA_INDICES ((theMine == null) ? MAX_LIGHT_DELTA_INDICES_STD : (g_data.IsD1File () || g_data.IsStdLevel ()) ? MAX_LIGHT_DELTA_INDICES_STD : MAX_LIGHT_DELTA_INDICES_D2X)
#define MAX_LIGHT_DELTA_VALUES ((theMine == null) ? MAX_LIGHT_DELTA_VALUES_STD : (g_data.IsD1File () || g_data.IsStdLevel ()) ? MAX_LIGHT_DELTA_VALUES_STD : MAX_LIGHT_DELTA_VALUES_D2X)

extern CMine* theMine;

#define CHECKMINE			if (theMine == null) return;
#define CHECKMINEV(_v)	if (theMine == null) return (_v);
#define CHECKMINEF		CHECKMINE(FALSE);

#endif //__mine_h