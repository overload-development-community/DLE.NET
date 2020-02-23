// Copyright (c) 1997 Bryan Aamot
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <string.h>
#include "stdafx.h"
#include "dle-xp.h"
#include "dlcDoc.h"
#include "mineview.h"
#include "define.h"
#include "types.h"
#include "global.h"
#include "poly.h"
#include "cfile.h"
/*
extern HPEN hPenRed, hPenDkRed, hPenGreen, hPenDkGreen, hPenBlue, hPenCyan, hPenDkCyan, 
	 hPenMagenta, hPenYellow, hPenDkYellow, hPenOrange, hPenGold, hPenGray, 
	 hPenLtGray;
extern HRGN hrgnBackground, hrgnLowerBar, hrgnTopBar, hrgnAll;
*/
//-----------------------------------------------------------------------
// CONSTANTS
//-----------------------------------------------------------------------
#define OP_EOF        0	/* eof                                 */
#define OP_DEFPOINTS  1	/* defpoints                (not used) */
#define OP_FLATPOLY   2	/* flat-shaded polygon                 */
#define OP_TMAPPOLY   3	/* texture-mapped polygon              */
#define OP_SORTNORM   4	/* sort by normal                      */
#define OP_RODBM      5	/* rod bitmap               (not used) */
#define OP_SUBCALL    6	/* call a subobject                    */
#define OP_DEFP_START 7	/* defpoints with start                */
#define OP_GLOW       8	/* m_info.glow value for next poly            */

#define MAX_INTERP_COLORS 100
#define MAX_POINTS_PER_POLY 25

//-----------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------
#define W(p)   (*((short *) (p)))
#define WP(p)  ((short *) (p))
#define VP(p)  ((CFixVector*) (p))
#define calcNormal(a, b)
#define glNormal3fv(a)
#define glColor3ub(a, b, c)
#define glBegin(a)
#define glVertex3i(x, y, z)
#define glEnd()
#define glTexCoord2fv(a)

#define UINTW int

//-----------------------------------------------------------------------
// local prototypes
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// globals
//-----------------------------------------------------------------------
static CGameObject* renderObject;
static tModelRenderData modelRenderData;
static CVertex renderOffset;
static short glow_num = -1;
static CDoubleVector normal;	// Storage for calculated surface normal
static tModelRenderPoly* faceP;
static int pt, pt0, n_points;
static int lastObjectType = -1;
static int lastObjectId = -1;
static APOINT poly_xy [MAX_POLYMODEL_POINTS];

CPolyModel* renderModel;

//-----------------------------------------------------------------------
// int MultiplyFix ()
//-----------------------------------------------------------------------

int MultiplyFix (int a, int b) {
  return (int) ((double (a) * double (b))/F1_0);
}

//-----------------------------------------------------------------------
// DrawModel ()
//-----------------------------------------------------------------------

void CMineView::DrawModel () 
{
if (renderModel)
	renderModel->Draw (&m_view, m_pDC);
}

//-----------------------------------------------------------------------
// set_model_points ()
//
// Rotates, translates, then sets screen points (xy) for 3d model points
//-----------------------------------------------------------------------

void CPolyModel::SetModelPoints (int start, int end) 
{
	CVertex	pt;

for (int i = start; i < end; i++) {
	//int x0 = modelRenderData.points[i].v.x;
	//int y0 = modelRenderData.points[i].v.y;
	//int z0 = modelRenderData.points[i].v.z;

	// rotate point using Objects () rotation matrix
	pt = renderObject->m_location.orient * modelRenderData.points [i];
	// set point to be in world coordinates
	pt += renderObject->m_location.pos;
	// now that points are relative to set screen xy points (poly_xy)
	m_view->Project (pt, poly_xy [i]);
	}
}


//-----------------------------------------------------------------------
// draw_poly
//
// Action - Draws a polygon if it is facing the outward.
//          Used global device context handle set by SetupModel ()
//-----------------------------------------------------------------------

void tModelRenderPoly::Draw (CViewMatrix* view, CDC* pDC) 
{
if (view->CheckNormal (renderObject, offset, normal)) {
	  int i, j;

	POINT aPoints [MAX_POLYMODEL_POLY_POINTS];
	for (i = 0; i < n_verts; i++) {
		j = verts [i];
		aPoints [i].x = poly_xy [j].x;
		aPoints [i].y = poly_xy [j].y;
		}
	pDC->Polygon (aPoints, n_verts);
	}
}

