#include <afxwin.h>

#include "mine.h"
#include "dle-xp.h"

CTriggerManager triggerManager;

//------------------------------------------------------------------------------

bool CTriggerManager::Full (void) 
{ 
return Count (0) >= MAX_TRIGGERS; 
}

//------------------------------------------------------------------------------

int CTriggerManager::CmpObjTriggers (CTrigger& pi, CTrigger& pm)
{
	short i = pi.Info ().nObject;
	short m = pm.Info ().nObject;

if (i < m)
	return -1;
if (i > m)
	return 1;
i = pi.Info ().type;
m = pm.Info ().type;
return (i < m) ? -1 : (i > m) ? 1 : 0;
}

//------------------------------------------------------------------------------

void CTriggerManager::SortObjTriggers (short left, short right)
{
	CTrigger median = m_triggers [1][(left + right) / 2];
	short	l = left, r = right;

do {
	while (CmpObjTriggers (m_triggers [1][l], median) < 0)
		l++;
	while (CmpObjTriggers (m_triggers [1][r], median) > 0)
		r--;
	if (l <= r) {
		if (l < r)
			Swap (m_triggers [1][l], m_triggers [1][r]);
		l++;
		r--;
		}
	}
while (l < r);
if (l < right)
	SortObjTriggers (l, right);
if (left < r)
	SortObjTriggers (left, r);
}

//------------------------------------------------------------------------------

void CTriggerManager::SortObjTriggers (void)
{
	int	h = ObjTriggerCount ();

if (h > 1) {
	undoManager.Begin (__FUNCTION__, udTriggers);
	for (ushort i = 0; i < h; i++)
		ObjTrigger (i)->Index () = i;
	SortObjTriggers (0, h - 1);
	undoManager.End (__FUNCTION__);
	}
}

//------------------------------------------------------------------------------

void CTriggerManager::RenumberObjTriggers (void)
{
	CTrigger*	pTrigger = ObjTrigger (0);
	int			i;

undoManager.Begin (__FUNCTION__, udTriggers);
for (i = ObjTriggerCount (); i; i--, pTrigger++)
	pTrigger->Info ().nObject = objectManager.Index (objectManager.FindBySig (pTrigger->Info ().nObject));
i = ObjTriggerCount ();
while (i) {
	if (ObjTrigger (--i)->Info ().nObject < 0)
		DeleteFromObject (i);
	}
SortObjTriggers ();
undoManager.End (__FUNCTION__);
}

//------------------------------------------------------------------------------

void CTriggerManager::RenumberTargetObjs (void)
{
	CTrigger* pTrigger = ObjTrigger (0);

undoManager.Begin (__FUNCTION__, udTriggers);
for (int i = ObjTriggerCount (); i; i--, pTrigger++) {
	CSideKey* pTarget = pTrigger->Target ();
	for (int j = 0; j < pTrigger->Count (); j++) {
		if (pTarget->m_nSide >= 0) // trigger target is geometry
			pTarget++;
		else {
			CGameObject* pObject = objectManager.FindBySig (pTarget->m_nSegment);
			if (pObject == null)
				pTrigger->Delete (j--);
			else
				(pTarget++)->m_nSegment = objectManager.Index (pObject);
			}
		}
	}
undoManager.End (__FUNCTION__);
}

//------------------------------------------------------------------------------

short CTriggerManager::Add (void) 
{ 
if (!HaveResources ())
	return NO_TRIGGER; 

#if USE_FREELIST
int nTrigger = --m_free;
m_triggers [nTrigger].Clear ();
WallTriggerCount ()++;
return nTrigger;
#else
return WallTriggerCount ()++;
#endif
}

//------------------------------------------------------------------------------

