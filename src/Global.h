

#ifndef __global_h
#define __global_h

#include "types.h"
#include "segment.h"
#include "GameObject.h"

extern CMissionData missionData;
extern char message [4096];
extern char descentFolder [2][256];
extern char missionFolder [256];
extern char modFolder [256];
extern char szPlayerProfile [20];

extern ubyte doorClipTable [NUM_OF_CLIPS_D2];

#endif //__global_h4