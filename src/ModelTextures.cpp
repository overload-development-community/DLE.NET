
#include "Types.h"
#include "carray.h"
#include "textures.h"
#include "ModelTextures.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

int CModelTextures::Read (char* pszFolder)
{
	char szFile [256];

for (int i = 0; i < m_nTextures; i++) {
	m_textures [i].Clear ();
	sprintf (szFile, "%s\\%s", pszFolder, m_names [i].Buffer ());
	if (!m_textures [i].LoadFromFile (szFile))
		return 0;
	}
return 1;
}

//------------------------------------------------------------------------------

int CModelTextures::ReadBinary (CFileManager& fp)
{
m_nTextures = fp.ReadInt32 ();
if (m_nTextures > 20) {	//probably invalid
	fp.Close ();
	Destroy ();
	return 0;
	}

if (!Create (m_nTextures)) {
	fp.Close ();
	Destroy ();
	return 0;
	}

for (int i = 0; i < m_nTextures; i++) {
	int h = fp.ReadInt32 ();
	if (h) {
		if (!m_names [i].Create (h)) {
			fp.Close ();
			Destroy ();
			return 0;
			}
		m_names [i].Read (fp);
		if (!m_textures [i].LoadFromFile (m_names [i].Buffer ())) {
			fp.Close ();
			Destroy ();
			return 0;
			}
		}
	}
m_nTeam.Read (fp);
return 1;
}

//------------------------------------------------------------------------------

void CModelTextures::WriteBinary (CFileManager& fp)
{
fp.Write (m_nTextures);
for (int i = 0; i < m_nTextures; i++) {
	int h = int (m_names [i].Length ());
	fp.Write (h);
	fp.Write (m_names [i].Buffer (), 1, h);
	}
fp.Write (m_nTeam.Buffer (), 1, m_nTextures);
}

//------------------------------------------------------------------------------

void CModelTextures::Release (void)
{
if ((m_textures.Buffer ()))
	for (int i = 0; i < m_nTextures; i++)
		m_textures [i].Clear ();
}

//------------------------------------------------------------------------------

int CModelTextures::Bind (int bCustom)
{
if ((m_textures.Buffer ()))
	for (int i = 0; i < m_nTextures; i++) 
		m_textures [i].GLBind (GL_TEXTURE0, GL_MODULATE);
return 1;
}

//------------------------------------------------------------------------------

void CModelTextures::Destroy (void)
{
if (m_names.Buffer ()) {
	for (int i = 0; i < m_nTextures; i++)
		m_names [i].Destroy ();
	m_names.Destroy ();
	}
m_nTeam.Destroy ();
m_textures.Destroy ();
m_nTextures = 0;
}

//------------------------------------------------------------------------------

bool CModelTextures::Create (int nTextures)
{
if (nTextures <= 0)
	return false;
if (!(m_textures.Create (nTextures)))
	return false;
if (!(m_names.Create (nTextures)))
	return false;
if (!(m_nTeam.Create (nTextures)))
	return false;
m_nTextures = nTextures;
m_names.Clear ();
m_nTeam.Clear ();
return true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

