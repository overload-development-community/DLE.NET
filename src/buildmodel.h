#ifndef _BUILDMODEL_H
#define _BUILDMODEL_H

int AllocModel (RenderModel::CModel *pModel);
int FreeModelItems (RenderModel::CModel *pModel);
void FreeAllPolyModelItems (void);
void SortFaces (RenderModel::CSubModel *subModelP, int left, int right, CBitmap *pTextures);
void SortFaceVerts (RenderModel::CModel *pModel, RenderModel::CSubModel *psm, RenderModel::CVertex *psv);
void SetupModel (RenderModel::CModel *pModel, int bHires, int bSort);
int BuildModel (CObject *pObject, int nModel, CPolyModel *polyModelP, CDynamicArray<CBitmap*>& modelBitmaps, CFloatVector* objColorP, int bHires);

int BuildModelFromASE (CObject *pObject, int nModel);
int BuildModelFromPOF (CObject *pObject, int nModel, CPolyModel *polyModelP, CDynamicArray<CBitmap*>& modelBitmaps, CFloatVector *objColorP);

//------------------------------------------------------------------------------

#endif //_BUILDMODEL_H
//eof
