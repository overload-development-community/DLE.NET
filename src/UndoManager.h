#ifndef __undoman_h
#define __undoman_h

#include "mine.h"

#define DLE_MAX_UNDOS	500
#define DETAIL_BACKUP	0

//------------------------------------------------------------------------------

typedef struct tUndoBuffer {
	tUndoBuffer	*prevBuf;
	tUndoBuffer	*nextBuf;
	CMineData	undoBuffer;
} tUndoBuffer;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#if DETAIL_BACKUP

class CUndoItem {
	public:
		CGameItem*	m_item;
		CGameItem*	m_parent;
		uint			m_nId; // used by undo manager

		inline uint& Id (void) { return m_nId; }

		inline CGameItem* Parent (void) { return m_parent; }

		inline CGameItem* Item (void) { return m_item; }

		inline void Undo (void) { m_item->Undo (); }

		inline void Redo (void) { m_item->Redo (); }

		void Setup (CGameItem* item, CGameItem* parent, eEditType editType, int nBackupId);
	};

#else //DETAIL_BACKUP

template< class _T >
class CUndoItem {
	private:
		_T*	m_backup;
		_T*	m_source;
		int	m_length;
		int*	m_sourceLength;
		uint	m_nId; // used by undo manager

		int Create (_T* source, int length) {
			if (m_length > -1) 
				return -1;
			if (0 < (m_length = length)) {
				m_backup = new _T [length];
				if (m_backup == null) {
					m_length = -1;
					return 0;
					}
				memcpy (m_backup, source, m_length * sizeof (_T));
				}
			m_source = source;
			return 1;
			}

	public:
		bool Backup (_T* source, int* length) {
			int i = Create (source, *length);
			if (i == 0)
				return false;
			if (i > 0)
				m_sourceLength = length;
			return true;
			}

		inline bool Backup (_T* source, int length = 1) {
			m_sourceLength = null;
			return Create (source, length) >= 0;
			}

		bool Restore (void) {
			if (m_length < 0)
				return false;
			if (m_backup != null)
				memcpy (m_source, m_backup, m_length * sizeof (_T));
			if (m_sourceLength != null)
				*m_sourceLength = m_length;
			return true;
			}	

		void Destroy (void) {
			if (m_backup != null) {
				delete m_backup;
				Reset ();
				}
			}

		void Reset (void) {
			m_backup = null;
			m_source = null;
			m_length = -1;
			m_sourceLength = null;
			}

		bool Diff (void) {
			if (m_length < 0)
				return false;
			if ((m_sourceLength != null) && (m_length != *m_sourceLength))
				return true;
			if (m_backup == null)
				return false;
			return memcmp (m_backup, m_source, m_length * sizeof (_T)) != 0;
			}

		bool Cleanup (void) {
			if (!Diff ())
				Destroy ();
			return !Empty ();
			}

		inline bool Empty (void) { return m_backup == null; }

		CUndoItem () : m_backup (null), m_source (null), m_length (-1), m_sourceLength (null) {}

		~CUndoItem () { Destroy (); }
	};

#endif //DETAIL_BACKUP

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

class CUndoData {
public:
	CUndoItem<CVertex>				m_vertices;
	CUndoItem<CSegment>				m_segments;
	CUndoItem<CObjectProducer>		m_robotMakers;
	CUndoItem<CObjectProducer>		m_equipMakers;
	CUndoItem<CWall>					m_walls;
	CUndoItem<CDoor>					m_doors;
	CUndoItem<CTrigger>				m_triggers [2];
	CUndoItem<CReactorTrigger>		m_reactorTriggers;
	CUndoItem<CGameObject>			m_objects;
	CUndoItem<CRobotInfo>			m_robotInfo;
	CUndoItem<CLightDeltaIndex>	m_deltaIndices;
	CUndoItem<CLightDeltaValue>	m_deltaValues;
	CUndoItem<CVariableLight>		m_variableLights;
	CUndoItem<CFaceColor>			m_faceColors;
	CUndoItem<CTextureColor>		m_textureColors;
	CUndoItem<CVertexColor>			m_vertexColors;
	CUndoItem<CSecretExit>			m_secretExit;
	CUndoItem<CReactorData>			m_reactorData;
	CSelection							m_selections [2];

	bool	m_bSelections;
	uint	m_nId;
	char* m_szFunction;
	int	m_dataFlags;
	short	m_backupCalls [12];

	inline uint& Id (void) { return m_nId; }

	inline char* Function (void) { return m_szFunction; }
	 
	bool Backup (int dataFlags, char* szFunction = null);

	void Cancel (int dataFlags);

	bool Cleanup (void);

	void Restore (void);

	void Destroy (void);

	void Reset (void);

	void CountCalls (int dataFlags, int count);

	int CallCount (void);

	inline bool Modified (void) { return m_dataFlags != 0; }

	inline int DataFlags (void) { return m_dataFlags; }

