#if 0

#include "mine.h"

#if 0

// declarations for easy reference

class vec3 {
	public:
		double x, y, z;

	vec3 () : x (0.0), y (0.0), z (0.0) {}
	vec3 (double x, double y, double z) : x (x), y (y), z (z) {}
	vec3 (vec3 v) : x (v.x), y (v.y), z (v.z) {}
};

class CVertex {
	public:
		vec3 v;

		CVertex () {}
		CVertex (double x, double y, double z) : v (x, y, z) {}
		CVertex (CVertex p) : v (p.v) {}

		double& operator[] (int i) { return (double*) &v [i]; };

		CVertex& operator= (CVertex other) {
			v = other.v;
			return *this;
			}

		CVertex& operator+= (CVertex other) {
			v.x += other.x;
			v.y += other.y;
			v.z += other.z;
			return *this;

		bool operator== (CVertex other) { return (v.x == other.x) && (v.y == other.y) && (v.z == other.z); }

		CVertex operator+ (CVertex v1, CVertex v2) { return CVertex (v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); }

		CVertex operator- (CVertex v1, CVertex v2) { return CVertex (v1.x - v2.x, v1.y - v2.y, v1.z - v2.z); }

		CVertex operator+ (CVertex v1, double s1, CVertex v2, double s2) { return CVertex (v1.x * s1 + v2.x * s2, v1.y * s1 + v2.y * s2, v1.z * s1 + v2.z * s2); }

		CVertex operator- (CVertex v1, double s1, CVertex v2, double s2) { return CVertex (v1.x * s1 - v2.x * s2, v1.y * s1 - v2.y * s2, v1.z * s1 - v2.z * s2); }

		CVertex operator* (double s) { return CVertex (v.x * s, v.y * s, v.z * s); }

		operator tPoint& () { return *((tPoint*) &v); }
	};

struct move_par_pnts {
    struct list *pnt_list, *side_list, *segment_list;
    tPoint *ref_pnt;
    tNode *segment;
    int wall;
};

struct move_par_objects {
    int withtagged;
    tNode *nc;
};
struct move_par_corr {
    tTunnelPath *pPath;
    tTunnel *tunnelP;
};

typedef union move_params {
    struct move_par_pnts point;
    struct move_par_objects object;
    struct move_par_corr corridor;
} tMoveParams;

typedef struct corridor_path {
    tPoint pos, orientation [3]; 
    double length, tangentVecLen, oldTwist; 	// tangentVecLen = length of tangential vector, oldTwist  ==  twist angle of last calculation 
    int fixed; 							//  <= 0 is moved from the program,  > 0 can be moved by the user 
    tTunnelPath * pModifyToo; 		// for circle, movecorr 
} tTunnelPath; 

typedef struct corridorelement {
    tPoint center, **points, *rel_points; 
    tNode **segments; 
} tTunnelElement; 

typedef struct corridor {
    struct list segments, points; 
    struct list elements; 
    int numVertices, numSegments; 		// in one corridor element 
    int * connected, * frontWall;	// array of size numSegments * 6. In each element j is segment_no * 6 + wall of the neighbour segment to segment j / 6 wall j % 6 saved ( - 1 if no neighbour). 
    double depth; 
    struct list path; 				// the points through which the corridor runs 
    struct leveldata * ld; 
    tPoint orientation [3]; 	// default coordsystem on the startside of the corridor (x, y, z vectors)
    tNode * startSegment; 			// the segment where the corridor starts 
    int startWall; 
     // the window stuff: 
    struct w_window * win; 
    struct w_button * b_ok, * b_cancel, * b_ins, * b_del, * b_setend, * b_depth, * b_stdform; 
} tTunnel; 

typedef struct node 
 { 
 tNode *succ,*pred; 
 int no;
 union {
  struct wi_window *w_w;
  struct ws_bitmap *w_bm;
  struct wi_menu *w_m;
  struct wi_button *w_b;
  struct w_description *w_d;
  tSegment *segP;
  struct producer *cp;
  tPoint *pPoint;
  struct listpoint *lp;
  struct macro *m;
  struct object *t;
  struct door *d;
  struct sdoor *sd;
  struct turnoff *to;
  struct changed_light *cl;
  struct infoitem *i;
  struct line *l;
  struct leveldata *lev;
  struct lightsource *ls;
  struct flickering_light *fl;
  struct ls_effect *lse;
  tTunnelElement *pCorrElem;
  tTunnelPath *pPath;
  struct movewall *mw;
  struct clickhit *ch;
  tNode *pNode;
  void *v; } item;
 } tNode;

typedef struct segment {
    unsigned char type;
    char prodnum;
    /* value: count every segment with type==4 and write this number in value. */
    unsigned char value;
    unsigned char flags;	/* is set by Descent ?? */
    uint16_t light;
    uint16_t high_light;
    uint16_t pts[8];
    uint16_t childSegmentIds[6];
    unsigned char doors[6];
    /* my stuff */
    struct list sdoors;
    struct wall *walls[6];
    tNode *vertices [8], *childSegments[6], *d[6];
    tNode *cp;	/* producer */
    tNode *tagged;	/* corresponding node in taglist */
    tNode *tagged_walls[6];
    struct polygon *polygons[12];
    unsigned char recalc_polygons[6];
    struct list objects;
    struct list fl_lights;	/* list of f.l. lights that affect this
				   segment */
} tSegment;

typedef tPoint {
	double xyz [3];
} tPoint;

#endif

class CTunnelPath {
	public:
		CDoubleVector	m_pos;
		CDoubleMatrix	m_orientation;
		double			m_length;
		double			m_tangentVecLen;
		double			m_oldTwist;
		bool				m_bFixed;
		CTunnelPath*	m_modify;
	};

class CTunnelElement {
	public:
		CFloatVector						m_center;
		CDynamicArray<CFloatVector*>	m_vertices;
		CDynamicArray<CFloatVector>	m_relVertices;
		CDynamicArray<CSegment>			m_segments;
	};

class CTunnel {
	public:
		CDLL<CSegment*, CSegment*>					m_segments;
		CDLL<CVertex*, CVertex*>					m_vertices;
		CDLL<CTunnelElement*, CTunnelElement*>	m_elements;
		CDLL<CTunnelPath*, CTunnelPath*>			m_path;
		int												m_nVertices;
		int												m_nSegments;
		CDoubleMatrix									m_orientation;
		CSideKey											m_start;
		int*												m_connected;
		CStack<short>									m_frontWalls;
		double											m_depth;

