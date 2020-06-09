#pragma once

#include <assert.h>
#include <math.h>
#include <memory.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <memory>
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
#include "textures.h"
#include "robot.h"
#include "Light.h"
#include "GameObject.h"
#include "GlobalData.h"
#include "ItemIterator.h"
#include "MineInfo.h"
#include "wall.h"
#include "trigger.h"
#include "PolyModel.h"

#include "IRenderer.h"
#include "FileManager.h"
#include "SegmentManager.h"
#include "ObjectManager.h"
#include "TriggerManager.h"
#include "UndoManager.h"
