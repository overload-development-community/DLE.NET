#ifndef __selection_h
#define __selection_h

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

class CSelection : public CSideKey, public ISelection
{
	protected:
		short m_nEdge;
		short m_nPoint;
		short m_nObject;
		short	m_nIndex;

	public:
		CSelection (CSideKey key = CSideKey (0, DEFAULT_SIDE)) :
			CSideKey (key),
			m_nEdge (DEFAULT_EDGE),
			m_nPoint (DEFAULT_POINT),
			m_nObject (DEFAULT_OBJECT)
		{}

		void Get (CSideKey& key) const;

		void Get (short& nSegment, short& nSide) const;

		CSegment* Segment (void) const;

		inline short SegmentId (void) const { return m_nSegment; }

		inline short SideId (void) const { return m_nSide; }

		inline short Edge (void) const { return m_nEdge; }

		inline short Point (void) const { return m_nPoint; }

		void SetSegmentId (short nSegment) override;

		void SetSideId (short nSide) override;

		inline void SetEdge (short nEdge) { 
			if (Side () != null)
				Side ()->m_nPoint = (ubyte) nEdge;
			m_nPoint = m_nEdge = nEdge; 
			}

		inline void SetPoint (short nPoint) { 
			if (Side () != null)
				Side ()->m_nPoint = (ubyte) nPoint;
			m_nEdge = m_nPoint = nPoint; 
			}

		short ChildId (void);

		CSegment* ChildSeg (void);

		CSide* Side (void) const;

		CSide* OppositeSide (void);

		CWall* Wall (void);

		CTrigger* Trigger (void);

		CGameObject* Object (void) const;

		inline short ObjectId (void) const { return m_nObject; }

		inline void SetObjectId (short nObject) { m_nObject = nObject; }

		CVertex* Vertex (short offset = 0) const;

		inline ushort VertexId (void) { return Segment ()->VertexId (m_nSide, m_nPoint); }

		CColor* LightColor (void);

		void Setup (short nSegment = -1, short nSide = -1, short nLine = -1, short nPoint = -1);

		void Reset (void) {
			m_nSegment = 0;
			m_nSide = DEFAULT_SIDE;
			m_nEdge = DEFAULT_EDGE,
			m_nPoint = DEFAULT_POINT,
			m_nObject = DEFAULT_OBJECT;
			}

		inline void Setup (CSideKey key) { Setup (key.m_nSegment, key.m_nSide); }

		void Fix (short nSegment);

		void FixObject();

		inline short& Index (void) { return m_nIndex; }

		virtual operator CSideKey() const override
		{
			CSideKey key;
			Get(key);
			return key;
		}
};

extern CSelection selections [3];
extern CSelection* current;
extern CSelection* other;
extern CSelection* nearest;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#endif //__selection_h
