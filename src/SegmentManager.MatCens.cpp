// Segment.cpp

#include "mine.h"
#include "dle-xp.h"

// -----------------------------------------------------------------------------

void CSegmentManager::RenumberProducers (ubyte nFunction, short nClass, bool bKeepOrder)
{
int h, i = 0, nErrors = 0; 

if (bKeepOrder) {
	for (i = 0; i < ProducerCount (nClass); i++) {
		m_producers [nClass][i].m_info.nSegment = -1;
		++nErrors;
		}
	}
else {
	m_producers [nClass].SortAscending (0, ProducerCount (nClass) - 1);
	for (h = i = 0; i < ProducerCount (nClass); i++) {
		short nSegment = m_producers [nClass][i].m_info.nSegment; 
		if (nSegment < 0) 
			++nErrors;
		else {
			CSegment* pSegment = Segment (nSegment); 
			if ((pSegment->m_info.function == nFunction) && (pSegment->m_info.nProducer < 0)) 
				m_producers [nClass][i].m_info.nProducer = pSegment->m_info.nProducer = h++; 
			else {
				m_producers [nClass][i].m_info.nSegment = -1; 
				++nErrors;
				}
			}
		}
	}

if (nErrors) { // not all matcens assigned to a segment - try to find a segment that doesn't have a matcen
	CSegment* pSegment = Segment (0);
	for (int i = Count (); i; i--, pSegment++) {
		if ((pSegment->m_info.function == nFunction) && (pSegment->m_info.nProducer < 0)) {
			for (int j = 0; j < ProducerCount (nClass); j++) {
				if (m_producers [nClass][j].m_info.nSegment < 0) {
					pSegment->m_info.function = nFunction;
					pSegment->m_info.nProducer = j;
					m_producers [nClass][j].m_info.nSegment = Count () - i;
					nErrors--;
					break;
					}
				}
			}
		}
	}

if (nErrors) { // delete remaining unassigned matcens
	nErrors = 0;
	for (int i = 0, j = 0; j < ProducerCount (nClass); j++) {
		if (m_producers [nClass][i].m_info.nSegment >= 0)
			Segment (m_producers [nClass][i].m_info.nSegment)->m_info.nProducer = i++;
		else {
			if (i < j)
				m_producers [nClass][i] = m_producers [nClass][j];
			++nErrors;
			}
		}
	ProducerCount (nClass) -= nErrors;
	}
}

// -----------------------------------------------------------------------------

void CSegmentManager::RenumberRobotMakers (bool bKeepOrder)
{
RenumberProducers (SEGMENT_FUNC_ROBOTMAKER, 0, bKeepOrder);
}

// -----------------------------------------------------------------------------

void CSegmentManager::RenumberEquipMakers (bool bKeepOrder)
{
RenumberProducers (SEGMENT_FUNC_EQUIPMAKER, 1, bKeepOrder);
}

// -----------------------------------------------------------------------------

void CSegmentManager::RenumberProducers (bool bKeepOrder)
{
undoManager.Begin (__FUNCTION__, udSegments);
CSegment* pSegment = Segment (0);
for (int h = 0, i = Count (); i; i--, pSegment++)
	pSegment->m_info.nProducer = -1;
RenumberRobotMakers (bKeepOrder);
RenumberEquipMakers (bKeepOrder);
pSegment = Segment (0);
for (int h = 0, i = Count (); i; i--, pSegment++)
	if ((pSegment->m_info.function == SEGMENT_FUNC_PRODUCER) ||
		 (pSegment->m_info.function == SEGMENT_FUNC_REPAIRCEN))
		pSegment->m_info.value = h++; 
	else if ((pSegment->m_info.function == SEGMENT_FUNC_ROBOTMAKER) ||
				(pSegment->m_info.function == SEGMENT_FUNC_EQUIPMAKER)) {
		if (pSegment->m_info.nProducer >= 0) {
			pSegment->m_info.value = h++; 
			}
		else {
			pSegment->m_info.value = -1; 
			pSegment->m_info.function = SEGMENT_FUNC_NONE;
			}
		}
	else
		pSegment->m_info.value = -1; 
undoManager.End (__FUNCTION__);
}

// ----------------------------------------------------------------------------- 

CSegment* CSegmentManager::FindRobotMaker (short i)
{
	CSegment* pSegment = Segment (i);

for (; i < Count (); i++, pSegment++)
	if (pSegment->m_info.function == SEGMENT_FUNC_ROBOTMAKER)
		return pSegment;
return null;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//eof segmentmanager.cpp