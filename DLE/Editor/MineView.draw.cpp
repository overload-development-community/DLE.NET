// MineView.Draw.cpp: Mine rendering
//

#include "stdafx.h"

extern short nDbgSeg, nDbgSide;
extern int nDbgVertex;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

static inline double sqr(long v) { return double(v) * double(v); }

void CMineView::UpdateNearestSelection()
{
	nearest->SetSegmentId(-1);
	nearest->SetSideId(-1);
	nearest->SetEdge(-1);
	m_presenter.UpdateNearestSelection(nullptr);

	if ((m_inputHandler.MouseState() == eMouseStateSelect && m_inputHandler.HasMouseMovedInCurrentState()) ||
		m_inputHandler.MouseState() == eMouseStateApplySelect)
	{
		switch (m_selectMode)
		{
		case SelectMode::Point:
		{
			CRect viewport;
			GetClientRect(viewport);

			double minDist = 1e30;
			short nNearestVertex = -1;

			CDoubleVector mousePos(double(LastMousePos().x), double(LastMousePos().y), 0.0);

			short nVertices = vertexManager.Count();
			for (short nVertex = 0; nVertex < nVertices; nVertex++)
			{
				CVertex& v = vertexManager[nVertex];
				if (!v.InRange(viewport.right, viewport.bottom, GetRenderer()))
					continue;
				double dist = Distance(mousePos, v.m_proj);
				if (minDist > dist)
				{
					minDist = dist;
					nNearestVertex = nVertex;
				}
			}
			if (nNearestVertex >= 0)
			{
				nearest->Setup(-1, -1, -1, nNearestVertex);
				m_presenter.UpdateNearestSelection(nearest);
			}
		}
		break;

		case SelectMode::Line:
		{
			CSegment* nearestSegment = null;
			CSide* nearestSide = null;
			short nNearestEdge = FindNearestLine(&nearestSegment, &nearestSide, false);
			if (nNearestEdge >= 0)
			{
				nearest->Setup(segmentManager.Index(nearestSegment), nearestSegment->SideIndex(nearestSide), nNearestEdge, nNearestEdge);
				m_presenter.UpdateNearestSelection(nearest);
			}
		}
		break;

		case SelectMode::Side:
		{
			CRect viewport;
			GetClientRect(viewport);

			auto selectableSides = GatherSelectableSides(viewport, LastMousePos().x, LastMousePos().y, ViewFlag(eViewMineSkyBox), false);
			if (!selectableSides.empty())
			{
				double minDist = 1e30;

				CSegment* nearestSegment = null;
				CSide* nearestSide = null;

				for (CSide* pSide : selectableSides)
				{
					CSegment* pSegment = segmentManager.Segment(pSide->GetParent());
					CVertex& center = pSide->Center();
					double dist = sqrt(sqr(LastMousePos().x - center.m_screen.x) + sqr(LastMousePos().y - center.m_screen.y));
					if (minDist > dist)
					{
						minDist = dist;
						nearestSegment = segmentManager.Segment(pSide->GetParent());
						nearestSide = pSide;
					}
				}

				nearest->Setup(segmentManager.Index(nearestSegment), nearestSegment->SideIndex(nearestSide));
				m_presenter.UpdateSelectableSides(selectableSides);
				if (nearest->m_nSegment >= 0)
				{
					m_presenter.UpdateNearestSelection(nearest);
				}
			}
		}
		break;

		case SelectMode::Segment:
		{
			CRect viewport;
			GetClientRect(viewport);

			auto selectableSides = GatherSelectableSides(viewport, LastMousePos().x, LastMousePos().y, ViewFlag(eViewMineSkyBox), true);
			if (!selectableSides.empty())
			{
				double minDist = 1e30;

				CSegment* nearestSegment = null;
				CSide* nearestSide = null;

				short nSegment = -1;
				for (CSide* pSide : selectableSides)
				{
					if (nSegment == pSide->GetParent())
						continue;
					CSegment* pSegment = segmentManager.Segment(nSegment = pSide->GetParent());
					CVertex& center = pSegment->Center();
					double dist = sqrt(sqr(LastMousePos().x - center.m_screen.x) + sqr(LastMousePos().y - center.m_screen.y));
					if (minDist > dist)
					{
						minDist = dist;
						nearestSegment = pSegment;
						nearestSide = pSide;
					}
				}

				nearest->Setup(segmentManager.Index(nearestSegment), nearestSegment->SideIndex(nearestSide));
				m_presenter.UpdateSelectableSides(selectableSides);
				if (nearest->m_nSegment >= 0)
				{
					m_presenter.UpdateNearestSelection(nearest);
				}
			}
		}
		break;

		default:
			break;
		}
	}
}

// -----------------------------------------------------------------------------