//-----------------------------------------------------------------------
//
// Calls the CGameObject interpreter to render an CGameObject.
// The CGameObject renderer is really a seperate pipeline. returns true if drew
//
//-----------------------------------------------------------------------

void CPolyModel::Render (byte *p) 
{
while (W (p) != OP_EOF) {
	switch (W (p)) {
		// Point Definitions with Start Offset:
		// 2  ushort      n_points       number of points
		// 4  ushort      start_point    starting point
		// 6  ushort      unknown
		// 8  CFixVector  pts[n_points]  x, y, z data
		case OP_DEFP_START: {
			pt0 = W (p+4);
			n_points = W (p+2);
			modelRenderData.n_points += n_points;
			assert (W (p+6)==0);
			assert (modelRenderData.n_points < MAX_POLYMODEL_POINTS);
			assert (pt0+n_points < MAX_POLYMODEL_POINTS);
			for (pt = 0;pt < n_points; pt++) 
				modelRenderData.points [pt+pt0] = VP (p+8)[pt] + renderOffset;
			SetModelPoints (pt0, pt0+n_points);
			p += W (p+2)*sizeof (CFixVector) + 8;
			break;
			}
		// Flat Shaded Polygon:
		// 2  ushort     n_verts
		// 4  CFixVector vector1
		// 16 CFixVector vector2
		// 28 ushort     color
		// 30 ushort     verts[n_verts]
		case OP_FLATPOLY: {
			faceP = &modelRenderData.polys[modelRenderData.n_polys];
			faceP->n_verts = W (p+2);
			faceP->offset = *VP (p+4);
			faceP->normal = *VP (p+16);
			for (pt = 0; pt < faceP->n_verts; pt++) 
				faceP->verts[pt] = WP (p+30)[pt];
			faceP->Draw (m_view, m_pDC);
			p += 30 + ((faceP->n_verts&~1)+1)*2;
			break;
			}
		// Texture Mapped Polygon:
		// 2  ushort     n_verts
		// 4  CFixVector vector1
		// 16 CFixVector vector2
		// 28 ushort     nBaseTex
		// 30 ushort     verts[n_verts]
		// -- UVL        uvls[n_verts]
		case OP_TMAPPOLY: {
			faceP = &modelRenderData.polys[modelRenderData.n_polys];
			faceP->n_verts  = W (p+2);
			assert (faceP->n_verts>=MIN_POLYMODEL_POLY_POINTS);
			assert (faceP->n_verts<=MAX_POLYMODEL_POLY_POINTS);
			assert (modelRenderData.n_polys < MAX_POLYMODEL_POLYS);
			faceP->offset   = *VP (p+4);
			faceP->normal   = *VP (p+16);
			faceP->color    = -1;
			faceP->nBaseTex = W (p+28);
			faceP->glow_num = glow_num;
			for (pt = 0; pt < faceP->n_verts; pt++) 
				faceP->verts[pt] = WP (p+30)[pt];
			p += 30 + ((faceP->n_verts&~1)+1)*2;
			faceP->Draw (m_view, m_pDC);
			p += faceP->n_verts * 12;
			break;
			}
		// Sort by Normal
		// 2  ushort      unknown
		// 4  CFixVector  Front Model normal
		// 16 CFixVector  Back Model normal
		// 28 ushort      Front Model Offset
		// 30 ushort      Back Model Offset
		case OP_SORTNORM: {
			/* = W (p+2); */
			/* = W (p+4); */
			/* = W (p+16); */
			if (m_view->CheckNormal (renderObject, *VP (p+4), *VP (p+16))) {
			  Render (p + W (p+28));
			  Render (p + W (p+30));
				}
			else {
			  Render (p + W (p+30));
			  Render (p + W (p+28));
				}
			p += 32;
			break;
			}
		// Call a Sub Object
		// 2  ushort     n_anims
		// 4  CFixVector offset
		// 16 ushort     model offset
		case OP_SUBCALL: {
			renderOffset += *VP (p+4);
			Render (p + W (p+16));
			renderOffset -= *VP (p+4);
			p += 20;
			break;
			}
		// Glow Number for Next Poly
		// 2 UINTW  Glow_Value
		case OP_GLOW: {
			glow_num = W (p+2);
			p += 4;
			break;
			}
		default: 
			assert (0);
		}
	}
return;
}