	CUndoData () : m_nId (0), m_bSelections (false) { 
		memset (m_backupCalls, 0, sizeof (m_backupCalls));
		}

	~CUndoData () { Destroy (); }
	};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

class CBufPtr {
	public:
		int	m_size;
		int	m_index;
		bool	m_bTemporary;

		void Setup (int size) { 
			if (m_index >= (m_size = size))
				m_index = m_size - 1;
			}

		const CBufPtr& operator= (CBufPtr& other) {
			m_index = other.m_index;
			m_size = other.m_size;
			return *this;
			}

		const CBufPtr& operator= (const int i) { 
			m_index = i; 
			return *this;
			}

		const bool operator== (const int i) { return m_index == i; }

		const bool operator== (const CBufPtr& other) { return m_index == other.m_index; }

		const bool operator!= (const int i) { return m_index != i; }

		const bool operator!= (const CBufPtr& other) { return m_index != other.m_index; }

		const bool operator< (const int i) { return m_index < i; }

		const bool operator> (const int i) { return m_index > i; }

		const bool operator< (const CBufPtr& other) { return m_index < other.m_index; }

		const bool operator> (const CBufPtr& other) { return m_index > other.m_index; }

		const int operator++ (int) { 
			int i = m_index;
			m_index = ++m_index % m_size; 
			return i;
			}

		const int operator++ () { return m_index = ++m_index % m_size; }

		const int operator-- (int) { 
			int i = m_index;
			m_index = ((m_index == 0) ? m_size : m_index) - 1; 
			return i;
			}

		const int operator-- () { return m_index = ((m_index == 0) ? m_size : m_index) - 1; }

		const int operator-= (const int i) { 
			m_index -= i; 
			if (m_index < 0)
				m_index += m_size;
			return m_index;
			}

		const int operator+= (const int i) { 
			m_index += i; 
			if (m_index >= m_size)
				m_index -= m_size;
			return m_index;
			}

		const int operator- (const int i) { 
			int h = m_index - i; 
			return (h < 0) ? h + m_size : h;
			}

		const int operator+ (const int i) { 
			int h = m_index + i; 
			if (h >= m_size)
				h -= m_size;
			return h;
			}

		inline void SetTemporary (bool bTemporary) { m_bTemporary = bTemporary; }
		inline bool GetTemporary (void) { return m_bTemporary; }
		const int operator* (void) { return m_index; }

		CBufPtr (int size = DLE_MAX_UNDOS, int index = -1) : m_size (size), m_index (index), m_bTemporary (false) {}
	};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

class CUndoHistory : public CStack<char*>
{
	public:
		void Push (char* szFunction);
		void Pop (char* szFunction);
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

class CUndoManager
{
	private:
		CUndoData		m_buffer [DLE_MAX_UNDOS];
		CBufPtr			m_nHead;
		CBufPtr			m_nTail;
		CBufPtr			m_nCurrent;
		CUndoData		m_current;
		//CUndoItem*	m_head;
		//CUndoItem*	m_tail;
		//CUndoItem*	m_current;
		int				m_nMaxSize;
		int				m_nLock;
		int				m_nMode;
		int				m_nNested;
		uint				m_nId;
		bool				m_bCollect;
		CUndoHistory	m_history;
		CUndoHistory	m_lockHistory;

	public:
		inline CUndoData* Head (void) { return (m_nHead < 0) ? null : &m_buffer [*m_nHead]; }

		inline CUndoData* Tail (void) { return (m_nTail < 0) ? null : &m_buffer [*m_nTail]; }

		inline CUndoData* Current (void) { return (m_nCurrent < 0) ? null : &m_buffer [*m_nCurrent]; }

		int Backup (CGameItem* parent, eEditType editType);

		void Backup (void);

		inline uint& Id (void) { return m_nId; }

		bool Undo (bool bRestore = true);

		bool Redo (void);

		void Truncate (void);

		void Reset (void);

		bool Revert (bool bRestore = true);

		int Count (void);

		inline void Lock (char* szFunction) { 
			++m_nLock; 
			m_lockHistory.Push (szFunction);
			}

		inline bool Locked (void) { return (m_nLock > 0); }

		void Unlock (char* caller);

		int SetMaxSize (int maxSize);

		inline int MaxSize (void) { return m_nMaxSize; }

		void SetModified (bool bModified);

		void Begin (char* szFunction, int dataFlags, bool bAccumulate = false);

		void End (char* szFunction);

		void Cancel (char* szFunction, int dataFlags);

		void Unroll (char* szFunction, bool bRestore = true);

		char* CreateId (char* szDest, char* szSrc);

		bool Collecting (void);

		CUndoManager (int maxSize = 100);

		~CUndoManager ();

private:
		void Append (void);

		inline int Advance (int i) { return (i == m_nMaxSize - 1) ? 0 : i + 1; }

		inline int Reverse (int i) { return (i == 0) ? m_nMaxSize - 1 : i - 1; }
};

extern CUndoManager undoManager;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#endif //__undoman_h
