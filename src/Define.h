#ifndef __define_h
#define __define_h

#include <math.h>

// -----------------------------------------------------------------------------

typedef signed char sbyte;
typedef unsigned char ubyte;
typedef unsigned short ushort;
typedef unsigned int uint;

#define _const_

// -----------------------------------------------------------------------------

#define ErrorMsg(a) AfxMessageBox(a)
#define QueryMsg(a) (DLE.ExpertMode () ? IDYES : AfxMessageBox (a, MB_YESNO))
#define Query2Msg(a,i) AfxMessageBox(a,i)
#define STATUSMSG(a) {if (DLE.MainFrame ()) DLE.MainFrame ()->StatusMsg(a);}
#define INFOMSG(a) {if (DLE.MainFrame () && !DLE.MineView ()->DelayRefresh ()) DLE.MainFrame ()->InfoMsg(a);}
#define DEBUGMSG(a) {}//{if (DLE.MainFrame ()) DLE.MainFrame ()->DebugMsg(a);}

#define sizeofa(_a) (sizeof (_a) / sizeof ((_a) [0]))

#define IN_RANGE(_value,_bounds) ((-(_bounds) <= (_value)) && ((_value) <= (_bounds)))

#define M_PI	3.141592653589793240
#define M_PI_2	(M_PI / 2.0)

// -----------------------------------------------------------------------------

static inline double Round (double value, double round = 1.0) { return (value >= 0) ? value + round / 2.0 : value - round / 2.0; }

// -----------------------------------------------------------------------------

template<typename _T> 
inline void Swap (_T& a, _T& b) { _T h = a; a = b; b = h; }

template<typename _T> 
inline _T Clamp (_T v, _T vMin, _T vMax) { return (v < vMin) ? vMin : (v > vMax) ? vMax : v; }

template<typename _T> 
inline _T MinVal (_T a, _T b) { return (a <= b) ? a : b; }

template<typename _T> 
inline _T MaxVal (_T a, _T b) { return (a >= b) ? a : b; }

// -----------------------------------------------------------------------------

inline void Wrap (short& value, short delta, short min, short max) 
{
value += delta;
if (value > max)
	value = min;
else if (value < min)
	value = max;
}

// -----------------------------------------------------------------------------

#define X2F(_v)	((float) (_v) / 65536.0f)
#define F2X(_v)	((int) Round ((double (_v)) * 65536.0f))
#define X2D(_v)	((double) (_v) / 65536.0)
#define D2X(_v)	((int) Round ((double (_v)) * 65536.0))
#define X2I(_v)	((_v) / 65536)
#define I2X(_v)	((int) (_v) * 65536)

// -----------------------------------------------------------------------------

inline int FixMul (int n, int m)
{
return (int) ((double) n * (double) m / 65536.0);
}

inline int FixDiv (int n, int m)
{
return (int) ((double) n / (double) m * 65536.0);
}

// -----------------------------------------------------------------------------

inline double Sign (double v) 
{ 
return (v < 0.0) ? -1.0 : 1.0; 
}

// -----------------------------------------------------------------------------

inline int FixLog (int x) 
{
return (x >= 1) ? (int) (log ((double) x) + 0.5) : 0; // round (assume value is positive)
}

//------------------------------------------------------------------------------

inline int FixExp (int x) 
{
return (x >= 0 && x <= 21) ? (int) (exp ((double) x) + 0.5) : 1; // round (assume value is positive)
}

// -----------------------------------------------------------------------------
//Some handy constants 
#define f0_5   (I2X(1) / 2)
#define f1_0   I2X(1)
#define f2_0   I2X(2)
#define f3_0   I2X(3)
#define f10_0  I2X(10)


#define F0_5   f0_5
#define F1_0   f1_0
#define F2_0   f2_0
#define F3_0   f3_0
#define F10_0  f10_0

#if FIX_IS_DOUBLE

#define xabs(_i)	fabs(_i)

#else

#define xabs(_i)	labs(_i)

#endif

//	-----------------------------------------------------------------------------------------------------------

#define MAX_JOIN_DISTANCE (20*20)