	CTunnel (CSideKey start = CSideKey (-1, -1)) : m_nVertices (0), m_nSegments (0), m_start (start), m_connected (null), m_depth (20.0 * 65536.0) {}
	int CreateStart (void);
	void CreateStartSegments (CSideKey key, CVertex& a, CVertex& nv)
	};

// -----------------------------------------------------------------------------  

void CTunnel::CreateStartSegments (CSideKey key, CVertex& a, CVertex& nv)
{
	//tNode * n; 
	int i, j, nbWall; 
//	struct nWall w; 

CSegment* segP = segmentManager.Segment (key);
if (segP->HasChild (key.m_nSide))
	return;
CSide* sideP = segP->Side (key.m_nSide);
 if (!sideP->IsTagged ())
	 return;
for (i = 0, j = sideP->VertexCount (); i < j; i++) { 
	CVertex* v = segP->Vertex (key.m_nSide, i);
	CVertex t = *v - a; 
	double f = Dot (t, nv); 
	if (f != 0.0f) 
		*v -= nv * f;
	}
m_frontWalls.Push (key.m_nSide);
sideP->UnTag ();
segP->Tag ();
segmentManager.Create (key);
checkmem (n = insertsegment (NULL, NULL, segment, nWall, tunnelP->depth)); 
for (i = 0; i < 4; i++) 
	insertpnt (n, wallpts [oppwalls [nWall]][i]); 
segment->item.segP->walls [nWall]->texture1 = w.texture1; 
segment->item.segP->walls [nWall]->texture2 = w.texture2; 
w = * segment->item.segP->walls [nWall]; 
for (i = 0; i < 4; i++) 
	segment->item.segP->walls [nWall]->corners [i] = w.corners [i]; 
change_segment_list (n, &l->segments, &l->pts, &tunnelP->segments, &tunnelP->points);   
for (i = 0; i < 4; i++)
	if (segment->item.segP->childSegments [nb_sides [nWall][i]] && (nbWall = findnbwalltoline (segment, segment->item.segP->childSegments [nb_sides [nWall][i]]->item.segP, nWall, i, (i + 1) & 3)) != -1)
		make_start_segments (tunnelP, segment->item.segP->childSegments [nb_sides [nWall][i]], nbWall, a, nv); 
}

// -----------------------------------------------------------------------------  
#if 0
inline tPoint& VecInc (tPoint& d, tPoint& v) { 
	for (int i = 0; i < 3; i++) 
		d += v; 
	return d; 
	}

inline tPoint& VecDec (tPoint& d, tPoint& v) { 
	for (int i = 0; i < 3; i++) 
		d -= v; 
	return d; 
	}

inline tPoint& VecIncScaled (tPoint& d, tPoint& v, double s) { 
	for (int i = 0; i < 3; i++) 
		d += v * s; 
	return d; 
	}

inline tPoint& VecDecScaled (tPoint& d, tPoint& v, double s) { 
	for (int i = 0; i < 3; i++) 
		d -= v * s; 
	return d; 
	}

inline tPoint& VecAdd (tPoint& d, tPoint& v1, tPoint& v2) { 
	for (int i = 0; i < 3; i++) 
		d = v1 + v2; 
	return d; 
	}

inline tPoint& VecSub (tPoint& d, tPoint& v1, tPoint& v2) { 
	for (int i = 0; i < 3; i++) 
		d = v1 - v2; 
	return d; 
	}

inline tPoint& VecAddScaled (tPoint& d, tPoint& v1, double s1, tPoint& v2, double s2) { 
	for (int i = 0; i < 3; i++) 
		d = v1 * s1 + v2 * s2; 
	return d; 
	}

inline tPoint& VecSubScaled (tPoint& d, tPoint& v1, double s1, tPoint& v2, double s2) { 
	for (int i = 0; i < 3; i++) 
		d = v1 * s1 - v2 * s2; 
	return d; 
	}

inline tPoint& VecScale (tPoint& d, double s) { 
	for (int i = 0; i < 3; i++) 
		d *= s; 
	return d; 
	}

inline tPoint& VecCopy (tPoint& d, tPoint& v) {
	for (int i = 0; i < 3; i++) 
		d = v; 
	return d; 
	}

inline tPoint& VecCopyScaled (tPoint& d, tPoint& v, double s) {
	for (int i = 0; i < 3; i++) 
		d = v * s; 
	return d; 
	}
#endif
// -----------------------------------------------------------------------------  
/*  make coordsystem for tangential vector m with twist
using ref_coords as reference coordsystem (last orientation). Store
the result in orientation and the new reference orientation (same
as the orientation without twist) in ref_coords. */ 

