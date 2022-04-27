#pragma once

namespace DLEDotNet
{
    namespace ManagedWrappers
    {
        public ref class Texture
        {
        private:
            CTexture* m_texture = nullptr;

        public:
            Texture(CTexture* texture);

            property int TextureIndex
            {
                int get();
            }

            property System::String^ TextureName
            {
                System::String^ get();
            }
        };
    }
}
