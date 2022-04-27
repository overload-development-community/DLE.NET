#include "pch.h"
#include "glew.h"
#include "ViewMatrix.h"
#include "Frustum.h"
#include "FBO.h"
#include "renderer.h"
#include "TextureProjector.h"
#include "TunnelMaker.h"
#include "MineView.Presenter.h"

#include "Delegates.h"
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
                eViewMode internalViewMode;
                switch (mode)
                {
                case 0: // SparseWireframe
                    internalViewMode = eViewWireFrameSparse;
                    break;
                case 1: // FullWireframe
                    internalViewMode = eViewWireFrameFull;
                    break;
                case 2: // TextureAndWireframe
                    internalViewMode = eViewTexturedWireFrame;
                    break;
                case 3: // TextureOnly
                    internalViewMode = eViewTextured;
                    break;
                default:
                    internalViewMode = eViewWireFrameFull;
                    break;
                }
                m_presenter->SetViewMode(internalViewMode);
            }

            void UpdateDepthPerception(uint depthPerceptionLevel)
            {
                m_presenter->Renderer().ViewMatrix()->SetDepthScale(depthPerceptionLevel);
            }

            void UpdateGeometryVisibility(uint geometryVisibility)
            {
                m_presenter->ViewMineFlags() = static_cast<eMineViewFlags>(geometryVisibility);
            }

            void UpdateObjectVisibility(uint objectVisibility)
            {
                m_presenter->ViewObjectFlags() = static_cast<eObjectViewFlags>(objectVisibility);
            }

            void CenterCurrentSegment()
            {
                m_currentSelection->Segment()->ComputeCenter();
                m_presenter->Renderer().SetCenter(m_currentSelection->Segment()->Center(), 0);
            }

            void CenterCurrentObject()
            {
                CVertex position(m_currentSelection->Object()->Position());
                m_presenter->Renderer().SetCenter(position, 0);
            }

            void ResetZoom()
            {
                m_presenter->FitToView();
            }

            void ZoomIn()
            {
                m_presenter->Renderer().ZoomIn(1, false, g_proxyDelegateManager->GetViewMoveRate());
            }

            void ZoomOut()
            {
                m_presenter->Renderer().ZoomOut(1, false, g_proxyDelegateManager->GetViewMoveRate());
            }

            short GetNearestSegmentNumToScreenPosition(int x, int y)
            {
                SelectionAdaptor nearestElement;
                m_presenter->FindNearestTexturedSide(x, y, &nearestElement);
                return nearestElement.SegmentId();
            }

            short GetNearestSideNumToScreenPosition(int x, int y)
            {
                SelectionAdaptor nearestElement;
                m_presenter->FindNearestTexturedSide(x, y, &nearestElement);
                return nearestElement.SideId();
            }

            short GetNearestLineNumToScreenPosition(int x, int y)
            {
                SelectionAdaptor nearestElement;
                m_presenter->FindNearestLine(x, y, &nearestElement, m_currentSelection->Side());
                return nearestElement.Edge();
            }

            short GetNearestPointNumToScreenPosition(int x, int y)
            {
                SelectionAdaptor nearestElement;
                m_presenter->FindNearestVertex(x, y, &nearestElement, m_currentSelection->Side());
                return nearestElement.Point();
            }

            short GetNearestObjectNumToScreenPosition(int x, int y)
            {
                SelectionAdaptor nearestElement;
                m_presenter->FindNearestObject(x, y, &nearestElement);
                return nearestElement.ObjectId();
            }

        private:
            CMineViewPresenter* m_presenter;
            SelectionAdaptor* m_currentSelection;
            SelectionAdaptor* m_otherSelection;
        };
    }
}
