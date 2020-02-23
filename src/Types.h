#ifndef __types_h
#define __types_h

#include "define.h"
#include "GameItem.h"
#include "FileManager.h"
#include "glew.h"

# pragma pack(push, packing)
# pragma pack(1)

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

class CSideKey {
	public:
		short	m_nSegment;
		short	m_nSide;

		CSideKey (short nSegment = -1, short nSide = -1) : m_nSegment(nSegment), m_nSide(nSide) {}

		CSideKey (CSideKey& other) : m_nSegment (other.m_nSegment), m_nSide (other.m_nSide) {}

		inline bool operator == (CSideKey& other) { return (m_nSegment == other.m_nSegment) && (m_nSide == other.m_nSide); }
	
		inline bool operator != (CSideKey& other) { return (m_nSegment != other.m_nSegment) || (m_nSide != other.m_nSide); }
	
		inline bool operator < (CSideKey& other) { return (m_nSegment < other.m_nSegment) || ((m_nSegment == other.m_nSegment) && (m_nSide < other.m_nSide)); }
	
		inline bool operator <= (CSideKey& other) { return (m_nSegment < other.m_nSegment) || ((m_nSegment == other.m_nSegment) && (m_nSide <= other.m_nSide)); }
	
		inline bool operator > (CSideKey& other) { return (m_nSegment > other.m_nSegment) || ((m_nSegment == other.m_nSegment) && (m_nSide > other.m_nSide)); }
	
		inline bool operator >= (CSideKey& other) { return (m_nSegment > other.m_nSegment) || ((m_nSegment == other.m_nSegment) && (m_nSide >= other.m_nSide)); }

		void Read (CFileManager* fp) {
			m_nSegment = fp->ReadInt16 ();
			m_nSide = fp->ReadInt16 ();
			}

		void Write (CFileManager* fp) {
			fp->Write (m_nSegment);
			fp->Write (m_nSide);
			}

		void Clear (void) {
			m_nSegment = m_nSide = -1;
			}
	};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

typedef struct tUVL {
public:
	double u, v;
	uint l; 
} tUVL;

class CUVL;

class CUVL /*: public tUVL */{
public:
	double u, v;
	uint l; 

public:
	inline void Read (CFileManager* fp) {
		u = double (fp->ReadInt16 ()) / 2048.0;
		v = double (fp->ReadInt16 ()) / 2048.0;
		l = uint (fp->ReadUInt16 ());
		}

	inline void Write (CFileManager* fp) {
		fp->WriteInt16 (short (Round (u * 2048.0)));
		fp->WriteInt16 (short (Round (v * 2048.0)));
		ushort h = ushort ((l > 0xFFFF) ? 0xFFFF : l);
		fp->Write (h);
		}

	explicit CUVL (double u = 0.0, double v = 0.0) : u (u), v (v), l (0) {}

	CUVL& operator= (CUVL other) {
		u = other.u;
		v = other.v;
		l = other.l;
		return *this;
		}

	inline void Clear (void) { u = v = 0.0, l = 0; }

	inline void Set (short _u, short _v, short _l) { u = _u, v = _v, l = _l; }

	inline void GLColor (bool bShaded, float alpha = 1.0f) {
		float brightness = bShaded ? X2F (l) : 1.0f;
		glColor4f (brightness, brightness, brightness, alpha);
		}

	inline void GLTexCoord (void) { glTexCoord2d (u, v); }

	inline CUVL operator+ (CUVL& other) { return CUVL (u + other.u, v + other.v); }

	inline CUVL operator- (CUVL& other) { return CUVL (u - other.u, v - other.v); }

	inline CUVL& operator+= (CUVL other) { 
		u += other.u;
		v += other.v;
		return *this;
		}

	inline CUVL operator-= (CUVL other) {
		u -= other.u;
		v -= other.v;
		return *this;
		}

	inline CUVL operator* (double factor) { return CUVL (u * factor, v * factor); }

	inline double Mag (void) { return sqrt (u * u + v * v); }

	inline CUVL& Normalize (void) {
		double m = Mag ();
		if (m != 0.0) {
			u /= m;
			v /= m;
			}
		return *this;
		}

	void Rotate (double angle);
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

typedef struct rgbColor {
	ubyte	r, g, b;
} rgbColor;

typedef struct rgbColord {
	double r, g, b;
} rgbColord;

typedef struct rgbColorf {
	float	r, g, b;
} rgbColorf;

typedef struct rgbaColorf {
	float r, g, b, a;
} rgbaColorf;

// -----------------------------------------------------------------------------

typedef struct tColor {
	ubyte		index;
	rgbColord	color;
} tColor;

// -----------------------------------------------------------------------------

class CColor : public CGameItem {
	public:
		tColor	m_info;

		void Reset (void);

		void Read (CFileManager& fp, bool bFlag = false);

		void Write (CFileManager& fp, bool bFlag = false);

		virtual void Clear (void) { memset (&m_info, 0, sizeof (m_info)); }

		virtual CGameItem* Clone (void);

		virtual void Backup (eEditType editType = opModify);

		virtual CGameItem* Copy (CGameItem* pDest);

		CColor (eItemType itemType = itUndefined) : CGameItem (itemType) {}

		inline ubyte Red (void) { return (ubyte) (m_info.color.r * 255); }

		inline ubyte Green (void) { return (ubyte) (m_info.color.g * 255); }

		inline ubyte Blue (void) { return (ubyte) (m_info.color.b * 255); }
};


class CFaceColor : public CColor {
	public:
		CFaceColor () : CColor (itFaceColor) {}

		inline const CFaceColor& operator= (const CColor& other) { 
			m_info = other.m_info; 
			return *this;
			}
};

class CTextureColor : public CColor {
	public:
		CTextureColor () : CColor (itTextureColor) {}

