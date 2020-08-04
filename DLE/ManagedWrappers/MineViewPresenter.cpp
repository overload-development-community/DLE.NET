#include "pch.h"
#include "glew.h"
#include "ViewMatrix.h"
#include "Frustum.h"
#include "FBO.h"
#include "renderer.h"
#include "TextureProjector.h"
#include "TunnelMaker.h"
#include "MineView.Presenter.h"

namespace DLEDotNet
{
    namespace ManagedWrappers
    {
        public ref class MineViewPresenter
        {
        public:
            MineViewPresenter(HWND targetWindow) :
                m_presenter(new CMineViewPresenter(targetWindow))
            {}

            ~MineViewPresenter()
            {
                delete m_presenter;
            }

        private:
            CMineViewPresenter* m_presenter;
        };
    }
}
