#ifndef __gameobject_h
#define __gameobject_h

#include "define.h"

# pragma pack(push, packing)
# pragma pack(1)

#include "FileManager.h"
#include "Types.h"
#include "Vector.h"
#include "Vertex.h"
#include "ViewMatrix.h"
#include "Renderer.h"

//------------------------------------------------------------------------

extern int powerupSize [MAX_POWERUP_IDS_D2];

extern int robotSize [MAX_ROBOT_IDS_TOTAL];

extern int robotShield [MAX_ROBOT_IDS_TOTAL];

extern ubyte robotClip [MAX_ROBOT_IDS_TOTAL];

extern ubyte powerupClip [MAX_POWERUP_IDS_D2];

extern ubyte powerupTypeTable [MAX_POWERUP_IDS_D2];

//------------------------------------------------------------------------

typedef struct {
  short jointnum;
  CAngleVector angles;
} JOINTPOS;

//------------------------------------------------------------------------

typedef struct {
  ubyte	renderType;		// How to draw 0=laser, 1=blob, 2=object
  ubyte	persistent;		// 0 = dies when it hits something, 1 = continues (eg, fusion cannon)
  short	nModel;			// Model num if rendertype==2.
  short	nInnerModel;	// Model num of inner part if rendertype==2.

  ubyte	flash_vclip;		// What vclip to use for muzzle flash
  ubyte	robot_hit_vclip;	// What vclip for impact with robot
  short	flash_sound;		// What sound to play when fired

  ubyte	wall_hit_vclip;	// What vclip for impact with wall
  ubyte	fire_count;			// Number of bursts fired from EACH GUN per firing.
									// ..For weapons which fire from both sides, 3*fire_count shots will be fired.
  short	robot_hit_sound;	// What sound for impact with robot

  ubyte	ammo_usage;			// How many units of ammunition it uses.
  ubyte	weapon_vclip;		// Vclip to render for the weapon, itself.
  short	wall_hit_sound;	// What sound for impact with wall

  ubyte	destructible;		// If !0, this weapon can be destroyed by another weapon.
  ubyte	matter;				// Flag: set if this object is matter (as opposed to energy)
  ubyte	bounce;				// 1==always bounces, 2=bounces twice
  ubyte	homing_flag;		// Set if this weapon can home in on a target.

  ubyte	speedvar;			// allowed variance in speed below average, /128: 64 = 50% meaning if speed = 100,
									// ..can be 50..100

  ubyte	flags;				// see values above
  ubyte	flash;				// Flash effect
  ubyte	afterburnerSize;	// Size of blobs in F1_0/16 units, specify in bitmaps.tbl as floating point. Player afterburner size = 2.5.
  ubyte	children;				// ID of weapon to drop if this contains children.  -1 means no children.
  int		energy_usage;			// How much fuel is consumed to fire this weapon.
  int		fire_wait;				// Time until this weapon can be fired again.
  int		multi_damage_scale;	// Scale damage by this amount when applying to player in multiplayer. F1_0 means no change.
  ushort bitmap;					// Pointer to bitmap if rendertype==0 or 1.
  int		blobSize;				// Size of blob if blob type
  int		flashSize;				// How big to draw the flash
  int		impactSize;				// How big of an impact
  int		strength[NDL];			// How much damage it can inflict
  int		speed[NDL];				// How fast it can move, difficulty level based.
  int		mass;						// How much mass it has
  int		drag;						// How much drag it has
  int		thrust;					// How much thrust it has
  int		poLenToWidthRatio;	// For polyobjects, the ratio of len/width. (10 maybe?)
  int		light;					// Amount of light this weapon casts.
  int		lifetime;				// Lifetime in seconds of this weapon.
  int		damageRadius;			// Radius of damage caused by weapon, used for missiles (not lasers) to apply to damage to things it did not hit
  ushort	picture [2];			// a picture and a hires picture of the weapon for the cockpit 
} WEAPON_INFO;

//------------------------------------------------------------------------

typedef struct {
  int		nAnimation;
  int		hit_sound;
  int		size;				// 3d size of longest dimension
  int		light;			// amount of light cast by this powerup, set in bitmaps.tbl
} POWERUP_TYPE_INFO;

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

class CObjPhysicsInfo {
public:
	tFixVector velocity;   // velocity vector of this object 
	tFixVector thrust;     // constant force applied to this object 
	int        mass;       // the mass of this object 
	int        drag;       // how fast this slows down 
	int        brakes;     // how much brakes applied 
	tFixVector rotVel;     // rotational velecity (angles) 
	tFixVector rotThrust;  // rotational acceleration 
	short     turnRoll;    // rotation caused by turn banking 
	ushort     flags;      // misc physics flags 

	void Read (CFileManager* fp);
	void Write (CFileManager* fp);
};

//------------------------------------------------------------------------
// stuctures for different kinds of simulation

