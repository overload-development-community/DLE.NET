// Segment.h

#ifndef __matcen_h
#define __matcen_h

#include "define.h"
#include "Types.h"
#include "cfile.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

typedef struct tMatCenter {
	int  objFlags [2]; /* Up to 32 different Descent 1 robots */
	//  int  robot_flags2;// Additional 32 robots for Descent 2
	int    hitPoints;  /* How hard it is to destroy this particular matcen */
	int    interval;    /* Interval between materializations */
	short  nSegment;      /* Segment this is attached to. */
	short  nFuelCen; /* Index in fuelcen array. */
} tMatCenter;

// -----------------------------------------------------------------------------

class CMatCenter : public CGameItem {
public:
	tMatCenter	m_info;

	void Read (CFileManager& fp, int version = 0, bool bFlag = false);
	
	void Write (CFileManager& fp, int version = 0, bool bFlag = false);

	virtual void Clear (void) { memset (&m_info, 0, sizeof (m_info)); }

	void Setup (short nSegment, short nIndex, int nFlags) {
		Clear ();
		m_info.nSegment = nSegment;
		m_info.objFlags [0] = nFlags;
		m_info.nFuelCen = nIndex;
		};
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#endif //__matcen_h