#include "pch.h"
#include "SelectionWrapper.h"

namespace DLEDotNet
{
    namespace ManagedWrappers
    {
        SelectionAdaptor::SelectionAdaptor(DLEDotNet::ManagedWrappers::ISelection^ selection) :
            m_segmentId{ selection->SegmentId },
            m_sideId{ selection->SideId },
            m_edgeNum{ selection->PointNum },
            m_pointNum{ selection->PointNum },
            m_objectId{ selection->ObjectId }
        {
        }

        short SelectionAdaptor::SegmentId() const
        {
            return m_segmentId;
        }

        short SelectionAdaptor::SideId() const
        {
            return m_sideId;
        }

        short SelectionAdaptor::Edge() const
        {
            return m_edgeNum;
        }

        short SelectionAdaptor::Point() const
        {
            return m_pointNum;
        }

        short SelectionAdaptor::ObjectId() const
        {
            return m_objectId;
        }

        CSegment* SelectionAdaptor::Segment() const
        {
            return segmentManager.Segment(m_segmentId);
        }

        CSide* SelectionAdaptor::Side() const
        {
            return Segment()->Side(m_sideId);
        }

        CVertex* SelectionAdaptor::Vertex(short vertexNum) const
        {
            return Segment()->Vertex(m_sideId, m_pointNum + vertexNum);
        }

        SelectionAdaptor::operator CSideKey() const
        {
            return CSideKey(m_segmentId, m_sideId);
        }

        CGameObject* SelectionAdaptor::Object() const
        {
            return objectManager.Object(m_objectId);
        }

        void SelectionAdaptor::SetSegmentId(short segmentId)
        {
            m_segmentId = segmentId;
        }

        void SelectionAdaptor::SetSideId(short sideId)
        {
            m_sideId = sideId;
        }

        void SelectionAdaptor::SetEdge(short edgeNum)
        {
            m_edgeNum = edgeNum;
        }

        void SelectionAdaptor::SetPoint(short pointNum)
        {
            m_pointNum = pointNum;
        }

        void SelectionAdaptor::SetObjectId(short objectId)
        {
            m_objectId = objectId;
        }
    }
}
