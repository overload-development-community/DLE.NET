// dlcView.cpp: implementation of the CMineView class
//

#include "stdafx.h"

short nDbgSeg = -1, nDbgSide = -1;
int nDbgVertex = -1;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CRenderer::Reset (void)
{
Center ().Clear ();
Translation ().Clear ();
Scale ().Set (1.0, 1.0, 1.0);
Rotation ().Set (M_PI / 4.0, M_PI / 4.0, 0.0);
}

//------------------------------------------------------------------------------

bool CRenderer::InitViewDimensions (void)
{
	CRect	rc;

m_pParent->GetClientRect (rc);
int width = (rc.Width () + 3) & ~3; // long word align
int height = rc.Height ();
if ((ViewWidth () == width) && (ViewHeight () == height))
	return false;
ViewWidth () = width;
ViewHeight () = height;
return true;
}

//------------------------------------------------------------------------------

void CRenderer::ComputeViewLimits (CRect* pRC)
{
	CRect		rc (LONG_MAX, LONG_MAX, -LONG_MAX, -LONG_MAX);

MinViewPoint ().m_screen.z = LONG_MAX;
MaxViewPoint ().m_screen.z = -LONG_MAX;
for (int h, i = 0, j = vertexManager.Count (); i < j; i++) {
	CVertex& v = vertexManager [i];
	if (v.Status () == 255)
		continue;
	h = v.m_screen.x;
	if (rc.left > h) {
		rc.left = h;
		MinViewPoint ().v.x = h;
		}
	if (rc.right < h) {
		rc.right = h;
		MaxViewPoint ().v.x = h;
		}
	h = v.m_screen.y;
	if (rc.top > h) {
		rc.top = h;
		MinViewPoint ().v.y = h;
		}
	if (rc.bottom < h) {
		rc.bottom = h;
		MaxViewPoint ().v.y = h;
		}
	h = v.m_screen.z;
	if (MinViewPoint ().m_screen.z > h) {
		MinViewPoint ().v.z = h;
		MinViewPoint ().m_screen.z = h;
		}
	if (MaxViewPoint ().m_screen.z < h) {
		MaxViewPoint ().v.z = h;
		MaxViewPoint ().m_screen.z = h;
		}
	}
MinViewPoint ().m_screen.x = rc.left;
MaxViewPoint ().m_screen.x = rc.right;
MinViewPoint ().m_screen.y = rc.bottom;
MaxViewPoint ().m_screen.y = rc.top;

if (pRC)
	*pRC = rc;
}

//------------------------------------------------------------------------------

