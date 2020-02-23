#ifndef OBJECT_TYPES_H
#define OBJECT_TYPES_H

#include "define.h"

# pragma pack(push, packing)
# pragma pack(1)

#include "cfile.h"
#include "Types.h"
#include "Vector.h"

typedef struct {
  short jointnum;
  CAngleVector angles;
} JOINTPOS;

typedef struct {
  char	renderType;		// How to draw 0=laser, 1=blob, 2=object
  char	persistent;		// 0 = dies when it hits something, 1 = continues (eg, fusion cannon)
  short	nModel;		// Model num if rendertype==2.
  short	nInnerModel;	// Model num of inner part if rendertype==2.

  char	flash_vclip;		// What vclip to use for muzzle flash
  char	robot_hit_vclip;	// What vclip for impact with robot
  short	flash_sound;		// What sound to play when fired

  char	wall_hit_vclip;		// What vclip for impact with wall
  char	fire_count;		// Number of bursts fired from EACH GUN per firing.
				// ..For weapons which fire from both sides, 3*fire_count shots will be fired.
  short	robot_hit_sound;	// What sound for impact with robot

  char	ammo_usage;		// How many units of ammunition it uses.
  char	weapon_vclip;		// Vclip to render for the weapon, itself.
  short	wall_hit_sound;		// What sound for impact with wall

  char	destructible;		// If !0, this weapon can be destroyed by another weapon.
  char	matter;			// Flag: set if this object is matter (as opposed to energy)
  char	bounce;			// 1==always bounces, 2=bounces twice
  char	homing_flag;		// Set if this weapon can home in on a target.

  byte	speedvar;		// allowed variance in speed below average, /128: 64 = 50% meaning if speed = 100,
				// ..can be 50..100

  byte	flags;				// see values above
  char	flash;				// Flash effect
  char	afterburnerSize;	// Size of blobs in F1_0/16 units, specify in bitmaps.tbl as floating point. Player afterburner size = 2.5.
  char	children;				// ID of weapon to drop if this contains children.  -1 means no children.
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

typedef struct {
  int	vclip_num;
  int	hit_sound;
  int		size;				// 3d size of longest dimension
  int		light;			// amount of light cast by this powerup, set in bitmaps.tbl
} POWERUP_TYPE_INFO;

class CObjPhysicsInfo {
public:
	tFixVector velocity;   /*velocity vector of this object */
	tFixVector thrust;     /*constant force applied to this object */
	int        mass;       /*the mass of this object */
	int        drag;       /*how fast this slows down */
	int        brakes;     /*how much brakes applied */
	tFixVector rotvel;     /*rotational velecity (angles) */
	tFixVector rotthrust;  /*rotational acceleration */
	short     turnroll;   /*rotation caused by turn banking */
	ushort     flags;      /*misc physics flags */

	void Read (CFileManager& fp, int version);
	void Write (CFileManager& fp, int version);
};

/*stuctures for different kinds of simulation */

class CObjLaserInfo {
public:
	short parent_type;     /* The type of the parent of this object */
	short parent_num;      /* The object's parent's number */
	int parent_signature;/* The object's parent's signature... */
	int   creation_time;   /*  Absolute time of creation. */
	char  last_hitobj;     /*  For persistent weapons (survive object collision), object it most recently hit. */
	char  track_goal;      /*  Object this object is tracking. */
	int   multiplier;      /*  Power if this is a fusion bolt (or other super weapon to be added). */

	void Read (CFileManager& fp, int version);
	void Write (CFileManager& fp, int version);
};

class CObjExplosionInfo {
public:
	int   spawn_time;     /* when lifeleft is < this, spawn another */
	int   delete_time;    /* when to delete object */
	char  delete_objnum;  /* and what object to delete */
	char  attach_parent;  /* explosion is attached to this object */
	char  prev_attach;    /* previous explosion in attach list */
	char  next_attach;    /* next explosion in attach list */

	void Read (CFileManager& fp, int version);
	void Write (CFileManager& fp, int version);
};

class CObjLightInfo {
public:
  int  intensity;    /*how bright the light is */

	void Read (CFileManager& fp, int version);
	void Write (CFileManager& fp, int version);
};

class CObjPowerupInfo {
public:
	int  count;      /*how many/much we pick up (vulcan cannon only?) */

	void Read (CFileManager& fp, int version);
	void Write (CFileManager& fp, int version);
};

class CObjVClipInfo {
public:
	int vclip_num;
	int	frametime;
	char	framenum;

	void Read (CFileManager& fp, int version);
	void Write (CFileManager& fp, int version);
};

/*structures for different kinds of rendering */

class CObjPolyModelInfo {
public:
	int      nModel;        /*which polygon model */
	tAngleVector anim_angles[MAX_SUBMODELS];  /*angles for each subobject */
	int      subobj_flags;     /*specify which subobjs to draw */
	int      tmap_override;    /*if this is not -1, map all face to this */
	char       alt_textures;     /*if not -1, use these textures instead */

	void Read (CFileManager& fp, int version);
	void Write (CFileManager& fp, int version);
};

class CObjAIInfo {
public:
	byte  behavior;            /*  */
	char   flags[MAX_AI_FLAGS]; /* various flags, meaning defined by constants */
	short  hide_segment;        /*  Segment to go to for hiding. */
	short  hide_index;          /*  Index in Path_seg_points */
	short  path_length;         /*  Length of hide path. */
	short  cur_path_index;      /*  Current index in path. */

	short  follow_path_start_seg;  /*  Start segment for robot which follows path. */
	short  follow_path_end_seg;    /*  End segment for robot which follows path. */

	int  danger_laser_signature;
	short  danger_laser_num;

	void Read (CFileManager& fp, int version);
	void Write (CFileManager& fp, int version);
};

class CSmokeInfo {
public:
	int			nLife;
	int			nSize [2];
	int			nParts;
	int			nSpeed;
	int			nDrift;
	int			nBrightness;
	byte			color [4];
	char			nSide;
	char			nType;
	char			bEnabled;

	void Read (CFileManager& fp, int version);
	void Write (CFileManager& fp, int version);
};

class CLightningInfo {
public:
	int			nLife;
	int			nDelay;
	int			nLength;
	int			nAmplitude;
	int			nOffset;
	short			nLightnings;
	short			nId;
	short			nTarget;
	short			nNodes;
	short			nChildren;
	short			nSteps;
	char			nAngle;
	char			nStyle;
	char			nSmoothe;
	char			bClamp;
	char			bPlasma;
	char			bSound;
	char			bRandom;
	char			bInPlane;
	char			bEnabled;
	byte			color [4];

	void Read (CFileManager& fp, int version);
	void Write (CFileManager& fp, int version);
};

class CSoundInfo {
public:
	int			nVolume;
	char			szFilename [40];
	char			bEnabled;

	void Read (CFileManager& fp, int version);
	void Write (CFileManager& fp, int version);
};

typedef struct tObjContentsInfo {
public:
	char			type; //  Type of object this object contains (eg, spider contains powerup) 
	char			id;   //  ID of object this object contains (eg, id = blue type = key) 
	char			count;// number of objects of type:id this object contains 
} tObjContentsInfo;

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
} tObjLocationInfo;

