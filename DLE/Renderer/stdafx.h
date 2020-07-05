#pragma once

#include <windows.h>
#include <stddef.h>
#include <atltypes.h> // CPoint
#include <io.h>

#include <string>
#include <vector>

// glew
#include <glew.h>

// DLE.Core
#include "Define.h"
#include "Vector.h"
#include "matrix.h"
#include "IFileManager.h"
#include "carray.h"
#include "cstack.h"
#include "SLL.h"
#include "AVLTree.h"

// DLE.DataTypes
#include "GameItem.h"
#include "Vertex.h"
#include "Types.h"
#include "textures.h"
#include "side.h"
#include "segment.h"
#include "ModelTextures.h"
#include "ASEModel.h"
#include "OOFModel.h"
#include "MineInfo.h"
#include "ItemIterator.h"
#include "IViewMatrix.h"
#include "GameObject.h"
// Begin GlobalData section
#include "Light.h"
#include "robot.h"
#include "GlobalData.h"
// End GlobalData section
#include "PolyModel.h"
#include "rendermodel.h"

// DLE.Managers
#include "FileManager.h"
#include "IRenderer.h"
#include "PaletteManager.h"
#include "SegmentManager.h"
#include "TunnelMaker.h"
#include "VertexManager.h"
#include "LightManager.h"
#include "TextureManager.h"
#include "MemoryFile.h"
#include "ModelManager.h"