#define PREFS_SHOW_POINT_COORDINATES 1
#define PREFS_SHOW_CROSS_HAIR        2
#define PREFS_HIDE_TAGGED_BLOCKS     4
#define PREFS_DONT_SAVE_LIGHT_DELTAS 8

// My own defines
#define EXTENDED_HAM 1
#define NORMAL_HAM	0

#define LEVEL_VERSION				27	//current level version

#define MAX_PLAYERS_D2				8
#define MAX_PLAYERS_D2X				16
#define MAX_COOP_PLAYERS			3

#define NUM_HOT_BUTTONS				21

// defines for reading ham files
#define N_ROBOT_TYPES_D1			30
#define N_ROBOT_TYPES_D2			66
#define N_ROBOT_JOINTS_D2			1145
#define N_POLYGON_MODELS_D2		166
#define N_OBJBITMAPS_D2				422
#define N_OBJBITMAPPTRS_D2			502 // why is this not the same as N_OBJBITMAPS_D2???

#define MAX_ROBOT_TYPES				85
#define MAX_ROBOT_JOINTS			1250
#define MAX_POLYGON_MODELS			200
#define MAX_OBJ_BITMAPS				600
#define MAX_WEAPON_TYPES			65

// defines to support robot structures
#define MAX_EFFECTS_D2				110
#define MAX_EFFECTS_D1				60
#define MAX_ANIMATIONS_D2			110
#define MAX_ANIMATIONS_D1			70
#define MAX_WALL_ANIMATIONS_D2	60
#define MAX_WALL_ANIMATIONS_D1	30
#define MAX_ANIMATION_FRAMES		30
#define MAX_GUNS						8  //should be multiple of 4 for ubyte array
#define NDL								5  // Guessed at this value (B. Aamot 9/14/96)
#define N_ANIM_STATES				5

// the following are already defined below
// #define MAX_CLIP_FRAMES 50  // (called MAX_WALL_EFFECT_FRAMES_D2)
// #define MAX_SUBMODELS   10  // I guessed at this value (BAA)

// allow all caps types for vectors
#define RDL_FILE						0
#define RL2_FILE						1

#define SMALLMINE_ID					51

#define MINE_CLIP_NUMBER			159

#define N_WALL_TEXTURES_D1			26
#define N_WALL_TEXTURES_D2			51
#define NUM_OF_CLIPS_D1				24
#define NUM_OF_CLIPS_D2				49

#define MAX_WALL_TYPES				6
#define D2_MAX_WALL_TYPES			8

#define TEXTURE_STRING_TABLE_D1 1000
#define TEXTURE_STRING_TABLE_D2 2000
#define ROBOT_STRING_TABLE      3000
#define POWERUP_STRING_TABLE    4000
#define HOT_BUTTON_STRING_TABLE 5000

// macros
#define my_sqrt(a)	sqrt(fabs((double)(a)))
#define DISABLE_BUTTON(id) EnableWindow(GetDlgItem(HWindow,id),FALSE);
#define ENABLE_BUTTON(id)  EnableWindow(GetDlgItem(HWindow,id),TRUE);

// -----------------------------------------------------------------------------
// spline related constants
// object related constants
#define AIB_STILL                0x80
#define AIB_NORMAL               0x81
#define AIB_GET_BEHIND           0x82
#define AIB_RUN_FROM             0x83
#define AIB_SNIPE                0x84
#define AIB_STATION              0x85
#define AIB_FOLLOW_PATH          0x86

#define MIN_BEHAVIOR					0x80
#define MAX_BEHAVIOR					0x86

// -----------------------------------------------------------------------------

#define MAX_OBJECT_NUMBER			12
#define MAX_CONTAINS_NUMBER		2
#define HOSTAGE_CLIP_NUMBER		33
#define VCLIP_BIG_EXPLOSION		0
#define VCLIP_SMALL_EXPLOSION		2

#define D1_PLAYER_CLIP_NUMBER		43
#define D2_PLAYER_CLIP_NUMBER		108
#define PLAYER_CLIP_NUMBER			(DLE.IsD1File () ? D1_PLAYER_CLIP_NUMBER : D2_PLAYER_CLIP_NUMBER)

