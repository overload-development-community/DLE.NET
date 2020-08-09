#include "pch.h"
#include "glew.h"
#include "ViewMatrix.h"
#include "Frustum.h"
#include "FBO.h"
#include "renderer.h"
#include "TextureProjector.h"
#include "TunnelMaker.h"
#include "MineView.Presenter.h"

#include "SelectionWrapper.h"

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
                if (m_currentSelection)
                {
                    delete m_currentSelection;
                }
                if (m_otherSelection)
                {
                    delete m_otherSelection;
                }
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

            void UpdateCurrentSelection(DLEDotNet::ManagedWrappers::ISelection^ selection)
            {
                if (m_currentSelection)
                {
                    delete m_currentSelection;
                }
                m_currentSelection = new SelectionAdaptor(selection);
                m_presenter->UpdateCurrentSelection(m_currentSelection);
            }

            void UpdateOtherSelection(DLEDotNet::ManagedWrappers::ISelection^ selection)
            {
                if (m_otherSelection)
                {
                    delete m_otherSelection;
                }
                m_otherSelection = new SelectionAdaptor(selection);
                m_presenter->UpdateOtherSelection(m_otherSelection);
            }

            void UpdateSelectionMode(uint mode)
            {
                m_presenter->UpdateSelectMode(static_cast<SelectMode>(mode));
            }

            void UpdateViewMode(uint mode)
            {
                m_presenter->SetViewMode(static_cast<eViewMode>(mode));
            }

            void UpdateDepthPerception(uint depthPerceptionLevel)
            {
                m_presenter->Renderer().ViewMatrix()->SetDepthScale(depthPerceptionLevel);
            }

            void ResetZoom()
            {
                m_presenter->FitToView();
            }

        private:
            CMineViewPresenter* m_presenter;
            SelectionAdaptor* m_currentSelection;
            SelectionAdaptor* m_otherSelection;
        };
    }
}