class CObjLaserInfo {
public:
	short parent_type;   // The type of the parent of this object 
	short parent_num;    // The object's parent's number 
	int parent_signature;// The object's parent's signature... 
	int creation_time;   //  Absolute time of creation. 
	ubyte last_hitobj;    //  For persistent weapons (survive object collision), object it most recently hit. 
	ubyte track_goal;     //  Object this object is tracking. 
	int multiplier;		//  Power if this is a fusion bolt (or other super weapon to be added). 

	void Read (CFileManager* fp);
	void Write (CFileManager* fp);
};

//------------------------------------------------------------------------

class CObjExplosionInfo {
public:
	int   spawn_time;     // when lifeleft is < this, spawn another 
	int   delete_time;    // when to delete object 
	ubyte  delete_objnum;  // and what object to delete 
	ubyte  attach_parent;  // explosion is attached to this object 
	ubyte  prev_attach;    // previous explosion in attach list 
	ubyte  next_attach;    // next explosion in attach list 

	void Read (CFileManager* fp);
	void Write (CFileManager* fp);
};

//------------------------------------------------------------------------

class CObjLightInfo {
public:
  int  intensity;    // how bright the light is

	void Read (CFileManager* fp);
	void Write (CFileManager* fp);
};

//------------------------------------------------------------------------

class CObjPowerupInfo {
public:
	int  count;      // how many/much we pick up (vulcan cannon only?) 

	void Read (CFileManager* fp);
	void Write (CFileManager* fp);
};

//------------------------------------------------------------------------

class CObjAnimationInfo {
public:
	int	nAnimation;
	int	nFrameTime;
	ubyte	nFrame;

	void Read (CFileManager* fp);
	void Write (CFileManager* fp);
};

//------------------------------------------------------------------------
// structures for different kinds of rendering 

class CObjPolyModelInfo {
public:
	int      nModel;				// which polygon model 
	tAngleVector anim_angles[MAX_SUBMODELS];  // angles for each subobject 
	int      subobj_flags;     // specify which subobjs to draw 
	int      nOverrideTexture;    // if this is not -1, map all face to this 
	ubyte     alt_textures;     // if not -1, use these textures instead 

	void Read (CFileManager* fp);
	void Write (CFileManager* fp);
};

//------------------------------------------------------------------------

class CObjAIInfo {
public:
	ubyte  behavior;           
	ubyte   flags[MAX_AI_FLAGS]; // various flags, meaning defined by constants 
	short  hide_segment;        // Segment to go to for hiding. 
	short  hide_index;          // Index in Path_seg_points 
	short  path_length;         // Length of hide path. 
	short  cur_path_index;      // Current index in path. 

	short  follow_path_start_seg;  // Start segment for robot which follows path. 
	short  follow_path_end_seg;    // End segment for robot which follows path. 

	int  danger_laser_signature;
	short  danger_laser_num;

	void Read (CFileManager* fp);
	void Write (CFileManager* fp);
};

//------------------------------------------------------------------------

class CParticleInfo {
public:
	int			nLife;
	int			nSize [2];
	int			nParts;
	int			nSpeed;
	int			nDrift;
	int			nBrightness;
	ubyte			color [4];
	ubyte			nSide;
	ubyte			nType;
	ubyte			bEnabled;

	void Read (CFileManager* fp);
	void Write (CFileManager* fp);
};

//------------------------------------------------------------------------

class CLightningInfo {
public:
	int			nLife;
	int			nDelay;
	int			nLength;
	int			nAmplitude;
	int			nOffset;
	int			nWayPoint;
	short			nBolts;
	short			nId;
	short			nTarget;
	short			nNodes;
	short			nChildren;
	short			nFrames;
	ubyte			nWidth;
	ubyte			nAngle;
	ubyte			nStyle;
	ubyte			nSmoothe;
	ubyte			bClamp;
	ubyte			bPlasma;
	ubyte			bSound;
	ubyte			bRandom;
	ubyte			bInPlane;
	ubyte			bEnabled;
	ubyte			color [4];

	void Read (CFileManager* fp);
	void Write (CFileManager* fp);
};

//------------------------------------------------------------------------

class CSoundInfo {
public:
	int			nVolume;
	char			szFilename [40];
	ubyte			bEnabled;

	void Read (CFileManager* fp);
	void Write (CFileManager* fp);
};

//------------------------------------------------------------------------

class CWayPointInfo {
public:
	int			nId;
	int			nSuccessor;
	int			nSpeed;

	void Read (CFileManager* fp);
	void Write (CFileManager* fp);
	};

//------------------------------------------------------------------------

typedef struct tObjContentsInfo {
public:
	ubyte			type;  // Type of object this object contains (eg, spider contains powerup) 
	ubyte			id;    // ID of object this object contains (eg, id = blue type = key) 
	ubyte			count; // number of objects of type:id this object contains 
} tObjContentsInfo;

//------------------------------------------------------------------------

