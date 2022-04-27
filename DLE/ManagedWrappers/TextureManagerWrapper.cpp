#include "pch.h"
#include "TextureManagerWrapper.h"
#include "TextureManager.h"

using namespace DLEDotNet::ManagedWrappers;

namespace DLEDotNet
{
    namespace ManagedWrappers
    {
        public ref class TextureManager
        {
        public:
            static property TextureManager^ Instance
            {
                TextureManager^ get()
                {
                    if (!s_instance)
                    {
                        s_instance = gcnew TextureManager();
                        // Texture info will not be loaded from .PIG without calling Reload, but
                        // in DLE.NET we don't have a valid PIG path yet so we have to defer
                        textureManager.Setup();
                    }
                    return s_instance;
                }
            }

            property int Count
            {
                int get() { return g_data.textureManager->MaxTextures(); }
            }

            property Texture^ Textures[int]
            {
                Texture^ get(int index) { return gcnew Texture(g_data.textureManager->Textures(index)); }
            }

            void Reload()
            {
                textureManager.LoadTextures();
            }

        private:
            static TextureManager^ s_instance = nullptr;
        };
    }
}

Texture::Texture(CTexture* texture) :
    m_texture(texture)
{}

int Texture::TextureIndex::get()
{
    return static_cast<int>(m_texture->Index());
}

System::String^ Texture::TextureName::get()
{
    return gcnew System::String(m_texture->Name());
}