CTrigger* CTriggerManager::AddToWall (short nWall, short type, bool bAddWall) 
{
	static short defWallTypes [NUM_TRIGGER_TYPES] = {
		WALL_OPEN, WALL_OPEN, WALL_OPEN, WALL_OPEN, WALL_ILLUSION, 
		WALL_OPEN, WALL_OPEN, WALL_OPEN, WALL_OPEN, WALL_OPEN, 
		WALL_OPEN, WALL_OPEN, WALL_OPEN, WALL_OPEN, WALL_ILLUSION,
		WALL_OPEN, WALL_OPEN, WALL_OPEN, WALL_OPEN, WALL_OPEN,
		WALL_OPEN, WALL_OPEN, WALL_OPEN, WALL_OPEN, WALL_OPEN,
		WALL_OPEN, WALL_OPEN, WALL_OPEN, WALL_OPEN, WALL_OPEN,
		WALL_OPEN, WALL_OPEN, WALL_OPEN, WALL_OPEN
		};

	static short defWallTextures [NUM_TRIGGER_TYPES] = {
		0, 0, 0, 0, 426, 
		0, 0, 0, 0, 0, 
		0, 0, 0, 0, 426,
		0, 0
		};

// check if there's already a trigger on the current side
if (segmentManager.Trigger () != null) {
	ErrorMsg ("There is already a trigger on this side");
	return null;
	}
if (Full ()) {
	ErrorMsg ("The maximum number of triggers has been reached.");
	return null;
	}
// if no wall at current side, try to add a wall of proper type
undoManager.Begin (__FUNCTION__, udTriggers);

CWall* pWall = current->Wall ();

if (pWall == null) {
	if (bAddWall) {
		if (wallManager.Count () >= MAX_WALLS) {
			ErrorMsg ("Cannot add a wall for this trigger\nsince the maximum number of walls is already reached.");
			undoManager.Unroll (__FUNCTION__);
			return null;
			}
		pWall = wallManager.Create (CSideKey (), (current->ChildId () < 0) ? WALL_OVERLAY : defWallTypes [type], 0, 0, -1, defWallTextures [type]);
		if (pWall == null) {
			ErrorMsg ("Cannot add a wall for this trigger.");
			undoManager.Unroll (__FUNCTION__);
			return null;
			}
		}
	else {
		ErrorMsg ("You need to add a wall to this side before you can add a trigger.");
		undoManager.Unroll (__FUNCTION__);
		return null;
		}
	}
// if D1, then convert type to flag value
ushort flags;
if (DLE.IsD1File ()) {
	switch(type) {
		case TT_OPEN_DOOR:
			flags = TRIGGER_CONTROL_DOORS;
			break;
		case TT_MATCEN:
			flags = TRIGGER_MATCEN;
			break;
		case TT_EXIT:
			flags = TRIGGER_EXIT;
			break;
		case TT_SECRET_EXIT:
			flags = TRIGGER_SECRET_EXIT;
			break;
		case TT_ILLUSION_OFF:
			flags = TRIGGER_ILLUSION_OFF;
			break;
		case TT_ILLUSION_ON:
			flags = TRIGGER_ILLUSION_ON;
			break;
		case TT_ENERGY_DRAIN:
			flags = TRIGGER_ENERGY_DRAIN;
			break;
		case TT_SHIELD_DAMAGE:
			flags = TRIGGER_SHIELD_DAMAGE;
			break;
		default:
			flags = 0;
		}
	type = 0;
	}
else
	flags = 0;

#if USE_FREELIST
int nTrigger = --m_free;
Count (0)++;
#else
short nTrigger = Count (0)++;
#endif
CTrigger* pTrigger = Trigger (nTrigger);
pTrigger->Setup (type, flags);
pTrigger->Index () = nTrigger;
pWall->SetTrigger (nTrigger);
UpdateReactor ();
undoManager.End (__FUNCTION__);
DLE.MineView ()->Refresh ();
return Trigger (nTrigger);
}

//------------------------------------------------------------------------------

void CTriggerManager::DeleteFromWall (short nDelTrigger) 
{
if (nDelTrigger == NO_TRIGGER)
	return;

if (nDelTrigger < 0) {
	CWall* pWall = current->Wall ();
	if (pWall == null)
		return;
	nDelTrigger = pWall->Info ().nTrigger;
	}

CTrigger* delTrigP = Trigger (nDelTrigger, 0);

if (delTrigP == null)
	return;

undoManager.Begin (__FUNCTION__, udTriggers);
wallManager.UpdateTrigger (nDelTrigger, NO_TRIGGER);

#if USE_FREELIST
m_free += nDelTrigger;
WallTriggerCount ()--;
#else
if (nDelTrigger < --WallTriggerCount ()) {
	*delTrigP = *WallTrigger (WallTriggerCount ());
	CWall* pWall = wallManager.FindByTrigger (WallTriggerCount ());
	if (pWall != null) {
		undoManager.Begin (__FUNCTION__, udWalls);
		pWall->Info ().nTrigger = (ubyte) nDelTrigger;
		undoManager.End (__FUNCTION__);
		}
	}
#endif

undoManager.End (__FUNCTION__);
DLE.MineView ()->Refresh ();
UpdateReactor ();
}

//------------------------------------------------------------------------------
// Mine - DeleteTrigger
//------------------------------------------------------------------------------

