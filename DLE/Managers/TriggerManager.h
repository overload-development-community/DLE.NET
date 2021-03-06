#ifndef __trigman_h
#define __trigman_h

//------------------------------------------------------------------------

#define MAX_TRIGGERS	((theMine == null) ? MAX_TRIGGERS_D2 : (g_data.IsD1File () || (g_data.LevelVersion () < 12)) ? MAX_TRIGGERS_D1 : MAX_TRIGGERS_D2)

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

class CTriggerManager : public ITriggerManager
{
	private:
		triggerList				m_triggers [2];
		CMineItemInfo			m_info [2];
		reactorTriggerList	m_reactorTriggers;
		CMineItemInfo			m_reactorInfo;
		CReactorData			m_reactorData;

	public:
		void ResetInfo (void) {
			m_info [0].Reset ();
			m_info [1].Reset ();
			m_reactorInfo.Reset ();
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

		short Add (CSideKey key);

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

		bool AutoAddTrigger(CSideKey source, CSideKey target, short wallType, ushort wallFlags, ushort triggerType);

		bool AddDoorTrigger(CSideKey source, CSideKey target, short wallType, ushort wallFlags, ushort triggerType);

		bool AddOpenDoor(CSideKey source, CSideKey target);

		bool AddRobotMaker(CSideKey source, CSideKey target);

		bool AddShieldDrain(CSideKey source, CSideKey target);

		bool AddEnergyDrain(CSideKey source, CSideKey target);

		bool AddUnlock(CSideKey source, CSideKey target);

		bool AddExit (short type); 

		bool AddNormalExit (void); 

		bool AddSecretExit (void); 

		short FindBySide (short& nTrigger, CSideKey key);

		CTrigger* FindByTarget (CSideKey key, short i = 0);

		void UpdateReactor (void);

		bool HaveResources (CSideKey key);

		bool Full (void);

		virtual void ReadInfo (IFileManager* fp) { m_info [0].Read (fp); }

		virtual void WriteInfo (IFileManager* fp) { m_info [0].Write (fp); }

		virtual void ReadReactorInfo (IFileManager* fp) { m_reactorInfo.Read (fp); }

		virtual void WriteReactorInfo (IFileManager* fp) { m_reactorInfo.Write (fp); }

		void Read (CFileManager* fp);

		void Write (CFileManager* fp);

		void ReadReactor (CFileManager* fp);

		void WriteReactor (CFileManager* fp);

		void Clear (void);

		CTriggerManager () {
			ResetInfo ();
			Clear ();
			}

		CTrigger* AddToWall (CSideKey key, short type, bool bAddWall);

		CTrigger* AddToObject (short nObject, short type);

		void DeleteFromWall (CSideKey key);

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
