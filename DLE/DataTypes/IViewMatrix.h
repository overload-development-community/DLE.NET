#pragma once

class CViewData {
public:
    CDoubleMatrix m_transformation[2];
    CDoubleVector m_translate;
    CDoubleVector m_rotate;
    CDoubleVector m_scale;
    CDoubleVector m_origin;
};

class IViewMatrix
{
public:
    virtual CViewData& Data(int n) = 0;
    virtual void Transform(CDoubleVector& v, CDoubleVector& w) = 0;
    virtual void Project(CDoubleVector& s, CDoubleVector& v) = 0;
};
