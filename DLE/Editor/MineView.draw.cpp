// MineView.Draw.cpp: Mine rendering
//

#include "stdafx.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CMineView::UpdateNearestSelection()
{
	nearest->SetSegmentId(-1);
	nearest->SetSideId(-1);
	nearest->SetEdge(-1);
	m_presenter.UpdateNearestSelection(nullptr);

	if ((m_inputHandler.MouseState() == eMouseStateSelect && m_inputHandler.HasMouseMovedInCurrentState()) ||
		m_inputHandler.MouseState() == eMouseStateApplySelect)
	{
		nearest->Setup();

		switch (m_selectMode)
		{
		case SelectMode::Point:
			m_presenter.FindNearestVertex(LastMousePos().x, LastMousePos().y, nearest);
			m_presenter.UpdateNearestSelection(nearest);
			break;
		case SelectMode::Line:
			m_presenter.FindNearestLine(LastMousePos().x, LastMousePos().y, nearest);
			m_presenter.UpdateNearestSelection(nearest);
			break;
		case SelectMode::Side:
			m_presenter.FindNearestSide(LastMousePos().x, LastMousePos().y, nearest);
			if (nearest->SegmentId() >= 0)
			{
				m_presenter.UpdateNearestSelection(nearest);
			}
			break;
		case SelectMode::Segment:
			m_presenter.FindNearestSegment(LastMousePos().x, LastMousePos().y, nearest);
			if (nearest->SegmentId() >= 0)
			{
				m_presenter.UpdateNearestSelection(nearest);
			}
			break;
		default:
			break;
		}
	}
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