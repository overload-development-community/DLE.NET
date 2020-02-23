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
		byte*				m_custom;
		byte*				m_default;	
		tBMIInfo			m_bmi;
		CPalette*		m_render;
		LPLOGPALETTE	m_dlcLog;
		PALETTEENTRY*	m_colorMap;

	public:
		void Load (void);
		int LoadCustom (CFileManager& fp, long size);
		int SaveCustom (CFileManager& fp);
		void FreeCustom (void);
		void FreeDefault (void);
		void FreeRender (void);
		inline void Release (void) {
			FreeCustom ();
			FreeDefault ();
			FreeRender ();
			}
		inline void Reload (void) {
			Release ();
			Current ();
			}

		inline byte* Custom () { return m_custom; }
		inline byte* Default () { return m_default; }
		inline CPalette* Render () { return m_render; }
		inline PALETTEENTRY* ColorMap () { return m_colorMap; }
		byte* Current (void);
		BITMAPINFO* BMI (void) { return Current () ? (BITMAPINFO*) &m_bmi : null; }

		CPaletteManager () : m_custom(null), m_default(null) {}
		~CPaletteManager () { Release (); }

	private:
		const char* Resource (void);
		byte FadeValue (byte c, int f);
		void SetupBMI (byte* palette);
		short SetupRender (byte* palette);
		byte* LoadDefault (void);
		void Encode (byte* palette);
		void Decode (byte* palette);

};

extern CPaletteManager paletteManager;

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

#endif //__palette_h