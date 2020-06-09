#include "stdafx.h"
#include "ResourceManager.h"

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

ubyte* CResource::Load (const char* szName, const char* szCategory) 
{
Unload ();
HRSRC hRes = FindResourceA (g_data.hModule, szName, szCategory);
if (!hRes)
	return null;
m_handle = LoadResource (g_data.hModule, hRes);
if (!m_handle)
	return null;
m_data = (ubyte *) LockResource (m_handle);
if (!m_data)
	return null;
m_size = SizeofResource (g_data.hModule, hRes);
return m_data;
}

//------------------------------------------------------------------------

CStringResource::CStringResource()
{
	m_value[0] = 0;
	m_hInst = g_data.hModule;
}

char* CStringResource::Load (int nResource)
{
	size_t l = strlen (m_value);

LoadStringA (m_hInst, nResource, m_value + l, int (sizeof (m_value) - l));
return m_value;
}

//------------------------------------------------------------------------

void LoadTextureFromResource(CTexture* texture, int nId)
{
	CResource res;
	ubyte* pData = res.Load(nId);
	if (!pData)
		return;
	texture->LoadFromData(pData, res.Size());
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

//eof ResourceManager.cpp