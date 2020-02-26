// Copyright (c) 1997 Bryan Aamot

#include "stdafx.h"
#include "mine.h"
#include "dle-xp.h"
#include "ModelTextures.h"
#include "ASEModel.h"
#include "RenderModel.h"
#include "ModelManager.h"
#include "glew.h"

CModelManager modelManager;

int nDbgModel = -1;

//------------------------------------------------------------------------------
// MACROS
//------------------------------------------------------------------------------

#define W(p)   (*((ushort *) (p)))
#define WP(p)  ((ushort *) (p))
#define VP(p)  ((CFixVector*) (p))

#define calcNormal(a, b)
#define glNormal3fv(a)
#define glColor3ub(a, b, c)
#define glBegin(a)
#define glVertex3i(x, y, z)
#define glEnd()
#define glTexCoord2fv(a)

#define UINTW int

//------------------------------------------------------------------------------

#define PAL2RGBA(_c)	(((_c) >= 31) ? 1.0 : double (_c) / 31.0)

typedef struct tIntUVL {
	int u, v, l;
} tIntUVL;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

int CModelManager::Model (void)
{
if (theMine == null)
	return -1;

	uint nModel;

switch (m_object->m_info.type) {
	case OBJ_PLAYER:
	case OBJ_COOP:
		nModel = D2_PLAYER_CLIP_NUMBER;
		break;
	case OBJ_WEAPON:
		nModel = MINE_CLIP_NUMBER;
		break;
	case OBJ_REACTOR:
		if (DLE.IsD1File ())
			// D1 has one reactor model regardless of ID
			nModel = 93;
		else switch (m_object->m_info.id) {
			case 0:  nModel = 93;  break;
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
		nModel = robotManager.RobotInfo (m_object->m_info.id)->Info ().nModel;
		break;
	default:
		return -1;
	}
return nModel;
}

//------------------------------------------------------------------------------

int CModelManager::Setup (CGameObject *pObject, CRenderer* renderer, CDC* pDC) 
{
m_renderer = renderer;
m_viewMatrix = renderer->ViewMatrix ();
m_pDC = pDC;

m_offset.Clear ();
m_data.nPoints = 0;
m_nGlow = -1;
modelManager.m_object = pObject;
m_nModel = Model ();

if (m_nModel < 0)
	return 0;

int bVertigo = pObject->Id () > N_ROBOT_TYPES_D2;

if (m_polyModels [0][m_nModel].m_info.renderData || (!bVertigo && (m_polyModels [1][m_nModel].m_info.renderData || (m_renderModels [m_nModel].m_nModel >= 0))))
	return 1;

char filename [256];

strcpy_s (filename, sizeof (filename), descentFolder [1]);
char *slash = strrchr (filename, '\\');
if (slash)
	*(slash+1) = '\0';
else
	filename [0] = '\0';

int bCustom = robotManager.RobotInfo (pObject->Id ())->Info ().bCustom;

if (bCustom || bVertigo) {
	char *psz = strstr (filename, "data");
	if (psz)
		*psz = '\0';
	}
if (bVertigo) {
	strcat_s (filename, sizeof (filename), "missions\\d2x.hog");
	if (!ReadModelData (filename, "d2x.ham", true))
		return 0;
	}
else {
	strcat_s (filename, sizeof (filename), "descent2.ham");
	if (!ReadModelData (filename, "d2x.ham", bCustom != 0))
		return 0;
	//strcpy_s (d2xFilename, sizeof (d2xFilename), "..\\missions\\d2x.hog");
	//if (!ReadModelData (d2xFilename, "d2x.ham", true))
	//	return 0;
	*strstr (filename, "descent2.ham") = '\0';
	strcat_s (filename, sizeof (filename), "d2x-xl.hog");
	robotManager.ReadHXM (filename, "d2x-xl.hxm", false);
	robotManager.Reset ();
	}
return 1;
}

//------------------------------------------------------------------------------

void CModelManager::Reset (void)
{
CPolyModel* pModel = &m_polyModels [1][0];
for (int i = 0; i < MAX_POLYGON_MODELS; i++, pModel++) {
	m_renderModels [i].Destroy ();
	m_aseModels [i].Destroy ();
	m_oofModels [i].Destroy ();
	pModel->Release ();
	pModel->Clear ();
	}
}

//------------------------------------------------------------------------------

int CModelManager::ReadModelData (char* filename, char *szSubFile, bool bCustom) 
{
	CFileManager fp;

if (!fp.Open (filename, "rb"))
	return 0;

	uint	id;
	uint	i, n;

if (bCustom) {
	if (0 > hogManager->FindSubFile (fp, filename, szSubFile, null)) {
		fp.Close ();
		return 0;
		}

	id = fp.ReadInt32 ();	  					   
	if (id != 0x5848414DL) {
		fp.Close ();
		return 0;
		}
	fp.ReadInt32 ();											
	n = fp.ReadInt32 ();										
	fp.Seek (n * sizeof (WEAPON_INFO), SEEK_CUR);	
	n = fp.ReadInt32 ();										
	for (i = 0; i < n; i++)
		robotManager.RobotInfo (N_ROBOT_TYPES_D2 + i)->Read (&fp);
	n = fp.ReadInt32 ();                         
	fp.Seek (n * sizeof (JOINTPOS), SEEK_CUR);     
	n = fp.ReadInt32 ();                          
	for (i = 0; i < n; i++) 
		m_polyModels [0][N_POLYGON_MODELS_D2 + i].Read (&fp);
	for (i = 0; i < n; i++) 
		m_polyModels [0][N_POLYGON_MODELS_D2 + i].Read (&fp, true);
	if (strstr (szSubFile, ".ham")) {
		fp.Seek (n * sizeof (int) * 2, SEEK_CUR); // skip dying and dead model numbers
		n = fp.ReadInt32 ();                          
		fp.Read (&m_textureIndex [0][m_nTextures], sizeof (ushort), n);
		memcpy (&m_textureIndex [1][m_nTextures], &m_textureIndex [0][m_nTextures], sizeof (ushort) * n);
		m_nTextures += n;
		n = fp.ReadInt32 ();                          
		fp.Read (&m_modelTextureIndex [0][m_nIndices], sizeof (ushort), n);
		memcpy (&m_modelTextureIndex [1][m_nIndices], &m_modelTextureIndex [0][m_nIndices], sizeof (ushort) * n);
		m_nIndices += n;
		}
	}
else {
	id = fp.ReadInt32 ();	  					   
	if (id != 0x214d4148L) {
		fp.Close ();
		return 0;
		}
	fp.ReadInt32 ();                              
	n = fp.ReadUInt32 ();                         
	fp.Seek (n * sizeof (ushort), SEEK_CUR);	 
	fp.Seek (n * sizeof (tTextureEffectInfo), SEEK_CUR); 
	n = fp.ReadUInt32 ();                          
	fp.Seek (n * sizeof (ubyte), SEEK_CUR);     
	fp.Seek (n * sizeof (ubyte), SEEK_CUR);     
	n = fp.ReadUInt32 ();                          
#if 1
	fp.Read (m_animations, sizeof (tAnimationInfo), n);
#else
	fp.Seek (n * sizeof (tAnimationInfo), SEEK_CUR);     
#endif
	n = fp.ReadUInt32 ();                          
	fp.Seek (n * sizeof (tEffectInfo), SEEK_CUR);     
	n = fp.ReadUInt32 ();                          
	fp.Seek (n * sizeof (tWallEffectInfo), SEEK_CUR);    
	n = fp.ReadUInt32 ();                          
	for (i = 0; i < n; i++) 
		robotManager.RobotInfo (i)->Read (&fp);
	n = fp.ReadUInt32 ();                         
	fp.Seek (n * sizeof (JOINTPOS), SEEK_CUR);     
	n = fp.ReadUInt32 ();                         
	fp.Seek (n * sizeof (WEAPON_INFO), SEEK_CUR);  
	n = fp.ReadUInt32 ();                         
	fp.Seek (n * sizeof (POWERUP_TYPE_INFO), SEEK_CUR); 
	n = fp.ReadUInt32 ();                          
	for (i = 0; i < n; i++) 
		m_polyModels [0][i].Read (&fp);
	for (i = 0; i < n; i++) 
		m_polyModels [0][i].Read (&fp, true);
	fp.Seek (2 * n * sizeof (int), SEEK_CUR);
	n = fp.ReadUInt32 ();                          
	fp.Seek (2 * n * sizeof (short), SEEK_CUR);
	n = fp.ReadUInt32 ();                         
	fp.Read (m_textureIndex [0], sizeof (ushort), n);
	fp.Read (m_modelTextureIndex [0], sizeof (ushort), n);
	for (m_nTextures = n; !m_textureIndex [0][m_nTextures - 1]; m_nTextures--)
		;
	for (m_nIndices = n; !m_modelTextureIndex [0][m_nIndices - 1]; m_nIndices--)
		;
	}
fp.Close ();
return 1;
}

//------------------------------------------------------------------------------

void CModelManager::ReadCustomModelData (CMemoryFile& mf, int bCustom)
{
uint n, i, j;

if (bCustom) {
	memcpy (m_textureIndex [1], m_textureIndex [0], sizeof (m_textureIndex [1]));
	memcpy (m_modelTextureIndex [1], m_modelTextureIndex [0], sizeof (m_modelTextureIndex [1]));
	}
n = mf.ReadUInt32 ();
mf.Seek (n * (sizeof (int) + sizeof (JOINTPOS)), SEEK_CUR);
n = mf.ReadUInt32 ();
for (j = 0; j < n; j++) {
	i = mf.ReadUInt32 ();
#ifdef _DEBUG
	if (i == nDbgModel)
		nDbgModel = nDbgModel;
#endif
	m_polyModels [bCustom][i].Release ();
	m_polyModels [bCustom][i].Clear ();
	m_polyModels [bCustom][i].Read (&mf);
	m_polyModels [bCustom][i].Read (&mf, true);
	m_polyModels [bCustom][i].m_bCustom = bCustom;
	mf.Seek (2 * sizeof (int), SEEK_CUR);
	}
if (bCustom) {
	n = mf.ReadUInt32 ();
	for (j = 0; j < n; j++) {
		i = mf.ReadInt32 ();
		m_textureIndex [1][i] = mf.ReadUInt16 ();
		}
	n = mf.ReadUInt32 ();
	for (j = 0; j < n; j++) {
		i = mf.ReadInt32 ();
		m_modelTextureIndex [1][i] = mf.ReadUInt16 ();
		}
	}
}

//------------------------------------------------------------------------------

void CModelManager::ReadCustomModelData (ubyte* buffer, long bufSize, int bCustom)
{
CMemoryFile mf;
if (mf.Open (buffer, bufSize))
	ReadCustomModelData (mf, bCustom);
}

//------------------------------------------------------------------------------

void CModelManager::ReadMod (char* pszFolder)
{
for (int i = 0; i < MAX_POLYGON_MODELS; i++) {
	DLE.MainFrame ()->Progress ().StepIt ();
#ifdef _DEBUG
	if (i == nDbgModel)
		nDbgModel = nDbgModel;
#endif
	if (m_polyModels [1][i].m_info.renderData || (m_aseModels [i].m_nModel >= 0) || (m_oofModels [i].m_nModel >= 0))
		continue; // already have a custom model
	if (m_aseModels [i].Read (pszFolder, i))
		m_renderModels [i].BuildFromASE (m_aseModels [i]);
	else if (m_oofModels [i].Read (pszFolder, i))
		m_renderModels [i].BuildFromOOF (m_oofModels [i]);
	}
}

//------------------------------------------------------------------------------

void CModelManager::LoadMod (void)
{
if (DLE.MakeModFolders ("models")) {
	DLE.MainFrame ()->InitProgress (2 * MAX_POLYGON_MODELS);
	char szFile [256];
	sprintf (szFile, "%s.hxm", DLE.m_modFolders [2]);
	robotManager.ReadHXM (szFile);
	// first read the level specific textures
	ReadMod (DLE.m_modFolders [0]);
	// then read the mission wide textures
	ReadMod (DLE.m_modFolders [1]);
	DLE.MainFrame ()->Progress ().DestroyWindow ();
	}
}

//------------------------------------------------------------------------------
// set_model_points ()
//
// Rotates, translates, then sets screen points (xy) for 3d model points
//------------------------------------------------------------------------------

void CModelManager::SetPoints (int start, int end) 
{
for (int i = start; i < end; i++) {
	CVertex& v = m_screenPoly [i];
	// rotate point using Objects () rotation matrix
	m_object->Transform (v, m_data.points [i]);
	// now that points are relative to set screen xy points (modelManager.m_screenPoly)
	v.Transform (m_viewMatrix);
	v.Project (m_viewMatrix);
	if (m_screenRect [0].x > v.m_screen.x)
		m_screenRect [0].x = v.m_screen.x;
	if (m_screenRect [0].y > v.m_screen.y)
		m_screenRect [0].y = v.m_screen.y;
	if (m_screenRect [1].x < v.m_screen.x)
		m_screenRect [1].x = v.m_screen.x;
	if (m_screenRect [1].y < v.m_screen.y)
		m_screenRect [1].y = v.m_screen.y;
	}
}

//------------------------------------------------------------------------------

void CModelManager::Polygon (ushort* index, int nVertices, tTexCoord2d* texCoords, rgbColord* color, short nTexture)
{ 
if (m_renderer->Type ()) {
	m_renderer->TexturedPolygon (nTexture, texCoords, color, m_screenPoly, nVertices, index);
	}
else {
	CPoint points [MAX_POLYMODEL_POLY_POINTS];
	for (int i = 0; i < nVertices; i++) {
		int j = index [i];
		points [i].x = m_screenPoly [j].m_screen.x;
		points [i].y = m_screenPoly [j].m_screen.y;
		}
	m_renderer->Polygon (points, nVertices); 
	}
}

//------------------------------------------------------------------------------

void CModelManager::Draw (void) 
{ 
if (m_nModel >= 0) {
#ifdef _DEBUG
	if (m_nModel == nDbgModel)
		nDbgModel = nDbgModel;
#endif
	m_screenRect [0].x = m_screenRect [0].y = 0x7FFFFFFF;
	m_screenRect [1].x = m_screenRect [1].y = -0x7FFFFFFF;
	if (m_renderer->Type ()) {
		m_renderer->BeginRender ();

		if (m_polyModels [1][m_nModel].Draw (m_viewMatrix, 0))
			;
		else if (m_renderModels [m_nModel].Render (m_viewMatrix, m_object))
			;
		else
			m_polyModels [0][m_nModel].Draw (m_viewMatrix, 0);
		m_renderer->EndRender ();
		}
	else {
		for (int nStage = 0; nStage < 2; nStage++) {
			m_renderer->BeginRender (nStage > 0);
			m_polyModels [0][m_nModel].Draw (m_viewMatrix, nStage ? 1 : -1);
			m_renderer->EndRender ();
			}
		}
	}
}

//------------------------------------------------------------------------------

void CModelRenderPoly::Draw (void) 
{
//if (modelManager.CheckNormal (offset, normal))
	modelManager.Polygon (index, nVerts, texCoords, &color, nTexture);
}

//------------------------------------------------------------------------------

void CModelManager::Render (ubyte *p, int nStage, int bCustom, ushort firstTexture)
{
	int pt, pt0;

while (W (p) != OP_EOF) {
	switch (W (p)) {
		// Point Definitions with Start Offset:
		// 2  ushort      modelManager.m_nPolyPoints       number of points
		// 4  ushort      start_point    starting point
		// 6  ushort      unknown
		// 8  CFixVector  pts[modelManager.m_nPolyPoints]  x, y, z data
		case OP_DEFP_START: {
			pt0 = W (p+4);
			if (nStage <= 0) {
				modelManager.m_nPolyPoints = W (p+2);
				modelManager.m_data.nPoints += modelManager.m_nPolyPoints;
				for (pt = 0; pt < modelManager.m_nPolyPoints; pt++) 
					modelManager.m_data.points [pt+pt0] = modelManager.m_offset + CDoubleVector (VP (p+8)[pt]);
				modelManager.SetPoints (pt0, pt0 + modelManager.m_nPolyPoints);
				}
			p += W (p+2)*sizeof (CFixVector) + 8;
			break;
			}
		// Flat Shaded Polygon:
		// 2  ushort     nVerts
		// 4  CFixVector vector1
		// 16 CFixVector vector2
		// 28 ushort     color
		// 30 ushort     verts[nVerts]
		case OP_FLATPOLY: {
			modelManager.m_face = &modelManager.m_data.polys [modelManager.m_data.nPolys];
			modelManager.m_face->nVerts = W (p+2);
			if (nStage >= 0) {
				modelManager.m_face->offset = *VP (p+4);
				modelManager.m_face->normal = *VP (p+16);
				ushort c = W (p+28);
				modelManager.m_face->color.r = PAL2RGBA ((c >> 10) & 31);
				modelManager.m_face->color.g = PAL2RGBA ((c >> 5) & 31);
				modelManager.m_face->color.b = PAL2RGBA (c & 31);
				modelManager.m_face->nTexture = 0;
				p += 30;
				for (pt = 0; pt < modelManager.m_face->nVerts; pt++) 
					modelManager.m_face->index [pt] = WP (p)[pt];
				modelManager.m_face->Draw ();
				}
			else
				p += 30;
			p += (modelManager.m_face->nVerts | 1) * 2;
			break;
			}
		// Texture Mapped Polygon:
		// 2  ushort     nVerts
		// 4  CFixVector vector1
		// 16 CFixVector vector2
		// 28 ushort     nBaseTex
		// 30 ushort     verts[nVerts]
		// -- UVL        uvls[nVerts]
		case OP_TMAPPOLY: {
			modelManager.m_face = &modelManager.m_data.polys [modelManager.m_data.nPolys];
			modelManager.m_face->nVerts = W (p+2);
			if (nStage >= 0) {
				modelManager.m_face->offset = *VP (p+4);
				modelManager.m_face->normal = *VP (p+16);
				modelManager.m_face->color.r = 
				modelManager.m_face->color.g = 
				modelManager.m_face->color.b = 1.0;
				modelManager.m_face->nTexture = -modelManager.m_textureIndex [bCustom][modelManager.m_modelTextureIndex [bCustom][firstTexture + W (p+28)]]; // texture index is based on 1!
				modelManager.m_face->nGlow = modelManager.m_nGlow;
				p += 30;
				tIntUVL* uvls = (tIntUVL*) (p + (modelManager.m_face->nVerts | 1) * 2);
				for (pt = 0; pt < modelManager.m_face->nVerts; pt++) {
					modelManager.m_face->texCoords [pt].u = X2D (uvls [pt].u);
					modelManager.m_face->texCoords [pt].v = -X2D (uvls [pt].v);
					modelManager.m_face->index [pt] = WP (p)[pt];
					}
				modelManager.m_face->Draw ();
				}
			else
				p += 30;
			p += (modelManager.m_face->nVerts | 1) * 2;
			p += modelManager.m_face->nVerts * 12;
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
			if (modelManager.CheckNormal (*VP (p+4), *VP (p+16))) {
			  Render (p + W (p+28), nStage, bCustom, firstTexture);
			  Render (p + W (p+30), nStage, bCustom, firstTexture);
				}
			else {
			  Render (p + W (p+30), nStage, bCustom, firstTexture);
			  Render (p + W (p+28), nStage, bCustom, firstTexture);
				}
			p += 32;
			break;
			}
		// Call a Sub Object
		// 2  ushort     n_anims
		// 4  CFixVector offset
		// 16 ushort     model offset
		case OP_SUBCALL: {
			modelManager.m_offset += *VP (p+4);
			Render (p + W (p+16), nStage, bCustom, firstTexture);
			modelManager.m_offset -= *VP (p+4);
			p += 20;
			break;
			}
		// Glow Number for Next Poly
		// 2 UINTW  Glow_Value
		case OP_GLOW: {
			modelManager.m_nGlow = W (p+2);
			p += 4;
			break;
			}
		default: 
			assert (0);
		}
	}
return;
}

//------------------------------------------------------------------------------
//eof ModelManager.cpp

