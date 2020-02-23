#ifndef __itemiterator_h
#define __itemiterator_h

#include "freelist.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

template < class _T >
class CGameItemIterator {
	protected:
		_T*	m_buffer;
		int	m_count;
		int	m_index;
		_T		m_null;

	public:
		CGameItemIterator (_T* buffer, int count, int start = 0) : m_index (start), m_buffer (buffer), m_count (count - start) {
			if (count < 0) {
				m_index = -count - start;
				m_count = -count;
				}
			else {
				m_index = start;
				m_count = count - start;
				}
			}

#if USE_FREELIST

		//prefix increment
		inline _T& operator++ () { 
			while (!end () && (m_buffer [++m_index].Index () < 0))
				;
			--m_count;
			return &m_buffer [m_index]; 
			}

		// postfix increment
		inline _T& operator++ (int) { 
			while (!end () && (m_buffer [m_index++].Index () < 0))
				;
			--m_count;
			return &m_buffer [m_index - 1]; 
			}

		// prefix decrement
		inline _T& operator-- () { 
			while (!end () && (m_buffer [--m_index].Index () < 0))
				;
			--m_count;
			return m_buffer [m_index]; 
			}

		// postfix decrement
		inline _T& operator-- (int) { 
			while (!end () && (m_buffer [m_index--].Index () < 0))
				;
			--m_count;
			return m_buffer [m_index - 1]; 
			}

#else //USE_FREELIST

		//prefix increment
		inline _T& operator++ () { 
			if (m_count <= 0)
				return m_null;
			--m_count;
			return m_buffer [++m_index];
			}

		// postfix increment
		inline _T& operator++ (int) { 
			if (m_count <= 0)
				return m_null;
			--m_count;
			return m_buffer [m_index++];
			}

		// prefix decrement
		inline _T& operator-- () { 
			if (m_count <= 0)
				return m_null;
			--m_count;
			return m_buffer [--m_index];
			}

		// postfix decrement
		inline _T& operator-- (int) { 
			if (m_count <= 0)
				return m_null;
			--m_count;
			return m_buffer [m_index--];
			}

#endif // USE_FREELIST

		inline CGameItemIterator& operator= (int i) { 
			m_index = i;
			return *this;
			}

		inline const bool end (void) { return m_count == 0; }

		inline operator bool() { return !end (); }

		inline const bool operator== (int i) { return m_index == i; }

		inline const bool operator!= (int i) { return m_index != i; }

		inline _T* operator-> () { return /*end () ? null :*/ &m_buffer [m_index]; }

		inline _T& operator* () { return /*end () ? m_null :*/ m_buffer [m_index]; }

		inline const int Index (void) { return m_index; }
	};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#endif // __itemiterator_h

