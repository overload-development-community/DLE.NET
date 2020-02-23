// dlcView.h : interface of the CMineView class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __toolview_h
#define __toolview_h

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxwin.h>
#include <afxext.h>
#include <afxcview.h>

#include "DlcDoc.h"
#include "Matrix.h"
#include "PolyModel.h"
#include "define.h"
#include "types.h"
#include "global.h"
#include "dle-xp-res.h"
#include "textures.h"
#include "texedit.h"
#include "resourcemanager.h"

extern int nLayout;

//------------------------------------------------------------------------------
#if 1
class CExtBitmapButton : public CBitmapButton
{
	public:
		DECLARE_DYNCREATE(CExtBitmapButton)
	public:
		int	m_nId;
		CWnd	*m_pParent;
		UINT	m_nState;
		int	m_nValue;
		//virtual int OnToolHitTest (CPoint point, TOOLINFO* pTI);
		BOOL AutoLoad (UINT nId, CWnd* pParent) {
			m_nId = nId;
			m_pParent = pParent;
			m_nState = WM_LBUTTONUP;
			m_nValue = -1;
			return CBitmapButton::AutoLoad (nId, pParent);
			}
		afx_msg void OnLButtonUp (UINT nFlags, CPoint point);
		afx_msg void OnLButtonDown (UINT nFlags, CPoint point);
		afx_msg void OnMouseMove (UINT nFlags, CPoint point);
		afx_msg void OnSetFocus (CWnd* pOldWnd);
		afx_msg void OnKillFocus (CWnd* pNewWnd);
		void Notify (UINT nMsg);

		DECLARE_MESSAGE_MAP ()
};
#endif

//------------------------------------------------------------------------------

class CColorControl : public CWnd {
	private:
		COLORREF	m_bkColor;

	public:
		afx_msg void OnEraseBkGnd (CDC *pDC);
		afx_msg void OnPaint ();

		inline void SetColor (COLORREF bkColor) { m_bkColor = bkColor; }

		CColorControl () : m_bkColor (RGB (128, 128, 128)) {}

		DECLARE_MESSAGE_MAP ()
	};

//------------------------------------------------------------------------------

class CDlgHelpers {
	private:
		CWnd*	m_pParent;

	public:
		COLORREF	m_custColors [16];

	public:
		CDlgHelpers (CWnd* pParent = null) : m_pParent (pParent) {}

		void DDX_Double (CDataExchange * pDX, int nIDC, double& fVal, double min = 1, double max = 0, LPSTR pszFmt = "%1.2f", LPSTR pszErrMsg = null);
		void DDX_Slider (CDataExchange * pDX, int nIdC, int& nTic);
		int DDX_Int (CDataExchange * pDX, int nIdC, int i);
		int DDX_Flag (CDataExchange * pDX, int nIdC, int i);
		void InitSlider (int nIdC, int nMin, int nMax);
		int GetCheck (int nIdC);
		void CBInit (CComboBox *pcb, char* pszNames [], ubyte *pIndex, ubyte *pItemData, int nMax, int nType = 0, bool bAddNone = false);
		int CBAddString (CComboBox *pcb, char *str);
		void SelectItemData (CComboBox *pcb, int nItemData);
		void CBUpdateListWidth (CComboBox *pcb);
		void EnableControls (int nIdFirst, int nIdLast, BOOL bEnable);
		BOOL OnToolTipNotify (UINT id, NMHDR *pNMHDR, LRESULT *pResult);

		BOOL SelectColor (BYTE& red, BYTE& green, BYTE& blue);
		void CreateColorCtrl (CColorControl *pWnd, int nIdC);
		void UpdateColorCtrl (CColorControl *pWnd, COLORREF color);

		inline CWnd *Parent (void) { return m_pParent; }

		inline CComboBox *CBCtrl (int nId) { return (CComboBox *) m_pParent->GetDlgItem (nId); }
		inline CListBox *LBCtrl (int nId) { return (CListBox *) m_pParent->GetDlgItem (nId); }
		inline CButton *BtnCtrl (int nId){ return (CButton *) m_pParent->GetDlgItem (nId); }
		inline CSliderCtrl *SlCtrl (int nId) { return (CSliderCtrl *) m_pParent->GetDlgItem (nId); }
		inline CScrollBar *SBCtrl (int nId) { return (CScrollBar *) m_pParent->GetDlgItem (nId); }
	};

//------------------------------------------------------------------------------

class CTabDlg : public CDialog, public CDlgHelpers {
	protected:
		BOOL m_bInited;
		BOOL m_bValid;
		BOOL m_bHaveData;

	public:
		inline BOOL HaveData (CDataExchange * pDX) { 
			if (!(theMine && m_bInited))
				return false;
			if (!pDX->m_bSaveAndValidate)
				m_bHaveData = true;
			return m_bHaveData;
			}

		virtual bool Refresh (void) { 
			UpdateData (FALSE); 
			return true; 
			}

		virtual BOOL OnSetActive () { return FALSE; }
		virtual BOOL OnKillActive () { return FALSE; }
		virtual void EnableControls (BOOL bEnable) {}

		inline BOOL Valid (void) { return m_bValid; }

		CTabDlg (UINT nId, CWnd* pParent = null);

		//virtual BOOL PreTranslateMessage (MSG* pMsg);

		BOOL OnToolTipNotify (UINT id, NMHDR *pNMHDR, LRESULT *pResult);

		DECLARE_MESSAGE_MAP ()
	};

//------------------------------------------------------------------------------

class CConvertDlg : public CDialog
{
	public:
		bool			m_bInited;
		CWnd			m_showD1;
		CWnd			m_showD2;
		HINSTANCE	m_hInst;
		CResource	m_res;
		short*		m_pTextures;

		CConvertDlg (CWnd *pParent = null);
      virtual BOOL OnInitDialog ();
		void EndDialog (int nResult);
      virtual void DoDataExchange (CDataExchange *pDX);
		void CreateImgWnd (CWnd *pImgWnd, int nIdC);
		void Reset ();
		void Refresh ();
		afx_msg void OnPaint ();
		afx_msg void OnSetD1 ();
		afx_msg void OnSetD2 ();
		virtual void OnOK (void);
		inline CComboBox *CBD1 ()
			{ return (CComboBox *) GetDlgItem (IDC_CONVERT_D1); }
		inline CComboBox *CBD2 ()
			{ return (CComboBox *) GetDlgItem (IDC_CONVERT_D2); }

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CToolDlg : public CPropertyPage, public CDlgHelpers
{
	public:
		DECLARE_DYNCREATE(CToolDlg)
   protected:
      CPropertySheet *  m_pParent;
	public:
		bool	m_bInited;
		bool	m_bHaveData;
		bool	m_bVisible;
		UINT	m_nId;

		CToolDlg (UINT nIdTemplate = 0, CPropertySheet *pParent = null);

		~CToolDlg () { 
			if (nLayout == 2)
				SavePos ();
			m_pParent = null, m_bInited = m_bHaveData = false; 
			}

      virtual BOOL OnInitDialog ();

      virtual BOOL OnSetActive () {
			if (nLayout == 2)
				BringWindowToTop ();
			else {
				if ((theMine == null) || !CPropertyPage::OnSetActive ())
					return FALSE;
				CPropertyPage::UpdateData (FALSE);
				}
         return TRUE;
         };

      virtual void DoDataExchange (CDataExchange * pDX) {};

		inline bool HaveData (CDataExchange * pDX) { 
			if (!(theMine && m_bInited))
				return false;
			if (!pDX->m_bSaveAndValidate)
				m_bHaveData = true;
			return m_bHaveData;
			}

		virtual void Refresh (void) { CPropertyPage::UpdateData (FALSE); }

		void Hide (void) { 
			if (IsWindow (m_hWnd) && IsWindowVisible () && (nLayout == 2)) {
				ShowWindow (SW_HIDE); 
				m_bVisible = false;
				}
			}
		void Show (void) { 
			if (IsWindow (m_hWnd) && (nLayout == 2)) {
				if (!IsWindowVisible ()) {
					m_bVisible = true;
					ShowWindow (SW_RESTORE); 
					}
				SetForegroundWindow ();
				CPropertyPage::UpdateData (FALSE);
				}
			}
#if 0
		int OnToolHitTest (CPoint point, TOOLINFO* pTI);
#endif
		BOOL OnToolTipNotify (UINT id, NMHDR *pNMHDR, LRESULT *pResult);
		void CreateImgWnd (CWnd * pImgWnd, int nIdC);
		virtual void PreSubclassWindow ();
		afx_msg void OnClose () { Hide (); }
		afx_msg void OnSelectPrevTab ();
		afx_msg void OnSelectNextTab ();
		afx_msg void OnSetFocus (CWnd* prevWnd) { 
			if (nLayout == 2) 
				SetForegroundWindow (); //BringWindowToTop (); 
			}
		void GetCtrlClientRect (CWnd *pWnd, CRect& rc);
		inline bool Inited () { return m_bInited; }

		void SavePos (void);

		DECLARE_MESSAGE_MAP()
};

//------------------------------------------------------------------------------
// CExtSliderCtrl links a slider control, a spin control and an edit or text field.
// Controls an id of zero is passed for are ignored (not existant)
// Controls with a negative idea are reversed (e.g. vertical sliders have their min at the bottom and the max at their top)
// or are text fields 

class CExtSliderCtrl : public CDlgHelpers {
	private:
		CDialog*			m_parent;
		int				m_nSlider;
		int				m_nSpinner;
		int				m_nText;
		int				m_nMin;
		int				m_nMax;
		double			m_nScale;
		double			m_nTextScale;
		int				m_nValue;
		int				m_nStep;
		int				m_nPageSize;
		char*				m_format;
		bool				m_bReverseSlider;
		bool				m_bReverseSpinner;
		bool				m_bEditableText;
		bool				m_bUpdating;
		char*				(__cdecl *m_formatter) (char*, int);

