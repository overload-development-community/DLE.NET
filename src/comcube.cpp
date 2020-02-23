// ComCube.cpp : Implementation of CComCube
#include "stdafx.h"
#include "dle-xp.h"
#include "mine.h"
#include "ComCube.h"

/////////////////////////////////////////////////////////////////////////////
// CComCube


STDMETHODIMP CComCube::get_StaticLight(long* pRetVal)
{
	*pRetVal = _pCube->m_info.staticLight;
	return S_OK;
}

STDMETHODIMP CComCube::put_StaticLight(long val)
{
	_pCube->m_info.staticLight = val;
	return S_OK;
}
