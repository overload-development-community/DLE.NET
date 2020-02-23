
#include "mine.h"
#include "dle-xp.h"

CUndoManager undoManager;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#if DETAIL_BACKUP

void CUndoItem::Setup (CGameItem* item, CGameItem* parent, eEditType editType, int nBackupId)
{
m_parent = parent; 
m_item = item;
m_parent->SetParent (m_item);
m_item->SetParent (m_parent);
m_item->Id () = Id (); // copy these to parent to make subsequent cloning simpler
m_parent->EditType () = m_item->EditType () = editType;
m_nBackupId = nBackupId;
}

#endif

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

bool CUndoData::Cleanup (void) 
{
	bool bEmpty = true;

if (m_vertices.Cleanup ()) bEmpty = false;
if (m_segments.Cleanup ()) bEmpty = false;
if (m_robotMakers.Cleanup ()) bEmpty = false;
if (m_equipMakers.Cleanup ()) bEmpty = false;
if (m_walls.Cleanup ()) bEmpty = false;
if (m_doors.Cleanup ()) bEmpty = false;
if (m_triggers [0].Cleanup ()) bEmpty = false;
if (m_triggers [1].Cleanup ()) bEmpty = false;
if (m_reactorTriggers.Cleanup ()) bEmpty = false;
if (m_reactorData.Cleanup ()) bEmpty = false;
if (m_objects.Cleanup ()) bEmpty = false;
if (m_secretExit.Cleanup ()) bEmpty = false;
if (m_robotInfo.Cleanup ()) bEmpty = false;
if (m_deltaIndices.Cleanup ()) bEmpty = false;
if (m_deltaValues.Cleanup ()) bEmpty = false;
if (m_variableLights.Cleanup ()) bEmpty = false;
if (m_faceColors.Cleanup ()) bEmpty = false;
if (m_textureColors.Cleanup ()) bEmpty = false;
if (m_vertexColors.Cleanup ()) bEmpty = false;
return bEmpty;
}

//------------------------------------------------------------------------------

void CUndoData::Destroy (void) 
{
m_vertices.Destroy ();
m_segments.Destroy ();
m_robotMakers.Destroy ();
m_equipMakers.Destroy ();
m_walls.Destroy ();
m_doors.Destroy ();
m_triggers [0].Destroy ();
m_triggers [1].Destroy ();
m_reactorTriggers.Destroy ();
m_objects.Destroy ();
m_robotInfo.Destroy ();
m_deltaIndices.Destroy ();
m_deltaValues.Destroy ();
m_variableLights.Destroy ();
m_faceColors.Destroy ();
m_textureColors.Destroy ();
m_vertexColors.Destroy ();
m_secretExit.Destroy ();
m_reactorData.Destroy ();
m_bSelections = false;
m_dataFlags = 0;
}

//------------------------------------------------------------------------------

void CUndoData::CountCalls (int dataFlags, int count)
{
for (int i = 0; i < 12; i++)
	if (dataFlags & (1 << i))
		m_backupCalls [i] += count;
}

//------------------------------------------------------------------------------

int CUndoData::CallCount (void)
{
	int count = 0;

for (int i = 0; i < 12; i++)
	count += m_backupCalls [i];
return count;
}

//------------------------------------------------------------------------------