//-----------------------------------------------------------------------
// ReadModelData ();
//-----------------------------------------------------------------------

#define FREAD(b)	fread (&b, sizeof (b), 1, file)

int CPolyModel::Read (CFileManager& fp, bool bRenderData) 
{
if (bRenderData) {
	Release ();
	if (!(m_info.renderData = new byte [m_info.dataSize]))
		return 0;
	return fp.Read (m_info.renderData, m_info.dataSize, 1) == 1;
	}
else {
	m_info.nModels = fp.ReadInt32 ();
	m_info.dataSize = fp.ReadInt32 ();
	fp.ReadInt32 ();
	m_info.renderData = null;
	for (int i = 0; i < MAX_SUBMODELS; i++)
		m_info.subModels [i].ptr = fp.ReadInt32 ();
	for (int i = 0; i < MAX_SUBMODELS; i++)
		fp.Read (m_info.subModels [i].offset);
	for (int i = 0; i < MAX_SUBMODELS; i++)
		fp.Read (m_info.subModels [i].norm);
	for (int i = 0; i < MAX_SUBMODELS; i++)
		fp.Read (m_info.subModels [i].pnt);
	for (int i = 0; i < MAX_SUBMODELS; i++)
		m_info.subModels [i].rad = fp.ReadInt32 ();
	for (int i = 0; i < MAX_SUBMODELS; i++)
		m_info.subModels [i].parent = (byte) fp.ReadSByte ();
	for (int i = 0; i < MAX_SUBMODELS; i++)
		fp.Read (m_info.subModels [i].vMin);
	for (int i = 0; i < MAX_SUBMODELS; i++)
		fp.Read (m_info.subModels [i].vMax);
	fp.Read (m_info.vMin);
	fp.Read (m_info.vMax);
	m_info.rad = fp.ReadInt32 ();
	m_info.textureCount = fp.ReadByte ();
	m_info.firstTexture = fp.ReadUInt16 ();
	m_info.simplerModel = fp.ReadByte ();
	}
return 1;
}

//-----------------------------------------------------------------------

int CMineView::ReadModelData (char* filename, char *szSubFile, bool bCustom) 
{
	CFileManager fp;

if (fp.Open (filename, "rb"))
	return 1;

	uint	id;
	uint	i, n;

if (bCustom) {
	struct level_header level;
	char data[3];
	long position;

	fp.Read (data, 3, 1); // verify signature "DHF"
	if (data[0] != 'D' || data[1] != 'H' || data[2] != 'F') {
		fp.Close ();
		return 1;
		}
	position = 3;
	while (!fp.EoF ()) {
		fp.Seek (position, SEEK_SET);
		if ((fp.Read (&level, sizeof (struct level_header), 1) != 1) ||
			 (level.size > 10000000L || level.size < 0)) {
				fp.Close ();
				return 1;
				}
		if (!strcmp (level.name, szSubFile)) {
			id = fp.ReadInt32 ();	  					   // read id
			if (id != 0x5848414DL) {
				fp.Close ();
				return 1;
				}
			fp.ReadUInt16 ();                              // read version
			n = fp.ReadInt16 ();                         // n_weapon_types
			fp.Seek (n * sizeof (WEAPON_INFO), SEEK_CUR);  // weapon_info
			n = fp.ReadInt16 ();                         // n_robot_types
			for (i = 0; i < n; i++)
				theMine->RobotInfo (N_ROBOT_TYPES_D2 + i)->Read (fp);
			n  = fp.ReadInt16 ();                         // n_robot_joints
			fp.Seek (n * sizeof (JOINTPOS), SEEK_CUR);     // robot_joints
			break;
			}
		position += sizeof (struct level_header) + level.size;
		}
	n = fp.ReadInt16 ();                          // n_curModels
	assert (n <= MAX_POLYGON_MODELS);
	for (i = 0; i < n; i++) 
		m_polyModels [N_POLYGON_MODELS_D2 + i].Read (fp);
	for (i = 0; i < n; i++) 
		m_polyModels [N_POLYGON_MODELS_D2 + i].Read (fp, true);
	}
else {
	id = fp.ReadInt32 ();	  					   // read id
	if (id != 0x214d4148L) {
		fp.Close ();
		return 1;
		}
	fp.ReadInt16 ();                              // read version
	n  = fp.ReadInt16 ();                         // n_tmap_info
	fp.Seek (n * sizeof (ushort), SEEK_CUR);	 // bitmap_indicies
	fp.Seek (n * sizeof (TMAP_INFO), SEEK_CUR); // tmap_info
	n = fp.ReadInt16 ();                          // n_sounds
	fp.Seek (n * sizeof (byte), SEEK_CUR);     // sounds
	fp.Seek (n * sizeof (byte), SEEK_CUR);     // alt_sounds
	n = fp.ReadInt16 ();                          // n_vclips
	fp.Seek (n * sizeof (VCLIP), SEEK_CUR);     // video clips
	n = fp.ReadInt16 ();                          // n_eclips
	fp.Seek (n * sizeof (ECLIP), SEEK_CUR);     // effect clips
	n = fp.ReadInt16 ();                          // n_wclips
	fp.Seek (n * sizeof (WCLIP), SEEK_CUR);     // weapon clips
	n = fp.Tell ();
	n = fp.ReadInt16 ();                          // n_robots
	for (i = 0; i < n; i++) 
		theMine->RobotInfo (i)->Read (fp);
	n = fp.ReadInt16 ();                          // n_robot_joints
	fp.Seek (n * sizeof (JOINTPOS), SEEK_CUR);     // robot joints
	n = fp.ReadInt16 ();                          // n_weapon
	fp.Seek (n * sizeof (WEAPON_INFO), SEEK_CUR);  // weapon info
	n = fp.ReadInt16 ();                          // n_powerups
	fp.Seek (n * sizeof (POWERUP_TYPE_INFO), SEEK_CUR); // powerup info
	n = fp.ReadInt16 ();                          // n_curModels
	assert (n <= MAX_POLYGON_MODELS);
	for (i = 0; i < n; i++) 
		m_polyModels [i].Read (fp);
	for (i = 0; i < n; i++) 
		m_polyModels [i].Read (fp, true);
	}
fp.Close ();
return 0;
}

