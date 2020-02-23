// TexProjectTool.cpp
//

#include "stdafx.h"
#include "dle-xp.h"
#include "toolview.h"

BEGIN_MESSAGE_MAP(CTextureProjectTool, CTextureTabDlg)
	ON_BN_CLICKED (IDC_TEXPROJECT_PLANAR, OnProjectPlanar)
	ON_BN_CLICKED (IDC_TEXPROJECT_CYLINDER, OnProjectCylinder)
	ON_BN_CLICKED (IDC_TEXPROJECT_APPLY, OnProjectApply)
	ON_BN_CLICKED (IDC_TEXPROJECT_CANCEL, OnProjectCancel)
	ON_BN_CLICKED (IDC_TEXPROJECT_ORIGINXUP, OnProjectOffsetXUp)
	ON_BN_CLICKED (IDC_TEXPROJECT_ORIGINXDOWN, OnProjectOffsetXDown)
	ON_BN_CLICKED (IDC_TEXPROJECT_ORIGINYUP, OnProjectOffsetYUp)
	ON_BN_CLICKED (IDC_TEXPROJECT_ORIGINYDOWN, OnProjectOffsetYDown)
	ON_BN_CLICKED (IDC_TEXPROJECT_ORIGINZUP, OnProjectOffsetZUp)
	ON_BN_CLICKED (IDC_TEXPROJECT_ORIGINZDOWN, OnProjectOffsetZDown)
	ON_BN_CLICKED (IDC_TEXPROJECT_ROTPUP, OnProjectRotPUp)
	ON_BN_CLICKED (IDC_TEXPROJECT_ROTPDOWN, OnProjectRotPDown)
	ON_BN_CLICKED (IDC_TEXPROJECT_ROTBUP, OnProjectRotBUp)
	ON_BN_CLICKED (IDC_TEXPROJECT_ROTBDOWN, OnProjectRotBDown)
	ON_BN_CLICKED (IDC_TEXPROJECT_ROTHUP, OnProjectRotHUp)
	ON_BN_CLICKED (IDC_TEXPROJECT_ROTHDOWN, OnProjectRotHDown)
	ON_BN_CLICKED (IDC_TEXPROJECT_USCALEUP, OnProjectScaleUUp)
	ON_BN_CLICKED (IDC_TEXPROJECT_USCALEDOWN, OnProjectScaleUDown)
	ON_BN_CLICKED (IDC_TEXPROJECT_VSCALEUP, OnProjectScaleVUp)
	ON_BN_CLICKED (IDC_TEXPROJECT_VSCALEDOWN, OnProjectScaleVDown)
	ON_BN_CLICKED (IDC_TEXPROJECT_RESETO, OnProjectResetOffset)
	ON_BN_CLICKED (IDC_TEXPROJECT_RESETD, OnProjectResetDirection)
	ON_BN_CLICKED (IDC_TEXPROJECT_RESETS, OnProjectResetScaling)
	ON_BN_CLICKED (IDC_TEXPROJECT_PREVIEW, OnProjectPreview)
	ON_BN_CLICKED (IDC_TEXPROJECT_COPIEDONLY, OnProjectCopiedOnly)
	ON_EN_KILLFOCUS (IDC_TEXPROJECT_ORIGINX, OnProjectOffsetX)
	ON_EN_KILLFOCUS (IDC_TEXPROJECT_ORIGINY, OnProjectOffsetY)
	ON_EN_KILLFOCUS (IDC_TEXPROJECT_ORIGINZ, OnProjectOffsetZ)
	ON_EN_KILLFOCUS (IDC_TEXPROJECT_ROTP, OnProjectRotP)
	ON_EN_KILLFOCUS (IDC_TEXPROJECT_ROTB, OnProjectRotB)
	ON_EN_KILLFOCUS (IDC_TEXPROJECT_ROTH, OnProjectRotH)
	ON_EN_KILLFOCUS (IDC_TEXPROJECT_USCALE, OnProjectScaleU)
	ON_EN_KILLFOCUS (IDC_TEXPROJECT_VSCALE, OnProjectScaleV)
END_MESSAGE_MAP()