 void init_ce_coords (CVertex* m, double twist, CVertex* ref_coords, CDoubleMatrix& orientation)
{
	int i; 
	double l, a, b, q, r, s; 
	CVertex e [2]; 

orientation [2] = *m;  
normalize (&orientation [2]);  
 // construct two arbitrary vectors orthogonal to m (a righthanded coordsystem) 
CROSS (&e [0], &ref_coords [1], &orientation [2]); 
l = LENGTH (&e [0]); 
if (l < 0.1) 
	CROSS (&e [0], &ref_coords [0], &orientation [2]);  normalize (&e [0]);  
else 
	VecScale (e [0], l);
CROSS (&e [1], &orientation [2], &e [0]); 
// now maximize the function c [0] * rc [0] + c [1] * rc [1] with c [0] = r * e [0] + s * e [1], c [1] = -s * e [0] + r * e [1]  and r^2 + s^2 = 1 
a = DOT (&e [0], &ref_coords [0]) + DOT (&e [1], &ref_coords [1]); 
b = DOT (&e [1], &ref_coords [0]) - DOT (&e [0], &ref_coords [1]); 
q = sqrt (a * a + b * b); 
if (q >= 0.001) { 
	r = a / q;  
	s = b / q;  
	}
else {  // e [0] x e [1] is (anti)parallel to rc [0] x rc [1] 
	r = 1.0;  
	s = 0.0;  
	}
orientation [0] = e [0] * r + e [1] * s;
orientation [0] = e [1] * r - e [0] * s;
ref_coords [0] = orientation [0];  
ref_coords [1] = orientation [1]; 
turnsinglepnt (&orientation [2], cos (twist), sin (twist), &orientation [0]); 
turnsinglepnt (&orientation [2], cos (twist), sin (twist), &orientation [1]); 
}
 
// -----------------------------------------------------------------------------  
/*  initialize the relative points in the corridorelement ce corridor c.
Use orientation as coordsystem */ 

void init_ce_relpoints (tTunnel * tunnelP, tTunnelElement * pCorrElem, CVertex* orientation)
{
CVertex p; 
int i, j; 
for (i = 0; i < tunnelP->numVertices; i++) { 
	p = pCorrElem->points [i] - pCorrElem->center;
   INVMATRIXMULT (&pCorrElem->rel_points [i], orientation, &p);  
	}
}
 
// -----------------------------------------------------------------------------  

int connect_segment_layer (tTunnel * tunnelP, tNode * pStart)
{
	tNode * n, * n2; 
	int i, j, k; 

w_display_msgs (0); 
for (n = pStart, i = 0; n->succ != NULL && i < tunnelP->numSegments; n = n->succ, i++)
	for (j = 0; j < 6; j++)
		if (tunnelP->connected [i * 6 + j] >= 0 && n->item.segP->childSegments [j] == NULL) { 
			for (n2 = pStart, k = 0; n2->succ != NULL && k < tunnelP->connected [i * 6 + j] / 6; n2 = n2->succ, k++)
				; 
			if (n2->succ && n2->item.segP->childSegments [tunnelP->connected [i * 6 + j] % 6] == NULL)
				if (!connectsegments (&tunnelP->points, n, j, n2, tunnelP->connected [i * 6 + j] % 6)) 
					return 0; 
			}
w_display_msgs (1); 
return 1; 
}
 
// -----------------------------------------------------------------------------  
/*  Add a new layer of segments to corridor c. */ 

int new_segment_layer (tTunnel * tunnelP)
{
	tNode * fsegment, * n; 
	int i; 
	
my_assert (tunnelP->elements.Tail ()->succ != NULL); 
checkmem (fsegment = insertsegment (&tunnelP->segments, &tunnelP->points, tunnelP->elements.Tail ()->item.pCorrElem->segments [0], tunnelP->frontWall [0], tunnelP->depth)); 
for (i = 1; i < tunnelP->numSegments; i++)
	checkmem (insertsegment (&tunnelP->segments, &tunnelP->points, tunnelP->elements.Tail ()->item.pCorrElem->segments [i], tunnelP->frontWall [i], tunnelP->depth)); 
if (!connect_segment_layer (tunnelP, fsegment)) {
	for (n = fsegment; n->succ != NULL; n = n->succ) 
		deletesegment (&tunnelP->segments, &tunnelP->points, n); 
	return 0; 
	}
add_correlem (tunnelP, fsegment, 0); 
return 1; 
}
  
// -----------------------------------------------------------------------------  
/*  recalculate the corridor element n in corridor c. New orientation are a, 
orientation. prev_coords are the orientation of the previous corridor element. */ 

tNode * recalc_correlem (tTunnel * tunnelP, tNode * n, CVertex* pCenter, CVertex* orientation, CVertex* prev_coords)
{
int i, j; 
if (n->succ == NULL) { 
	if (!new_segment_layer (tunnelP)) return NULL;  
	n = tunnelP->elements.Tail (); 
	init_ce_relpoints (tunnelP, n->item.pCorrElem, prev_coords);  
	}
n->item.pCorrElem->center = *pCenter; 
for (i = 0; i < tunnelP->numVertices; i++) { 
	MATRIXMULT (n->item.pCorrElem->points [i], orientation, &n->item.pCorrElem->rel_points [i]); 
	n->item.pCorrElem->points [i] += n->item.pCorrElem->center;
	}
return n->succ; 
}

// -----------------------------------------------------------------------------  

void kill_corr_elem (tTunnel * tunnelP, tNode * n)
{ 
	int i, j; 

FREE (n->item.pCorrElem->points); 
for (i = 0; i < tunnelP->numSegments; i++)  {
	// to avoid a recalculation of some textures orientation which will cause a crash if the segment was an illegal one 
	for (j = 0; j < 6; j++)
		if (n->item.pCorrElem->segments [i]->item.segP->walls [j])
			n->item.pCorrElem->segments [i]->item.segP->walls [j]->locked = 1;  
	deletesegment (&tunnelP->segments, &tunnelP->points, n->item.pCorrElem->segments [i]); 
	}
FREE (n->item.pCorrElem->segments);  
FREE (n->item.pCorrElem);  
killnode (&tunnelP->elements, n); 
} 
   
// -----------------------------------------------------------------------------  

void corr_get_pnt (CVertex* ci, double t, CVertex* p)
{
p = ci [0] + ci [1] * t + ci [2] * (t * t) + ci [3] * (t * t * t);
}

// -----------------------------------------------------------------------------  

void corr_get_gradient (CVertex* ci, double t, CVertex* p)
{
p = ci [1] + ci [2] * (2 * t) + ci [3] * (3 * t * t);
}

// -----------------------------------------------------------------------------  

void corr_init_coeff (CVertex* ci, tTunnelPath * pathStartP, tTunnelPath * pathEndP)
{
	int i; 

ci [0] = pathStartP->pos;  
// This is the solution for a polynom of 3rd grade through two pts 
ci [1] = pathStartP->orientation [2] * pathStartP->tangentVecLen;  
ci [2] = pathEndP * 3 - pathStartP * 3 - 2 * pathStartP->orientation [2] * pathStartP->tangentVecLen - pathEndP->orientation [2] * pathEndP->tangentVecLen; 
ci [3] = pathStartP * 2 - pathEndP * 2+ pathStartP->orientation [2] * pathStartP->tangentVecLen + pathEndP->orientation [2] * pathEndP->tangentVecLen; 
}

// -----------------------------------------------------------------------------  
/*  return the twistvalue for the path from predecessor of t to t. ref_coords
are the orientation [0] and [1] of the corridor element of t calculating
out of the predecessor without twist */ 

double corr_get_twist (tTunnelPath * pPath, CVertex* ref_coords)
{
	CVertex orientation [3], n; 
	double twist; 

orientation [0] = ref_coords [0];  
orientation [1] = ref_coords [1]; 
init_ce_coords (&pPath->orientation [2], 0.0, orientation, orientation); 
CROSS (&n, &orientation [0], &pPath->orientation [0]); 
if (fabs (twist = DOT (&n, &pPath->orientation [2])) < 0.0001) 
	twist = 0; 
else 
	twist = twist < 0.0f ? -acos (DOT (&pPath->orientation [0], &orientation [0])) : acos (DOT (&pPath->orientation [0], &orientation [0])); 
while (twist - pPath->oldTwist < -M_PI) 
	twist += 2 * M_PI; 
while (twist - pPath->oldTwist > M_PI) 
	twist -= 2 * M_PI; 
pPath->oldTwist = twist; 
return twist; 
}
 
// -----------------------------------------------------------------------------  
/*  recalculate the complete path from st to et starting with 
corridorelement start. if the result is not correct, return a NULL
(the old orientation are not restored), otherwise a pointer to the last corridor 
element concerning to this path. */ 

tNode * recalc_path (tTunnel * tunnelP, tTunnelPath * pathStartP, tTunnelPath * pathEndP, tNode * pStart)
{
	CVertex ci [COEFF_NUM], dx1, ref_coords [2], * cp, old_coords [3]; 
	int i, num_parts, count; 
	double s, last_s, t, dt, d_twist, twist, overall_twist; 
	tNode * n, * tn; 
	struct list curve; 

dx1 = pathEndP - pathStartP; 
if (LENGTH (&dx1) < 2 * tunnelP->depth) 
	return 0; 
corr_init_coeff (ci, pathStartP, pathEndP); 
// Now divide the curve in oldlength / depth * CORR_FINENESS parts and step through it
num_parts = pathEndP->length / tunnelP->depth * CORRIDOR_FINENESS + 0.5; 
if (num_parts < CORRIDOR_FINENESS * 2) 
	return 0; 
dt = 1.0 / num_parts; 
// untwist the coordsystems first
initlist (&curve); 
ref_coords [0] = pathStartP->orientation [0];  
ref_coords [1] = pathStartP->orientation [1]; 
// t is the parameter of the curve, s is the length 
for (t = 0.0, s = last_s = 0.0, count = 0, n = NULL; t <= 1.0; t += dt) { 
	corr_get_gradient (ci, t, &dx1); 
	s += sqrt (DOT (&dx1, &dx1)) * dt; 
	count++; 
	if (s - last_s >= tunnelP->depth) { 
		checkmem (cp = MALLOC (sizeof (CVertex) * 4)); 
		checkmem (n = addnode (&curve, -1, cp)); 
		init_ce_coords (&dx1, 0.0, ref_coords, &cp [1]); 
		corr_get_pnt (ci, t, &cp [0]);  n->no = 0; 
		if (count < CORRIDOR_FINENESS) { 
			dt /= 2;  
			n->no = 1;  
			}
		else if (count > CORRIDOR_FINENESS * 2) { 
			dt *= 2;  
			n->no = -1;  
			}
		last_s = s;  
		count = 0;  
		}
	}
if (n == NULL) 
	return NULL; 
pathEndP->length = s; 
if (s - last_s < tunnelP->depth / 2) { 
	n = n->pred;  
	FREE (n->succ->item.p);  
	killnode (&curve, n->succ);  
	}
//  OK, now calculate the corresponding twisted segments 
overall_twist = corr_get_twist (pathEndP, &n->item.p [1]); 
ref_coords [0] = pathStartP->orientation [0];  
ref_coords [1] = pathStartP->orientation [1]; 
for (i = 0; i < 3; i++) 
	old_coords [i] = pathStartP->orientation [i]; 
// n = recalc_correlem (tunnelP, pStart, &pathStartP, pathStartP->orientation, old_coords);  
n = pStart->succ; 
d_twist = overall_twist / (curve.size + 1); 
for (tn = curve.Head ()->succ, twist = 0.0; tn != NULL; tn = tn->succ, twist += d_twist) {
	turn (&tn->pred->item.p [1], &tn->pred->item.p [1], 0, 1, 2, twist); 
	n = recalc_correlem (tunnelP, n, &tn->pred->item.p [0], &tn->pred->item.p [1], old_coords);
	if (n == NULL) { 
		pStart = NULL;  
		break;  
		}
	for (i = 0; i < 3; i++) 
		old_coords [i] = tn->pred->item.p [1 + i]; 
	if (tn->pred->no > 0) 
		d_twist /= 2;  
	else if (tn->pred->no < 0) 
		d_twist *= 2; 
	FREE (tn->pred->item.p);  
	killnode (&curve, tn->pred); 
	}
if (pStart == NULL || (n = recalc_correlem (tunnelP, n, &pathEndP, pathEndP->orientation, old_coords)) == NULL)
	return NULL; 
for (tn = pStart; tn != n && tn->succ != NULL; tn = tn->succ)
	for (i = 0; i < tunnelP->numSegments; i++)
		if (!testsegment (tn->item.pCorrElem->segments [i], 0)) 
			return NULL;  
return n->pred; 
}

// -----------------------------------------------------------------------------  

int recalc_corridor (tTunnel * tunnelP)
{
	tNode * pPath, * pCorrElem = tunnelP->elements.Head (); 
	CVertex d; 
	int i; 

my_assert (tunnelP->path.size > 1);   /*  at least two points */ 
for (pPath = tunnelP->path.Head ()->succ; pPath->succ != NULL; pPath = pPath->succ) {
	d = pPath->item.pPath->pos - pPath->pred->item.pPath->pos; 
	if (LENGTH (&d) < 3 * tunnelP->depth) 
		return 0; 
	pCorrElem = recalc_path (tunnelP, pPath->pred->item.pPath, pPath->item.pPath, pCorrElem);
	if (pCorrElem == NULL) 
		return 0; 
	}
for (pPath = pCorrElem->succ->succ; pPath != NULL; pPath = pPath->succ) 
	kill_corr_elem (tunnelP, pPath->pred); 
return 1; 
}
 
// -----------------------------------------------------------------------------  

int movecorr (CVertex* pDelta, union move_params * params)
{ 
	tTunnel * tunnelP; 
	tTunnelPath * pPath, * modPath, * curPath [5], oldPath [5]; 
	CVertex d, r; 
	double f; 
	int i, ok = 0; 

tunnelP = params->corridor.tunnelP; 
pPath = params->tunnelP.pPath; 
if (pPath->fixed == -1) 
	return 0; 
plotcorridor (tunnelP); 
switch (tunnelP->b_stdform->item.ls->selected) {
	case 0: 
		oldPath [0] = *pPath; 
		pPath += pDelta;  
		ok = recalc_corridor (tunnelP); 
		if (!ok) { 
			*pPath = oldPath [0];  
			recalc_corridor (tunnelP);  
			}
		break; 

	case 1: 
		modPath = pPath->pModifyToo; 
		oldPath [0] = *pPath; 
		oldPath [1] = *modPath; 
		pPath += pDelta;  
		d = pPath - modPath;  
		pPath->tangentVecLen = modPath->tangentVecLen = LENGTH (&d); 
		pPath->orientation [2] = modPath->orientation [2] = d / pPath->tangentVecLen; 
		init_ce_coords (&pPath->orientation [2], 0.0, &pPath->orientation [0], pPath->orientation); 
		init_ce_coords (&modPath->orientation [2], 0.0, &modPath->orientation [0], modPath->orientation); 
		ok = recalc_corridor (tunnelP); 
		if (!ok) { 
			*pPath = oldPath [0];  
			*modPath = oldPath [1];  
			recalc_corridor (tunnelP);  
			}
		break; 

	case 2: 
		curPath [0] = pPath; 
		for (i = 1; i < 5; i++) 
			curPath [i] = curPath [i - 1]->pModifyToo; 
		for (i = 0; i < 5; i++) 
			oldPath [i] = *curPath [i]; 
		f = DOT (pDelta, &pPath->orientation [2]);  
		r = pDelta - pPath->orientation [2] * f;  
		if (curPath [4]->fixed) { // ->pPath = mt1 
			curPath [1]->tangentVecLen = curPath [3]->tangentVecLen = curPath [4]->tangentVecLen = curPath [4]->tangentVecLen + CIRCLE_FACTOR * ((DOT (&r, &pPath->orientation [1]) < 0.0f) ? -LENGTH (&r) : LENGTH (&r)); 
			for (i = 0; i < 3; i++) { 
				curPath [0] += r;  
				curPath [2] -= r;  
				}
			}
		else { // ->pPath = mt2 
			curPath [1]->tangentVecLen = curPath [4]->tangentVecLen = curPath [4]->tangentVecLen + CIRCLE_FACTOR * ((DOT (&r, &pPath->orientation [1]) < 0.0f) ? -LENGTH (&r) : LENGTH (&r)); 
			for (i = 0; i < 3; i++) { 
				curPath [0] += r; 
				curPath [1] += r / 2.0; 
				curPath [4] += r / 2.0;  
				}
			}
		ok = recalc_corridor (tunnelP); 
		if (!ok) { 
			for (i = 0; i < 5; i++) 
				*curPath [i] = oldPath [i];  
			recalc_corridor (tunnelP);  
		}
	break; 
	}
plotcorridor (tunnelP); 
return ok; 
}
 
// -----------------------------------------------------------------------------  

int turncorr (CVertex* t_axis, double fx, double fy, int with_left, union move_params * params)
{
	tTunnel * c; 
	CVertex oldx, orientation [3]; 
	int ok, i, j, k; 
	tTunnelPath * t; 
	
c = params->c.c;  
t = params->c.t; 
if (c->b_stdform->item.ls->selected != 0) 
	return 0; 
plotcorridor (c); 
oldx = t;  
for (i = 0; i < 3; i++) 
	orientation [i] = t->orientation [i]; 
fx = fx >= M_PI ? fx - 2 * M_PI : (fx <= -M_PI ? fx + 2 * M_PI : fx); 
fy = fy >= M_PI ? fy - 2 * M_PI : (fy <= -M_PI ? fy + 2 * M_PI : fy); 
if (!with_left && fabs (fx) < fabs (fy)) { 
	t->tangentVecLen += fy / M_PI * c->depth;                          
	if (t->tangentVecLen < c->depth) 
		t->tangentVecLen = c->depth;  
	}
else {
	if (!with_left) { 
		i = 0;  j = 1;  
		k = 2;  
		}
	else if (fabs (fx) > fabs (fy)) { 
		i = 2;  
		j = 0;  
		k = 1;  
		}
	else { 
		i = 1;  
		j = 2;  
		k = 0;  
		}
	turn (orientation, t->orientation, i, j, k, k == 0 ? fy : fx); 
	}
ok = recalc_corridor (c); 
if (!ok) { 
	t = oldx;  
	for (i = 0; i < 3; i++) 
		t->orientation [i] = orientation [i];  
	recalc_corridor (c);  
	}
plotcorridor (c); 
return ok; 
}

// -----------------------------------------------------------------------------  

