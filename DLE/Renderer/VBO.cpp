#include "stdafx.h"
#include "rendermodel.h"
#include "oglmatrix.h"
#include "ViewMatrix.h"
#include "Frustum.h"
#include "FBO.h"
#include "renderer.h"

bool CRendererGL::CreateModelVBOData(RenderModel::CModel& model)
{
    glGetError();
    glGenBuffersARB(1, &model.m_vboDataHandle);
    int i = glGetError();
    if (i)
    {
        return false;
    }
    model.m_vboIndexHandle = 0;
    glGenBuffersARB(1, &model.m_vboIndexHandle);
    return true;
}

void CRendererGL::BindModelVBOData(RenderModel::CModel& model)
{
    if (model.m_vertBuf[0].Buffer()) // points to graphics driver buffer for VBO based rendering
    {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, model.m_vboDataHandle);
        glEnableClientState(GL_VERTEX_ARRAY);
        glBufferDataARB(GL_ARRAY_BUFFER, model.m_nFaceVerts * sizeof(RenderModel::CRenderVertex),
            reinterpret_cast<void*> (model.m_vertBuf[0].Buffer()), GL_STATIC_DRAW_ARB);
    }

    if (model.m_index[0].Buffer()) // points to graphics driver buffer for VBO based rendering
    {
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, model.m_vboIndexHandle);
        glEnableClientState(GL_VERTEX_ARRAY);
        glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, model.m_nFaceVerts * sizeof(short),
            reinterpret_cast<void*> (model.m_index[0].Buffer()), GL_STATIC_DRAW_ARB);
    }

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}

void CRendererGL::DestroyModelVBOData(RenderModel::CModel& model)
{
    if (model.m_vboDataHandle)
        glDeleteBuffersARB(1, &model.m_vboDataHandle);
    if (model.m_vboIndexHandle)
        glDeleteBuffersARB(1, &model.m_vboIndexHandle);
}
