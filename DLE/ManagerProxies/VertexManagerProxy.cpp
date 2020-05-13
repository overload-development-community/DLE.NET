#include "pch.h"
#include "VertexManager.h"

// TODO This currently generates a lot of linker errors, largely because of GlobalData.h.
// To make further progress, it will be necessary to do further refactoring to move
// implementations out of DLE.Editor where possible, and create alternate implementations
// where it isn't.
// DLE.Managers has been removed from references because of a similar problem; that needs
// fixing, too.
//CVertexManager vertexManager;