 /*  Changes the segment from the lists lc1, lp1 to lc2, lp2. All points
 must be unique and new. No doors allowed. */ 
void change_segment_list (tNode * n, struct list * lc1, struct list * lp1, struct list * lc2, struct list * lp2)
{
	int i; 

unlistnode (lc1, n); 
listnode_tail (lc2, n); 
for (i = 0; i < 8; i++) { 
	unlistnode (lp1, n->item.segP->vertices [i]);  
	listnode_tail (lp2, n->item.segP->vertices [i]);  
	}
}
 
// -----------------------------------------------------------------------------  

void init_connected (tTunnel * tunnelP)
{
	int i, nSegment, nbWall; 
	tNode * nc, * segP; 

my_assert (tunnelP->connected = MALLOC (tunnelP->numSegments * 6 * sizeof (int))); 
sortlist (&l->tagged [tt_segment], 0); 
my_assert (l->tagged [tt_segment].size == tunnelP->numSegments); 
for (segP = l->tagged [tt_segment].Head (), nSegment = 0; segP->succ != NULL && nSegment < tunnelP->numSegments; nSegment++, segP = segP->succ) {
	for (i = 0; i < 6; i++) {
		tunnelP->connected [nSegment * 6 + i] = -1; 
		if ((nc = segP->item.pNode->item.segP->childSegments [i]) != NULL && testtag (tt_segment, nc)) {
			for (nbWall = 0; nbWall < 6; nbWall++) 
				if (nc->item.segP->childSegments [nbWall] == segP->item.pNode) 
					break; 
			my_assert (nbWall < 6); 
			tunnelP->connected [nSegment * 6 + i] = nc->item.segP->tagged->no * 6 + nbWall;  
			}
		}
	}
}
 
// -----------------------------------------------------------------------------  

int CTunnel::CreateStart (void)
{
	int i; 

//untagall (tt_segment); 
CVertex a = *current->Vertex ();
CVertex m1 = *current->Vertex (1) - a;
CVertex m2 = *current->Vertex (-1) - a;
m1.Normalize ();  
m2.Normalize ();  
CVertex nv = CrossProduct (m1, m2);
CreateStartSegments (*current, &a, &nv); 
init_connected (tunnelP); 
untagall (tt_segment); 
if (tunnelP->numSegments == 0) 
	return 0; 
connect_segment_layer (m_segments.Head ());  
tunnelP->numVertices = tunnelP->points.size / 2; 
add_correlem (tunnelP, tunnelP->segments.Head (), 1);  
add_correlem (tunnelP, tunnelP->segments.Head (), 0); 
for (i = 1; i < CORRIDOR_STDLENGTH; i++) 
	new_segment_layer (tunnelP); 
return 1; 
}
 
// -----------------------------------------------------------------------------  

void delete_corridor (tTunnel * c)
{
	tNode * n; 

FREE (c->connected);  
FREE (c->frontWall); 
freelist (&c->segments, freesegment); 
freelist (&c->points, freelistpnt); 
for (n = c->elements.Head ()->succ; n != NULL; n = n->succ) { 
	FREE (n->pred->item.pCorrElem->points);  
	FREE (n->pred->item.pCorrElem->rel_points); 
	FREE (n->pred->item.pCorrElem->segments);   
	FREE (n->pred->item.pCorrElem);  
	FREE (n->pred);  
	}
freelist (&c->path, free); 
FREE (c); 
}
 
// -----------------------------------------------------------------------------  

void finish_corridor (tTunnel * c)
{
	tNode * n, * np, **first_layer; 
	int i; 

sortlist (&l->segments, 0);  sortlist (&l->pts, 0); 
checkmem (first_layer = MALLOC (sizeof (tNode * ) * c->numSegments)); 
for (i = 0, n = c->segments.Head (); i < c->numSegments; i++, n = n->succ) {
	my_assert (n->succ != NULL);  
	first_layer [i] = n;  
	}
for (n = c->segments.Head ()->succ; n != NULL; n = n->succ) { 
	unlistnode (&c->segments, np = n->pred);  
	listnode_tail (&l->segments, np);  
	}
for (n = c->points.Head ()->succ; n != NULL; n = n->succ) { 
	unlistnode (&c->points, np = n->pred);  
	listnode_tail (&l->pts, np);  
	}
for (i = 0; i < c->numSegments; i++)
	connectsides (first_layer [i], oppwalls [c->frontWall [i]]); 
for (n = first_layer [0]; n->succ != NULL; n = n->succ)
	for (i = 0; i < 6; i++) 
		if (n->item.segP->walls [i] != NULL) 
			recalcwall (n->item.segP, i); 
FREE (first_layer); 
delete_corridor (c); 
}

// -----------------------------------------------------------------------------  

void close_corr_win (tTunnel * c)
{
w_deletebutton (c->b_ok);  
w_deletebutton (c->b_cancel); 
w_deletebutton (c->b_ins);  
w_deletebutton (c->b_del); 
w_deletebutton (c->b_setend); 
c->b_ok = c->b_cancel = c->b_ins = c->b_del = c->b_setend = NULL; 
if (view.whichdisplay) { 
	corr_win2_xpos = w_xwinspacecoord (c->win->xpos); 
	corr_win2_ypos = w_ywinspacecoord (c->win->ypos);  
	}
else { 
	corr_win1_xpos = w_xwinspacecoord (c->win->xpos); 
	corr_win1_ypos = w_ywinspacecoord (c->win->ypos);  
	}
w_closewindow (c->win);  
c->win = NULL; 
l->cur_corr = NULL;  
plotlevel ();  
}

// -----------------------------------------------------------------------------  

void closerout_corr_win (struct w_window * w, void * d)
{ 
if (yesnomsg (TXT_FINISHCORRIDOR)) 
	finish_corridor (l->cur_corr); 
else if (yesnomsg (TXT_DELETECORRIDOR)) 
	delete_corridor (l->cur_corr); 
else 
	return; 
close_corr_win (l->cur_corr); 
}
 
// -----------------------------------------------------------------------------  

void corr_ok (struct w_button * b)
{
if (!yesnomsg (TXT_FINISHCORRIDOR)) 
	return; 
finish_corridor (l->cur_corr);  
close_corr_win (l->cur_corr);  
}
 
// -----------------------------------------------------------------------------  

void corr_cancel (struct w_button * b)
{
if (!yesnomsg (TXT_DELETECORRIDOR)) 
	return; 
delete_corridor (l->cur_corr);  
close_corr_win (l->cur_corr);  
}

// -----------------------------------------------------------------------------  

void change_num_tp (int insdel)
{
	tTunnelPath * new_tp, * pathStartP, * pathEndP; 
	CVertex ci [COEFF_NUM], ref_coords [2], dx, orientation [3]; 
	tNode * n, * last; 
	int num_parts; 
	double t, length, s, last_s, dt; 
	struct list new_path, old_path; 

initlist (&new_path); 
checkmem (new_tp = MALLOC (sizeof (tTunnelPath))); 
*new_tp = *l->cur_corr->path.Head ()->item.pPath; 
checkmem (last = addnode (&new_path, -1, new_tp)); 
for (length = 0.0, n = l->cur_corr->path.Head ()->succ; n->succ != NULL; n = n->succ)
	length += n->item.pPath->length; 
for (s = last_s = 0.0, n = l->cur_corr->path.Head ()->succ; n->succ != NULL; n = n->succ) {
	pathStartP = n->pred->item.pPath;  
	pathEndP = n->item.pPath; 
	num_parts = pathEndP->length / l->cur_corr->depth * CORRIDOR_FINENESS + 0.5; 
	dt = 1.0 / num_parts;  
	corr_init_coeff (ci, pathStartP, pathEndP); 
	ref_coords [0] = pathStartP->orientation [0];  
	ref_coords [1] = pathStartP->orientation [1]; 
	for (t = 0.0; t <= 1.0; t += dt) {
		corr_get_gradient (ci, t, &dx); 
		s += sqrt (DOT (&dx, &dx)) * dt; 
		// this must be done before the if because ref_coords are changed 
		init_ce_coords (&dx, 0.0, ref_coords, orientation); 
		if (s - last_s >= length / (l->cur_corr->path.size - 1 + insdel)) {
			checkmem (new_tp = MALLOC (sizeof (tTunnelPath))); 
			checkmem (addnode (&new_path, -1, new_tp)); 
			new_tp->fixed = 0;  
			new_tp->length = s;  
			new_tp->tangentVecLen = LENGTH (&dx); 
			new_tp->oldTwist = pathEndP->oldTwist * (s - pathStartP->length) / pathEndP->length; 
			corr_get_pnt (ci, t, &new_tp); 
			turn (orientation, new_tp->orientation, 0, 1, 2, new_tp->oldTwist); 
			last_s = s; 
			}
		}
	}
if (new_path.size <= (l->cur_corr->path.size - 1 + insdel)) {
	checkmem (new_tp = MALLOC (sizeof (tTunnelPath))); 
	checkmem (addnode (&new_path, -1, new_tp)); 
	new_tp->fixed = 0;  
	}
*new_path.Tail ()->item.pPath = *l->cur_corr->path.Tail ()->item.pPath; 
copylisthead (&old_path, &l->cur_corr->path); 
copylisthead (&l->cur_corr->path, &new_path); 
if (!recalc_corridor (l->cur_corr)) { 
	printmsg (TXT_CORRWEIRD);  
	copylisthead (&new_path, &l->cur_corr->path); 
	copylisthead (&l->cur_corr->path, &old_path); 
	recalc_corridor (l->cur_corr);  freelist (&new_path, free);  
	}
else freelist (&old_path, free); 
plotlevel (); 
 }

// -----------------------------------------------------------------------------  

void corr_ins_tp (struct w_button * b) 
{ 
change_num_tp (1); 
}

// -----------------------------------------------------------------------------  

void corr_del_tp (struct w_button * b)  
{
if (l->cur_corr->path.size > 2) 
	change_num_tp (-1); 
} 

// -----------------------------------------------------------------------------  

void corr_setend (struct w_button * b)
{
	int i, j; 
	tTunnelPath * end, oldend; 
	CVertex e1, e2; 
	tSegment * segP = view.pCurrSegment->item.segP; 
	double f; 

my_assert (l != NULL && l->cur_corr != NULL); 
plotcorridor (l->cur_corr); 
end = l->cur_corr->path.Tail ()->item.pPath;  end->oldTwist = 0.0; 
oldend = * end; 
end->pos = CVertex (0.0, 0.0, 0.0);
for (i = 0; i < 4; i++) 
	end->pos += *segP->vertices [wallpts [view.currWall][i]]->item.pPoint; 
end->pos /= 4.0f; 
e1 = *segP->vertices [wallpts [view.currWall][(view.currEdge + 1) & 3]]->item.pPoint - *segP->vertices [wallpts [view.currWall][view.currEdge]]->item.pPoint; 
e2 = *segP->vertices [wallpts [view.currWall][(view.currEdge - 1) & 3]]->item.pPoint - *segP->vertices [wallpts [view.currWall][view.currEdge]]->item.pPoint; 
normalize (&e1);  
normalize (&e2);  
f = DOT (&e1, &e2); 
end->orientation [0] = e1; 
end->orientation [1] = e2 - e1 * f; 
normalize (&end->orientation [1]); 
CROSS (&end->orientation [2], &end->orientation [0], &end->orientation [1]);  
i = recalc_corridor (l->cur_corr); 
if (!i) { 
	*end = oldend;  
	recalc_corridor (l->cur_corr);  
	printmsg (TXT_CORRWEIRD);  
	}
plotcorridor (l->cur_corr); 
}

// -----------------------------------------------------------------------------  

void corr_form (struct w_button * b)
{ 
	tTunnel * tunnelP = b->data; 
	tNode * n; 
	tTunnelPath * pathStartP, * pathEndP, * mt1, * mt2, * mt3; 
	int i, j; 

switch (b->item.ls->selected) {
	case 0:  /*  user defined */  
		w_activatebutton (tunnelP->b_ins);  
		w_drawbutton (tunnelP->b_ins); 
		w_activatebutton (tunnelP->b_del);  
		w_drawbutton (tunnelP->b_del); 
		w_activatebutton (tunnelP->b_setend);  
		w_drawbutton (tunnelP->b_setend); 
		for (n = tunnelP->path.Head ()->succ; n->succ != NULL; n = n->succ) 
			n->item.pPath->fixed = 0; 
		pathStartP = tunnelP->path.Head ()->item.pPath; 
		pathEndP = tunnelP->path.Tail ()->item.pPath; 
		pathStartP->fixed = -1; 
		if (pathStartP->pos == pathEndP->pos) 
				pathEndP->pos -= pathEndP->orientation [2] * MIN_CUBEDEPTH * 65536.0; 
		break; 

	case 1:  /*  straight */ 
		w_undrawbutton (tunnelP->b_ins);  
		w_deactivatebutton (tunnelP->b_ins); 
		w_undrawbutton (tunnelP->b_del);  
		w_deactivatebutton (tunnelP->b_del); 
		w_undrawbutton (tunnelP->b_setend);  
		w_deactivatebutton (tunnelP->b_setend); 
		if (tunnelP->path.size > 2)
		for (n = tunnelP->path.Head ()->succ->succ; n->succ != NULL; n = n->succ) { 
			FREE (n->pred->item.pPath);  
			killnode (&tunnelP->path, n->pred);  
			}
		pathStartP = tunnelP->path.Head ()->item.pPath;  
		pathEndP = tunnelP->path.Tail ()->item.pPath; 
		for (i = 0; i < 3; i++) 
			pathStartP->orientation [i] = pathEndP->orientation [i] = tunnelP->orientation [i]; 
		pathStartP->tangentVecLen = pathEndP->tangentVecLen = pathEndP->length = CORRIDOR_STDLENGTH * tunnelP->depth; 
		pathStartP->oldTwist = pathEndP->oldTwist = 0.0;  
		pathEndP->pModifyToo = pathStartP; 
		pathEndP = pathStartP + pathEndP->length * pathStartP->orientation [2]; 
		pathStartP->fixed = 1;  
		pathEndP->fixed = 0; 
		break; 

	case 2:  /*  circle */ 
		w_undrawbutton (tunnelP->b_ins);  
		w_deactivatebutton (tunnelP->b_ins); 
		w_undrawbutton (tunnelP->b_del);  
		w_deactivatebutton (tunnelP->b_del); 
		w_undrawbutton (tunnelP->b_setend);  
		w_deactivatebutton (tunnelP->b_setend); 
		if (tunnelP->path.size > 2)
			for (n = tunnelP->path.Head ()->succ; n->succ != NULL; n = n->succ) {
				FREE (n->pred->item.pPath);  
				killnode (&tunnelP->path, n->pred);  
				}
		for (i = 0; i < 3; i++) {
			checkmem (pathStartP = MALLOC (sizeof (tTunnelPath))); 
			checkmem (addnode (&tunnelP->path, -1, pathStartP));  
			}
		pathStartP = tunnelP->path.Head ()->item.pPath;  
		pathEndP = tunnelP->path.Tail ()->item.pPath; 
		mt1 = tunnelP->path.Head ()->succ->item.pPath;  
		mt3 = tunnelP->path.Tail ()->pred->item.pPath; 
		mt2 = tunnelP->path.Head ()->succ->succ->item.pPath; 
		pathStartP->oldTwist = pathEndP->oldTwist = mt1->oldTwist = mt2->oldTwist = mt3->oldTwist = 0.0; 
		pathStartP->fixed = pathEndP->fixed = mt3->fixed = 1;  
		mt1->fixed = mt2->fixed = 0; 
		pathStartP->tangentVecLen = mt1->tangentVecLen = mt2->tangentVecLen = mt3->tangentVecLen = pathEndP->tangentVecLen = tunnelP->depth * CORRIDOR_STDLENGTH; 
		pathStartP->length = 0.0;  
		mt1->length = M_PI / 2 * pathStartP->tangentVecLen; 
		mt2->length = mt1->length * 2;  
		mt3->length = mt1->length * 3; 
		pathEndP->length = mt1->length * 4; 
		mt1->pModifyToo = mt2;  
		mt2->pModifyToo = mt3;  
		mt3->pModifyToo = pathEndP; 
		pathEndP->pModifyToo = pathStartP;  
		pathStartP->pModifyToo = mt1; 
		pathEndP = pathStartP; 
		for (i = 0; i < 3; i++) { 
			for (j = 0; j < 3; j++) 
				pathEndP->orientation [j] = pathStartP->orientation [j]; 
			mt1->orientation [0] = mt2->orientation [0] = mt3->orientation [0] = pathStartP->orientation [0]; 
			mt1->orientation [1] = pathStartP->orientation [2]; 
			mt1->orientation [2] = -pathStartP->orientation [1]; 
			mt2->orientation [1] = -pathStartP->orientation [1]; 
			mt2->orientation [2] = -pathStartP->orientation [2]; 
			mt3->orientation [1] = -pathStartP->orientation [2]; 
			mt3->orientation [2] = pathStartP->orientation [1]; 
			mt1 = pathStartP->pos + (-pathStartP->orientation [1] + pathStartP->orientation [2]) * pathStartP->tangentVecLen * CIRCLE_FACTOR; 
			mt2 = pathStartP->pos - pathStartP->orientation [1] * pathStartP->tangentVecLen * 2 * CIRCLE_FACTOR; 
			mt3 = pathStartP->pos + (-pathStartP->orientation [1] - pathStartP->orientation [2]) * pathStartP->tangentVecLen * CIRCLE_FACTOR; 
			}
		break; 
	}
recalc_corridor (tunnelP);  plotlevel (); 
}

// -----------------------------------------------------------------------------  

void corr_depth (struct w_button * b)
{
	double new_d; 
	tTunnel *c = b->data; 

if (sscanf (b->item.str->str, " % g", &new_d) == 1 && new_d >= MIN_CUBEDEPTH && new_d <= MAX_CUBEDEPTH) 
	c->depth = new_d * 65536.0; 
sprintf (b->item.str->str, " % 10.2f", c->depth / 65536.0); 
w_drawbutton (b);  
recalc_corridor (b->data);  
plotlevel (); 
}
 
// -----------------------------------------------------------------------------  

int corr_win1_xpos = -1, corr_win1_ypos = -1;  
int corr_win2_xpos = -1, corr_win2_ypos = -1;  
//struct w_window init_corr_win = { 0, 400, 100, 150, 107, -1, -1, 0, NULL, 0, NULL, wb_drag|wb_close|wb_shrink, wr_normal, NULL, NULL, closerout_corr_win, NULL }; 

#define NUM_STDFORMS 3
//const char * stdforms_txt [NUM_STDFORMS] = { TXT_CUSTOM, TXT_STRAIGHT, TXT_CIRCLE }; 
#if 0
void init_corr_window (tTunnel * c, const char * levname)
{
	static struct w_b_press ok, cancel, ins, del, setend; 
	static struct w_b_string depth; 
	static struct w_b_choose stdform; 
	char * win_tit; 
	int xs; 

checkmem (win_tit = MALLOC (strlen (TXT_CORRWIN) + strlen (levname) + 1)); 
strcpy (win_tit, TXT_CORRWIN);  
strcat (win_tit, levname); 
init_corr_win.title = win_tit; 
if (view.whichdisplay) 
	if (corr_win2_xpos < 0) { 
		init_corr_win.xpos = 0;  
		init_corr_win.ypos = w_ymaxwinsize () - init_corr_win.ysize;  
		}
	else 
		init_corr_win.xpos = corr_win2_xpos;  init_corr_win.ypos = corr_win2_ypos;  
	}
else if (corr_win1_xpos < 0) { 
	init_corr_win.xpos = w_xmaxwinsize () - init_corr_win.xsize; 
	init_corr_win.ypos = 0;  
	}
else { 
	init_corr_win.xpos = corr_win1_xpos;  
	init_corr_win.ypos = corr_win1_ypos;  
	}
checkmem (c->win = w_openwindow (&init_corr_win)); 
xs = w_xwininsize (c->win); 
ok.delay = 0;  
ok.repeat = -1;  
ok.l_pressed_routine = ok.r_pressed_routine = NULL; 
ok.l_routine = ok.r_routine = corr_ok; 
checkmem (c->b_ok = w_addstdbutton (c->win, e_b_press, 0, 0, xs / 2, -1, TXT_OK, &ok, 1)); 
cancel = ok;  
cancel.l_routine = cancel.r_routine = corr_cancel; 
checkmem (c->b_cancel = w_addstdbutton (c->win, e_b_press, xs / 2, 0, xs - xs / 2, -1, TXT_CANCEL, &cancel, 1)); 
stdform.d_xsize = -1;  
stdform.num_options = NUM_STDFORMS; 
stdform.options = stdforms_txt;  
stdform.selected = 0; 
stdform.select_lroutine = stdform.select_rroutine = corr_form; 
checkmem (c->b_stdform = w_addstdbutton (c->win, e_b_choose, 0, c->b_ok->ysize, xs, -1, TXT_STDFORM, &stdform, 1)); 
depth.d_xsize = -1;  
depth.max_length = 20; 
checkmem (depth.str = MALLOC (depth.max_length + 1)); 
sprintf (depth.str, " % 10.2f", c->depth / 65536.0); 
depth.allowed_char = isfloat; 
depth.l_char_entered = depth.r_char_entered = NULL; 
depth.l_string_entered = depth.r_string_entered = corr_depth; 
checkmem (c->b_depth = w_addstdbutton (c->win, e_b_string, 0, c->b_stdform->ypos + c->b_stdform->ysize, xs, -1, TXT_CORRCUBEDEPTH, &depth, 1)); 
ins = ok;  
ins.l_routine = ins.r_routine = corr_ins_tp; 
checkmem (c->b_ins = w_addstdbutton (c->win, e_b_press, 0, c->b_depth->ypos + c->b_depth->ysize, xs / 2, -1, TXT_INSERT, &ins, 1)); 
del = ok;  
del.l_routine = del.r_routine = corr_del_tp; 
checkmem (c->b_del = w_addstdbutton (c->win, e_b_press, xs / 2, c->b_ins->ypos, xs - xs / 2, -1, TXT_DELETE, &del, 1)); 
setend = ok;  
setend.l_routine = setend.r_routine = corr_setend; 
checkmem (c->b_setend = w_addstdbutton (c->win, e_b_press, 0, c->b_del->ypos + c->b_del->ysize, xs, -1, TXT_CORRSETEND, &setend, 1)); 
c->b_depth->data = c;  
c->b_stdform->data = c; 
}
#endif
// -----------------------------------------------------------------------------  

