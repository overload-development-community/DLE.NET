#ifndef __AVLTree_H
#define __AVLTree_H

#include "define.h"
#include "types.h"

template < class _T, class _K > 
class CAVLTree {

#define  AVL_OVERFLOW   1
#define  AVL_BALANCED   0
#define  AVL_UNDERFLOW  3

	public:
		typedef bool (*walkCallback) (_T& data);

		template < class _U > 
		class CNode {
			public:
				CNode*	m_left;
				CNode*	m_right;
				_U			m_data;
				byte		m_balance;

			CNode () : m_left (null), m_right (null), m_balance (0) {}
			};

		CNode<_T>*		m_root;
		CNode<_T>*		m_current;
		_T					m_data;
		_K					m_key;
		bool				m_duplicate;
		bool				m_changed;
		walkCallback	m_callback;

		// -----------------------------------------------------------------------------

		CAVLTree () : m_root (null) {}

		// -----------------------------------------------------------------------------

		~CAVLTree () { Destroy (); }

		// -----------------------------------------------------------------------------

		private: void Destroy (CNode<_T>* node)
		{
		if (node) {
			Destroy (node->m_left);
			Destroy (node->m_right);
			delete node;
			}
		}

		// -----------------------------------------------------------------------------

		public: void Destroy (void)
		{
		Destroy (m_root);
		}

		// -----------------------------------------------------------------------------

		private: _T* Add (CNode<_T>** node)
		{
		if (!(m_current = new CNode<_T>))
			return null;
		*node = m_current;
		m_current->m_data = m_data;
		m_changed = true;
		return &m_current->m_data;
		}

		// -----------------------------------------------------------------------------

		private: void BalanceLeftGrowth (CNode<_T>** node)
		{
			CNode<_T>*	p = *node;

		if (m_changed) {
			CNode<_T>* p1, * p2;
			switch (p->m_balance) {
				case AVL_UNDERFLOW:
					p1 = p->m_left;
					if (p1->m_balance == AVL_UNDERFLOW) { // simle LL rotation
						p->m_left = p1->m_right;
						p1->m_right = p;
						p->m_balance = AVL_BALANCED;
						p = p1;
						}
					else { // double LR rotation
	               p2 = p1->m_right;
		            p1->m_right = p2->m_left;
			         p2->m_left = p1;
						p->m_left = p2->m_right;
						p2->m_right = p;
						p->m_balance = (p2->m_balance == AVL_UNDERFLOW) ? AVL_OVERFLOW : AVL_BALANCED;
						p1->m_balance = (p2->m_balance == AVL_OVERFLOW) ? AVL_UNDERFLOW : AVL_BALANCED;
						p = p2;
						}
					p->m_balance = AVL_BALANCED;
					m_changed = FALSE;
					break;
   
				case AVL_BALANCED:
					p->m_balance = AVL_UNDERFLOW;
					break;
   
				case AVL_OVERFLOW:
					p->m_balance = AVL_BALANCED;
					m_changed = FALSE;
					break;
				}
			}
		*node = p;
		}

		// -----------------------------------------------------------------------------

		private: void BalanceRightGrowth (CNode<_T>** node)
		{
			CNode<_T>*	p = *node;

		if (m_changed) {
			CNode<_T>* p1, * p2;
			switch (p->m_balance) {
				case AVL_UNDERFLOW:
					p->m_balance = AVL_BALANCED;
					m_changed = false;
					break;
   
				case AVL_BALANCED:
					p->m_balance = AVL_OVERFLOW;
					break;
   
				case AVL_OVERFLOW:
					p1 = p->m_right;
					if (p1->m_balance == AVL_OVERFLOW) { // simple RR rotation
						p->m_right = p1->m_left;
						p1->m_left = p;
						p->m_balance = AVL_BALANCED;
						p = p1;
						}
					else { // double RL rotation
						p2 = p1->m_left;
						p1->m_left = p2->m_right;
						p2->m_right = p1;
						p->m_right = p2->m_left;
						p2->m_left = p;
						p->m_balance = (p2->m_balance == AVL_OVERFLOW) ? AVL_UNDERFLOW : AVL_BALANCED;
						p1->m_balance = (p2->m_balance == AVL_UNDERFLOW) ? AVL_OVERFLOW : AVL_BALANCED;
						p = p2;
						}
					p->m_balance = AVL_BALANCED;
					m_changed = FALSE;
				}
			}
		*node = p;
		}

		// -----------------------------------------------------------------------------

		public: _T* Insert (CNode<_T>** node)
		{
			CNode<_T>*	p = *node;

		if (!p) 
			return Add (node);

		if (p->m_data > m_key) {
			if (!Insert (&p->m_left))
				return null;
			BalanceLeftGrowth (&p);
			}
		else if (p->m_data < m_key) {
			if (!Insert (&p->m_right))
				return null;
			BalanceRightGrowth (&p);
			}
		else {
			m_duplicate = true;
			m_current = p;
			}
		*node = p;
		return &m_current->m_data;
		} 

		// -----------------------------------------------------------------------------

		public: _T* Insert (_T data, _K key)
		{
		m_data = data;
		m_key = key;
		m_changed = 
		m_duplicate = false;
		return Insert (&m_root);
		}

		// -----------------------------------------------------------------------------

