#include "pch.h"
#include "mine.h"

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
        };
    }
}