void dec_makecorridor (int ec)
{
	int i, w; 
	CSegment*		segP; 
		pathStartP, * pathEndP; 
	//tNode * n;  
	CVertex ref_coords [2]; 

if (segP->HasChild (current->SideId ())) { 
	//printmsg (TXT_CUBETAGGEDON);  
	return;  
	}
//untagall (tt_segment);  
//tag (tt_wall, view.pCurrSegment, view.currWall); 

CTunnel tunnel (*current);
if (!tunnel.m_frontWalls.Create (1000))
	return;
tunnel.m_frontWalls.SetGrowth (1000);


if (!make_start_corridor (tunnelP)) { 
	FREE (tunnelP);  
	return;  
	}
initlist (&tunnelP->path); 
CTunnelPath* pathStartP = new CTunnelPath;
CTunnelPath* pathEndP = new CTunnelPath;

checkmem (pathStartP = MALLOC (sizeof (tTunnelPath))); 
checkmem (pathEndP = MALLOC (sizeof (tTunnelPath))); 
pathStartP->fixed = -1;  
pathStartP->pos = tunnelP->elements.Head ()->item.pCorrElem->center;  
pathEndP->fixed = 0;  
pathEndP->pos = tunnelP->elements.Tail ()->item.pCorrElem->center; 
addnode (&tunnelP->path, -1, pathStartP);  
addnode (&tunnelP->path, -1, pathEndP); 
for (i = 0; i < 3; i++) { 
	pathStartP->orientation [2] = pathEndP - pathStartP; 
	pathStartP->orientation [0] = tunnelP->elements.Head ()->item.pCorrElem->points [1][i] - tunnelP->elements.Head ()->item.pCorrElem->points [0][i];  
	}
pathEndP->length = LENGTH (&pathStartP->orientation [2]);  
normalize (&pathStartP->orientation [2]); 
pathEndP->orientation [2] = pathStartP->orientation [2];  
pathStartP->length = 0.0; 
pathStartP->tangentVecLen = pathEndP->tangentVecLen = pathEndP->length;  
normalize (&pathStartP->orientation [0]); 
CROSS (&pathStartP->orientation [1], &pathStartP->orientation [2], &pathStartP->orientation [0]); 
for (i = 0; i < 3; i++) 
	tunnelP->orientation [i] = pathEndP->orientation [i] = pathStartP->orientation [i];  
ref_coords [0] = pathStartP->orientation [0];  
ref_coords [1] = pathStartP->orientation [1]; 
pathStartP->oldTwist = pathEndP->oldTwist = 0.0; 
for (n = tunnelP->elements.Head (); n->succ != NULL; n = n->succ) 
	init_ce_relpoints (tunnelP, n->item.pCorrElem, pathStartP->orientation); 
init_corr_window (tunnelP, l->fullname); 
recalc_corridor (tunnelP);     
l->cur_corr = tunnelP;  
tunnelP->ld = l;  
plotlevel (); 
}

