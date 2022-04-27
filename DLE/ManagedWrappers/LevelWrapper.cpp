#include "pch.h"
#include "mine.h"

#include "ObjectWrapper.h"
#include "SegmentManagerWrapper.h"

using namespace System;

namespace DLEDotNet
{
    namespace ManagedWrappers
    {
        public ref class Level
        {
        public:
            Level()
            {
                if (!theMine)
                {
                    theMine = new CMine();
                }
            }

            void Open(String^ fileName)
            {
                theMine->Load(msclr::interop::marshal_as<std::string>(fileName).c_str());
            }

            property Segment^ Segments[int]
            {
                Segment^ get(int index)
                {
                    return gcnew Segment(segmentManager.Segment(index));
                }
            }

            property int SegmentCount
            {
                int get() { return segmentManager.Count(); }
            }

            property LevelObject^ Objects[int]
            {
                LevelObject^ get(int index)
                {
                    return gcnew LevelObject(objectManager.Object(index));
                }
            }

            property int ObjectCount
            {
                int get() { return objectManager.Count(); }
            }
        };
    }
}
