#ifndef __fileman_h
#define __fileman_h

#include <stdio.h>
#include <sys/types.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#ifndef _WIN32_WCE
#include <errno.h>
#endif

#include "define.h"
#include "Vector.h"
#include "Matrix.h"

// ----------------------------------------------------------------------------
// the maximum length of a filename
#define FILENAME_LEN			256

class CFilename {
	public:
		char m_buffer [FILENAME_LEN + 1];
	public:
		CFilename () { m_buffer [0] = '\0'; }
		inline CFilename& operator= (CFilename& other) { 
			memcpy (m_buffer, other.m_buffer, sizeof (m_buffer)); 
			return *this;
			}
		inline CFilename& operator= (const char* other) { 
			strncpy_s (m_buffer, sizeof (m_buffer), other, sizeof (m_buffer)); 
			return *this;
			}
		inline bool operator== (CFilename& other)
			{ return !strcmp (m_buffer, other.m_buffer); }
		inline bool operator< (CFilename& other)
			{ return strcmp (m_buffer, other.m_buffer) < 0; }
		inline bool operator> (CFilename& other)
			{ return strcmp (m_buffer, other.m_buffer) > 0; }
		inline operator const char*()
			{ return reinterpret_cast<char*> (&m_buffer [0]); }
	};


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

typedef struct tFileInfo {
	FILE*	file;
	char*	name;
	long	size;
	long	position;
} tFileInfo;

// ----------------------------------------------------------------------------

class CFileManager {
	protected:
		tFileInfo	m_info;

	public:
		CFileManager () { Init (); }
		~CFileManager () { Close (); };
		void Init (void);

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

		int Length (void);							// Returns actual size of file...
		int EoF (void);
		int Error (void);
		inline int Size (void) { return m_info.size; }
		inline char* const Name (void) { return (m_info.name == null) ? "" : m_info.name; }

		// prototypes for reading basic types from fp
		int ReadInt32 (void);
		uint ReadUInt32 (void);
		short ReadInt16 (void);
		ushort ReadUInt16 (void);
		ubyte ReadUByte (void);
		sbyte ReadSByte (void);
		char ReadChar (void);
		short ReadFixAng (void);
		float ReadFloat (void);
		double ReadDouble (void);
		void* ReadBytes (void* buffer, int bufLen);
		void ReadString (char* buffer, int bufLen);

		int ReadVector (tFixVector& v);
		int ReadVector (tDoubleVector& v);
		int ReadVector (tAngleVector& v);
		int ReadMatrix (CFixMatrix& v);
		int ReadMatrix (CDoubleMatrix& m);

		inline int Read (CFixVector& v) { return ReadVector (v.v); }
		inline int Read (CDoubleVector& v) { return ReadVector (v.v); }
		inline int Read (CAngleVector& v) { return ReadVector (v.v); }
		inline int Read (CFixMatrix& m) { return ReadMatrix (m); }
		inline int Read (CDoubleMatrix& m) { return ReadMatrix (m); }

		int WriteInt32 (int v);
		int WriteUInt32 (uint v);
		int WriteInt16 (short v);
		int WriteUInt16 (ushort v);
		int WriteByte (ubyte v);
		int WriteSByte (sbyte v);
		int WriteChar (char v);
		int WriteFloat (float v);
		int WriteDouble (double v);

		inline int Write (int& v) { return WriteInt32 (v); }
		inline int Write (uint& v) { return WriteUInt32 (v); }
//		inline int Write (int& v) { return WriteInt32 (v); }
		inline int Write (short& v) { return WriteInt16 (v); }
		inline int Write (ushort& v) { return WriteUInt16 (v); }
		inline int Write (ubyte& v) { return WriteByte (v); }
		inline int Write (sbyte& v) { return WriteSByte (v); }
		inline int Write (char& v) { return WriteChar (v); }
//		inline int Write (short& v) { return WriteInt16 (v); }
		inline int Write (float& v) { return WriteFloat (v); }
		inline int Write (double& v) { return WriteDouble (v); }

		inline void Flush (void) { fflush (m_info.file); }

		void* WriteBytes (void* buffer, int length);
		int WriteString (const char *buf);

		void WriteVector (const tAngleVector& v);
		void WriteVector (const tFixVector& v);
		void WriteVector (const tDoubleVector& v);
		inline void Write (const CAngleVector& v) { WriteVector (v.v); }
		inline void Write (const CFixVector& v) { WriteVector (v.v); }
		inline void Write (const CDoubleVector& v) { WriteVector (v.v); }

		void WriteMatrix (const CFixMatrix& m);
		void WriteMatrix (const CDoubleMatrix& m);
		inline void Write (const CFixMatrix& m) { WriteMatrix (m); }
		inline void Write (const CDoubleMatrix& m) { WriteMatrix (m); }

		int Copy (const char *pszSrc, const char *pszDest);
		ubyte* ReadData (const char *filename);
		time_t Date (const char *filename);

		static int Exist (const char *filename);	// Returns true if file exists on disk (1) or in hog (2).
		static int Delete (const char *filename);
		static int Rename (const char *oldname, const char *newname, const char *folder = null);
		static int MkDir (const char *pathname);
		static FILE* GetFileHandle (const char *filename, const char *mode);
		static void SplitPath (const char *szFullPath, char *szFolder, char *szFile, char *szExt);
		static void ChangeFilenameExtension (char *dest, const char *src, const char *new_ext);

		typedef struct {
			const char *szFilterText;
			const char *szExt;
		} tFileFilter;
		static bool RunOpenFileDialog (char *szFilename, const size_t cchFilename, const char *szFilterText, const char *szExt, HWND hWnd = 0);
		static bool RunOpenFileDialog (char *szFilename, const size_t cchFilename, const tFileFilter *filters, const DWORD nFilters, HWND hWnd = 0);
		static bool RunMultiOpenDialog (CArray <CString> &filenames, const char *szFilterText, const char *szExt, HWND hWnd = 0);
		static bool RunMultiOpenDialog (CArray <CString> &filenames, const tFileFilter *filters, const DWORD nFilters, HWND hWnd = 0);
		static bool RunSaveFileDialog (char *szFilename, const size_t cchFilename, const char *szFilterText, const char *szExt, HWND hWnd = 0);
		static bool RunSaveFileDialog (char *szFilename, const size_t cchFilename, const tFileFilter *filters, const DWORD nFilters, HWND hWnd = 0);
		static bool RunMultiSaveDialog (char *szPath, const size_t cchPath, const char *szPromptText, HWND hWnd = 0);

		inline FILE*& File (void) { return m_info.file; }

		inline bool IsOpen (void) { return File () != null; }

	private:
		inline long SetPos (long pos) {
			m_info.position = pos;
			if (m_info.size < pos)
				m_info.size = pos;
			return pos;
			}
	};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#ifdef _WIN32
char *UnicodeToAsc (char *str, const wchar_t *w_str);
wchar_t *AscToUnicode (wchar_t *w_str, const char *str);
#endif

#ifdef _WIN32_WCE
# define errno -1
# define strerror (x) "Unknown Error"
#endif

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#endif
