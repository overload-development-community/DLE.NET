#ifndef __texturemanager_h
#define __texturemanager_h

#include "textures.h"
#include "carray.h"
#include "sll.h"

//------------------------------------------------------------------------

class CAnimationClipInfo {
	public:
		short						m_nType; // 0: geometry, 1: object, 2: effect
		short						m_nTexture; // the level texture id of the texture the animation belongs to
		int						m_nPlayTime;
		int						m_nFrameTime;
		bool						m_bBidirectional;
		CDynamicArray<short>	m_frames;

		CAnimationClipInfo () : m_nType (0), m_nTexture (-1), m_nPlayTime (0), m_nFrameTime (0), m_bBidirectional (false) {}
		int LoadAnimationFrames (CFileManager& fp, int nMaxFrames, bool bIndices);

		inline short Key (void) { return m_nTexture; }
		inline size_t Find (short nFrame) { return m_frames.Find (nFrame); }
		inline bool operator== (short nFrame) { return Find (nFrame) >= 0; }
		inline size_t FrameCount (void) { return m_frames.Length (); }
		inline short Frame (ushort i = 0) { return m_frames.Buffer () ? m_frames [i % (ushort) m_frames.Length ()] : 0; }
		inline int FrameTime (void) { return m_nFrameTime; }
		inline bool Bidirectional (void) { return m_bBidirectional; }
};

typedef CSLL< CAnimationClipInfo, CAnimationClipInfo >	CAnimationClipList;

//------------------------------------------------------------------------

class CAnimationClipLoader {
	public:
		virtual int LoadAnimationClips (CAnimationClipList& animations, CFileManager& fp) = 0;
};

class CAnimationClipLoaderD1 : public CAnimationClipLoader {
	public:
		virtual int LoadAnimationClips (CAnimationClipList& animations, CFileManager& fp);
};

class CAnimationClipLoaderD2 : public CAnimationClipLoader {
	public:
		virtual int LoadAnimationClips (CAnimationClipList& animations, CFileManager& fp);
};

class CEffectClipLoaderD1 : public CAnimationClipLoader {
	public:
		virtual int LoadAnimationClips (CAnimationClipList& animations, CFileManager& fp);
};

class CEffectClipLoaderD2 : public CAnimationClipLoader {
	public:
		virtual int LoadAnimationClips (CAnimationClipList& animations, CFileManager& fp);
};

class CWallEffectClipLoaderD1 : public CAnimationClipLoader {
	public:
		virtual int LoadAnimationClips (CAnimationClipList& animations, CFileManager& fp);
};

class CWallEffectClipLoaderD2 : public CAnimationClipLoader {
	public:
		virtual int LoadAnimationClips (CAnimationClipList& animations, CFileManager& fp);
};

class CAnimationLoaderFactory {
	public:
		CAnimationClipLoader* GetAnimationLoader (int nVersion, int nType) {
			if (nVersion) {
				switch (nType) {
					case 0:
						return new CAnimationClipLoaderD2; // object (robot) animations
					case 1:
						return new CEffectClipLoaderD2; // effect (e.g. explosion) animations
					case 2:
						return new CWallEffectClipLoaderD2; // wall (geometry) texture animations
					}
				}
			else {
				switch (nType) {
					case 0:
						return new CAnimationClipLoaderD1;
					case 1:
						return new CEffectClipLoaderD1;
					case 2:
						return new CWallEffectClipLoaderD1;
					}
				}
			return null;
			}
	};

//------------------------------------------------------------------------

class CDtxSoundList {
public:
	~CDtxSoundList ();

	void Load (CFileManager& fp, long nFileSize);
	int Count () { return m_headers.Length (); }
	uint WriteHeader (CFileManager& fp, uint nItem, uint nOffset);
	void WriteSoundClip (CFileManager& fp, uint nItem);

private:
	CDynamicArray <PIG_SOUND> m_headers;
	CDynamicArray <void*> m_sounds;
};

//------------------------------------------------------------------------

#define MAX_TEXTURES ((theMine == null) ? MAX_TEXTURES_D2 : DLE.IsD1File () ? MAX_TEXTURES_D1 : MAX_TEXTURES_D2)

//------------------------------------------------------------------------

#define SMOKE_ICON		0
#define SNOW_ICON			1
#define RAIN_ICON			2
#define BUBBLE_ICON		3
#define FIRE_ICON			4
#define LIGHTNING_ICON	5
#define SOUND_ICON		6
#define WAYPOINT_ICON	7
#define LIGHT_ICON		8
#define CIRCLE_ICON		9
#define ICON_COUNT		10

#if 1 //_DEBUG

typedef CDynamicArray< CTexture > textureList;

#else

typedef CTexture* textureList;

#endif

class CTextureManager {
	private:
		textureList		m_textures [2];
		CDynamicArray <CTexture*>	m_overrides [2]; // Items can be blank if there is no custom texture
		CDynamicArray <bool>			m_bModified [2]; // Tracks whether textures have changed since last save
		CDynamicArray <CTexture*>	m_previous [2];  // Previous texture as of save, to allow reverts
		CAnimationClipList			m_animationClips [2];
		CDynamicArray<CAnimationClipInfo*>	m_animationIndex [2];

