#ifndef _CARRAY_H
#define _CARRAY_H

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <string.h>
#include <stdlib.h>

#ifndef DBG
#	ifdef _DEBUG
#		define DBG 1
#	else
#		define DBG 0
#	endif
#endif

#define DBG_ARRAYS	DBG

#include "cquicksort.h"

void ArrayError (const char* pszMsg);

//-----------------------------------------------------------------------------

template < class _T > 
class CDynamicArray : public CQuickSort < _T > {

	template < class _U > 
	class CArrayData {
		public:
			_U*		buffer;
			_U			nil;
			uint		length;
			uint		pos;
			int		nMode;
			bool		bWrap;
			};

	protected:
		CArrayData<_T>	m_data;

	public:
		template < class _V >
		class Iterator {
			private:
				_V*			m_start;
				_V*			m_end;
				_V*			m_p;
				CDynamicArray<_V>&	m_a;
			public:
				Iterator () : m_p (null) {}
				Iterator (CDynamicArray<_V>& a) : m_a (a), m_p (null) {}
				operator bool() const { return m_p != null; }
				_V* operator*() const { return m_p; }
				Iterator& operator++() { 
					if (m_p) {
						if (m_p < m_end)
							m_p++;
						else
							m_p = null;
						}
					return *this;
					}
				Iterator& operator--() { 
					if (m_p) {
						if (m_p > m_end)
							m_p--;
						else
							m_p = null;
						}
					return *this;
					}
				_V* Start (void) { m_p = m_start = m_a.Start (); m_end = m_a.End (); }
				_V* End (void) { m_p = m_start = m_a.End (); m_end = m_a.Start (); }
			};

		CDynamicArray () { 
			Init (); 
			}
		
		CDynamicArray (uint nLength) { 
			Init (); 
			Create (nLength);
			}

		template <size_t _N>
		CDynamicArray (_T const (&source) [_N]) : CDynamicArray (_N) { operator= ((_T*)source); }
		
		~CDynamicArray() { Destroy (); }
		
		void Init (void) { 
			m_data.buffer = reinterpret_cast<_T *> (null); 
			m_data.length = 0;
			m_data.pos = 0;
			m_data.nMode = 0;
			m_data.bWrap = false;
			memset (&m_data.nil, 0, sizeof (m_data.nil));
			}

		void Clear (ubyte filler = 0, uint count = 0xffffffff) { 
#if DBG_ARRAYS
			if ((count != 0xffffffff) && (count > 1000000)) {
				count = count;
				ArrayError ("array overflow\n");
				}
			if ((count == 0xffffffff) && (m_data.length > 512 * 512 * 16 * 4)) {
				count = count;
				ArrayError ("array overflow\n");
				}
#endif
			if (m_data.buffer) 
				memset (m_data.buffer, filler, sizeof (_T) * ((count < m_data.length) ? count : m_data.length)); 
			}
		
		inline bool IsElement (_T* elem, bool bDiligent = false) {
			if (!m_data.buffer || (elem < m_data.buffer) || (elem >= m_data.buffer + m_data.length))
				return false;	// no buffer or element out of buffer
			if (bDiligent) {
				uint i = static_cast<uint> (reinterpret_cast<ubyte*> (elem) - reinterpret_cast<ubyte*> (m_data.buffer));
				if (i % sizeof (_T))	
					return false;	// elem in the buffer, but not properly aligned
				}
			return true;
			}

#if DBG_ARRAYS
		inline int Index (_T* elem) { 
			if (IsElement (elem))
				return static_cast<int> (elem - m_data.buffer); 
			ArrayError ("invalid array index\n");
			return -1;
			}
#else
		inline uint Index (_T* elem) { return uint (elem - m_data.buffer); }
#endif

#if DBG_ARRAYS
		inline _T* Pointer (uint i) const { 
			if (!m_data.buffer || (i >= m_data.length)) {
				ArrayError ("invalid array handle or index\n");
				return null;
				}
			return m_data.buffer + i; 
			}
#else
		inline _T* Pointer (uint i) const { return m_data.buffer + i; }
#endif

		void Destroy (void) { 
			if (m_data.buffer) {
				if (!m_data.nMode) {
					delete[] m_data.buffer;
#if DBG_ARRAYS
					m_data.buffer = reinterpret_cast<_T *> (null); 
#endif
					}
				Init ();
				}
			}
			
		_T *Create (uint length) {
			if (m_data.length != length) {
				Destroy ();
				try {
					if ((m_data.buffer = new _T [length]))
						m_data.length = length;
					}
				catch(...) {
#if DBG_ARRAYS
					ArrayError ("invalid buffer size\n");
#endif
					m_data.buffer = null;
					}
				}
			return m_data.buffer;
			}
			
