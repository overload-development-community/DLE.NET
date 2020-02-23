#ifndef __freelist_h
#define __freelist_h

# pragma pack(push, packing)
# pragma pack(1)

#define USE_FREELIST 0

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#if USE_FREELIST

template <class _T>
class CFreeList {
	protected:
		int*	m_freeList;
		int	m_size;
		int	m_count;
		_T*	m_buffer;

	public:
		CFreeList () : m_buffer (null), m_freeList (null), m_size (0), m_count (0) {}

		~CFreeList () { Destroy (); }
		
		bool Create (_T* buffer, int size) {
			m_freeList = new int [size];
			if (m_freeList == null)
				return false;
			m_buffer = buffer;
			m_size = size;
			Reset ();
			return true;
			}

		void Reset (void) {
			if (m_freeList != null) {
				m_count = 0;
				for (int i = m_size; m_count < m_size; m_count++) {
					m_freeList [m_count] = --i;
					m_buffer [m_count].Index () = -1;
					}
				}
			}

		void Destroy (void) {
			if (m_freeList) {
				delete[] m_freeList;
				m_freeList = null;
				}
			m_size = 0;
			}

		inline int Free (void) { return m_count; }

		inline bool Empty (void) { return m_count == 0; }

		inline int Get (void) { 
			if (m_count <= 0) 
				return -1;
			int i = m_freeList [--m_count];
			m_buffer [i].Index () = i;
			return i;
			}

		inline void Put (int i) { 
			if (m_count < m_size) {
				m_buffer [i].Index () = -1;
				m_freeList [m_count++] = i;
				}
			}

		inline const int operator-- () { return Get (); }

		inline const int operator-- (int) { return Get (); }

		inline const void operator+= (int i) { Put (i); }
	};

#define FREELIST(_T) CFreeList<_T> m_free;

#else // USE_FREELIST

#define FREELIST(_T)

#endif // USE_FREELIST

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#endif // __freelist_h

