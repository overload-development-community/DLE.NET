#ifndef __gameitem_h
#define __gameitem_h

#include "define.h"
#include "FileManager.h"
#include "crc.h"
#include "DLL.h"

# pragma pack(push, packing)
# pragma pack(1)

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

typedef enum {
	itUndefined,
	itVertex,
	itSegment,
	itProducer,
	itWall,
	itDoor,
	itTrigger,
	itObject,
	itRobot,
	itVariableLight,
	itDeltaLightValue,
	itDeltaLightIndex,
	itTextureColor,
	itFaceColor,
	itVertexColor
	} eItemType;

typedef enum {
	opNone, opAdd, opDelete, opModify, opMove
} eEditType;

class CGameItem {
	protected:
		int			m_nIndex;
		uint			m_nId;
		eItemType	m_itemType;
		eEditType	m_editType;
		CGameItem*	m_parent;
		CGameItem*	m_link;

	public:
		CGameItem (eItemType itemType = itUndefined) : m_nIndex (-1), m_itemType (itemType), m_editType (opNone), m_parent (null), m_link (null) {}

		inline bool Used (void) { return m_nIndex >= 0; }

		inline int& Index (void) { return m_nIndex; }

		inline uint& Id (void) { return m_nId; }

		inline eItemType& ItemType (void) { return m_itemType; }

		inline eEditType& EditType (void) { return m_editType; }

		virtual void Clear (void) {}

		virtual void Backup (eEditType editType = opModify) {}

		virtual CGameItem* Clone (void) { return null; }

		virtual CGameItem* Copy (CGameItem* pDest) { return null; }

		virtual void Undo (void) {}

		virtual void Redo (void) {}

		inline CGameItem* GetParent (void) { return m_parent; }

		inline CGameItem* SetParent (CGameItem* parent) { return m_parent = parent; }

		inline CGameItem* GetLink (void) { return m_link; }

		inline CGameItem* SetLink (CGameItem* link) { return m_link = link; }
	};

class CGameItemList : public CDLL< CGameItem*, CGameItem* > {};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#endif // __gameitem_h

