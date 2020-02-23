
#include "stdafx.h"
#include <stdio.h>
#include <string.h>

#include "mine.h"
#include "dle-xp.h"
#include "dlcdoc.h"
#include "PaletteManager.h"
#include "TextureManager.h"

extern short nDbgSeg, nDbgSide;
extern int nDbgVertex;

// D2 list translate to D1 equivelant textures.
// D1 texture numbers on left, D2 textures on right, closest match first.
// A zero in either column means the texture number in the other column
// is the default, used if no match is found.
static short texMapD2toD1 [] = {
	-1,
	0,1,-1,
	1,0,-1,
	3,1,-1,
	6,181,270,399,402,403,-1,
	7,271,-1,
	8,2,-1,
	10,272,-1,
	11,183,400,401,-1,
	13,3,191,-1,
	14,4,-1,
	15,5,-1,
	16,6,-1,
	19,7,-1,
	22,8,-1,
	23,9,-1,
	24,10,-1,
	26,11,-1,
	27,12,-1,
	29,13,-1,
	30,14,-1,
	31,15,-1,
	32,16,-1,
	33,17,-1,
	34,18,-1,
	35,19,-1,
	36,20,-1,
	37,21,-1,
	38,163,-1,
	40,22,-1,
	42,23,-1,
	43,24,127,-1,
	45,25,-1,
	46,26,-1,
	47,27,-1,
	48,28,-1,
	49,172,-1,
	50,179,-1,
	51,29,-1,
	52,30,145,-1,
	53,31,-1,
	54,32,-1,
	55,409,-1,
	56,33,-1,
	58,34,-1,
	59,35,178,-1,
	60,36,153,-1,
	61,37,-1,
	62,38,167,-1,
	63,39,-1,
	64,40,-1,
	65,41,-1,
	66,42,168,199,-1,
	67,43,-1,
	68,44,138,157,-1,
	69,45,-1,
	70,46,-1,
	71,47,-1,
	72,48,-1,
	73,49,173,-1,
	74,50,155,-1,
	75,51,156,-1,
	76,52,128,188,-1,
	77,53,132,190,-1,
	78,54,143,-1,
	79,55,174,-1,
	80,56,-1,
	81,57,-1,
	82,58,198,-1,
	83,59,137,176,187,-1,
	84,60,-1,
	85,61,-1,
	86,62,134,139,-1,
	87,63,135,180,-1,
	88,149,150,-1,
	89,64,-1,
	90,65,133,186,-1,
	91,66,144,154,-1,
	92,67,-1,
	93,68,-1,
	94,69,-1,
	95,70,-1,
	96,71,-1,
	97,72,-1,
	98,73,171,-1,
	99,74,-1,
	100,75,-1,
	101,76,-1,
	102,77,-1,
	103,78,-1,
	104,79,-1,
	105,80,-1,
	106,81,-1,
	107,82,169,-1,
	108,83,-1,
	109,84,196,-1,
	110,85,136,-1,
	111,86,192,-1,
	112,87,-1,
	113,88,193,-1,
	114,89,-1,
	115,90,-1,
	116,91,184,195,-1,
	117,92,-1,
	118,93,-1,
	119,94,-1,
	120,95,-1,
	121,96,147,-1,
	122,97,146,151,-1,
	123,98,170,-1,
	124,99,-1,
	125,100,-1,
	126,101,-1,
	127,102,130,131,175,185,189,-1,
	128,103,-1,
	129,104,-1,
	130,105,162,197,-1,
	131,106,161,-1,
	132,148,160,-1,
	133,107,-1,
	134,108,-1,
	135,109,-1,
	136,110,-1,
	137,111,-1,
	138,112,165,-1,
	139,113,152,166,-1,
	140,114,158,-1,
	141,159,177,-1,
	142,115,-1,
	143,116,-1,
	144,117,-1,
	145,118,-1,
	146,119,-1,
	148,120,-1,
	149,121,-1,
	150,122,-1,
	151,123,164,-1,
	152,124,140,182,-1,
	153,125,141,142,194,-1,
	156,200,-1,
	157,201,-1,
	162,202,-1,
	163,203,-1,
	164,204,-1,
	165,205,-1,
	166,206,-1,
	172,207,-1,
	173,208,-1,
	176,209,-1,
	177,210,-1,
	178,211,-1,
	179,212,-1,
	180,213,-1,
	181,214,-1,
	182,215,-1,
	183,216,-1,
	184,217,-1,
	186,218,-1,
	187,219,-1,
	188,220,-1,
	189,221,-1,
	190,222,-1,
	191,223,-1,
	192,224,-1,
	196,225,-1,
	197,226,-1,
	201,227,-1,
	203,228,-1,
	204,229,-1,
	205,230,-1,
	206,231,-1,
	207,232,-1,
	208,233,-1,
	209,234,-1,
	212,235,-1,
	213,236,-1,
	214,237,-1,
	215,238,-1,
	216,239,-1,
	217,240,-1,
	218,241,-1,
	219,242,-1,
	221,243,-1,
	222,244,-1,
	224,245,-1,
	225,246,-1,
	231,249,-1,
	232,250,-1,
	233,251,-1,
	234,252,-1,
	235,253,-1,
	236,254,-1,
	237,255,-1,
	238,256,-1,
	239,248,257,-1,
	244,258,-1,
	245,247,259,-1,
	247,260,-1,
	248,261,-1,
	249,262,-1,
	250,340,341,343,344,345,346,-1,
	251,412,429,-1,
	252,410,427,-1,
	253,411,428,-1,
	254,263,268,423,424,-1,
	255,264,267,269,350,-1,
	256,265,-1,
	261,273,-1,
	262,274,-1,
	264,275,-1,
	265,276,-1,
	267,277,-1,
	268,278,-1,
	269,279,-1,
	270,129,280,-1,
	271,281,-1,
	272,282,-1,
	273,283,-1,
	274,284,-1,
	275,285,-1,
	276,126,266,286,-1,
	277,287,-1,
	278,288,-1,
	279,289,348,349,-1,
	280,290,-1,
	281,291,-1,
	282,293,-1,
	284,295,-1,
	285,296,-1,
	286,298,-1,
	287,300,-1,
	288,301,-1,
	289,302,-1,
	290,303,-1,
	291,304,-1,
	292,305,-1,
	293,306,-1,
	294,307,-1,
	295,308,-1,
	296,309,-1,
	297,310,-1,
	298,380,-1,
	299,311,-1,
	300,312,-1,
	301,313,-1,
	302,314,-1,
	303,315,-1,
	304,316,-1,
	305,317,-1,
	306,318,-1,
	307,319,-1,
	308,320,-1,
	309,321,-1,
	310,322,-1,
	311,323,-1,
	312,324,-1,
	313,325,338,-1,
	314,326,339,342,347,-1,
	316,327,-1,
	317,328,-1,
	318,329,-1,
	319,330,-1,
	320,331,-1,
	321,332,-1,
	322,333,-1,
	323,334,-1,
	324,335,-1,
	325,336,-1,
	326,337,-1,
	327,352,-1,
	328,353,-1,
	329,354,-1,
	330,382,-1,
	332,355,-1,
	333,404,405,406,-1,
	334,356,-1,
	335,357,-1,
	336,358,-1,
	337,359,-1,
	338,360,-1,
	339,361,-1,
	340,362,-1,
	341,370,-1,
	343,366,-1,
	345,368,-1,
	347,364,-1,
	349,351,372,-1,
	351,374,-1,
	352,375,-1,
	354,377,-1,
	355,407,408,-1,
	356,378,426,-1,

	371,435,754,901,-1,
	376,440,-1,
	387,451,649,672,-1,
	399,463,-1,
	413,477,-1,
	419,483,858,-1,
	424,488,-1,
	436,500,-1,
	444,508,-1,
	459,523,790,886,-1,
	472,536,687,702,871,-1,
	486,550,-1,
	492,556,725,817,827,838,-1,
	500,564,763,772,-1,
	508,572,731,-1,
	515,579,806,-1,
	521,585,717,-1,
	529,593,-1,
	536,600,738,745,-1,
	543,608,780,-1,
	550,615,-1,
	563,628,-1,
	570,635,849,-1,
	577,642
	};

