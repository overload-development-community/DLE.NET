#ifndef __trigger_h
#define __trigger_h

#define MAX_TRIGGERS_D1			100
#define MAX_TRIGGERS_D2			254
#define MAX_OBJ_TRIGGERS		254
#define MAX_REACTOR_TRIGGERS	10
#define TRIGGER_LIMIT			MAX_TRIGGERS_D2
#define MAX_TRIGGER_FLAGS		12
#define NO_TRIGGER				255

#pragma pack(push, packing)
#pragma pack(1)

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

#define MAX_TRIGGER_TARGETS	10

class CTriggerTargets {
	protected:
		short		m_count;
		CSideKey	m_targets [MAX_TRIGGER_TARGETS];

	public:
		CTriggerTargets () { m_count = 0; }

		void Clear (void);

		void Read (IFileManager* fp);

		void Write (IFileManager* fp);

		short Add (const CSideKey& key);

		short Delete (short i = -1);

		int Delete (CSideKey key);

		void Update (short nOldSeg, short nNewSeg);

		int Find (const CSideKey& key);

		inline short Add (short nSegment, short nSide) { return Add (CSideKey (nSegment, nSide)); }

		inline short Pop (void) { return Delete (m_count - 1); }

		inline int Find (short nSegment, short nSide) { return Find (CSideKey (nSegment, nSide)); }

		inline CSideKey Target (uint i) { return m_targets [i]; }

		inline short& Segment (uint i) { return m_targets [i].m_nSegment; }

		inline short& Side (uint i) { return m_targets [i].m_nSide; }

		inline CSideKey& operator[](uint i) { return m_targets [i]; }

		inline CSideKey* Target (int i = 0) { return &m_targets [i]; }

		inline short& Count (void) { return m_count; }
	};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

typedef struct tTrigger {
	ubyte		type;
	ushort	flags;
	int		value;
	int		time;
	short		nObject;
} tTrigger;

//------------------------------------------------------------------------

class CTrigger : public CTriggerTargets, public CGameItem {
	private:
		struct tTrigger m_info;
		//inline CSideKey& operator[](uint i) { return targets [i]; }

	public:
		inline tTrigger& Info (void) { return m_info; }

		inline ubyte& Type (void) { return m_info.type; }

		inline ushort& Flags (void) { return m_info.flags; }

		void Read (IFileManager* fp, bool bObjTrigger);

		void Write (IFileManager* fp, bool bObjTrigger);

		virtual void Clear (void);

		virtual CGameItem* Clone (void);

		virtual void Backup (eEditType editType = opModify);

		virtual CGameItem* Copy (CGameItem* pDest);

		void Setup (short type, short flags);

		bool IsExit (bool bSecret = true);

		inline bool IsD2X (void) { return m_info.type >= TT_TELEPORT; }

		inline const bool operator< (const CTrigger& other) {
			return (m_info.nObject < other.m_info.nObject) || ((m_info.nObject == other.m_info.nObject) && (m_info.type < other.m_info.type)); 
			}
		inline const bool operator> (const CTrigger& other) {
			return (m_info.nObject > other.m_info.nObject) || ((m_info.nObject == other.m_info.nObject) && (m_info.type > other.m_info.type)); 
			}
	};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

class CReactorTrigger : public CTriggerTargets, public CGameItem {
	public:

		void Read (IFileManager* fp = 0, bool bFlag = false);

		void Write (IFileManager* fp = 0, bool bFlag = false);

		virtual void Clear (void) { CTriggerTargets::Clear (); }

		virtual CGameItem* Clone (void);

		virtual void Backup (eEditType editType = opModify);

		virtual CGameItem* Copy (CGameItem* pDest);
};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

#pragma pack(pop, packing)

#endif // __trigger_h

