#include "stdafx.h"
#include "dle-xp.h"
#include "TextureProjector.h"

#define PREVIEWUVLS_EXPAND_INCREMENT 256

CTextureProjector textureProjector;

CTextureProjector::CTextureProjector(void)
{
m_bActive = FALSE;
m_projectScaleU = 0;
m_projectScaleV = 0;
m_bProjectPreview = TRUE;
m_bProjectCopiedOnly = FALSE;
}

CTextureProjector::~CTextureProjector(void)
{
}

void CTextureProjector::Start (int nProjectionMode)
{
	m_nProjectionMode = nProjectionMode;

	if(m_bActive) {
		switch(m_nProjectionMode) {
		case PROJECTION_MODE_PLANAR:
			m_projectScaleU = m_projectScaleV;
			break;

		case PROJECTION_MODE_CYLINDER:
			m_projectScaleU = CalcAverageRadius() * 0.4;
			break;

		default:
			break;
		}
	} else {
		m_bActive = TRUE;

		switch(m_nProjectionMode) {
		case PROJECTION_MODE_PLANAR:
			ResetProjectOffset();
			ResetProjectDirection();
			m_projectScaleU = 1;
			m_projectScaleV = 1;
			break;

		case PROJECTION_MODE_CYLINDER:
			ResetProjectOffset();
			ResetProjectDirection();
			// 4 repeats for every 10 units radius - this scales exactly to a square cross-section.
			// May distort a little for more sides; substantially for triangles but these are rare.
			m_projectScaleU = CalcAverageRadius() * 0.4;
			m_projectScaleV = 1;
			break;

		default:
			break;
		}
	}
}

void CTextureProjector::End (BOOL bApply)
{
	if (bApply)
		Project ();

	m_bActive = FALSE;
	m_projectScaleU = 0;
	m_projectScaleV = 0;
}

void CTextureProjector::ResetProjectOffset ()
{
	m_vCenter = current->Segment ()->ComputeCenter ();
}

void CTextureProjector::ResetProjectDirection ()
{
	CSegment* pSegment = current->Segment ();
	CSide* pSide = current->Side ();
	short nSide = current->SideId ();
	short nEdge = current->Edge ();
	ubyte v1 = pSide->VertexIdIndex (nEdge);
	ubyte v2 = pSide->VertexIdIndex (nEdge + 1);
	pSide->ComputeNormals (pSegment->m_info.vertexIds, m_vCenter);
	pSegment->ComputeCenter (nSide);

	CDoubleVector fVec = pSide->Normal ();
	fVec.Negate ();
	CDoubleVector uVec = vertexManager [pSegment->m_info.vertexIds [v1]] - vertexManager [pSegment->m_info.vertexIds [v2]];
	uVec.Normalize ();
	CDoubleVector rVec = CrossProduct (uVec, fVec);

	m_projectOrient = CDoubleMatrix (rVec, uVec, fVec);
}

void CTextureProjector::ResetProjectScaling ()
{
	switch(m_nProjectionMode) {
	case PROJECTION_MODE_PLANAR:
		m_projectScaleU = 1;
		m_projectScaleV = 1;
		break;

	case PROJECTION_MODE_CYLINDER:
		m_projectScaleU = CalcAverageRadius() * 0.4;
		m_projectScaleV = 1;
		break;

	default:
		break;
	}
}

double CTextureProjector::CalcAverageRadius ()
{
	double sumDistance = 0;
	ushort count = 0;

	#pragma omp parallel for if (segmentManager.Count () > 15) reduction (+ : sumDistance, count)
	for (int i = 0; i < segmentManager.Count (); i++) {
		for (short j = 0; j < MAX_SIDES_PER_SEGMENT; j++) {
			if (!ShouldProjectFace (i, j))
				continue;
			CDoubleVector vSideCenter = segmentManager.Segment (i)->ComputeCenter (j);
			sumDistance += Distance (vSideCenter, ProjectPointOnLine (&m_vCenter, &m_projectOrient.U (), &vSideCenter));
			count++;
		}
	}

	if (!count)
		return 0;
	return sumDistance / count;
}