static short doorMapD2toD1 [] = {
	-1, 0, 0, 36, 50,
	-1, 1, 1,
	-1, 3, 3, 26, 29,
	-1, 4, 4,
	-1, 5, 5,
	-1, 6, 6, 46,
	-1, 7, 7,
	-1, 9, 9,
	-1, 10, 10,
	-1, 11, 11, 28, 40, 49,
	-1, 12, 12, 30, 48,
	-1, 13, 13, 
	-1, 14, 14, 32, 42, 43, 44, 47,
	-1, 15, 15, 37, 38,
	-1, 16, 16, 33,
	-1, 17, 17, 41,
	-1, 18, 18, 31,
	-1, 19, 19,
	-1, 20, 20, 34, 35,
	-1, 21, 21, 39,
	-1, 22, 22,
	-1, 23, 23, 
	-1, 24, 24, 45,
	-1, 25, 25
	};

// D2, D1
static short botMapD2toD1 [] = {
	 -1, 0, 0, 57, // medium hulk (brown, concussion)
	 -1, 1, 1, // medium lifter (green, red claws)
	 -1, 2, 2, 26, 28, 29, 60, // spider (red, ti stream)
	 -1, 3, 3, 37, // class 1 drone (orange, red laser)
	 -1, 4, 4, // class 2 drone (blue, blue laser)
	 -1, 5, 5, // cloaked driller (vulcan)
	 -1, 6, 6, 55, // cloaked hulk (green laser)
	 -1, 7, 7, // class 2 supervisor (blueish)
	 -1, 8, 8, 50, // secondary lifter (red, advanced lifter spawn)
	 -1, 9, 9, 24, 34, 69, // class 1 heavy driller (plasma gun)
	 -1, 11, 11, 43, // class 1 platform (gray, concussion missiles)
	 -1, 10, 10, 14, // class 3 gopher (green, laser)
	 -1, 12, 12, 39, 48, // class 2 platform (green, green laser)
	 -1, 13, 13, // split pot (flat red, 4-sided pyramid)
	 -1, 14, 14, 41, 49, 56, 58, 61, 63, // small spider (red, red laser, spider spawn)
	 -1, 15, 15, 30, 53, // fusion hulk (purple)
	 -1, 16, 16, 51, // super hulk (red, homers)
	 -1, 17, 17, 31, 32, 52, 75, // boss 1 (yellow)
	 -1, 18, 18, 54, // cloaked lifter (green)
	 -1, 19, 19, 27, 36, // class 1 driller (vulcan)
	 -1, 20, 20, 25, 38, // small hulk (green, green laser)
	 -1, 21, 21, 40, // advanced lifter (red)
	 -1, 22, 24, // ptmc defense (gray, blue laser)
	 -1, 23, 25, 45, 46, 62, 64, 76 // boss 2 (red)
	};

