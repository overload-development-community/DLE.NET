#pragma once

namespace DLEDotNet
{
    namespace ManagedWrappers
    {
        public interface class ISelection
        {
            property short SegmentId { short get(); }
            property short SideId { short get(); }
            property short PointNum { short get(); }
            property short ObjectId { short get(); }
        };

        public class SelectionAdaptor : public ::ISelection
        {
        public:
            SelectionAdaptor();
            SelectionAdaptor(DLEDotNet::ManagedWrappers::ISelection^ selection);

            // Inherited via ISelection
            short SegmentId() const override;
            short SideId() const override;
            short Edge() const override;
            short Point() const override;
            short ObjectId() const override;
            CSegment* Segment() const override;
            CSide* Side() const override;
            CVertex* Vertex(short vertexNum = 0) const override;
            operator CSideKey() const override;
            CGameObject* Object() const override;
            void SetSegmentId(short segmentId) override;
            void SetSideId(short sideId) override;
            void SetEdge(short edgeNum) override;
            void SetPoint(short pointNum) override;
            void SetObjectId(short objectId) override;

        private:
            short m_segmentId;
            short m_sideId;
            short m_edgeNum;
            short m_pointNum;
            short m_objectId;
        };
    }
}
