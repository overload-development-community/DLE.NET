#ifndef __pogdialog_h
#define __pogdialog_h

class CPogDialog : public CDialog, private CDlgHelpers {
	public:
		CPogDialog (CWnd *pParentWnd = null, bool bLevelLoaded = false,
			bool bPreselectTexture = false, uint uiSelectedTexture = 0);
		~CPogDialog (void);

		virtual BOOL OnInitDialog (void);
		virtual BOOL DestroyWindow (void);
		virtual void DoDataExchange (CDataExchange *pDX);

	private:
		CImageList	m_customTextureIcons;
		CAnimTexWnd m_preview;

		enum TextureFilters : int {
			TextureFilters_Level = 0,
			TextureFilters_Robot,
			TextureFilters_Powerup,
			TextureFilters_Misc,
			NUM_TEXTUREFILTERS // must come last
			};

		struct TextureListItemData {
			uint uiIndex;
			bool bIsFrame;
			uint uiParentTextureIndex;
			};

		bool	m_bLevelLoaded;
		bool	m_bPaletteQueryDone;
		int	m_bShowCustomOnly; // int used for DDX_Check compatibility
		int	m_filters [NUM_TEXTUREFILTERS];
		bool	m_bPreselectTexture;
		uint	m_uiPreselectedTexture;
		int	m_nTexPreviousFocused, m_nTexCurrentFocused;
		int	m_iSavedSelectedItem;
		char	m_szPreviousPigPath [256];

		inline CListCtrl *TextureList (void) { return (CListCtrl *) GetDlgItem (IDC_POGMANAGER_LIST); }
		inline CComboBox *PaletteList (void) { return (CComboBox *) GetDlgItem (IDC_POGMANAGER_PALETTE); }
		void RebuildTextureList (void);
		void AddTextureListFrames (const CTexture *pTexture);
		uint RemoveTextureListFrames (const CTexture *pTexture);
		void AddTextureListItem (int nListItem, const CTexture *pTexture, bool asFrame);
		void UpdateTextureListItem (int nListItem);
		bool IsTextureIncluded (const CTexture *pTexture);
		TextureFilters ClassifyTexture (const CTexture *pTexture);
		static int CALLBACK CompareTextures (LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
		bool ParentTexturesAreEqual (const CTexture *pTexture, const CTexture *pOtherTexture);
		void UpdateTextureListFrameExpansion (void);
		void UpdateTexturePreviewAndControls (void);
		bool IsAnimatedTextureRoot (uint uiTextureListIndex);
		void ScrollTextureList (uint uiTextureListIndex);
		void SetFocusedTexture (uint uiTextureListIndex);
		int GetFocusedTextureIndex (void);
		int GetTextureListIndexFromId (uint nIndex);
		const CTexture *GetFocusedTexture (bool bGetRootTexture = false);
		const CTexture *GetTextureAtIndex (uint uiTextureListIndex);
		bool IsSingleTextureSelected (void);
		typedef void (CPogDialog::*ExecuteOnSelectedTexturesCallback)(const CTexture *pTexture, va_list args);
		bool ExecuteOnSelectedTextures (ExecuteOnSelectedTexturesCallback callback, ...);

		// Supports < 0 values to allow nulls
		inline const CTexture *CPogDialog::GetTextureFromId (int nIndex) {
			return (nIndex >= 0) ? textureManager.TextureByIndex (nIndex) : null;
		}
		inline TextureListItemData *GetTextureListItemData (uint uiTextureListIndex) {
			return (TextureListItemData *)TextureList ()->GetItemData (uiTextureListIndex);
		}

		afx_msg void OnTextureListSelectionChanged (NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnTextureListClick (NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnPaint (void);
		afx_msg void OnRevert (void);
		afx_msg void OnEdit (void);
		afx_msg void OnImport (void);
		afx_msg void OnExport (void);
		afx_msg void OnShowAll (void);
		afx_msg void OnChangeFilter (void);
		afx_msg void OnChangePalette (void);
		afx_msg void OnOK (void);
		afx_msg void OnCancel (void);

		void RevertTexture (const CTexture *pTexture, va_list args);
		void ExportTexture (const CTexture *pTexture, va_list args);

	DECLARE_MESSAGE_MAP ()
	};

#endif