BEGIN_MESSAGE_MAP (CConvertDlg, CDialog)
	ON_WM_PAINT ()
	ON_CBN_SELCHANGE (IDC_CONVERT_D1, OnSetD1)
	ON_CBN_SELCHANGE (IDC_CONVERT_D2, OnSetD2)
END_MESSAGE_MAP ()

//------------------------------------------------------------------------
// DIALOG - CConvertDlg (constructor)
//------------------------------------------------------------------------

CConvertDlg::CConvertDlg (CWnd *pParent)
	: CDialog (IDD_CONVERT, pParent)
{
m_bInited = false;
}

//------------------------------------------------------------------------
// CConvertDlg - ~CConvertDlg (destructor)
//------------------------------------------------------------------------

void CConvertDlg::EndDialog (int nResult) 
{
if (m_bInited) {
	m_showD1.DestroyWindow ();
	m_showD2.DestroyWindow ();
	}
CDialog::EndDialog (nResult);
}

//------------------------------------------------------------------------------

void CConvertDlg::CreateImgWnd (CWnd *pImgWnd, int nIdC)
{
CWnd *pParentWnd = GetDlgItem (nIdC);
CRect rc;
pParentWnd->GetClientRect (rc);
pImgWnd->Create (null, null, WS_CHILD | WS_VISIBLE, rc, pParentWnd, 0);
}

//------------------------------------------------------------------------
// CConvertDlg - SetupWindow
//------------------------------------------------------------------------

BOOL CConvertDlg::OnInitDialog () 
{
CDialog::OnInitDialog ();

CreateImgWnd (&m_showD1, IDC_CONVERT_SHOWD1);
CreateImgWnd (&m_showD2, IDC_CONVERT_SHOWD2);

if (!(m_pTextures = (short *) m_res.Load (IDR_TEXTURE_D1D2)))
	return FALSE;

CComboBox *pcb = CBD1 ();
short	nSeg,	nSide, nTextures;
short segCount = segmentManager.Count ();
char	szName [80];
int h;
CSegment *pSegment = segmentManager.Segment (0);
CSide *pSide;
// add textures that have been used to Texture 1 combo box
for (nSeg = segCount; nSeg; nSeg--, pSegment++) {
	for (pSide = pSegment->m_sides, nSide = 6; nSide; nSide--, pSide++) {
		short nTextures [2] = {pSide->BaseTex (), pSide->OvlTex (0)};
		for (int i = 0; i < 2; i++) {
			if (nTextures [i] != -1) {
				// read name of texture from Descent 1 texture resource
				LoadString (m_hInst, TEXTURE_STRING_TABLE_D1 + nTextures [i], szName, sizeof (szName));
				// if not already in list box, add it
				if (pcb->FindStringExact (-1, szName) < 0) {
					h = pcb->AddString (szName);
					pcb->SetItemData (h, nTextures [i]);
					}
				}
			}
		}
	}
pcb->SetCurSel (0);

  // add complete set for Texture 2 combo box
nTextures = DLE.IsD1File () ? MAX_TEXTURES_D1 : MAX_TEXTURES_D2;
pcb = CBD2 ();
for (int i = 0; i < nTextures; i++) {
// read szName of texture from Descent 2 texture resource
	LoadString (m_hInst, TEXTURE_STRING_TABLE_D2 + i, szName, sizeof (szName));
	if (*szName != '*') {
		h = pcb->AddString (szName);
		pcb->SetItemData (h, i);
		}
	}
m_bInited = true;
Refresh ();
return TRUE;
}

void CConvertDlg::DoDataExchange (CDataExchange *pDX)
{
Refresh ();
}

//------------------------------------------------------------------------
// CConvertDlg - RefreshData
//------------------------------------------------------------------------