#define D1_COOP_CLIP_NUMBER		44
#define D2_COOP_CLIP_NUMBER		108
#define COOP_CLIP_NUMBER			(DLE.IsD1File () ? D1_COOP_CLIP_NUMBER : D2_COOP_CLIP_NUMBER)

#define D1_REACTOR_CLIP_NUMBER	39
#define D2_REACTOR_CLIP_NUMBER	93 // D2 version of D1 reactor, only referenced during conversion
#define REACTOR_CLIP_NUMBER		(DLE.IsD1File () ? D1_REACTOR_CLIP_NUMBER : D2_REACTOR_CLIP_NUMBER)

// -----------------------------------------------------------------------------

#define HOSTAGE_SIZE   0x50000L
#define PLAYER_SIZE    0x46c35L
#define REACTOR_SIZE   0xC14EDL
#define WEAPON_SIZE	  148158L

#define WEAPON_SHIELD  (20*F1_0)
#define DEFAULT_SHIELD 0x640000L
#define REACTOR_SHIELD 0xC82000L

#define MAX_AI_OPTIONS_D1 6
#define MAX_AI_OPTIONS_D2 9
#define MAX_AI_BEHAVIOR_TABLE 8

// -----------------------------------------------------------------------------
// segment insertion modes
#define ORTHOGONAL	0
#define EXTEND			1
#define MIRROR			2
#define N_CUBE_MODES 3

#define max(a,b)        (((a) > (b)) ? (a) : (b))
#define min(a,b)        (((a) < (b)) ? (a) : (b))

// -----------------------------------------------------------------------------

#define MAX_MACROS			100
#define MAX_TEXTURES_D1		584
#define MAX_TEXTURES_D2		910
#define MAX_WALL_FLAGS_D1	5
#define MAX_WALL_FLAGS_D2	9
#define MAX_WALL_FLAGS		9
#define NUM_LIGHTS_D1		48
#define NUM_LIGHTS_D2		85
#define MAX_SOUNDS_D2		254     // bad to have sound 255!
#define MAX_SOUNDS_D1		250     // bad to have sound 255!

#define COMPILED_MINE_VERSION 0

#define LEFT_BUTTON			0x01
#define RIGHT_BUTTON			0x02

#define ASPECT_TOP			3
#define ASPECT_BOT			5
#define ANGLE_RATE			0.01
#define MOVE_RATE				5

#define MAX_POLY				6

#define PI						3.141592653589793240

inline double Radians (double a) { return a * (PI / 180.0); }
inline double Degrees (double a) { 
	double d = a * (180.0 / PI); 
#if 1
	while (d < 0.0)
		d += 360.0;
	while (d > 360.0)
		d -= 360.0;
#else
	while (d < -180.0)
		d += 360.0;
	while (d > 180.0)
		d -= 360.0;
#endif
	return d;
	}

// -----------------------------------------------------------------------------
// macro modes 
#define MACRO_OFF			0
#define MACRO_RECORD		1
#define MACRO_PLAY		2

// -----------------------------------------------------------------------------
// select modes 
#define POINT_MODE		0
#define LINE_MODE			1
#define SIDE_MODE			2
#define SEGMENT_MODE		3
#define OBJECT_MODE		4
#define BLOCK_MODE		5
#define N_SELECT_MODES	6

// -----------------------------------------------------------------------------
// edit modes 
#define EDIT_OFF			0
#define EDIT_MOVE			1
#define N_EDIT_MODES		2

// -----------------------------------------------------------------------------

#define DEFAULT_SEGMENT 0
#define DEFAULT_SIDE    4
#define DEFAULT_EDGE    0 // line of the current side (0..3) 
#define DEFAULT_POINT   0 // point of the current side (0..3) 
#define DEFAULT_OBJECT  0

// -----------------------------------------------------------------------------