//-----------------------------------------------------------------------

CPolyModel* CMineView::RenderModel (CGameObject* objP)
{
if ((theMine == null))
	return null;

	uint nModel;

switch (objP->m_info.type) {
	case OBJ_PLAYER:
	case OBJ_COOP:
		nModel = D2_PLAYER_CLIP_NUMBER;
		break;
	case OBJ_WEAPON:
		nModel = MINE_CLIP_NUMBER;
		break;
	case OBJ_CNTRLCEN:
		switch (objP->m_info.id) {
			case 1:  nModel = 95;  break;
			case 2:  nModel = 97;  break;
			case 3:  nModel = 99;  break;
			case 4:  nModel = 101; break;
			case 5:  nModel = 103; break;
			case 6:  nModel = 105; break;
			default: nModel = 97;  break; // level 1's reactor
		}
		break;
	case OBJ_ROBOT:
		nModel = theMine->RobotInfo ((objP->m_info.id >= N_ROBOT_TYPES_D2) ? objP->m_info.id /*- N_ROBOT_TYPES_D2*/ : objP->m_info.id)->m_info.nModel;
		break;
	default:
		return null;
	}
return m_polyModels + nModel;
}

//-----------------------------------------------------------------------
// SetupModel ()
//
// Action - sets the global handle used when drawing polygon models
//-----------------------------------------------------------------------

int CMineView::SetupModel (CGameObject *objP) 
{
renderOffset.Clear ();
modelRenderData.n_points = 0;
glow_num = -1;

if (null == (renderModel = RenderModel (renderObject = objP)))
	return 1;
if (renderModel->m_info.renderData)
	return 0;

  char filename[256];

strcpy_s (filename, sizeof (filename), descentPath [1]);
char *slash = strrchr (filename, '\\');
if (slash)
	*(slash+1) = '\0';
else
	filename[0] = '\0';

bool bCustom = (objP->m_info.type == OBJ_ROBOT) && (objP->m_info.id >= N_ROBOT_TYPES_D2);

if (bCustom) {
	char *psz = strstr (filename, "data");
	if (psz)
		*psz = '\0';
	}
strcat_s (filename, sizeof (filename), bCustom ? "missions\\d2x.hog" : "descent2.ham");
if (ReadModelData (filename, bCustom ? "d2x.ham" : "", bCustom))
	renderModel = null;
return renderModel == null;
}

//-----------------------------------------------------------------------

