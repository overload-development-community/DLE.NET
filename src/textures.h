#ifndef __textures_h
#define __textures_h

#include "Define.h"
#include "Types.h"
#include "carray.h"
#include "glew.h"

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

/* pig file types */
typedef struct {
  int nTextures;
  int nSounds;
} PIG_HEADER_D1;

typedef struct {
	char name[8];
	ubyte dflags; /* this is only important for large bitmaps like the cockpit */
	ubyte width;
	ubyte height;
	ubyte flags;
	ubyte avgColor;
	uint offset;
} PIG_TEXTURE_D1;

typedef struct {
	int nId;
	int nVersion;
	int nTextures;
} PIG_HEADER_D2;

typedef struct {
	char name[8];
	ubyte dflags;   // bits 0-5 anim frame num, bit 6 abm flag
	ubyte width;    // low 8 bits here, 4 more bits in pad
	ubyte height;   // low 8 bits here, 4 more bits in pad
	ubyte whExtra;  // bits 0-3 xsize, bits 4-7 ysize
	ubyte flags;    // see BM_FLAG_XXX in define.h
	ubyte avgColor; // average color
	uint offset;
} PIG_TEXTURE_D2;

typedef struct {
	char name[8];
	int length;
	int data_length;
	int offset;
} PIG_SOUND;

typedef struct {
	char name[8];
	short number;
} TEXTURE;

typedef struct {
	ubyte		flags;     //values defined above
	ubyte		pad[3];    //keep alignment
	int		lighting;  //how much light this casts
	int		damage;    //how much damage being against this does (for lava)
	short		nEffectClip; //the tEffectInfo that changes this, or -1
	short		destroyed; //bitmap to show when destroyed, or -1
	short		slide_u,slide_v;    //slide rates of texture, stored in 8:8 fix
}  tTexMapInfoD2;

typedef struct {
	char		filename[13];
	ubyte		flags;
	int		lighting;		// 0 to 1
	int		damage;			//how much damage being against this does
	int		nEffectClip;		//if not -1, the tEffectInfo that changes this   
} tTexMapInfoD1;

//------------------------------------------------------------------------

static inline int Pow2ize (int v) 
{
#if 1
for (int i = 2; i <= 0x10000000; i *= 2)
	if (v <= i)
		return i;
#endif
return v;
}