bool CUndoData::Backup (int dataFlags, char* szFunction) 
{
if (szFunction && *szFunction)
	m_dataFlags = 0;
else if (dataFlags == (m_dataFlags & dataFlags))
	return false;

if ((dataFlags & udVertices) && !(m_dataFlags & udVertices))
	m_vertices.Backup (vertexManager.Vertex (0), &vertexManager.Count ());

if ((dataFlags & udSegments) && !(m_dataFlags & udSegments))
	m_segments.Backup (segmentManager.Segment (0), &segmentManager.Count ());

if ((dataFlags & udProducers) && !(m_dataFlags & udProducers)) {
	m_robotMakers.Backup (segmentManager.RobotMaker (0), &segmentManager.RobotMakerCount ());
	m_equipMakers.Backup (segmentManager.EquipMaker (0), &segmentManager.EquipMakerCount ());
	}

if ((dataFlags & udWalls) && !(m_dataFlags & udWalls)) 
	m_walls.Backup (wallManager.Wall (0), &wallManager.WallCount ());

if ((dataFlags & udTriggers) && !(m_dataFlags & udTriggers)) {
	m_triggers [0].Backup (triggerManager.Trigger (0, 0), &triggerManager.Count (0));
	m_triggers [1].Backup (triggerManager.Trigger (0, 1), &triggerManager.Count (1));
	m_reactorTriggers.Backup (triggerManager.ReactorTrigger (0), &triggerManager.ReactorTriggerCount ());
	m_reactorData.Backup (&triggerManager.ReactorData ());
	}	

if ((dataFlags & udObjects) && !(m_dataFlags & udObjects)) {
	m_objects.Backup (objectManager.Object (0), &objectManager.Count ());
	m_secretExit.Backup (&objectManager.SecretExit ());
	}

if ((dataFlags & udRobots) && !(m_dataFlags & udRobots))
	m_robotInfo.Backup (robotManager.RobotInfo (0), &robotManager.Count ());

if ((dataFlags & udVariableLights) && !(m_dataFlags & udVariableLights))
	m_variableLights.Backup (lightManager.VariableLight (0), &lightManager.Count ());

if ((dataFlags & udStaticLight) && !(m_dataFlags & udStaticLight)) {
	 m_faceColors.Backup (lightManager.FaceColor (0), segmentManager.Count () * 6);
	 m_textureColors.Backup (lightManager.TexColor (0), MAX_TEXTURES_D2);
	 m_vertexColors.Backup (lightManager.VertexColor (0), vertexManager.Count ());
	}

if ((dataFlags & udDynamicLight) && !(m_dataFlags & udDynamicLight)) {
	m_deltaIndices.Backup (lightManager.LightDeltaIndex (0), &lightManager.DeltaIndexCount ());
	m_deltaValues.Backup (lightManager.LightDeltaValue (0), &lightManager.DeltaValueCount ());
	}

if (!m_bSelections) {
	memcpy (m_selections, selections, sizeof (selections));
	m_bSelections = true;
	}
m_dataFlags |= dataFlags;
CountCalls (dataFlags, 1);
if (szFunction)
	m_szFunction = szFunction;
return true;
}

//------------------------------------------------------------------------------

void CUndoData::Restore (void) 
{
m_vertices.Restore ();
m_segments.Restore ();
m_robotMakers.Restore ();
m_equipMakers.Restore ();
m_walls.Restore ();
m_triggers [0].Restore ();
m_reactorData.Restore ();
m_triggers [1].Restore ();
m_reactorTriggers.Restore ();
m_objects.Restore ();
m_secretExit.Restore ();
m_robotInfo.Restore ();
m_variableLights.Restore ();
m_faceColors.Restore ();
m_textureColors.Restore ();
m_vertexColors.Restore ();
m_deltaIndices.Restore ();
m_deltaValues.Restore ();
memcpy (selections, m_selections, sizeof (selections));
}

//------------------------------------------------------------------------------

