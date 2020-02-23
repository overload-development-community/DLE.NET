
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "dle-xp.h"

#include "MainFrame.h"
#include "global.h"
#include "toolview.h"
#include "PogDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

double gRotateRates [2] = {M_PI / 32.0, -M_PI / 256.0};
int gMoveRates [2] = {10, -5};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE (CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP (CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP (CMainFrame)
	ON_WM_CREATE ()
	ON_WM_CLOSE ()
	ON_WM_DESTROY ()
	ON_WM_SIZE ()
	ON_COMMAND_EX (ID_VIEW_TOOLBAR, OnBarCheck)
	ON_COMMAND (ID_TOOLS_EDITORTOOLBAR,OnEditorToolbar)
	ON_COMMAND (ID_FILE_INFORMATION, OnCheckMine)
	ON_COMMAND (ID_FILE_EXTBLKFMT, OnExtBlkFmt)
	ON_COMMAND (ID_FILE_CHECKMINE, OnCheckMine)
	ON_COMMAND (ID_CONVERTMINE_D1, OnConvertToD1)
	ON_COMMAND (ID_CONVERTMINE_STANDARD, OnConvertToStandard)
	ON_COMMAND (ID_CONVERTMINE_VERTIGO, OnConvertToVertigo)
	ON_COMMAND (ID_CONVERTMINE_D2X, OnConvertToD2X)
	ON_COMMAND (ID_CONVERTMINE_OVERLOAD, OnConvertToOverload)
	ON_COMMAND (ID_EDIT_UNDO, OnUndo)
	ON_COMMAND (ID_EDIT_REDO, OnRedo)
	ON_COMMAND (ID_EDITGEO_FWD, OnEditGeoFwd)
	ON_COMMAND (ID_EDITGEO_UP, OnEditGeoUp)
	ON_COMMAND (ID_EDITGEO_BACK, OnEditGeoBack)
	ON_COMMAND (ID_EDITGEO_LEFT, OnEditGeoLeft)
	ON_COMMAND (ID_EDITGEO_GROW, OnEditGeoGrow)
	ON_COMMAND (ID_EDITGEO_RIGHT, OnEditGeoRight)
	ON_COMMAND (ID_EDITGEO_ROTLEFT, OnEditGeoRotLeft)
	ON_COMMAND (ID_EDITGEO_DOWN, OnEditGeoDown)
	ON_COMMAND (ID_EDITGEO_ROTRIGHT, OnEditGeoRotRight)
	ON_COMMAND (ID_EDITGEO_SHRINK, OnEditGeoShrink)
	ON_COMMAND (ID_VIEW_REDRAW, OnRedraw)
	ON_COMMAND (ID_EDIT_TAG, OnEditTag)
	ON_COMMAND (ID_EDIT_TAGALL, OnEditTagAll)
	ON_COMMAND (ID_EDIT_UNTAG_ALL, OnEditUnTagAll)
	ON_COMMAND (ID_EDIT_TEXTURE, OnEditTexture)
	ON_COMMAND (ID_EDIT_SEGMENT, OnEditSegment)
	ON_COMMAND (ID_EDIT_WALL, OnEditWall)
	ON_COMMAND (ID_EDIT_OBJECT, OnEditObject)
	ON_COMMAND (ID_EDIT_LIGHT, OnEditLight)
	ON_COMMAND (ID_EDIT_PREFS, OnEditPreferences)
	ON_COMMAND (ID_VIEW_ZOOMIN, OnViewZoomin)
	ON_COMMAND (ID_VIEW_ZOOMOUT, OnViewZoomout)
	ON_COMMAND (ID_VIEW_FITTOVIEW, OnViewFitToView)
	ON_COMMAND (ID_VIEW_TOGGLE_PERSPECTIVE, OnViewTogglePerspective)
	ON_COMMAND (ID_MAKE_POINTS_PARALLEL, OnMakePointsParallel)
	ON_COMMAND (ID_VIEW_COLLAPSE_EDGE, OnViewCollapseEdge)
	ON_COMMAND (ID_VIEW_CREATE_WEDGE, OnViewCreateWedge)
	ON_COMMAND (ID_VIEW_CREATE_PYRAMID, OnViewCreatePyramid)
	ON_COMMAND (ID_VIEW_SHIFT_VERTICES, OnViewShiftVertices)
	ON_COMMAND (ID_VIEW_TOGGLEVIEWS, OnToggleViews)
	ON_COMMAND (ID_VIEW_TOGGLETEXPANE, OnToggleTexturePane)
	ON_COMMAND (ID_FILE_PREFERENCES, OnEditPreferences)
	ON_COMMAND (ID_FILE_EDITMISSIONFILE, OnEditMission)
	ON_COMMAND (ID_FILE_EDITHOG, OnEditHog)
	ON_COMMAND (ID_FILE_EDITPOG, OnEditPog)
	ON_COMMAND (ID_VIEW_ALIGNSIDEROTATION, OnViewAlignSideRotation)
	ON_COMMAND (ID_VIEW_ALLLINES, OnViewWireFrameFull)
	ON_COMMAND (ID_VIEW_CENTERENTIREMINE, OnViewCenterEntireMine)
	ON_COMMAND (ID_VIEW_CENTERONCURRENTCUBE, OnViewCenterOnCurrentSegment)
	ON_COMMAND (ID_VIEW_CENTERONCURRENTOBJECT, OnViewCenterOnCurrentObject)
	ON_COMMAND (ID_VIEW_HIDELINES, OnViewHidelines)
	ON_COMMAND (ID_VIEW_LIGHTS, OnViewLights)
	ON_COMMAND (ID_VIEW_NEARBYCUBELINES, OnViewNearbySegmentLines)
	ON_COMMAND (ID_VIEW_OBJECTS_ALLOBJECTS, OnViewObjectsAll)
	ON_COMMAND (ID_VIEW_OBJECTS_CONTROLCENTER, OnViewObjectsReactor)
	ON_COMMAND (ID_VIEW_OBJECTS_HOSTAGES, OnViewObjectsHostages)
	ON_COMMAND (ID_VIEW_OBJECTS_KEYS, OnViewObjectsKeys)
	ON_COMMAND (ID_VIEW_OBJECTS_NOOBJECTS, OnViewObjectsNone)
	ON_COMMAND (ID_VIEW_OBJECTS_PLAYERS, OnViewObjectsPlayers)
	ON_COMMAND (ID_VIEW_OBJECTS_POWERUPS, OnViewObjectsPowerups)
	ON_COMMAND (ID_VIEW_OBJECTS_ROBOTS, OnViewObjectsRobots)
	ON_COMMAND (ID_VIEW_OBJECTS_WEAPONS, OnViewObjectsWeapons)
	ON_COMMAND (ID_VIEW_PAN_DOWN, OnViewPanDown)
	ON_COMMAND (ID_VIEW_PAN_IN, OnViewPanIn)
	ON_COMMAND (ID_VIEW_PAN_LEFT, OnViewPanLeft)
	ON_COMMAND (ID_VIEW_PAN_OUT, OnViewPanOut)
	ON_COMMAND (ID_VIEW_PAN_RIGHT, OnViewPanRight)
	ON_COMMAND (ID_VIEW_PAN_UP, OnViewPanUp)
	ON_COMMAND (ID_VIEW_PARTIALLINES, OnViewWireFrameSparse)
	ON_COMMAND (ID_VIEW_ROTATE_CLOCKWISE, OnViewRotateClockwise)
	ON_COMMAND (ID_VIEW_ROTATE_COUNTERCLOCKWISE, OnViewRotateCounterclockwise)
	ON_COMMAND (ID_VIEW_ROTATE_LEFT, OnViewRotateLeft)
	ON_COMMAND (ID_VIEW_ROTATE_RIGHT, OnViewRotateRight)
	ON_COMMAND (ID_VIEW_ROTATE_DOWN, OnViewRotateDown)
	ON_COMMAND (ID_VIEW_ROTATE_UP, OnViewRotateUp)
	ON_COMMAND (ID_VIEW_SHADING, OnViewShading)
	ON_COMMAND (ID_VIEW_DELTALIGHTS, OnViewDeltaLights)
	ON_COMMAND (ID_VIEW_SPECIAL, OnViewSpecial)
	ON_COMMAND (ID_VIEW_TEXTUREMAPPED, OnViewTextured)
	ON_COMMAND (ID_VIEW_WALLS, OnViewWalls)
	ON_COMMAND (ID_VIEW_ALLTEXTURES, OnViewUsedTextures)
	ON_COMMAND (ID_SEL_PREV_TAB, OnSelectPrevTab)
	ON_COMMAND (ID_SEL_NEXT_TAB, OnSelectNextTab)
	ON_COMMAND (ID_SEL_POINTMODE, OnSelectPointMode)
	ON_COMMAND (ID_SEL_EDGEMODE, OnSelectEdgeMode)
	ON_COMMAND (ID_SEL_SIDEMODE, OnSelectSideMode)
	ON_COMMAND (ID_SEL_SEGMENTMODE, OnSelectCubeMode)
	ON_COMMAND (ID_SEL_OBJECTMODE, OnSelectObjectMode)
	ON_COMMAND (ID_SEL_BLOCKMODE, OnSelectBlockMode)
	ON_COMMAND (ID_JOIN_POINTS, OnJoinPoints)
	ON_COMMAND (ID_SPLIT_POINTS, OnSplitPoints)
	ON_COMMAND (ID_JOIN_LINES, OnJoinLines)
	ON_COMMAND (ID_SPLIT_LINES, OnSplitLines)
	ON_COMMAND (ID_JOIN_SIDES, OnJoinSides)
	ON_COMMAND (ID_SPLIT_SIDES, OnSplitSides)
	ON_COMMAND (ID_JOIN_CURRENTSIDE, OnJoinCurrentSide)
	ON_COMMAND (ID_SPLIT_CURRENTSIDE, OnSplitCurrentSide)
	ON_COMMAND (ID_JOIN_SEGMENTS, OnJoinSegments)
	ON_COMMAND (ID_INSMODE_NORMAL, OnInsModeNormal)
	ON_COMMAND (ID_INSMODE_EXTEND, OnInsModeExtend)
	ON_COMMAND (ID_INSMODE_MIRROR, OnInsModeMirror)
	ON_COMMAND (ID_TOGGLE_INSMODE, OnToggleInsMode)
	ON_COMMAND (ID_SEL_NEXT_POINT, OnSelNextPoint)
	ON_COMMAND (ID_SEL_PREV_POINT, OnSelPrevPoint)
	ON_COMMAND (ID_SEL_NEXT_LINE, OnSelNextLine)
	ON_COMMAND (ID_SEL_PREV_LINE, OnSelPrevLine)
	ON_COMMAND (ID_SEL_NEXT_SIDE, OnSelNextSide)
	ON_COMMAND (ID_SEL_PREV_SIDE, OnSelPrevSide)
	ON_COMMAND (ID_SEL_NEXT_SEGMENT, OnSelNextSegment)
	ON_COMMAND (ID_SEL_PREV_SEGMENT, OnSelPrevSegment)
	ON_COMMAND (ID_SEL_NEXT_OBJECT, OnSelNextObject)
	ON_COMMAND (ID_SEL_PREV_OBJECT, OnSelPrevObject)
	ON_COMMAND (ID_SEL_SEGMENT_FORWARD, OnSelSegmentForward)
	ON_COMMAND (ID_SEL_SEGMENT_BACKWARDS, OnSelSegmentBackwards)
	ON_COMMAND (ID_SEL_SEGMENT_FORWARD_KEY, OnSelSegmentForwardKey)
	ON_COMMAND (ID_SEL_SEGMENT_BACKWARDS_KEY, OnSelSegmentBackwardsKey)
	ON_COMMAND (ID_SEL_OTHER_SEGMENT, OnSelOtherSegment)
	ON_COMMAND (ID_SEL_OTHER_SIDE, OnSelOtherSide)
	ON_COMMAND (ID_MAKE_POINTS_PARALLEL, OnMakePointsParallel)
	ON_COMMAND (ID_VIEW_COLLAPSE_EDGE, OnCollapseEdge)
	ON_COMMAND (ID_VIEW_CREATE_WEDGE, OnCreateWedge)
	ON_COMMAND (ID_VIEW_CREATE_PYRAMID, OnCreatePyramid)
	ON_COMMAND (ID_EDIT_RECALCULATE_CENTER, OnEditRecalculateCenter)
	ON_COMMAND (ID_EDIT_SET_CENTER, OnEditSetCenter)
	ON_COMMAND (ID_SEL_NEXT_SEGMENT_ELEM, OnSelNextSegmentElem)
	ON_COMMAND (ID_SEL_PREV_SEGMENT_ELEM, OnSelPrevSegmentElem)
	ON_COMMAND (ID_SEL_NEXT_SEGMENT_ELEM_KEY, OnSelNextSegmentElemKey)
	ON_COMMAND (ID_SEL_PREV_SEGMENT_ELEM_KEY, OnSelPrevSegmentElemKey)
	ON_COMMAND (ID_TOOLS_TEXTUREEDIT, OnEditTexture)
	ON_COMMAND (ID_TOOLS_SEGMENTEDIT, OnEditSegment)
	ON_COMMAND (ID_TOOLS_WALLEDIT, OnEditWall)
	ON_COMMAND (ID_TOOLS_TRIGGEREDIT, OnEditTrigger)
	ON_COMMAND (ID_TOOLS_OBJECTEDIT, OnEditObject)
	ON_COMMAND (ID_TOOLS_EFFECTEDIT, OnEditEffect)
	ON_COMMAND (ID_TOOLS_LIGHTADJUSTMENT, OnEditLight)
	ON_COMMAND (ID_TOOLS_REACTORTRIGGERS, OnEditReactor)
	ON_COMMAND (ID_TOOLS_MISSIONEDIT, OnEditMission)
	ON_COMMAND (ID_TOOLS_DIAGNOSIS, OnEditDiagnostics)
	ON_COMMAND (ID_TOOLS_TXTFILTERS, OnTxtFilters)
	ON_COMMAND (ID_TOOLS_PREFERENCES, OnEditPreferences)
	ON_COMMAND (ID_TOOLS_CURVEGENERATOR, OnTunnelGenerator)
	ON_COMMAND (ID_SPLINE_INCREASE, OnIncSpline)
	ON_COMMAND (ID_SPLINE_DECREASE, OnDecSpline)
	ON_COMMAND (ID_FINER_TUNNEL, OnFinerTunnel)
	ON_COMMAND (ID_COARSER_TUNNEL, OnCoarserTunnel)
	ON_UPDATE_COMMAND_UI (ID_FILE_EDITHOG, OnUpdateEditHog)
	//ON_UPDATE_COMMAND_UI (ID_FILE_EDITPOG, OnUpdateEditPog)
	ON_UPDATE_COMMAND_UI (ID_VIEW_TOGGLEVIEWS, OnUpdateToggleViews)
	ON_UPDATE_COMMAND_UI (ID_VIEW_TOGGLETEXPANE, OnUpdateToggleTexPane)
	ON_UPDATE_COMMAND_UI (ID_FILE_EXTBLKFMT, OnUpdateExtBlkFmt)
	ON_UPDATE_COMMAND_UI (ID_TOOLS_EDITORTOOLBAR,OnUpdateEditorToolbar)
	ON_UPDATE_COMMAND_UI (ID_INSMODE_NORMAL, OnUpdateInsModeNormal)
	ON_UPDATE_COMMAND_UI (ID_INSMODE_EXTEND, OnUpdateInsModeExtend)
	ON_UPDATE_COMMAND_UI (ID_INSMODE_MIRROR, OnUpdateInsModeMirror)
	ON_UPDATE_COMMAND_UI (ID_VIEW_ALLTEXTURES, OnUpdateViewUsedTextures)
	ON_UPDATE_COMMAND_UI (ID_VIEW_WALLS, OnUpdateViewWalls)
	ON_UPDATE_COMMAND_UI (ID_VIEW_SPECIAL, OnUpdateViewSpecial)
	ON_UPDATE_COMMAND_UI (ID_VIEW_SHADING, OnUpdateViewShading)
	ON_UPDATE_COMMAND_UI (ID_VIEW_DELTALIGHTS, OnUpdateViewDeltaLights)
	ON_UPDATE_COMMAND_UI (ID_VIEW_PARTIALLINES, OnUpdateViewPartiallines)
	ON_UPDATE_COMMAND_UI (ID_VIEW_OBJECTS_WEAPONS, OnUpdateViewObjectsWeapons)
	ON_UPDATE_COMMAND_UI (ID_VIEW_OBJECTS_ROBOTS, OnUpdateViewObjectsRobots)
	ON_UPDATE_COMMAND_UI (ID_VIEW_OBJECTS_POWERUPS, OnUpdateViewObjectsPowerups)
	ON_UPDATE_COMMAND_UI (ID_VIEW_OBJECTS_PLAYERS, OnUpdateViewObjectsPlayers)
	ON_UPDATE_COMMAND_UI (ID_VIEW_OBJECTS_NOOBJECTS, OnUpdateViewObjectsNoobjects)
	ON_UPDATE_COMMAND_UI (ID_VIEW_OBJECTS_KEYS, OnUpdateViewObjectsKeys)
	ON_UPDATE_COMMAND_UI (ID_VIEW_OBJECTS_HOSTAGES, OnUpdateViewObjectsHostages)
	ON_UPDATE_COMMAND_UI (ID_VIEW_OBJECTS_CONTROLCENTER, OnUpdateViewObjectsControlcenter)
	ON_UPDATE_COMMAND_UI (ID_VIEW_OBJECTS_ALLOBJECTS, OnUpdateViewObjectsAllobjects)
	ON_UPDATE_COMMAND_UI (ID_VIEW_NEARBYCUBELINES, OnUpdateViewNearbySegmentLines)
	ON_UPDATE_COMMAND_UI (ID_VIEW_LIGHTS, OnUpdateViewLights)
	ON_UPDATE_COMMAND_UI (ID_VIEW_HIDELINES, OnUpdateViewHidelines)
	ON_UPDATE_COMMAND_UI (ID_VIEW_ALLLINES, OnUpdateViewAlllines)
	ON_UPDATE_COMMAND_UI (ID_VIEW_TEXTUREMAPPED, OnUpdateViewTexturemapped)
	ON_UPDATE_COMMAND_UI (ID_VIEW_COLLAPSE_EDGE, OnUpdateViewCollapse)
	ON_UPDATE_COMMAND_UI (ID_VIEW_CREATE_WEDGE, OnUpdateViewCollapse)
	ON_UPDATE_COMMAND_UI (ID_VIEW_CREATE_PYRAMID, OnUpdateViewCollapse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()

//------------------------------------------------------------------------------
// CMainFrame message handlers

void CMainFrame::ResetMineZoom ()
{
if (m_mineZoom) {
	MineView ()->ZoomOut (m_mineZoom);
	m_mineZoom = 0;
	}
}

void CMainFrame::ResetPaneMode ()
{
if (m_paneMode) {
	m_paneMode = 0;
	ResetMineZoom ();
	m_toolBar.SetButtonInfo (44, ID_VIEW_TOGGLEVIEWS, TBBS_BUTTON, 38);
	}
}

void CMainFrame::ShowTools ()
{
RecalcLayout (1,1);
if (m_paneMode == 1)
	ResetMineZoom ();
m_paneMode = 0;
}

void CMainFrame::OnEditTag () 
{
segmentManager.TagSelected ();
}

void CMainFrame::OnEditTagAll () 
{
segmentManager.TagAll ();
}

void CMainFrame::OnEditUnTagAll () 
{
segmentManager.UnTagAll ();
}

void CMainFrame::OnEditTexture () { 
ShowTools ();
ToolView ()->EditTexture (); 
}

void CMainFrame::OnEditWall () 
{
ShowTools ();
ToolView ()->EditWall ();
}

void CMainFrame::OnEditSegment () 
{
ShowTools ();
ToolView ()->EditSegment ();
}

void CMainFrame::OnEditTrigger () 
{
ShowTools ();
ToolView ()->EditTrigger ();
}

void CMainFrame::OnEditObject () 
{
ShowTools ();
ToolView ()->EditObject ();
}

void CMainFrame::OnEditEffect () 
{
ShowTools ();
ToolView ()->EditEffect ();
}

void CMainFrame::OnEditLight () 
{
ShowTools ();
ToolView ()->EditLight ();
ToolView ()->LightTool ()->OnOK ();
}

void CMainFrame::OnEditMission () 
{
ShowTools ();
ToolView ()->EditMission ();
}

void CMainFrame::OnEditHog ()
{
hogManager->Setup (DLE.GetDocument ()->File (), null);
if (hogManager->DoModal () == IDOK)
	DLE.GetDocument ()->OpenFile (false, hogManager->MissionName (), hogManager->LevelName ());
}

void CMainFrame::OnEditPog ()
{
CPogDialog e (DLE.MainFrame (), true);
e.DoModal ();
MineView ()->Refresh ();
}

void CMainFrame::OnEditDiagnostics () 
{
ShowTools ();
ToolView ()->EditDiag ();
}

void CMainFrame::OnEditPreferences () 
{
ShowTools ();
ToolView ()->EditPrefs ();
}

void CMainFrame::OnEditReactor () 
{
ShowTools ();
ToolView ()->EditReactor ();
}

void CMainFrame::OnTunnelGenerator () 
{
ShowTools ();
tunnelMaker.Run ();
}

void CMainFrame::OnTxtFilters () 
{
ShowTools ();
ToolView ()->EditTxtFilters ();
}

//------------------------------------------------------------------------------

void CMainFrame::OnIncSpline () 
{
tunnelMaker.Stretch ();
}

void CMainFrame::OnDecSpline () 
{
tunnelMaker.Shrink ();
}

void CMainFrame::OnFinerTunnel () 
{
tunnelMaker.Finer ();
}

void CMainFrame::OnCoarserTunnel () 
{
tunnelMaker.Coarser ();
}

void CMainFrame::OnRedraw () 
{
MineView ()->Refresh ();
}

void CMainFrame::OnToggleViews () 
{
//if (nLayout < 2) 
	{
	if (m_paneMode == 1) {
		RecalcLayout (0,0);
		m_paneMode = 0;
		m_toolBar.SetButtonInfo (44, ID_VIEW_TOGGLEVIEWS, TBBS_BUTTON, 38);
		//ResetMineZoom ();
		}
	else {
		RecalcLayout (1,1);
		m_paneMode = 1;
		m_toolBar.SetButtonInfo (44, ID_VIEW_TOGGLEVIEWS, TBBS_BUTTON, 39);
		//m_mineZoom = MineView ()->ZoomIn (2);
		}
	}
}

void CMainFrame::OnToggleTexturePane () 
{
if (!bSingleToolPane)
	RecalcLayout (m_toolMode, (m_textureMode == 1) ? 2 : 1);
}

void CMainFrame::OnViewZoomin () 
{
MineView ()->ZoomIn ();
}

void CMainFrame::OnViewZoomout () 
{
MineView ()->ZoomOut ();
}

void CMainFrame::OnViewFitToView () 
{
MineView ()->FitToView ();
}

void CMainFrame::OnViewTogglePerspective () 
{
appSettings.TogglePerspective ();
}

void CMainFrame::OnMakePointsParallel () 
{
segmentManager.MakePointsParallel ();
}

void CMainFrame::OnViewCollapseEdge () 
{
if (segmentManager.CollapseEdge ())
	MineView ()->NextSide ();
}

void CMainFrame::OnViewCreateWedge () 
{
segmentManager.CreateWedge ();
}

void CMainFrame::OnViewCreatePyramid () 
{
segmentManager.CreatePyramid ();
}

void CMainFrame::OnViewShiftVertices () 
{
current->Segment ()->ShiftVertices (current->SideId ());
}

void CMainFrame::OnViewAlignSideRotation () 
{
MineView ()->AlignSide ();	
}

void CMainFrame::OnViewCenterEntireMine () 
{
MineView ()->CenterOnMine ();	
}

void CMainFrame::OnViewCenterOnCurrentSegment () 
{
MineView ()->CenterOnSegment ();	
}

void CMainFrame::OnViewCenterOnCurrentObject () 
{
MineView ()->CenterOnObject ();
}

//------------------------------------------------------------------------------

void CMainFrame::OnViewWireFrameFull () 
{
CMineView* mineView = MineView ();
if ((mineView->Renderer ().Type () == 1) && mineView->ViewOption (eViewTextured))
	mineView->SetViewOption (eViewTexturedWireFrame);
else
	mineView->SetViewOption (eViewWireFrameFull);
}

void CMainFrame::OnViewHidelines () 
{
MineView ()->SetViewOption (eViewHideLines);
}

void CMainFrame::OnViewNearbySegmentLines () 
{
MineView ()->SetViewOption (eViewNearbyCubeLines);
}

void CMainFrame::OnViewWireFrameSparse () 
{
CMineView* mineView = MineView ();
if ((mineView->Renderer ().Type () == 1) && mineView->ViewOption (eViewTextured))
	mineView->SetViewOption (eViewTexturedWireFrame);
else
	mineView->SetViewOption (eViewWireFrameSparse);
}

void CMainFrame::OnViewTextured () 
{
CMineView* mineView = MineView ();
if ((mineView->Renderer ().Type () == 1) && (mineView->ViewOption (eViewWireFrameFull) || mineView->ViewOption (eViewWireFrameSparse)))
	mineView->SetViewOption (eViewTexturedWireFrame);
else
	mineView->SetViewOption (eViewTextured);
}

//------------------------------------------------------------------------------

void CMainFrame::OnViewLights () 
{
MineView ()->ToggleViewMine (eViewMineLights);
}
void CMainFrame::OnViewShading () 
{
MineView ()->ToggleViewMine (eViewMineIllumination);
}
void CMainFrame::OnViewDeltaLights () 
{
MineView ()->ToggleViewMine (eViewMineVariableLights);
}
void CMainFrame::OnViewWalls () 
{
MineView ()->ToggleViewMine (eViewMineWalls);
}
void CMainFrame::OnViewSpecial () 
{
MineView ()->ToggleViewMine (eViewMineSpecial);
}
void CMainFrame::OnViewUsedTextures () 
{
GetTextureView ()->ToggleViewFlag (eViewMineUsedTextures);
GetToolView ()->SettingsTool ()->Refresh ();
}

void CMainFrame::OnJoinPoints ()
{
segmentManager.JoinPoints ();
}

void CMainFrame::OnJoinLines ()
{
segmentManager.JoinLines ();
}

void CMainFrame::OnJoinSides ()
{
segmentManager.Join (*current, false);
}

void CMainFrame::OnJoinCurrentSide ()
{
segmentManager.Join (*current, true);
}

void CMainFrame::OnJoinSegments ()
{
segmentManager.JoinSegments ();
}

void CMainFrame::OnSplitPoints ()
{
segmentManager.SeparatePoints ();
}

void CMainFrame::OnSplitLines ()
{
segmentManager.SeparateLines ();
}

void CMainFrame::OnSplitSides ()
{
segmentManager.SeparateSegments ();
}

void CMainFrame::OnSplitCurrentSide ()
{
segmentManager.SeparateSegments ();
}

void CMainFrame::UpdateInsModeButtons (short mode)
{
	static char *szInsMode [] = {" insert: normal", " insert: extend", " insert: mirror"};

int i;
for (i = 0; i <= ID_INSMODE_MIRROR - ID_INSMODE_NORMAL; i++)
	m_toolBar.GetToolBarCtrl ().CheckButton (ID_INSMODE_NORMAL + i, i == mode);
InsModeMsg (szInsMode [mode]);
}

void CMainFrame::SetInsertMode (short mode)
{
UpdateInsModeButtons (segmentManager.SetAddMode (mode));
}

void CMainFrame::OnInsModeNormal ()
{
SetInsertMode (ORTHOGONAL);
}

void CMainFrame::OnInsModeExtend ()
{
SetInsertMode (EXTEND);
}

void CMainFrame::OnInsModeMirror ()
{
SetInsertMode (MIRROR);
}

void CMainFrame::OnToggleInsMode ()
{
UpdateInsModeButtons (segmentManager.SetAddMode ( (segmentManager.AddMode () + 1) % 3));
}

//------------------------------------------------------------------------------

void CMainFrame::OnExtBlkFmt ()
{
blockManager.Extended () = !blockManager.Extended ();
}

void CMainFrame::OnUpdateToggleViews (CCmdUI* pCmdUI)
{
pCmdUI->SetCheck (m_paneMode == 1);
}

void CMainFrame::OnUpdateToggleTexPane (CCmdUI* pCmdUI)
{
pCmdUI->SetCheck (m_textureMode == 1);
}

void CMainFrame::OnUpdateExtBlkFmt (CCmdUI* pCmdUI)
{
pCmdUI->SetCheck (blockManager.Extended ());
}

void CMainFrame::OnUpdateEditorToolbar (CCmdUI* pCmdUI)
{
pCmdUI->SetCheck (m_bEditorTB);
}

void CMainFrame::OnUpdateInsModeNormal (CCmdUI* pCmdUI)
{
pCmdUI->SetCheck (segmentManager.AddMode () == ORTHOGONAL);
}

void CMainFrame::OnUpdateInsModeExtend (CCmdUI* pCmdUI)
{
pCmdUI->SetCheck (segmentManager.AddMode () == EXTEND);
}

void CMainFrame::OnUpdateInsModeMirror (CCmdUI* pCmdUI)
{
pCmdUI->SetCheck (segmentManager.AddMode () == MIRROR);
}

void CMainFrame::UpdateSelectButtons (eSelectModes mode)
{
	static char *szSelMode [] = {" select: pos", " select: line", " select: side", " select: segment", " select: object", " select: block"};

for (int i = 0; i <= ID_SEL_BLOCKMODE - ID_SEL_POINTMODE; i++)
	m_toolBar.GetToolBarCtrl ().CheckButton (ID_SEL_POINTMODE + i, i == mode);
SelModeMsg (szSelMode [mode]);
}

void CMainFrame::SetSelectMode (eSelectModes mode)
{
UpdateSelectButtons (mode);
MineView ()->SetSelectMode (mode);
}

void CMainFrame::OnSelectPrevTab ()
{
ShowTools ();
ToolView ()->PrevTab ();
}

void CMainFrame::OnSelectNextTab ()
{
ShowTools ();
ToolView ()->NextTab ();
}

void CMainFrame::OnSelectPointMode ()
{
SetSelectMode (eSelectPoint);
}

void CMainFrame::OnSelectEdgeMode ()
{
SetSelectMode (eSelectLine);
}

void CMainFrame::OnSelectSideMode ()
{
SetSelectMode (eSelectSide);
}

void CMainFrame::OnSelectCubeMode ()
{
SetSelectMode (eSelectSegment);
}

void CMainFrame::OnSelectObjectMode ()
{
SetSelectMode (eSelectObject);
}

void CMainFrame::OnSelectBlockMode ()
{
SetSelectMode (eSelectBlock);
}

//------------------------------------------------------------------------------

void CMainFrame::OnViewObjectsAll () 
{
MineView ()->SetViewObjectFlags (eViewObjectsAll);
}

void CMainFrame::OnViewObjectsHostages () 
{
MineView ()->ToggleViewObjects (eViewObjectsHostages);
}

void CMainFrame::OnViewObjectsKeys () 
{
MineView ()->ToggleViewObjects (eViewObjectsKeys);
}

void CMainFrame::OnViewObjectsNone () 
{
MineView ()->SetViewObjectFlags (eViewObjectsNone);
}

void CMainFrame::OnViewObjectsPlayers () 
{
MineView ()->ToggleViewObjects (eViewObjectsPlayers);
}

void CMainFrame::OnViewObjectsPowerups () 
{
MineView ()->ToggleViewObjects (eViewObjectsPowerups);
}

void CMainFrame::OnViewObjectsRobots () 
{
MineView ()->ToggleViewObjects (eViewObjectsRobots);
}

void CMainFrame::OnViewObjectsWeapons () 
{
MineView ()->ToggleViewObjects (eViewObjectsWeapons);
}

void CMainFrame::OnViewObjectsReactor () 
{
MineView ()->ToggleViewObjects (eViewObjectsControlCenter);
}

//------------------------------------------------------------------------------

void CMainFrame::OnViewPanIn () 
{
if (MineView ()->m_viewOption != eViewNearbyCubeLines)
	MineView ()->Pan ('Z', gMoveRates [MineView ()->Perspective ()]);
}

void CMainFrame::OnViewPanOut () 
{
if (MineView ()->m_viewOption != eViewNearbyCubeLines)
	MineView ()->Pan ('Z', -gMoveRates [MineView ()->Perspective ()]);
}

void CMainFrame::OnViewPanRight () 
{
/*if (MineView ()->Perspective ())
	OnViewRotateRight ();
else*/ if (MineView ()->m_viewOption != eViewNearbyCubeLines)
	MineView ()->Pan ('X', MineView ()->Perspective () ? gMoveRates [0] : -gMoveRates [0]);
}

void CMainFrame::OnViewPanLeft () 
{
/*if (MineView ()->Perspective ())
	OnViewRotateLeft ();
else*/ if (MineView ()->m_viewOption != eViewNearbyCubeLines)
	MineView ()->Pan ('X', MineView ()->Perspective () ? -gMoveRates [0] : gMoveRates [0]);
}

void CMainFrame::OnViewPanUp () 
{
/*if (MineView ()->Perspective ())
	OnViewRotateUp ();
else*/ if (MineView ()->m_viewOption != eViewNearbyCubeLines)
	MineView ()->Pan ('Y', MineView ()->Renderer ().Type () ? gMoveRates [0] : -gMoveRates [0]);
}

void CMainFrame::OnViewPanDown () 
{
/*if (MineView ()->Perspective ())
	OnViewRotateDown ();
else*/ if (MineView ()->m_viewOption != eViewNearbyCubeLines)
	MineView ()->Pan ('Y', MineView ()->Renderer ().Type () ? -gMoveRates [0] : gMoveRates [0]);
}

void CMainFrame::OnViewRotateCounterclockwise () 
{
MineView ()->Rotate ('Z', MineView ()->Perspective () ? gRotateRates [1] : gRotateRates [0]);
}

void CMainFrame::OnViewRotateClockwise () 
{
MineView ()->Rotate ('Z', MineView ()->Perspective () ? -gRotateRates [1] : -gRotateRates [0]);
}

void CMainFrame::OnViewRotateLeft () 
{
MineView ()->Rotate ('Y', MineView ()->Perspective () ? gRotateRates [1] : gRotateRates [0]);
}

void CMainFrame::OnViewRotateRight () 
{
MineView ()->Rotate ('Y', MineView ()->Perspective () ? -gRotateRates [1] : -gRotateRates [0]);
}

void CMainFrame::OnViewRotateDown () 
{
MineView ()->Rotate ('X', MineView ()->Perspective () ? gRotateRates [1] : -gRotateRates [0]);
}

void CMainFrame::OnViewRotateUp () 
{
MineView ()->Rotate ('X', MineView ()->Perspective () ? -gRotateRates [1] : gRotateRates [0]);
}

//------------------------------------------------------------------------------

void CMainFrame::OnUpdateViewObjectsHostages (CCmdUI* pCmdUI) 
{
pCmdUI->SetCheck (MineView ()->ViewObjectFlags () & eViewObjectsHostages ? 1 : 0);
//ToolView ()->Refresh ();
}
void CMainFrame::OnUpdateViewObjectsKeys (CCmdUI* pCmdUI) 
{
pCmdUI->SetCheck (MineView ()->ViewObjectFlags () & eViewObjectsKeys ? 1 : 0);
//ToolView ()->Refresh ();
}
void CMainFrame::OnUpdateViewObjectsPlayers (CCmdUI* pCmdUI) 
{
pCmdUI->SetCheck (MineView ()->ViewObjectFlags () & eViewObjectsPlayers ? 1 : 0);
//ToolView ()->Refresh ();
}
void CMainFrame::OnUpdateViewObjectsPowerups (CCmdUI* pCmdUI) 
{
pCmdUI->SetCheck (MineView ()->ViewObjectFlags () & eViewObjectsPowerups ? 1 : 0);
//ToolView ()->Refresh ();
}
void CMainFrame::OnUpdateViewObjectsRobots (CCmdUI* pCmdUI) 
{
pCmdUI->SetCheck (MineView ()->ViewObjectFlags () & eViewObjectsRobots ? 1 : 0);
//ToolView ()->Refresh ();
}
void CMainFrame::OnUpdateViewObjectsWeapons (CCmdUI* pCmdUI) 
{
pCmdUI->SetCheck (MineView ()->ViewObjectFlags () & eViewObjectsWeapons ? 1 : 0);
//ToolView ()->Refresh ();
}
void CMainFrame::OnUpdateViewObjectsNoobjects (CCmdUI* pCmdUI) 
{
pCmdUI->SetCheck (MineView ()->ViewObjectFlags () == 0 ? 1 : 0);
//ToolView ()->Refresh ();
}
void CMainFrame::OnUpdateViewObjectsControlcenter (CCmdUI* pCmdUI) 
{
pCmdUI->SetCheck (MineView ()->ViewObjectFlags () & eViewObjectsControlCenter ? 1 : 0);
//ToolView ()->Refresh ();
}
void CMainFrame::OnUpdateViewObjectsAllobjects (CCmdUI* pCmdUI) 
{
pCmdUI->SetCheck (MineView ()->ViewObjectFlags () == eViewObjectsAll ? 1 : 0);
//ToolView ()->Refresh ();
}

//------------------------------------------------------------------------------

void CMainFrame::OnUpdateViewLights (CCmdUI* pCmdUI) 
{
pCmdUI->SetCheck (MineView ()->ViewMineFlags () & eViewMineLights ? 1 : 0);
//ToolView ()->Refresh ();
}
void CMainFrame::OnUpdateViewShading (CCmdUI* pCmdUI) 
{
pCmdUI->SetCheck (MineView ()->ViewMineFlags () & eViewMineIllumination ? 1 : 0);
ToolView ()->Refresh ();
}
void CMainFrame::OnUpdateViewDeltaLights (CCmdUI* pCmdUI) 
{
pCmdUI->SetCheck (MineView ()->ViewMineFlags () & eViewMineVariableLights ? 1 : 0);
//ToolView ()->Refresh ();
}
void CMainFrame::OnUpdateViewWalls (CCmdUI* pCmdUI) 
{
pCmdUI->SetCheck (MineView ()->ViewMineFlags () & eViewMineWalls ? 1 : 0);
//ToolView ()->Refresh ();
}

void CMainFrame::OnUpdateViewSpecial (CCmdUI* pCmdUI) 
{
pCmdUI->SetCheck (MineView ()->ViewMineFlags () & eViewMineSpecial ? 1 : 0);
//ToolView ()->Refresh ();
}

void CMainFrame::OnUpdateViewUsedTextures (CCmdUI* pCmdUI) 
{
pCmdUI->SetCheck (GetTextureView ()->m_viewFlags & eViewMineUsedTextures ? 1 : 0);
//ToolView ()->Refresh ();
}

void CMainFrame::OnUpdateViewCollapse (CCmdUI* pCmdUI) 
{
pCmdUI->Enable ((BOOL) DLE.IsD2XLevel ());
}

//------------------------------------------------------------------------------

void CMainFrame::OnUpdateViewAlllines (CCmdUI* pCmdUI) 
{
pCmdUI->SetCheck (MineView ()->m_viewOption == eViewWireFrameFull ? 1 : 0);
}
void CMainFrame::OnUpdateViewHidelines (CCmdUI* pCmdUI) 
{
pCmdUI->SetCheck (MineView ()->m_viewOption == eViewHideLines ? 1 : 0);
}
void CMainFrame::OnUpdateViewNearbySegmentLines (CCmdUI* pCmdUI) 
{
pCmdUI->SetCheck (MineView ()->m_viewOption == eViewNearbyCubeLines ? 1 : 0);
}
void CMainFrame::OnUpdateViewPartiallines (CCmdUI* pCmdUI) 
{
pCmdUI->SetCheck (MineView ()->m_viewOption == eViewWireFrameSparse ? 1 : 0);
}

void CMainFrame::OnUpdateViewTexturemapped (CCmdUI* pCmdUI) 
{
pCmdUI->SetCheck (MineView ()->m_viewOption == eViewTextured ? 1 : 0);
}

//------------------------------------------------------------------------------

void CMainFrame::OnUpdateEditHog (CCmdUI* pCmdUI)
{
pCmdUI->Enable ((BOOL) DLE.GetDocument ()->IsInHog ());
}

void CMainFrame::OnUpdateEditPog (CCmdUI* pCmdUI)
{
pCmdUI->Enable ((BOOL) DLE.IsD2File ());
}

//------------------------------------------------------------------------------

bool CMainFrame::EditGeoFwd (void)
{
if (!theMine->EditGeoFwd ())
	return false;
MineView ()->Refresh ();
return true;
}

bool CMainFrame::EditGeoUp (void)
{
if (!theMine->EditGeoUp ())
	return false;
MineView ()->Refresh ();
return true;
}

bool CMainFrame::EditGeoBack (void)
{
if (!theMine->EditGeoBack ())
	return false;
MineView ()->Refresh ();
return true;
}

bool CMainFrame::EditGeoRotLeft (void)
{
if (!theMine->EditGeoRotLeft ())
	return false;
MineView ()->Refresh ();
return true;
}

bool CMainFrame::EditGeoGrow (void)
{
if (!theMine->EditGeoGrow ())
	return false;
MineView ()->Refresh ();
return true;
}

bool CMainFrame::EditGeoRotRight (void)
{
if (!theMine->EditGeoRotRight ())
	return false;
MineView ()->Refresh ();
return true;
}

bool CMainFrame::EditGeoLeft (void)
{
if (!theMine->EditGeoLeft ())
	return false;
MineView ()->Refresh ();
return true;
}

bool CMainFrame::EditGeoDown (void)
{
if (!theMine->EditGeoDown ())
	return false;
MineView ()->Refresh ();
return true;
}

bool CMainFrame::EditGeoRight (void)
{
if (!theMine->EditGeoRight ())
	return false;
MineView ()->Refresh ();
return true;
}

bool CMainFrame::EditGeoShrink (void)
{
if (!theMine->EditGeoShrink ())
	return false;
MineView ()->Refresh ();
return true;
}

//------------------------------------------------------------------------------

void CMainFrame::OnEditGeoFwd ()
{
if (MineView ()->Perspective ())
	EditGeoUp ();
else
	EditGeoFwd ();
}

void CMainFrame::OnEditGeoBack ()
{
if (MineView ()->Perspective ())
	EditGeoDown ();
else
	EditGeoBack ();
}

void CMainFrame::OnEditGeoUp ()
{
if (MineView ()->Perspective ())
	EditGeoFwd ();
else
	EditGeoUp ();
}

void CMainFrame::OnEditGeoDown ()
{
if (MineView ()->Perspective ())
	EditGeoBack ();
else
EditGeoDown ();
}

void CMainFrame::OnEditGeoRotLeft ()
{
EditGeoRotLeft ();
}

void CMainFrame::OnEditGeoGrow ()
{
EditGeoGrow ();
}

void CMainFrame::OnEditGeoRotRight ()
{
EditGeoRotRight ();
}

void CMainFrame::OnEditGeoLeft ()
{
EditGeoLeft ();
}

void CMainFrame::OnEditGeoRight ()
{
EditGeoRight ();
}

void CMainFrame::OnEditGeoShrink ()
{
EditGeoShrink ();
}

//------------------------------------------------------------------------------

void CMainFrame::OnSelNextPoint ()
{
if (MineView ()->SelectMode (POINT_MODE))
	MineView ()->NextPoint ();
else
	MineView ()->SetSelectMode (POINT_MODE);
}

//------------------------------------------------------------------------------

void CMainFrame::OnSelPrevPoint ()
{
if (MineView ()->SelectMode (POINT_MODE))
	MineView ()->PrevPoint ();
else
	MineView ()->SetSelectMode (POINT_MODE);
}

//------------------------------------------------------------------------------

void CMainFrame::OnSelNextLine ()
{
if (MineView ()->SelectMode (LINE_MODE))
	MineView ()->NextLine ();
else
	MineView ()->SetSelectMode (LINE_MODE);
}

//------------------------------------------------------------------------------

void CMainFrame::OnSelPrevLine ()
{
if (MineView ()->SelectMode (LINE_MODE))
	MineView ()->PrevLine ();
else
	MineView ()->SetSelectMode (LINE_MODE);
}

//------------------------------------------------------------------------------

void CMainFrame::OnSelNextSide ()
{
if (MineView ()->SelectMode (SIDE_MODE))
	MineView ()->NextSide ();
else
	MineView ()->SetSelectMode (SIDE_MODE);
}

//------------------------------------------------------------------------------

void CMainFrame::OnSelPrevSide ()
{
if (MineView ()->SelectMode (SIDE_MODE))
	MineView ()->PrevSide ();
else
	MineView ()->SetSelectMode (SIDE_MODE);
}

//------------------------------------------------------------------------------

void CMainFrame::OnSelNextSegment ()
{
if (MineView ()->SelectMode (SEGMENT_MODE))
	MineView ()->NextSegment ();
else
	MineView ()->SetSelectMode (SEGMENT_MODE);
}

//------------------------------------------------------------------------------

void CMainFrame::OnSelPrevSegment ()
{
if (MineView ()->SelectMode (SEGMENT_MODE))
	MineView ()->PrevSegment ();
else
	MineView ()->SetSelectMode (SEGMENT_MODE);
}

//------------------------------------------------------------------------------

void CMainFrame::OnSelNextObject ()
{
if (MineView ()->SelectMode (OBJECT_MODE))
	MineView ()->NextObject ();
else
	MineView ()->SetSelectMode (OBJECT_MODE);
}

//------------------------------------------------------------------------------

void CMainFrame::OnSelPrevObject ()
{
if (MineView ()->SelectMode (OBJECT_MODE))
	MineView ()->NextObject ();
else
	MineView ()->SetSelectMode (OBJECT_MODE);
}

//------------------------------------------------------------------------------

void CMainFrame::OnSelSegmentForward ()
{
MineView ()->SegmentForward ();
}

//------------------------------------------------------------------------------

void CMainFrame::OnSelSegmentBackwards ()
{
MineView ()->SegmentBackwards ();
}

//------------------------------------------------------------------------------

void CMainFrame::OnSelNextSegmentElem ()
{
MineView ()->NextSegmentElement ();
}

//------------------------------------------------------------------------------

void CMainFrame::OnSelPrevSegmentElem ()
{
MineView ()->PrevSegmentElement ();
}

//------------------------------------------------------------------------------

void CMainFrame::OnSelNextSegmentElemKey ()
{
if (MineView ()->Perspective ())
	OnViewPanRight ();
else
	MineView ()->NextSegmentElement ();
}

//------------------------------------------------------------------------------

void CMainFrame::OnSelPrevSegmentElemKey ()
{
if (MineView ()->Perspective ())
	OnViewPanLeft ();
else
	MineView ()->PrevSegmentElement ();
}

//------------------------------------------------------------------------------

void CMainFrame::OnSelSegmentForwardKey ()
{
if (MineView ()->Perspective ())
	OnViewPanIn ();
else
	MineView ()->SegmentForward ();
}

//------------------------------------------------------------------------------

void CMainFrame::OnSelSegmentBackwardsKey ()
{
if (MineView ()->Perspective ())
	OnViewPanOut ();
else
	MineView ()->SegmentBackwards ();
}

//------------------------------------------------------------------------------

void CMainFrame::OnSelOtherSegment ()
{
MineView ()->SelectOtherSegment ();
}

//------------------------------------------------------------------------------

void CMainFrame::OnSelOtherSide () 
{
MineView ()->SelectOtherSide ();
}

//------------------------------------------------------------------------------

void CMainFrame::OnCollapseEdge () 
{
segmentManager.CollapseEdge ();
}

//------------------------------------------------------------------------------

void CMainFrame::OnCreateWedge () 
{
segmentManager.CreateWedge ();
}

//------------------------------------------------------------------------------

void CMainFrame::OnCreatePyramid () 
{
segmentManager.CreatePyramid ();
}

//------------------------------------------------------------------------------

void CMainFrame::OnUndo () 
{
if (undoManager.Undo ())
	MineView ()->Refresh ();
}

//------------------------------------------------------------------------------

void CMainFrame::OnRedo () 
{
if (undoManager.Redo ())
	MineView ()->Refresh ();
}

//------------------------------------------------------------------------------

void CMainFrame::OnCheckMine () 
{
ShowTools ();
ToolView ()->EditDiag ();
ToolView ()->DiagTool ()->OnCheckMine ();
}

//------------------------------------------------------------------------------

void CMainFrame::AdjustMine (int nVersion) 
{
DLE.MineView ()->DelayRefresh (true);
if (nVersion < 3) {
	segmentManager.DeleteD2X ();
	triggerManager.DeleteD2X ();
	wallManager.DeleteD2X ();
	objectManager.DeleteD2X ();
	}
if (nVersion < 2)
	objectManager.DeleteVertigo ();
DLE.MineView ()->DelayRefresh (false);
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------------

void CMainFrame::OnEditRecalculateCenter ()
{
theMine->UpdateCenter ();
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------------

void CMainFrame::OnEditSetCenter ()
{
current->Segment ()->ComputeCenter ();
theMine->SetCenter (current->Segment ()->Center ());
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------------

//eof MainFrame.InterfaceHandlers.cpp