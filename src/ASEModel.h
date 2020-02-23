#ifndef _ASE_H
#define _ASE_H

#include "ModelTextures.h"

#define SLOWMOTION_MODEL	(MAX_POLYGON_MODELS - 33)
#define BULLETTIME_MODEL	(MAX_POLYGON_MODELS - 34)
#define HOSTAGE_MODEL		(MAX_POLYGON_MODELS - 35)
#define BULLET_MODEL			(MAX_POLYGON_MODELS - 36)
#define COCKPIT_MODEL		(MAX_POLYGON_MODELS - 37)

#define MAX_THRUSTERS		16
#define REAR_THRUSTER		1
#define FRONT_THRUSTER		2
#define LEFT_THRUSTER		4
#define RIGHT_THRUSTER		8
#define TOP_THRUSTER			16
#define BOTTOM_THRUSTER		32
#define FRONTAL_THRUSTER	64
#define LATERAL_THRUSTER	128

#define FR_THRUSTERS			(FRONT_THRUSTER | REAR_THRUSTER)
#define LR_THRUSTERS			(LEFT_THRUSTER | RIGHT_THRUSTER)
#define TB_THRUSTERS			(TOP_THRUSTER | BOTTOM_THRUSTER)

namespace ASE {

class CVertex {
	public:
		CDoubleVector	m_vertex;
		CDoubleVector	m_normal;
	};

class CFace {
	public:
		CDoubleVector	m_vNormal;
		ushort			m_nVerts [3];	// indices of vertices 
		ushort			m_nTexCoord [3];
		short				m_nTexture;
};

class CSubModel {
	public:
		CSubModel*		m_next;
		char				m_szName [256];
		char				m_szParent [256];
		short				m_nSubModel;
		short				m_nParent;
		short				m_nTexture;
		ushort			m_nFaces;
		ushort			m_nVerts;
		ushort			m_nTexCoord;
		ushort			m_nIndex;
		ubyte				m_bRender;
		ubyte				m_bGlow;
		ubyte				m_bFlare;
		ubyte				m_bBillboard;
		ubyte				m_bThruster;
		ubyte				m_bWeapon;
		ubyte				m_bHeadlight;
		ubyte				m_bBombMount;
		char				m_nGun;
		char				m_nBomb;
		char				m_nMissile;
		char				m_nType;
		char				m_nWeaponPos;
		char				m_nGunPoint;
		char				m_nBullets;
		char				m_bBarrel;
		CDoubleVector	m_vOffset;

		CDynamicArray<CFace>			m_faces;
		CDynamicArray<CVertex>		m_vertices;
		CDynamicArray<tTexCoord2d>	m_texCoord;

	public:
		CSubModel () { Init (); }
		~CSubModel () { Destroy (); }
		void Init (void);
		void Destroy (void);
		int Read (CFileManager& fp, int& nVerts, int& nFaces);
		int SaveBinary (CFileManager& fp);
		int ReadBinary (CFileManager& fp);

	private:
		int ReadNode (CFileManager& fp);
		int ReadMeshVertexList (CFileManager& fp);
		int ReadMeshFaceList (CFileManager& fp);
		int ReadVertexTexCoord (CFileManager& fp);
		int ReadFaceTexCoord (CFileManager& fp);
		int ReadMeshNormals (CFileManager& fp);
		int ReadMesh (CFileManager& fp, int& nVerts, int& nFaces);
	};

class CModel {
	public:
		CModelTextures			m_textures;
		CSubModel*				m_subModels;
		int						m_nModel;
		int						m_nSubModels;
		int						m_nVerts;
		int						m_nFaces;
		int						m_bCustom;
		const char*				m_folder;

	public:
		CModel () { Init (); }
		~CModel () { Destroy (); }
		void Init (void);
		bool Create (void);
		void Destroy (void);
		int Read (const char* pszFolder, const short nModel);
		int SaveBinary (const char* pszFolder, const short nModel);
		int ReadBinary (const char* pszFolder, const short nModel, time_t tASE);
		int ReloadTextures (void);
		int ReleaseTextures (void);
		int FreeTextures (void);

		static int Error (const char *pszMsg);

	private:
		int ReadTexture (CFileManager& fp, int nTexture);
		int ReadOpacity (CFileManager& fp, int nTexture);
		int ReadMaterial (CFileManager& fp);
		int ReadMaterialList (CFileManager& fp);
		int FindSubModel (const char *pszName);
		void LinkSubModels (void);
		int ReadSubModel (CFileManager& fp);
	};

} //ASEModel

int ASE_ReloadTextures (void);
int ASE_ReleaseTextures (void);

#endif //_ASE_H