// -----------------------------------------------------------------------------  

void dec_makeedgecoplanar (int ec)
{
	tNode * n; 
	CVertex a, r, s, d, * p [4]; 
	double fr, fs; 
	int i, j; 

if (!l) { 
	printmsg (TXT_NOLEVEL);  
	return;  
	}
for (n = l->tagged [tt_edge].Head ()->succ; n != NULL; n = n->succ) {
	for (i = 0; i < 4; i++)
		if (i != (n->pred->no % 24) % 4 && testtag (tt_edge, n->pred->item.pNode, (n->pred->no % 24) / 4, i))
			break; 
	if (i == 4) {  // only one edge is tagged on this side. move it into the plane of the three others. 
		for (j = 0; j < 4; j++)
			p [j] = n->pred->item.pNode->item.segP->vertices [wallpts [(n->pred->no % 24) / 4][(( (n->pred->no % 24) % 4) + j) & 3]]->item.p; 
		a = * p [2]; 
		r = p [3] - a;  
		s = p [1] - a; 
		d = p [0] - a;  
		normalize (&r);  
		normalize (&s); 
		fr = DOT (&r, &d);  
		fs = DOT (&s, &d); 
		for (j = 0; j < 3; j++) 
			p [0][j] = fr * r.xyz [j] + fs * s.xyz [j] + a.xyz [j]; 
		untag (tt_edge, n->pred->item.pNode, (n->no % 24) / 4, (n->no % 24) % 4); 
		recalcwall (n->pred->item.pNode->item.segP, (n->no % 24) / 4); 
		}
	}
plotlevel ();  
drawopts (); 
}

#endif