void CConvertDlg::Refresh () 
{
if (!(m_bInited && theMine))
	return;

	short texture1,texture2;
	short fileTypeBackup;
#ifdef _DEBUG
	DWORD nError;
#endif

// find matching entry for Texture 1
//  CBD1 ()->GetSelString (message,sizeof (message));
//  texture1 = GetTextureID (message);
texture1 = (short) (CBD1 ()->GetItemData (CBD1 ()->GetCurSel ()));
texture2 = m_pTextures [texture1];
if (LoadString (m_hInst, TEXTURE_STRING_TABLE_D2 + texture2, message, sizeof (message)))
	CBD2 ()->SelectString (-1, message);
#ifdef _DEBUG
else
	nError = GetLastError ();
#endif
fileTypeBackup = DLE.FileType ();
theMine->SetFileType (RL2_FILE);
paletteManager.Reload ();
PaintTexture (&m_showD2, 0, texture2, 0);
theMine->SetFileType (RDL_FILE);
paletteManager.Reload ();
PaintTexture (&m_showD1, 0, texture1, 0);

  // restore file type (should always be RDL_TYPE)
  theMine->SetFileType (fileTypeBackup);
}

//------------------------------------------------------------------------
// CConvertDlg - Sent if we need to redraw an item
//------------------------------------------------------------------------

void CConvertDlg::OnPaint ()
{
CDialog::OnPaint ();
Refresh ();
}

//------------------------------------------------------------------------
// CConvertDlg - Ok
//------------------------------------------------------------------------
/*
void CConvertDlg::OnOK ()
{
Convert ();
CDialog::OnOK ();
}
*/
//------------------------------------------------------------------------
// CConvertDlg - Texture1 Msg
//------------------------------------------------------------------------

void CConvertDlg::OnSetD1 ()
{
Refresh ();
}

//------------------------------------------------------------------------
// CConvertDlg - Texture1 Msg
//------------------------------------------------------------------------

void CConvertDlg::OnSetD2 ()
{
short texture1 = (short) (CBD1 ()->GetItemData (CBD1 ()->GetCurSel ()));
short texture2 = (short) (CBD2 ()->GetItemData (CBD2 ()->GetCurSel ()));
m_pTextures [texture1] = texture2;
Refresh ();
}

//------------------------------------------------------------------------
// CConvertDlg - Convert
//------------------------------------------------------------------------

void CConvertDlg::OnOK ()
{
CHECKMINE;
theMine->ConvertD1toD2 (m_pTextures);
CDialog::OnOK ();
}

//------------------------------------------------------------------------

