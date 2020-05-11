#ifndef __resourcemanager_h
#define __resourcemanager_h

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

class CResource {
private:
	HGLOBAL		m_handle;
	ubyte*			m_data;
	size_t		m_size;

public:
	ubyte* Load (const char* szName, const char* szCategory = "RC_DATA");

	inline ubyte* Load (const int nId, const char* szCategory = "RC_DATA") { return Load (MAKEINTRESOURCE (nId), szCategory); }

	inline void Unload (void) {
		if (m_handle) {
			FreeResource (m_handle);
			m_handle = 0;
			}
		}

	inline size_t Size (void) { return m_size; }

	inline ubyte* Data (void) { return m_data; }

	CResource () : m_handle (0), m_data(null), m_size(0) {}

	~CResource () { Unload (); }

};

//------------------------------------------------------------------------

class CStringResource {
private:
	char			m_value [1024];
	HINSTANCE	m_hInst;

public:
	CStringResource () { 
		m_value [0] = 0; 
		m_hInst = AfxGetInstanceHandle ();
		}

	char* Load (int nResource);
	
	inline char* Value (void) { return m_value; }
	
	inline size_t Size (void) { return sizeof (m_value); }
	
	inline size_t Length (void) { return strlen (m_value); }
	
	inline void Clear (void) { m_value [0] = 0; }
};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

#endif //__resourcemanager_h