std::vector<CSide*> CMineView::GatherSelectableSides(CRect& viewport, long xMouse, long yMouse, bool bAllowSkyBox, bool bSegments)
{
	short nSegments = segmentManager.Count();
	std::vector<CSide*> selectableSides;
	std::vector<CSegment*> selectableSegments;

#ifdef NDEBUG
#pragma omp parallel for if (nSegments > 15)
#endif
	for (short nSegment = 0; nSegment < nSegments; nSegment++) {
		CSegment* pSegment = segmentManager.Segment(nSegment);
		if ((pSegment->Function() == SEGMENT_FUNC_SKYBOX) && !bAllowSkyBox)
			continue;
		if (pSegment->m_info.bTunnel)
			continue;
		bool bSegmentSelected = false;
		short nSide = 0;
		for (; nSide < 6; nSide++) {
			nSide = Renderer().IsSegmentSelected(*pSegment, viewport, xMouse, yMouse, nSide, bSegments);
			if (nSide < 0)
				break;
#ifdef NDEBUG
#pragma omp critical
#endif
			{
				if (!bSegmentSelected) {
					bSegmentSelected = true;
					selectableSegments.push_back(pSegment);
				}
			}
			CSide* pSide = pSegment->Side(nSide);
#ifdef NDEBUG
#pragma omp critical
#endif
			{
				selectableSides.push_back(pSide);
			}
			pSide->SetParent(nSegment);
		}
	}
	return selectableSides;
}

//--------------------------------------------------------------------------

void CMineView::UpdateStatusText()
{
	short currSide, currPoint;

	CHECKMINE;

	*message = '\0';
	#if 0
	if (preferences & PREFS_SHOW_POINT_COORDINATES)
	{
		strcat_s(message, sizeof(message), "  point (x, y,z): (");
		short vertex = segmentManager.Segment(0)[current->SegmentId()].m_info.vertexIds[sideVertexTable[current->SideId()][current->Point()]];
		char	szCoord[20];
		sprintf_s(szCoord, sizeof(szCoord), "%1.4f,%1.4f,%1.4f)",
			vertexManager.Vertex(vertex)->v.x, vertexManager.Vertex(vertex)->v.y, vertexManager.Vertex(vertex)->v.z);
		strcat_s(message, sizeof(message), szCoord);
	}
	else
	#endif
	{
		// calculate segment size (length between center point of opposing sides)
		strcat_s(message, sizeof(message), "  segment size: ");
		CDoubleVector center1, center2;
		double length;
		center1 = segmentManager.CalcSideCenter(CSideKey(current->SegmentId(), 0));
		center2 = segmentManager.CalcSideCenter(CSideKey(current->SegmentId(), 2));
		length = Distance(center1, center2);
		sprintf_s(message + strlen(message), sizeof(message) - strlen(message), "%.1f", (double)length);
		strcat_s(message, sizeof(message), " x ");
		center1 = segmentManager.CalcSideCenter(CSideKey(current->SegmentId(), 1));
		center2 = segmentManager.CalcSideCenter(CSideKey(current->SegmentId(), 3));
		length = Distance(center1, center2);
		sprintf_s(message + strlen(message), sizeof(message) - strlen(message), "%.1f", (double)length);
		strcat_s(message, sizeof(message), " x ");
		center1 = segmentManager.CalcSideCenter(CSideKey(current->SegmentId(), 4));
		center2 = segmentManager.CalcSideCenter(CSideKey(current->SegmentId(), 5));
		length = Distance(center1, center2);
		sprintf_s(message + strlen(message), sizeof(message) - strlen(message), "%.1f", (double)length);
	}
	strcat_s(message, sizeof(message), ",  segment:");
	_itoa_s(current->SegmentId(), message + strlen(message), sizeof(message) - strlen(message), 10);
	strcat_s(message, sizeof(message), " side:");
	_itoa_s((currSide = current->SideId()) + 1, message + strlen(message), sizeof(message) - strlen(message), 10);
	strcat_s(message, sizeof(message), " point:");
	_itoa_s(currPoint = current->Point(), message + strlen(message), sizeof(message) - strlen(message), 10);
	strcat_s(message, sizeof(message), " vertex:");
	_itoa_s(current->Segment()->m_info.vertexIds[current->Side()->VertexIdIndex(currPoint)], message + strlen(message), sizeof(message) - strlen(message), 10);

	strcat_s(message, sizeof(message), ",  textures:");
	strcat_s(message, sizeof(message), " 1st:");
	_itoa_s(current->Side()->BaseTex(), message + strlen(message), sizeof(message) - strlen(message), 10);
	strcat_s(message, sizeof(message), " 2nd:");
	_itoa_s(current->Side()->OvlTex(0), message + strlen(message), sizeof(message) - strlen(message), 10);

	strcat_s(message, sizeof(message), ",  zoom:");
	double zoom = log(10 * Scale().v.x) / log(zoomScales[GetRenderer()]);
	if (zoom > 0)
	zoom += 0.5;
	else
	zoom -= 0.5;
	sprintf_s(message + strlen(message), sizeof(message) - strlen(message), "%1.2f", zoom);
	STATUSMSG(message);
}

//------------------------------------------------------------------------------


//eof