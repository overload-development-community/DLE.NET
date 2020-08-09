#include "pch.h"
#include "glew.h"
#include "ViewMatrix.h"
#include "Frustum.h"
#include "FBO.h"
#include "renderer.h"
#include "TextureProjector.h"
#include "TunnelMaker.h"
#include "MineView.Presenter.h"

using namespace System;

namespace DLEDotNet
{
    namespace ManagedWrappers
    {
        enum class PerspectiveMode
        {
            FirstPerson = 1,
            ThirdPerson = 0
        };

        public ref class MineViewPresenter
        {
        public:
            MineViewPresenter(IntPtr targetWindow) :
                m_presenter{ new CMineViewPresenter(reinterpret_cast<HWND>(targetWindow.ToPointer())) }
            {}

            ~MineViewPresenter()
            {
                delete m_presenter;
            }

            void Paint()
            {
                m_presenter->Draw();
            }

            property RendererType Renderer
            {
                RendererType get() { return m_presenter->GetRenderer(); }
                void set(RendererType value) { m_presenter->SetRenderer(value); }
            }

            property PerspectiveMode Perspective
            {
                PerspectiveMode get() { return (PerspectiveMode)m_presenter->Perspective(); }
                void set(PerspectiveMode value) { m_presenter->SetPerspective((int)value); }
            }

        private:
            CMineViewPresenter* m_presenter;
        };
    }
}
