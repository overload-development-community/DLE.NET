
#include "Mine.h"
#include "dle-xp.h"

CVertexManager vertexManager;

// ----------------------------------------------------------------------------- 

bool CVertexManager::Full (void) 
{ 
return Count () >= VERTEX_LIMIT; 
}

// ----------------------------------------------------------------------------- 

int CVertexManager::Overflow (int nVertices) 
{ 
if (nVertices < 0)
	nVertices = Count ();
return DLE.IsD1File () 
		 ? (nVertices > MAX_VERTICES_D1) 
		 : DLE.IsStdLevel () 
			? (nVertices > MAX_VERTICES_D2) 
			: (nVertices > VERTEX_LIMIT) 
				? -1 
				: 0;
}

// ----------------------------------------------------------------------------- 

ushort CVertexManager::Add (ushort* nVertices, ushort count, bool bUndo) 
{ 

#if USE_FREELIST

ushort nVertex;
for (ushort i = 0; i < count; i++) {
	if (m_free.Empty ())
		return i;
	nVertex = --m_free;
	Vertex (nVertex)->Clear ();
	if (bUndo)
		Vertex (nVertex)->Backup (opAdd);
	nVertices [i] = nVertex;
	Count ()++;
	}
return count; 

#else //USE_FREELIST

if (Count () + count > VERTEX_LIMIT)
	return 0;
for (ushort i = 0; i < count; i++) {
	nVertices [i] = Count () + i;
	Vertex (nVertices [i])->Clear();
	}
undoManager.Begin (__FUNCTION__, udVertices);
Count () += count;
undoManager.End (__FUNCTION__);
return count;

#endif //USE_FREELIST
}

// ----------------------------------------------------------------------------- 

void CVertexManager::Delete (ushort nDelVert, bool bUndo)
{
#if USE_FREELIST

if (bUndo)
	Vertex (nDelVert)->Backup (opDelete);
m_free += (int) nDelVert;
Count ()--;

#else //USE_FREELIST

undoManager.Begin (__FUNCTION__, udVertices);
if (nDelVert < Count ()) {
	if (nDelVert < --Count ()) {
		Vertex (nDelVert)->v = Vertex (Count ())->v;
		//Vertex (Count ())->Status ()
		Vertex (nDelVert)->Status () = 0;
		segmentManager.UpdateVertices (Count (), nDelVert);
		}
	}
undoManager.End (__FUNCTION__);

#endif //USE_FREELIST
}

// ----------------------------------------------------------------------------- 

void CVertexManager::Delete (ushort* vertices, short nVertices)
{
while (--nVertices >= 0)
	Delete (vertices [nVertices]);
}

// ----------------------------------------------------------------------------- 

void CVertexManager::DeleteUnused (void)
{
UnTagAll (NEW_MASK);
// mark all used verts
segmentManager.TagAll (NEW_MASK);
undoManager.Begin (__FUNCTION__, udVertices);
for (CVertexIterator vi (-Count ()); vi; ) {
	--vi;
	if (!(vi->Status () & NEW_MASK))
		Delete (vi.Index ()); 
	}
undoManager.End (__FUNCTION__);
}

// ----------------------------------------------------------------------------- 

CVertex* CVertexManager::Find (CDoubleVector v)
{
for (CVertexIterator vi; vi; vi++)
	if (*vi == v)
		return &(*vi);
return null;
}

// ----------------------------------------------------------------------------- 

void CVertexManager::SetIndex (void)
{
int j = 0;
for (CVertexIterator vi; vi; vi++)
	vi->Index () = j++;
}

// ----------------------------------------------------------------------------- 

void CVertexManager::Read (CFileManager* fp)
{
for (int i = 0; i < Count (); i++)
	m_vertices [i].Read (fp);
}

// ----------------------------------------------------------------------------- 

void CVertexManager::Write (CFileManager* fp)
{
m_info.offset = fp->Tell ();
int j = Count ();
for (int i = 0; i < j; i++)
	m_vertices [i].Write (fp);
}

// ----------------------------------------------------------------------------- 

void CVertexManager::TagAll (ubyte mask)
{
CVertex* v = &m_vertices [0];
for (int i = Count (); i; i--, v++)
	v->Status () |= mask; 
}

// ----------------------------------------------------------------------------- 

void CVertexManager::UnTagAll (ubyte mask)
{
CVertex* v = &m_vertices [0];
for (int i = Count (); i; i--, v++)
	v->Status () &= ~mask; 
}

// ----------------------------------------------------------------------------- 

bool CVertexManager::HasTaggedVertices (ubyte mask)
{
CVertex* v = &m_vertices [0];
for (int i = Count (); i; i--, v++)
	if (v->IsTagged (mask))
		return true;
return false;
}

// ----------------------------------------------------------------------------- 

void CVertexManager::Clear (void)
{
CVertex* v = &m_vertices [0];
for (int i = Count (); i; i--, v++)
	v->Clear ();
}

// ----------------------------------------------------------------------------- 

CDoubleVector CVertexManager::GetCenter (void)
{
	CDoubleVector	vMin (1e30, 1e30, 1e30), vMax (-1e30, -1e30, -1e30), v;

if (m_info.count) {
	for (CVertexIterator vi; vi; vi++) {
		v = *vi;
		if (vMin.v.x > v.v.x)
			vMin.v.x = v.v.x;
		if (vMin.v.y > v.v.y)
			vMin.v.y = v.v.y;
		if (vMin.v.z > v.v.z)
			vMin.v.z = v.v.z;
		if (vMax.v.x < v.v.x)
			vMax.v.x = v.v.x;
		if (vMax.v.y < v.v.y)
			vMax.v.y = v.v.y;
		if (vMax.v.z < v.v.z)
			vMax.v.z = v.v.z;
		}
	}
vMin += vMax;
vMin /= 2.0;
return vMin;
}

// ----------------------------------------------------------------------------- 

void CVertexManager::SetCenter (CDoubleVector v)
{
if (m_info.count) {
	for (CVertexIterator vi; vi; vi++) 
		*vi -= v;
	}
}

// ----------------------------------------------------------------------------- 