BOOL CTextureProjectTool::OnInitDialog ()
{
if (!CTabDlg::OnInitDialog ())
	return FALSE;

m_projectRotP = 0;
m_projectRotB = 0;
m_projectRotH = 0;

m_btnProjPlanar.AutoLoad (IDC_TEXPROJECT_PLANAR, this);
m_btnProjCylinder.AutoLoad (IDC_TEXPROJECT_CYLINDER, this);
m_btnOffXDown.AutoLoad (IDC_TEXPROJECT_ORIGINXUP, this);
m_btnOffXUp.AutoLoad (IDC_TEXPROJECT_ORIGINXDOWN, this);
m_btnOffYDown.AutoLoad (IDC_TEXPROJECT_ORIGINYUP, this);
m_btnOffYUp.AutoLoad (IDC_TEXPROJECT_ORIGINYDOWN, this);
m_btnOffZDown.AutoLoad (IDC_TEXPROJECT_ORIGINZUP, this);
m_btnOffZUp.AutoLoad (IDC_TEXPROJECT_ORIGINZDOWN, this);
m_btnRotPDown.AutoLoad (IDC_TEXPROJECT_ROTPDOWN, this);
m_btnRotPUp.AutoLoad (IDC_TEXPROJECT_ROTPUP, this);
m_btnRotBDown.AutoLoad (IDC_TEXPROJECT_ROTBDOWN, this);
m_btnRotBUp.AutoLoad (IDC_TEXPROJECT_ROTBUP, this);
m_btnRotHDown.AutoLoad (IDC_TEXPROJECT_ROTHDOWN, this);
m_btnRotHUp.AutoLoad (IDC_TEXPROJECT_ROTHUP, this);
m_btnScaleUDown.AutoLoad (IDC_TEXPROJECT_USCALEDOWN, this);
m_btnScaleUUp.AutoLoad (IDC_TEXPROJECT_USCALEUP, this);
m_btnScaleVDown.AutoLoad (IDC_TEXPROJECT_VSCALEDOWN, this);
m_btnScaleVUp.AutoLoad (IDC_TEXPROJECT_VSCALEUP, this);

m_bInited = TRUE;
UpdateData (FALSE);
return TRUE;
}

void CTextureProjectTool::DoDataExchange (CDataExchange *pDX)
{
if (!HaveData (pDX)) 
	return;

if (!pDX->m_bSaveAndValidate) {
	textureProjector.m_projectOrient.CopyTo (m_projectRotP, m_projectRotB, m_projectRotH);
	m_projectRotP = Degrees (m_projectRotP);
	m_projectRotB = Degrees (m_projectRotB);
	m_projectRotH = Degrees (m_projectRotH);
	ClampProjectDirection ();
	}
DDX_Double (pDX, IDC_TEXPROJECT_ORIGINX, textureProjector.m_vCenter.v.x);
DDX_Double (pDX, IDC_TEXPROJECT_ORIGINY, textureProjector.m_vCenter.v.y);
DDX_Double (pDX, IDC_TEXPROJECT_ORIGINZ, textureProjector.m_vCenter.v.z);
DDX_Double (pDX, IDC_TEXPROJECT_ROTP, m_projectRotP);
DDX_Double (pDX, IDC_TEXPROJECT_ROTB, m_projectRotB);
DDX_Double (pDX, IDC_TEXPROJECT_ROTH, m_projectRotH);
DDX_Double (pDX, IDC_TEXPROJECT_USCALE, textureProjector.m_projectScaleU);
DDX_Double (pDX, IDC_TEXPROJECT_VSCALE, textureProjector.m_projectScaleV);
DDX_Check (pDX, IDC_TEXPROJECT_PREVIEW, textureProjector.m_bProjectPreview);
DDX_Check (pDX, IDC_TEXPROJECT_COPIEDONLY, textureProjector.m_bProjectCopiedOnly);
if (pDX->m_bSaveAndValidate)
	UpdateOrientation ();
}

bool CTextureProjectTool::Refresh (void)
{
	UpdateProjectWnd ();
	UpdateData (FALSE);
	return true;
}

void CTextureProjectTool::UpdateProjectWnd (void)
{
	CDlgHelpers::EnableControls (IDC_TEXPROJECT_ORIGINX, IDC_TEXPROJECT_COPIEDONLY, textureProjector.m_bActive);
}

void CTextureProjectTool::ClampProjectDirection ()
{
	// Keep display roughly in the -179 to 180 range
	if (m_projectRotP + 180 < 0.01)
		m_projectRotP += 360;
	else if (m_projectRotP > 180.01)
		m_projectRotP -= 360;
	if (m_projectRotB + 180 < 0.01)
		m_projectRotB += 360;
	else if (m_projectRotB > 180.01)
		m_projectRotB -= 360;
	if (m_projectRotH + 180 < 0.01)
		m_projectRotH += 360;
	else if (m_projectRotH > 180.01)
		m_projectRotH -= 360;
}

void CTextureProjectTool::UpdateOrientation ()
{
	ClampProjectDirection ();
	textureProjector.m_projectOrient.Set (sin (Radians (m_projectRotP)), cos (Radians (m_projectRotP)),
		sin (Radians (m_projectRotB)), cos (Radians (m_projectRotB)),
		sin (Radians (m_projectRotH)), cos (Radians (m_projectRotH)));
}

void CTextureProjectTool::StartProjectionTool (int nProjectionMode)
{
	UpdateData (TRUE);
	textureProjector.Start (nProjectionMode);

	if (textureProjector.m_bProjectPreview)
		DLE.MineView ()->Refresh (false);
	UpdateData (FALSE);
	UpdateProjectWnd ();
}

void CTextureProjectTool::EndProjectionTool (BOOL bApply)
{
	textureProjector.End (bApply);
	if (bApply || textureProjector.m_bProjectPreview)
		DLE.MineView ()->Refresh (false);

	// Clear UI and disable the controls
	UpdateData (FALSE);
	UpdateProjectWnd ();
}

