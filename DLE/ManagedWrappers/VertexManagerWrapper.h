#pragma once

namespace DLEDotNet
{
    namespace ManagedWrappers
    {
        public ref class Vertex
        {
        private:
            CVertex* m_vertex = nullptr;

        public:
            Vertex(CVertex* vertex);

            property double X
            {
                double get();
                void set(double value);
            }

            property double Y
            {
                double get();
                void set(double value);
            }

            property double Z
            {
                double get();
                void set(double value);
            }
        };
    }
}
