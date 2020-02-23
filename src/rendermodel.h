
#ifndef _RENDERMODEL_H
#define _RENDERMODEL_H

#include <math.h>

#include "mine.h"
#include "dle-xp.h"
#include "asemodel.h"
#include "oofmodel.h"

namespace RenderModel {

class CModel;

//	-----------------------------------------------------------------------------

#define VBO_OFFSET(_i)	(GLvoid *) ((char *) null + (_i))

class CRenderVertex {
	public:
		CFloatVector			m_vertex;
		CFloatVector			m_normal;
		rgbaColorf				m_color;
		tTexCoord2d				m_texCoord;
		};

class CVertex {
	public:
		tTexCoord2d				m_texCoord;
		rgbaColorf				m_renderColor;
		CFloatVector			m_vertex;
		CFloatVector			m_normal;
		rgbaColorf				m_baseColor;
		ushort					m_nIndex;
		char						m_bTextured;
	};

inline int operator- (RenderModel::CVertex* f, CDynamicArray<RenderModel::CVertex>& a) { return a.Index (f); }

class CFace {
	public:
		CFixVector				m_vNormal;
		ushort					m_nVerts;
		short						m_nTexture;
		CTexture*				m_textureP;
		ushort					m_nIndex;
		ushort					m_nId;
		ubyte						m_nSubModel;
		ubyte						m_bThruster;
		ubyte						m_bGlow :2;
		ubyte						m_bBillboard;

	public:
		void SetTexture (CTexture* textureP);
		int GatherVertices (CDynamicArray<CVertex>&, CDynamicArray<CVertex>&, int nIndex);
		
		static int Compare (const CFace* pf, const CFace* pm);

		inline const bool operator< (CFace& other) { return m_nSubModel < other.m_nSubModel; }
		inline const bool operator> (CFace& other) { return m_nSubModel > other.m_nSubModel; }
		const bool operator!= (CFace& other);
	};

inline int operator- (RenderModel::CFace* f, CDynamicArray<RenderModel::CFace>& a) { return a.Index (f); }

class CSubModel {
	public:
		short						m_nSubModel;
#if DBG
		char						m_szName [256];
#endif
		CFixVector				m_vOffset;
		CFixVector				m_vCenter;
		CFloatVector			m_vMin;
		CFloatVector			m_vMax;
		CFace*					m_faces;
		short						m_nParent;
		ushort					m_nFaces;
		ushort					m_nIndex;
		short						m_nBitmap;
		short						m_nHitbox;
		int						m_nRad;
		ushort					m_nAngles;
		ubyte						m_nType :2;
		ubyte						m_bRender :1;
		ubyte						m_bFlare :1;
		ubyte						m_bWeapon :1;
		ubyte						m_bHeadlight :1;
		ubyte						m_bBullets :1;
		ubyte						m_bBombMount :1;
		ubyte						m_bGlow :2;
		ubyte						m_bBillboard :1;
		ubyte						m_bThruster;
		char						m_nGunPoint;
		char						m_nGun;
		char						m_nBomb;
		char						m_nMissile;
		char						m_nWeaponPos;
		ubyte						m_nFrames;
		ubyte						m_iFrame;
		time_t					m_tFrame;

	public:
		CSubModel () { Init (); }
		~CSubModel () { Destroy (); }
		void Init (void) { memset (this, 0, sizeof (*this)); }
		bool Create (void);
		void Destroy (void) { Init (); }
		void InitMinMax (void);
		void SetMinMax (CFloatVector *pVertex);
		void SortFaces (CTexture* textureP);
		void GatherVertices (CDynamicArray<CVertex>& source, CDynamicArray<CVertex>& dest);
		void Size (CModel* pm, CObject* pObject, CFixVector* vOffset);
		int Filter (int nGunId = 0, int nBombId = 0, int nMissileId = 0, int nMissiles = 4);
		void Render (CModel& model, short nSubModel, int nGunId = 0, int nBombId = 0, int nMissileId = 0, int nMissiles = 4);
	};

inline int operator- (RenderModel::CSubModel* f, CDynamicArray<RenderModel::CSubModel>& a) { return a.Index (f); }

class CVertNorm {
	public:
		CFloatVector	vNormal;
		ubyte		nVerts;
	};


class CModel {
	public:

