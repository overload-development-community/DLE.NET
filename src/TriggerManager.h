#ifndef __trigman_h
#define __trigman_h

#include "define.h"
#include "FileManager.h"
#include "carray.h"
#include "trigger.h"
#include "MineInfo.h"
#include "FreeList.h"
#include "ItemIterator.h"

//------------------------------------------------------------------------

#define MAX_TRIGGERS_D1			100
#define MAX_TRIGGERS_D2			254
#define MAX_OBJ_TRIGGERS		254
#define MAX_REACTOR_TRIGGERS	10
#define TRIGGER_LIMIT			MAX_TRIGGERS_D2
#define MAX_TRIGGER_FLAGS		12
#define NO_TRIGGER				255

#define MAX_TRIGGERS	((theMine == null) ? MAX_TRIGGERS_D2 : (DLE.IsD1File () || (DLE.LevelVersion () < 12)) ? MAX_TRIGGERS_D1 : MAX_TRIGGERS_D2)

//------------------------------------------------------------------------

#ifdef _DEBUG

typedef CStaticArray< CTrigger, TRIGGER_LIMIT > triggerList;
typedef CStaticArray< CTrigger, TRIGGER_LIMIT > objTriggerList;
typedef CStaticArray< CReactorTrigger, MAX_REACTOR_TRIGGERS > reactorTriggerList;

#else

typedef CTrigger triggerList [MAX_TRIGGERS_D2];
typedef CTrigger objTriggerList [MAX_OBJ_TRIGGERS];
typedef CReactorTrigger reactorTriggerList [MAX_REACTOR_TRIGGERS];

#endif

//------------------------------------------------------------------------

class CReactorData {
	public:
		int	time;
		int	strength;

	inline const CReactorData& operator= (const CReactorData& other) { 
		time = other.time;
		strength = other.strength;
		return *this;
		}

	inline const bool operator== (const CReactorData& other) { return (time == other.time) && (strength == other.strength); }

	inline const bool operator!= (const CReactorData& other) { return (time != other.time) || (strength != other.strength); }
};

//------------------------------------------------------------------------

class CTriggerManager {
	private:
		triggerList				m_triggers [2];
		CMineItemInfo			m_info [2];
		reactorTriggerList	m_reactorTriggers;
		CMineItemInfo			m_reactorInfo;
		CReactorData			m_reactorData;
		FREELIST(CTrigger)

	public:
		void ResetInfo (void) {
			m_info [0].Reset ();
			m_info [1].Reset ();
			m_reactorInfo.Reset ();
#if USE_FREELIST
			m_free.Reset ();
#endif
			}

		inline triggerList& TriggerList (int i) { return m_triggers [i]; }

		inline triggerList& WallTriggerList (void) { return TriggerList (0); }

		inline objTriggerList& ObjTriggerList (void) { return TriggerList (1); }

		inline CReactorData& ReactorData (void) { return m_reactorData; }

		inline int& ReactorTime (void) { return m_reactorData.time; }

		inline int& ReactorStrength (void) { return m_reactorData.strength; }

		inline CTrigger* Trigger (int i, int nClass = 0) { return (i == NO_TRIGGER) ? null : &m_triggers [nClass][i]; }

		inline int& Count (int nClass) { return m_info [nClass].count; }

		inline int& WallTriggerCount (void) { return Count (0); }

		inline int& ObjTriggerCount (void) { return Count (1); }

		inline int& ReactorTriggerCount (void) { return m_reactorInfo.count; }

		inline CTrigger* WallTrigger (int i) { return Trigger (i, 0); }

		inline CTrigger* ObjTrigger (int i) { return Trigger (i, 1); }

		short Index (CTrigger* pTrigger) { return (short) (pTrigger - &m_triggers [0][0]); }

		void SetIndex (void);

		inline reactorTriggerList& ReactorTriggers (void) { return m_reactorTriggers; }

		inline CReactorTrigger* ReactorTrigger (int i = 0) { return &m_reactorTriggers [i]; }

		void SortObjTriggers (void);

		void RenumberObjTriggers (void);

		void RenumberTargetObjs (void);

		void DeleteTarget (CSideKey key);

		void DeleteTarget (CSideKey key, short nClass);

		inline void DeleteTarget (short nSegment, short nSide) { DeleteTarget (CSideKey (nSegment, nSide)); }

		short Add (void);

		inline CTrigger* Create (short nItem, short type, bool bAddWall = false) {
			return (nItem < 0) ? AddToObject (-nItem - 1, type) : AddToWall (nItem, type, bAddWall);
			}

		inline void Delete (short nDelTrigger) {
			if (nDelTrigger < 0)
				DeleteFromObject (-nDelTrigger - 1);
			else
				DeleteFromWall (nDelTrigger);
			}

		void DeleteObjTriggers (short nObject);

		void DeleteTargets (CSideKey key);

		void UpdateTargets (short nOldSeg, short nNewSeg);

		bool AutoAddTrigger (short wallType, ushort wallFlags, ushort triggerType);

		bool AddDoorTrigger (short wallType, ushort wallFlags, ushort triggerType);

		bool AddOpenDoor (void); 

		bool AddRobotMaker (void); 

		bool AddShieldDrain (void); 

		bool AddEnergyDrain (void); 

		bool AddUnlock (void); 

		bool AddExit (short type); 

		bool AddNormalExit (void); 

		bool AddSecretExit (void); 

		short FindBySide (short& nTrigger, CSideKey key);

		CTrigger* FindByTarget (CSideKey key, short i = 0);

		void UpdateReactor (void);

		bool HaveResources (void);

#if USE_FREELIST
		inline bool Full (void) { return m_free.Empty (); }
#else
		bool Full (void);
#endif
		inline void ReadInfo (CFileManager* fp) { m_info [0].Read (fp); }

		inline void WriteInfo (CFileManager* fp) { m_info [0].Write (fp); }

		inline void ReadReactorInfo (CFileManager* fp) { m_reactorInfo.Read (fp); }

		inline void WriteReactorInfo (CFileManager* fp) { m_reactorInfo.Write (fp); }

		void Read (CFileManager* fp);

		void Write (CFileManager* fp);

		void ReadReactor (CFileManager* fp);

		void WriteReactor (CFileManager* fp);

		void Clear (void);

		CTriggerManager () {
			ResetInfo ();
			Clear ();
#if USE_FREELIST
			m_free.Create (WallTrigger (0), TRIGGER_LIMIT);
#endif
			}

		CTrigger* AddToWall (short nWall, short type, bool bAddWall);

		CTrigger* AddToObject (short nObject, short type);

		void DeleteFromWall (short nDelTrigger = -1);

		void DeleteFromObject (short nDelTrigger);

		void DeleteD2X (void);

	private:
		int CmpObjTriggers (CTrigger& pi, CTrigger& pm);

		void SortObjTriggers (short left, short right);

		void Clear (int nType);
};

extern CTriggerManager triggerManager;

//------------------------------------------------------------------------

class CTriggerIterator : public CGameItemIterator<CTrigger> {
	public:
		CTriggerIterator(int nClass) : CGameItemIterator (triggerManager.Trigger (0, nClass), triggerManager.Count (nClass)) {}
	};

class CWallTriggerIterator : public CTriggerIterator {
	public:
		CWallTriggerIterator () : CTriggerIterator(0) {}
	};

class CObjTriggerIterator : public CTriggerIterator {
	public:
		CObjTriggerIterator () : CTriggerIterator(1) {}
	};

//------------------------------------------------------------------------

#endif //__trigman_h