		inline _T* Buffer (uint i = 0) { return m_data.buffer + i; }
		
		void SetBuffer (_T *buffer, int nMode = 0, uint length = 0xffffffff) {
			if (m_data.buffer != buffer) {
				if (!(m_data.buffer = buffer))
					Init ();
				else {
					if (length != 0xffffffff)
						m_data.length = length;
					m_data.nMode = nMode;
					}
				}
			}
			
		_T* Resize (uint length, bool bCopy = true) {
			if (m_data.nMode == 2)
				return m_data.buffer;
			if (!m_data.buffer)
				return Create (length);
			_T* p;
			try {
				p = new _T [length];
				}
			catch(...) {
#if DBG_ARRAYS
				ArrayError ("invalid buffer size\n");
#endif
				p = null;
				}
			if (!p)
				return m_data.buffer;
			if (bCopy) {
				memcpy (p, m_data.buffer, ((length > m_data.length) ? m_data.length : length) * sizeof (_T)); 
				Clear (); // hack to avoid d'tors
				}
			m_data.length = length;
			if (length > 0)
				m_data.pos %= length;
			else
				m_data.pos = 0;
			delete[] m_data.buffer;
			return m_data.buffer = p;
			}

		inline uint Length (void) const { return m_data.length; }

		inline _T* Current (void) const { return m_data.buffer ? m_data.buffer + m_data.pos : null; }

		inline size_t Size (void) const { return m_data.length * sizeof (_T); }
#if DBG_ARRAYS
		inline _T& operator[] (uint i) { 
			if (m_data.buffer && (i < m_data.length))
				return m_data.buffer [i];
			if (i == m_data.length)
				return m_data.nil; 
			else {
				ArrayError ("invalid array handle or index\n");
				return m_data.nil; 
				}
			}
#else
		inline _T& operator[] (uint i) { return m_data.buffer [i]; }
#endif

		inline _T& operator= (CDynamicArray<_T>& source) { return Copy (source); }

		inline _T& operator= (_T* source) { 
			if (m_data.buffer) 
				memcpy (m_data.buffer, source, m_data.length * sizeof (_T)); 
			return m_data.buffer [0];
			}

		_T& Copy (CDynamicArray<_T>& source, uint offset = 0) { 
			if (((static_cast<int> (m_data.length)) >= 0) && (static_cast<int> (source.m_data.length) > 0)) {
				if ((m_data.buffer && (m_data.length >= source.m_data.length + offset)) || Resize (source.m_data.length + offset, false)) {
					memcpy (m_data.buffer + offset, source.m_data.buffer, ((m_data.length - offset < source.m_data.length) ? m_data.length - offset : source.m_data.length) * sizeof (_T)); 
					}
				}
			return m_data.buffer [0];
			}

		inline _T& operator+ (CDynamicArray<_T>& source) { 
			uint offset = m_data.length;
			if (m_data.buffer) 
				Resize (m_data.length + source.m_data.length);
			return Copy (source, offset);
			}

		inline bool operator== (CDynamicArray<_T>& other) const { 
			return (m_data.length == other.m_data.length) && !(m_data.length && memcmp (m_data.buffer, other.m_data.buffer)); 
			}

		inline bool operator!= (CDynamicArray<_T>& other) const { 
			return (m_data.length != other.m_data.length) || (m_data.length && memcmp (m_data.buffer, other.m_data.buffer)); 
			}

		inline _T* Start (void) const { return m_data.buffer; }

		inline _T* End (void) const { return (m_data.buffer && m_data.length) ? m_data.buffer + m_data.length - 1 : null; }

		inline _T* operator++ (void) { 
			if (!m_data.buffer)
				return null;
			if (m_data.pos < m_data.length - 1)
				m_data.pos++;
			else if (m_data.bWrap) 
				m_data.pos = 0;
			else
				return null;
			return m_data.buffer + m_data.pos;
			}

		inline _T* operator-- (void) { 
			if (!m_data.buffer)
				return null;
			if (m_data.pos > 0)
				m_data.pos--;
			else if (m_data.bWrap)
				m_data.pos = m_data.length - 1;
			else
				return null;
			return m_data.buffer + m_data.pos;
			}

#if DBG_ARRAYS

		inline _T* operator+ (uint i) { 
			if (m_data.buffer && (i < m_data.length))
				return m_data.buffer + i;
			if (i == m_data.length)
				return null;
			else {
				ArrayError ("invalid array handle or index\n");
				return  null; 
				}
			}

#else

		inline _T* operator+ (uint i) { return m_data.buffer ? m_data.buffer + i : null; }

#endif

		inline _T* operator- (uint i) { return m_data.buffer ? m_data.buffer - i : null; }

