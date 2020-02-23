#ifndef __memfile_h
#define __memfile_h

#include "FileManager.h"

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class CMemoryFile : public CFileManager {
	private:
		ubyte* m_buffer;
		bool  m_bLocalBuffer;

	public:
		virtual int Open (const char *filename, const char *mode);
		virtual size_t Read (void *buf, size_t elsize, size_t nelem);
		virtual int Close (bool bReset = true);
		virtual int Seek (size_t offset, int whence = SEEK_SET);
		virtual int Tell (void);
		virtual char *GetS (char *buf, size_t n);
		virtual int Write (const void *buf, int elsize, int nelem);
		virtual int GetC (void);
		virtual int PutC (int c);
		virtual int PutS (const char *str);

		int Open (ubyte* buffer, long bufSize);

		int Load (CFileManager* fp, size_t size);

		int Load (const ubyte* buffer, size_t size);

		bool Create (size_t size);

		inline bool Empty (void) { return m_info.size == 0; }

		CMemoryFile () : CFileManager (), m_buffer (null), m_bLocalBuffer (false) {}

		inline void SetBuffer (ubyte * buffer) { m_buffer = buffer; }

		~CMemoryFile () { Close (); }
	};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#endif
