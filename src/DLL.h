
#ifndef __DLL_H
#define __DLL_H

#include "define.h"
#include "types.h"

// -----------------------------------------------------------------------------
// Singly Linked List

template < class _T, class _K >
class CDLLIterator;

template < class _T, class _K >
class CDLL {
	public:
		friend class CDLLIterator<_T, _K>;

		template < class _U >
		class CNode {
			public:
				CNode*	m_pred;
				CNode*	m_succ;
				_U			m_data;

			CNode () : m_pred (null), m_succ (null) {}

			inline CNode * GetPred (void) { return m_pred; }

			inline CNode * GetSucc (void) { return m_succ; }

			inline CNode * SetPred (CNode * pred) { return m_pred = pred; }

			inline CNode * SetSucc (CNode * succ) { return m_succ = succ; }

			inline void InsertBefore (CNode * node) {
				if (SetPred (node->GetPred ()) != null)
					GetPred ()->SetSucc (this);
				SetSucc (node);
				node->SetPred (this);
				}

			inline void InsertAfter (CNode * node) {
				SetPred (node);
				if (SetSucc (node->GetSucc ()) != null)
					GetSucc ()->SetPred (this);
				node->SetSucc (this);
				}

			inline void Unlink (void) {
				if (GetPred ())
					GetPred ()->SetSucc (GetSucc ());
				if (GetSucc ())
					GetSucc ()->SetPred (GetPred ());
				SetPred (null);
				SetSucc (null);
				}
			};

		CNode<_T>*	m_head;
		CNode<_T>*	m_tail;
		uint			m_length;

	private:
		CNode<_T>* FindNode (_K key) {
			for (CNode<_T>* pNode = m_head; pNode; pNode = pNode->m_link) {
				if (pNode->m_data == key)
					return pNode;
				}
			return null;
			}


	public:
		CDLL () : m_head (null), m_tail (null), m_length (0) {}

		~CDLL () { Destroy (); }

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
			CNode<_T>* pNode = FindNode (key);
			return pNode ? &pNode->m_data : null;
			}

		bool Remove (_K key) {
			CNode<_T*> pNode = FindNode (key);
			if (!pNode)
				return false;
			pNode->Unlink ();
			delete pNode;
			--m_length;
			return true;
			}

		_T* InsertBefore (_T data, _K key) {
			if (!(m_current = new CNode<_T>))
				return null;
			CNode<_T>* pNode = FindNode (key);
			if (!pNode)
				pNode = m_head;
			m_current->InsertBefore (pNode);
			if (!m_head)
				m_head = m_tail = m_current;
			else if (m_tail->GetSucc ())
				m_tail = m_tail->GetSucc ();
			++m_length;
			pNode->m_data = data;
			return &pNode->m_data;
			}


		_T* InsertAfter (_T data, _K key) {
			if (!(m_current = new CNode<_T>))
				return null;
			CNode<_T>* pNode = FindNode (key);
			if (!pNode)
				pNode = m_head;
			m_current->InsertAfter (pNode);
			if (!m_head)
				m_head = m_tail = m_current;
			else if (m_tail->GetSucc ())
				m_tail = m_tail->GetSucc ();
			++m_length;
			pNode->m_data = data;
			return &pNode->m_data;
			}

		_T* Add (_T data) {
			m_current->InsertBefore (m_head);
			if (!m_head)
				m_head = m_tail = m_current;
			else if (m_tail->GetSucc ())
				m_tail = m_tail->GetSucc ();
			pNode->m_data = data;
			return &pNode->m_data;
			}

		_T* Append (_T data) {
			m_current->InsertAfter (m_tail);
			if (!m_head)
				m_head = m_tail = m_current;
			else if (m_tail->GetSucc ())
				m_tail = m_tail->GetSucc ();
			pNode->m_data = data;
			return &pNode->m_data;
			}

	inline CDLL<_T, _K>& operator= (CDLL<_T, _K>& other) {
		m_head = other.m_head;
		return *this;
		}
	};

// -----------------------------------------------------------------------------

template < class _T, class _K >
class CDLLIterator {
	private:
		CDLL<_T, _K>&	m_dll;
		typename CDLL<_T, _K>::CNode<_T>* m_current;

	public:
		CDLLIterator (CDLL<_T, _K>& dll) : m_dll (dll) {}

		inline _T* Begin (void) { 
			m_current = m_dll.Head ();
			return m_current ? &m_current->m_data : null; 
			}

		inline _T* End (void) { 
			m_current = m_dll.Tail ();
			return m_current ? &m_current->m_data : null; 
			}

		inline _T* Done (void) { return m_current = null; }

		inline CDLLIterator& operator++() { m_current = m_current->GetSucc (); }
		
		inline CDLLIterator& operator--() { m_current = m_current->GetPred (); }
		
		inline _T* operator->() { return m_current ? &m_current->m_data : null; }
		
		inline _T* operator*() { return m_current ? &m_current->m_data : null; }
	};

// -----------------------------------------------------------------------------

#endif //__DLL_H