#define TAGGED_MASK  ubyte (0x80) // used on wallFlags & vertexStatus 
#define DELETED_MASK ubyte (0x40) // used on wallFlags & vertexStatus 
#define NEW_MASK     ubyte (0x20) // used on vertexStatus      
#define SHARED_MASK	ubyte (0x10)
#define ALIGNED_MASK ubyte (0x08) // indicates a side has already been aligned
#define TUNNEL_MASK	ubyte (0x04)
#define ALIGN_MASK   ubyte (0x02) // indicates a side needs aligning

// -----------------------------------------------------------------------------

#define SHOW_LINES_POINTS    0x01
#define SHOW_LINES_PARTIAL   0x02
#define SHOW_LINES_ALL       0x04
#define SHOW_LINES_NEARBY    0x08
#define SHOW_FILLED_POLYGONS 0x10

// -----------------------------------------------------------------------------

#define POWERUP_WEAPON_MASK  0x01
#define POWERUP_KEY_MASK     0x02
#define POWERUP_POWERUP_MASK 0x04
#define POWERUP_UNKNOWN_MASK 0xff // show the type if any other mask is on

#define POW_AMMORACK						35

// -----------------------------------------------------------------------------

#define MAX_ROBOT_ID_D1				24
#define MAX_ROBOT_IDS_TOTAL		78
#define MAX_POWERUP_IDS_D1			26
#define MAX_POWERUP_IDS_D2			50
#define MAX_POWERUP_IDS_USED		48
#define MAX_POWERUP_IDS				(DLE.IsD1File () ? MAX_POWERUP_IDS_D1 : MAX_POWERUP_IDS_D2)

// -----------------------------------------------------------------------------

#define POW_EXTRA_LIFE				0
#define POW_ENERGY					1
#define POW_SHIELD_BOOST			2
#define POW_LASER						3

#define POW_KEY_BLUE					4
#define POW_KEY_RED					5
#define POW_KEY_GOLD					6

#define POW_HOARD_ORB				7       // use unused slot
#define POW_MONSTERBALL				8

#define POW_CONCUSSION_1        	10
#define POW_CONCUSSION_4        	11      // 4-pack MUST follow single missile

#define POW_QUADLASER           	12

#define POW_VULCAN       			13
#define POW_SPREADFIRE   			14
#define POW_PLASMA       			15
#define POW_FUSION       			16
#define POW_PROXMINE    			17
#define POW_HOMINGMSL_1       	18
#define POW_HOMINGMSL_4       	19      // 4-pack MUST follow single missile
#define POW_SMARTMSL    			20
#define POW_MEGAMSL         		21
#define POW_VULCAN_AMMO         	22
#define POW_CLOAK               	23
#define POW_TURBO               	24
#define POW_INVUL				     	25
#define POW_MEGAWOW             	27

#define POW_GAUSS        			28
#define POW_HELIX        			29
#define POW_PHOENIX      			30
#define POW_OMEGA        			31

#define POW_SUPERLASER         	32
#define POW_FULL_MAP            	33
#define POW_CONVERTER           	34
#define POW_AMMORACK           	35
#define POW_AFTERBURNER         	36
#define POW_HEADLIGHT           	37

#define POW_FLASHMSL_1         	38
#define POW_FLASHMSL_4         	39      // 4-pack MUST follow single missile
#define POW_GUIDEDMSL_1    		40
#define POW_GUIDEDMSL_4    		41      // 4-pack MUST follow single missile
#define POW_SMARTMINE          	42

#define POW_MERCURYMSL_1   		43
#define POW_MERCURYMSL_4   		44      // 4-pack MUST follow single missile
#define POW_EARTHSHAKER 			45

#define POW_BLUEFLAG           	46
#define POW_REDFLAG            	47

#define POW_SLOWMOTION				48
#define POW_BULLETTIME				49

// -----------------------------------------------------------------------------

#define MAX_POWERUP_TYPES_D2			48
#define MAX_POWERUP_TYPES				50
#define D2X_POWERUP_TYPES				100
#define MAX_POWERUP_TYPES_D1  		29

#define MAX_OBJECTS_D1					350
#define MAX_OBJECTS_D2					2000

