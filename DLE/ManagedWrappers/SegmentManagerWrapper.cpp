#include "pch.h"
#include "SegmentManager.h"

using namespace System::Collections;

namespace DLEDotNet
{
    namespace ManagedWrappers
    {
        public ref class SegmentManagerWrapper
        {
        public:
            static property SegmentManagerWrapper^ Instance
            {
                SegmentManagerWrapper^ get()
                {
                    if (!s_instance)
                    {
                        s_instance = gcnew SegmentManagerWrapper();
                    }
                    return s_instance;
                }
            }

            property int Count
            {
                int get() { return segmentManager.Count(); }
            }

            // TODO This needs to point to a wrapper or pure managed code cannot use it
            property CSegment& Segments[int]
            {
                CSegment& get(int index) { return *segmentManager.Segment(index); }
            }

        private:
            static SegmentManagerWrapper^ s_instance = nullptr;
        };
    }
}
