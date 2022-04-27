#pragma once

#include "VertexManagerWrapper.h"
#include "TextureManagerWrapper.h"

namespace DLEDotNet
{
    namespace ManagedWrappers
    {
        public ref class UVL
        {
        private:
            CUVL* m_uvl = nullptr;

        public:
            UVL(CUVL* uvl);

            property double U
            {
                double get();
                void set(double value);
            }

            property double V
            {
                double get();
                void set(double value);
            }

            property double L
            {
                double get();
                void set(double value);
            }
        };

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
                Vertex^ get(int index);
            }

            property UVL^ UVLs[int]
            {
                UVL^ get(int index);
            }

            property Texture^ PrimaryTexture
            {
                Texture^ get();
                void set(Texture^ value);
            }

            property Texture^ SecondaryTexture
            {
                Texture^ get();
                void set(Texture^ value);
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

            property double Light
            {
                double get();
                void set(double value);
            }

            property int NumSides
            {
                int get();
            }

            property Side^ Sides[int]
            {
                Side^ get(int index);
            }

            int IndexOf(Side^ side);

            operator CSegment* ();
        };
    }
}