#define MAX_WALL_SWITCHES           50
#define MAX_WALL_LINKS              100
#define MAX_WALL_ANIMS_D1				30  // Maximum different types of doors Descent 1
#define MAX_WALL_ANIMS_D2				60  // Maximum different types of doors Descent 2
#define MAX_DOOR_ANIMS_D1				25  // Maximum different types of doors Descent 1
#define MAX_DOOR_ANIMS_D2				50  // Maximum different types of doors Descent 2
#define MAX_WALL_EFFECT_FRAMES_D1	20 // Descent 1
#define MAX_WALL_EFFECT_FRAMES_D2	50 // Descent 2
#define MAX_STUCK_OBJECTS	         32
#define MAX_SIDES_PER_SEGMENT			6
#define MAX_VERTICES_PER_SEGMENT		8
#define MAX_AI_FLAGS						11 // This MUST cause word (4 bytes) alignment in CObjAIInfo, allowing for one ubyte mode 
#define MAX_SUBMODELS					10 // I guessed at this value (BAA) 

// -----------------------------------------------------------------------------
// wall numbers 
#define WLEFT								0
#define WTOP								1
#define WRIGHT								2
#define WBOTTOM							3
#define WBACK								4
#define WFRONT								5

// -----------------------------------------------------------------------------
// misc 
#define DEFAULT_LIGHTING				F0_5      // (F1_0/2) 

// -----------------------------------------------------------------------------
// Trigger flags for Descent 1
#define TRIGGER_CONTROL_DOORS			1	// Control Trigger 
#define TRIGGER_SHIELD_DAMAGE			2	// Shield Damage Trigger 
#define TRIGGER_ENERGY_DRAIN			4	// Energy Drain Trigger 
#define TRIGGER_EXIT						8	// End of level Trigger 
#define TRIGGER_ON						16	// Whether Trigger is active 
#define TRIGGER_ONE_SHOT				32	// If Trigger can only be triggered once 
#define TRIGGER_MATCEN					64	// Trigger for materialization centers 
#define TRIGGER_ILLUSION_OFF			128	// Switch Illusion OFF trigger 
#define TRIGGER_SECRET_EXIT			256	// Exit to secret level 
#define TRIGGER_ILLUSION_ON			512	// Switch Illusion ON trigger 
#define TRIGGER_OPEN_WALL				1024
#define TRIGGER_CLOSE_WALL				2048
#define TRIGGER_MAKE_ILLUSIONARY		4096

// -----------------------------------------------------------------------------
// Trigger types for Descent 2
#define TT_OPEN_DOOR				0	// Open a door
#define TT_CLOSE_DOOR		   1	// Close a door
#define TT_MATCEN					2	// Activate a matcen
#define TT_EXIT					3	// End the level
#define TT_SECRET_EXIT		   4	// Go to secret level
#define TT_ILLUSION_OFF		   5	// Turn an illusion off
#define TT_ILLUSION_ON		   6	// Turn an illusion on
#define TT_UNLOCK_DOOR		   7	// Unlock a door
#define TT_LOCK_DOOR				8	// Lock a door
#define TT_OPEN_WALL				9	// Makes a wall open
#define TT_CLOSE_WALL			10	//	Makes a wall closed
#define TT_ILLUSORY_WALL		11	// Makes a wall illusory
#define TT_LIGHT_OFF				12	// Turn a light off
#define TT_LIGHT_ON				13	// Turn s light on
#define TT_TELEPORT				14
#define TT_SPEEDBOOST			15
#define TT_CAMERA					16
#define TT_SHIELD_DAMAGE_D2	17
#define TT_ENERGY_DRAIN_D2		18
#define TT_CHANGE_TEXTURE		19
#define TT_SMOKE_LIFE			20
#define TT_SMOKE_SPEED			21
#define TT_SMOKE_DENS			22
#define TT_SMOKE_SIZE			23
#define TT_SMOKE_DRIFT			24
#define TT_COUNTDOWN				25
#define TT_SPAWN_BOT				26
#define TT_SMOKE_BRIGHTNESS	27
#define TT_SET_SPAWN				28
#define TT_MESSAGE				29
#define TT_SOUND					30
#define TT_MASTER					31
#define TT_ENABLE_TRIGGER		32
#define TT_DISABLE_TRIGGER		33
#define TT_DISARM_ROBOT			34
#define TT_REPROGRAM_ROBOT		35
#define TT_SHAKE_MINE			36
#define NUM_TRIGGER_TYPES		37