		uint				m_nTextures [2];
		char**			m_names [2];
		char				m_paletteName [2][256];
		ushort*			m_index [2];
		uint				m_nOffsets [2];
		CPigTexture*	m_info [2];
		CPigHeader		m_header [2];
		CBGRA				m_bmBuf [512 * 512 * 32];	// max texture size: 512x512, RGBA, 32 frames
		char				m_pigFiles [2][256];
		bool				m_bAvailable [2];
		bool*				m_bUsed [2];
		CTexture			m_arrow;
		CTexture			m_icons [ICON_COUNT];

	public:
		// Looks up textures by level texture ID. Pointers returned by this function should not be
		// stored persistently as they may change.
#if !DBG
		inline 
#endif
		CTexture* Textures (int nTexture, int nVersion = -1) { 
			int nVersionResolved = (nVersion < 0) ? Version () : nVersion;
			if (!m_textures [nVersionResolved].Buffer ())
				return null;
			if (nTexture < 0)
				return TextureByIndex (-nTexture - 1, nVersionResolved);
			return TextureByIndex (Index (nTexture), nVersionResolved);
			}

		inline bool HaveAnimationInfo (int nVersion = -1) { return m_animationIndex [Version (nVersion)].Buffer () != null; }

		inline CAnimationClipInfo* AnimationIndex (short nTexture, int nVersion = -1) { return HaveAnimationInfo () ? m_animationIndex [Version (nVersion)][nTexture] : null; }

		inline int AnimationFrame (int nTexture) { 
			nTexture = (nTexture < 0) ? -nTexture - 1 : Index (nTexture);
			CAnimationClipInfo* pClipInfo = AnimationIndex (nTexture); 
			if (!pClipInfo)
				return -1;
			return int (pClipInfo->Find (nTexture));
			}

		inline bool IsAnimationRoot (int nTexture, int nVersion = -1) { 
			nTexture = (nTexture < 0) ? -nTexture - 1 : Index (nTexture);
			CAnimationClipInfo* pClipInfo = AnimationIndex (nTexture, nVersion); 
			return pClipInfo ? nTexture == pClipInfo->Frame (0) : false;
			}

		inline bool IsAnimationFrame (int nTexture) { 
			return AnimationFrame (nTexture) >= 0;
			}

		inline bool IsAnimationSubFrame (int nTexture) { 
			CAnimationClipInfo* pClipInfo = AnimationIndex ((nTexture < 0) ? -nTexture - 1 : Index (nTexture)); 
			return pClipInfo ? nTexture != pClipInfo->Frame (0) : false;
			}

		inline int AnimationRoot (int nTexture) { 
			CAnimationClipInfo* pClipInfo = AnimationIndex ((nTexture < 0) ? -nTexture - 1 : Index (nTexture)); 
			return pClipInfo ? pClipInfo->Frame (0) : -1;
			}

		inline bool ShareAnimation (int nTexture1, int nTexture2) { 
			int nRoot = AnimationRoot (nTexture1);
			return (nRoot >= 0) && (nRoot == AnimationRoot (nTexture2));
			}

		// Looks up textures by global texture ID. Pointers returned by this function should not be
		// stored persistently as they may change.
#if !DBG
		inline 
#endif
		CTexture* TextureByIndex (uint nIndex, int nVersion = -1) {
			int nVersionResolved = (nVersion < 0) ? Version () : nVersion;
			if (!m_textures [nVersionResolved].Buffer ())
				return null;
			if (nIndex >= (uint) GlobalTextureCount (nVersionResolved))
				return null;
			if (m_overrides [nVersionResolved][nIndex] != null)
				return m_overrides [nVersionResolved][nIndex];
			return &m_textures [nVersionResolved][nIndex];
			}

		inline uint Index (int nTexture, int nVersion = -1) {
			// It's possible to store an out-of-bounds texture number; bad idea but shouldn't crash
			if ((nTexture >= 0) && (nTexture < MaxTextures (nVersion)))
				return m_index [(nVersion < 0) ? Version () : nVersion][nTexture] - 1;
			else
				return 0;
			}

		int TexIdFromIndex (uint nIndex, int nVersion = -1);

		inline CPigTexture* Info (int nVersion, uint nIndex) {
			if (!m_info [nVersion])
				return null;
			return &m_info [nVersion][nIndex];
			}

		// Location at which texture bitmap data is stored in the PIG
		inline uint TexDataOffset (int nVersion = -1) {
			return m_nOffsets [(nVersion < 0) ? Version () : nVersion];
			}

		int MaxTextures (int nVersion = -1);

		int GlobalTextureCount (int nVersion = -1);
		
		bool LoadTextures (int nVersion = -1, bool bClearExisting = true);
		
		short PrevAnimationFrame (short nTexture);

		short NextAnimationFrame (short nTexture);

		bool Check (int nTexture);
		
		void Load (ushort nBaseTex, ushort nOvlTex);