		inline const CTextureColor& operator= (const CColor& other) { 
			m_info = other.m_info; 
			return *this;
			}

		inline const bool operator== (const CColor& other) { 
			return memcmp (&m_info, &other.m_info, sizeof (m_info)) == 0;
			}

		inline const bool operator!= (const CColor& other) { 
			return memcmp (&m_info, &other.m_info, sizeof (m_info)) != 0;
			}
};

class CVertexColor : public CColor {
	public:
		CVertexColor () : CColor (itVertexColor) {}

		inline const CVertexColor& operator= (const CColor& other) { 
			m_info = other.m_info; 
			return *this;
			}
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#if 0

class CBaseLevelHeader {
	public:
		virtual void* Info (void) = 0;
		virtual int Size (void) = 0;
		virtual void SetSize (int size) = 0;
		virtual int FileSize (void) = 0;
		virtual char* Name (void) = 0;
		virtual int NameSize (void) = 0;

		void Reset (void) {
			*Name () = '\0';
			SetSize (0);
			}

		int Read (CFileManager* fp) {
			if (fp->Read (Info (), Size (), 1) != 1)
				return 0;
			if ((FileSize () < 0) || (FileSize () > 10000000L))
				return 0;
			Name () [NameSize () - 1] = '\0';
			_strlwr_s (Name (), NameSize ());
			return 1;
			}

		int Write (CFileManager* fp) { return fp->Write (Info (), Size (), 1) == 1; }
	};

# pragma pack(push)
# pragma pack(1)

class CStdLevelInfo {
	public:
		char name [13];
		int size;
};

#pragma pack(pop)

class CStdLevelHeader : public CBaseLevelHeader {
	protected:
		CStdLevelInfo	m_info;

	public:
		CStdLevelHeader () { m_info.size = 0, m_info.name [0] = '\0'; }
		virtual void* Info (void) { return (void*) &m_info; }
		virtual int Size (void) { return sizeof (m_info); }
		virtual void SetSize (int size) { m_info.size = size; }
		virtual int FileSize (void) { return m_info.size; }
		virtual char* Name (void) { return m_info.name; }
		virtual int NameSize (void) { return sizeof (m_info.name); }
	};

# pragma pack(push)
# pragma pack(1)

class CExtLevelInfo {
	public:
		char name [256];
		int size;
};

#pragma pack(pop)

class CExtLevelHeader : public CBaseLevelHeader {
	protected:
		CExtLevelInfo	m_info;

	public:
		CExtLevelHeader () { m_info.size = 0, m_info.name [0] = '\0'; }
		virtual void* Info (void) { return (void*) &m_info; }
		virtual int Size (void) { return sizeof (m_info); }
		virtual void SetSize (int size) { m_info.size = size; }
		virtual int FileSize (void) { return m_info.size; }
		virtual char* Name (void) { return m_info.name; }
		virtual int NameSize (void) { return sizeof (m_info.name); }
	};

class CLevelHeader {
	protected:
		CBaseLevelHeader*	m_header;

	public:
		explicit CLevelHeader (CBaseLevelHeader* header = null) : m_header (header) {}
		~CLevelHeader () { if (m_header) { delete m_header; m_header = null; } }

	inline int Size (void) { return m_header ? m_header->Size () : 0; }
	inline void SetSize (int size) { if (m_header) m_header->SetSize (size); }
	inline int FileSize (void) { return m_header ? m_header->FileSize () : 0; }
	inline char* Name (void) { return m_header ? m_header->Name () : null; }
	inline int NameSize (void) { return m_header ? m_header->NameSize () : 0; }
	inline int Read (CFileManager* fp) { return m_header ? m_header->Read (fp) : 0; }
	inline int Write (CFileManager* fp) { return m_header ? m_header->Write (fp) : 0; }
	};


class CLevelHeaderFactory {
	public:
		CBaseLevelHeader* Create (int nType) {
			if (nType == 1)
				return new CExtLevelHeader ();
			if (nType == 0)
				return new CStdLevelHeader ();
			return null;
		}
	};

#else

class CLevelHeader {
	public:
		int	m_size;
		char	m_name [13];
		char	m_longName [256];
		int	m_bExtended;

	public:
		explicit CLevelHeader (int bExtended = 0) : m_size (0), m_bExtended (bExtended) { m_name [0] = '\0'; m_longName [0] = '\0'; }

		inline char* Name (void) { return m_bExtended ? m_longName : m_name; }
		inline int NameSize (void) { return m_bExtended ? sizeof (m_longName) : sizeof (m_name); }
		inline int Size (void) { return sizeof (m_size) + sizeof (m_name) + m_bExtended * sizeof (m_longName); }
		inline int FileSize (void) { return m_bExtended ? -m_size : m_size; }
		inline void SetFileSize (int size) { m_size = m_bExtended ? -size : size; }
		inline int Extended (void) { return m_size < 0; }

		int Read (CFileManager* fp);
		int Write (CFileManager* fp, int bExtended = -1);
	};

#endif

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define MAX_LEVELS	1000

class CMissionData {
	public:
		char	missionName [256];
		char  missionInfo [8][256];
		int	authorFlags [2];
		int	missionType;
		int	missionFlags [6];
		int	customFlags [3];
		char	levelList [MAX_LEVELS][260];	//18 == ########.###,####'\0' == levlname.ext,secr
		char	comment [4000];
		int   numLevels;
		int	numSecrets;
	};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

class CVertex;
class CSegment;
class CSide;
class CWall;
class CTrigger;
class CObject;
class CSegmentManager;
class CWallManager;
class CTriggerManager;
class CMine;

extern CMine* theMine;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#endif // __types_h

