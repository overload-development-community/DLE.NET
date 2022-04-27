#pragma once

namespace DLEDotNet
{
    namespace ManagedWrappers
    {
        public ref class LevelObject
        {
        private:
            CGameObject* m_object= nullptr;

        public:
            LevelObject(CGameObject* object);

            property int ObjectIndex
            {
                int get();
            }
        };
    }
}