#define TT_SHIELD_DAMAGE      100    // added to support d1 shield damage
#define TT_ENERGY_DRAIN       101    // added to support d1 energy drain

// -----------------------------------------------------------------------------
// Trigger flags for Descent 2
#define TF_NO_MESSAGE			1	// Don't show a message when triggered
#define TF_ONE_SHOT				2	// Only trigger once
#define TF_DISABLED				4	// Set after one-shot fires
#define TF_PERMANENT				8	// indestructable switch for repeated operation
#define TF_ALTERNATE				16 // switch will assume the opposite function after operation
#define TF_SET_ORIENT			32 // switch will assume the opposite function after operation
#define TF_SILENT					64
#define TF_AUTOPLAY				128

// -----------------------------------------------------------------------------

#define MAX_LIGHT_DELTA_INDICES_STD	500
#define MAX_LIGHT_DELTA_INDICES_D2X	3000
#define MAX_LIGHT_DELTA_VALUES_STD	10000
#define MAX_LIGHT_DELTA_VALUES_D2X	65536
#define LIGHT_DELTA_SCALE     		2048	// Divide light to allow 3 bits integer, 5 bits fraction.

// -----------------------------------------------------------------------------
// Texture flags
#define BM_FLAG_TRANSPARENT        1
#define BM_FLAG_SUPER_TRANSPARENT  2
#define BM_FLAG_NO_LIGHTING        4
#define BM_FLAG_RLE                8
#define BM_FLAG_PAGED_OUT          16
#define BM_FLAG_RLE_BIG            32
#define BM_DFLAG_ANIMATED          0x40
#define BM_DFLAG_ANIMFRAME_MASK    0x3F

// -----------------------------------------------------------------------------
// New unimplemented trigger ideas  
#define CONTROL_ROBOTS				8	// If Trigger modifies robot behavior 
#define CONTROL_LIGHTS_ON			16	// If Trigger turns on lights in a certain area 
#define CONTROL_LIGHTS_OFF			32	// If Trigger turns off lights in a certain area 
#define TRIGGER_CONTROL_ROBOTS	64	// If Trigger is a Door control trigger (Linked) 

// -----------------------------------------------------------------------------
// Various wall types. 
#define WALL_NORMAL				0  	// Normal wall 
#define WALL_BLASTABLE			1  	// Removable (by shooting) wall 
#define WALL_DOOR					2  	// Door  
#define WALL_ILLUSION			3  	// Wall that appears to be there, but you can fly thru 
#define WALL_OPEN					4	// Just an open side. (Trigger) 
#define WALL_CLOSED				5	// Wall.  Used for transparent walls. 
#define WALL_OVERLAY				6 	// Goes over an actual solid side.  For triggers (Descent 2)
#define WALL_CLOAKED				7 	// Can see it, and see through it  (Descent 2)
#define WALL_COLORED				8

// Various wall flags. 
#define WALL_BLASTED				1  	// Blasted out wall. 
#define WALL_DOOR_OPENED		2  	// Open door.  
#define WALL_RENDER_ADDITIVE	4
#define WALL_DOOR_LOCKED		8	// Door is locked. 
#define WALL_DOOR_AUTO			16	// Door automatically closes after time. 
#define WALL_ILLUSION_OFF		32	// Illusionary wall is shut off. 
#define WALL_WALL_SWITCH		64	// This wall is openable by a wall switch (Descent 2)
#define WALL_BUDDY_PROOF		128	// Buddy assumes he cannot get through this wall (Descent 2)
#define WALL_IGNORE_MARKER		256

//wall clip flags (new for Descent 2)
#define WCF_EXPLODES				1	//door explodes when opening
#define WCF_BLASTABLE			2	//this is a blastable wall
#define WCF_TMAP1					4	//this uses primary tmap, not tmap2
#define WCF_HIDDEN				8	//this uses primary tmap, not tmap2