void CTextureProjector::Project (CDynamicArray< CPreviewUVL > *previewUVLs)
{
	if (!m_bActive || !m_nProjectionMode)
		return;
	if (previewUVLs && !m_bProjectPreview)
		return;

	uint nModifiedUVLs = 0;
	if (!previewUVLs)
		undoManager.Begin (__FUNCTION__, udSegments, true);
	else
		previewUVLs->Create (PREVIEWUVLS_EXPAND_INCREMENT);

	#pragma omp parallel for if (segmentManager.Count () > 15 && !previewUVLs)
	for (int i = 0; i < segmentManager.Count (); i++) {
		#pragma omp parallel for if (segmentManager.Count () > 15 && !previewUVLs)
		for (short j = 0; j < MAX_SIDES_PER_SEGMENT; j++) {
			if (!ShouldProjectFace (i, j))
				continue;

			uint nThisUVL = nModifiedUVLs++;
			if (previewUVLs && nModifiedUVLs >= previewUVLs->Length ()) {
				previewUVLs->Resize (nModifiedUVLs + PREVIEWUVLS_EXPAND_INCREMENT);
			}

			CSegment* pSegment = segmentManager.Segment (i);
			CSide *pSide = pSegment->Side (j);
			CUVL vTranslate;
			double maxAngle = 0, minAngle = (2 * M_PI);
			CUVL uvlModified [4];

			for (short k = 0; k < pSide->VertexCount (); k++) {
				// Copy UVL from face
				uvlModified [k] = *pSide->Uvls (k);

				switch (m_nProjectionMode) {
				case PROJECTION_MODE_PLANAR:
					uvlModified [k].u = Dot (*pSegment->Vertex (j, k) - m_vCenter, m_projectOrient.R ()) / 20.0 - 0.5;
					uvlModified [k].v = Dot (*pSegment->Vertex (j, k) - m_vCenter, m_projectOrient.U ()) / 20.0 - 0.5;
					break;
				case PROJECTION_MODE_CYLINDER:
					{
						CDoubleVector vIntersection = ProjectPointOnPlane (&m_vCenter, &m_projectOrient.U (), pSegment->Vertex (j, k));
						vIntersection = m_projectOrient * (vIntersection - m_vCenter);
						double angle = (vIntersection.v.x || vIntersection.v.z) ? atan3 (vIntersection.v.x, vIntersection.v.z) : 0;
						maxAngle = max (maxAngle, angle);
						minAngle = min (minAngle, angle);
						uvlModified [k].u = angle / (2 * M_PI);
					}
					uvlModified [k].v = Dot (*pSegment->Vertex (j, k) - m_vCenter, m_projectOrient.U ()) / 20.0 - 0.5;
					break;
				default:
					break;
				}
				// V tends to be reversed because textures are top-down
				uvlModified [k].u *= m_projectScaleU;
				uvlModified [k].v *= -m_projectScaleV;
			}
			if (m_nProjectionMode == PROJECTION_MODE_CYLINDER) {
				for (short k = 0; k < pSide->VertexCount (); k++) {
					// Check if the texture wrapped and if so, bump the lower-angle UVs up
					if ((maxAngle - minAngle) > M_PI && uvlModified [k].u < 0) {
						uvlModified [k].u += abs (m_projectScaleU);
					}

					// Translate textures so the first vertex is in the -20 to 20 range
					if (k == 0) {
						vTranslate = CUVL (fmod (uvlModified [k].u, 1) - uvlModified [k].u,
							fmod (uvlModified [k].v, 1) - uvlModified [k].v);
					}
					uvlModified [k] += vTranslate;
				}
			}

			// Write modified UVLs back to face or to preview
			if (previewUVLs) {
				(*previewUVLs) [nThisUVL] = CPreviewUVL (i, j);
				for (short k = 0; k < pSide->VertexCount (); k++) {
					(*previewUVLs) [nThisUVL].m_uvlOld [k] = *pSide->Uvls (k);
					(*previewUVLs) [nThisUVL].m_uvlPreview [k] = uvlModified [k];
				}
			} else {
				for (short k = 0; k < pSide->VertexCount (); k++)
					*pSide->Uvls (k) = uvlModified [k];
			}
		}
	}

	if (!previewUVLs)
		undoManager.End (__FUNCTION__);
	else if (nModifiedUVLs > 0)
		previewUVLs->Resize (nModifiedUVLs);
	else
		previewUVLs->Destroy ();
}

bool CTextureProjector::ShouldProjectFace (short nSegment, short nSide)
{
	CTextureTool *texTool = DLE.ToolView ()->TextureTool ();
	if (!texTool)
		return false;
	CSide *pSide = segmentManager.Segment (nSegment)->Side (nSide);
	if (pSide->Shape () > SIDE_SHAPE_TRIANGLE)
		return false;
	// If any vertices are tagged, use tagged faces
	if (vertexManager.HasTaggedVertices ()) {
		if (!pSide->IsTagged ())
			return false;
	}
	// Find faces that match the saved texture(s) if requested
	if (m_bProjectCopiedOnly && texTool->m_bUse1st && (pSide->BaseTex () != texTool->m_saveTexture [0]))
		return false;
	if (m_bProjectCopiedOnly && texTool->m_bUse2nd && (pSide->OvlTex (0) != texTool->m_saveTexture [1]))
		return false;
	return true;
}