	public:
		short										m_nModel;
		CDynamicArray<CTexture>*			m_textures;
		int										m_teamTextures [8];
		CDynamicArray<CFloatVector>		m_vertices;
		CDynamicArray<CFloatVector>		m_vertNorms;
		CDynamicArray<CFaceColor>			m_color;
		CDynamicArray<CVertex>				m_faceVerts;
		CDynamicArray<CVertex>				m_sortedVerts;
		CDynamicArray<ubyte>					m_vbData;
		CDynamicArray<tTexCoord2d>			m_vbTexCoord;
		CDynamicArray<rgbaColorf>			m_vbColor;
		CDynamicArray<CFloatVector>		m_vbVerts;
		CDynamicArray<CFloatVector>		m_vbNormals;
		CDynamicArray<CSubModel>			m_subModels;
		CDynamicArray<CFace>					m_faces;
		CDynamicArray<CRenderVertex>		m_vertBuf [2];
		CDynamicArray<ushort>				m_index [2];
		short										m_nGunSubModels [MAX_GUNS];
		float										m_fScale;
		short										m_nType; //-1: custom mode, 0: default model, 1: alternative model, 2: hires model
		ushort									m_nFaces;
		ushort									m_iFace;
		ushort									m_nVerts;
		ushort									m_nFaceVerts;
		ushort									m_iFaceVert;
		ushort									m_nSubModels;
		ushort									m_nTextures;
		ushort									m_iSubModel;
		short										m_bHasTransparency;
		short										m_bValid;
		short										m_bRendered;
		ushort									m_bBullets;
		CFixVector								m_vBullets;
		GLuint									m_vboDataHandle;
		GLuint									m_vboIndexHandle;

	public:
		CModel () { Init (); }
		~CModel () { Destroy (); }
		void Init (void);
		void Setup (int bHires, int bSort);
		bool Create (void);
		void Destroy (void);
		ushort FilterVertices (CDynamicArray<CFloatVector>& vertices, ushort nVertices);

		static int CmpVerts (const CFloatVector* pv, const CFloatVector* pm);

		int BuildFromASE (ASE::CModel& aseModel);
		int BuildFromOOF (OOF::CModel& oofModel);
		int BuildFromPOF (int nModel, CPolyModel& polyModel, CDynamicArray<CTexture*>& modelTextures, CFloatVector* objColorP);

		int Render (CViewMatrix* view, CGameObject* pObject, int nGunId = 0, int nBombId = 0, int nMissileId = 0, int nMissiles = 4);

	private:
		void CountASEModelItems (ASE::CModel& aseModel);
		void GetASEModelItems (int nModel, ASE::CModel& aseModel, float fScale);

		void CountOOFModelItems (OOF::CModel& oofModel);
		void GetOOFModelItems (int nModel, OOF::CModel& oofModel, float fScale);

		void AssignPOFFaces (void);
		int CountPOFModelItems (void* modelDataP, ushort* pnSubModels, ushort* pnVerts, ushort* pnFaces, ushort* pnFaceVerts);
		CFace* AddPOFFace (CSubModel* psm, CFace* pmf, CFixVector* pn, ubyte* p, CDynamicArray<CTexture*>& modelTexture, bool bTextured = true);
		int GetPOFModelItems (void *modelDataP, CAngleVector *pAnimAngles, int nThis, int nParent,
									 int bSubObject, CDynamicArray<CTexture*>& modelTextures, CFloatVector *objColorP);

	};	

//	-----------------------------------------------------------------------------------------------------------

} //RenderModel

#endif