void CTriggerManager::DeleteTarget (CSideKey key, short nClass) 
{
CTrigger* pTrigger = &m_triggers [nClass][0];
undoManager.Begin (__FUNCTION__, udTriggers);
for (int i = 0; i < Count (nClass); i++, pTrigger++)
	pTrigger->Delete (key);
undoManager.End (__FUNCTION__);
}

//------------------------------------------------------------------------------
// Mine - FindTrigger
//------------------------------------------------------------------------------

short CTriggerManager::FindBySide (short& nTrigger, CSideKey key)
{
current->Get (key);
CWall *pWall = wallManager.FindBySide (key);
if (pWall != null) {
	nTrigger = pWall->Info ().nTrigger;
	return wallManager.Index (pWall);
	}
return NO_WALL;
}

//------------------------------------------------------------------------------------
// Mine - FindTrigger
//------------------------------------------------------------------------------------

CTrigger* CTriggerManager::FindByTarget (CSideKey key, short i)
{
for (CWallTriggerIterator ti; ti; ti++) {
	int j = ti->Find (key);
	if (j >= 0)
		return &(*ti);
	}
return null;
}

//------------------------------------------------------------------------------------
// UpdateReactor()
//
// Action - Updates control center Triggers () so that exit door opens
//          when the reactor blows up.  Removes any invalid segment/sides
//          from reactorTriggers if they exist.
//------------------------------------------------------------------------------------

void CTriggerManager::UpdateReactor (void) 
{
  CReactorTrigger *reactorTrigger = ReactorTrigger (0);	// only one reactor trigger per level

undoManager.Begin (__FUNCTION__, udTriggers);
// remove items from list that do not point to a wall
for (short nTarget = 0; nTarget < reactorTrigger->Count (); nTarget++) {
	if (!wallManager.FindBySide ((*reactorTrigger) [nTarget]))
		reactorTrigger->Delete (nTarget);
	}
// add any exits to target list that are not already in it
for (CWallIterator wi; wi; wi++) {
	CWall* pWall = &(*wi);
	CTrigger* pTrigger = pWall->Trigger ();
	if (pTrigger == null)
		continue;
	bool bExit = pTrigger->IsExit (false);
	bool bFound = (reactorTrigger->Find (*pWall) >= 0);
	if (bExit && !bFound)
		reactorTrigger->Add (*pWall);
	}
undoManager.End (__FUNCTION__);
}

//------------------------------------------------------------------------------------

CTrigger* CTriggerManager::AddToObject (short nObject, short type) 
{
	CGameObject* pObject = (nObject < 0) ? current->Object () : objectManager.Object (nObject);

if (pObject == null) {
	ErrorMsg ("Couldn't find object to attach triggers to.");
	return null;
	}

if ((pObject->Info ().type != OBJ_ROBOT) && 
	 (pObject->Info ().type != OBJ_CAMBOT) &&
	 (pObject->Info ().type != OBJ_POWERUP) &&
	 (pObject->Info ().type != OBJ_HOSTAGE) &&
	 (pObject->Info ().type != OBJ_REACTOR)) {
	ErrorMsg ("Object triggers can only be attached to robots, reactors, hostages, powerups and cameras.");
	return null;
	}

if (ObjTriggerCount () >= MAX_OBJ_TRIGGERS) {
	ErrorMsg ("The maximum number of object triggers has been reached.");
	return null;
	}

nObject = objectManager.Index (pObject);
undoManager.Begin (__FUNCTION__, udTriggers);
short nTrigger = ObjTriggerCount ()++;
CTrigger* pTrigger = ObjTrigger (nTrigger);
pTrigger->Setup (type, 0);
pTrigger->Info ().nObject = nObject;
pTrigger->Index () = nTrigger;
SortObjTriggers ();
undoManager.End (__FUNCTION__);
for (ushort i = ObjTriggerCount (); i; )
	if (ObjTrigger (--i)->Index () == nTrigger) 
		return ObjTrigger (i);
return ObjTrigger (nTrigger);
}

//------------------------------------------------------------------------------

void CTriggerManager::DeleteFromObject (short nDelTrigger) 
{
if ((nDelTrigger < 0) || (nDelTrigger >= ObjTriggerCount ()))
	return;
undoManager.Begin (__FUNCTION__, udTriggers);
if (nDelTrigger < --ObjTriggerCount ())
	*ObjTrigger (nDelTrigger) = *ObjTrigger (ObjTriggerCount ());
ObjTrigger (ObjTriggerCount ())->Index () = -1; // mark as unused (needed by the trigger iterator)
undoManager.End (__FUNCTION__);
}

