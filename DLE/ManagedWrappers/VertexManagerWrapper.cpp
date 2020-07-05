#include "pch.h"
#include "VertexManager.h"
#include "VertexManagerWrapper.h"

using namespace DLEDotNet::ManagedWrappers;

Vertex::Vertex(CVertex* vertex) :
    m_vertex(vertex)
{
}

double Vertex::X::get()
{
    return m_vertex->v.x;
}

void Vertex::X::set(double value)
{
    m_vertex->v.x = value;
}

double Vertex::Y::get()
{
    return m_vertex->v.y;
}

void Vertex::Y::set(double value)
{
    m_vertex->v.y = value;
}

double Vertex::Z::get()
{
    return m_vertex->v.z;
}

void Vertex::Z::set(double value)
{
    m_vertex->v.z = value;
}