void CRenderer::ComputeBrightness (CFaceListEntry& face, ushort brightness [4], int bVariableLights)
{
#ifdef _DEBUG
if ((face.m_nSegment == nDbgSeg) && ((nDbgSide < 0) || (face.m_nSide == nDbgSide)))
	nDbgSeg = nDbgSeg;
#endif

	CSegment*	pSegment = segmentManager.Segment (face.m_nSegment);
	CSide*		pSide = pSegment->Side (face.m_nSide);
	short			nVertices = pSide->VertexCount ();
	int			b [4];

for (int i = 0; i < nVertices; i++) {
	b [i] = (int) pSide->m_info.uvls [i].l;
	// clip brightness
	//if (b [i] & 0x8000) {
	//	b [i] = 0x7fff;
	//	}
	}

#ifdef _DEBUG
if ((face.m_nSegment == nDbgSeg) && ((nDbgSide < 0) || (face.m_nSide == nDbgSide)))
	nDbgSeg = nDbgSeg;
#endif

if (bVariableLights /*&& (lightManager.LightIsOn (face) < 1)*/) {
	// check each variable light whether it affects side face
	// search delta light index to see if current side has light
	int indexCount = lightManager.DeltaIndexCount ();
//#pragma omp parallel for if (indexCount > 15)
	for (int i = 0; i < indexCount; i++) {
		CLightDeltaIndex* pIndex = lightManager.LightDeltaIndex (i);
#ifdef _DEBUG
		if ((pIndex->m_nSegment == nDbgSeg) && ((nDbgSide < 0) || (pIndex->m_nSide == nDbgSide)))
			nDbgSeg = nDbgSeg;
#endif
		if (lightManager.LightIsOn (*pIndex))
			continue; // light is on or there's no variable light for this delta light index (e.g. only blastable)
		if (*pIndex == face) {
			int lightDelta = (int) lightManager.Brightness (face);
			for (short k = 0; k < nVertices; k++) 
//#pragma omp atomic
				b [k] -= lightDelta;
			}
		else {
			CLightDeltaValue* pLightDelta = lightManager.LightDeltaValue (pIndex->m_info.index);
			ushort nSides = pIndex->m_info.count;
			for (ushort j = 0; j < nSides; j++, pLightDelta++) {
				if (*pLightDelta == face) {
					for (short k = 0; k < nVertices; k++) {
						//short lightDelta = ;
						//if (lightDelta >= 0x20)
						//	lightDelta = 0x7fff;
						//else
						//	lightDelta <<= 10;
//#pragma omp atomic
						b [k] -= pLightDelta->m_info.vertLight [k];
						}
					break;
					}
				}
			}
		}
	}

for (int i = 0; i < nVertices; i++) 
	brightness [i] = (b [i] < 0) ? 0 : (b [i] > 32767) ? 32767 : b [i];

#ifdef _DEBUG
if ((face.m_nSegment == nDbgSeg) && ((nDbgSide < 0) || (face.m_nSide == nDbgSide)))
	nDbgSeg = nDbgSeg;
#endif
}

//------------------------------------------------------------------------------

static int nLevel = 0;