typedef struct CObjLocationInfo {
public:
	CVertex			pos;           // absolute x,y,z coordinate of center of object 
	CVertex			lastPos;			// where object was last frame 
	CDoubleMatrix	orient;        // orientation of object in world 

	inline void Clear (void) {
		pos.Clear ();
		lastPos.Clear ();
		orient.Clear ();
		}

	inline void operator+= (CDoubleVector v) {
		pos += v;
		lastPos += v;
		}

	inline void operator-= (CDoubleVector v) {
		pos -= v;
		lastPos -= v;
		}

} tObjLocationInfo;

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

typedef struct tGameObject {
	short				signature;     // reduced size to save memory 
	ubyte				type;          // what type of object this is... robot, weapon, hostage, powerup, fireball 
	ubyte				id;            // which form of object...which powerup, robot, etc. 
	ubyte				controlType;   // how this object is controlled 
	ubyte				movementType;  // how this object moves 
	ubyte				renderType;    // how this object renders 
	ubyte				flags;         // misc flags 
	ubyte				multiplayer;   // object only available in multiplayer games 
	short				nSegment;      // segment number containing object 
	tObjContentsInfo contents;
	int				size;          // 3d size of object - for collision detection 
	int				shields;       // Starts at maximum, when <0, object dies.. 
} tGameObject;

class CGameObject : public CGameItem {
	public:
		tGameObject			m_info;
		CObjLocationInfo	m_location;

		// movement info, determined by MOVEMENT_TYPE 
		union {
			CObjPhysicsInfo	physInfo; // a physics object 
			tFixVector			spinRate; // for spinning objects 
			} mType;

		// control info, determined by CONTROL_TYPE 
		union {
			CObjLaserInfo     laserInfo;
			CObjExplosionInfo explInfo;   //NOTE: debris uses this also 
			CObjAIInfo			aiInfo;
			CObjLightInfo     lightInfo;  //why put this here?  Didn't know what else to do with it. 
			CObjPowerupInfo   powerupInfo;
			CWayPointInfo		wayPointInfo;
			} cType;

		// render info, determined by RENDER_TYPE 
		union {
			CObjPolyModelInfo	polyModelInfo; //polygon model 
			CObjAnimationInfo	animationInfo;	//vclip 
			CParticleInfo		particleInfo;
			CLightningInfo		lightningInfo;
			CSoundInfo			soundInfo;
			} rType;

		// CGameObject () { memset (this, 0, sizeof (*this)); }
		inline tObjContentsInfo& Contents (void) { return m_info.contents; }

		inline tGameObject& Info (void) { return m_info; }

		inline ubyte& Id (void) { return m_info.id; }

		inline ubyte& Type (void) { return m_info.type; }

		void Read (CFileManager* fp = 0, bool bFlag = false);

		void Write (CFileManager* fp = 0, bool bFlag = false);

		void Create (ubyte type, short nSegment);

		void Setup (ubyte type);

		void DrawBitmap (CWnd* pWindow);

		int DrawArrow (CRenderer& renderer, int bCurrent);

		bool DrawSprite (CRenderer& renderer);

		void DrawHighlight (CRenderer& renderer, int bCurrent);

		bool IsInView (CRenderer& renderer, bool wholeObject);

		CVertex& Transform (CVertex& vDest, CVertex& vSource);

		int CheckNormal (CViewMatrix& viewMatrix, CVertex& a, CVertex& b);

		int CheckNormal (CViewMatrix& viewMatrix, CFixVector& a, CFixVector& b);

		CSegment* Segment (void);

		inline CDoubleVector& Position (void) { return m_location.pos; }

		inline CDoubleMatrix& Orient (void) { return m_location.orient; }

		virtual void Clear (void) {
			memset (&m_info, 0, sizeof (m_info)); 
			m_location.Clear ();
			}
		
		inline bool IsD2X (void) { return m_info.type >= OBJ_CAMBOT; }

		inline bool IsVertigo (void) { return (m_info.type == OBJ_ROBOT) && (m_info.id > N_ROBOT_TYPES_D2); }

		virtual CGameItem* Clone (void);

		virtual void Backup (eEditType editType = opModify);

		virtual CGameItem* Copy (CGameItem* pDest);

		virtual void Undo (void);

		virtual void Redo (void);

		int Contains (ubyte nType);

		int TypeContains (ubyte nType);

		bool IsBoss (void);

		inline bool HasPolyModel (void) {
			return (m_info.renderType == RT_POLYOBJ) &&
					 (Type () != OBJ_CAMBOT) && 
					 (Type () != OBJ_MONSTERBALL) && 
					 (Type () != OBJ_EXPLOSION) && 
					 (Type () != OBJ_SMOKE) && 
					 (Type () != OBJ_EFFECT);
			}

		inline bool HasSprite (void) {
				return (Type () == OBJ_EFFECT) ||
						 (m_info.renderType == RT_WEAPON_VCLIP) ||
						 (m_info.renderType == RT_HOSTAGE) ||
						 (m_info.renderType == RT_POWERUP) ||
						 (m_info.renderType == RT_FIREBALL);
			}

		short Sprite (void);

		bool CheckEffectType (bool bFix = true);
	};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

#endif // __gameobject_h

