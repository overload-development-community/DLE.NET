#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include  "mine.h"
#include  "dle-xp.h"
#include  "Renderer.h"
#include "ASEModel.h"

static char	szLine [1024];
static char	szLineBackup [1024];
static int nLine = 0;
static CFileManager *aseFile = null;
static char *pszToken = null;
static int bErrMsg = 0;

#define ASE_ROTATE_MODEL	1
#define ASE_FLIP_TEXCOORD	1

using namespace ASE;

#define MODEL_DATA_VERSION 1010	//must start with something bigger than the biggest model number

extern int nDbgModel;

#define USE_BINARY_MODELS 0

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

int CModel::Error (const char *pszMsg)
{
if (!bErrMsg) {
	char szMsg [356];
	if (pszMsg)
		sprintf (szMsg, "%s: error in line %d (%s)\n", aseFile->Name (), nLine, pszMsg);
	else
		sprintf (szMsg, "%s: error in line %d\n", aseFile->Name (), nLine);
	INFOMSG (szMsg);
	bErrMsg = 1;
	}
return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

static float FloatTok (const char *delims)
{
pszToken = strtok (null, delims);
if (!(pszToken && *pszToken))
	CModel::Error ("missing data");
return pszToken ? (float) atof (pszToken) : 0;
}

//------------------------------------------------------------------------------

static int IntTok (const char *delims)
{
pszToken = strtok (null, delims);
if (!(pszToken && *pszToken))
	CModel::Error ("missing data");
return pszToken ? atoi (pszToken) : 0;
}

//------------------------------------------------------------------------------

static char CharTok (const char *delims)
{
pszToken = strtok (null, delims);
if (!(pszToken && *pszToken))
	CModel::Error ("missing data");
return pszToken ? *pszToken : '\0';
}

//------------------------------------------------------------------------------

static char szEmpty [1] = "";

static char *StrTok (const char *delims)
{
pszToken = strtok (null, delims);
if (!(pszToken && *pszToken))
	CModel::Error ("missing data");
return pszToken ? pszToken : szEmpty;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

static void ReadVector (CFileManager& fp, CDoubleVector *pv)
{
#if ASE_ROTATE_MODEL
	pv->v.x = FloatTok (" \t");
	pv->v.z = FloatTok (" \t");
	pv->v.y = FloatTok (" \t");
#else	// need to rotate model for Descent
	int	i;

for (i = 0; i < 3; i++)
	pv [i] = FloatTok (" \t");
#endif
}

//------------------------------------------------------------------------------

static char* ReadLine (CFileManager& fp)
{
while (!fp.EoF ()) {
	fp.GetS (szLine, sizeof (szLine));
	nLine++;
	strcpy (szLineBackup, szLine);
	_strupr (szLine);
	if ((pszToken = strtok (szLine, " \t")))
		return pszToken;
	}
return null;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CSubModel::Init (void)
{
m_next = null;
memset (m_szName, 0, sizeof (m_szName));
memset (m_szParent, 0, sizeof (m_szParent));
m_nSubModel = 0;
m_nParent = 0;
m_nTexture = 0;
m_nFaces = 0;
m_nVerts = 0;
m_nTexCoord = 0;
m_nIndex = 0;
m_bRender = 1;
m_bGlow = 0;
m_bFlare = 0;
m_bBillboard = 0;
m_bThruster = 0;
m_bWeapon = 0;
m_bHeadlight = 0;
m_bBombMount = 0;
m_nGun = -1;
m_nBomb = -1;
m_nMissile = -1;
m_nType = 0;
m_nWeaponPos = 0;
m_nGunPoint = -1;
m_nBullets = -1;
m_bBarrel = 0;
m_vOffset.Clear ();
}

//------------------------------------------------------------------------------

void CSubModel::Destroy (void)
{
m_faces.Destroy ();
m_vertices.Destroy ();
m_texCoord.Destroy ();
Init ();
}

//------------------------------------------------------------------------------

int CSubModel::ReadNode (CFileManager& fp)
{
if (CharTok (" \t") != '{')
	return CModel::Error ("syntax error");
while ((pszToken = ReadLine (fp))) {
	if (*pszToken == '}')
		return 1;
	if (!strcmp (pszToken, "*TM_POS")) {
		m_vOffset.Clear (); //FloatTok (" \t");
		}
	}
return CModel::Error ("unexpected end of file");
}

//------------------------------------------------------------------------------

int CSubModel::ReadMeshVertexList (CFileManager& fp)
{
	CVertex*	pv;
	int		i;

if (CharTok (" \t") != '{')
	return CModel::Error ("syntax error");
while ((pszToken = ReadLine (fp))) {
	if (*pszToken == '}') {
		if (m_bBillboard) {
			m_vOffset /= m_nVerts;
			for (i = 0; i < m_nVerts; i++)
				m_vertices [i].m_vertex -= m_vOffset;
			}
		return 1;
		}
	if (!strcmp (pszToken, "*MESH_VERTEX")) {
		if (!m_vertices)
			return CModel::Error ("no vertices found");
		i = IntTok (" \t");
		if ((i < 0) || (i >= m_nVerts))
			return CModel::Error ("invalid vertex number");
		pv = m_vertices + i;
		ReadVector (fp, &pv->m_vertex);
		if (m_bBillboard)
			m_vOffset += pv->m_vertex;
		//pv->m_vertex -= m_vOffset;
		}
	}
return CModel::Error ("unexpected end of file");
}

//------------------------------------------------------------------------------

int CSubModel::ReadMeshFaceList (CFileManager& fp)
{
	CFace	*pf;
	int	i;

if (CharTok (" \t") != '{')
	return CModel::Error ("syntax error");
while ((pszToken = ReadLine (fp))) {
	if (*pszToken == '}')
		return 1;
	if (!strcmp (pszToken, "*MESH_FACE")) {
		if (!m_faces)
			return CModel::Error ("no faces found");
		i = IntTok (" \t");
		if ((i < 0) || (i >= m_nFaces))
			return CModel::Error ("invalid face number");
		pf = m_faces + i;
		for (i = 0; i < 3; i++) {
			strtok (null, " :\t");
			pf->m_nVerts [i] = IntTok (" :\t");
			}
		do {
			pszToken = StrTok (" :\t");
			if (!*pszToken)
				return CModel::Error ("unexpected end of file");
			} while (strcmp (pszToken, "*MESH_MTLID"));
		pf->m_nTexture = IntTok (" ");
		}
	}
return CModel::Error ("unexpected end of file");
}

//------------------------------------------------------------------------------

int CSubModel::ReadVertexTexCoord (CFileManager& fp)
{
if (CharTok (" \t") != '{')
	return CModel::Error ("syntax error");
while ((pszToken = ReadLine (fp))) {
	if (*pszToken == '}')
		return 1;
	if (!strcmp (pszToken, "*MESH_TVERT")) {
		if (!m_texCoord)
			return CModel::Error ("no texture coordinates found");
		int i = IntTok (" \t");
		if ((i < 0) || (i >= m_nTexCoord))
			return CModel::Error ("invalid texture coordinate number");
		tTexCoord2d* pt = m_texCoord + i;
#if ASE_FLIP_TEXCOORD
		pt->u = FloatTok (" \t");
		pt->v = -FloatTok (" \t");
#else
		for (i = 0; i < 2; i++)
			pt->vec [i] = FloatTok (" \t");
#endif
		}
	}
return CModel::Error ("unexpected end of file");
}

//------------------------------------------------------------------------------

int CSubModel::ReadFaceTexCoord (CFileManager& fp)
{
if (CharTok (" \t") != '{')
	return CModel::Error ("syntax error");
while ((pszToken = ReadLine (fp))) {
	if (*pszToken == '}')
		return 1;
	if (!strcmp (pszToken, "*MESH_TFACE")) {
		if (!m_faces)
			return CModel::Error ("no faces found");
		int i = IntTok (" \t");
		if ((i < 0) || (i >= m_nFaces))
			return CModel::Error ("invalid face number");
		CFace* pf = m_faces + i;
		for (i = 0; i < 3; i++)
			pf->m_nTexCoord [i] = IntTok (" \t");
		}
	}
return CModel::Error ("unexpected end of file");
}

//------------------------------------------------------------------------------

int CSubModel::ReadMeshNormals (CFileManager& fp)
{
	CFace*	pf;
	CVertex*	pv;
	int		i;

if (CharTok (" \t") != '{')
	return CModel::Error ("syntax error");
while ((pszToken = ReadLine (fp))) {
	if (*pszToken == '}')
		return 1;
	if (!strcmp (pszToken, "*MESH_FACENORMAL")) {
		if (!m_faces)
			return CModel::Error ("no faces found");
		i = IntTok (" \t");
		if ((i < 0) || (i >= m_nFaces))
			return CModel::Error ("invalid face number");
		pf = m_faces + i;
		ReadVector (fp, &pf->m_vNormal);
		}
	else if (!strcmp (pszToken, "*MESH_VERTEXNORMAL")) {
		if (!m_vertices)
			return CModel::Error ("no vertices found");
		i = IntTok (" \t");
		if ((i < 0) || (i >= m_nVerts))
			return CModel::Error ("invalid vertex number");
		pv = m_vertices + i;
		ReadVector (fp, &pv->m_normal);
		}
	}
return CModel::Error ("unexpected end of file");
}

//------------------------------------------------------------------------------

int CSubModel::ReadMesh (CFileManager& fp, int& nFaces, int& nVerts)
{
if (CharTok (" \t") != '{')
	return CModel::Error ("syntax error");
while ((pszToken = ReadLine (fp))) {
	if (*pszToken == '}')
		return 1;
	if (!strcmp (pszToken, "*MESH_NUMVERTEX")) {
		if (m_vertices.Buffer ())
			return CModel::Error ("duplicate vertex list");
		m_nVerts = IntTok (" \t");
		if (!m_nVerts)
			return CModel::Error ("no vertices found");
		nVerts += m_nVerts;
		if (!(m_vertices.Create (m_nVerts)))
			return CModel::Error ("out of memory");
		m_vertices.Clear ();
		}
	else if (!strcmp (pszToken, "*MESH_NUMTVERTEX")) {
		if (m_texCoord.Buffer ())
			return CModel::Error ("no texture coordinates found");
		m_nTexCoord = IntTok (" \t");
		if (m_nTexCoord) {
			if (!(m_texCoord.Create (m_nTexCoord)))
				return CModel::Error ("out of memory");
			}
		}
	else if (!strcmp (pszToken, "*MESH_NUMFACES")) {
		if (m_faces.Buffer ())
			return CModel::Error ("no faces found");
		m_nFaces = IntTok (" \t");
		if (!m_nFaces)
			return CModel::Error ("no faces specified");
		nFaces += m_nFaces;
		if (!(m_faces.Create (m_nFaces)))
			return CModel::Error ("out of memory");
		m_faces.Clear ();
		}
	else if (!strcmp (pszToken, "*MESH_VERTEX_LIST")) {
		if (!ReadMeshVertexList (fp))
			return CModel::Error (null);
		}
	else if (!strcmp (pszToken, "*MESH_FACE_LIST")) {
		if (!ReadMeshFaceList (fp))
			return CModel::Error (null);
		}
	else if (!strcmp (pszToken, "*MESH_NORMALS")) {
		if (!ReadMeshNormals (fp))
			return CModel::Error (null);
		}
	else if (!strcmp (pszToken, "*MESH_TVERTLIST")) {
		if (!ReadVertexTexCoord (fp))
			return CModel::Error (null);
		}
	else if (!strcmp (pszToken, "*MESH_TFACELIST")) {
		if (!ReadFaceTexCoord (fp))
			return CModel::Error (null);
		}
	}
return CModel::Error ("unexpected end of file");
}

//------------------------------------------------------------------------------

int CSubModel::Read (CFileManager& fp, int& nFaces, int& nVerts)
{
while ((pszToken = ReadLine (fp))) {
	if (*pszToken == '}')
		return 1;
	if (!strcmp (pszToken, "*NODE_NAME")) {
		strcpy (m_szName, StrTok (" \t\""));
		if (strstr (m_szName, "GLOW") != null)
			m_bGlow = 1;
		if (strstr (m_szName, "FLARE") != null) {
			m_bGlow =
			m_bFlare = 
			m_bBillboard = 1;
			}
		if (strstr (m_szName, "$GUNPNT"))
			m_nGunPoint = atoi (m_szName + 8);
		if (strstr (m_szName, "$BOMBMOUNT"))
			m_bBombMount = 1;
		else if (strstr (m_szName, "$BULLETS"))
			m_nBullets = 1;
		else if (strstr (m_szName, "$DUMMY") != null)
			m_bRender = 0;
		else if (strstr (m_szName, "$THRUSTER-") != null) {
			if (m_szName [10] == 'R') // rear
				m_bThruster |= REAR_THRUSTER;
			else if (m_szName [10] == 'F') // front
				m_bThruster |= FRONT_THRUSTER;
			if (m_szName [11] == 'L') // left
				m_bThruster |= LEFT_THRUSTER;
			else if (m_szName [11] == 'R') // right
				m_bThruster |= RIGHT_THRUSTER;
			if (m_szName [12] == 'T') // top
				m_bThruster |= TOP_THRUSTER;
			else if (m_szName [12] == 'B') // bottom
				m_bThruster |= BOTTOM_THRUSTER;
			if (!m_bThruster) // stay compatible with older models
				m_bThruster = REAR_THRUSTER;
			if (m_bThruster < 3)
				m_bThruster |= FRONTAL_THRUSTER;
			else
				m_bThruster |= LATERAL_THRUSTER;
			}
		else if (strstr (m_szName, "$THRUSTER") != null) 
			m_bThruster = REAR_THRUSTER;
		else if (strstr (m_szName, "$WINGTIP") != null) {
			m_bWeapon = 1;
			m_nGun = 0;
			m_nBomb =
			m_nMissile = -1;
			m_nType = atoi (m_szName + 8) + 1;
			}
		else if (strstr (m_szName, "$GUN") != null) {
			m_bWeapon = 1;
			m_nGun = atoi (m_szName + 4) + 1;
			m_nWeaponPos = atoi (m_szName + 6) + 1;
			m_nBomb =
			m_nMissile = -1;
			}
		else if (strstr (m_szName, "$BARREL") != null) {
			m_bWeapon = 1;
			m_nGun = atoi (m_szName + 7) + 1;
			m_nWeaponPos = atoi (m_szName + 9) + 1;
			m_nBomb =
			m_nMissile = -1;
			m_bBarrel = 1;
			}
		else if (strstr (m_szName, "$MISSILE") != null) {
			m_bWeapon = 1;
			m_nMissile = atoi (m_szName + 8) + 1;
			m_nWeaponPos = atoi (m_szName + 10) + 1;
			m_nGun =
			m_nBomb = -1;
			}
		else if (strstr (m_szName, "$BOMB") != null) {
			m_bWeapon = 1;
			m_nBomb = atoi (m_szName + 6) + 1;
			m_nGun =
			m_nMissile = -1;
			}
		else if (strstr (m_szName, "$LIGHTBEAM") != null) {
			m_bHeadlight = 1;
			m_bGlow = 1;
			m_bFlare = 0;
			}
		}
	else if (!strcmp (pszToken, "*NODE_PARENT")) {
		strcpy (m_szParent, StrTok (" \t\""));
		}
	if (!strcmp (pszToken, "*NODE_TM")) {
		if (!ReadNode (fp))
			return CModel::Error (null);
		}
	else if (!strcmp (pszToken, "*MESH")) {
		if (!ReadMesh (fp, nFaces, nVerts))
			return CModel::Error (null);
		}
	else if (!strcmp (pszToken, "*MATERIAL_REF")) {
		m_nTexture = IntTok (" \t");
		}
	}
return CModel::Error ("unexpected end of file");
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CModel::Init (void)
{
m_subModels = null;
m_nModel = -1;
m_nSubModels = 0;
m_nVerts = 0;
m_nFaces = 0;
}

//------------------------------------------------------------------------------

void CModel::Destroy (void)
{
	CSubModel	*psmi, *psmj;

for (psmi = m_subModels; psmi; ) {
	psmj = psmi;
	psmi = psmi->m_next;
	delete psmj;
	}
FreeTextures ();
Init ();
}

//------------------------------------------------------------------------------

int CModel::ReleaseTextures (void)
{
m_textures.Release ();
return 0;
}

//------------------------------------------------------------------------------

int CModel::ReloadTextures (void)
{
return m_textures.Bind (m_bCustom);
}

//------------------------------------------------------------------------------

int CModel::FreeTextures (void)
{
m_textures.Destroy ();
return 0;
}
//------------------------------------------------------------------------------

int CModel::ReadTexture (CFileManager& fp, int nTexture)
{
	CTexture& tex = m_textures.Texture (nTexture);
	char		 fn [FILENAME_LEN], *ps;
	int		 l;

if (CharTok (" \t") != '{')
	return CModel::Error ("syntax error");
tex.SetFlat (false);
while ((pszToken = ReadLine (fp))) {
	if (*pszToken == '}')
		return 1;
	if (!strcmp (pszToken, "*BITMAP")) {
		if (tex.IsLoaded ())	//duplicate
			return CModel::Error ("duplicate bitmap");
		CFileManager::SplitPath (StrTok ("\""), null, fn, null);
		char szFile [256];
		sprintf (szFile, "%s\\%s", m_folder, fn);
		if (!strrchr (szFile, '.'))
			strcat (szFile, ".tga");
		if (!tex.LoadFromFile (::_strlwr (szFile)))
			return CModel::Error ("texture not found");
		l = (int) strlen (::_strlwr (fn)) + 1;
		if (!m_textures.Name (nTexture).Create (l))
			return CModel::Error ("out of memory");
		memcpy (m_textures.Name (nTexture).Buffer (), fn, l);
		if ((ps = strstr (fn, "color")))
			m_textures.Team (nTexture) = atoi (ps + 5) + 1;
		else
			m_textures.Team (nTexture) = 0;
		}
	}
return CModel::Error ("unexpected end of file");
}

//------------------------------------------------------------------------------

int CModel::ReadOpacity (CFileManager& fp, int nTexture)
{
	CTexture& tex = m_textures.Texture (nTexture);

if (CharTok (" \t") != '{')
	return CModel::Error ("syntax error");
tex.SetFlat (false);
while ((pszToken = ReadLine (fp))) {
	if (*pszToken == '}')
		return 1;
	if (!strcmp (pszToken, "*BITMAP")) {
		if (!tex.IsLoaded ())	//duplicate
			return CModel::Error ("missing glow bitmap");
		}
	}
return CModel::Error ("unexpected end of file");
}

//------------------------------------------------------------------------------

int CModel::ReadMaterial (CFileManager& fp)
{
int i = IntTok (" \t");
if ((i < 0) || (i >= m_textures.Count ()))
	return CModel::Error ("invalid bitmap number");
if (CharTok (" \t") != '{')
	return CModel::Error ("syntax error");
CTexture& tex = m_textures.Texture (i);
tex.SetFlat (true);
while ((pszToken = ReadLine (fp))) {
	if (*pszToken == '}')
		return 1;
	if (!strcmp (pszToken, "*MATERAL_DIFFUSE")) {
		rgbaColorf color;
		color.r = (ubyte) (FloatTok (" \t") * 255 + 0.5);
		color.g = (ubyte) (FloatTok (" \t") * 255 + 0.5);
		color.b = (ubyte) (FloatTok (" \t") * 255 + 0.5);
		color.a = 1.0f;
		tex.SetAverageColor (color);
		}
	else if (!strcmp (pszToken, "*MAP_DIFFUSE")) {
		if (!ReadTexture (fp, i))
			return CModel::Error (null);
		}
	else if (!strcmp (pszToken, "*MAP_OPACITY")) {
		if (!ReadOpacity (fp, i))
			return CModel::Error (null);
		}
	}
return CModel::Error ("unexpected end of file");
}

//------------------------------------------------------------------------------

int CModel::ReadMaterialList (CFileManager& fp)
{
if (CharTok (" \t") != '{')
	return CModel::Error ("syntax error");
if (!(pszToken = ReadLine (fp)))
	return CModel::Error ("unexpected end of file");
if (strcmp (pszToken, "*MATERIAL_COUNT"))
	return CModel::Error ("material count missing");
int nTextures = IntTok (" \t");
if (!nTextures)
	return CModel::Error ("no bitmaps specified");
if (!(m_textures.Create (nTextures)))
	return CModel::Error ("out of memory");
while ((pszToken = ReadLine (fp))) {
	if (*pszToken == '}') {
		if (!nTextures)
			return 1;
		return CModel::Error ("bitmaps missing");
		}
	if (!strcmp (pszToken, "*MATERIAL")) {
		if (!ReadMaterial (fp))
			return CModel::Error (null);
		nTextures--;
		}
	}
return CModel::Error ("unexpected end of file");
}

//------------------------------------------------------------------------------

int CModel::ReadSubModel (CFileManager& fp)
{
	CSubModel	*psm;

if (CharTok (" \t") != '{')
	return CModel::Error ("syntax error");
if (!(psm = new CSubModel))
	return CModel::Error ("out of memory");
psm->m_nSubModel = m_nSubModels++;
psm->m_next = m_subModels;
m_subModels = psm;
return psm->Read (fp, m_nFaces, m_nVerts);
}

//------------------------------------------------------------------------------

int CModel::FindSubModel (const char* pszName)
{
	CSubModel *psm;

for (psm = m_subModels; psm; psm = psm->m_next)
	if (!strcmp (psm->m_szName, pszName))
		return psm->m_nSubModel;
return -1;
}

//------------------------------------------------------------------------------

void CModel::LinkSubModels (void)
{
	CSubModel	*psm;

for (psm = m_subModels; psm; psm = psm->m_next)
	psm->m_nParent = FindSubModel (psm->m_szParent);
}

//------------------------------------------------------------------------------

int CSubModel::SaveBinary (CFileManager& fp)
{
fp.Write (m_szName, 1, sizeof (m_szName));
fp.Write (m_szParent, 1, sizeof (m_szParent));
fp.Write (m_nSubModel);
fp.Write (m_nParent);
fp.Write (m_nTexture);
fp.Write (m_nFaces);
fp.Write (m_nVerts);
fp.Write (m_nTexCoord);
fp.Write (m_nIndex);
fp.Write (m_bRender);
fp.Write (m_bGlow);
fp.Write (m_bFlare);
fp.Write (m_bBillboard);
fp.Write (m_bThruster);
fp.Write (m_bWeapon);
fp.Write (m_bHeadlight);
fp.Write (m_bBombMount);
fp.Write (m_nGun);
fp.Write (m_nBomb);
fp.Write (m_nMissile);
fp.Write (m_nType);
fp.Write (m_nWeaponPos);
fp.Write (m_nGunPoint);
fp.Write (m_nBullets);
fp.Write (m_bBarrel);
fp.WriteVector (m_vOffset.v);
m_faces.Write (fp);
m_vertices.Write (fp);
m_texCoord.Write (fp);
return 1;
}

//------------------------------------------------------------------------------

int CModel::SaveBinary (const char* pszFolder, const short nModel)
{
	CFileManager	fp;

char szFile [256];
sprintf (szFile, "%s\\model%3d.bin", pszFolder, nModel);

if (!fp.Open (szFile, "wb"))
	return 0;
fp.WriteInt32 (MODEL_DATA_VERSION);
fp.Write (m_nModel);
fp.Write (m_nSubModels);
fp.Write (m_nVerts);
fp.Write (m_nFaces);
fp.Write (m_bCustom);

m_textures.WriteBinary (fp);

CSubModel*	pSubModel = m_subModels;

for (int i = 0; i < m_nSubModels; i++, pSubModel = pSubModel->m_next)
	pSubModel->SaveBinary (fp);
fp.Close ();
return 1;
}

//------------------------------------------------------------------------------

int CSubModel::ReadBinary (CFileManager& fp)
{
m_next = null;
fp.Read (m_szName, 1, sizeof (m_szName));
fp.Read (m_szParent, 1, sizeof (m_szParent));
m_nSubModel = fp.ReadInt16 ();
m_nParent = fp.ReadInt16 ();
m_nTexture = fp.ReadInt16 ();
m_nFaces = fp.ReadInt16 ();
m_nVerts = fp.ReadInt16 ();
m_nTexCoord = fp.ReadInt16 ();
m_nIndex = fp.ReadInt16 ();
m_bRender = ubyte (fp.ReadUByte ());
m_bGlow = ubyte (fp.ReadUByte ());
m_bFlare = ubyte (fp.ReadUByte ());
m_bBillboard = ubyte (fp.ReadUByte ());
m_bThruster = ubyte (fp.ReadUByte ());
m_bWeapon = ubyte (fp.ReadUByte ());
m_bHeadlight = ubyte (fp.ReadUByte ());
m_bBombMount = ubyte (fp.ReadUByte ());
m_nGun = fp.ReadUByte ();
m_nBomb = fp.ReadUByte ();
m_nMissile = fp.ReadUByte ();
m_nType = fp.ReadUByte ();
m_nWeaponPos = fp.ReadUByte ();
m_nGunPoint = fp.ReadUByte ();
m_nBullets = fp.ReadUByte ();
m_bBarrel = fp.ReadUByte ();
fp.ReadVector (m_vOffset.v);
if ((m_nFaces > 100000) || (m_nVerts > 100000) || (m_nTexCoord > 100000))	//probably invalid
	return 0;
if ((m_nFaces && !m_faces.Create (m_nFaces)) ||
	 (m_nVerts && !m_vertices.Create (m_nVerts)) ||
	(m_nTexCoord && !m_texCoord.Create (m_nTexCoord)))
	return 0;
m_faces.Read (fp);
m_vertices.Read (fp);
m_texCoord.Read (fp);
return 1;
}

//------------------------------------------------------------------------------

int CModel::ReadBinary (const char* pszFolder, const short nModel, time_t tASE)
{
	CFileManager	fp;
	int				h, i;
	
char szFile [256];
sprintf (szFile, "%s\\model%3d.bin", pszFolder, nModel);

time_t tBIN = fp.Date (szFile);

if (tASE > tBIN)
	return 0;

if (!fp.Open (szFile, "rb"))
	return 0;
h = fp.ReadInt32 ();
if (h != MODEL_DATA_VERSION) {
	fp.Close ();
	Destroy ();
	return 0;
	}
m_nModel = fp.ReadInt32 ();
m_nSubModels = fp.ReadInt32 ();
m_nVerts = fp.ReadInt32 ();
m_nFaces = fp.ReadInt32 ();
m_bCustom = fp.ReadInt32 ();

m_subModels = null;
if (m_textures.ReadBinary (fp)) {
	fp.Close ();
	Destroy ();
	return 0;
	}

CSubModel*	pSubModel, * pTail = null;

m_subModels = null;
for (i = 0; i < m_nSubModels; i++) {
	if (!(pSubModel = new CSubModel)) {
		fp.Close ();
		Destroy ();
		return 0;
		}
	if (m_subModels)
		pTail->m_next = pSubModel;
	else
		m_subModels = pSubModel;
	pTail = pSubModel;
	try {
		if (!pSubModel->ReadBinary (fp)) {
			fp.Close ();
			Destroy ();
			return 0;
			}
		}
	catch(...) {
		char szMsg [256];
		sprintf (szMsg, "Compiled model file 'model%03d.bin' is damaged and will be replaced\n", m_nModel);
		INFOMSG (szMsg);
		fp.Close ();
		Destroy ();
		return 0;
		}
	}

return 1;
}

//------------------------------------------------------------------------------

int CModel::Read (const char* pszFolder, const short nModel)
{
	CFileManager	fp;
	int				nResult = 1;

if (m_nModel >= 0)
	return 0;

#ifdef _DEBUG
if (nModel == nDbgModel)
	nDbgModel = nDbgModel;
#endif

char szFile [256];
if (nModel == 108)
	sprintf (szFile, "%s\\pyrogl.ase", pszFolder);
else if (nModel == 109)
	sprintf (szFile, "%s\\wolf.ase", pszFolder);
else if (nModel == 110)
	sprintf (szFile, "%s\\phantomxl.ase", pszFolder);
else
	sprintf (szFile, "%s\\model%d.ase", pszFolder, nModel);

#if USE_BINARY_MODELS
try {
	if (ReadBinary (pszFolder, nModel, fp.Date (szFile)))
		return 1;
	}
catch(...) {
	char szMsg [256];
	sprintf (szMsg, "Compiled model file 'model%03d.bin' is damaged and will be replaced\n", nModel);
	INFOMSG (szMsg);
	Destroy ();
	}
#endif
if (!fp.Open (szFile, "rb")) 
	return 0;
	
m_folder = pszFolder;
bErrMsg = 0;
aseFile = &fp;
Init ();
m_nModel = nModel;
while ((pszToken = ReadLine (fp))) {
	if (!strcmp (pszToken, "*MATERIAL_LIST")) {
		if (!(nResult = ReadMaterialList (fp)))
			break;
		}
	else if (!strcmp (pszToken, "*GEOMOBJECT")) {
		if (!(nResult = ReadSubModel (fp)))
			break;
		}
	}
fp.Close ();
if (!nResult)
	Destroy ();
else {
	LinkSubModels ();
#if USE_BINARY_MODELS
	SaveBinary (pszFolder, nModel);
#endif
	}
aseFile = null;
return nResult;
}

//------------------------------------------------------------------------------
//eof

