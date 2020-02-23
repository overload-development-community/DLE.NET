#ifndef __palette_h
#define __palette_h

#include "resourcemanager.h"

//------------------------------------------------------------------------
int Luminance (int r, int g, int b);

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

typedef struct tBMIInfo {
	BITMAPINFOHEADER header;
	RGBQUAD colors [256];
} tBMIInfo;

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

class CPaletteManager {
	private:
		char				m_name [15];
		ubyte				m_rawData [37 * 256];
		CBGR				m_custom [256];
		CBGR				m_default [256];	
		CBGR				m_superTransp;
		rgbColorf		m_superTranspf;
		ubyte				m_fadeTables [34 * 256];
		ubyte				m_clut [0xFFFFFF]; // RGB -> palette
		uint				m_clutValid [0xFFFFFF / 32];
		tBMIInfo			m_bmi;
		CPalette*		m_render;
		LPLOGPALETTE	m_dlcLog;
		PALETTEENTRY*	m_colorMap;
		bool				m_bHaveDefault;
		bool				m_bHaveCustom;

	public:
		void Load (void);

		void LoadName (CFileManager* fp);

		inline void SetName (const char* name) { strncpy_s (m_name, sizeof (m_name), name, sizeof (m_name)); }

		int LoadCustom (CFileManager* fp, long size);

		int SaveCustom (CFileManager* fp);

		void FreeRender (void);

		inline void Release (void) {
			FreeRender ();
			m_bHaveDefault = false;
			m_bHaveCustom = false;
			}
		inline void Reload (char* pszName = null) {
			Release ();
			Current (0, pszName);
			}

		inline ubyte* FadeTable (void) { return m_fadeTables; }

		inline char* Name (void) { return m_name; }

		inline CBGR* Custom (void) { return m_bHaveCustom ? m_custom : null; }

		inline CBGR* Default (void) { return m_bHaveDefault ? m_default : null; }

		inline CPalette* Render (void) { return m_render; }

		inline PALETTEENTRY* ColorMap (void) { return m_colorMap; }

		CBGR* Current (int i = 0, char* pszName = null);

		BITMAPINFO* BMI (void) { return Current () ? (BITMAPINFO*) &m_bmi : null; }

		ubyte ClosestColor (CBGR& color, bool bAllowTransp = true);

		inline CBGR SuperTranspKey (void) { return m_superTransp; }

		inline rgbColorf* SuperTranspKeyf (void) { return &m_superTranspf; }

		inline bool SuperTransp (const CBGR& color) { return color == m_superTransp; }

		CPaletteManager () : m_bHaveDefault (false), m_bHaveCustom (false) { *m_name = '\0'; }

		~CPaletteManager () { Release (); }

		inline void ResetCLUT (void) { memset (m_clutValid, 0, sizeof (m_clutValid)); }

		int NumAvailablePalettes (int nVersion);

		const char* AvailablePaletteName (int nPalette, int nVersion);

	private:
		const char* SelectResource (char* pszName);

		inline ubyte FadeValue (ubyte c, int f);

		void SetupBMI (CBGR* palette);

		short SetupRender (CBGR* palette);

		CBGR* LoadDefault (char* pszName = null);

		void Decode (CBGR* dest);

		void Encode (CBGR* src);

		void CreateFadeTable (void);
};

extern CPaletteManager paletteManager;

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

#endif //__palette_h