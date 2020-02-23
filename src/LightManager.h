#ifndef __lightman_h
#define __lightman_h

#include "define.h"

# pragma pack(push, packing)
# pragma pack(1)

#include "Vector.h"
#include "FileManager.h"
#include "carray.h"

#define DEFAULT_LIGHT_RENDER_DEPTH 6

// -----------------------------------------------------------------------------

typedef struct {
	short	ticks;
	short	impulse;
} tLightTimer;

// -----------------------------------------------------------------------------

typedef struct {
	bool	bIsOn;
	bool	bWasOn;
} tLightStatus;

// -----------------------------------------------------------------------------

#define MAX_VARIABLE_LIGHTS 100

class CLightAnimInfo {
	public:
		tLightTimer		m_timers [MAX_VARIABLE_LIGHTS];
		tLightStatus	m_status [SEGMENT_LIMIT][MAX_SIDES_PER_SEGMENT];

		inline void Clear (void) {
			memset (m_timers, 0, sizeof (m_timers));
			memset (m_status, 0xff, sizeof (m_status));
			};
	};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#if 1 //def _DEBUG

typedef CStaticArray< CLightDeltaIndex, MAX_LIGHT_DELTA_INDICES_D2X > lightDeltaIndexList;
typedef CStaticArray< CLightDeltaValue, MAX_LIGHT_DELTA_VALUES_D2X > lightDeltaValueList;
typedef CStaticArray< CVariableLight, MAX_VARIABLE_LIGHTS > variableLightList;

typedef CStaticArray< CFaceColor, SEGMENT_LIMIT * 6 > faceColorList;
typedef CStaticArray< CTextureColor, MAX_TEXTURES_D2 > texColorList;
typedef CStaticArray< CVertexColor, VERTEX_LIMIT > vertexColorList;

#else

typedef CLightDeltaIndex lightDeltaIndexList [MAX_LIGHT_DELTA_INDICES_D2X];
typedef CLightDeltaValue lightDeltaValueList [MAX_LIGHT_DELTA_VALUES_D2X];
typedef CVariableLight variableLightList [MAX_VARIABLE_LIGHTS];

typedef CFaceColor faceColorList [SEGMENT_LIMIT * 6];
typedef CTextureColor texColorList [MAX_TEXTURES_D2];
typedef CVertexColor vertexColorList [VERTEX_LIMIT];

#endif

class CLightManager {
	private:
		variableLightList		m_variableLights;
		int						m_nCount;
		lightDeltaIndexList	m_deltaIndex;
		lightDeltaValueList	m_deltaValues;
		CMineItemInfo			m_deltaIndexInfo;
		CMineItemInfo			m_deltaValueInfo;

		faceColorList			m_faceColors;
		texColorList			m_texColors;
		vertexColorList		m_vertexColors;
		int						m_bApplyFaceLightSettingsGlobally;

		uint						m_lightMap [MAX_TEXTURES_D2];

		int						m_staticRenderDepth;
		int						m_deltaRenderDepth;
		int						m_nNoLightDeltas;

		int						m_nTexColorCount;	// dummy for undo system
		int						m_nQuality;

		double					m_fLightScale;
		double					m_fBrightness;
		double					m_fObjectLight;
		double					m_fDeltaLightScale;

		//long						m_defLightMap [MAX_TEXTURES_D2];
		CLightAnimInfo			m_animInfo;

	public:
		inline void ResetInfo (void) {
			m_deltaIndexInfo.Reset ();
			m_deltaValueInfo.Reset ();
			}

		inline lightDeltaIndexList& LightDeltaIndices (void) { return m_deltaIndex; }

		inline lightDeltaValueList& LightDeltaValues (void) { return m_deltaValues; }

		inline variableLightList& VariableLights (void) { return m_variableLights; }

		inline vertexColorList& VertexColors (void) { return m_vertexColors; }

		inline CLightDeltaIndex* LightDeltaIndex (int i) { return &m_deltaIndex [i]; }