//------------------------------------------------------------------------------

void CTriggerManager::DeleteObjTriggers (short nObject) 
{
undoManager.Begin (__FUNCTION__, udTriggers);
for (short i = ObjTriggerCount (); i; )
	if (ObjTrigger (--i)->Info ().nObject == nObject)
		DeleteFromObject (i);
undoManager.End (__FUNCTION__);
}

// -----------------------------------------------------------------------------

void CTriggerManager::DeleteTargets (CSideKey key) 
{
undoManager.Begin (__FUNCTION__, udTriggers);
for (CWallTriggerIterator wi; wi; wi++)
	wi->Delete (key);

for (short i = ObjTriggerCount (); i > 0; )
	if (ObjTrigger (--i)->Delete (key) == 0) // no targets left
		DeleteFromObject (i);
undoManager.End (__FUNCTION__);
}

// -----------------------------------------------------------------------------

void CTriggerManager::UpdateTargets (short nOldSeg, short nNewSeg) 
{
for (CWallTriggerIterator ti; ti; ti++)
	ti->Update (nOldSeg, nNewSeg);
}

// ----------------------------------------------------------------------------- 

void CTriggerManager::SetIndex (void)
{
int j = 0;
for (CWallTriggerIterator ti; ti; ti++)
	ti->Index () = j++;
}

// -----------------------------------------------------------------------------

void CTriggerManager::ReadReactor (CFileManager* fp)
{
if (m_reactorInfo.Restore (fp)) {
	for (short i = 0; i < ReactorTriggerCount (); i++)
		m_reactorTriggers [i].Read (fp);
	}
}

// -----------------------------------------------------------------------------

void CTriggerManager::WriteReactor (CFileManager* fp)
{
if (m_reactorInfo.Setup (fp)) {
	m_reactorInfo.size = 42;
	for (short i = 0; i < ReactorTriggerCount (); i++)
		m_reactorTriggers [i].Write (fp);
	}
}

// -----------------------------------------------------------------------------

void CTriggerManager::Read (CFileManager* fp)
{
if (m_info [0].Restore (fp)) {
	for (short i = 0; i < Count (0); i++) {
		if (Count (0) < MAX_TRIGGERS) {
	#if USE_FREELIST
			CTrigger* pTrigger = WallTrigger (--m_free);
	#else
			CTrigger* pTrigger = WallTrigger (i);
	#endif
			pTrigger->Read (fp, false);
			pTrigger->Index () = i;
			}
		else {
			CTrigger t;
			t.Read (fp, false);
			}
		}
	if (Count (0) > MAX_TRIGGERS)
		Count (0) = MAX_TRIGGERS;

	int bObjTriggersOk = 1;

	if (DLE.FileVersion () >= 33) {
		ObjTriggerCount () = fp->ReadInt32 ();
		for (short i = 0; i < ObjTriggerCount (); i++) {
			m_triggers [1][i].Read (fp, true);
			m_triggers [1][i].Index () = i;
			}
		if (DLE.FileVersion () >= 40) {
			for (short i = 0; i < ObjTriggerCount (); i++)
				m_triggers [1][i].Info ().nObject = fp->ReadInt16 ();
			}
		else {
			for (short i = 0; i < ObjTriggerCount (); i++) {
				fp->ReadInt16 ();
				fp->ReadInt16 ();
				m_triggers [1][i].Info ().nObject = fp->ReadInt16 ();
				}
			if (DLE.FileVersion () < 36)
				fp->Seek (700 * sizeof (short), SEEK_CUR);
			else
				fp->Seek (2 * sizeof (short) * fp->ReadInt16 (), SEEK_CUR);
			}
		}
	if (bObjTriggersOk && ObjTriggerCount ())
		SortObjTriggers ();
	else {
		ObjTriggerCount () = 0;
		Clear (1);
		}
	}
}

// -----------------------------------------------------------------------------

void CTriggerManager::Write (CFileManager* fp)
{
if (Count (0) + Count (1) == 0)
	m_info [0].offset = -1;
else {
	m_info [0].size = DLE.IsD1File () ? 54 : 52; // 54 = sizeof (trigger)
	m_info [0].offset = fp->Tell ();

	if (Count (0)) {
		for (CWallTriggerIterator ti; ti; ti++)
			ti->Write (fp, false);
		}

	if (DLE.LevelVersion () >= 12) {
		fp->Write (ObjTriggerCount ());
		if (ObjTriggerCount () > 0) {
			SortObjTriggers ();
			short i;
			for (i = 0; i < ObjTriggerCount (); i++)
				ObjTrigger (i)->Write (fp, true);
			for (i = 0; i < ObjTriggerCount (); i++)
				fp->WriteInt16 (ObjTrigger (i)->Info ().nObject);
			}
		}
	}
}