// Wall states 
#define WALL_DOOR_CLOSED		0	// Door is closed 
#define WALL_DOOR_OPENING		1	// Door is opening. 
#define WALL_DOOR_WAITING		2	// Waiting to close 
#define WALL_DOOR_CLOSING		3	// Door is closing 
#define WALL_DOOR_CLOAKING		5	// Wall is going from closed -> open (Descent 2)
#define WALL_DOOR_DECLOAKING	6	// Wall is going from open -> closed (Descent 2)

// -----------------------------------------------------------------------------

#define KEY_NONE					1
#define KEY_BLUE					2
#define KEY_RED					4
#define KEY_GOLD					8

// -----------------------------------------------------------------------------
//Object types 
#define OBJ_NONE         255  // unused object 
#define OBJ_WALL           0  // A wall... not really an object, but used for collisions 
#define OBJ_FIREBALL       1  // a fireball, part of an explosion 
#define OBJ_ROBOT          2  // an evil enemy 
#define OBJ_HOSTAGE        3  // a hostage you need to rescue 
#define OBJ_PLAYER         4  // the player on the console 
#define OBJ_WEAPON         5  // a laser, missile, etc 
#define OBJ_CAMERA         6  // a camera to slew around with 
#define OBJ_POWERUP        7  // a powerup you can pick up 
#define OBJ_DEBRIS         8  // a piece of robot 
#define OBJ_REACTOR        9  // the control center 
#define OBJ_FLARE         10  // a flare 
#define OBJ_CLUTTER       11  // misc objects 
#define OBJ_GHOST         12  // what the player turns into when dead 
#define OBJ_LIGHT         13  // a light source, & not much else 
#define OBJ_COOP          14  // a cooperative player object. 
#define OBJ_MARKER		  15
#define OBJ_CAMBOT		  16
#define OBJ_MONSTERBALL	  17
#define OBJ_SMOKE			  18
#define OBJ_EXPLOSION	  19
#define OBJ_EFFECT		  20
#define MAX_OBJECT_TYPES  21

#define PARTICLE_ID			0
#define LIGHTNING_ID			1
#define SOUND_ID				2
#define WAYPOINT_ID			3
#define MAX_EFFECT_ID		WAYPOINT_ID

#define PARTICLE_TYPE_SMOKE		0
#define PARTICLE_TYPE_SPRAY		1
#define PARTICLE_TYPE_BUBBLES		2
#define PARTICLE_TYPE_FIRE			3
#define PARTICLE_TYPE_WATERFALL	4
#define PARTICLE_TYPE_RAIN			5
#define PARTICLE_TYPE_SNOW			6

// -----------------------------------------------------------------------------
//Result types 
#define RESULT_NOTHING  0 // Ignore this collision 
#define RESULT_CHECK    1 // Check for this collision 

// -----------------------------------------------------------------------------
//Control types - what tells this object what do do 
#define CT_NONE        0  // doesn't move (or change movement) 
#define CT_AI          1  // driven by AI 
#define CT_EXPLOSION   2  //explosion sequencer 
#define CT_FLYING      4  //the player is flying 
#define CT_SLEW        5  //slewing 
#define CT_FLYTHROUGH  6  //the flythrough system 
#define CT_WEAPON      9  //laser, etc. 
#define CT_REPAIRCEN  10  //under the control of the repair center 
#define CT_MORPH      11  //this object is being morphed 
#define CT_DEBRIS     12  //this is a piece of debris 
#define CT_POWERUP    13  //animating powerup blob 
#define CT_LIGHT      14  //doesn't actually do anything 
#define CT_REMOTE     15  //controlled by another net player 
#define CT_CNTRLCEN   16  //the control center/main reactor  
#define CT_WAYPOINT	 17

// -----------------------------------------------------------------------------
//Movement types 
#define MT_NONE       0  //doesn't move 
#define MT_PHYSICS    1  //moves by physics 
#define MT_SPINNING   3  //this object doesn't move, just sits and spins 

