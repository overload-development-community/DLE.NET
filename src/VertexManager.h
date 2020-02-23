#ifndef __vertman_h
#define __vertman_h

#include "define.h"
#include "FileManager.h"
#include "carray.h"
#include "cstack.h"
#include "Types.h"
#include "Selection.h"
#include "segmentManager.h"
#include "FreeList.h"
#include "ItemIterator.h"

//------------------------------------------------------------------------

#define MAX_VERTICES_D1		2808 // descent 1 max # of vertices
#define MAX_VERTICES_D2		(MAX_SEGMENTS_D2 * 4 + 8) // descent 2 max # of vertices
#define VERTEX_LIMIT			(SEGMENT_LIMIT * 3 + 8) // descent 2 max # of vertices
#define MAX_VERTEX			0xfff7 // highest permissible vertex number - higher number are reserved for segment collapsing purposes!

#define MAX_VERTICES ((theMine == null) ? MAX_VERTICES_D2 : DLE.IsD1File () ? MAX_VERTICES_D1 : DLE.IsStdLevel () ? MAX_VERTICES_D2 : VERTEX_LIMIT)

//------------------------------------------------------------------------

#ifdef _DEBUG

typedef CStaticArray< CVertex, VERTEX_LIMIT > vertexList;

#else

typedef CVertex vertexList [VERTEX_LIMIT];

#endif

//------------------------------------------------------------------------

class CVertexManager;

class CVertexUpdate {
	public:
		CVertex	m_vertex;
		ushort	m_index;

	inline void Set (ushort index);
};

class CVertexManager  {
	public:
		vertexList					m_vertices;
		CMineItemInfo				m_info;
		FREELIST(CVertex)

	public:
		// Segment and side getters
		// Vertex getters
		void ResetInfo (void) { 
			m_info.Reset (); 
#if USE_FREELIST
			m_free.Reset ();
#endif
			}

		inline CVertex& operator[] (int i) { return m_vertices [i]; }

		inline vertexList& Vertices (void) { return m_vertices; }

		inline CVertex *Vertex (int i) { return &m_vertices [i]; }

		inline ubyte& Status (int i = 0) { return Vertex (i)->Status (); }

		inline int& Count (void) { return m_info.count; }
#if USE_FREELIST
		inline bool Full (void) { return m_free.Empty (); }
#else
		bool Full (void);
#endif
		int Overflow (int nVertices = -1);

		inline int& FileOffset (void) { return m_info.offset; }

		inline ushort Index (CVertex* pVertex) { return (ushort) (pVertex - &m_vertices [0]); }

		ushort Add (ushort* nVertices, ushort count = 1, bool bUndo = true);

		void Delete (ushort nDelVert, bool bUndo = true);

		void Delete (ushort* vertices, short nVertices);

		void DeleteUnused (void);

		void SetIndex (void);

		void Read (CFileManager* fp);

		void Write (CFileManager* fp);

		void Clear (void);

		void TagAll (ubyte mask = TAGGED_MASK);

		void UnTagAll (ubyte mask = TAGGED_MASK);

		bool HasTaggedVertices (ubyte mask = TAGGED_MASK);

		CDoubleVector GetCenter (void);

		void SetCenter (CDoubleVector v);

		CVertex* Find (CDoubleVector v);

		CVertexManager () {
			ResetInfo ();
			Clear ();
#if USE_FREELIST
			m_free.Create (Vertex (0), VERTEX_LIMIT);
#endif
			}
	};

extern CVertexManager vertexManager;

//------------------------------------------------------------------------

inline void CVertexUpdate::Set (ushort index) 
{
m_index = index;
m_vertex = vertexManager [index];
}

//------------------------------------------------------------------------

class CVertexIterator : public CGameItemIterator<CVertex> {
	public:
		CVertexIterator(int length = 0) : CGameItemIterator (vertexManager.Vertex (0), (length == 0) ? vertexManager.Count () : length) {}
	};

//------------------------------------------------------------------------

#endif //__vertman_h