void CMine::ConvertD1toD2 (short* textureMap) 
{
	short				i, j;
	short				nSegment, nSide, d1Texture, mode,
						segCount = segmentManager.Count (),
						wallCount = wallManager.WallCount ();
	int				lightMap [MAX_TEXTURES_D2];
	texColorList	colorMap;

memcpy (lightMap, lightManager.LightMap (), sizeof (lightMap));
#if DBG
colorMap = lightManager.ColorMap ();
#else
memcpy (colorMap.Buffer (), lightManager.ColorMap ().Buffer (), sizeof (colorMap));
#endif

textureManager.ReleaseTextures ();
undoManager.Reset ();	//no undo possible; palette changes to difficult to handle
// reload internal stuff for d2
theMine->SetFileType (RL2_FILE);
paletteManager.Reload ();
textureManager.LoadTextures ();
lightManager.LoadDefaults ();

  // convert textures
CSegment* pSegment = segmentManager.Segment (0);
for (nSegment = 0; nSegment < segCount; nSegment++, pSegment++) {
	CSide* pSide = pSegment->m_sides;
	for (nSide = 0; nSide < 6; nSide++, pSide++) {
#ifdef _DEBUG
		if ((nSegment == nDbgSeg) && ((nDbgSide < 0) || (nSide == nDbgSide)))
			nDbgSeg = nDbgSeg;
#endif
		if (pSide->m_info.nWall >= wallCount)
			pSide->m_info.nWall = NO_WALL;
		if ((pSegment->ChildId (nSide) == -1) || (pSide->m_info.nWall < wallCount)) {
			d1Texture = pSegment->m_sides [nSide].BaseTex ();
			if ((d1Texture >= 0) && (d1Texture < MAX_TEXTURES_D1)) {
				pSide->m_info.nBaseTex = textureMap [d1Texture];
				*lightManager.LightMap (pSide->BaseTex ()) = lightMap [d1Texture];
				*lightManager.TexColor (pSide->BaseTex ()) = colorMap [d1Texture];
				}
			else { 
				DEBUGMSG (" Level converter: Invalid texture 1 found")
				pSide->m_info.nBaseTex = 0;
				}
			d1Texture = pSide->OvlTex ();
			mode = d1Texture & ALIGNMENT_MASK;
			d1Texture &= TEXTURE_MASK;
			if (d1Texture > 0 && d1Texture < MAX_TEXTURES_D1) {
				pSide->m_info.nOvlTex = textureMap [d1Texture] | mode;
				*lightManager.LightMap (pSide->BaseTex ()) = lightMap [d1Texture];
				*lightManager.TexColor (pSide->BaseTex ()) = colorMap [d1Texture];
				}
			else if (d1Texture < 0) {
				DEBUGMSG (" Level converter: Invalid texture 2 found")
				pSide->m_info.nOvlTex = 0;
				}
			}
		}
	}

// defined D2 wall parameters
//--------------------------------------
CWall* pWall = wallManager.Wall (0);
for (i = 0; i < wallCount; i++, pWall++) {
	pWall->Info ().controllingTrigger = 0;
	pWall->Info ().cloakValue = 0;
	}

// change pTrigger type and flags
//-------------------------------------------
CTrigger* pTrigger = triggerManager.Trigger (0);
for (i = 0; i < triggerManager.WallTriggerCount (); i++, pTrigger++) {
	switch (pTrigger->Flags ()) {
		case TRIGGER_CONTROL_DOORS:
			pTrigger->Type () = TT_OPEN_DOOR;
			break;
		case TRIGGER_EXIT:
			pTrigger->Type () = TT_EXIT;
			break;
		case TRIGGER_MATCEN:
			pTrigger->Type () = TT_MATCEN;
			break;
		case TRIGGER_ILLUSION_OFF:
			pTrigger->Type () = TT_ILLUSION_OFF;
			break;
		case TRIGGER_ILLUSION_ON:
			pTrigger->Type () = TT_ILLUSION_ON;
			break;
		case TRIGGER_SECRET_EXIT:
			pTrigger->Type () = TT_SECRET_EXIT;
			break;
		case TRIGGER_OPEN_WALL:
			pTrigger->Type () = TT_OPEN_WALL;
			break;
		case TRIGGER_CLOSE_WALL:
			pTrigger->Type () = TT_CLOSE_WALL;
			break;

		// unsupported types
		case TRIGGER_ON:
		case TRIGGER_ONE_SHOT:
		case TRIGGER_SHIELD_DAMAGE:
		case TRIGGER_ENERGY_DRAIN:
		default:
			DEBUGMSG (" Level converter: Unsupported trigger type; trigger deleted")
			triggerManager.Delete (i);
			i--;
			pTrigger--;
			continue;
		}
	pTrigger->Flags () = 0;
	}

// set robot_center nProducer and robot_flags2
//-----------------------------------------------
for (i = 0; i < segmentManager.RobotMakerCount (); i++) {
	segmentManager.RobotMaker (i)->m_info.objFlags [1] = 0;
	for (j = 0, pSegment = segmentManager.Segment (0); j <= segCount; j++, pSegment++)
		if ((pSegment->m_info.function == SEGMENT_FUNC_ROBOTMAKER) && (pSegment->m_info.nProducer == i))
				segmentManager.RobotMaker (i)->m_info.nProducer = (short)(pSegment->m_info.value);
	}

// set equip_center nProducer and robot_flags2
//-----------------------------------------------
for (i = 0; i < segmentManager.EquipMakerCount (); i++) {
	segmentManager.EquipMaker (i)->m_info.objFlags [1] = 0;
	for (j = 0, pSegment = segmentManager.Segment (0); j <= segCount; j++, pSegment++)
		if ((pSegment->m_info.function == SEGMENT_FUNC_EQUIPMAKER) && (pSegment->m_info.nProducer == i))
				segmentManager.EquipMaker (i)->m_info.nProducer = (short)(pSegment->m_info.value);
	}

// Objects ()
//-----------------------------------------------

CGameObject* pObject = objectManager.Object (0);
for (i = 0; i < objectManager.Count (); i++, pObject++) {
// int clip numbers for poly Objects () (except robots)
	switch (pObject->Type ()) {
		case OBJ_PLAYER: // the player on the console
			pObject->rType.polyModelInfo.nModel = D2_PLAYER_CLIP_NUMBER;
			break;
		case OBJ_REACTOR: // the control center
			pObject->rType.polyModelInfo.nModel = D2_REACTOR_CLIP_NUMBER;
			pObject->Id () = 0; // only one reactor ID in D2 for this reactor type
			break;
		case OBJ_COOP: // a cooperative player object
			pObject->rType.polyModelInfo.nModel = D2_COOP_CLIP_NUMBER;
			break;
		}
	}

// d2 light data and indicies
//--------------------------------------------
lightManager.DeltaIndexCount () = 0;
lightManager.DeltaValueCount () = 0;
lightManager.Count () = 0;
lightManager.ComputeStaticLight (true);
lightManager.CalcAverageCornerLight (true);
lightManager.ScaleCornerLight (true);
lightManager.SetObjectLight (true);
lightManager.ComputeVariableLight (1);
lightManager.Normalize ();

// d2 reactor and secret segment
//----------------------------------------------
triggerManager.ReactorTime () = 0x1e;
theMine->ReactorStrength () = 0xffffffffL;
objectManager.SecretSegment () = 0L;

objectManager.SecretOrient ().Set (1, 0, 0, 0, 0, 1, 0, 1, 0);
DLE.MineView ()->ResetView (true);
}

//------------------------------------------------------------------------

static short MapD2ToD1 (short nId, short nDefault, short* map, int mapSize, bool bFallback = true)
{
	short nNearest = -1;

for (int i = 2; i < mapSize; i++) {
	if (map [i - 1] == -1)
		continue; // hit a D1 id, so keep on searching
	short h = map [i];
	if (h == nId) {
		while (map [--i] != -1)
			;
		return map [i + 1];
		}
	if (bFallback && (h < nId) && (h > nNearest)) // find the biggest map entry < nId
		nNearest = h;
	}
return (nNearest < 0) 
		 ? nDefault 
		 : MapD2ToD1 (nNearest, nDefault, map, mapSize); // locate the map entry for nNearest as a fallback
}

