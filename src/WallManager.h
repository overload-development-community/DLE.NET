#ifndef __wallman_h
#define __wallman_h

#include "define.h"
#include "carray.h"
#include "FreeList.h"
#include "ItemIterator.h"

// -----------------------------------------------------------------------------

#define MAX_WALLS_D1		175
#define MAX_WALLS_D2		255
#define MAX_WALLS_D2X	2047
#define WALL_LIMIT		2047

#define MAX_WALLS ((theMine == null) ? MAX_WALLS_D2 : DLE.IsD1File () ? MAX_WALLS_D1 : (DLE.LevelVersion () < 12) ? MAX_WALLS_D2 : WALL_LIMIT)

#define NO_WALL			WALL_LIMIT

#define MAX_DOORS_D1						50 // Maximum number of open doors Descent 1
#define MAX_DOORS_D2						90 // Maximum number of open doors Descent 2
#define DOOR_LIMIT						MAX_DOORS_D2

#define MAX_DOORS ((theMine == null) ? MAX_DOORS_D2 : DLE.IsD1File () ? MAX_DOORS_D1 : (DLE.LevelVersion () < 12) ? MAX_DOORS_D2 : DOOR_LIMIT)

// -----------------------------------------------------------------------------

#ifdef _DEBUG

typedef CStaticArray< CWall, WALL_LIMIT > wallList;
typedef CStaticArray< CDoor, DOOR_LIMIT > doorList;

#else

typedef CWall wallList [WALL_LIMIT];
typedef CDoor doorList [DOOR_LIMIT];

#endif

// -----------------------------------------------------------------------------

class CWallManager {
	private:
		wallList				m_walls;
		doorList				m_doors;
		CMineItemInfo		m_info [2];
		FREELIST(CWall)

	public:
		void ResetInfo (void) {
			m_info [0].Reset ();
			m_info [1].Reset ();
#if USE_FREELIST
			m_free.Reset ();
#endif
			}

		inline wallList& WallList (void) { return m_walls; }

		inline int& Count (int i = 0) { return m_info [i].count; }

		inline int& WallCount (void) { return Count (0); }

		inline int& DoorCount (void) { return Count (1); }

		inline CWall* Wall (int i) { return (i == NO_WALL) ? null : &m_walls [i]; }

		inline doorList& DoorList (void) { return m_doors; }

		inline CDoor* Door (int i) { return &m_doors [i]; }

		CWall* OppositeWall (CSideKey key);

		CWall* OppositeWall (short nSegment, short nSide);

		inline short Index (CWall* pWall) { return (pWall == null) ? -1 : short (pWall - &m_walls [0]); }

		void SetIndex (void);

		CWall* FindBySide (CSideKey key, int i = 0);

		//inline CWall* FindBySide (short nSegment, short nSide, int i = 0) { return FindBySide (CSideKey (nSegment, nSide), i); }
	
		CWall* FindBySegment (short nSegment, int i = 0);

		CWall* FindByTrigger (short nTrigger, int i = 0);

		void UpdateTrigger (short nOldTrigger, short nNewTrigger);

		void UpdateSegment (short nOldSeg, short nNewSeg);

		bool Full (void);

		short Add (bool bUndo = false, CSideKey* key = null);

		void Remove (short nDelWall);
		//CWall* Create (short nSegment, short nSide, short type, ushort flags, ubyte keys, char nClip, short nTexture);

		inline CWall* Create (CSideKey key, short type, ushort flags, ubyte keys, char nClip, short nTexture);

		void Delete (short nDelWall = -1);

		bool CreateDoor (ubyte type, ubyte flags, ubyte keys, char nClip, short nTexture); 

		bool CreateAutoDoor (char nClip = -1, short nTexture = -1); 

		bool CreatePrisonDoor (void); 

		bool CreateGuideBotDoor (void); 

		bool CreateFuelCell (void); 

		bool CreateIllusion (void); 

		bool CreateForceField (void); 

		bool CreateFan (void);

		bool CreateWaterFall (void);

		bool CreateLavaFall (void); 

		bool CreateGrate (void); 

		bool CreateNormalExit (void);

		bool CreateSecretExit (void);

		bool CreateExit (short type);

		bool HaveResources (CSideKey* key = null);

		inline void ReadWallInfo (CFileManager* fp) { m_info [0].Read (fp); }

		inline void WriteWallInfo (CFileManager* fp) { m_info [0].Write (fp); }

		inline void ReadDoorInfo (CFileManager* fp) { m_info [1].Read (fp); }

		inline void WriteDoorInfo (CFileManager* fp) { m_info [1].Write (fp); }

		inline void ReadInfo (CFileManager* fp) {
			ReadWallInfo (fp);
			ReadDoorInfo (fp);
			}	

		inline void WriteInfo (CFileManager* fp) {
			WriteWallInfo (fp);
			WriteDoorInfo (fp);
			}	

		void Read (CFileManager* fp);

		void Write (CFileManager* fp);

		void Clear (void);

		int Fix (void);

		inline bool IsVisible (short nWall) {
			CWall* pWall = Wall (nWall);
			return (pWall != null) && pWall->IsVisible ();
			}

		inline bool IsDoor (short nWall) {
			CWall* pWall = Wall (nWall);
			return (pWall != null) && pWall->IsDoor ();
			}

		inline bool IsVariable (short nWall) {
			CWall* pWall = Wall (nWall);
			return (pWall != null) && pWall->IsVariable ();
			}

		inline bool IsExit (short nWall) {
			CWall* pWall = Wall (nWall);
			if (pWall == null) 
				return false;
			CTrigger* pTrigger = pWall->Trigger ();
			return (pTrigger != null) && pTrigger->IsExit ();
			}

		bool ClipFromTexture (CSideKey key);

		void CheckForDoor (CSideKey key);

		void DeleteD2X (void);

		CWallManager () {
			ResetInfo ();
			Clear ();
#if USE_FREELIST
			m_free.Create (Wall (0), WALL_LIMIT);
#endif
			}

	private:
		void ReadWalls (CFileManager* fp);

		void WriteWalls (CFileManager* fp);

		void ReadDoors (CFileManager* fp);

		void WriteDoors (CFileManager* fp);
};

extern CWallManager wallManager;

//------------------------------------------------------------------------

class CWallIterator : public CGameItemIterator<CWall> {
	public:
		CWallIterator(int length = 0, int start = 0) : CGameItemIterator (wallManager.Wall (0), (length == 0) ? wallManager.WallCount () : length, start) {}
	};

class CDoorIterator : public CGameItemIterator<CDoor> {
	public:
		CDoorIterator(int length = 0) : CGameItemIterator (wallManager.Door (0), (length == 0) ? wallManager.DoorCount () : length) {}
	};

// -----------------------------------------------------------------------------

#endif //__wallman_h
