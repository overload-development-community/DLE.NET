#include "pch.h"
#include "VertexManagerWrapper.h"
#include "SegmentManagerWrapper.h"

using namespace LibDescent::Data;
using namespace System::Collections;
using namespace DLEDotNet::ManagedWrappers;

namespace DLEDotNet
{
    namespace ManagedWrappers
    {
        public ref class SegmentManager
        {
        public:
            static property SegmentManager^ Instance
            {
                SegmentManager^ get()
                {
                    if (!s_instance)
                    {
                        s_instance = gcnew SegmentManager();
                    }
                    return s_instance;
                }
            }

            property int Count
            {
                int get() { return segmentManager.Count(); }
            }

            property Segment^ Segments[int]
            {
                Segment^ get(int index) { return gcnew Segment(segmentManager.Segment(index)); }
            }

            int IndexOf(Segment^ segment)
            {
                return segmentManager.Index(segment);
            }

        private:
            static SegmentManager^ s_instance = nullptr;
        };

        Segment::Segment(CSegment* segment) :
            m_segment(segment)
        {}

        inline int Segment::IndexOf(Side^ side)
        {
            return m_segment->SideIndex(side);
        }

        Segment::operator CSegment* ()
        {
            return m_segment;
        }

        SegFunction Segment::Function::get()
        {
            return (SegFunction)m_segment->Function();
        }

        void Segment::Function::set(SegFunction value)
        {
            m_segment->Function() = (int)value;
        }

        double Segment::Light::get()
        {
            return static_cast<double>(m_segment->Info().staticLight) / (24 * 327.68);
        }

        void Segment::Light::set(double value)
        {
            m_segment->m_info.staticLight = static_cast<int>(value * 24 * 327.68);
        }

        int Segment::NumSides::get()
        {
            return 6 - m_segment->Shape();
        }

        Side^ Segment::Sides::get(int index)
        {
            return gcnew Side(m_segment, index);
        }

        Side::Side(CSegment* segment, int sideNum) :
            m_segment(segment),
            m_side(segment->Side(sideNum))
        {}

        Side::operator CSide* ()
        {
            return m_side;
        }

        int Side::NumPoints::get()
        {
            return 4 - m_side->Shape();
        }

        Vertex^ Side::Points::get(int index)
        {
            return gcnew Vertex(m_segment->Vertex(m_side->VertexIdIndex(index)));
        }

        UVL^ Side::UVLs::get(int index)
        {
            return gcnew UVL(m_side->Uvls(index));
        }

        Texture^ Side::PrimaryTexture::get()
        {
            return gcnew Texture(g_data.textureManager->TextureByIndex(m_side->BaseTex()));
        }

        void Side::PrimaryTexture::set(Texture^ value)
        {
            short nSecondaryTexture = m_side->OvlTex(0); // SetTextures doesn't appear to need alignment
            m_side->SetTextures(value->TextureIndex, nSecondaryTexture);
        }

        Texture^ Side::SecondaryTexture::get()
        {
            return gcnew Texture(g_data.textureManager->TextureByIndex(m_side->OvlTex(0)));
        }

        void Side::SecondaryTexture::set(Texture^ value)
        {
            short nPrimaryTexture = m_side->BaseTex();
            m_side->SetTextures(nPrimaryTexture, value->TextureIndex);
        }
    }
}

UVL::UVL(CUVL* uvl) :
    m_uvl(uvl)
{}

double UVL::U::get()
{
    return m_uvl->u;
}

void UVL::U::set(double value)
{
    m_uvl->u = value;
}

double UVL::V::get()
{
    return m_uvl->v;
}

void UVL::V::set(double value)
{
    m_uvl->v = value;
}

double UVL::L::get()
{
    return static_cast<double>(m_uvl->l / 327.68);
}

void UVL::L::set(double value)
{
    m_uvl->l = static_cast<ushort>(value * 327.68);
}