// ----------------------------------------------------------------------------- 

void CTriggerManager::Clear (int nClass)
{
for (int i = 0; i < Count (nClass); i++)
	m_triggers [nClass][i].Clear ();
}

// ----------------------------------------------------------------------------- 

void CTriggerManager::Clear (void)
{
Clear (0);
Clear (1);
}

// ----------------------------------------------------------------------------- 

bool CTriggerManager::HaveResources (void)
{
if (!wallManager.HaveResources ())
	return false;
if (Full ()) {
	ErrorMsg ("Maximum number of triggers reached");
	return false;
	}
return true;
}

//--------------------------------------------------------------------------
// AddDoorTrigger
//
// Action - checks other segment's side to see if there is a door there,
//          then adds a trigger for that door
//--------------------------------------------------------------------------

bool CTriggerManager::AutoAddTrigger (short wallType, ushort wallFlags, ushort triggerType) 
{
if (!HaveResources ())
	return false;
// make a new wall and a new trigger
undoManager.Begin (__FUNCTION__, udTriggers);
CWall* pWall = wallManager.Create (*current, (ubyte) wallType, wallFlags, KEY_NONE, -1, -1);
if (pWall != null) {
	CTrigger* pTrigger = AddToWall (wallManager.Index (pWall), triggerType, false);
	if (pTrigger != null) 
		pTrigger->Add (other->m_nSegment, other->m_nSide);
	undoManager.End (__FUNCTION__);
	DLE.MineView ()->Refresh ();
	return true;
	}
undoManager.End (__FUNCTION__);
return false;
}

// -----------------------------------------------------------------------------

bool CTriggerManager::AddDoorTrigger (short wallType, ushort wallFlags, ushort triggerType) 
{
if (other->Wall () == null) {
	ErrorMsg ("Other segment's side is not on a wall.\n\n"
				"Hint: Select a wall using the 'other segment' and\n"
				"select a trigger location using the 'current segment'.");
	return false;
	}
// automatically change the trigger type to open if not a door
if (other->Wall ()->Info ().type != WALL_DOOR)
	triggerType = TT_OPEN_WALL;
return AutoAddTrigger (wallType, wallFlags, triggerType);
}

// -----------------------------------------------------------------------------

bool CTriggerManager::AddOpenDoor (void) 
{
return AddDoorTrigger (WALL_OPEN,0,TT_OPEN_DOOR);
}

// -----------------------------------------------------------------------------

bool CTriggerManager::AddRobotMaker (void) 
{
if (other->Segment ()->Info ().function != SEGMENT_FUNC_ROBOTMAKER) {
	ErrorMsg ("There is no robot maker segment selected.\n\n"
				"Hint: Select a robot maker segment using the 'other segment' and\n"
				"select a trigger location using the 'current segment'.");
	return false;
	}
return AutoAddTrigger (WALL_OPEN, 0, TT_MATCEN);
}

// -----------------------------------------------------------------------------

bool CTriggerManager::AddShieldDrain (void) 
{
if (DLE.IsD2File ()) {
	ErrorMsg ("Descent 2 does not support shield damage Triggers ()");
   return false;
	}
return AutoAddTrigger (WALL_OPEN, 0, TT_SHIELD_DAMAGE);
}

// -----------------------------------------------------------------------------

bool CTriggerManager::AddEnergyDrain (void) 
{
if (DLE.IsD2File ()) {
	ErrorMsg ("Descent 2 does not support energy drain Triggers ()");
   return false;
	}
return AutoAddTrigger (WALL_OPEN, 0, TT_ENERGY_DRAIN);
}

// -----------------------------------------------------------------------------

bool CTriggerManager::AddUnlock (void) 
{
if (DLE.IsD1File ()) {
   ErrorMsg ("Control Panels are not supported in Descent 1.");
   return false;
	}
return AddDoorTrigger (WALL_OVERLAY, WALL_WALL_SWITCH, TT_UNLOCK_DOOR);
}

// -----------------------------------------------------------------------------

void CTriggerManager::DeleteD2X (void)
{
for (int i = Count (0) - 1; i >= 0; i--) {
	Delete (-i - 1); // delete all object triggers
	if (m_triggers [0][--i].IsD2X ())
		Delete (i);
	}
}

// -----------------------------------------------------------------------------
