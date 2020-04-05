#pragma once

#include <assert.h>
#include <math.h>
#include <memory.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#include <Windows.h>
#include <sys/types.h>
// For CPoint. Might need refactoring if it breaks /clr
#include <atltypes.h>

#include "Define.h"
#include "Vector.h"
#include "matrix.h"
#include "IFileManager.h"
#include "carray.h"
#include "SLL.h"
#include "AVLTree.h"
#include "cquicksort.h"
#include "cstack.h"

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
#include "ItemIterator.h"
#include "MineInfo.h"
#include "wall.h"
#include "trigger.h"
#include "GameObject.h"
#include "PolyModel.h"
