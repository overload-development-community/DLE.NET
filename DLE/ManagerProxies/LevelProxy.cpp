#include "pch.h"
#include "mine.h"

using namespace System;
using namespace LibDescent::Edit;

namespace DLEDotNet
{
    namespace ManagerProxies
    {
        public ref class LevelProxy : public ChangeableState
        {
        public:
            LevelProxy()
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