		inline CLightDeltaValue* LightDeltaValue (int i) { return &m_deltaValues [i]; }

		inline CVariableLight* VariableLight (int i) { return (i < 0) ? null : &m_variableLights [i]; }

		inline CVertexColor* VertexColor (int i) { return &m_vertexColors [i]; }

		inline texColorList& ColorMap (void) { return m_texColors; }

		inline CTextureColor* TexColor (short nTexture = 0) { return &m_texColors [nTexture & 0x1FFF]; }

		inline CLightAnimInfo& AnimInfo (void) { return m_animInfo; }

		inline tLightTimer* LightTimer (int i = 0) { return &m_animInfo.m_timers [i]; }

		inline tLightStatus* LightStatus (short nSegment = 0, short nSide = 0) { return &m_animInfo.m_status [nSegment][nSide]; }

		inline int& ApplyFaceLightSettingsGlobally (void) { return m_bApplyFaceLightSettingsGlobally; }

		inline uint* LightMap (int i = 0) { return &m_lightMap [i]; }
		
		inline int& Count (void) { return m_nCount; }

		inline int& TexColorCount (void) { return m_nTexColorCount = MAX_TEXTURES_D2; }

		inline int& DeltaIndexCount (void) { return m_deltaIndexInfo.count; }

		inline int& DeltaValueCount (void) { return m_deltaValueInfo.count; }

		inline void SetTexColor (short nBaseTex, CColor* pColor)	{
			if (ApplyFaceLightSettingsGlobally () && (IsLight (nBaseTex) != -1))
				m_texColors [nBaseTex] = *pColor;
			}

		inline CColor* GetTexColor (short nTexture, bool bIsTranspWall)	
			{ return m_bApplyFaceLightSettingsGlobally && (bIsTranspWall || (IsLight (nTexture) != -1)) ? &m_texColors [nTexture] : null; }

		inline faceColorList& FaceColors (void) { return m_faceColors; }

		inline CFaceColor* FaceColor (short nSegment, short nSide = 0) { return &m_faceColors [nSegment * 6 + nSide]; }

		CColor* LightColor (CSideKey key = CSideKey (), bool bUseTexColors = true);

		short LoadDefaults (void);
		bool HasCustomLightMap (void);
		bool HasCustomLightColors (void);

		inline double& Brightness (void) { return m_fBrightness; }
		inline double& LightScale (void) { return m_fLightScale; }
		inline double& DeltaLightScale (void) { return m_fDeltaLightScale; }
		inline double& ObjectLight (void) { return m_fObjectLight; }

		inline int& Quality (void) { return m_nQuality; }

		void ScaleCornerLight (bool bAll = false);
		void CalcAverageCornerLight (bool bAll = false);
		void ComputeStaticLight (bool bAll = false, bool bCopyTexLights = false);
		void SetObjectLight (bool bAll = false, bool bDynSegLights = false);
		bool CalcDeltaLights (int force, int recursion_depth);
		void ComputeVariableLight (int force = 1);

		void Normalize (void);

		int FindLight (int nTexture, tTextureLight* texLightP, int nLights);

		int IsLight (int nBaseTex);
		bool IsBlastableLight (int nBaseTex);
		bool IsVariableLight (CSideKey key);

		int FindDeltaLight (short nSegment, short nSide, short *pi = null);
		CLightDeltaIndex* LightDeltaIndex (CSideKey key);
		short VariableLight (CSideKey key = CSideKey ());
		CVariableLight* AddVariableLight (short index = -1);
		short AddVariableLight (CSideKey key, uint mask = 0xAAAAAAAA, int time = 0x10000 / 4);
		bool DeleteVariableLight (CSideKey key = CSideKey ());
		void DeleteVariableLight (short index, bool bUndo = false);
		int IsExplodingLight(int nBaseTex);

		int LightIsOn (CLightDeltaIndex light);
		uint Brightness (CSegment* pSegment, CSide* pSide);
		uint Brightness (CSideKey& key);