		private: void BalanceLeftShrink (CNode<_T>* node)
		{
		if (m_changed) {
			CNode<_T>* p = *node;
			switch (p->m_balance) {
				case AVL_UNDERFLOW:
					p->m_balance = AVL_BALANCED;
					break;

				case AVL_BALANCED:
					p->m_balance = AVL_OVERFLOW;
					m_changed = false;
					break;

				case AVL_OVERFLOW:
					CNode<_T>* p1 = p->m_right;
					if (p1->m_balance != AVL_UNDERFLOW) { simple RR rotation
						p->m_right = p1->m_left;
						p1->m_left = p;
						if (p1->m_balance) {
							p->m_balance =
							p1->m_balance = AVL_BALANCED;
							}
						else {
							p->m_balance = AVL_OVERFLOW;
							p1->m_balance = AVL_UNDERFLOW;
							m_changed = false;
							}
						p = p1;
						}
					else { // double RL rotation
						CNode<_T>* p2 = p1->m_left;
						p1->m_left = p2->m_right;
						p2->m_right = p1;
						p->m_right = p2->m_left;
						p2->m_left = p;
						p->m_balance = (p2->m_balance == AVL_OVERFLOW) ? AVL_UNDERFLOW : AVL_BALANCED;
						p1->m_balance = (p2->m_balance == AVL_UNDERFLOW) ? AVL_OVERFLOW : AVL_BALANCED;
						p = p2;
						p->m_balance = AVL_BALANCED;
						}
				}
			*node = p;
			}
		}

		// -----------------------------------------------------------------------------

		private: void BalanceRightShrink (CNode<_T>* node)
		{
		if (m_changed) {
			CNode<_T>* p = *node;
			switch (p->m_balance) {
				case AVL_OVERFLOW:
					p->m_balance = AVL_BALANCED;
					break;

				case AVL_BALANCED:
					p->m_balance = AVL_UNDERFLOW;
					m_changed = false;
					break;

				case AVL_UNDERFLOW:
					CNode<_T>* p1 = p->m_left;
					if (p1->m_balance != AVL_OVERFLOW) { // simple LL rotation
						p->m_left = p1->m_right;
						p1->m_right = p;
						if (p1->m_balance) {
							p->m_balance = 
							p1->m_balance = AVL_BALANCED;
							}
						else {
							p->m_balance = AVL_UNDERFLOW;
							p1->m_balance = AVL_OVERFLOW;
							m_changed = false;
							}
						p = p1;
						}
					else { // double LR rotation
						CNode<_T>* p2 = p1->m_right;
						b = (INT1) p2->m_balance;
						p1->m_right = p2->m_left;
						p2->m_left = p1;
						p->m_left = p2->m_right;
						p2->m_right = p;
						p->m_balance = (b == AVL_UNDERFLOW) ? AVL_OVERFLOW : AVL_BALANCED;
						p1->m_balance = (b == AVL_OVERFLOW) ? AVL_UNDERFLOW : AVL_BALANCED;
						p = p2;
						p->m_balance = AVL_BALANCED;
						}
				}
			*node = p;
			}
		}

		// -----------------------------------------------------------------------------
		// Find the greatest element in the left sub tree of the tree node to be deleted
		// and put it in the place of the element to be deleted. To do that, the data
		// members of the two tree elements are simply exchanged.

		private: void Replace (CNode<_T>** node, CNode<_T>** deleted)
		{
			CAVLNode* r, d;
			void *  h;

		p = *node;
		if (p->m_right) {
			Replace (&p->m_right, deleted, m_changed);
			if (*branchShrinked)
				CAVL::BalanceRShrink (&(p), m_changed);
			}
		else {
			CNode<_T>* d = *deleted;
			Swap (p->m_data, (*deleted)->m_data);
			*deleted = p;
			p = p->m_left;
			m_changed = true;
			}
		*node = p;     
		} 

		// -----------------------------------------------------------------------------

		private: short Delete (CNode<_T>** node, _K key)
		{
			int avlRes = 0;
			CAVLNode* p = *node;

		if (p)
			m_changed = false;
		else if (p->m_data > key) {
			if (Delete (a, &p->m_left, ppData))
				return -1;
			BalanceLeftShrink (&p);
			}	
		else if (p->m_data < key) {
			if (Delete (&p->m_right))
				return -1;
			if (m_changed)
				BalanceRightShrink (&p);
			}
		else {
			avlRes = E_PTV_OK;
			d = p;
			if (!p->m_right) {
				p = p->m_left;
				m_changed = true;
				}
			else if (!p->m_left) {
				p = p->m_right;
				m_changed = true;
				}
			else {
				Replace (&d->m_left, &d);
				BalanceLeftShrink (&p);
				}
			delete d;
			}
		*node = p;
		return 0;
		}

		// -----------------------------------------------------------------------------

		public: _T* Find (_K key) 
		{
			CNode<_T>* p = m_root;

		while (p) {
			if (p->m_data > key)
				p = p->m_left;
			else if (p->m_data < key)
				p = p->m_right;
			else {
				m_current = p;
				return &p->m_data;
				}
			}
		return null;
		} 

		// -----------------------------------------------------------------------------

		private: bool Walk (CNode<_T>* node)
		{
		if (!Walk (node->m_left))
			return false;
		if (!node)
			return true;
		if (!m_callback (node->m_data))
			return false;
		if (!Walk (node->m_right))
			return false;
		return true;
		}

		// -----------------------------------------------------------------------------

		public: bool Walk (walkCallback callback)
		{
		m_callback = callback;
		return Walk (m_root);
		}

		// -----------------------------------------------------------------------------

	};

#endif //__AVLTree_H
