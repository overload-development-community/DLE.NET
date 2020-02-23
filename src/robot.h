#ifndef __robot_h
#define __robot_h

#ifdef _WIN32
#pragma pack (push, 1)
#endif

//------------------------------------------------------------------------------
//describes a list of joint positions
typedef struct tJointList {
public:
	short  n_joints;
	short  offset;

	void Read (CFileManager* fp) {
		n_joints = fp->ReadInt16 ();
		offset = fp->ReadInt16 ();
		}
	void Write (CFileManager* fp) {
		 fp->Write (n_joints);
		 fp->Write (offset);
		}
} tJointList;

//------------------------------------------------------------------------------

typedef struct tRobotGunInfo {
	CFixVector	points;
	ubyte			subModels;

	void Read (CFileManager* fp, int nField);
	void Write (CFileManager* fp, int nField);
} tRobotGunInfo;

//------------------------------------------------------------------------------

typedef struct tRobotExplInfo {
	short			nClip;
	short			nSound;

	void Read (CFileManager* fp);
	void Write (CFileManager* fp);
} tRobotExplInfo;

//------------------------------------------------------------------------------

typedef struct tRobotContentsInfo {
	sbyte			id;			// ID of powerup this robot can contain.
	sbyte			count;		// Max number of things this instance can contain.
	sbyte			prob;			// Probability that this instance will contain something in N/16
	sbyte			type;			// Type of thing contained, robot or powerup, in bitmaps.tbl, !0=robot, 0=powerup

	void Read (CFileManager* fp);
	void Write (CFileManager* fp);
} tRobotContentsInfo;

//------------------------------------------------------------------------------

typedef struct tRobotSoundInfo {
	ubyte			see;			// sound robot makes when it first sees the player
	ubyte			attack;		// sound robot makes when it attacks the player
	ubyte			claw;			// sound robot makes as it claws you (m_info.attackType should be 1)
	ubyte			taunt;		// sound robot makes after you die

	void Read (CFileManager* fp);
	void Write (CFileManager* fp);
} tRobotSoundInfo;

//------------------------------------------------------------------------------

typedef struct tRobotCombatInfo {
public:
	int			fieldOfView;		// compare this value with forward_vector.dot.vector_to_player, if fieldOfView <, then robot can see player
	int			firingWait [2];	// time in seconds between shots
	int			turnTime;			// time in seconds to rotate 360 degrees in a dimension
	int			maxSpeed;			// maximum speed attainable by this robot
	int			circleDistance;	// distance at which robot circles player
	sbyte			rapidFire;			// number of shots fired rapidly
	sbyte			evadeSpeed;			// rate at which robot can evade shots, 0=none, 4=very fast

	void Read (CFileManager* fp, int nField);
	void Write (CFileManager* fp, int nField);
} tRobotCombatInfo;

//------------------------------------------------------------------------------

typedef struct tRobotInfo {
	int						nModel;		  // which polygon model?
	tRobotGunInfo			guns [MAX_GUNS];
	tRobotExplInfo			expl [2];
	sbyte						weaponType [2];
	sbyte						n_guns;				// how many different gun positions
	tRobotContentsInfo	contents;
	sbyte						kamikaze;			// !0 means commits suicide when hits you, strength thereof. 0 means no.
	short						scoreValue;			// Score from this robot.
	sbyte						badass;				// Dies with badass explosion, and strength thereof, 0 means NO.
	sbyte						drainEnergy;		// Points of energy drained at each collision.
	int						lighting;			// should this be here or with polygon model?
	int						strength;			// Initial shields of robot
	int						mass;					// how heavy is this thing?
	int						drag;					// how much drag does it have?
	tRobotCombatInfo		combat [NDL];
	sbyte						cloakType;			// 0=never, 1=always, 2=except-when-firing
	sbyte						attackType;			// 0=firing, 1=charge (like green guy)
	tRobotSoundInfo		sounds;
	sbyte						bossFlag;			// 0 = not boss, 1 = boss.  Is that surprising?
	sbyte						companion;			// Companion robot, leads you to things.
	sbyte						smartBlobs;			// how many smart blobs are emitted when this guy dies!
	sbyte						energyBlobs;		// how many smart blobs are emitted when this guy gets hit by energy weapon!
	sbyte						thief;				// !0 means this guy can steal when he collides with you!
	sbyte						pursuit;				// !0 means pursues player after he goes around a corner.
														// ..4 = 4/2 pursue up to 4/2 seconds after becoming invisible if up to 4
														// ..segments away
	sbyte						lightCast;			// Amount of light cast. 1 is default.  10 is very large.
	sbyte						deathRoll;			// 0 = dies without death roll. !0 means does death roll, larger = faster and louder
														// m_info.bossFlag, companion, thief, & pursuit probably should also be bits in the flags ubyte.
	ubyte						flags;				// misc properties
	ubyte						bCustom;
	ubyte						pad [2];				// alignment
	ubyte						deathRollSound;	// if has deathroll, what sound?
	ubyte						glow;					// apply this light to robot itself. stored as 4:4 FIXed-point
	ubyte						behavior;			// Default behavior.
	ubyte						aim;					// 255 = perfect, less = more likely to miss.  0 != random, would look stupid.
														// ..0=45 degree spread.  Specify in bitmaps.tbl in range 0.0..1.0
	//animation info
	tJointList				animStates [MAX_GUNS+1][N_ANIM_STATES];
	int						always_0xabcd;		  // debugging
} tRobotInfo;

class CRobotInfo : public CGameItem {
	private:
		tRobotInfo	m_info;
		bool			m_bModified;

	public:
		void Read (CFileManager* fp, int version = 0, bool bFlag = false);

		void Write (CFileManager* fp, int version = 0, bool bFlag = false);

		virtual void Clear (void) {
			memset (&m_info, 0, sizeof (m_info));
			m_bModified = false;
			}

		virtual CGameItem* Clone (void);

		virtual void Backup (eEditType editType = opModify);

		virtual CGameItem* Copy (CGameItem* pDest);

		inline tRobotInfo& Info (void) { return m_info; }

		inline int GetStrength (void) { return (m_info.strength > 0) ? FixLog (m_info.strength) - 12 : 0; }

		inline void SetStrength (int strength) { m_info.strength = (strength > 0) ? FixExp (strength + 12) : 0; }

		inline bool IsModified () { return m_bModified; }

		inline void SetModified (bool bModified) { m_bModified = bModified; }
};

//------------------------------------------------------------------------------

#ifdef _WIN32
#pragma pack (pop)
#endif

#endif //__robot_h