static inline int Pow2Dim (int w, int h)
{
return (w < h) ? Pow2ize (h) : Pow2ize (w);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

typedef struct {
  ubyte	flags;				//values defined above
  ubyte	pad[3];				//keep alignment
  int		lighting;			//how much light this casts
  int		damage;				//how much damage being against this does (for lava)
  short	nEffectInfo;		//the eclip that changes this, or -1
  short	destroyed;			//bitmap to show when destroyed, or -1
  short	slide_u, slide_v; //slide rates of texture, stored in 8:8 int
} tTextureEffectInfo;

typedef struct {
	char	filename[13];
	ubyte	flags;
	int	lighting;		// 0 to 1
	int	damage;			//how much damage being against this does
	int	nEffectClip;		//if not -1, the tEffectInfo that changes this   
} tTextureEffectInfoD1;


typedef struct tAnimationInfo {
  int		nPlayTime;  //total time (in seconds) of clip
  int		nFrameCount;
  int		nFrameTime; //time (in seconds) of each frame
  int		nFlags;
  short	nSound;
  ushort	frames [MAX_ANIMATION_FRAMES];
  int		nLightValue;
} tAnimationInfo;

typedef struct tEffectInfo {
  tAnimationInfo  animationInfo;		//embedded bitmap (video) clip
  int		nTimeLeft;						//for sequencing
  int		nFrameCount;					//for sequencing
  short	nChangingWallTexture;	   //Which element of Textures array to replace.
  short	nChangingObjectTexture;		//Which element of ObjBitmapPtrs array to replace.
  int		flags;							//see above
  int		nCriticalAnimation;			//use this clip instead of above one when mine critical
  int		nDestTexture;					//use this bitmap when monitor destroyed
  int		nDestAnimation;				//what vclip to play when exploding
  int		nDestEffect;					//what eclip to play when exploding
  int		nDestSize;						//3d size of explosion
  int		nSound;							//what sound this makes
  int		nSegment, nSide;				//what pSegment & side, for one-shot clips
} tEffectInfo;

typedef struct tWallEffectInfo {
  int		 nPlayTime;
  short	 nFrameCount;
  short	 frames [MAX_WALL_EFFECT_FRAMES_D2];
  short	 nOpenSound;
  short	 nCloseSound;
  short	 nFlags;
  char	 filename [13];
  char	 pad;
} tWallEffectInfo;

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

class CPigHeader {
public:
	int nId;
	int nVersion;
	int nTextures;
	int nSounds;

	int m_nVersion;

	inline int Size (void) { return m_nVersion ? sizeof (PIG_HEADER_D2) : sizeof (PIG_HEADER_D1); }

	CPigHeader (int nVersion = 1) : nId (0), nVersion (0), nTextures (0), nSounds (0) {
		m_nVersion = nVersion;
		}

	void Read (CFileManager& fp) {
		if (m_nVersion == 1) {
			nId = fp.ReadInt32 ();
			nVersion = fp.ReadInt32 ();
			nTextures = fp.ReadInt32 ();
			}
		else {
			nTextures = fp.ReadInt32 ();
			nSounds = fp.ReadInt32 ();
			}
		}

	void Write (CFileManager& fp) {
		if (m_nVersion == 1) {
			 fp.Write (nId);
			 fp.Write (nVersion);
			 fp.Write (nTextures);
			}
		else {
			fp.Write (nTextures);
			fp.Write (nSounds);
			}
		}
};

//------------------------------------------------------------------------

typedef struct tPigTextureHeader {
	char		name[8];
	ubyte		dflags;   // bits 0-5 anim frame num, bit 6 abm flag
	ushort	width;    // low 8 bits here, 4 more bits in pad
	ushort	height;   // low 8 bits here, 4 more bits in pad
	ubyte		whExtra;  // bits 0-3 xsize, bits 4-7 ysize
	ubyte		flags;    // see BM_FLAG_XXX in define.h
	ubyte		avgColor; // average color
	uint		offset;
	} tPigTextureHeader;

class CPigTexture : public tPigTextureHeader {
public:
	int m_nVersion;

	CPigTexture (int nVersion = 1) {
		m_nVersion = nVersion;
		}

	inline int Size (void) { return m_nVersion ? sizeof (PIG_TEXTURE_D2) : sizeof (PIG_TEXTURE_D1); }

	void Decode (void) {
		width += (ushort) (whExtra % 16) * 256;
		if ((flags & 0x80) && (width > 256))
			height *= width;
		else
			height += (ushort) (whExtra / 16) * 256;
		}

	void Encode (void) {
		if ((flags & 0x80) && (width > 256)) {
			whExtra = width / 256;
			height /= width;
			}
		else {
			whExtra = (width / 256) + ((height / 256) * 16);
			height %= 256;
			}
		width %= 256;
		}

	void Setup (int nVersion, ushort w = 0, ushort h = 0, ubyte f = 0, uint o = 0) {
		m_nVersion = nVersion;
		width = w;
		height = h;
		whExtra = (w >> 8) | ((h >> 4) & 0xF0);
		flags = f;
		offset = o;
		dflags = 0;
		avgColor = 0;
		}

	void Read (CFileManager* fp, int nVersion = -1) {
		if (nVersion >= 0)
			m_nVersion = nVersion;
		fp->ReadBytes (name, sizeof (name));
		dflags = fp->ReadUByte ();
		width = (ushort) fp->ReadUByte ();
		height = (ushort) fp->ReadUByte ();
		if (m_nVersion == 1)
			whExtra = fp->ReadUByte ();
		else {
			name [7] = 0;
			whExtra = 0;
			}
		flags = fp->ReadUByte ();
		avgColor = fp->ReadUByte ();
		offset = fp->ReadUInt32 ();
		Decode ();
		}

	void Write (CFileManager* fp) {
		Encode ();
		fp->WriteBytes (name, sizeof (name));
		fp->Write (dflags);
		fp->WriteByte ((ubyte) width);
		fp->WriteByte ((ubyte) height);
		if (m_nVersion == 1)
			fp->Write (whExtra);
		fp->Write (flags);
		fp->Write (avgColor);
		fp->Write (offset);
		}

	inline int Dim (void) { 
		int w = Pow2ize (width);
		int h = Pow2ize (height); 
		return (w < h) ? h : w;
		}

	inline int BufSize (void) { 
		int d = Pow2Dim (width, height);
		return d * d;
		}
};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

typedef struct {
    ubyte  identSize;          // size of ID field that follows 18 ubyte header (0 usually)
    ubyte  colorMapType;      // type of colour map 0=none, 1=has palette
    ubyte  imageType;          // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

    short colorMapStart;     // first colour map entry in palette
    short colorMapLength;    // number of colours in palette
    ubyte  colorMapBits;      // number of bits per palette entry 15,16,24,32

    short xStart;             // image x origin
    short yStart;             // image y origin
    short width;              // image width in pixels
    short height;             // image height in pixels
    ubyte  bits;               // image bits per pixel 8,16,24,32
    ubyte  descriptor;         // image descriptor bits (vh flip bits)
} tTgaHeader;

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

typedef struct tRGBA {
	ubyte r, g, b, a;
} tRGBA;


typedef struct tBGRA {
	ubyte b, g, r, a;
} tBGRA;


typedef struct tABGR {
	ubyte a, b, g, r;
} tABGR;


class CBGR {
	public:
		ubyte b, g, r;

		CBGR (ubyte red = 0, ubyte green = 0, ubyte blue = 0)
			: r (red), g (green), b (blue)
			{}

		inline COLORREF ColorRef (void) { return RGB (r, g, b); }

		inline uint Delta (CBGR& other) {
			return (uint) (Sqr ((int) r - (int) other.r) + Sqr ((int) g - (int) other.g) + Sqr ((int) b - (int) other.b));
			}

		inline const bool operator== (const CBGR& other) const { return (r == other.r) && (g == other.g) && (b == other.b); }

	private:
		inline int Sqr (int i) { return i * i; }
};

class CBGRA : public CBGR {
	public:
		ubyte a;

		CBGRA& operator= (const tRGBA& other) {
			r = other.r;
			g = other.g;
			b = other.b;
			a = other.a;
			return *this;
			}

		CBGRA& operator= (const tBGRA& other) {
			r = other.r;
			g = other.g;
			b = other.b;
			a = other.a;
			return *this;
			}

		CBGRA& operator= (const CBGR& other) {
			r = other.r;
			g = other.g;
			b = other.b;
			a = 255;
			return *this;
			}

		CBGRA& operator= (COLORREF color) {
			r = GetRValue (color);
			g = GetGValue (color);
			b = GetBValue (color);
			a = 255;
			return *this;
			}

		CBGRA& operator= (const RGBQUAD& color) {
			r = color.rgbRed;
			g = color.rgbGreen;
			b = color.rgbBlue;
			a = 255;
			return *this;
			}

		CBGRA (ubyte red = 0, ubyte green = 0, ubyte blue = 0, ubyte alpha = 255)
			: CBGR (red, green, blue), a (alpha) 
			{}
};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

class CUV {
	public:
		int u, v;
};

// 0: Bitmap, 1: TGA (RGB)
enum eTextureFormat : ubyte {
	BMP = 0,
	TGA = 1
};

typedef struct tTexture {
	uint           width, height, bufSize;
	uint           nIndex;
	int            nTexture;
	bool           bFromMod, bAnimated, bTransparent, bSuperTransparent, bFlat;
	ubyte          bCustom;
	eTextureFormat  format;
	ubyte          nFrameCount, nFrame;
	char           szName [9];
	rgbaColorf     averageColor;
} tTexture;

class CTexture {
	private:
		tTexture    m_info;         // texture descriptor info that will persist across copies
		CBGRA*      m_data;         // bitmap data for this texture
		CBGRA*      m_renderBuffer; // separate (possibly) copy of the bitmap aligned for use in 3d view
		GLuint      m_glHandle;     // handle for the OpenGL texture associated with this texture
		bool        m_bExtBuffer;   // is the bitmap data stored in an externally-owned buffer?
		bool        m_bValid;       // is the texture properly loaded and containing image data?

		mutable ubyte m_nCurrentFrame; // used for playing texture animations; not intended to be saved to file

	public:
		CTexture (CBGRA* pData = null) : m_data (pData), m_renderBuffer (null), m_glHandle (0), m_bExtBuffer (pData != null), m_bValid (false) {
			memset (&m_info, 0, sizeof (m_info));
			m_info.nTexture = -1;
			m_nCurrentFrame = 0;
			}

		~CTexture() { Release (); }

		virtual void Clear (void) {
			memset (&m_info, 0, sizeof (m_info));
			m_info.nTexture = -1;
			Release ();
		}

		int LoadFromPig (CFileManager& fp, uint nIndex, int nVersion = -1);

		bool LoadFromPog (CFileManager& fp, CPigTexture& info);

		void LoadFromResource (int nId);

		bool LoadFromFile (char* pszFile, bool bErrorOnOpenFail = true);

		bool Save (char* pszFile) const;

		double Scale (short index = -1) const;

		int Shrink (int xFactor, int yFactor);

		void ApplyAlpha (void);

		ubyte* ToBitmap (void) const;

		ubyte* ToBitmap (bool bShrink);

		void ComputeIndex (ubyte* bmIndex) const;

		inline uint Index (void) const { return m_info.nIndex; }

		inline int Id (void) const { return m_info.nTexture; }

		inline bool Transparent (void) const { return m_info.bTransparent; }

		inline uint Width () const { return m_info.width; }

		inline uint Height () const { return m_info.height; }

		inline uint Size () const { return Width () * Height (); }

		inline uint FrameHeight () const { return IsSingleImageAnimation () ? Width () : Height (); }

		inline uint FrameSize () const { return IsSingleImageAnimation () ? Width () * Width () : Size (); }

		inline uint FrameOffset () const { return IsSingleImageAnimation () ? FrameSize () * GetCurrentFrame (true) : 0; }

		inline CBGRA* Buffer (uint i = 0) { return &m_data [i]; }

		inline const CBGRA* Buffer (uint i = 0) const { return &m_data [i]; }

		void CommitBufferChanges (void);

		inline uint RenderWidth () const {
			return (m_info.format == TGA) ? m_info.width : Pow2Dim (m_info.width, m_info.height);
		}

		inline uint RenderHeight () const {
			return (m_info.format == TGA) ? RenderWidth () : Pow2Dim (m_info.width, FrameHeight ());
		}

		inline uint RenderSize (void) const { return RenderWidth () * RenderHeight (); }

		inline uint RenderOffsetX (void) const { return (RenderWidth () - Width ()) / 2; }

		inline uint RenderOffsetY (void) const { return (RenderHeight () - Height ()) / 2; }

		inline CBGRA* RenderBuffer (uint i = 0) { return &m_renderBuffer [i]; }

		inline const CBGRA* RenderBuffer (uint i = 0) const { return &m_renderBuffer [i]; }

		inline bool IsLoaded (void) const { return m_bValid; }

		bool GLCreate (bool bForce = true);

		GLuint GLBind (GLuint nTMU, GLuint nMode) const;

		void GLRelease (void);

		void DrawLine (POINT pt0, POINT pt1, CBGRA color);

		void DrawAnimDirArrows (short nTexture);

		inline bool Copy (const CTexture& src) {
			if (!Allocate (src.Size ()))
				return false;
			memcpy (&m_info, &src.m_info, sizeof (m_info));
			memcpy (m_data, src.m_data, m_info.bufSize * sizeof (m_data [0]));
			m_glHandle = 0;
			m_bValid = true;
			m_bExtBuffer = false;
			GenerateRenderBuffer ();
			return true;
			}

		int BlendTextures (short nBaseTex, short nOvlTex, int x0, int y0);

		int BlendTextures (const CTexture* pBaseTex, const CTexture* pOvlTex, short nOvlAlignment, int x0, int y0, bool bCurrentFrameOnly = true);

		inline void SetFlat (bool bFlat) { m_info.bFlat = bFlat; }

		inline bool Flat (void) const { return m_info.bFlat; } // texture or just color?

		rgbaColorf& GetAverageColor (rgbaColorf& color);

		inline void SetAverageColor (float r, float g, float b, float a = 1.0f) { 
			m_info.averageColor.r = r, m_info.averageColor.g = g, m_info.averageColor.b = b, m_info.averageColor.r = a; }

		inline void SetAverageColor (rgbaColorf& color) { m_info.averageColor = color; }

		CBGRA& operator[] (uint i) { return *Buffer (i); }

		CBGRA& operator[] (CUV uv) { return *Buffer ((Height () - uv.v - 1) * Width () + uv.u); }

		inline bool IsCustom (void) const { return m_info.bCustom > 0; }

		inline void SetCustom (void) { m_info.bCustom = 1; }

		inline bool IsTransparent (void) const { return m_info.bTransparent; }

		inline bool IsSuperTransparent (void) const { return m_info.bSuperTransparent; }


		bool IsAnimated (int nVersion = -1) const;

		inline bool MaybeAnimated (void) const { 
			if (m_info.bAnimated)
				return true; 
			if (!Format ())
				return false;
			 if (Height () <= Width ())
				 return false;
			return (Height () % Width () == 0);
			}

		bool IsSingleImageAnimation (int nVersion = -1) const;

		// true = this is a frame of a type that can be assigned to a surface in a level (usually doors)
		bool IsAssignableFrame (void) const;

		void CalculateFrameCount (void);

		inline uint FrameNumber (void) const { return m_info.nFrame; }

		inline ubyte GetFrameCount (void) const { return MaxVal ((ubyte) 1, m_info.nFrameCount); }

		inline uint FrameTime (void) const { return 100; } // data comes from HAM, just using previous value (100) for now

		const CTexture *GetParent (void) const;

		const CTexture *GetFrame (uint nFrame) const;

		inline const char *Name (void) const { return m_info.szName; }

		inline eTextureFormat Format (void) const { return m_info.format; }

		bool CreateBitmap (CBitmap **ppImage, bool bScale = false, int width = -1, int height = -1) const;

		inline void SetFrameCount (ubyte nFrameCount) { m_info.nFrameCount = nFrameCount; }

		inline void SetFrame (ubyte nFrame) { m_info.nFrame = nFrame; }

		inline void SetAnimated (bool bAnimated) { m_info.bAnimated = bAnimated; }

		// Animated preview support
		inline ubyte GetCurrentFrame (bool bUpsideDown = false) const { return bUpsideDown ? MaxVal (0, GetFrameCount () - m_nCurrentFrame - 1) : m_nCurrentFrame; }

		inline void SetCurrentFrame (ubyte nFrame) const { m_nCurrentFrame = nFrame; }

	private:
		void Release (void);

		bool Allocate (int nSize);

		void GenerateRenderBuffer ();

		void Load (CFileManager& fp, CPigTexture& info);

		bool LoadTGA (CFileManager& fp);

		bool LoadTGA (char* pszFile);

		bool LoadBMP (CFileManager& fp);

		bool SaveTGA (CFileManager& fp) const;

		bool SaveBMP (CFileManager& fp) const;
};

//------------------------------------------------------------------------

#if EXTRA_TEXTURES
class CExtraTexture : public CTexture {
public:
	CExtraTexture*	m_next;
	ushort			m_index;
};
#endif

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

void RgbFromIndex (int nIndex, PALETTEENTRY& rgb);

bool PaintTexture (CWnd *pWindow, int bkColor = -1, int texture1 = -1, int texture2 = 0, int xOffset = 0, int yOffset = 0);

bool PaintTexture (CWnd *pWindow, int bkColor = -1, 
                   const CTexture *pBaseTex = null, const CTexture *pOvlTex = null, short nOvlAlignment = 0,
                   int xOffset = 0, int yOffset = 0, bool bCurrentFrameOnly = true);

bool TGA2Bitmap (tRGBA *pTGA, ubyte *pBM, int nWidth, int nHeight);

//------------------------------------------------------------------------

#endif //__textures_h