//------------------------------------------------------------------------

static short MapD2TextureToD1 (short nTexture)
{
return MapD2ToD1 (nTexture, nTexture, texMapD2toD1, sizeofa (texMapD2toD1));
}

//------------------------------------------------------------------------

static short MapD2RobotToD1 (short nRobot)
{
return MapD2ToD1 (nRobot, -1, botMapD2toD1, sizeofa (botMapD2toD1), false);
}

//------------------------------------------------------------------------

static short MapD2DoorToD1 (short nDoor)
{
return MapD2ToD1 (nDoor, (nDoor - 1) % (MAX_DOOR_ANIMS_D1 - 1) + 1, doorMapD2toD1, sizeofa (doorMapD2toD1));
}

//------------------------------------------------------------------------

static int MapD2PowerupToD1 (ubyte nId)
{
switch (nId) {
	case POW_SUPERLASER:
		return POW_LASER;
	case POW_GAUSS:
		return POW_VULCAN;
	case POW_HELIX:
		return POW_SPREADFIRE;
	case POW_PHOENIX:
		return POW_PLASMA;
	case POW_OMEGA:
		return POW_FUSION;
	case POW_FLASHMSL_1:
		return POW_CONCUSSION_1;
	case POW_FLASHMSL_4:
		return POW_CONCUSSION_4;
	case POW_GUIDEDMSL_1:
		return POW_HOMINGMSL_1;
	case POW_GUIDEDMSL_4:
		return POW_HOMINGMSL_4;
	case POW_SMARTMINE:
		return POW_PROXMINE;
	case POW_MERCURYMSL_1:
	case POW_MERCURYMSL_4:
		return POW_SMARTMSL;
	case POW_EARTHSHAKER:
		return POW_MEGAMSL;
	case POW_FULL_MAP:
	case POW_CONVERTER:
	case POW_AMMORACK:
	case POW_AFTERBURNER:
	case POW_HEADLIGHT:
	case POW_SLOWMOTION:
	case POW_BULLETTIME:
	case POW_BLUEFLAG:
	case POW_REDFLAG:
		return -1;
	default:
		return (nId < MAX_POWERUP_IDS_D1) ? nId : -1;
	}
}

//------------------------------------------------------------------------

