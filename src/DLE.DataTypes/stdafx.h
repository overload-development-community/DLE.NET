// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#include <assert.h>
#include <math.h>
#include <memory.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Windows.h>
// For CPoint. Might need refactoring if it breaks /clr
#include <atltypes.h>

#include "Define.h"
#include "Vector.h"
#include "matrix.h"
#include "IFileManager.h"
#include "carray.h"

#include "GameItem.h"
#include "Types.h"
#include "Vertex.h"
#include "side.h"
#include "segment.h"
#include "IRenderer.h"
#include "textures.h"
#include "robot.h"
#include "Light.h"
#include "GlobalData.h"

#endif //PCH_H