typedef struct tGameObject {
	short				signature;     // reduced size to save memory 
	char				type;          // what type of object this is... robot, weapon, hostage, powerup, fireball 
	char				id;            // which form of object...which powerup, robot, etc. 
	byte				controlType;  // how this object is controlled 
	byte				movementType; // how this object moves 
	byte				renderType;   //  how this object renders 
	byte				flags;         // misc flags 
	byte				multiplayer;   // object only available in multiplayer games 
	short				nSegment;      // segment number containing object 
	tObjContentsInfo contents;
	int				size;          // 3d size of object - for collision detection 
	int				shields;       // Starts at maximum, when <0, object dies.. 
} tGameObject;

class CGameObject : public CGameItem {
public:
	tGameObject			m_info;
	CObjLocationInfo	m_location;
	//movement info, determined by MOVEMENT_TYPE 
	union {
		CObjPhysicsInfo	physInfo; // a physics object 
		tFixVector			spinRate; // for spinning objects 
		} mType;

	//control info, determined by CONTROL_TYPE 
	union {
		CObjLaserInfo     laserInfo;
		CObjExplosionInfo explInfo;   //NOTE: debris uses this also 
		CObjAIInfo			aiInfo;
		CObjLightInfo     lightInfo;  //why put this here?  Didn't know what else to do with it. 
		CObjPowerupInfo   powerupInfo;
		} cType;

	//render info, determined by RENDER_TYPE 
	union {
		CObjPolyModelInfo	polyModelInfo;     //polygon model 
		CObjVClipInfo		vClipInfo;    //vclip 
		CSmokeInfo			smokeInfo;
		CLightningInfo		lightningInfo;
		CSoundInfo			soundInfo;
		} rType;

	//CGameObject () { memset (this, 0, sizeof (*this)); }

	virtual void Read (CFileManager& fp, int version = 0, bool bFlag = false);
	virtual void Write (CFileManager& fp, int version = 0, bool bFlag = false);
	virtual void Clear (void) {
		memset (&m_info, 0, sizeof (m_info)); 
		m_location.Clear ();
		}
	virtual CGameItem* Next (void) { return this + 1; }
};

#endif // OBJECT_TYPES_H

