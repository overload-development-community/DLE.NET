#pragma once

typedef struct tTexCoord2d {
	double u, v;
} tTexCoord2d;

typedef unsigned int GLuint;

#define SMOKE_ICON		0
#define SNOW_ICON			1
#define RAIN_ICON			2
#define BUBBLE_ICON		3
#define FIRE_ICON			4
#define LIGHTNING_ICON	5
#define SOUND_ICON		6
#define WAYPOINT_ICON	7
#define LIGHT_ICON		8
#define CIRCLE_ICON		9
#define ICON_COUNT		10

enum ePenColor {
	penBlack,
	penWhite,
	penGold,
	penRed,
	penMedRed,
	penGray,
	penLtGray,
	penDkGray,
	penGreen,
	penMedGreen,
	penDkGreen,
	penDkCyan,
	penMedCyan,
	penLtCyan,
	penBlue,
	penMedBlue,
	penLtBlue,
	penYellow,
	penOrange,
	penMagenta,
	penCount
};

class CViewData {
public:
	CDoubleMatrix	m_transformation[2];
	CDoubleVector	m_translate;
	CDoubleVector	m_rotate;
	CDoubleVector	m_scale;
	CDoubleVector	m_origin;
};

class IViewMatrix
{
public:
	virtual CViewData& Data(int n) = 0;
	virtual void Transform(CDoubleVector& v, CDoubleVector& w) = 0;
	virtual void Project(CDoubleVector& s, CDoubleVector& v) = 0;
};

class CTexture;
class CVertex;
class CGameObject;
namespace RenderModel {
	class CModel;
}

class IRenderer
{
public:
	virtual int Type() = 0;
	virtual int Perspective() = 0;
	virtual IViewMatrix* ViewMatrix() = 0;
	virtual int& ViewWidth() = 0;
	virtual int& ViewHeight() = 0;
	virtual bool IsSideInFrustum(short nSegment, short nSide) = 0;

	virtual void BeginRender(bool bOrtho = false) = 0;
	virtual void EndRender(bool bSwapBuffers = false) = 0;

	virtual void SelectPen(int nPen, float nWeight = 1.0f) = 0;
	virtual void SetLineWidth(float nWeight = 1.0f) = 0;
	virtual void MoveTo(int x, int y) = 0;
	virtual void MoveTo(CVertex& v) = 0;
	virtual void LineTo(int x, int y) = 0;
	virtual void LineTo(CVertex& v) = 0;
	virtual void Ellipse(CVertex& center, double xRad, double yRad) = 0;
	virtual void Polygon(CPoint* points, int nPoints) = 0;
	virtual void TexturedPolygon(short nTexture, tTexCoord2d* texCoords, rgbColord* color,
		CVertex* vertices, int nVertices, ushort* index) = 0;
	virtual void Sprite(const CTexture* pTexture, CVertex center, double width, double height, bool bAlways = false) = 0;
	virtual int RenderModel(RenderModel::CModel& model, CGameObject* object,
		int nGunId = 0, int nBombId = 0, int nMissileId = 0, int nMissiles = 4) = 0;
};