void CUndoData::Reset (void) 
{
m_vertices.Reset ();
m_segments.Reset ();
m_robotMakers.Reset ();
m_equipMakers.Reset ();
m_walls.Reset ();
m_triggers [0].Reset ();
m_triggers [1].Reset ();
m_reactorTriggers.Reset ();
m_reactorData.Reset ();
m_objects.Reset ();
m_secretExit.Reset ();
m_robotInfo.Reset ();
m_variableLights.Reset ();
m_faceColors.Reset ();
m_textureColors.Reset ();
m_vertexColors.Reset ();
m_deltaIndices.Reset ();
m_deltaValues.Reset ();
m_bSelections = false;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

CUndoManager::CUndoManager (int maxSize)
	: m_nHead (maxSize, -1), m_nTail (maxSize, -1), m_nCurrent (maxSize, 0), m_nMaxSize (maxSize), m_nNested (0), m_nMode (0), m_nLock (0)
{
m_history.Create (100);
m_history.SetGrowth (100);
m_lockHistory.Create (100);
m_lockHistory.SetGrowth (100);
}

//------------------------------------------------------------------------------

CUndoManager::~CUndoManager ()
{
Reset ();
}

//------------------------------------------------------------------------------

void CUndoManager::Reset ()
{
while (Count () > 0) {
	m_buffer [*m_nTail].Destroy ();
	--m_nTail;
	}
m_nHead = -1;
m_nTail = -1;
m_nCurrent = 0;
m_nLock = 0;
}

//------------------------------------------------------------------------------

int CUndoManager::SetMaxSize (int maxSize)
{
if (maxSize < 1)
	maxSize = 1;
else if (maxSize > DLE_MAX_UNDOS)
	maxSize = DLE_MAX_UNDOS;
int nExcess = Count () - maxSize;
if (nExcess > 0) {
	m_nCurrent -= nExcess;
	Truncate ();
	}
m_nHead.Setup (maxSize);
m_nTail.Setup (maxSize);
m_nCurrent.Setup (maxSize);
return m_nMaxSize = maxSize;
}

//------------------------------------------------------------------------------

bool CUndoManager::Undo (bool bRestore)
{
if (Locked ())
	return false;
// need a backup of the current state when starting to undo
PrintLog (0, "-> CUndoManager::Undo (current = %d, head = %d, tail = %d)\n", m_nCurrent.m_index, m_nHead.m_index, m_nTail.m_index);
if (bRestore && (m_nMode == 0) && (m_nCurrent == m_nTail) && !m_nTail.m_bTemporary) {
	Append ();
	m_nTail.m_bTemporary = true;
	m_current.Backup (udAll);
	}
if (m_nCurrent != m_nHead)
	--m_nCurrent;

if (bRestore) {
	m_nMode = 1;
	Current ()->Restore ();
	}
PrintLog (0, "<- CUndoManager::Undo (current = %d, head = %d, tail = %d)\n", m_nCurrent.m_index, m_nHead.m_index, m_nTail.m_index);
return true;
}

//------------------------------------------------------------------------------

bool CUndoManager::Redo (void)
{
if (Locked ())
	return false;
if (m_nCurrent == m_nTail)
	return false;
if (m_nMode == 0)
	return false;

m_nMode = 2;
PrintLog (0, "-> CUndoManager::Redo (current = %d, head = %d, tail = %d)\n", m_nCurrent.m_index, m_nHead.m_index, m_nTail.m_index);
++m_nCurrent;
Current ()->Restore ();
PrintLog (0, "<- CUndoManager::Redo (current = %d, head = %d, tail = %d)\n", m_nCurrent.m_index, m_nHead.m_index, m_nTail.m_index);
return true;
}

//------------------------------------------------------------------------------

void CUndoManager::Truncate (void)
{
if (Locked ())
	return;
if (m_nCurrent == m_nTail)
	return; // at end of undo list already

PrintLog (0, "-> CUndoManager::Truncate (current = %d, head = %d, tail = %d)\n", m_nCurrent.m_index, m_nHead.m_index, m_nTail.m_index);
//--m_nCurrent;
do {
	m_buffer [*m_nTail].Destroy ();
	--m_nTail;
	} while (m_nTail != m_nCurrent);

if (m_nCurrent == m_nHead) {
	m_nHead = 0;
	m_nTail = 0;
	m_nCurrent = 0;
	m_nId = 0;
	}
else {
	m_nId = Tail ()->Id () + 1;
	}
PrintLog (0, "<- CUndoManager::Truncate (current = %d, head = %d, tail = %d)\n", m_nCurrent.m_index, m_nHead.m_index, m_nTail.m_index);
}

//------------------------------------------------------------------------------

void CUndoManager::Append (void)
{
if (Locked ())
	return;
PrintLog (0, "-> CUndoManager::Append (current = %d, head = %d, tail = %d)\n", m_nCurrent.m_index, m_nHead.m_index, m_nTail.m_index);
if (m_nHead == -1) {
	m_nHead = 0;
	m_nTail = 0;
	}
else {
	Truncate ();
	if (m_nTail.m_bTemporary)
		m_nTail.m_bTemporary = false;
	else if (++m_nTail == *m_nHead) {	// buffer full
#if DETAIL_BACKUP
		int nId = Head ()->Id ();
		do { // remove all items with same backup id from buffer start
			delete Head ()->m_item;
			m_nHead = ++m_nHead % m_nMaxSize;
			} while (Head ()->Id () == nId);
#else
		Head ()->Destroy ();
		++m_nHead;
#endif
		}
	}
m_nCurrent = m_nTail;
PrintLog (0, "<- CUndoManager::Append (current = %d, head = %d, tail = %d)\n", m_nCurrent.m_index, m_nHead.m_index, m_nTail.m_index);
}

//------------------------------------------------------------------------------

int CUndoManager::Backup (CGameItem* parent, eEditType editType) 
{ 
#if DETAIL_BACKUP
SetModified (true);
if (parent->Id () == Id ()) { // item backup up in this batch already, so update the backup
	CGameItem* backup = parent->GetParent ();
	parent->Copy (backup);
	backup->SetParent (parent);
	return parent->Id ();
	}
else { // create a new backup
	CGameItem* item = parent->Clone ();
	if (item != null) {
		Append ();
		SetModified (true);
		Tail ()->Setup (item, parent, editType, Id ());
		}
	return Id ();
	}
#else
return Id ();
#endif
}

//------------------------------------------------------------------------------

bool CUndoManager::Collecting (void)
{
return m_bCollect && (m_nHead != -1) && (m_nTail != m_nHead) && (m_nCurrent == m_nTail + 1) && (m_current.Function () == Tail ()->Function ()) && (m_current.DataFlags () == Tail ()->DataFlags ());
}

//------------------------------------------------------------------------------

void CUndoManager::Backup (void)
{
if (!Locked () && !Collecting () && !m_current.Cleanup ()) {
	SetModified (true);
	PrintLog (1, "-> CUndoManager::Backup (current = %d, head = %d, tail = %d)\n", m_nCurrent.m_index, m_nHead.m_index, m_nTail.m_index);
	Append ();
	*Tail () = m_current;
	m_current.Reset ();
	PrintLog (-1, "<- CUndoManager::Backup (current = %d, head = %d, tail = %d)\n", m_nCurrent.m_index, m_nHead.m_index, m_nTail.m_index);
	Id ()++;
	}
}

//------------------------------------------------------------------------------

int CUndoManager::Count (void)
{
return (m_nTail > m_nHead) ? (m_nTail - *m_nHead + 1) : m_nTail + *m_nHead - m_nMaxSize;
}

//------------------------------------------------------------------------------

void CUndoManager::SetModified (bool bModified) 
{
DLE.GetDocument ()->SetModifiedFlag (bModified);
}  

//------------------------------------------------------------------------------

char* CUndoManager::CreateId (char* szDest, char* szSrc)
{
strcpy (szDest, szSrc);
return szDest;
}

//------------------------------------------------------------------------------

static char* szFlagNames [] = {" vertices", " segments", " producers", " walls", " doors", " triggers", " objects", " robots", " variable_lights", " static_light", " dynamic_light" };

void CUndoManager::Begin (char* szFunction, int dataFlags, bool bAccumulate) 
{
if (!Locked ()) {
	char szFlags [200];
	szFlags [0] = '\0';

	for (int i = 0; i < 11; i++)
		if (dataFlags & eUndoFlags (1 << i))
			strcat (szFlags, szFlagNames [i]);

	PrintLog (1, "Begin Undo (%s,%s)\n", szFunction, szFlags);
	m_history.Push (szFunction);
	if (0 == m_nNested++) {
		if (m_nMode != 0) {
			m_nMode = 0;
			m_current.Destroy ();
			}
		m_bCollect = bAccumulate;
		}
	m_current.Backup (dataFlags, (m_nNested == 1) ? szFunction : null);
	}
}

//------------------------------------------------------------------------------

void CUndoManager::End (char* szFunction) 
{
if (!Locked () && (m_nNested > 0)) {
	PrintLog (-1, "End Undo (%s)\n", szFunction);
	m_history.Pop (szFunction);
	if (--m_nNested == 0)
		Backup ();
	}
}

//------------------------------------------------------------------------------

void CUndoManager::Cancel (char* szFunction, int dataFlags) 
{
if (m_nNested > 0) {
	PrintLog (-1, "Cancel Undo (%s)\n", szFunction);
	m_current.CountCalls (dataFlags, -1);
	if (m_current.CallCount () == 0)
		Unroll (szFunction, false);
	}
}

//------------------------------------------------------------------------------

bool CUndoManager::Revert (bool bRestore)
{
if (!!Locked () || (m_nHead == -1))
	return false;
Undo (bRestore);
Truncate ();
return true;
}

//------------------------------------------------------------------------------

void CUndoManager::Unroll (char* szFunction, bool bRestore) 
{
if (m_nNested > 0) {
	PrintLog (-1, "Unroll Undo (%s)\n", szFunction);
	m_history.Pop (szFunction);
	if (--m_nNested == 0) {
		SetModified (false);
		Revert ();
		}
	}
}

//------------------------------------------------------------------------------

void CUndoManager::Unlock (char* szFunction) 
{ 
if (Locked ()) {
	m_nLock--; 
	m_lockHistory.Pop (szFunction);
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CUndoHistory::Push (char* szFunction)
{
if (ToS () && (*Top () == szFunction)) {
	char szMsg [256];
	sprintf_s (szMsg, sizeof (szMsg), "%s: Undo manager redundancy", szFunction);
	INFOMSG (szMsg);
	}
CStack<char*>::Push (szFunction);
}

//------------------------------------------------------------------------------

void CUndoHistory::Pop (char* szFunction)
{
if (ToS () && (*Top () != szFunction)) {
	char szMsg [256];
	sprintf_s (szMsg, sizeof (szMsg), "%s: Undo manager corruption", szFunction);
	INFOMSG (szMsg);
	do {
		CStack<char*>::Pop ();
		} while (ToS () && (*Top () != szFunction));
	}
CStack<char*>::Pop ();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
