#ifndef __botman_h
#define __botman_h

#include "Robot.h"
#include "PolyModel.h"

//------------------------------------------------------------------------

#define MAX_MATCENS ((theMine == null) ? MAX_NUM_MATCENS_D2 : (DLE.IsD1File () || (DLE.LevelVersion () < 12)) ? MAX_NUM_MATCENS_D1 : MAX_NUM_MATCENS_D2)

#define MAX_ROBOT_ID_D2 ((theMine == null) ? MAX_ROBOT_IDS_TOTAL : (DLE.LevelVersion () == 7) ? N_ROBOT_TYPES_D2 : MAX_ROBOT_IDS_TOTAL)

//------------------------------------------------------------------------

#ifdef _DEBUG

typedef CStaticArray< CRobotInfo, MAX_ROBOT_TYPES > robotInfoList;

#else

typedef CRobotInfo robotInfoList [MAX_ROBOT_TYPES];

#endif

class CRobotManager {
	private:
		robotInfoList	m_robotInfo [2];

		ubyte*				m_hxmExtraData;
		int				m_hxmExtraDataSize;
		int				m_nRobotTypes;

	public:
		void Reset (void);

		inline robotInfoList& RobotInfoList (void) { return m_robotInfo [0]; }

		inline robotInfoList& DefRobotInfoList (void) { return m_robotInfo [1]; }

		inline CRobotInfo* RobotInfo (int i, int j = 0) { return &m_robotInfo [j][i]; }

		inline CRobotInfo* DefRobotInfo (int i) { return RobotInfo (i, 1); }

		inline int& Count (void) { return m_nRobotTypes; }

		void Init (void); 

		bool IsCustomRobot (int nId);

		bool HasCustomRobots (void);

		bool HasModifiedRobots ();

		void LoadResource (int nRobot); 

		int ReadHAM (CFileManager* fp, int type = NORMAL_HAM); 

		int WriteHXM (CFileManager& fp); 

		int ReadHXM (CFileManager& fp, long size, bool bCustom = true); 

		int CRobotManager::ReadHXM (const char* filename, char *szSubFile = null, bool bCustom = true);

		void ClearHXMData ();

	};

extern CRobotManager robotManager;

//------------------------------------------------------------------------

#endif //__botman_h