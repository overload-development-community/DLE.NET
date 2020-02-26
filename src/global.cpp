//***************************************************************************
//
//  NAME:    GLOBAL.CPP
//  
//	PURPOSE:
//	
//	  BY:      Bryan Aamot
//	  
//		REV:
//		
//***************************************************************************

#include "stdafx.h"

#include "global.h"
#include "define.h"
#include "types.h"
#include "mine.h"
#include "LightManager.h"
#include "ModelTextures.h"
#include "ModelManager.h"

char message [4096];
char descentFolder [2][256] = {"\\programs\\d2\\data", "\\programs\\d2\\data"};
char missionFolder [256]= "\\programs\\d2\\missions";
char modFolder [256]= "\\programs\\d2\\mods";
char szPlayerProfile [20] = "";

CMissionData missionData;

GlobalData g_data{ &lightManager, &modelManager, &objectManager,
    &paletteManager, &robotManager, &segmentManager, &textureManager,
    &triggerManager, &wallManager, &vertexManager, &undoManager,
    current };

//------------------------------------------------------------------------------


