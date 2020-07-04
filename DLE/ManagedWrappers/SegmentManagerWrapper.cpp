#include "pch.h"
#include "SegmentManager.h"
#include "VertexManagerWrapper.h"

using namespace LibDescent::Data;
using namespace System::Collections;

namespace DLEDotNet
{
    namespace ManagedWrappers
    {
        public ref class Side
        {
        private:
            CSegment* m_segment = nullptr;
            CSide* m_side = nullptr;

        public:
            Side(CSegment* segment, int sideNum) :
                m_segment(segment),
                m_side(segment->Side(sideNum))
            {}

            property int NumPoints
            {
                int get() { return 4 - m_side->Shape(); }
            }

            property Vertex^ Points[int]
            {
                Vertex^ get(int index) { return gcnew Vertex(m_segment->Vertex(m_side->VertexIdIndex(index))); }
            }
        };

        public ref class Segment
        {
        private:
            CSegment* m_segment = nullptr;

        public:
            Segment(CSegment* segment) :
                m_segment(segment)
            {}

            property SegFunction Function
            {
                SegFunction get() { return (SegFunction)m_segment->Function(); }
                void set(SegFunction value) { m_segment->Function() = (int)value; }
            }

            property int NumSides
            {
                int get() { return 6 - m_segment->Shape(); }
            }

            property Side^ Sides[int]
            {
                Side^ get(int index) { return gcnew Side(m_segment, index); }
            }
        };

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

        private:
            static SegmentManager^ s_instance = nullptr;
        };
    }
}