void CRenderer::RenderSubModel(RenderModel::CModel& model, short nSubModel, int nGunId, int nBombId, int nMissileId, int nMissiles)
{
	RenderModel::CFace* pFace;
	int						i;
	short						nTexture = -1;
	ushort					nFaceVerts, nVerts, nIndex;
	auto submodel = model.m_subModels[nSubModel];

	if (submodel.Filter(nGunId, nBombId, nMissileId, nMissiles))
		return;

	if ((0 != submodel.m_vOffset.v.x) || (0 != submodel.m_vOffset.v.y) || (0 != submodel.m_vOffset.v.z))
		glTranslatef(X2F(submodel.m_vOffset.v.x), X2F(submodel.m_vOffset.v.y), X2F(submodel.m_vOffset.v.z));

	// render any dependent submodels
	for (int i = 0; i < model.m_nSubModels; i++) {
		if (model.m_subModels[i].m_nParent == nSubModel)
			RenderSubModel(model, i, nGunId, nBombId, nMissileId, nMissiles);
	}

	if (submodel.m_bBillboard) {
		float modelView[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				modelView[i * 4 + j] = (i == j) ? 1.0f : 0.0f;
		glLoadMatrixf(modelView);
	}

	glDisable(GL_TEXTURE_2D);

	for (i = submodel.m_nFaces, pFace = submodel.m_faces; i; ) {
		if (nTexture != pFace->m_nTexture) {
			if (0 > (nTexture = pFace->m_nTexture))
				glDisable(GL_TEXTURE_2D);
			else {
				glEnable(GL_TEXTURE_2D);
				if (!(*model.m_textures)[nTexture].GLBind(GL_TEXTURE0, GL_MODULATE))
					continue;
			}
		}
		nIndex = pFace->m_nIndex;
		if ((nFaceVerts = pFace->m_nVerts) > 4) {
			nVerts = nFaceVerts;
			pFace++;
			i--;
		}
		else {
			short nId = pFace->m_nId;
			nVerts = 0;
			do {
				nVerts += nFaceVerts;
				pFace++;
				i--;
			} while (i && (pFace->m_nId == nId));
		}
		glDrawRangeElements((nFaceVerts == 3) ? GL_TRIANGLES : (nFaceVerts == 4) ? GL_QUADS : GL_TRIANGLE_FAN,
			0, model.m_nFaceVerts - 1, nVerts, GL_UNSIGNED_SHORT,
			VBO_OFFSET(nIndex * sizeof(short)));
	}

	if ((0 != submodel.m_vOffset.v.x) || (0 != submodel.m_vOffset.v.y) || (0 != submodel.m_vOffset.v.z))
		glTranslatef(-X2F(submodel.m_vOffset.v.x), -X2F(submodel.m_vOffset.v.y), -X2F(submodel.m_vOffset.v.z));
}

int CRenderer::RenderModel(RenderModel::CModel& model, CGameObject* object,
	int nGunId, int nBombId, int nMissileId, int nMissiles)
{
	if (model.m_nModel < 0)
		return 0;
	glActiveTexture(GL_TEXTURE0);
	glClientActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, model.m_vboDataHandle);
	glNormalPointer(GL_FLOAT, 0, VBO_OFFSET(model.m_nFaceVerts * sizeof(CFloatVector)));
	glColorPointer(4, GL_FLOAT, 0, VBO_OFFSET(model.m_nFaceVerts * 2 * sizeof(CFloatVector)));
	glTexCoordPointer(2, GL_DOUBLE, 0, VBO_OFFSET(model.m_nFaceVerts * (2 * sizeof(CFloatVector) + sizeof(rgbaColorf))));
	glVertexPointer(3, GL_FLOAT, 0, VBO_OFFSET(0));
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, model.m_vboIndexHandle);

	if (model.m_nType < 0)
		glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(1);
	glColor3f(1.0f, 1.0f, 1.0f);

	ViewMatrix()->SetupOpenGL();
	ViewMatrix()->SetupOpenGL(&object->Orient(), &object->Position());

	for (int i = 0; i < model.m_nSubModels; i++) {
		if (model.m_subModels[i].m_nParent == -1)
			RenderSubModel(model, i, nGunId, nBombId, nMissileId, nMissiles);
	}

	for (; nLevel > 0; nLevel--)
		glPopMatrix();

	ViewMatrix()->ResetOpenGL();
	ViewMatrix()->ResetOpenGL();

	if (model.m_nType < 0)
		glCullFace(GL_FRONT);
	model.m_bRendered = 1;
	glCullFace(GL_FRONT);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
	glDisable(GL_TEXTURE_2D);
	return 1;
}

void CRenderer::DrawTunnelMaker(CViewMatrix* viewMatrix)
{
	DrawTunnelMakerPath(tunnelMaker.Path(), viewMatrix);
	DrawTunnelMakerTunnel(tunnelMaker.Tunnel(), viewMatrix);
}

void CRenderer::DrawTunnelMakerPath(const CTunnelPath* path, CViewMatrix* viewMatrix)
{
#ifdef _DEBUG

	for (int i = 0; i <= path->m_nSteps; i++)
		DrawTunnelMakerPathNode(path->m_nodes[i], viewMatrix);

#else

	// Haven't confirmed this works, there's no explicit copy-constructor.
	// If this bugs out, make one.
	CCubicBezier bezier(path->Bezier());

	BeginRender();
	for (int i = 0; i < 4; i++) {
		bezier.Transform(viewMatrix);
		bezier.Project(viewMatrix);
	}
	EndRender();

	BeginRender(true);
	SelectObject((HBRUSH)GetStockObject(NULL_BRUSH));
	SelectPen(penRed + 1);

	CMineView* mineView = DLE.MineView();
	if (bezier.GetPoint(1).InRange(mineView->ViewMax().x, mineView->ViewMax().y, Type())) {
		if (bezier.GetPoint(0).InRange(mineView->ViewMax().x, mineView->ViewMax().y, Type())) {
			MoveTo(bezier.GetPoint(0).m_screen.x, bezier.GetPoint(0).m_screen.y);
			LineTo(bezier.GetPoint(1).m_screen.x, bezier.GetPoint(1).m_screen.y);
			Ellipse(bezier.GetPoint(1), 4, 4);
		}
	}
	if (bezier.GetPoint(2).InRange(mineView->ViewMax().x, mineView->ViewMax().y, Type())) {
		if (bezier.GetPoint(3).InRange(mineView->ViewMax().x, mineView->ViewMax().y, Type())) {
			MoveTo(bezier.GetPoint(3).m_screen.x, bezier.GetPoint(3).m_screen.y);
			LineTo(bezier.GetPoint(2).m_screen.x, bezier.GetPoint(2).m_screen.y);
			Ellipse(bezier.GetPoint(2), 4, 4);
		}
	}
	EndRender();

#endif
}