void CMine::ConvertD2toD1 (void) 
{
	short				d2Texture, h, i, j;
	int				lightMap [MAX_TEXTURES_D2];
	texColorList	colorMap;
	char				szMsg [1000];
	int				nDeleted [5] = {0, 0, 0, 0, 0};

memcpy (lightMap, lightManager.LightMap (), sizeof (lightMap));
#if DBG
colorMap = lightManager.ColorMap ();
#else
memcpy (colorMap.Buffer (), lightManager.ColorMap ().Buffer (), sizeof (colorMap));
#endif

textureManager.RemoveCustomTextures ();
textureManager.ReleaseTextures ();
undoManager.Reset ();	//no undo possible; palette changes to difficult to handle
// reload internal stuff for d2
theMine->SetFileType (RDL_FILE);
theMine->SetLevelVersion (1);
paletteManager.Reload ();
textureManager.LoadTextures ();
lightManager.LoadDefaults ();

*szMsg = '\0';

for (i = wallManager.WallCount () - 1; i >= 0; i--) {
	if (wallManager.Wall (i)->Type () == WALL_OVERLAY) {
		nDeleted [1]++;
		if (wallManager.Wall (i)->Trigger ())
			nDeleted [2]++;
		wallManager.Delete (i);
		}
	}

CTrigger* pTrigger = triggerManager.Trigger (0);
for (i = 0; i < triggerManager.WallTriggerCount (); i++, pTrigger++) {
	switch (pTrigger->Type ()) {
		case TT_OPEN_DOOR:
			pTrigger->Flags () = TRIGGER_CONTROL_DOORS;
			break;
		case TT_EXIT:
			pTrigger->Flags () = TRIGGER_EXIT;
			break;
		case TT_MATCEN:
			pTrigger->Flags () = TRIGGER_MATCEN;
			break;
		case TT_OPEN_WALL:
			pTrigger->Flags () = TRIGGER_OPEN_WALL;
			break;
		case TT_CLOSE_WALL:
			pTrigger->Flags () = TRIGGER_CLOSE_WALL;
			break;
		case TT_ILLUSION_OFF:
			pTrigger->Flags () = TRIGGER_ILLUSION_OFF;
			break;
		case TT_ILLUSION_ON:
			pTrigger->Flags () = TRIGGER_ILLUSION_ON;
			break;
		case TT_SECRET_EXIT:
			pTrigger->Flags () = TRIGGER_SECRET_EXIT;
			break;
		case TT_ILLUSORY_WALL:
			pTrigger->Flags () = TRIGGER_MAKE_ILLUSIONARY;
			break;
		case TT_SHIELD_DAMAGE_D2:
			pTrigger->Flags () = TRIGGER_SHIELD_DAMAGE;
			break;
		case TT_ENERGY_DRAIN_D2:
			pTrigger->Flags () = TRIGGER_ENERGY_DRAIN;
			break;
		// unsupported types
		default:
			nDeleted [2]++;
			DEBUGMSG (" Level converter: Unsupported trigger type; trigger deleted")
			triggerManager.Delete (i);
			i--;
			pTrigger--;
			continue;
		}
	pTrigger->Type () = 0;
	}

#if 0 
// the following code will remove all segments exceeding the maximum segment count for Descent 1 levels and
// as well as all excess walls and triggers
if (0 < (h = segmentManager.Count () - MAX_SEGMENTS_D1)) {
	for (short nSegment = segmentManager.Count () - 1; nSegment >= 0; nSegment--) {
		if (segmentManager.IsExit (nSegment))
			continue;
		CGameObject* pObject;
		bool bDelete = true;
		for (int i = 0; pObject = objectManager.FindBySeg (nSegment); i = pObject ? pObject->Index () + 1 : objectManager.Count ()) {
			if ((pObject->Type () == OBJ_PLAYER) || pObject->IsBoss () || (pObject->Type () == OBJ_REACTOR)) {
				bDelete = false;
				break;
				}
			}
		if (bDelete) {
			nDeleted [0]++;
			CSideKey key (nSegment);
			CSegment* pSegment = segmentManager.Segment (nSegment);
			for (key.m_nSide = 0; key.m_nSide < 6; key.m_nSide++) {
				CWall* pWall = segmentManager.Wall (key);
				if (pWall) {
					nDeleted [1]++;
					if (pWall->Trigger ())
						nDeleted [2]++;
					}
				}
			segmentManager.Delete (nSegment);
			if (0 >= --h)
				break;
			}
		}
	}

if (0 < (h = wallManager.WallCount () - MAX_WALLS_D1)) {
	for (i = wallManager.WallCount () - 1; i >= 0; i--) {
		if (wallManager.IsExit (i))
			continue;
		j = wallManager.Index (wallManager.OppositeWall (*wallManager.Wall (i)));
		if (wallManager.IsExit (j))
			continue;
		nDeleted [1]++;
		if (wallManager.Wall (i)->Trigger ())
			nDeleted [2]++;
		wallManager.Delete (i);
		if (j >= 0) { 
			nDeleted [1]++;
			if (wallManager.Wall (j)->Trigger ())
				nDeleted [2]++;
			wallManager.Delete (j);
			--i, --h;
			}
		if (h <= 0)
			break;
		}
	}

CWall* pWall = wallManager.Wall (0);
for (i = 0; i < wallManager.WallCount (); i++, pWall++) {
	if ((pWall->Type () == WALL_CLOAKED) || (pWall->Type () == WALL_COLORED)) {
		pWall->Type () = WALL_CLOSED;
		segmentManager.Side (*pWall)->m_info.nBaseTex = 302; // keep transparency by applying empty - light texture
		}
	else if ((pWall->Type () == WALL_DOOR) || (pWall->Type () == WALL_BLASTABLE)) {
		pWall->Clip () = (char) MapD2DoorToD1 (pWall->Clip ());
		}
	pWall->Info ().controllingTrigger = 0;
	pWall->Info ().cloakValue = 0;
	}
#endif

CSegment* pSegment = segmentManager.Segment (0);
//char** names = textureManager.m_names [1];

for (short nSegment = 0; nSegment < segmentManager.Count (); nSegment++, pSegment++) {
	if (pSegment->Function () > SEGMENT_FUNC_ROBOTMAKER)
		segmentManager.Undefine (nSegment);
	pSegment->Props () = 0;

	CSide* pSide = pSegment->m_sides;
	for (short nSide = 0; nSide < 6; nSide++, pSide++) {
#ifdef _DEBUG
		if ((nSegment == nDbgSeg) && ((nDbgSide < 0) || (nSide == nDbgSide)))
			nDbgSeg = nDbgSeg;
#endif
//		if (names != textureManager.m_names [1])
//			names = textureManager.m_names [1];
		if ((pSegment->ChildId (nSide) == -1) || (pSide->m_info.nWall < wallManager.Count ())) {
			d2Texture = pSide->BaseTex ();
			pSide->m_info.nBaseTex = MapD2TextureToD1 (d2Texture);
			*lightManager.LightMap (pSide->BaseTex ()) = lightMap [d2Texture];
			*lightManager.TexColor (pSide->BaseTex ()) = colorMap [d2Texture];
			if (pSide->OvlTex (0)) {
				d2Texture = pSide->OvlTex ();
				short mode = d2Texture & ALIGNMENT_MASK;
				d2Texture &= TEXTURE_MASK;
				pSide->m_info.nOvlTex = MapD2TextureToD1 (d2Texture);
				*lightManager.LightMap (pSide->OvlTex (0)) = lightMap [d2Texture];
				*lightManager.TexColor (pSide->OvlTex (0)) = colorMap [d2Texture];
				pSide->m_info.nOvlTex |= mode;
				}
			}
		}
	}

// change trigger type and flags
//-------------------------------------------
if (triggerManager.WallTriggerCount () > 0)
	pTrigger = triggerManager.Trigger (triggerManager.WallTriggerCount () - 1);
for (i = triggerManager.WallTriggerCount () - 1; i >= 0; i--, pTrigger--) {
	if (!pTrigger->Count ()) {
		nDeleted [2]++;
		triggerManager.Delete (i);
		}
	}

if (0 < (h = triggerManager.WallTriggerCount () - MAX_TRIGGERS_D1)) {
	for (i = triggerManager.WallTriggerCount () - 1; i >= 0; i--) {
		if (!triggerManager.Trigger (i)->IsExit ()) {
			nDeleted [2]++;
			triggerManager.Delete (i);
			if (0 <= --h)
				break;
			}
		}
	}

// set robot_center nProducer and robot_flags2
//-----------------------------------------------
for (i = 0; i < segmentManager.RobotMakerCount (); i++) {
	segmentManager.RobotMaker (i)->m_info.objFlags [1] = 0;
	for (j = 0, pSegment = segmentManager.Segment (0); j <= segmentManager.Count (); j++, pSegment++)
		if ((pSegment->m_info.function == SEGMENT_FUNC_ROBOTMAKER) && (pSegment->m_info.nProducer == i))
				segmentManager.RobotMaker (i)->m_info.nProducer = (short)(pSegment->m_info.value);
	}

// objects
//-----------------------------------------------

if (objectManager.Count () > MAX_OBJECTS_D1) {
	for (i = objectManager.Count () - 1; i >= MAX_OBJECTS_D1; i--) {
		nDeleted [3]++;
		objectManager.Delete (i);
		}
	}

CGameObject* pObject = objectManager.Object (0);
short nPlayers = 0, nReactors = 0, nBosses = 0, nId;

for (i = 0; i < objectManager.Count (); i++, pObject++) {
// int clip numbers for poly Objects () (except robots)
	bool bDelete = false;
	switch (pObject->Type ()) {
		case OBJ_PLAYER: // the player on the console
			if (++nPlayers > MAX_PLAYERS_D2)
				bDelete = true;
			else
				pObject->rType.polyModelInfo.nModel = D1_PLAYER_CLIP_NUMBER;
			break;

		case OBJ_REACTOR: // the control center
			if (++nReactors > 1)
				bDelete = true;
			else
				pObject->rType.polyModelInfo.nModel = D1_REACTOR_CLIP_NUMBER;
			break;

		case OBJ_COOP: // a cooperative player object
			pObject->rType.polyModelInfo.nModel = D1_COOP_CLIP_NUMBER;
			break;

		case OBJ_ROBOT:
			if (0 > (nId = MapD2RobotToD1 (pObject->Id ())))
				bDelete = true;
			else if (pObject->IsBoss ())
				bDelete = (++nBosses > 1);
			pObject->Id () = (ubyte) nId;
			if (pObject->Contents ().count == 0)
				nId = -1;
			else if (pObject->Contents ().type == OBJ_ROBOT)
				nId = MapD2RobotToD1 (pObject->Contents ().id);
			else if (pObject->Contents ().type == OBJ_POWERUP) 
				nId = MapD2PowerupToD1 (pObject->Contents ().id);
			if (0 <= nId) 
				pObject->Contents ().id = (ubyte) nId;
			else {
				if (pObject->Contents ().count && pObject->Contents ().type)
					nDeleted [4]++;
				pObject->Contents ().type = 0;
				pObject->Contents ().id = 0;
				pObject->Contents ().count = 0;
				}
			break;

		case OBJ_POWERUP:
			nId = MapD2PowerupToD1 (pObject->Id ());
			if (0 > nId)
				bDelete = true;
			else
				pObject->Id () = (ubyte) nId;
			break;

		default:
			bDelete = (pObject->Type () >= OBJ_CAMBOT);
		}
	if (bDelete) {
		nDeleted [3]++;
		objectManager.Delete (i);
		i--;
		pObject--;
		}
	}

// d2 light data and indices
//--------------------------------------------

lightManager.DeltaIndexCount () = 0;
lightManager.DeltaValueCount () = 0;
lightManager.Count () = 0;
lightManager.ComputeStaticLight (true);
lightManager.CalcAverageCornerLight (true);
lightManager.ScaleCornerLight (true);
lightManager.SetObjectLight (true);
lightManager.ComputeVariableLight (1);
lightManager.Normalize ();
// d2 reactor and secret segment
//----------------------------------------------
triggerManager.ReactorTime () = 0x1e;
theMine->ReactorStrength () = 0xffffffffL;
objectManager.SecretSegment () = 0L;

objectManager.SecretOrient ().Set (1, 0, 0, 0, 0, 1, 0, 1, 0);
DLE.MineView ()->ResetView (true);
#if 0
sprintf_s (szMsg, sizeof (szMsg), 
			  "%d segments have been deleted\n%d walls have been deleted\n%d triggers have been deleted\n%d objects have been deleted\n%d contained objects have been deleted",
			  nDeleted [0], nDeleted [1], nDeleted [2], nDeleted [3], nDeleted [4]);
#endif
if (szMsg && strlen (szMsg))
	ErrorMsg (szMsg);
}


//------------------------------------------------------------------------

// eof convert.cpp