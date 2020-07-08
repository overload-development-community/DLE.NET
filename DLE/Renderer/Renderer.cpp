// dlcView.cpp: implementation of the CMineView class
//

#include "stdafx.h"
#include "ViewMatrix.h"
#include "Frustum.h"
#include "FBO.h"
#include "renderer.h"
#include "GLTextures.h"

short nDbgSeg = -1, nDbgSide = -1;
int nDbgVertex = -1;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CRenderer::Reset (const CDoubleVector& currentSegmentCenter)
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

::GetClientRect (m_hParent, rc);
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
				if (!GLTextureHelpers::GLBindTexture(&(*model.m_textures)[nTexture], GL_TEXTURE0, GL_MODULATE))
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

// -----------------------------------------------------------------------------

int CRenderer::DrawObjectArrow(CGameObject& object, int bCurrent)
{
#define ARROW_LINES 6

	int				i;
	CVertex			pt[ARROW_LINES];
	CLongVector		arrowLines[ARROW_LINES] = {
		{ 0,  4, -4},
		{ 0,  0, -4},
		{ 0,  0,  4},
		{-2,  0,  2},
		{ 2,  0,  2},
		{ 0,  0,  4}
	};
	short				xMax = ViewWidth() * 2;
	short				yMax = ViewHeight() * 2;
	bool				bOrtho = Perspective() == 0;

	BeginRender();
	for (i = 0; i < ARROW_LINES; i++) {
		CVertex v(arrowLines[i]);
		object.Transform(pt[i], v);
		pt[i].Transform(ViewMatrix());
		pt[i].Project(ViewMatrix());
	}
	EndRender();

	for (i = 0; i < ARROW_LINES; i++)
		if (!pt[i].InRange(xMax, yMax, Type()))
			return 0;
	if (bCurrent < 0)
		return 1;

	if (bOrtho) {
		BeginRender(true);
		MoveTo(pt[0].m_screen.x, pt[0].m_screen.y);
		for (i = 1; i < 6; i++)
			LineTo(pt[i].m_screen.x, pt[i].m_screen.y);
		if (bCurrent) {
			for (int dx = -1; dx < 2; dx++) {
				for (int dy = -1; dy < 2; dy++) {
					MoveTo(pt[0].m_screen.x + dx, pt[0].m_screen.y + dy);
					for (i = 1; i < 6; i++)
						LineTo(pt[i].m_screen.x + dx, pt[i].m_screen.y + dy);
				}
			}
		}
	}
	else {
		BeginRender();
		SetLineWidth(bCurrent ? 6.0f : 3.0f);
		MoveTo(pt[0]);
		for (i = 1; i < 6; i++)
			LineTo(pt[i]);
		SetLineWidth(3.0f);
	}
	EndRender();
	return 1;
}

// -----------------------------------------------------------------------------

short CGameObject::Sprite(void)
{
	if (Type() == OBJ_EFFECT) {
		switch (Id()) {
		case PARTICLE_ID:
			switch (rType.particleInfo.nType) {
			case PARTICLE_TYPE_SMOKE:
			case PARTICLE_TYPE_SPRAY:
			case PARTICLE_TYPE_WATERFALL:
				return SMOKE_ICON;
			case PARTICLE_TYPE_BUBBLES:
				return BUBBLE_ICON;
			case PARTICLE_TYPE_FIRE:
				return FIRE_ICON;
			case PARTICLE_TYPE_RAIN:
				return RAIN_ICON;
			case PARTICLE_TYPE_SNOW:
				return SNOW_ICON;
				return -1;
			}
			break;
		case LIGHTNING_ID:
			return LIGHTNING_ICON;
		case SOUND_ID:
			return SOUND_ICON;
		case WAYPOINT_ID:
			return WAYPOINT_ICON;
		default:
			return -1;
		}
	}
	if (!HasSprite())
		return -1;
	if ((0 > rType.animationInfo.nAnimation) || (rType.animationInfo.nAnimation >= MAX_ANIMATIONS))
		return -1;
	return g_data.modelManager->Animation(rType.animationInfo.nAnimation)->frames[0];
}

// -----------------------------------------------------------------------------

bool CRenderer::DrawObjectSprite(CGameObject& object)
{
	static rgbColord color = { 1.0, 1.0, 1.0 };
	static ushort index[4] = { 0, 1, 2, 3 };

	short nTexture = object.Sprite();
	bool	bEffect = object.Type() == OBJ_EFFECT;

	if (nTexture < 0)
		return false;

	// We render sprites using the D2 textures even in D1 levels
	const CTexture* pTexture = bEffect ? &g_data.textureManager->Icon(nTexture) : g_data.textureManager->TextureByIndex(nTexture, 1);
	if (!pTexture)
		return false;

	double dx, dy;
	double size = bEffect ? 5.0 : X2D(object.m_info.size);

	if (pTexture->Width() > pTexture->Height()) {
		dx = size;
		dy = dx * float(pTexture->Height()) / float(pTexture->Width());
	}
	else {
		dy = size;
		dx = dy * float(pTexture->Width()) / float(pTexture->Height());
	}

	BeginRender();
	Sprite(pTexture, object.Position(), dx, dy, false);
	EndRender();
	return true;
}

// -----------------------------------------------------------------------------

void CRenderer::DrawObjectHighlight(CGameObject& object, int bCurrent)
{
	IViewMatrix* viewMatrix = ViewMatrix();
	bool				bOrtho = Perspective() == 0;
	CVertex			center;
	double			d;

	BeginRender();
	center = object.Position();
	center.Transform(viewMatrix);
	if (bOrtho) {
		center.Project(viewMatrix);
		d = int(object.HasPolyModel() ? double(modelManager.ScreenRad()) * 1.1 : 2000 / center.m_view.v.z);
	}
	else
		d = object.HasPolyModel() ? X2D(object.m_info.size) * 1.1 : 5;
	EndRender();
	BeginRender(bOrtho);
	SelectPen(bCurrent ? penGold + 1 : penRed + 1, 2);
	Ellipse(center, -d, -d);
	EndRender();
}

