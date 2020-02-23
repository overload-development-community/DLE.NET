// dlcView.h : interface of the CMineView class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __textureview_h
#define __textureview_h

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "afxcview.h"
#include "DlcDoc.h"
#include "Matrix.h"
#include "mineview.h"
#include "PolyModel.h"
#include "texturefilter.h"

//------------------------------------------------------------------------------

class CTextureView : public CWnd {
	public:
// structs
		DECLARE_DYNCREATE(CTextureView)

		int				*m_pTextures;
		CPen				*m_penCyan;
		CSize				m_iconSize;
		CSize				m_iconSpace;
		CSize				m_viewSpace;
		int				m_nRows [2];
		uint				m_viewFlags;
		BOOL				m_bShowAll;
		bool				m_bDelayRefresh;
		bool				m_bLButtonDown;
		bool				m_bRButtonDown;
		CTextureFilter	m_filter;
		CSize				m_paneSize;

		CTextureView ();
		~CTextureView ();
		void Reset ();
		void QSortTexMap (short left, short right);
		void CreateTexMap (void);
		int QCmpTexFilters (int nTxt, int mTxt, uint mf, uint mf2);
		afx_msg BOOL OnEraseBkgnd (CDC* pDC);
		afx_msg void OnPaint ();
		afx_msg void OnSize (UINT nType, int cx, int cy);
		afx_msg void OnLButtonDown (UINT nFlags, CPoint point);
		afx_msg void OnRButtonDown (UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp (UINT nFlags, CPoint point);
		afx_msg void OnRButtonUp (UINT nFlags, CPoint point);
		afx_msg void OnMButtonDown (UINT nFlags, CPoint point);
		afx_msg void OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		afx_msg BOOL OnMouseWheel (UINT nFlags, short zDelta, CPoint pt);
		void Setup ();
		void RecalcLayout ();
		int PickTexture(CPoint &point,short &nBaseTex);
		void Refresh (bool bRepaint = true);
		void ToggleViewFlag(eMineViewFlags flag) {
			m_viewFlags ^= flag;
			Refresh ();
			}
		void SetViewFlags (uint flags) {
			if (m_viewFlags != flags) {
				m_viewFlags = flags; 
				Refresh ();
				}
			}
		inline CSize& PaneSize () { return m_paneSize; }
		inline uint GetViewFlags () { return m_viewFlags; }
		inline bool ShowAll ()
			{ return m_bShowAll && (m_filter.Filter () == 0xFFFFFFFF); }
		inline uint& TextureFilter (void) { return m_filter.Filter (); }
	DECLARE_MESSAGE_MAP()
};
                        
#endif //__textureview_h