	public:
		CExtSliderCtrl () 
			: m_parent (null), m_nSlider (0), m_nSpinner (0), m_nText (0), m_nMin (0), m_nMax (0), m_nScale (1.0), m_nTextScale (1.0), m_nValue (0), m_format (null), 
			  m_bReverseSlider (false), m_bReverseSpinner (false), m_bUpdating (false), m_formatter (null)
		{ }

		void Init (CDialog* parent, int nSliderId, int nSpinnerId, int nEditId, int nMin, int nMax, double nScale = 1.0, double nTextScale = 1.0, int nTicFreq = 1, char* format = null);
		inline void SetFormatter (char* (__cdecl *formatter) (char*, int)) { m_formatter = formatter; }
		inline void SetPageSize (int nPageSize) { m_nPageSize = nPageSize; }
		bool SetValue (int nValue, bool bScale = true);
		inline int GetValue (void) { return int (m_nValue * m_nScale + 0.5); }
		bool OnScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		bool OnEdit (void);
		void Update (void);
		inline void SetFormat (char* format) { m_format = format; }

		inline CSliderCtrl* Slider (void) { return m_nSlider ? (CSliderCtrl*) m_parent->GetDlgItem (m_nSlider) : null; }
		inline CSpinButtonCtrl* Spinner (void) { return m_nSpinner ? (CSpinButtonCtrl*) m_parent->GetDlgItem (m_nSpinner) : null; }
		inline CEdit* Text (void) { return (m_nText > 0) ? (CEdit*) m_parent->GetDlgItem (m_nText) : null; }

		void DoDataExchange (CDataExchange* pDX, int& value);
		void DoDataExchange (CDataExchange* pDX, short& value);
		void DoDataExchange (CDataExchange* pDX, ubyte& value);
	};

//------------------------------------------------------------------------------

class CToolTabCtrl : public CTabCtrl {
	public:
		DECLARE_DYNCREATE(CToolTabCtrl)

		UINT		m_nId;
		CDialog*	m_dialogs [24];
		int		m_nDlgCount;
		int		m_nCurrent;
		CWnd*		m_pParent;

		CToolTabCtrl () 
			: m_nDlgCount (0), m_nCurrent (0), m_pParent (null)
			{}

		bool Add (CDialog* pDlg, UINT nId, char* pszTitle);

		void Init (CWnd* pParent) { m_pParent = pParent, m_nDlgCount = 0, m_nCurrent = 0; }
		void Setup (void);
		void Update (int nCurrent = -1);

		afx_msg void OnLButtonDown (UINT nFlags, CPoint point);
		afx_msg void OnSelchangeTab (NMHDR* pNMHDR, LRESULT* pResult);

		inline CDialog* Current (void) { return m_nDlgCount ? m_dialogs [m_nCurrent] : null; }

	DECLARE_MESSAGE_MAP ()
	};

//------------------------------------------------------------------------------

class CTexToolDlg : public CToolDlg
{
	public:
		DECLARE_DYNCREATE(CTexToolDlg)

	CWnd		m_textureWnd;
	UINT_PTR	m_nTimer;
	int		m_nTexWndId;
	int		m_nTimerId;
	COLORREF	m_bkColor;
	int		m_frame [2];
	bool		m_bOtherSegment;
	int		m_xScrollOffset [2];
	int		m_yScrollOffset [2];

	CTexToolDlg (UINT nIdTemplate = 0, CPropertySheet *pParent = null, 
					 int nTexWndId = 0, int nTimerId = -1, COLORREF bkColor = RGB (0,0,0),
					 bool bOtherSegment = false);
	~CTexToolDlg ();
	bool Refresh (short nBaseTex = -1, short nOvlTex = -1, short nVisible = -1);
	bool AnimateTexture (void);
	bool ScrollTexture (short texIds []);
	bool UpdateTextureClip (short texIds []);

   virtual BOOL OnInitDialog ();
	afx_msg void OnPaint ();
	afx_msg void OnTimer (UINT_PTR nIdEvent);
	virtual BOOL OnSetActive ();
	virtual BOOL OnKillActive ();
	virtual BOOL TextureIsVisible ();

	DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CAnimTexWnd : public CWnd
{
	public:
		CAnimTexWnd ();
		bool StopAnimation ();
		bool StartAnimation (const CTexture *pTexture);

	private:
		UINT_PTR m_nAnimTimer;
		int		m_nTexId;
		int		m_nFrame;
		int		m_xScrollOffset [2];
		int		m_yScrollOffset [2];

		afx_msg void OnTimer (UINT_PTR nIdEvent);
		afx_msg void OnDestroy (void);

		bool ScrollTexture (short nTexId);
		bool UpdateTextureClip (short nTexId);
		bool AnimateTexture (short nTexId);

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CDiagTool : public CToolDlg
{
	public:
		int			m_nTrigger;
		int			m_targets;
		int			m_iTarget;
		int			m_nCountDown;
		int			m_nSecretReturn;
		char			m_szTarget [40];
		int			m_nObjects [16];
		int			m_nContained [2];
		int			m_nErrors [2];
		int			m_bAutoFixBugs;
		int			m_bShowWarnings;
		CReactorTrigger	*m_pTrigger;
		bool			m_bCheckMsgs;
		int			m_statsWidth;
		ubyte			m_playerId;
		ubyte			m_coopId;

		CDiagTool (CPropertySheet *pParent = null);
		~CDiagTool ();
      virtual BOOL OnInitDialog ();
      virtual void DoDataExchange (CDataExchange *pDX);
		virtual BOOL OnSetActive ();
		void Reset ();
		void Refresh ();
		LPSTR ItemText (int nValue, LPSTR pszPrefix = null);
		void CountObjects (void);
		int CountTextures (void);
		afx_msg void OnCheckMine ();
		afx_msg void OnShowBug ();
		afx_msg void OnClearList ();
		afx_msg void OnFixBugs ();
		afx_msg void OnShowWarnings ();
		int AddMessage (const char *pszMsg, int nMaxMsgs = 100, bool bCheckMsg = false);
		void UpdateStatsWidth (char* s);
		bool UpdateStats (char *szError, int nErrorLevel, 
							   int nSegment = -1, int nSide = -1, int nLine = -1, int nPoint = -1, 
							   int nChild = -1, int nWall = -1, int nTrigger = -1, int nObject = -1);
		double CalcFlatnessRatio (short nSegment, short nSide);
		double CalcDistance (CVertex* v1, CVertex* v2, CVertex* v3);
		double CalcAngle (short vert0,short vert1,short vert2,short vert3);
		void ClearBugList ();
		int CheckId (ubyte &type, ubyte &id);
		bool CheckSegments (void);
		bool CheckSegTypes (void);
		bool CheckWalls (void);
		bool CheckTriggers (void);
		bool CheckObjects (void);
		bool CheckAndFixPlayer (int nMin, int nMax, int nObject, int* players);
		bool CheckVertices (void);
		bool CheckBotGens (void);
		bool CheckEquipGens (void);
		bool CheckLights (void);
		bool TagSegment (short nSegment);
		char FindProducer (CObjectProducer* producerP, short nSegment, short* refList = null);
		void CountProducerRefs (int nSpecialType, short* refList, CObjectProducer* producerP, short nProducers);
		short FixProducers (int nSpecialType, short* segList, short* refList, CObjectProducer* producerP, short nProducers, char* pszType);
		short AssignProducers (int nSpecialType, short* segList, short* refList, CObjectProducer* producerP, short nProducers);
		short CleanupProducers (short* refList, CObjectProducer* producerP, short nProducers);

		inline CListView *LVStats ()
			{ return (CListView *) GetDlgItem (IDC_DIAG_STATS); }
		inline CListBox *LBBugs ()
			{ return LBCtrl (IDC_DIAG_BUGLIST); }
		inline CWnd *TargetEdit ()
			{ return GetDlgItem (IDC_REACTOR_TARGET); }
		CWall *OppWall (ushort nSegment, ushort nSide);

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CReactorTool : public CToolDlg
{
	public:
		int			m_nTrigger;
		int			m_targets;
		int			m_iTarget;
		int			m_nCountDown;
		int			m_nSecretReturn;
		int			m_nStrength;
		int			m_bDefaultStrength;
		char			m_szTarget [40];
		CReactorTrigger	*m_pTrigger;

		CReactorTool (CPropertySheet *pParent = null);
      virtual BOOL OnInitDialog ();
      virtual void DoDataExchange (CDataExchange *pDX);
		virtual BOOL OnSetActive ();
		void EnableControls (BOOL bEnable);
		void InitLBTargets ();
		void Reset ();
		void Refresh ();
		void AddTarget (short nSegment, short nSide);
		int FindTarget (short nSegment, short nSide);
		afx_msg void OnAddTarget ();
		afx_msg void OnDeleteTarget ();
		afx_msg void OnAddWallTarget ();
		afx_msg void OnDeleteWallTarget ();
		afx_msg void OnCountDown ();
		afx_msg void OnSecretReturn ();
		afx_msg void OnStrength ();
		afx_msg void OnStrengthDefault ();
		afx_msg void OnSetTarget ();
		inline CListBox *LBTargets ()
			{ return LBCtrl (IDC_REACTOR_TARGETLIST); }
		inline CWnd *TargetEdit ()
			{ return GetDlgItem (IDC_REACTOR_TARGET); }

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CMissionTool : public CToolDlg
{
	public:
		CMissionData	m_missionData;
		char				m_szLevel [26];

		CMissionTool (CPropertySheet *pParent = null);
	   virtual BOOL OnInitDialog ();
      virtual void DoDataExchange (CDataExchange *pDX);
		virtual void OnOK (void);
		virtual void OnCancel (void);
		afx_msg void OnSetLevelName (void);
		afx_msg void OnAdd (void);
		afx_msg void OnDelete (void);
		afx_msg void OnRename (void);
		afx_msg void OnUp (void);
		afx_msg void OnDown (void);
		afx_msg void OnFromHog (void);
		afx_msg void OnLoadLevel (void);
		void Refresh (void);
		virtual BOOL OnSetActive ();
		void BuildLevelList (void);
		int LevelNumber (char* pszLevel);
		LPSTR CopyLevelName (LPSTR pszDest, LPSTR pszSrc);
		LPSTR FixLevelName (LPSTR pszName);
		inline CListBox *LBLevels ()
			{ return LBCtrl (IDC_MISSION_LEVELLIST); }
		DECLARE_MESSAGE_MAP()
};

//------------------------------------------------------------------------------

class CSliderData {
	public:
		int		nId;
		int		nMin;
		int		nMax;
		double	nFactor;
		char**	pszLabels;
};

//------------------------------------------------------------------------------

class CObjectTabDlg : public CTabDlg
{
	public:
		virtual void RefreshRobot (void) {}
		virtual void UpdateRobot (void) {}

		afx_msg void OnChange () { UpdateRobot (); }

		void InitSliders (CDialog* parent, CExtSliderCtrl* sliders, CSliderData* data, int nSliders);

		CObjectTabDlg (UINT nId, CWnd* pParent = null) : CTabDlg (nId, pParent) {}
};

//------------------------------------------------------------------------------

class CObjectAITool : public CObjectTabDlg
{
	public:
		int m_bEndsLevel;

		afx_msg void OnSetObjAI ();

		inline CComboBox *CBBossType () { return CBCtrl (IDC_OBJ_AI_BOSSTYPE); }
		inline CComboBox *CBObjClassAI () { return CBCtrl (IDC_OBJ_CLASS_AI); }

      virtual BOOL OnInitDialog ();
		virtual void EnableControls (BOOL bEnable);
      virtual void DoDataExchange (CDataExchange *pDX);
		virtual bool Refresh (void);
		virtual void RefreshRobot (void);
		virtual void UpdateRobot (void);

		CObjectAITool (UINT nId, CWnd* pParent = null) : CObjectTabDlg (nId, pParent) {}

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CObjectCombatTool : public CObjectTabDlg
{
	public:
		CExtSliderCtrl	m_data [10];

      virtual BOOL OnInitDialog ();
		virtual void EnableControls (BOOL bEnable);
      virtual void DoDataExchange (CDataExchange *pDX);
		virtual bool Refresh (void);
		virtual void RefreshRobot (void);
		virtual void UpdateRobot (void);

		bool OnScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		void OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		void OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);

		inline CComboBox *CBWeapon1 () { return CBCtrl (IDC_OBJ_WEAPON1); }
		inline CComboBox *CBWeapon2 () { return CBCtrl (IDC_OBJ_WEAPON2); }

		CObjectCombatTool (UINT nId, CWnd* pParent = null) : CObjectTabDlg (nId, pParent) {}

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CObjectDeathTool : public CObjectTabDlg
{
	public:
		CExtSliderCtrl m_data [5];

      virtual BOOL OnInitDialog ();
		virtual void EnableControls (BOOL bEnable);
      virtual void DoDataExchange (CDataExchange *pDX);
		virtual bool Refresh (void);
		virtual void RefreshRobot (void);
		virtual void UpdateRobot (void);

		bool OnScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		void OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		void OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);

		afx_msg void OnSetContType ();

		CObjectDeathTool (UINT nId, CWnd* pParent = null) : CObjectTabDlg (nId, pParent) {}

		inline CComboBox *CBExplType () { return CBCtrl (IDC_OBJ_EXPLTYPE); }
		inline CComboBox *CBContType () { return CBCtrl (IDC_OBJ_CONT_TYPE); }
		inline CComboBox *CBContId () { return CBCtrl (IDC_OBJ_CONT_ID); }

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CObjectPhysicsTool : public CObjectTabDlg
{
	public:
		CExtSliderCtrl	m_data [6];

      virtual BOOL OnInitDialog ();
		virtual void EnableControls (BOOL bEnable);
      virtual void DoDataExchange (CDataExchange *pDX);
		virtual bool Refresh (void);
		virtual void RefreshRobot (void);
		virtual void UpdateRobot (void);

		bool OnScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		void OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		void OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);

		CObjectPhysicsTool (UINT nId, CWnd* pParent = null) : CObjectTabDlg (nId, pParent) {}

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CObjectSoundTool : public CObjectTabDlg
{
	public:
      virtual BOOL OnInitDialog ();
		virtual void EnableControls (BOOL bEnable);
      virtual void DoDataExchange (CDataExchange *pDX);
		virtual bool Refresh (void);
		virtual void RefreshRobot (void);
		virtual void UpdateRobot (void);

		CObjectSoundTool (UINT nId, CWnd* pParent = null) : CObjectTabDlg (nId, pParent) {}

		inline CComboBox *CBSoundExpl () { return CBCtrl (IDC_OBJ_SOUND_EXPLODE); }
		inline CComboBox *CBSoundSee () { return CBCtrl (IDC_OBJ_SOUND_SEE); }
		inline CComboBox *CBSoundAttack () { return CBCtrl (IDC_OBJ_SOUND_ATTACK); }
		inline CComboBox *CBSoundClaw () { return CBCtrl (IDC_OBJ_SOUND_CLAW); }
		inline CComboBox *CBSoundDeath () { return CBCtrl (IDC_OBJ_SOUND_DEATH); }

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CAdvancedObjTool : public CObjectTabDlg
{
	public:
		int			m_mass;
		int			m_drag;
		int			m_brakes;
		int			m_turnRoll;
		int			m_size;
		int			m_flags;
		int			m_shields;
		int			m_model;
		int			m_frame;
		int			m_frameNo;
		CFixVector	m_velocity;
		CFixVector	m_thrust;
		CFixVector	m_rotVel;
		CFixVector	m_rotThrust;

		CAdvancedObjTool (UINT nId, CWnd* pParent = null) : CObjectTabDlg (nId, pParent) {}
      virtual BOOL OnInitDialog ();
      virtual void DoDataExchange (CDataExchange *pDX);
		virtual BOOL OnSetActive ();
		virtual BOOL OnKillActive ();
		void Reset ();
		virtual bool Refresh (void);
		afx_msg void OnAccept ();
		template < int _id >
		afx_msg void OnField () {
			char szVal [5];
			::GetWindowText (GetDlgItem (_id)->m_hWnd, szVal, sizeof (szVal));
			if (!*szVal)
				return;
			OnAccept ();
			}

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CObjectPositionTool : public CObjectTabDlg
{
	public:
		double	m_nPos [3];
		double	m_nOrient [3];

		virtual BOOL OnInitDialog ();
		virtual void EnableControls (BOOL bEnable);
		virtual void DoDataExchange (CDataExchange *pDX);
		virtual bool Refresh (void);
		virtual void RefreshRobot (void);
		virtual void UpdateRobot (void);

		void RefreshObjOrient (CDoubleMatrix& orient);
		afx_msg void OnSetLoc ();
		afx_msg void OnResetLoc ();

		CObjectPositionTool (UINT nId, CWnd* pParent = null) : CObjectTabDlg (nId, pParent) {}

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CObjectTool : public CToolDlg
{
	public:
		CWnd				m_showObjWnd;
		CWnd				m_showSpawnWnd;
		CWnd				m_showTextureWnd;
		int				m_nSpawnQty;
		char				m_szInfo [200];
		CExtSliderCtrl	m_objContCount;
		CExtSliderCtrl	m_objContProb;
		CExtSliderCtrl	m_objSpawnQty;

		CToolTabCtrl	m_objectTools;

		CObjectTool (CPropertySheet *pParent = null);
		~CObjectTool ();
      virtual BOOL OnInitDialog ();
      virtual void DoDataExchange (CDataExchange *pDX);
		virtual BOOL OnSetActive ();
		void Reset ();
		void EnableControls (BOOL bEnable);
		void Refresh ();
		void RefreshRobot ();
		void DrawObjectImages ();
		void DrawObject (CWnd *pWnd, int type, int id);
		void SetTextureOverride ();
		bool SetPlayerId (CGameObject& obj, int objType, int *ids, int maxIds, char *pszError);
		void SetObjectId (CComboBox *pcb, short type, short id, short flag = 0);

		void UpdateRobot ();
		int ObjOfAKindCount (int nType = -1, int nId = -1);
		int GetObjectsOfAKind (int nType, CGameObject *objList []);
		void SetNewObjId (CGameObject *pObject, int nType, int nId, int nMaxId);

		afx_msg void OnPaint ();
		afx_msg void OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		afx_msg void OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		bool OnScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);

		afx_msg void OnAdd ();
		afx_msg void OnDelete ();
		afx_msg void OnDeleteAll ();
		afx_msg void OnMove ();
		afx_msg void OnReset ();
		afx_msg void OnDefault ();
		afx_msg void OnAdvanced ();
		afx_msg void OnSetObject ();
		afx_msg void OnSetObjType ();
		afx_msg void OnSetObjId ();
		afx_msg void OnSetObjAI ();
		afx_msg void OnSetTexture ();
		afx_msg void OnSetSpawnType ();
		afx_msg void OnSetSpawnId ();
		afx_msg void OnSetSpawnQty ();
		afx_msg void OnSort ();
		afx_msg void OnMultiplayer ();

		inline CComboBox *CBObjNo () { return CBCtrl (IDC_OBJ_OBJNO); }
		inline CComboBox *CBObjType (){ return CBCtrl (IDC_OBJ_TYPE); }
		inline CComboBox *CBObjId () { return CBCtrl (IDC_OBJ_ID); }
		inline CComboBox *CBObjAI () { return CBCtrl (IDC_OBJ_AI); }
		inline CComboBox *CBObjTexture () { return CBCtrl (IDC_OBJ_TEXTURE); }
		inline CComboBox *CBSpawnType () { return CBCtrl (IDC_OBJ_SPAWN_TYPE); }
		inline CComboBox *CBSpawnId () { return CBCtrl (IDC_OBJ_SPAWN_ID); }

		inline CObjectTabDlg* Current (void) { return (CObjectTabDlg*) m_objectTools.Current (); }

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CEffectTabDlg : public CTabDlg {
	public:
		CGameObject* GetEffect (CGameObject* pObject, ubyte nId, char* szType);

		bool Refresh (void);
		bool AddEffect (void);
		void DeleteEffect (void);

		BOOL OnSetActive ();
		BOOL OnKillActive ();

		afx_msg void OnShowWindow (BOOL bShow, UINT nStatus);
		afx_msg void OnActivate (UINT nState, CWnd* pWndOther, BOOL bMinimized);

		virtual void Add (void) {}
		virtual void Delete (void) { DeleteEffect (); }
		virtual void Copy (void) {}
		virtual void Paste (CGameObject* pObject = null, bool bRefresh = true, bool bVerbose = true) {}

		virtual CGameObject* GetEffect (CGameObject* pObject = null, bool bVerbose = true) = 0;
		virtual ubyte GetType (void) = 0;

		CEffectTabDlg (UINT nId, CWnd* pParent = null) : CTabDlg (nId, pParent) {}

		DECLARE_MESSAGE_MAP ()
	};

//------------------------------------------------------------------------------

class CEffectTool : public CToolDlg
{
	public:
		CToolTabCtrl	m_effectTools;

		CEffectTool (CPropertySheet *pParent = null);
		~CEffectTool ();
      virtual BOOL OnInitDialog ();
      virtual void DoDataExchange (CDataExchange *pDX);
		virtual BOOL OnSetActive ();
		virtual BOOL OnKillActive ();
		void Reset ();
		void Refresh ();
		void LoadEffectList ();
		void EnableControls (int nIdFirst, int nIdLast, BOOL bEnable);

		inline CComboBox *CBEffects ()
			{ return CBCtrl(IDC_EFFECT_OBJECTS); }

		afx_msg void OnEdit ();
		afx_msg void OnAdd ();
		afx_msg void OnDelete ();
		afx_msg void OnCopy ();
		afx_msg void OnPaste ();
		afx_msg void OnPasteAll ();
		afx_msg void OnSetObject ();

		afx_msg void OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);

		inline CEffectTabDlg* Current (void) { return (CEffectTabDlg*) m_effectTools.Current (); }

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CParticleEffectTool : public CEffectTabDlg
{
	public:
		CParticleInfo	m_particles;
		CExtSliderCtrl	m_data [11];
		CColorControl	m_colorWnd;

      virtual BOOL OnInitDialog ();
      virtual void DoDataExchange (CDataExchange *pDX);
		void EnableControls (BOOL bEnable);

		inline CComboBox *CBType () { return CBCtrl(IDC_PARTICLE_TYPE); }

		afx_msg void OnPaint ();
		afx_msg void OnSelectColor ();
		afx_msg void OnEdit ();
		afx_msg void OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		afx_msg void OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		bool OnExtSlider (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);

		void Add (void);
		void Copy (void);
		void Paste (CGameObject* pObject = null, bool bRefresh = true, bool bVerbose = true);
		bool Refresh (void);
		void UpdateColorCtrl (void);

		virtual ubyte GetType (void) { return PARTICLE_ID; }
		virtual CGameObject* GetEffect (CGameObject* pObject = null, bool bVerbose = true) { return CEffectTabDlg::GetEffect (pObject, PARTICLE_ID, bVerbose ? "particle" : null); }

		CParticleEffectTool (UINT nId, CWnd* pParent = null) : CEffectTabDlg (nId, pParent) {}

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CLightningEffectTool : public CEffectTabDlg
{
	public:
		CLightningInfo	m_lightning;
		CExtSliderCtrl	m_data [15];
		CColorControl	m_colorWnd;

      virtual BOOL OnInitDialog ();
      virtual void DoDataExchange (CDataExchange *pDX);
		void EnableControls (BOOL bEnable);
		void HiliteTarget (void);

		inline CComboBox *CBStyle () { return CBCtrl (IDC_LIGHTNING_STYLE); }

		afx_msg void OnPaint ();
		afx_msg void OnSelectColor ();
		afx_msg void OnSetStyle ();
		afx_msg void OnEdit ();
		afx_msg void OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		afx_msg void OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		bool OnExtSlider (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);

		void Add (void);
		void Copy (void);
		void Paste (CGameObject* pObject = null, bool bRefresh = true, bool bVerbose = true);
		bool Refresh (void);
		void UpdateColorCtrl (bool bSave = false);

		virtual ubyte GetType (void) { return LIGHTNING_ID; }
		virtual CGameObject* GetEffect (CGameObject* pObject = null, bool bVerbose = true) { return CEffectTabDlg::GetEffect (pObject, LIGHTNING_ID, bVerbose ? "lightning" : null); }

		CLightningEffectTool (UINT nId, CWnd* pParent = null) : CEffectTabDlg (nId, pParent) {}

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CSoundEffectTool : public CEffectTabDlg
{
	public:
		CSoundInfo		m_sound;
		CExtSliderCtrl	m_soundVolume;

      virtual BOOL OnInitDialog ();
      virtual void DoDataExchange (CDataExchange *pDX);
		void EnableControls (BOOL bEnable);

		inline CComboBox *CBStyle ()
			{ return CBCtrl(IDC_LIGHTNING_STYLE); }

		afx_msg void OnEdit ();
		void afx_msg OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);

		virtual void Add (void);
		virtual void Copy (void);
		virtual void Paste (CGameObject* pObject = null, bool bRefresh = true, bool bVerbose = true);

		virtual ubyte GetType (void) { return SOUND_ID; }
		virtual CGameObject* GetEffect (CGameObject* pObject = null, bool bVerbose = true) { return CEffectTabDlg::GetEffect (pObject, SOUND_ID, bVerbose ? "sound" : null); }

		CSoundEffectTool (UINT nId, CWnd* pParent = null) : CEffectTabDlg (nId, pParent) {}

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CWayPointTool : public CEffectTabDlg
{
	public:
		CWayPointInfo		m_wayPoint;

      virtual BOOL OnInitDialog ();
      virtual void DoDataExchange (CDataExchange *pDX);
		void EnableControls (BOOL bEnable);

		afx_msg void OnEdit ();

		void Add (void);
		void Copy (void);
		void Paste (CGameObject* pObject = null, bool bRefresh = true, bool bVerbose = true);

		virtual ubyte GetType (void) { return WAYPOINT_ID; }
		virtual CGameObject* GetEffect (CGameObject* pObject = null, bool bVerbose = true) { return CEffectTabDlg::GetEffect (pObject, WAYPOINT_ID, bVerbose ? "waypoint" : null); }

		CWayPointTool (UINT nId, CWnd* pParent = null) : CEffectTabDlg (nId, pParent) {}

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

typedef struct tFogControls {
	CExtSliderCtrl	transpSlider;
	CColorControl	colorWnd;
} tFogControls;

class CFogTool : public CEffectTabDlg
{
	public:
		CEffectTool		*m_pParent;
		tFogControls	m_fog [NUM_FOG_TYPES];

      virtual BOOL OnInitDialog ();
      virtual void DoDataExchange (CDataExchange *pDX);
		BOOL OnSetActive ();
		BOOL OnKillActive ();
		void EnableControls (BOOL bEnable);

		afx_msg void OnEdit ();
		afx_msg void OnPickWaterFogColor ();
		afx_msg void OnPickLavaFogColor ();
		afx_msg void OnPickLightFogColor ();
		afx_msg void OnPickDenseFogColor ();

#if 0
		afx_msg void OnShowWindow (BOOL bShow, UINT nStatus);
#endif

		void UpdateTransparency (int nFogType, int nValue);
		void PickColor (int nFogType);
		void UpdateColor (int nFogType);
		void OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);

		virtual bool Refresh (void);

		virtual ubyte GetType (void) { return 255; }
		virtual CGameObject* GetEffect (CGameObject* pObject = null, bool bVerbose = true) { return null; }

		CFogTool (UINT nId, CEffectTool* pParent = null) : CEffectTabDlg (nId, pParent) { m_pParent = pParent; }

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CSegmentTool : public CToolDlg
{
	public:
		int		m_nSegment;
		int		m_nSide;
		int		m_nPoint;
		int		m_nVertex;
		int		m_nFunction;
		int		m_nProps;
		int		m_nOwner;
		int		m_nGroup;
		int		m_bEndOfExit;
		short		m_nDamage [2];
		double	m_nLight;
		double	m_nCoord [3];
		int		m_nLastSegment;
		int		m_nLastSide;
		int		m_bSetDefTexture;

		CSpinButtonCtrl*	m_segIdSpinner;
		CSpinButtonCtrl*	m_segFuncSpinner;

		CSegmentTool (CPropertySheet *pParent = null);
		virtual BOOL OnInitDialog ();
		virtual void DoDataExchange (CDataExchange *pDX);
		virtual BOOL OnSetActive ();
		void InitCBSegmentId ();
		void Reset ();
		void EnableControls (BOOL bEnable);
		void Refresh ();
		void OnSide (int nSide);
		void OnPoint (int nPoint);
		int FindObjectInRobotMaker (CListBox *plb, LPSTR pszBot = null);
		int FindObjectInEquipMaker (CListBox *plb, LPSTR pszBot = null);
		bool IsRobotMaker (CSegment *pSegment);
		bool IsEquipMaker (CSegment *pSegment);
		void SetDefTexture (short nTexture);
	
		afx_msg void OnSetSegment ();
		afx_msg void OnSetType ();
		afx_msg void OnSetOwner ();
		afx_msg void OnSetGroup ();
		afx_msg void OnSetCoord ();
		afx_msg void OnResetCoord ();
		afx_msg void OnLight ();
		void OnDamage (int i);
		afx_msg void OnDamage0 ();
		afx_msg void OnDamage1 ();
		void OnProp (int nProp);
		afx_msg void OnProp1 ();
		afx_msg void OnProp2 ();
		afx_msg void OnProp3 ();
		afx_msg void OnProp4 ();
		afx_msg void OnProp5 ();
		afx_msg void OnProp6 ();
		afx_msg void OnProp7 ();
		void OnFlag (int nID, int& nFlag);
		afx_msg void OnSetDefTexture ();
		afx_msg void OnSide1 ();
		afx_msg void OnSide2 ();
		afx_msg void OnSide3 ();
		afx_msg void OnSide4 ();
		afx_msg void OnSide5 ();
		afx_msg void OnSide6 ();
		afx_msg void OnPoint1 ();
		afx_msg void OnPoint2 ();
		afx_msg void OnPoint3 ();
		afx_msg void OnPoint4 ();
		afx_msg void OnAddSegment ();
		afx_msg void OnAddProducer ();
		afx_msg void OnAddRepairCenter ();
		afx_msg void OnAddReactor ();
		afx_msg void OnSplitSegmentIn7 ();
		afx_msg void OnSplitSegmentIn8 ();
		afx_msg void OnCreateWedge ();
		afx_msg void OnCreatePyramid ();
		afx_msg void OnDeleteSegment ();
		afx_msg void OnOtherSegment ();
		afx_msg void OnAddObjectToProducer ();
		afx_msg void OnDeleteObjectFromProducer ();
		afx_msg void OnAddRobotMaker ();
		afx_msg void OnAddEquipMaker ();
		afx_msg void OnWallDetails ();
		afx_msg void OnTriggerDetails ();
		afx_msg void OnEndOfExit ();
		afx_msg void OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);

		void AddObjectToRobotMaker (void);
		void DeleteObjectFromRobotMaker (void);
		void AddObjectToEquipMaker (void);
		void DeleteObjectFromEquipMaker (void);

		inline CComboBox *CBSegmentId (void)
			{ return CBCtrl(IDC_SEGMENT_ID); }
		inline CComboBox *CBFunction (void)
			{ return CBCtrl(IDC_SEGMENT_FUNCTION); }
		inline CListBox *LBTriggers (void)
			{ return LBCtrl (IDC_SEGMENT_TRIGGERS); }
		inline CListBox *LBUsedBots (void)
			{ return LBCtrl (IDC_SEGMENT_USEDBOTS); }
		inline CListBox *LBAvailBots (void)
			{ return LBCtrl (IDC_SEGMENT_AVAILBOTS); }
		inline CButton *EndOfExit (void)
			{ return BtnCtrl (IDC_SEGMENT_ENDOFEXIT); }
		inline CButton *SetDefTexture (void)
			{ return BtnCtrl (IDC_SEGMENT_SETDEFTEXTURE); }
		inline CButton *Prop (int nProp)
			{ return BtnCtrl (IDC_SEGMENT_WATER + nProp); }
		inline CComboBox *CBOwner (void)
			{ return CBCtrl(IDC_SEGMENT_OWNER); }
		inline CSpinButtonCtrl* SegIdSpinner (void) 
			{ return (CSpinButtonCtrl*) GetDlgItem (IDC_SEGMENT_ID_SPINNER); }
		inline CSpinButtonCtrl* SegFuncSpinner (void) 
			{ return (CSpinButtonCtrl*) GetDlgItem (IDC_SEGMENT_FUNCTION_SPINNER); }

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CWallTool : public CTexToolDlg
{
	public:
		CWnd				m_textureWnd;
		int				m_nSegment;
		int				m_nSide;
		int				m_nTrigger;
		int				m_nWall [2];
		int				m_nType;
		int				m_nClip;
		double			m_nStrength;
		double			m_nCloak;
		int				m_bFlyThrough;
		int				m_bKeys [4];
		int				m_bFlags [MAX_WALL_FLAGS];
		char				m_szMsg [256];
		CWall*			m_pWall [2];
		CWall				m_defWall;
		CWall				m_defDoor;
		short				m_defTexture;
		short				m_defDoorTexture;
		short				m_defOvlTexture;
		BOOL				m_bBothSides;
		bool				m_bLock;
		bool				m_bDelayRefresh;
		CExtSliderCtrl	m_transpSlider;

		CWallTool (CPropertySheet *pParent = null);
		~CWallTool ();
      virtual BOOL OnInitDialog ();
      virtual void DoDataExchange (CDataExchange *pDX);
		virtual BOOL OnSetActive ();
		virtual BOOL OnKillActive ();
		void InitCBWallNo ();
		void Reset ();
		void EnableControls (BOOL bEnable);
		void Refresh ();
		virtual BOOL TextureIsVisible ();
		bool GetWalls ();
		CWall *GetOtherWall (void);

		afx_msg void OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		afx_msg void OnBothSides ();
		afx_msg void OnAddWall ();
		afx_msg void OnDeleteWall ();
		afx_msg void OnDeleteWallAll ();
		afx_msg void OnLock ();
		afx_msg void OnOtherSide ();
		afx_msg void OnSetWall ();
		afx_msg void OnSetType ();
		afx_msg void OnSetClip ();
		afx_msg void OnNoKey ();
		afx_msg void OnBlueKey ();
		afx_msg void OnGoldKey ();
		afx_msg void OnRedKey ();
		afx_msg void OnBlasted ();
		afx_msg void OnDoorOpen ();
		afx_msg void OnDoorLocked ();
		afx_msg void OnDoorAuto ();
		afx_msg void OnIllusionOff ();
		afx_msg void OnSwitch ();
		afx_msg void OnBuddyProof ();
		afx_msg void OnRenderAdditive ();
		afx_msg void OnIgnoreMarker ();
		afx_msg void OnAddDoorNormal ();
		afx_msg void OnAddDoorExit ();
		afx_msg void OnAddDoorSecretExit ();
		afx_msg void OnAddDoorPrison ();
		afx_msg void OnAddDoorGuideBot ();
		afx_msg void OnAddWallFuelCell ();
		afx_msg void OnAddWallIllusion ();
		afx_msg void OnAddWallForceField ();
		afx_msg void OnAddWallFan ();
		afx_msg void OnAddWallGrate ();
		afx_msg void OnAddWallWaterfall ();
		afx_msg void OnAddWallLavafall ();

		void OnKey (int i);
		void OnFlag (int i); 
		void OnStrength ();
		void OnCloak ();
		void UpdateTransparency (int nValue);

		inline CComboBox *CBWallNo ()
			{ return CBCtrl(IDC_WALL_WALLNO); }
		inline CComboBox *CBType ()
			{ return CBCtrl(IDC_WALL_TYPE); }
		inline CComboBox *CBClipNo ()
			{ return CBCtrl(IDC_WALL_CLIPNO); }
		inline CButton *KeyBtn (int i)
			{ return BtnCtrl (IDC_WALL_NOKEY + i); }
		inline CButton *FlagBtn (int i)
			{ return BtnCtrl (IDC_WALL_BLASTED + i); }
		inline CScrollBar *TransparencySlider (void)
			{ return (CScrollBar *) GetDlgItem (IDC_WALL_TRANSP_SLIDER); }
		inline CSpinButtonCtrl *TransparencySpinner (void)
			{ return (CSpinButtonCtrl *) GetDlgItem (IDC_WALL_TRANSP_SPINNER); }

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CTriggerTool : public CTexToolDlg
{
	public:
		int					m_nClass;
		int					m_nType;
		int					m_nTime;
		int					m_targets;
		int					m_iTarget;
		int					m_bD1Flags [MAX_TRIGGER_FLAGS];
		int					m_bD2Flags [16];
		int					m_nSliderValue;
		double				m_nStrength;
		CTrigger				m_defTrigger;
		CTrigger*			m_pTrigger;
		int					m_nTrigger;
		CTrigger*			m_pStdTrigger;
		int					m_nStdTrigger;
		CTrigger*			m_pObjTrigger;
		int					m_nObjTrigger;
		char					m_szTarget [40];
		int					m_bAutoAddWall;
		CWnd					m_showObjWnd;
		CWnd					m_showTexWnd;
		bool					m_bFindTrigger;

		CTriggerTool (CPropertySheet *pParent = null);
		~CTriggerTool ();
		virtual BOOL OnInitDialog ();
		virtual void DoDataExchange (CDataExchange *pDX);
		virtual BOOL OnSetActive ();
		virtual BOOL OnKillActive ();
		void LoadTextureListBoxes ();
		void InitCBTriggerNo ();
		void InitLBTargets ();
		void Reset ();
		void EnableControls (BOOL bEnable);
		void Refresh ();
		void AddTarget (short nSegment, short nSide);
		int FindTarget (short nSegment, short nSide);
		bool OnD1Flag (int i, int j = -1);
		void OnD2Flag (int i, int j = 0);
		void SetTriggerPtr (void);
		void ClearObjWindow (void);
		void DrawObjectImage ();
		virtual BOOL TextureIsVisible ();
		bool FindTrigger (short &nTrigger);
		bool TriggerHasSlider (void);
		int TriggerCount ();
		void OnSelect1st ();
		void OnSelect2nd ();
		void SelectTexture (int nIdC, bool bFirst);
		afx_msg void OnPaint ();
		afx_msg void OnAddTrigger ();
		afx_msg void OnDeleteTrigger ();
		afx_msg void OnDeleteTriggerAll ();
		afx_msg void OnSetTrigger ();
		afx_msg void OnSetType ();
		afx_msg void OnSetTarget ();
		afx_msg void OnAddTarget ();
		afx_msg void OnDeleteTarget ();
		afx_msg void OnAddWallTarget ();
		afx_msg void OnAddObjTarget ();
		afx_msg void OnCopyTrigger ();
		afx_msg void OnPasteTrigger ();
		afx_msg void OnStrength ();
		afx_msg void OnTime ();
		afx_msg void OnStandardTrigger ();
		afx_msg void OnObjectTrigger ();
		afx_msg void OnD2Flag1 ();
		afx_msg void OnD2Flag2 ();
		afx_msg void OnD2Flag3 ();
		afx_msg void OnD2Flag4 ();
		afx_msg void OnD2Flag5 ();
		afx_msg void OnD2Flag6 ();
		afx_msg void OnD2Flag7 ();
		afx_msg void OnD1Flag1 ();
		afx_msg void OnD1Flag2 ();
		afx_msg void OnD1Flag3 ();
		afx_msg void OnD1Flag4 ();
		afx_msg void OnD1Flag5 ();
		afx_msg void OnD1Flag6 ();
		afx_msg void OnD1Flag7 ();
		afx_msg void OnD1Flag8 ();
		afx_msg void OnD1Flag9 ();
		afx_msg void OnD1Flag10 ();
		afx_msg void OnD1Flag11 ();
		afx_msg void OnD1Flag12 ();
		afx_msg void OnAddOpenDoor ();
		afx_msg void OnAddRobotMaker ();
		afx_msg void OnAddShieldDrain ();
		afx_msg void OnAddEnergyDrain ();
		afx_msg void OnAddControlPanel ();
		afx_msg void OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		inline CComboBox *CBTriggerNo ()
			{ return CBCtrl(IDC_TRIGGER_TRIGGERNO); }
		inline CComboBox *CBType ()
			{ return CBCtrl(IDC_TRIGGER_D2TYPE); }
		inline CListBox *LBTargets ()
			{ return (CListBox *) GetDlgItem (IDC_TRIGGER_TARGETLIST); }
		inline CWnd *TargetEdit ()
			{ return GetDlgItem (IDC_TRIGGER_TARGET); }
		inline CScrollBar *SpeedBoostSlider ()
			{ return (CScrollBar *) GetDlgItem (IDC_TRIGGER_SLIDER); }
		inline CComboBox *CBTexture1 ()
			{ return CBCtrl(IDC_TRIGGER_TEXTURE1); }
		inline CComboBox *CBTexture2 ()
			{ return CBCtrl(IDC_TRIGGER_TEXTURE2); }
		inline short Texture1 (void)
			{ return (short) ((m_nTrigger >= 0) && (m_pTrigger && (m_nType == TT_CHANGE_TEXTURE)) ? m_pTrigger->Info ().value & 0xffff : 0); }
		inline short Texture2 (void)
			{ return (short) ((m_nTrigger >= 0) && (m_pTrigger && (m_nType == TT_CHANGE_TEXTURE)) ? m_pTrigger->Info ().value >> 16 : 0); }
		inline void SetTexture (short texture1, short texture2) {
			if ((m_nTrigger >= 0) && m_pTrigger && (m_nType == TT_CHANGE_TEXTURE)) {
				if (texture1 < 0)
					texture1 = Texture1 ();
				if (texture2 < 0)
					texture2 = Texture2 ();
				m_pTrigger->Info ().value = (int) (texture2 << 16) + (int) texture1; 
				}
			}
	
		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CTxtFilterTool : public CToolDlg
{
	public:
		CTxtFilterTool (CPropertySheet *pParent = null);
		~CTxtFilterTool ();
      virtual BOOL OnInitDialog ();
      virtual void DoDataExchange (CDataExchange *pDX);
		//virtual BOOL OnSetActive ();
		virtual BOOL OnKillActive ();
		
		void SetFilter (int i);
		void SetFilterRange (uint nFlags, int nValue);

		afx_msg void OnGrayRock ();
		afx_msg void OnBrownRock ();
		afx_msg void OnRedRock ();
		afx_msg void OnYellowRock ();
		afx_msg void OnGreenRock ();
		afx_msg void OnBlueRock ();
		afx_msg void OnIce ();
		afx_msg void OnStones ();
		afx_msg void OnGrass ();
		afx_msg void OnSand ();
		afx_msg void OnLava ();
		afx_msg void OnWater ();
		afx_msg void OnSteel ();
		afx_msg void OnConcrete ();
		afx_msg void OnBricks ();
		afx_msg void OnTarmac ();
		afx_msg void OnWalls ();
		afx_msg void OnFloors ();
		afx_msg void OnCeilings ();
		afx_msg void OnGrates ();
		afx_msg void OnFans ();
		afx_msg void OnLights ();
		afx_msg void OnEnergy ();
		afx_msg void OnForcefields ();
		afx_msg void OnTech ();
		afx_msg void OnLabels ();
		afx_msg void OnMonitors ();
		afx_msg void OnStripes ();
		afx_msg void OnMovers ();
		afx_msg void OnDoors ();
		afx_msg void OnSwitches ();
		afx_msg void OnRockAll ();
		afx_msg void OnRockNone ();
		afx_msg void OnNatureAll ();
		afx_msg void OnNatureNone ();
		afx_msg void OnBuildingAll ();
		afx_msg void OnBuildingNone ();
		afx_msg void OnTechAll ();
		afx_msg void OnTechNone ();
		afx_msg void OnSignAll ();
		afx_msg void OnSignNone ();
		afx_msg void OnOtherAll ();
		afx_msg void OnOtherNone ();
		afx_msg void OnTxtAll ();
		afx_msg void OnTxtNone ();
		afx_msg void OnTxtInvert ();

		DECLARE_MESSAGE_MAP()
};

//------------------------------------------------------------------------------

class CTextureTabDlg : public CTabDlg
{
	public:
		afx_msg void OnChange () { Refresh (); }
		virtual BOOL HandleTimer (UINT_PTR nIdEvent) { return FALSE; }

		CTextureTabDlg (UINT nId, CWnd* pParent = null) : CTabDlg (nId, pParent) {}

		void CreateImgWnd (CWnd * pImgWnd, int nIdC);
		void GetCtrlClientRect (CWnd *pWnd, CRect& rc);
};

//------------------------------------------------------------------------------

class CTextureAlignTool : public CTextureTabDlg
{
	public:
		CExtBitmapButton	m_btnZoomIn;
		CExtBitmapButton	m_btnZoomOut;
		CExtBitmapButton	m_btnHShrink;
		CExtBitmapButton	m_btnVShrink;
		CExtBitmapButton	m_btnHALeft;
		CExtBitmapButton	m_btnHARight;
		CExtBitmapButton	m_btnVAUp;
		CExtBitmapButton	m_btnVADown;
		CExtBitmapButton	m_btnRALeft;
		CExtBitmapButton	m_btnRARight;
		CBitmapButton		m_btnStretch2Fit;
		CBitmapButton		m_btnReset;
		CBitmapButton		m_btnResetTagged;
		CBitmapButton		m_btnAlignAll;
		CBitmapButton		m_btnChildAlign;
		CBitmapButton		m_btnHFlip;
		CBitmapButton		m_btnVFlip;
		CWnd				m_alignWnd;

		int					m_bShowTexture;
		int					m_bShowChildren;
		double				m_zoom;
		CPoint				m_apts [4];
		CPoint				m_minPt,
							m_maxPt,
							m_centerPt;
		double				m_alignX;
		double				m_alignY;
		double				m_alignAngle;
		double				m_alignUvPoint [4];
		int					m_alignRot2nd;
		BOOL				m_bIgnorePlane;
		BOOL				m_bIgnoreWalls;
		UINT				m_nTimerDelay;
		UINT_PTR			m_nTimer;
		UINT_PTR			m_nEditTimer;
		int					m_nEditFunc;

		CTextureAlignTool (UINT nId, CWnd* pParent = null) : CTextureTabDlg (nId, pParent) {}
		~CTextureAlignTool ();
		virtual BOOL OnInitDialog ();
		virtual void DoDataExchange (CDataExchange *pDX);
		virtual BOOL OnNotify (WPARAM wParam, LPARAM lParam, LRESULT *pResult);
		void OnEditTimer (void);
		virtual bool Refresh (void);
		virtual BOOL OnKillActive ();
		virtual BOOL HandleTimer (UINT_PTR nIdEvent);

		afx_msg void OnPaint ();
		afx_msg void OnAlignX ();
		afx_msg void OnAlignY ();
		afx_msg void OnAlignRot ();
		afx_msg void OnAlignLeft ();
		afx_msg void OnAlignRight ();
		afx_msg void OnAlignUp ();
		afx_msg void OnAlignDown ();
		afx_msg void OnAlignRotLeft ();
		afx_msg void OnAlignRotRight ();
		afx_msg void OnHShrink ();
		afx_msg void OnVShrink ();
		afx_msg void OnAlignReset ();
		afx_msg void OnAlignResetTagged ();
		afx_msg void OnAlignStretch2Fit ();
		afx_msg void OnAlignAll ();
		afx_msg void OnAlignChildren ();
		afx_msg void OnZoomIn ();
		afx_msg void OnZoomOut ();
		afx_msg void OnRot2nd0 ();
		afx_msg void OnRot2nd90 ();
		afx_msg void OnRot2nd180 ();
		afx_msg void OnRot2nd270 ();
		afx_msg void OnHFlip ();
		afx_msg void OnVFlip ();
		afx_msg void OnAlignIgnorePlane ();
		afx_msg void OnAlignIgnoreWalls ();

		void RefreshAlignment ();
		void UpdateAlignWnd (void);
		void RefreshAlignWnd ();
		void DrawAlignment (CDC *pDC);
		void RotateUV (double angle, bool bUpdate = true);
		void Rot2nd (int iAngle);
		void HAlign (int dir);
		void VAlign (int dir);
		void RAlign (int dir);
		void HFlip (void);
		void VFlip (void);
		void OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		void OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);

		inline CScrollBar *HScrollAlign (void)
			{ return (CScrollBar *) GetDlgItem (IDC_TEXALIGN_HSCROLL); }
		inline CScrollBar *VScrollAlign (void)
			{ return (CScrollBar *) GetDlgItem (IDC_TEXALIGN_VSCROLL); }

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CTextureProjectTool : public CTextureTabDlg
{
	public:
		CExtBitmapButton	m_btnProjPlanar;
		CExtBitmapButton	m_btnProjCylinder;
		CBitmapButton		m_btnOffXDown;
		CBitmapButton		m_btnOffXUp;
		CBitmapButton		m_btnOffYDown;
		CBitmapButton		m_btnOffYUp;
		CBitmapButton		m_btnOffZDown;
		CBitmapButton		m_btnOffZUp;
		CBitmapButton		m_btnRotPDown;
		CBitmapButton		m_btnRotPUp;
		CBitmapButton		m_btnRotBDown;
		CBitmapButton		m_btnRotBUp;
		CBitmapButton		m_btnRotHDown;
		CBitmapButton		m_btnRotHUp;
		CBitmapButton		m_btnScaleUDown;
		CBitmapButton		m_btnScaleUUp;
		CBitmapButton		m_btnScaleVDown;
		CBitmapButton		m_btnScaleVUp;

		double				m_projectRotP;
		double				m_projectRotB;
		double				m_projectRotH;

		CTextureProjectTool (UINT nId, CWnd* pParent = null) : CTextureTabDlg (nId, pParent) {}
		virtual BOOL OnInitDialog ();
		//virtual void EnableControls (BOOL bEnable);
		virtual void DoDataExchange (CDataExchange *pDX);
		virtual bool Refresh (void);

		afx_msg void OnProjectPlanar ();
		afx_msg void OnProjectCylinder ();
		afx_msg void OnProjectApply ();
		afx_msg void OnProjectCancel ();
		afx_msg void OnProjectOffsetX ();
		afx_msg void OnProjectOffsetY ();
		afx_msg void OnProjectOffsetZ ();
		afx_msg void OnProjectRotP ();
		afx_msg void OnProjectRotB ();
		afx_msg void OnProjectRotH ();
		afx_msg void OnProjectScaleU ();
		afx_msg void OnProjectScaleV ();
		afx_msg void OnProjectOffsetXUp ();
		afx_msg void OnProjectOffsetXDown ();
		afx_msg void OnProjectOffsetYUp ();
		afx_msg void OnProjectOffsetYDown ();
		afx_msg void OnProjectOffsetZUp ();
		afx_msg void OnProjectOffsetZDown ();
		afx_msg void OnProjectRotPUp ();
		afx_msg void OnProjectRotPDown ();
		afx_msg void OnProjectRotBUp ();
		afx_msg void OnProjectRotBDown ();
		afx_msg void OnProjectRotHUp ();
		afx_msg void OnProjectRotHDown ();
		afx_msg void OnProjectScaleUUp ();
		afx_msg void OnProjectScaleUDown ();
		afx_msg void OnProjectScaleVUp ();
		afx_msg void OnProjectScaleVDown ();
		afx_msg void OnProjectResetOffset ();
		afx_msg void OnProjectResetDirection ();
		afx_msg void OnProjectResetScaling ();
		afx_msg void OnProjectPreview ();
		afx_msg void OnProjectCopiedOnly ();

		void UpdateProjectWnd (void);
		void ClampProjectDirection ();
		void UpdateOrientation ();
		void StartProjectionTool (int nProjectionMode);
		void EndProjectionTool (BOOL bApply);

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CTextureLightTool : public CTextureTabDlg
{
	public:
		CBitmapButton		m_btnAddLight;
		CBitmapButton		m_btnDelLight;
		CExtSliderCtrl		m_lightTimerCtrl;
		CColorControl		m_lightWnd;
		CColorControl		m_colorWnd;
		CPaletteWnd			m_paletteWnd;

		UINT_PTR				m_nLightTimer;
		int					m_nLightDelay;
		double				m_nLightTime;
		int					m_nHighlight;
		char					m_szLight [33];
		int					m_iLight;
		BOOL					m_bLightEnabled;
		int					m_nColorIndex;
		PALETTEENTRY		m_rgbColor;

		CTextureLightTool (UINT nId, CWnd* pParent = null) : CTextureTabDlg (nId, pParent) {}
		~CTextureLightTool ();
		virtual BOOL OnInitDialog ();
		//virtual void EnableControls (BOOL bEnable);
		virtual void DoDataExchange (CDataExchange *pDX);
		virtual bool Refresh (void);
		virtual BOOL OnSetActive ();
		virtual BOOL OnKillActive ();
		virtual BOOL HandleTimer (UINT_PTR nIdEvent);

		afx_msg void OnPaint ();
		afx_msg void OnLight1 ();
		afx_msg void OnLight2 ();
		afx_msg void OnLight3 ();
		afx_msg void OnLight4 ();
		afx_msg void OnLight5 ();
		afx_msg void OnLight6 ();
		afx_msg void OnLight7 ();
		afx_msg void OnLight8 ();
		afx_msg void OnLight9 ();
		afx_msg void OnLight10 ();
		afx_msg void OnLight11 ();
		afx_msg void OnLight12 ();
		afx_msg void OnLight13 ();
		afx_msg void OnLight14 ();
		afx_msg void OnLight15 ();
		afx_msg void OnLight16 ();
		afx_msg void OnLight17 ();
		afx_msg void OnLight18 ();
		afx_msg void OnLight19 ();
		afx_msg void OnLight20 ();
		afx_msg void OnLight21 ();
		afx_msg void OnLight22 ();
		afx_msg void OnLight23 ();
		afx_msg void OnLight24 ();
		afx_msg void OnLight25 ();
		afx_msg void OnLight26 ();
		afx_msg void OnLight27 ();
		afx_msg void OnLight28 ();
		afx_msg void OnLight29 ();
		afx_msg void OnLight30 ();
		afx_msg void OnLight31 ();
		afx_msg void OnLight32 ();
		afx_msg void OnLightEdit ();
		afx_msg void OnLightOff ();
		afx_msg void OnLightOn ();
		afx_msg void OnLightStrobe4 ();
		afx_msg void OnLightStrobe8 ();
		afx_msg void OnLightFlicker ();
		afx_msg void OnLightDefault ();
		afx_msg void OnLightTimerEdit ();
		afx_msg void OnAddLight ();
		afx_msg void OnDeleteLight ();
		afx_msg void OnLButtonDown (UINT nFlags, CPoint point);
		afx_msg void OnSelectColor (void);

		void SetLightString ();
		void SetLightButtons (LPSTR szLight = null, int nSpeed = -1);
		bool SetLightDelay (int nSpeed = -1);
		void UpdateColorData (void);
		void UpdateLightWnd (void);
		void UpdateLight (void);
		void EnableLightControls (BOOL bEnable);
		void ToggleLight (int i);
		void AnimateLight (void);
		void SetWallColor (void);
		bool SideHasLight (void);
		//void CreateColorCtrl (CWnd *pWnd, int nIdC);
		//void UpdateColorCtrl (CWnd *pWnd, COLORREF color);
		void UpdatePaletteWnd (void);
		void OnHScroll(UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		void OnVScroll(UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		virtual BOOL OnNotify (WPARAM wParam, LPARAM lParam, LRESULT *pResult);

		inline CButton *LightButton (int i)
			{ return BtnCtrl (IDC_TEXLIGHT_1 + i); }
		inline CSliderCtrl *TimerSlider (void)
			{ return (CSliderCtrl *) GetDlgItem (IDC_TEXLIGHT_TIMERSLIDER); }

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CTextureTool : public CTexToolDlg
{
	public:
		CExtSliderCtrl		m_brightnessCtrl;
		CToolTabCtrl		m_textureTools;

		bool					m_bInitTextureListBoxes;

		int					m_lastTexture [2];
		int					m_lastMode;
		int					m_saveTexture [2];
		CUVL					m_saveUVLs [4];
//		int					frame [2];
		double				m_lights [4];
//		CWnd					m_textureWnd;
		int					m_bUse1st;
		int					m_bUse2nd;
		int					m_nBrightness;

		CTextureTool (CPropertySheet *pParent = null);
		~CTextureTool ();
      virtual BOOL OnInitDialog ();
      virtual void DoDataExchange (CDataExchange *pDX);
		virtual BOOL OnSetActive ();
		virtual BOOL OnKillActive ();
		afx_msg void LoadTextureListBoxes ();
		afx_msg void OnPaint ();
		afx_msg void OnTimer (UINT_PTR nIdEvent);
		afx_msg void OnSaveTexture ();
		afx_msg void OnEditTexture ();
		afx_msg void OnPasteSide ();
		afx_msg void OnPasteTouching ();
		afx_msg void OnTagPlane ();
		afx_msg void OnTagTextures ();
		afx_msg void OnReplace ();
		afx_msg void OnSetLight ();
		afx_msg void OnSelect1st ();
		afx_msg void OnSelect2nd ();
		afx_msg void OnPaste1st ();
		afx_msg void OnPaste2nd ();
		afx_msg void OnCleanup ();
		afx_msg void OnBrightnessEdit ();

		void GetBrightness (int nTexture);
		void SetBrightness (int nBrightness = 0);
		void AnimateTexture (void);

		void SelectTexture (int nIdC, bool bFirst);
		void PasteTexture (short nSegment, short nSide, short nDepth);
		bool GetAdjacentSide (short start_segment, short start_side, short linenum,
									 short *neighbor_segnum, short *neighbor_sidenum);
		void Refresh ();
		void RefreshTextureWnd ();
		void UpdateTextureWnd (void);
		void OnHScroll(UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		void OnVScroll(UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		void DrawTexture (short texture1, short texture2, int x0, int y0);
		int ScrollSpeed (ushort texture,int *x,int *y);

		inline CComboBox *CBTexture1 (void)
			{ return CBCtrl(IDC_TEXTURE_1ST); }
		inline CComboBox *CBTexture2 (void)
			{ return CBCtrl(IDC_TEXTURE_2ND); }
		inline CSliderCtrl *BrightnessSlider (void)
			{ return (CSliderCtrl *) GetDlgItem (IDC_TEXTURE_BRIGHTSLIDER); }
		inline CSpinButtonCtrl *BrightnessSpinner (void)
			{ return (CSpinButtonCtrl *) GetDlgItem (IDC_TEXTURE_BRIGHTSPINNER); }

		inline CTextureTabDlg* Current (void) { return (CTextureTabDlg*) m_textureTools.Current (); }

		DECLARE_MESSAGE_MAP()
};

//------------------------------------------------------------------------------

class CLightTool : public CToolDlg
{
	public:
		int				m_bIlluminate;
		int				m_bAvgCornerLight;
		int				m_bScaleLight;
		int				m_bSegmentLight;
		int				m_bDynSegmentLights;
		int				m_bDeltaLight;
		double			m_fVertexLight;
		int				m_nNoLightDeltas;
		int				m_staticRenderDepth;
		int				m_deltaRenderDepth;
		int				m_deltaLightFrameRate;
		int				m_bCopyTexLights;
		CExtSliderCtrl	m_renderControls [3];

		CLightTool (CPropertySheet *pParent = null);
      virtual BOOL OnInitDialog ();
		void SetDefaults (void);
		void OnOK (void);
		void OnCancel (void);
      virtual void DoDataExchange (CDataExchange *pDX);
		afx_msg void OnShowVariableLights ();
		afx_msg void OnSetQuality ();
		afx_msg void OnSetVertexLight ();
		afx_msg void OnDefaultLightAndColor ();
		afx_msg void OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);

		DECLARE_MESSAGE_MAP()
	};

//------------------------------------------------------------------------------

class CAppSettings {
	private:
		bool				m_bInited;
		bool				m_bInvalid;

	public:
		char				m_d1Folder [256];
		char				m_d2Folder [256];
		char				m_missionFolder [256];
		uint				m_mineViewFlags;
		uint				m_objViewFlags;
		uint				m_texViewFlags;
		int				m_depthPerception;
		double			m_depthPerceptions [4];
		int				m_iRotateRate;
		double			m_rotateRates [9];
		double			m_moveRate [2];
		int				m_movementMode;
		double			m_kbMoveScale;
		double			m_kbRotateScale;
		int				m_bFpInputLock;
		int				m_bExpertMode;
		int				m_bApplyFaceLightSettingsGlobally;
		int				m_bSortObjects;
		int				m_bAllowObjectOverlap;
		double			m_bumpIncrement;
		int				m_bUpdateTexAlign;
		int				m_nViewDist;
		int				m_nMineCenter;
		int				m_nRenderer;
		int				m_nPerspective;
		int				m_nMaxUndo;
		BOOL				m_bSplashScreen;
		BOOL				m_bDepthTest;

		void WritePrivateProfileInt (LPSTR szKey, int nValue);
		void WritePrivateProfileDouble (LPSTR szKey, double nValue);
		void Get (bool bGetFolders = true);
		void Set (int bUpdate = 0);
		void Setup (void);
		void Load (bool bInitialize = false);
		void Save (bool bSaveFolders = true);
		void SetPerspective (int nPerspective);
		void TogglePerspective (void);
		void ReloadTextures (int nVersion);
		void SetLayout (int nLayout);
		void SetRenderer (int nRenderer);
		void CompletePath (LPSTR pszPath, LPSTR pszFile, LPSTR pszExt);
	};

extern CAppSettings appSettings;

//------------------------------------------------------------------------------

class CSettingsTabDlg : public CTabDlg
{
	public:
		virtual void RefreshSettings (void) {}
		virtual void UpdateSettings (void);

		afx_msg void OnChange () { UpdateSettings (); }

		void InitSliders (CDialog* parent, CExtSliderCtrl* sliders, CSliderData* data, int nSliders);

		CSettingsTabDlg (UINT nId, CWnd* pParent = null) : CTabDlg (nId, pParent) {} 
};

//------------------------------------------------------------------------------

class CFileSettingsTool : public CSettingsTabDlg
{
	public:
		CBitmapButton	m_btnBrowseD1PIG;
		CBitmapButton	m_btnBrowseD2PIG;
		CBitmapButton	m_btnBrowseMissions;

      virtual BOOL OnInitDialog ();
      virtual void DoDataExchange (CDataExchange *pDX);
		bool BrowseFile (LPSTR fileType, LPSTR fileName, LPSTR fileExt, BOOL bOpen);

		afx_msg void OnOpenD1PIG (void);
		afx_msg void OnOpenD2PIG (void);
		afx_msg void OnOpenMissions (void);

		CFileSettingsTool (UINT nId, CWnd* pParent = null) : CSettingsTabDlg (nId, pParent) {}

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CLayoutSettingsTool : public CSettingsTabDlg
{
	public:
      virtual BOOL OnInitDialog ();
      virtual void DoDataExchange (CDataExchange *pDX);

		afx_msg void OnLayout0 (void);
		afx_msg void OnLayout1 (void);
		afx_msg void OnLayout2 (void);
		afx_msg void OnLayout3 (void);

		CLayoutSettingsTool (UINT nId, CWnd* pParent = null) : CSettingsTabDlg (nId, pParent) {}

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CRendererSettingsTool : public CSettingsTabDlg
{
	public:
      virtual BOOL OnInitDialog ();
      virtual void DoDataExchange (CDataExchange *pDX);

		void OnScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		void OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		void OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);

		afx_msg void OnRendererSW (void);
		afx_msg void OnRendererGL (void);
		afx_msg void OnSetMineCenter (void);

		CRendererSettingsTool (UINT nId, CWnd* pParent = null) : CSettingsTabDlg (nId, pParent) {}

		inline CSliderCtrl* ViewDistSlider () { return SlCtrl (IDC_PREFS_VIEWDIST); }
		inline CComboBox *CBMineCenter () { return CBCtrl (IDC_PREFS_MINECENTER); }

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CVisibilitySettingsTool : public CSettingsTabDlg
{
	public:
      virtual BOOL OnInitDialog ();
      virtual void DoDataExchange (CDataExchange *pDX);

		bool OnScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		void OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		void OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);

		afx_msg void OnViewObjectsNone (void);
		afx_msg void OnViewObjectsAll (void);
		afx_msg void OnViewMineNone (void);
		afx_msg void OnViewMineAll (void);

		CVisibilitySettingsTool (UINT nId, CWnd* pParent = null) : CSettingsTabDlg (nId, pParent) {}

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CEditorSettingsTool : public CSettingsTabDlg
{
	public:
		CExtSliderCtrl	m_rotateRate;

      virtual BOOL OnInitDialog ();
      virtual void DoDataExchange (CDataExchange *pDX);

		bool OnScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		void OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		void OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);

		CEditorSettingsTool (UINT nId, CWnd* pParent = null) : CSettingsTabDlg (nId, pParent) {}

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CMiscSettingsTool : public CSettingsTabDlg
{
	public:
      virtual BOOL OnInitDialog ();
      virtual void DoDataExchange (CDataExchange *pDX);

		bool OnScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		void OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
		void OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);

		CMiscSettingsTool (UINT nId, CWnd* pParent = null) : CSettingsTabDlg (nId, pParent) {}

		DECLARE_MESSAGE_MAP ()
};

//------------------------------------------------------------------------------

class CSettingsTool : public CToolDlg
{
	public:
		CToolTabCtrl	m_settingsTools;
		bool				m_bNoRefresh;
		bool				m_bInited;
		bool				m_bInvalid;

		CSettingsTool (CPropertySheet *pParent = null);
		~CSettingsTool ();
	   virtual BOOL OnInitDialog ();
      virtual void DoDataExchange (CDataExchange *pDX);
		virtual BOOL OnSetActive ();
		virtual void OnOK (void);
		virtual void OnCancel (void);
		void Refresh (void);
		void Update (bool bRefresh = true);

		inline CSettingsTabDlg* Current (void) { return (CSettingsTabDlg*) m_settingsTools.Current (); }

		DECLARE_MESSAGE_MAP()
};

//------------------------------------------------------------------------------

class CToolView : public CWnd {
	private:
		int				m_scrollRange [2];
		int				m_scrollPage [2];
		int				m_scrollOffs [2];
		CSize				m_paneSize;
		BOOL				m_bHScroll;
		BOOL				m_bVScroll;
		BOOL				m_bRecalcLayout;
		CToolDlg*		m_tools [12];

	public:
		DECLARE_DYNCREATE(CToolView)

		CPropertySheet	*m_pTools;
		CSegmentTool	*m_segmentTool;
		CWallTool		*m_wallTool;
		CTriggerTool	*m_triggerTool;
		CTextureTool	*m_textureTool;
		CObjectTool		*m_objectTool;
		CEffectTool		*m_effectTool;
		CLightTool		*m_lightTool;
		CSettingsTool	*m_settingsTool;
		CMissionTool	*m_missionTool;
		CReactorTool	*m_reactorTool;
		CDiagTool		*m_diagTool;
		CTxtFilterTool	*m_txtFilterTool;

		CToolView ();
		~CToolView ();
		void Destroy (void);
		int Setup ();
		void Reset () {};
		afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnDestroy (void);
		afx_msg void OnPaint ();
		afx_msg void OnSize (UINT nType, int cx, int cy);
		afx_msg BOOL OnEraseBkgnd (CDC* pDC);
		afx_msg void OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg void OnVScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg void OnSelectPrevTab ();
		afx_msg void OnSelectNextTab ();
		void CalcToolSize (void);
		void RecalcLayout (int nToolMode = -1, int nTextureMode = -1);
		void MoveWindow (int x, int y, int nWidth, int nHeight, BOOL bRepaint = TRUE);
		void MoveWindow (LPCRECT lpRect, BOOL bRepaint = TRUE);
		inline CSize& PaneSize () { return m_paneSize; }
		void SetActive (int nPage);
		inline void EditTexture () { SetActive (0); }
		inline void EditSegment () { SetActive (1); }
		inline void EditWall () { SetActive (2); }
		inline void EditTrigger () { SetActive (3); }
		inline void EditObject () { SetActive (4); }
		inline void EditEffect () { SetActive (5); }
		inline void EditLight () { SetActive (6); }
		inline void EditReactor () { SetActive (7); }
		inline void EditMission () { SetActive (8); }
		inline void EditDiag () { SetActive (9); }
		inline void EditTxtFilters () { SetActive (10); }
		inline void EditPrefs () { SetActive (11); }
		inline CSegmentTool *SegmentTool ()
			{ return m_segmentTool; }
		inline CTextureTool *TextureTool ()
			{ return m_textureTool; }
		inline CWallTool *WallTool ()
			{ return m_wallTool; }
		inline CTriggerTool *TriggerTool ()
			{ return m_triggerTool; }
		inline CLightTool *LightTool ()
			{ return m_lightTool; }
		inline CObjectTool *ObjectTool ()
			{ return m_objectTool; }
		inline CMissionTool *MissionTool ()
			{ return m_missionTool; }
		inline CDiagTool *DiagTool ()
			{ return m_diagTool; }
		inline CTxtFilterTool *TextureFilter ()
			{ return m_txtFilterTool; }
		inline CSettingsTool *SettingsTool ()
			{ return m_settingsTool; }
		void Refresh ();
		void CycleTab (int nDir);
		void NextTab ();
		void PrevTab ();
	DECLARE_MESSAGE_MAP()
};
                        
//------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif //__toolview_h
