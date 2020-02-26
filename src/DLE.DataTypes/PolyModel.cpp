// Copyright (c) 1997 Bryan Aamot
#include "stdafx.h"
#include "PolyModel.h"

//------------------------------------------------------------------------------

void CPolyModel::Read (IFileManager* fp, bool bRenderData) 
{
if (bRenderData) {
	Release ();
	if ((m_info.renderData = new ubyte [m_info.dataSize]))
		fp->Read (m_info.renderData, m_info.dataSize, 1);
	}
else {
	m_info.nModels = fp->ReadInt32 ();
	m_info.dataSize = fp->ReadInt32 ();
	fp->ReadInt32 ();
	Release ();
	for (int i = 0; i < MAX_SUBMODELS; i++)
		m_info.subModels [i].ptr = fp->ReadInt32 ();
	for (int i = 0; i < MAX_SUBMODELS; i++)
		fp->Read (m_info.subModels [i].offset);
	for (int i = 0; i < MAX_SUBMODELS; i++)
		fp->Read (m_info.subModels [i].norm);
	for (int i = 0; i < MAX_SUBMODELS; i++)
		fp->Read (m_info.subModels [i].pnt);
	for (int i = 0; i < MAX_SUBMODELS; i++)
		m_info.subModels [i].rad = fp->ReadInt32 ();
	for (int i = 0; i < MAX_SUBMODELS; i++)
		m_info.subModels [i].parent = (ubyte) fp->ReadSByte ();
	for (int i = 0; i < MAX_SUBMODELS; i++)
		fp->Read (m_info.subModels [i].vMin);
	for (int i = 0; i < MAX_SUBMODELS; i++)
		fp->Read (m_info.subModels [i].vMax);
	fp->Read (m_info.vMin);
	fp->Read (m_info.vMax);
	m_info.rad = fp->ReadInt32 ();
	m_info.textureCount = fp->ReadUByte ();
	m_info.firstTexture = fp->ReadUInt16 ();
	m_info.simplerModel = fp->ReadUByte ();
	}
}

//------------------------------------------------------------------------------
//eof polymodel.cpp

