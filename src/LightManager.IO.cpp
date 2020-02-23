
#include "Mine.h"
#include "dle-xp.h"

//------------------------------------------------------------------------------

void CLightManager::ReadLightMap (CFileManager& fp, uint nSize)
{
fp.Read (m_lightMap, nSize, 1);
}

//------------------------------------------------------------------------------

void CLightManager::WriteLightMap (CFileManager& fp)
{
fp.Write (m_lightMap, sizeof (m_lightMap), 1);
}

// ------------------------------------------------------------------------

void CLightManager::LoadColors (CColor *pc, int nColors, int nFirstVersion, int nNewVersion, CFileManager& fp)
{
	bool bNewFormat = DLE.LevelVersion () >= nNewVersion;

if (DLE.LevelVersion () > nFirstVersion) { 
	for (; nColors; nColors--, pc++)
		pc->Read (fp, bNewFormat);
	}
}

// ------------------------------------------------------------------------

void CLightManager::SaveColors (CColor *pc, int nColors, CFileManager& fp)
{
for (; nColors; nColors--, pc++)
	pc->Write (fp);
}

// ------------------------------------------------------------------------

void CLightManager::ResetColors (CColor *pc, int nColors)
{
for (; nColors; nColors--, pc++)
	pc->Reset ();
}

//------------------------------------------------------------------------------

void CLightManager::ReadColorMap (CFileManager& fp)
{
LoadColors (&m_texColors [0], MAX_TEXTURES_D2, 0, 0, fp);
}

//------------------------------------------------------------------------------

void CLightManager::WriteColorMap (CFileManager& fp)
{
SaveColors (&m_texColors [0], MAX_TEXTURES_D2, fp);
}

// -----------------------------------------------------------------------------

void CLightManager::ReadColors (CFileManager& fp)
{
if (DLE.LevelVersion () == 9) {
#if 1
	LoadColors (FaceColor (0), segmentManager.Count () * 6, 9, 14, fp);
	LoadColors (FaceColor (0), segmentManager.Count () * 6, 9, 14, fp);
	LoadColors (VertexColor (0), vertexManager.Count (), 9, 15, fp);
#else
	fp.Read (FaceColors (), sizeof (CColor), segmentManager.Count () * 6); //skip obsolete side colors 
	fp.Read (FaceColors (), sizeof (CColor), segmentManager.Count () * 6);
	fp.Read (VertexColors (), sizeof (CColor), vertexManager.Count ());
#endif
	}
else if (DLE.LevelVersion () > 9) {
	LoadColors (VertexColor (0), vertexManager.Count (), 9, 15, fp);
	LoadColors (FaceColor (0), segmentManager.Count () * 6, 9, 14, fp);
	LoadColors (TexColor (0), MAX_TEXTURES_D2, 10, 16, fp);
	}
else {
	ResetColors (VertexColor (0), vertexManager.Count ());
	ResetColors (FaceColor (0), segmentManager.Count () * 6);
	//ResetColors (TexColor (0), MAX_TEXTURES_D2);
	}
}

//------------------------------------------------------------------------------

void CLightManager::WriteColors (CFileManager& fp)
{
SaveColors (VertexColor (0), vertexManager.Count (), fp);
SaveColors (FaceColor (0), segmentManager.Count () * 6, fp);
SaveColors (TexColor (0), MAX_TEXTURES_D2, fp);
}

//------------------------------------------------------------------------------

void CLightManager::ReadVariableLights (CFileManager* fp)
{
if (DLE.LevelVersion () > 6) {
	lightManager.Count () = (short) fp->ReadInt32 ();
	for (int i = 0, j = lightManager.Count (); i < j; i++) {
		if (i < MAX_VARIABLE_LIGHTS) {
			VariableLight (i)->Read (fp);
			if ((VariableLight (i)->m_nSegment < 0) /*|| (VariableLight (i)->m_nSegment >= segmentManager.Count ())*/) 
				--lightManager.Count ();
			}
		else { // skip excess data
			CVariableLight l;
			l.Read (fp);
			}
		}
	}
}

//------------------------------------------------------------------------------

void CLightManager::WriteVariableLights (CFileManager* fp)
{
if (DLE.LevelVersion () > 6) {
	long fPos = fp->Tell ();
	fp->Write (lightManager.Count ());
	for (int i = 0, j = lightManager.Count (); i < j; i++) {
		if ((VariableLight (i)->m_nSegment < 0) || (VariableLight (i)->m_nSegment >= segmentManager.Count ())) 
			--lightManager.Count ();
		else
			VariableLight (i)->Write (fp);
		}
	fp->Seek (fPos, SEEK_SET);
	fp->Write (lightManager.Count ());
	fp->Seek (0, SEEK_END);
	}
}

//------------------------------------------------------------------------------

void CLightManager::ReadLightDeltas (CFileManager* fp)
{
if (DLE.IsD2File ()) {

	bool bD2X = (DLE.LevelVersion () >= 15) && (DLE.FileVersion () >= 34);
	int i;

	for (i = 0; i < m_deltaIndexInfo.count; i++)
		m_deltaIndex [i].Read (fp, bD2X);
	for (i = 0; i < m_deltaValueInfo.count; i++)
		m_deltaValues [i].Read (fp);
	}
else
	ResetInfo ();
}

//------------------------------------------------------------------------------

void CLightManager::WriteLightDeltas (CFileManager* fp)
{
if (DeltaIndexCount () > 0) {
	if ((DLE.LevelVersion () >= 15) && (DLE.FileVersion () >= 34)) 
		SortDeltaIndex ();

	bool bD2X = (DLE.LevelVersion () >= 15) && (DLE.FileVersion () >= 34);
	int i;

	m_deltaIndexInfo.size = 6;
	m_deltaIndexInfo.offset = fp->Tell ();
	for (i = 0; i < m_deltaIndexInfo.count; i++)
		m_deltaIndex [i].Write (fp, bD2X);

	m_deltaValueInfo.size = 8;
	m_deltaValueInfo.offset = fp->Tell ();
	for (i = 0; i < m_deltaValueInfo.count; i++)
		m_deltaValues [i].Write (fp);
#if USE_FREELIST
	UnsortDeltaIndex (); // otherwise the undo manager will screw up
#endif
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
