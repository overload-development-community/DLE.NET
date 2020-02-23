
#include "define.h"
#include "Types.h"
#include "Vector.h"
#include "Vertex.h"
#include "FileManager.h"
#include "UndoManager.h"

// -----------------------------------------------------------------------------

void CVertex::Read (CFileManager* fp, bool bFlag) 
{ 
m_status = 0;
fp->ReadVector (v); 
}

// -----------------------------------------------------------------------------

void CVertex::Write (CFileManager* fp, bool bFlag) 
{ 
fp->WriteVector (v); 
}

// -----------------------------------------------------------------------------

CGameItem* CVertex::Copy (CGameItem* pDest)
{
if (pDest != null)
	*dynamic_cast<CVertex*> (pDest) = *this;
return pDest;
}

// -----------------------------------------------------------------------------

CGameItem* CVertex::Clone (void)
{
return Copy (new CVertex);	// only make a copy if modified
}

// -----------------------------------------------------------------------------

void CVertex::Backup (eEditType editType)
{
Id () = undoManager.Backup (this, editType);
}

// -----------------------------------------------------------------------------

void CVertex::Undo (void)
{
switch (EditType ()) {
	case opAdd:
		vertexManager.Delete (Index (), false);
		break;
	case opDelete:
		ushort nVertex;
		vertexManager.Add (&nVertex, 1, false);
		// fall through
	case opModify:
		*GetParent () = *this;
		break;
	}
}

// -----------------------------------------------------------------------------

void CVertex::Redo (void)
{
switch (EditType ()) {
	case opDelete:
		vertexManager.Delete (Index ());
		break;
	case opAdd:
		vertexManager.Add (false);
		// fall through
	case opModify:
		*GetParent () = *this;
		break;
	}
}

// -----------------------------------------------------------------------------

void CVertex::Clear (void)
{
m_status = 0;
CDoubleVector::Clear ();
m_view.Clear ();
m_delta.Clear ();
}

// -----------------------------------------------------------------------------

void CVertex::Transform (CViewMatrix* m)
{
m->Transform (m_view, *this);
}

// -----------------------------------------------------------------------------

void CVertex::Project (CViewMatrix* m)
{
m->Project (m_proj, m_view);
m_screen.x = (long) Round (m_proj.v.x);
m_screen.y = (long) Round (m_proj.v.y);
m_screen.z = (long) Round (m_proj.v.z * 10000.0); // 5 digits precision
}

// -----------------------------------------------------------------------------
