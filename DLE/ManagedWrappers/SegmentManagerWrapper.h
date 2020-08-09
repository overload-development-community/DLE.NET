#pragma once

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
            Side(CSegment* segment, int sideNum);

            property int NumPoints
            {
                int get();
            }

            property Vertex^ Points[int]
            {
                Vertex ^ get(int index);
            }

            operator CSide* ();
        };

        public ref class Segment
        {
        private:
            CSegment* m_segment = nullptr;

        public:
            Segment(CSegment* segment);

            property LibDescent::Data::SegFunction Function
            {
                LibDescent::Data::SegFunction get();
                void set(LibDescent::Data::SegFunction value);
            }

            property int NumSides
            {
                int get();
            }

            property Side^ Sides[int]
            {
                Side ^ get(int index);
            }

            int IndexOf(Side^ side);

            operator CSegment* ();
        };
    }
}