void CTextureProjectTool::OnProjectPlanar ()
{
	StartProjectionTool (PROJECTION_MODE_PLANAR);
}

void CTextureProjectTool::OnProjectCylinder ()
{
	StartProjectionTool (PROJECTION_MODE_CYLINDER);
}

void CTextureProjectTool::OnProjectApply ()
{
	EndProjectionTool (TRUE);
}

void CTextureProjectTool::OnProjectCancel ()
{
	EndProjectionTool (FALSE);
}

void CTextureProjectTool::OnProjectOffsetXUp ()
{
	UpdateData (TRUE);
	textureProjector.m_vCenter.v.x += (DLE.MineView ()->MineMoveRate ());
	UpdateData (FALSE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectOffsetXDown ()
{
	UpdateData (TRUE);
	textureProjector.m_vCenter.v.x -= (DLE.MineView ()->MineMoveRate ());
	UpdateData (FALSE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectOffsetYUp ()
{
	UpdateData (TRUE);
	textureProjector.m_vCenter.v.y += (DLE.MineView ()->MineMoveRate ());
	UpdateData (FALSE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectOffsetYDown ()
{
	UpdateData (TRUE);
	textureProjector.m_vCenter.v.y -= (DLE.MineView ()->MineMoveRate ());
	UpdateData (FALSE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectOffsetZUp ()
{
	UpdateData (TRUE);
	textureProjector.m_vCenter.v.z += (DLE.MineView ()->MineMoveRate ());
	UpdateData (FALSE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectOffsetZDown ()
{
	UpdateData (TRUE);
	textureProjector.m_vCenter.v.z -= (DLE.MineView ()->MineMoveRate ());
	UpdateData (FALSE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectRotPUp ()
{
	UpdateData (TRUE);
	m_projectRotP += Degrees (theMine->RotateRate ());
	UpdateOrientation ();
	UpdateData (FALSE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectRotPDown ()
{
	UpdateData (TRUE);
	m_projectRotP -= Degrees (theMine->RotateRate ());
	UpdateOrientation ();
	UpdateData (FALSE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectRotBUp ()
{
	UpdateData (TRUE);
	m_projectRotB += Degrees (theMine->RotateRate ());
	UpdateOrientation ();
	UpdateData (FALSE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectRotBDown ()
{
	UpdateData (TRUE);
	m_projectRotB -= Degrees (theMine->RotateRate ());
	UpdateOrientation ();
	UpdateData (FALSE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectRotHUp ()
{
	UpdateData (TRUE);
	m_projectRotH += Degrees (theMine->RotateRate ());
	UpdateOrientation ();
	UpdateData (FALSE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectRotHDown ()
{
	UpdateData (TRUE);
	m_projectRotH -= Degrees (theMine->RotateRate ());
	UpdateOrientation ();
	UpdateData (FALSE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectScaleUUp ()
{
	UpdateData (TRUE);
	textureProjector.m_projectScaleU += (DLE.MineView ()->MineMoveRate () / 20.0);
	UpdateData (FALSE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectScaleUDown ()
{
	UpdateData (TRUE);
	textureProjector.m_projectScaleU -= (DLE.MineView ()->MineMoveRate () / 20.0);
	UpdateData (FALSE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectScaleVUp ()
{
	UpdateData (TRUE);
	textureProjector.m_projectScaleV += (DLE.MineView ()->MineMoveRate () / 20.0);
	UpdateData (FALSE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectScaleVDown ()
{
	UpdateData (TRUE);
	textureProjector.m_projectScaleV -= (DLE.MineView ()->MineMoveRate () / 20.0);
	UpdateData (FALSE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectOffsetX ()
{
	UpdateData (TRUE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectOffsetY ()
{
	UpdateData (TRUE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectOffsetZ ()
{
	UpdateData (TRUE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectRotP ()
{
	UpdateData (TRUE);
	UpdateData (FALSE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectRotB ()
{
	UpdateData (TRUE);
	UpdateData (FALSE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectRotH ()
{
	UpdateData (TRUE);
	UpdateData (FALSE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectScaleU ()
{
	UpdateData (TRUE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectScaleV ()
{
	UpdateData (TRUE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectResetOffset ()
{
	UpdateData (TRUE);
	textureProjector.ResetProjectOffset();
	UpdateData (FALSE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectResetDirection ()
{
	UpdateData (TRUE);
	textureProjector.ResetProjectDirection();
	UpdateData (FALSE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectResetScaling ()
{
	UpdateData (TRUE);
	textureProjector.ResetProjectScaling();
	UpdateData (FALSE);
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectPreview ()
{
	textureProjector.m_bProjectPreview = !textureProjector.m_bProjectPreview;
	DLE.MineView ()->Refresh (false);
}

void CTextureProjectTool::OnProjectCopiedOnly ()
{
	textureProjector.m_bProjectCopiedOnly = !textureProjector.m_bProjectCopiedOnly;
	DLE.MineView ()->Refresh (false);
}

//eof TexProjectTool.cpp