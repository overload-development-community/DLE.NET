#ifndef __trigger_h
#define __trigger_h

#include "define.h"

# pragma pack(push, packing)
# pragma pack(1)

#include "FileManager.h"
#include "Types.h"

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

		void Read (CFileManager* fp);

		void Write (CFileManager* fp);

		short Add (CSideKey key);

		short Delete (short i = -1);

		int Delete (CSideKey key);

		void Update (short nOldSeg, short nNewSeg);

		int Find (CSideKey key);

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

		void Read (CFileManager* fp, bool bObjTrigger);

		void Write (CFileManager* fp, bool bObjTrigger);

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

		void Read (CFileManager* fp = 0, bool bFlag = false);

		void Write (CFileManager* fp = 0, bool bFlag = false);

		virtual void Clear (void) { CTriggerTargets::Clear (); }

		virtual CGameItem* Clone (void);

		virtual void Backup (eEditType editType = opModify);

		virtual CGameItem* Copy (CGameItem* pDest);
};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

#endif // __trigger_h

