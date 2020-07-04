#ifndef __FRUSTUM_H
#define __FRUSTUM_H

// -----------------------------------------------------------------------------------

#define CC_OFF_LEFT     1
#define CC_OFF_RIGHT    2
#define CC_OFF_BOT      4
#define CC_OFF_TOP      8
#define CC_BEHIND       0x80

class CFrustum {
	public:
		CVertex	m_corners [8];
		CVertex	m_centers [6];
		CVertex	m_normals [6];

	void Setup (CRect viewport, double fov);
	bool Contains (short nSegment, short nSide);
	};

// -----------------------------------------------------------------------------------


#endif //__FRUSTUM_H

