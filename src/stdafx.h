// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__C46B3F46_5EFF_11D2_AE2A_00C0F03014A5__INCLUDED_)
#define AFX_STDAFX_H__C46B3F46_5EFF_11D2_AE2A_00C0F03014A5__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC OLE automation classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//#include "types.h"

#define _ATL_APARTMENT_THREADED
#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
class CDLCModule : public CComModule
{
public:
	LONG Unlock();
	LONG Lock();
	DWORD dwThreadID;
};
extern CDLCModule _Module;
#include <atlcom.h>

#include <assert.h>
#include <errno.h>
#include <io.h>
#include <math.h>
#include <memory.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "define.h"
#include "Vector.h"
#include "matrix.h"
#include "IFileManager.h"
#include "carray.h"
#include "GameItem.h"
#include "types.h"
#include "Vertex.h"
#include "side.h"
#include "segment.h"
#include "IRenderer.h"
#include "GameObject.h"
#include "Light.h"
#include "textures.h"
#include "robot.h"
#include "GlobalData.h"
#include "global.h"
#include "MemoryFile.h"
#include "GameItem.h"
#include "Selection.h"
#include "MineInfo.h"
#include "glew.h"
#include "ViewMatrix.h"
#include "Frustum.h"
#include "FBO.h"
#include "renderer.h"

#include "mine.h"
#include "DlcDoc.h"
#include "MainFrame.h"
#include "dle-xp.h"

#include "ModelTextures.h"
#include "ModelManager.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__C46B3F46_5EFF_11D2_AE2A_00C0F03014A5__INCLUDED_)