		void CreateLightMap (void);
		void ReadLightMap (CFileManager& fp, uint nSize);
		void WriteLightMap (CFileManager& fp);
		void WriteColorMap (CFileManager& fp);
		void ReadColorMap (CFileManager& fp);
		void ReadColors (CFileManager& fp);
		void WriteColors (CFileManager& fp);
		void ResetColors (CColor *pc, int nColors);

		void ReadVariableLights (CFileManager* fp);
		void WriteVariableLights (CFileManager* fp);
		void ReadLightDeltas (CFileManager* fp);
		void WriteLightDeltas (CFileManager* fp);

		inline void ReadDeltaIndexInfo (CFileManager* fp) { m_deltaIndexInfo.Read (fp); }
		inline void WriteDeltaIndexInfo (CFileManager* fp) { m_deltaIndexInfo.Write (fp); }
		inline void ReadDeltaValueInfo (CFileManager* fp) { m_deltaValueInfo.Read (fp); }
		inline void WriteDeltaValueInfo (CFileManager* fp) { m_deltaValueInfo.Write (fp); }

		inline void ReadLightDeltaInfo (CFileManager* fp) {
			ReadDeltaIndexInfo (fp);
			ReadDeltaValueInfo (fp);
			}

		inline void WriteLightDeltaInfo (CFileManager* fp) {
			WriteDeltaIndexInfo (fp);
			WriteDeltaValueInfo (fp);
			}

		void SortDeltaIndex (void);

		inline void SetRenderDepth (int staticRenderDepth = DEFAULT_LIGHT_RENDER_DEPTH, int deltaRenderDepth = DEFAULT_LIGHT_RENDER_DEPTH) {
			m_staticRenderDepth = staticRenderDepth;
			m_deltaRenderDepth = deltaRenderDepth;
			};

		CLightManager () : 
			m_nCount (0), 
			m_bApplyFaceLightSettingsGlobally (1), 
			m_staticRenderDepth (DEFAULT_LIGHT_RENDER_DEPTH), 
			m_deltaRenderDepth (DEFAULT_LIGHT_RENDER_DEPTH), 
			m_nQuality (0),
			m_fLightScale (100.0),
			m_fBrightness (100.0),
			m_fObjectLight (50.0),
			m_fDeltaLightScale (100.0)
			{}

	private:
		ubyte* ColorToFloat (CColor* pColor, ubyte* pData);

		void CLightManager::LoadColors (CColor *pc, int nColors, int nFirstVersion, int nNewVersion, CFileManager& fp);

		void CLightManager::SaveColors (CColor *pc, int nColors, CFileManager& fp);

		double CornerLight (CVertex* sourceCorners, CVertex& sourceNormal, short nSourceVerts, 
								  CVertex* destCorner, CVertex& destNormal,
								  short nStartSeg, short nDestSeg, short nDestSide, bool bIgnoreAngle);

		bool CalcCornerLights (double cornerLights [], int nChildSeg, int nChildSide, int nSourceSeg, int nSourceSide, CVertex* sourceCorner, CVertex& sourceNormal, bool bIgnoreAngle);

		bool PointSeesPoint (CVertex* p1, CVertex* p2, short nStartSeg, short nDestSeg, short nDestSide);

		bool FaceHasVariableLight (CSideKey key, CSide* pSide, CWall* pWall);

		bool ComputeLightDeltas (int bForce, int nDepth);

		int FindLightDelta (short nSegment, short nSide, short *pi);

		uint LightWeight (short nBaseTex);
		
		void SortDeltaIndex (int left, int right);

		void UnsortDeltaIndex (int left, int right);

		void UnsortDeltaIndex (void);

		void BlendColors (CColor* pSrcColor, CColor* pDestColor, double srcBrightness, double destBrightness);

		void GatherLight (short nSrcSide, short nSrcSeg, uint brightness, bool bAll = false, bool bCopyTexLights = false);

		void GatherFaceLight (double cornerLights [], CSegment* pSegment, short nSide, uint brightness, CColor* pLightColor);
};

extern CLightManager lightManager;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#endif // __lightman_h

