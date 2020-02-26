#include "stdafx.h"
#include "Vertex.h"

// -----------------------------------------------------------------------------

void CVertex::Read (IFileManager* fp, bool bFlag) 
{ 
m_status = 0;
fp->ReadVector (v); 
}

// -----------------------------------------------------------------------------

void CVertex::Write (IFileManager* fp, bool bFlag) 
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
Id () = g_data.undoManager->Backup (this, editType);
}

// -----------------------------------------------------------------------------

void CVertex::Undo (void)
{
switch (EditType ()) {
	case opAdd:
		g_data.vertexManager->Delete (Index (), false);
		break;
	case opDelete:
		ushort nVertex;
		g_data.vertexManager->Add (&nVertex, 1, false);
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
		g_data.vertexManager->Delete (Index ());
		break;
	case opAdd:
		// No idea if this works, but at least it compiles
		ushort nVertex;
		g_data.vertexManager->Add (&nVertex, 1);
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

void CVertex::Transform (IViewMatrix* m)
{
m->Transform (m_view, *this);
}

// -----------------------------------------------------------------------------

void CVertex::Project (IViewMatrix* m)
{
m->Project (m_proj, m_view);
m_screen.x = (long) Round (m_proj.v.x);
m_screen.y = (long) Round (m_proj.v.y);
m_screen.z = (long) Round (m_proj.v.z * 10000.0); // 5 digits precision
}

// -----------------------------------------------------------------------------