// -----------------------------------------------------------------------------

bool CRenderer::IsObjectInView(CGameObject& object, bool wholeObject)
{
	bool result = true;
	short xMaxVisible = ViewWidth() * 2;
	short yMaxVisible = ViewHeight() * 2;

	BeginRender();
	if (wholeObject) {
		// Check visibility of each point of a simulated bounding box. We don't
		// always have a model so will have to construct the vertices
		double cubeOffset = sqrt(pow(X2D(object.m_info.size), 2) / 3);
		bool allVerticesInView = true;
		CVertex boundsVertexRaw, boundsVertexProjected;

		for (int x = 1; true; x *= -1) {
			for (int y = 1; true; y *= -1) {
				for (int z = 1; true; z *= -1) {
					boundsVertexRaw = CVertex(x * cubeOffset, y * cubeOffset, z * cubeOffset);
					object.Transform(boundsVertexProjected, boundsVertexRaw);
					boundsVertexProjected.Transform(ViewMatrix());
					boundsVertexProjected.Project(ViewMatrix());
					allVerticesInView &= boundsVertexProjected.InRange(xMaxVisible, yMaxVisible, Type());
					if (z < 0) break;
				}
				if (y < 0) break;
			}
			if (x < 0) break;
		}

		result = allVerticesInView;
	}
	else {
		// Check visibility of the center of the object only
		CVertex centerProjected;
		object.Transform(centerProjected, CVertex(0, 0, 0));
		centerProjected.Transform(ViewMatrix());
		centerProjected.Project(ViewMatrix());
		result = centerProjected.InRange(xMaxVisible, yMaxVisible, Type());
	}
	EndRender();

	return result;
}

//--------------------------------------------------------------------------

short CRenderer::IsSegmentSelected(CSegment& segment, CRect& viewport, long xMouse, long yMouse, short nSide, bool bSegments)
{
	segment.ComputeCenter();
	IViewMatrix* viewMatrix = ViewMatrix();
	if (bSegments) {
		CVertex& center = segment.Center();
		center.Transform(viewMatrix);
		center.Project(viewMatrix);
		if ((center.m_screen.x < 0) || (center.m_screen.y < 0) || (center.m_screen.x >= viewport.right) || (center.m_screen.y >= viewport.bottom) || (center.m_view.v.z < 0.0))
			return -1;
	}
	short nSegment = segmentManager.Index(&segment);
#ifdef _DEBUG
	if (nSegment == nDbgSeg)
		nDbgSeg = nDbgSeg;
#endif
	CSide* pSide = segment.Side(nSide);
	for (; nSide < 6; nSide++, pSide++) {
		if (!IsSideInFrustum(nSegment, nSide))
			continue;
		pSide->SetParent(nSegment);
		if (!pSide->IsSelected(viewport, xMouse, yMouse, segment.m_info.vertexIds))
			continue;
		if (!bSegments) {
			segment.ComputeCenter(nSide);
			CVertex& center = pSide->Center();
			if (!pSide->IsVisible()) {
				pSide->ComputeNormals(segment.m_info.vertexIds, Center());
				CVertex normal;
				center += pSide->Normal(2) * 2.0;
				center.Transform(viewMatrix);
				center.Project(viewMatrix);
			}
			if ((center.m_screen.x < 0) || (center.m_screen.y < 0) || (center.m_screen.x >= viewport.right) || (center.m_screen.y >= viewport.bottom) || (center.m_view.v.z < 0.0))
				continue;
		}
		return nSide;
	}
	return -1;
}

void CRenderer::Zoom(int nSteps, double zoom, double viewMoveRate)
{
	if (nSteps < 0)
		zoom = 1.0 / zoom;
	else if (nSteps > 1)
		zoom = pow(zoom, nSteps);
	Scale().v.z *= zoom;
	ViewMatrix()->Scale(zoom);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

COLORREF CRenderData::PenColor(int nPen)
{
	static COLORREF penColors[] = {
		RGB(  0,  0,  0),
		RGB(255,255,255),
		RGB(255,196,  0),
		RGB(255,  0,  0),
		RGB(255,  0, 64),
		RGB(128,128,128),
		RGB(160,160,160),
		RGB( 64, 64, 64),
		RGB(  0,255,  0),
		RGB(  0,255,128),
		RGB(  0,128,  0),
		RGB(  0,128,128),
		RGB(  0,192,192),
		RGB(  0,255,255),
		RGB(  0, 64,255),
		RGB(  0,128,255),
		RGB(  0,255,255),
		RGB(255,196,  0),
		RGB(255,128,  0),
		RGB(255,  0,255)
	};

	return (nPen < penCount) ? penColors[nPen] : RGB(0, 0, 0);
}

//------------------------------------------------------------------------------

CRenderData::CRenderData()
{
	m_renderBuffer = 0;
	m_depthBuffer = null;
	m_bDepthTest = true;

	for (int nWeight = 0; nWeight < 2; nWeight++)
		for (int nPen = 0; nPen < penCount; nPen++)
			m_pens[nWeight][nPen] = ::CreatePen(PS_SOLID, nWeight + 1, PenColor(nPen));
}

//------------------------------------------------------------------------------

CRenderData::~CRenderData()
{
	for (int nWeight = 0; nWeight < 2; nWeight++)
		for (int nPen = 0; nPen < penCount; nPen++)
			::DeleteObject(m_pens[nWeight][nPen]);
}

//eof