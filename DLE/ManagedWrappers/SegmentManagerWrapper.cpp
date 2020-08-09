#include "pch.h"
#include "VertexManagerWrapper.h"
#include "SegmentManagerWrapper.h"

using namespace LibDescent::Data;
using namespace System::Collections;

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
                Segment ^ get(int index) { return gcnew Segment(segmentManager.Segment(index)); }
            }

            int IndexOf(Segment^ segment)
            {
                return segmentManager.Index(segment);
            }

        private:
            static SegmentManager^ s_instance = nullptr;
        };

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

        int Segment::NumSides::get()
        {
            return 6 - m_segment->Shape();
        }

        Side^ Segment::Sides::get(int index)
        {
            return gcnew Side(m_segment, index);
        }
    }
}
