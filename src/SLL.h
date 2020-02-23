
#ifndef __SLL_H
#define __SLL_H

#include "define.h"
#include "types.h"

// -----------------------------------------------------------------------------
// Singly Linked List

template < class _T, class _K >
class CSLLIterator;

template < class _T, class _K >
class CSLL {
	public:
		friend class CSLLIterator<_T, _K>;


		template < class _U >
		class CNode {
			public:
				CNode*	m_succ;
				_U			m_data;

			CNode () : m_succ (null) {}

			CNode* GetSucc (void) { return m_succ; }

			CNode* SetSucc (CNode* succ) { return m_succ = succ; }
			};


		CNode<_T>*	m_head;
		CNode<_T>*	m_tail;
		uint			m_length;

	private:
		CNode<_T>* Find (_K key, CNode<_T>*& pPred) {
			pPred = null;
			for (CNode<_T>* pNode = m_head; pNode; pNode = pNode->m_succ) {
				if (pNode->m_data == key)
					return pNode;
				pPred = pNode;
				}
			return null;
			}

	public:
		CSLL () : m_head (null), m_tail (null), m_length (0) {}

		~CSLL () { Destroy (); }

		inline CNode<_T>* Head (void) { return m_head; }

		inline CNode<_T>* Tail (void) { return m_tail; }

		inline uint Length (void) { return m_length; }

		void Destroy (void) {
			CNode<_T>* linkP;
			for (CNode<_T>* pNode = m_head; pNode; pNode = linkP) {
				linkP = pNode->GetSucc ();
				delete pNode;
				}
			m_head = null;
			m_tail = null;
			m_length = 0;
			}

		_T* Find (_K key) {
			for (CNode<_T>* pNode = m_head; pNode; pNode = pNode->GetSucc ()) {
				if (pNode->m_data == key)
					return &pNode->m_data;
				}
			return null;
			}

		int Index (_K key) {
			int index = 0;
			for (CNode<_T>* pNode = m_head; pNode; pNode = pNode->GetSucc (), index++) {
				if (pNode->m_data == key)
					return index;
				}
			return -1;
			}

		bool Remove (_K key) {
			CNode<_T>* pPred, * pNode = Find (key, pPred);
			if (!pNode)
				return false;
			if (pPred)
				pPred->SetSucc (pNode->GetSucc ());
			if (m_head == pNode)
				m_head = pNode->GetSucc ();
			if (m_tail == pNode)
				m_tail = pPred;
			pNode->SetSucc (null);
			delete pNode;
			--m_length;
			return true;
			}

		_T* Add (_T data) {
			CNode<_T>* pNode = new CNode<_T>;
			if (!pNode)
				return null;
			pNode->SetSucc (m_head);
			m_head = pNode;
			if (!m_tail)
				m_tail = pNode;
			++m_length;
			pNode->m_data = data;
			return &pNode->m_data;
			}

		_T* Append (_T data) {
			CNode<_T>* pNode = new CNode<_T>;
			if (!pNode)
				return null;
			if (m_tail)
				m_tail->SetSucc (pNode);
			else
				m_head = pNode;
			m_tail = pNode;
			++m_length;
			pNode->m_data = data;
			return &pNode->m_data;
			}

		_T* Append (void) {
			CNode<_T>* pNode = new CNode<_T>;
			if (!pNode)
				return null;
			if (m_tail)
				m_tail->SetSucc (pNode);
			else
				m_head = pNode;
			m_tail = pNode;
			++m_length;
			return &pNode->m_data;
			}

	inline CSLL<_T, _K>& operator= (CSLL<_T, _K>& other) {
		m_head = other.Head ();
		m_tail = other.Tail ();
		m_length = other.Length ();
		return *this;
		}
	};

// -----------------------------------------------------------------------------

template < class _T, class _K >
class CSLLIterator {
	private:
		CSLL<_T, _K>&	m_sll;
		typename CSLL<_T, _K>::CNode<_T>*	m_current;

	public:
		CSLLIterator (CSLL<_T, _K>& sll) : m_sll (sll) {}

		inline _T* Begin (void) { 
			m_current = m_sll.Head ();
			return m_current ? &m_current->m_data : null; 
			}

		inline _T* End (void) { return null; }

		inline CSLLIterator& operator++() { m_current = m_current->GetSucc (); }
		
		inline CSLLIterator& operator++(int) { 
			m_current = m_current->GetSucc (); 
			return *this;
			}
		
		inline _T* operator->() { return m_current ? &m_current->m_data : null; }
		
		inline _T* operator*() { return m_current ? &m_current->m_data : null; }
	};

// -----------------------------------------------------------------------------

#endif //__SLL_H