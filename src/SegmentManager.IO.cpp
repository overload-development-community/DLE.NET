// Segment.cpp

#include "mine.h"
#include "dle-xp.h"

extern short nDbgSeg, nDbgSide;

// ------------------------------------------------------------------------

void CSegmentManager::SetIndex (void)
{
int j = 0;
for (CSegmentIterator si; si; si++)
	si->Index () = j++;
}

// ------------------------------------------------------------------------

void CSegmentManager::ReadSegments (CFileManager* fp)
{
if (m_segmentInfo.Restore (fp)) {
	int i;

	for (i = 0; i < Count (); i++) {
#ifdef _DEBUG
		if (i == nDbgSeg)
			nDbgSeg = nDbgSeg;
#endif
		if (i < SEGMENT_LIMIT) {
			m_segments [i].Reset ();
			m_segments [i].Read (fp);
			}
		else {
			CSegment s;
			s.Read (fp);
			}
		}
	if (Count () > SEGMENT_LIMIT)
		Count () = SEGMENT_LIMIT;
	if (!DLE.IsD2File ())
		return;
	for (i = 0; i < Count (); i++) {
#ifdef _DEBUG
		if (i == nDbgSeg)
			nDbgSeg = nDbgSeg;
#endif
		m_segments [i].ReadExtras (fp, true);
		}
	}
}

// ------------------------------------------------------------------------

void CSegmentManager::WriteSegments (CFileManager* fp)
{
if (m_segmentInfo.Setup (fp)) {
	m_segmentInfo.offset = fp->Tell ();

	int j = Count ();
	for (int i = 0; i < j; i++) {
#ifdef _DEBUG
		if (i == nDbgSeg)
			nDbgSeg = nDbgSeg;
#endif
		m_segments [i].Write (fp);
		}
	if (!DLE.IsD2File ())
		return;
	for (int i = 0; i < j; i++)
		m_segments [i].WriteExtras (fp, true);
	}
}

// ------------------------------------------------------------------------

void CSegmentManager::ReadProducers (CFileManager* fp, int nClass)
{
if (m_producerInfo [nClass].Restore (fp)) {
	for (int i = 0; i < ProducerCount (nClass); i++) {
		if (i < MAX_MATCENS)
			m_producers [nClass][i].Read (fp);
		else {
			CObjectProducer m;
			m.Read (fp);
			}
		}
	if (ProducerCount (nClass) > MAX_MATCENS)
		ProducerCount (nClass) = MAX_MATCENS;
	}
}

// ------------------------------------------------------------------------

void CSegmentManager::WriteProducers (CFileManager* fp, int nClass)
{
if (m_producerInfo [nClass].Setup (fp)) {
	m_producerInfo [nClass].size = (DLE.IsD1File () && (nClass == 0)) ? 16 : 20; 
	m_producerInfo [nClass].offset = fp->Tell ();
	for (int i = 0; i < ProducerCount (nClass); i++)
		m_producers [nClass][i].Write (fp);
	}
}

// ------------------------------------------------------------------------

void CSegmentManager::ReadRobotMakers (CFileManager* fp)
{
ReadProducers (fp, 0);
}

// ------------------------------------------------------------------------

void CSegmentManager::WriteRobotMakers (CFileManager* fp)
{
WriteProducers (fp, 0);
}

// ------------------------------------------------------------------------

void CSegmentManager::ReadEquipMakers (CFileManager* fp)
{
ReadProducers (fp, 1);
}

// ------------------------------------------------------------------------

void CSegmentManager::WriteEquipMakers (CFileManager* fp)
{
WriteProducers (fp, 1);
}

// ------------------------------------------------------------------------

void CSegmentManager::Clear (void)
{
for (int i = 0; i < Count (); i++)
	m_segments [i].Clear ();
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
//eof segmentmanager.cpp