		void ReadMod (char* pszFolder);
	
		void LoadMod (void);

		CTexture* OverrideTexture (uint nIndex, const CTexture* newTexture = null, int nVersion = -1);

		void RevertTexture (uint nIndex, int nVersion = -1);
		
		void ReleaseTextures (void);

		bool HasCustomTextures (void);
		
		int CountCustomTextures (void);

		int CountModifiedTextures (void);
		
		void UnTagUsedTextures (void);

		void TagUsedTextures (void);

		inline bool IsTextureUsed (int nTexture, int nVersion = -1) {
			if (nTexture < 0 || nTexture >= MaxTextures (nVersion))
				return false;
			return m_bUsed [(nVersion < 0) ? Version () : nVersion][nTexture];
			}
		
		void RemoveCustomTextures (bool bUnusedOnly = true);

		void CommitTextureChanges (void);

		void UndoTextureChanges (void);
		
		CFileManager* OpenPigFile (int nVersion);

		inline bool HaveInfo (int nVersion) { return m_info [nVersion] != null; }
		
		int Version (int nVersion = -1);
		
		inline char* Name (int nVersion, short nTexture) { 
			if (nVersion < 0)
				nVersion = Version ();
			if (!m_names [nVersion])
				return "";
			char* p = m_names [nVersion][nTexture];
			return p ? p : "";
			}
		
		inline int Index (CTexture* pTexture, int nVersion = -1) { return int (pTexture - &m_textures [(nVersion < 0) ? Version () : nVersion][0]); }

		inline CTexture& Arrow (void) { return m_arrow; }

		inline CTexture& Icon (int i) { return m_icons [i]; }

		inline int HaveArrow (void) { return m_arrow.Buffer () != null; }

		inline bool IsLava (short nTexture) { return (strstr (Name (-1, nTexture), "lava") != null); }

		bool IsScrolling (UINT16 texture);

		int ScrollSpeed (UINT16 texture, int *x, int *y);

		int ScrollDirection (UINT16 texture);

		inline double Scale (int nVersion, short nTexture) {
			return m_textures [nVersion].Buffer ()
					 ? Textures (nTexture, nVersion)->Scale (nTexture)
					 : 1.0;
			}

		int UsedCount (uint nIndex);

		inline int UsedCount (const CTexture *pTexture) { return pTexture ? UsedCount (pTexture->Index ()) : 0; }

		inline const CTexture* BaseTextures (uint nIndex, int nVersion = -1) {
			return &m_textures [(nVersion < 0) ? Version () : nVersion][nIndex];
			}

		int ReadPog (CFileManager& fp, long nFileSize);

		int ReadDtx (CFileManager& fp, long nFileSize);

		int CreatePog (CFileManager& fp);

		int CreateDtx (CFileManager& fp, CDtxSoundList& soundList);

		int WriteCustomTexture (CFileManager& fp, const CTexture *pTexture, bool bUseBMPFileFormat = false);

		void Setup (void);

		void Destroy (void);

		bool Reload (int nVersion, bool bForce = true);

		bool ChangePigFile (const char *pszPigPath, int nVersion = -1);

		bool Available (int nVersion = -1);

		int InitShaders (void);

		int DeployShader (int nType, CFaceListEntry* fle);

		void CreateGLTextures (int nVersion = -1);

		CBGRA* SharedBuffer (void) { return m_bmBuf; }

		size_t SharedBufferSize (void) { return sizeof (m_bmBuf); }

		CTexture *UpdateTextureClip (short& nTexId, short& nPrevTexId, int& nFrame, int& nDirection);
	
		bool CheckScrollingTexture (short nTexId, int xScrollOffset [], int yScrollOffset [], int& x, int& y);

		void UpdateScrollingTexture (int xScrollOffset [], int yScrollOffset [], int x, int y);


		CTextureManager() { 
			m_paletteName [0][0] = 0; 
			m_bAvailable [0] = m_bAvailable [1] = false;
			}
	
		~CTextureManager() { Destroy (); }

	private:
		int LoadIndex (int nVersion);

		void LoadNames (int nVersion);

		bool LoadInfo (int nVersion);

		void ReleaseTextures (int nVersion);

		void Create (int nVersion);

		void Destroy (int nVersion);

		uint WriteCustomTextureHeader (CFileManager& fp, const CTexture *pTexture, uint nId, uint nOffset);

		int LoadAnimationClips (CAnimationClipList& animations, CFileManager& fp, CAnimationClipLoader* loader);

		bool OpenAnimationFile (CFileManager& fp);

		bool SkipToAnimationData (CFileManager& fp);

		void LoadAnimationData (int nVersion = -1);

		void BuildAnimationIndex (int nVersion);

		CAnimationClipInfo* FindAnimation (short nTexture);

		int CreateMissingAnimationClips (int nVersion);

		int AddMissingAnimationClip (CAnimationClipList& animations, int nIndex, int nFrames);

	};

extern CTextureManager textureManager;

//------------------------------------------------------------------------

#endif //__texturemanager_h