// -----------------------------------------------------------------------------
//Render types 
#define RT_NONE			0  //does not render 
#define RT_POLYOBJ		1  //a polygon model 
#define RT_FIREBALL		2  //a fireball 
#define RT_LASER			3  //a laser 
#define RT_HOSTAGE		4  //a hostage 
#define RT_POWERUP		5  //a powerup 
#define RT_MORPH			6  //a robot being morphed 
#define RT_WEAPON_VCLIP 7  //a weapon that renders as a vclip 
#define RT_THRUSTER		8	 // like afterburner, but doesn't cast light
#define RT_EXPLBLAST		9	 // white explosion light blast
#define RT_SHRAPNELS		10	 // white explosion light blast
#define RT_PARTICLE		11
#define RT_LIGHTNING    12
#define RT_SOUND			13

// -----------------------------------------------------------------------------
//misc object flags 
#define OF_EXPLODING       1  //this object is exploding 
#define OF_SHOULD_BE_DEAD  2  //this object should be dead, so next time we can, we should delete this object. 
#define OF_DESTROYED       4  //this has been killed, and is showing the dead version 
#define OF_SILENT          8  //this makes no sound when it hits a wall.  Added by MK for weapons,
                              //if you extend it to other types, do it completely! 
#define OF_ATTACHED       16  //this object is a fireball attached to another object 
#define OF_HARMLESS       32  //this object does no damage.  Added to make quad lasers do 1.5 damage as normal lasers. 

// -----------------------------------------------------------------------------
//physics flags 
#define PF_TURNROLL			1    // roll when turning 
#define PF_LEVELLING			2    // level object with closest side 
#define PF_BOUNCE				4    // bounce (not slide) when hit will 
#define PF_WIGGLE				8    // wiggle while flying 
#define PF_STICK				16    // object sticks (stops moving) when hits wall 
#define PF_PERSISTENT		32    // object keeps going even after it hits another object (eg, fusion cannon) 
#define PF_USES_THRUST		64    // this object uses its thrust 

// -----------------------------------------------------------------------------

#define SIDE_SHAPE_RECTANGLE  0
#define SIDE_SHAPE_TRIANGLE   1
#define SIDE_SHAPE_EDGE       2
#define SIDE_SHAPE_POINT      3
#define SIDE_SHAPE_UNDEFINED  4 // side has no vertex at all? should never happen

#define SEGMENT_SHAPE_CUBE    0
#define SEGMENT_SHAPE_WEDGE   1
#define SEGMENT_SHAPE_PYRAMID 2

// -----------------------------------------------------------------------------

#define MAX_TEXLIGHT		0x20000
#define TEXLIGHT_SCALE  1.28

// -----------------------------------------------------------------------------

#define LASER_INDEX         0
#define VULCAN_INDEX        1
#define SPREADFIRE_INDEX    2
#define PLASMA_INDEX        3
#define FUSION_INDEX        4
#define SUPER_LASER_INDEX   5
#define GAUSS_INDEX         6
#define HELIX_INDEX         7
#define PHOENIX_INDEX       8
#define OMEGA_INDEX         9

#define CONCUSSION_INDEX    0
#define HOMING_INDEX        1
#define PROXMINE_INDEX		 2
#define SMART_INDEX         3
#define MEGA_INDEX          4
#define FLASHMSL_INDEX      5
#define GUIDED_INDEX        6
#define SMARTMINE_INDEX     7
#define MERCURY_INDEX		 8
#define EARTHSHAKER_INDEX   9

#define SUPER_WEAPON        5

// -----------------------------------------------------------------------------

#if defined(_DEBUG) && !defined(_M_IA64) && !defined(_M_AMD64)
#	define BRK _asm int 3;
#	define CBRK(_cond)	if (_cond) BRK;
#else
#	define BRK
#	define CBRK(_cond)
#endif

#define atan3(y,x)  atan2((double) (y), (double) (x)) //((y) == (x)) ? atan2((double) (y) + 0.00000001, (double) (x)) : atan2((double) (y), (double) (x))

#define IMG_BKCOLOR	RGB (196,196,196)

#ifdef _DEBUG
#	define _DBG 1
#else
#	define _DBG 0
#endif

#define null NULL

// -----------------------------------------------------------------------------

#endif // __define_h
