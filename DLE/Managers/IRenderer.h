#pragma once

class CGameObject;
namespace RenderModel {
	class CModel;
}

class IRenderer
{
public:
	virtual int Type() = 0;
	virtual IViewMatrix* ViewMatrix() = 0;

	virtual void BeginRender(bool bOrtho = false) = 0;
	virtual void EndRender(bool bSwapBuffers = false) = 0;

	virtual void Polygon(CPoint* points, int nPoints) = 0;
	virtual void TexturedPolygon(short nTexture, tTexCoord2d* texCoords, rgbColord* color,
		CVertex* vertices, int nVertices, ushort* index) = 0;
	virtual int RenderModel(RenderModel::CModel& model, CGameObject* object,
		int nGunId = 0, int nBombId = 0, int nMissileId = 0, int nMissiles = 4) = 0;
};