void CTextureProjector::DrawProjectGuides (CRenderer& renderer, CViewMatrix* viewMatrix)
{
if(!m_bActive || !m_nProjectionMode)
	return;
CDC* pDC = renderer.DC ();

CVertex vCenter = m_vCenter;
CVertex projectAxes [3] = { m_projectOrient.R (), m_projectOrient.U (), m_projectOrient.F () };
CVertex planeVertices [4] = { CVertex (-5, -5, 0), CVertex (-5, 5, 0), CVertex (5, 5, 0), CVertex (5, -5, 0) };
CPoint planePoints [4];
CDoubleVector vScale (m_projectScaleU, m_projectScaleV, 1);
int nCylinderLines = (int)ceil (abs (m_projectScaleU));
CVertex *cylinderLineVertices [2];
CVertex cylinderEndVertices [2][60];
CPoint cylinderEndPoints [2][60];

renderer.BeginRender ();
vCenter.Transform (viewMatrix);
vCenter.Project (viewMatrix);
for (int i = 0; i < 3; i++) {
	projectAxes [i].Normalize ();
	projectAxes [i] *= 5.0;
	projectAxes [i] += vCenter;
	projectAxes [i].Transform (viewMatrix);
	projectAxes [i].Project (viewMatrix);
	}
CDoubleMatrix inverse = m_projectOrient.Inverse ();
switch (m_nProjectionMode) {
	case PROJECTION_MODE_PLANAR:
		for (int i = 0; i < 4; i++) {
			planeVertices [i] /= vScale;
			planeVertices [i] = inverse * planeVertices [i];
			planeVertices [i] += vCenter;
			planeVertices [i].Transform (viewMatrix);
			planeVertices [i].Project (viewMatrix);
			planePoints [i] = CPoint (planeVertices [i].m_screen.x, planeVertices [i].m_screen.y);
			}
		break;
	case PROJECTION_MODE_CYLINDER:
		for (int i = 0; i < 2; i++) {
			cylinderLineVertices [i] = new CVertex [nCylinderLines];
			for (int j = 0; j < nCylinderLines; j++) {
				cylinderLineVertices [i][j] = CVertex (0, pow ((double)-1, i) * 5 / m_projectScaleV, 5);
				cylinderLineVertices [i][j].Rotate (CDoubleVector (0, 1, 0), j * 2 * M_PI / m_projectScaleU);
				cylinderLineVertices [i][j] = inverse * cylinderLineVertices [i][j];
				cylinderLineVertices [i][j] += vCenter;
				cylinderLineVertices [i][j].Transform (viewMatrix);
				cylinderLineVertices [i][j].Project (viewMatrix);
				}
			for (int j = 0; j < 60; j++) {
				cylinderEndVertices [i][j] = CVertex (0, pow ((double)-1, i) * 5 / m_projectScaleV, 5);
				cylinderEndVertices [i][j].Rotate (CDoubleVector (0, 1, 0), j * 2 * M_PI / 60);
				cylinderEndVertices [i][j] = inverse * cylinderEndVertices [i][j];
				cylinderEndVertices [i][j] += vCenter;
				cylinderEndVertices [i][j].Transform (viewMatrix);
				cylinderEndVertices [i][j].Project (viewMatrix);
				cylinderEndPoints [i][j] = CPoint (cylinderEndVertices [i][j].m_screen.x,
					cylinderEndVertices [i][j].m_screen.y);
				}
			}
		break;
	default:
		break;
}
renderer.EndRender ();

renderer.BeginRender (true);
renderer.SelectObject ((HBRUSH)GetStockObject (NULL_BRUSH));
static ePenColor pens [3] = { penRed, penMedGreen, penMedBlue };

renderer.Ellipse (vCenter, 4, 4);
for (int i = 0; i < 3; i++) {
	renderer.SelectPen (pens [i] + 1);
	renderer.MoveTo (vCenter.m_screen.x, vCenter.m_screen.y);
	renderer.LineTo (projectAxes [i].m_screen.x, projectAxes [i].m_screen.y);
	}
renderer.SelectPen (penDkGreen + 1);
switch (m_nProjectionMode) {
	case PROJECTION_MODE_PLANAR:
		renderer.Polygon (planePoints, 4);
		break;
	case PROJECTION_MODE_CYLINDER:
		renderer.Polygon (cylinderEndPoints [0], 60);
		renderer.Polygon (cylinderEndPoints [1], 60);
		for (int i = 0; i < nCylinderLines; i++) {
			renderer.MoveTo (cylinderLineVertices [0][i].m_screen.x, cylinderLineVertices [0][i].m_screen.y);
			renderer.LineTo (cylinderLineVertices [1][i].m_screen.x, cylinderLineVertices [1][i].m_screen.y);
			}
		delete [] cylinderLineVertices [0];
		delete [] cylinderLineVertices [1];
		break;
	default:
		break;
}
renderer.EndRender ();
}

void CPreviewUVL::Apply ()
{
CSegment* pSegment = segmentManager.Segment (m_nSegment);
if (pSegment) {
	CSide* pSide = pSegment->Side (m_nSide);
	if (pSide) {
		for (short nVertex = 0; nVertex < pSide->VertexCount (); nVertex++) {
			*pSide->Uvls (nVertex) = m_uvlPreview [nVertex];
			}
		}
	}
}

void CPreviewUVL::Revert ()
{
CSegment* pSegment = segmentManager.Segment (m_nSegment);
if (pSegment) {
	CSide* pSide = pSegment->Side (m_nSide);
	if (pSide) {
		for (short nVertex = 0; nVertex < pSide->VertexCount (); nVertex++) {
			*pSide->Uvls (nVertex) = m_uvlOld [nVertex];
			}
		}
	}
}