		CDynamicArray<_T>& ShareBuffer (CDynamicArray<_T>& child) {
			memcpy (&child.m_data, &m_data, sizeof (m_data));
			if (!child.m_data.nMode)
				child.m_data.nMode = 1;
			return child;
			}

		inline bool operator! () const { return m_data.buffer == null; }

		inline uint Pos (void) const { return m_data.pos; }

		inline void Pos (uint pos) { m_data.pos = pos % m_data.length; }

		size_t Read (CFileManager& fp, uint nCount = 0, uint nOffset = 0) { 
			if (!m_data.buffer)
				return -1;
			if (nOffset >= m_data.length)
				return -1;
			if (!nCount)
				nCount = m_data.length - nOffset;
			else if (nCount > m_data.length - nOffset)
				nCount = m_data.length - nOffset;
			return int (fp.Read (m_data.buffer + nOffset, sizeof (_T), nCount));
			}

		size_t Write (CFileManager& fp, uint nCount = 0, uint nOffset = 0) { 
			if (!m_data.buffer)
				return -1;
			if (nOffset >= m_data.length)
				return -1;
			if (!nCount)
				nCount = m_data.length - nOffset;
			else if (nCount > m_data.length - nOffset)
				nCount = m_data.length - nOffset;
			return int (fp.Write (m_data.buffer + nOffset, sizeof (_T), nCount));
			}

		inline void SetWrap (bool bWrap) { m_data.bWrap = bWrap; }

		inline void SortAscending (int left = 0, int right = -1) { 
			if (m_data.buffer) 
				CQuickSort<_T>::SortAscending (m_data.buffer, left, (right >= 0) ? right : m_data.length - 1); 
				}

		inline void SortDescending (int left = 0, int right = -1) {
			if (m_data.buffer) 
				CQuickSort<_T>::SortDescending (m_data.buffer, left, (right >= 0) ? right : m_data.length - 1);
			}

		inline size_t Find (_T key) const {
			for (uint i = 0; i < m_data.length; i++)
				if (key == m_data.buffer [i])
					return i;
			return -1;
			}

		inline size_t FindSorted (_T key) {
			size_t i = 0, j = size_t (m_data.length);
			_T t;
			do {
				m = (i + j) / 2;
				t = m_data.buffer [m];
				if (key < t)
					r = m - 1;
				else if (key > t)
					l = m + 1;
				else
					return m;
			} while (i <= j);
			return -1;
			}

#ifdef _WIN32
		inline void SortAscending (comparator compare, int left = 0, int right = -1) {
			if (m_data.buffer) 
				CQuickSort<_T>::SortAscending (m_data.buffer, left, (right >= 0) ? right : m_data.length - 1, compare);
			}

		inline void SortDescending (comparator compare, int left = 0, int right = -1) {
			if (m_data.buffer) 
				CQuickSort<_T>::SortDescending (m_data.buffer, left, (right >= 0) ? right : m_data.length - 1, compare);
			}
#endif
	};

//-----------------------------------------------------------------------------

inline int operator- (char* v, CDynamicArray<char>& a) { return a.Index (v); }
inline int operator- (ubyte* v, CDynamicArray<ubyte>& a) { return a.Index (v); }
inline int operator- (short* v, CDynamicArray<short>& a) { return a.Index (v); }
inline int operator- (ushort* v, CDynamicArray<ushort>& a) { return a.Index (v); }
inline int operator- (int* v, CDynamicArray<int>& a) { return a.Index (v); }
inline int operator- (uint* v, CDynamicArray<uint>& a) { return a.Index (v); }

class CCharArray : public CDynamicArray<char> {
	public:
		inline char* operator= (const char* source) { 
			uint l = uint (strlen (source) + 1);
			if ((l > this->m_data.length) && !this->Resize (this->m_data.length + l))
				return null;
			memcpy (this->m_data.buffer, source, l);
			return this->m_data.buffer;
		}
};

//class CByteArray : public CDynamicArray<ubyte> {};
//class CShortArray : public CDynamicArray<short> {};
//class CUShortArray : public CDynamicArray<ushort> {};
//class CIntArray : public CDynamicArray<int> {};
//class CUIntArray : public CDynamicArray<uint> {};
//class CFloatArray : public CDynamicArray<float> {};

//-----------------------------------------------------------------------------

template < class _T, uint length > 
class CStaticArray : public CDynamicArray < _T > {

	template < class _U, uint _length > 
	class CStaticArrayData {
		public:
			_U		buffer [_length];
			};

	protected:
		CStaticArrayData< _T, length > m_staticData;

	public:
		CStaticArray () { Create (length); }

		_T *Create (uint _length) { 
			this->SetBuffer (m_staticData.buffer, 2, _length); 
			return m_data.buffer;
			}
		void Destroy (void) { }
	};

//-----------------------------------------------------------------------------


#endif //_CARRAY_H