void CRenderer::DrawTunnelMakerPathNode(const CTunnelPathNode& node, CViewMatrix* viewMatrix)
{
	CDoubleMatrix m;
	m = node.m_rotation.Inverse();
	CVertex v[4] = { m.R(), m.U(), m.F(), node.m_axis };

	BeginRender();
	CVertex vertex(node.m_vertex);
	vertex.Transform(viewMatrix);
	vertex.Project(viewMatrix);
	for (int i = 0; i < 4; i++) {
		v[i].Normalize();
		v[i] *= 5.0;
		v[i] += vertex;
		v[i].Transform(viewMatrix);
		v[i].Project(viewMatrix);
	}
	EndRender();

	BeginRender(true);
	SelectObject((HBRUSH)GetStockObject(NULL_BRUSH));
	static ePenColor pens[4] = { penRed, penMedGreen, penMedBlue, penOrange };

	Ellipse(vertex, 4, 4);
	for (int i = 0; i < 4; i++) {
		SelectPen(pens[i] + 1);
		MoveTo(vertex.m_screen.x, vertex.m_screen.y);
		LineTo(v[i].m_screen.x, v[i].m_screen.y);
	}
	EndRender();
}

void CRenderer::DrawTunnelMakerTunnel(const CTunnel* tunnel, CViewMatrix* viewMatrix)
{
	BeginRender();
	for (int i = 0; i <= tunnel->m_nSteps; i++) {
		for (int j = 0; j < 4; j++) {
			CVertex& v = vertexManager[tunnel->m_segments[i].m_nVertices[j]];
			v.Transform(viewMatrix);
			v.Project(viewMatrix);
		}
	}
	EndRender();

	BeginRender(true);
	SelectObject((HBRUSH)GetStockObject(NULL_BRUSH));
	SelectPen(penBlue + 1);
	CMineView* mineView = DLE.MineView();
	for (int i = 1; i <= tunnel->m_nSteps; i++)
		DrawTunnelMakerSegment(tunnel->m_segments[i]);
	EndRender();
}

void CRenderer::DrawTunnelMakerSegment(const CTunnelSegment& segment)
{
	BeginRender(false);
#ifdef NDEBUG
	if (mineView->GetRenderer() && (mineView->ViewOption(eViewTexturedWireFrame) || mineView->ViewOption(eViewTextured)))
#endif
	{
		glLineStipple(1, 0x0c3f);  // dot dash
		glEnable(GL_LINE_STIPPLE);
	}
	for (int i = (int)segment.m_elements.Length(); --i >= 0; )
		DLE.MineView()->DrawSegmentWireFrame(segmentManager.Segment(segment.m_elements[i].m_nSegment), false, false, 1);
	EndRender();
#ifdef NDEBUG
	if (mineView->GetRenderer() && (mineView->ViewOption(eViewTexturedWireFrame) || mineView->ViewOption(eViewTextured)))
#endif
		glDisable(GL_LINE_STIPPLE);
}

//eof