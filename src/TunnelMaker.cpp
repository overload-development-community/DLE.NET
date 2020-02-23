

#include "mine.h"
#include "dle-xp.h"
#include "Quaternion.h"

//------------------------------------------------------------------------------
/*
The tunnel maker creates a structure of connected segments leading from an arbitrary number of start 
sides to an end point. The start point is the center of the 'current' side. The end point is the 
center of the 'other' side.

The start sides consist of all tagged sides that are directly or indirectly connected to the 
current side (via edges or other connected, tagged sides) and are at an angle of 22.5° or less to the 
current side.

The tunnel path is determined by a cubic bezier curve from the start to the end point of the tunnel and 
the normals of the current and other side. Its granularity (number of segments) and curvature can be 
changed interactively by pressing ALT+8 / ALT+9 (granularity) and CTRL+8 / CTRL+9 (curvature via length 
of start and end normals).

The tunnel vertices are computed by gathering all start vertices and rotating them to the proper 
orientation for each tunnel path point. 

The rotation matrixes are built from the tangent vector of each path point (average of the two path 
vectors starting and ending at that point) and a rotation angle around the rotation matrix' forward vector. 
The rotation angle is computed from the total z rotation angle between the start and the end points 
orientation, and is scaled by the factor of the path length at the current node to the total path length.

Start and end orientations are created from the current and other sides' normals (forward) and current 
edges (right; right = <side vertex @ side's current point + 1> - <side vertex @ side's current point>).
The end sides orientation is properly flipped to point in the direction of the tunnel path.

The total procedure is:

- gather all start sides and vertices
- construct start and end orientations
- compute overall z rotation angle
- compute tunnel path
- for each path node:
     - construct rotation matrix from start rotation, forward vector <next node - previous node> and weighted z rotation
     - compute tunnel vertices at current path node by rotating the start vertices using the node's rotation matrix
	  - assign current path node's tunnel vertices to the tunnel segments related to the current path node

A few implementation details.

First a path of reference points ("nodes") between the start and end point is created by use of a cubic bezier function. 
Then a set of base vertices is computed by taking the vertices of all start sides and subtracting the tunnel start point 
from them (so now they are relative to the start point). 

Then the orientation (rotation) matrices for each path node is created. They are interpolated between the start and end 
node's orientations. These get computed from the start and end sides' normal (forward), the current edge (right) and 
their perpendicular vector (up). The end side's normal gets negated to point to the proper direction. 

To determine the z rotation angle between the start and end orientations, the end orientation's x and y rotations are undone 
in relation to the start orientation by computing the angles between the two orientation matrices' z (forward) axes and 
rotating the end side's orientation matrix around the perpendicular vector of the start and end orientations' z axis. Now the 
start and end sides' forward vectors are identical (minus slight aberrations caused by limitiations in double floating point 
arithmetic), and the their z axis rotation ("twist") can be computed from the angle between their right axes. 

The only problem remaining is to determine whether the rotation is clockwise or counter clockwise. To determine that, the start 
matrix right vector is rotated and twisted back into the end position (by reversing the previous unrotation and applying the twist 
angle rotation). If the result of this transformation has a dot product > 0.999 with the end orientation's right vector, I am done. 
Otherwise difference angle between the two vectors is added to the twist angle.

Each path node orientation has two base parameters: The direction of its forward vector (here the vector of the previous to 
the next path node is used to smoothly average the angle of the perpendicular plane in regard to the forward vector) and the 
twist (angle around forward vector) relative to the start orientation. To interpolate a path node orientation, it's twist is 
computed by scaling the total twist with the quotient of the path length at that node and the total path length 
(twist * length / total_length). The required x and y rotations are done via a quaternion based rotation around a rotation 
axis which is perpendicular to the current path node's and the base side orientation's forward vectors and the angle between 
these two vectors. The base orientation's right and up vectors are rotated around this axis (the forward vector doesn't need 
to, since we have it already). Next, the local twist is applied by rotating the local up and right vectors around the forward 
vector by the current twist angle.

This yields a rotation matrix in the current coordinate system (based on the identity matrix) for each path node. To compute 
the cross section at each path node, the start vertices simply need to be rotated using these rotation matrixes (their inverse, 
to be precise). What remains to be done is to enter the proper vertex indices in each tunnel segment (which was quite straight 
forward with the data structures I have devised).

*/

CTunnelMaker tunnelMaker;

#ifdef _DEBUG

static bool bTwist = true;
static bool bUnTwist = true;

#endif

#define ROTAXIS_METHOD	0
#define UNTWIST			0

//------------------------------------------------------------------------------

#define CURRENT_POINT(a) ((current->Point () + (a))&0x03)

//------------------------------------------------------------------------------

char szTunnelMakerError [] = "You must exit tunnel creation before performing this function";

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

long CCubicBezier::Faculty (int n) 
{
long i = 1;

for (int j = n; j >= 2; j--) 
	i *= j;
return i;
}

//------------------------------------------------------------------------------
//   Coeff(n,i) - returns n!/(i!*(n-i)!)
//------------------------------------------------------------------------------

double CCubicBezier::Coeff (int n, int i) 
{
return ((double) Faculty (n) / ((double) Faculty (i) * (double) Faculty (n-i)));
}

//------------------------------------------------------------------------------
//   Blend(i,n,u) - returns a weighted coefficient for each point in a nSegment
//------------------------------------------------------------------------------

double CCubicBezier::Blend (int i, int n, double u) 
{
double partial = Coeff (n, i) * pow (u, i) * pow (1 - u, n - i);
return partial;
}

//------------------------------------------------------------------------------
//   BezierFcn(pt,u,n,p [][]) - sets (x,y,z) for u=#/segs based on m_bezierPoints p
//------------------------------------------------------------------------------

CDoubleVector CCubicBezier::Compute (double u) 
{
	CDoubleVector v;

for (int i = 0; i < 4; i++) {
	double b = Blend (i, 3, u);
	v += m_points [i] * b;
	}
return v;
}

//------------------------------------------------------------------------------

void CCubicBezier::Transform (CViewMatrix* viewMatrix)
{
for (int i = 0; i < 4; i++)
	m_points [i].Transform (viewMatrix);
}

//------------------------------------------------------------------------------

void CCubicBezier::Project (CViewMatrix* viewMatrix)
{
for (int i = 0; i < 4; i++)
	m_points [i].Project (viewMatrix);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CTunnelBase::Setup (CSelection* selection, double sign, bool bStart)
{
m_bStart = bStart;
if (selection) {
	m_nSelection = selection - &selections [0];
	*((CSelection*) this) = *selection;
	}
m_sign = sign;
CSegment* pSegment = segmentManager.Segment (m_nSegment);
CSide* pSide = pSegment->Side (m_nSide);
pSegment->ComputeNormals (m_nSide);
m_normal = pSide->Normal () * sign;
pSegment->CreateOppVertexIndex (m_nSide, m_oppVertexIndex);
m_point = pSegment->ComputeCenter (m_nSide);
for (int i = 0; i < 4; i++)
	m_vertices [i] = *Segment ()->Vertex (m_nSide, i);
m_rotation.F () = m_normal;
m_rotation.R () = m_vertices [(m_nPoint + 1) % pSide->VertexCount ()] - m_vertices [m_nPoint];
m_rotation.R ().Normalize ();
#if !ROTAXIS_METHOD
// As far as I can tell we do have to do this to allow users to pick matching lines at each end
// (in some cases there can be no good "opposite" line)
m_rotation.R () *= -sign;
#endif
m_rotation.U () = CrossProduct (m_rotation.F (), m_rotation.R ());
m_rotation.U ().Normalize ();
m_updateStatus = NoUpdate;
}

//------------------------------------------------------------------------------
// Determine whether the tunnel needs to be updated
// For the start side:
//   If the start sides are tagged, only update when the current edge or a vertex of the start side have changed
//   Otherwise, update when the vertex, edge, segment and/or side have changed (to preserve legacy behavior)
// For the end side, also update when the segment and/or side have changed

CTunnelBase::eUpdateStatus CTunnelBase::IsUpdateNeeded (CSelection* selection, bool bStartSidesTagged)
{
	bool bNewSide = CSideKey (*this) != CSideKey (*selection);

if (!(m_bStart && bStartSidesTagged) && bNewSide) {
	*((CSelection*) this) = *((CSelection*) selection);
	return m_updateStatus = UpdateSide;
	}
if (!bNewSide && (Edge () != selection->Edge ())) {
	m_nEdge = selection->Edge ();
	m_nPoint = selection->Point ();
	return m_updateStatus = UpdateOrientation;
	}
CSegment* pSegment = segmentManager.Segment (m_nSegment);
for (int i = 0; i < 4; i++)
	if (m_vertices [i] != *pSegment->Vertex (m_nSide, i))
		return m_updateStatus = UpdateOrientation;
return m_updateStatus = NoUpdate;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#if UNTWIST

void CTunnelElement::Untwist (short nSide) 
{
#ifdef _DEBUG
if (!bUnTwist)
	return;
#endif

  double				minDist = 1e10;
  short				nOppSide = oppSideTable [nSide];
  ubyte				oppVertexIndex [4];
  CDoubleVector	edges [4];

CSegment* pSegment = segmentManager.Segment (m_nSegment);
pSegment->CreateOppVertexIndex (nSide, oppVertexIndex);

CVertex* v0 = pSegment->Vertex (nSide, 0);
short nOffset = 0;
for (short j = 0; j < 4; j++) {
	edges [j] = *pSegment->Vertex (ushort (oppVertexIndex [j])) - *v0;
	double d = edges [j].Mag ();
	edges [j] /= d;
	if (d < minDist) {
		minDist = d;
		nOffset = j;
		}
	}
#if 0
if (nOffset == 0) {
	pSegment->ComputeNormals (nSide);
	CDoubleVector& n = pSegment->Side (nSide)->Normal ();
	for (short j = 0; j < 4; j++) {
		double d = fabs (Dot (edges [j], n));
		if (d < minDist) {
			minDist = d;
			nOffset = j;
			}
		}
	}
#endif
if (nOffset != 0) {
	short vertexIds [4];
	for (short j = 0; j < 4; j++)
		vertexIds [j] = pSegment->VertexId (nSide, j);
	for (short j = 0; j < 4; j++)
		pSegment->SetVertexId (nSide, nOffset + j, vertexIds [j]);
	}
}

#endif

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CTunnelSegment::AssignVertices (CTunnelPath& path)
{
short nSegments = short (m_elements.Length ());
for (short i = 0; i < nSegments; i++) {
	CTunnelElement& element = m_elements [i];
	short* vertexIndex = path.m_startSides [i].m_nVertexIndex;
	for (short j = 0; j < 4; j++) 
		element.m_nVertices [j] = m_nVertices [vertexIndex [j]];
	}
}

//------------------------------------------------------------------------------
// create a tunnel segment (tunnel 'slice') and correlate each element's vertices
// with its corresponding vertices

bool CTunnelSegment::Create (CTunnelPath& path, short nSegments, short nVertices, bool bSegment)
{
Release ();
if (!(m_elements.Resize (nSegments) && m_nVertices.Resize (nVertices)))
	return false;
if (!vertexManager.Add (&m_nVertices [0], nVertices))
	return false;
for (short i = 0; i < nSegments; i++) {
	CTunnelElement& element = m_elements [i];
	if (!bSegment)
		element.m_nSegment = SEGMENT_LIMIT;
	else {
		if (0 > (element.m_nSegment = segmentManager.Add ()))
			return false;
		segmentManager.Segment (element.m_nSegment)->m_info.bTunnel = 1;
		}
	}
AssignVertices (path);
return true;
}

//------------------------------------------------------------------------------

void CTunnelSegment::Release (void)
{
if (!(m_nVertices.Buffer () && (m_elements.Buffer ())))
	return;
for (int i = (int) m_elements.Length (); --i >= 0; ) {
	segmentManager.Remove (m_elements [i].m_nSegment);
	m_elements [i].m_nSegment = SEGMENT_LIMIT + 1;
	}
for (int i = (int) m_nVertices.Length (); --i >= 0; ) {
	vertexManager.Delete (m_nVertices [i]);
	m_nVertices [i] = MAX_VERTEX + 1;
	}
}

//------------------------------------------------------------------------------

void CTunnelSegment::Draw (void)
{
#if 1
CMineView* mineView = DLE.MineView ();
mineView->Renderer ().BeginRender (false);
#ifdef NDEBUG
if (mineView->GetRenderer () && (mineView->ViewOption (eViewTexturedWireFrame) || mineView->ViewOption (eViewTextured))) 
#endif
	{
	glLineStipple (1, 0x0c3f);  // dot dash
	glEnable (GL_LINE_STIPPLE);
	}
for (int i = (int) m_elements.Length (); --i >= 0; ) 
	mineView->DrawSegmentWireFrame (segmentManager.Segment (m_elements [i].m_nSegment), false, false, 1);
mineView->Renderer ().EndRender ();
#ifdef NDEBUG
if (mineView->GetRenderer () && (mineView->ViewOption (eViewTexturedWireFrame) || mineView->ViewOption (eViewTextured))) 
#endif
	glDisable (GL_LINE_STIPPLE);
#endif
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CTunnel::Setup (CTunnelBase base [2])
{
Release ();
Destroy ();
memcpy (m_base, base, sizeof (m_base));
}

//------------------------------------------------------------------------------

void CTunnel::Release (void)
{
if (m_segments.Buffer ())
	for (int i = m_nSteps; i >= 0; i--)
		m_segments [i].Release ();
}

//------------------------------------------------------------------------------

void CTunnel::Destroy (void)
{
m_nSteps = 0;
m_segments.Destroy ();
}

//------------------------------------------------------------------------------
// Walk through all tunnel segments and assign the tunnel vertices to their 
// corresponding segments. Each segment's tunnel vertex ids have been computed
// when preparing the tunnel path and have been stored in CTunnelElement::m_nVertices.

void CTunnel::AssignVertices (void)
{
for (uint nElement = 0, nElements = m_segments [0].m_elements.Length (); nElement < nElements; nElement++) {
	CTunnelElement * e0, * e1 = &m_segments [0].m_elements [nElement];
	for (short nSegment = 1; nSegment <= m_nSteps; nSegment++) {
		e0 = e1;
		e1 = &m_segments [nSegment].m_elements [nElement];
		CSegment* pSegment = segmentManager.Segment (e1->m_nSegment);
		for (short nVertex = 0; nVertex < 4; nVertex++) {
			pSegment->SetVertexId (m_base [0].m_nSide, nVertex, e1->m_nVertices [nVertex]);
			pSegment->SetVertexId (m_base [0].m_oppVertexIndex [nVertex], e0->m_nVertices [nVertex]);
			}
#if UNTWIST // A hack that is not required anymore ...
		//if (nSegment == m_nSteps)
			e1->Untwist (m_base [0].m_nSide);
#endif
		}
	}
}

//------------------------------------------------------------------------------

bool CTunnel::Create (CTunnelPath& path) 
{
	short nSegments = (short) path.m_startSides.Length ();
	short nVertices = (short) path.m_nStartVertices.Length ();

if (m_nSteps != path.Steps ()) { // allocate sufficient memory for required segments and vertices
	if (m_nSteps > 0)
		Release ();
	if ((path.Steps () > m_nSteps) && !m_segments.Resize (path.Steps () + 1, false))
		return false;
	m_nSteps = path.Steps ();
	for (int i = 0; i <= m_nSteps; i++) {
		if (!m_segments [i].Create (path, nSegments, nVertices, i > 0))
			return false;
		}
	}

// Pre-calculate morph vectors if necessary since we'll need to use them a lot
CDynamicArray <CDoubleVector> vMorph;
CDynamicArray <ushort> nVertexMap;
if (path.m_bMorph) {
	ushort nSideVertices = path.m_base [0].Side ()->VertexCount ();
	if (!vMorph.Create (nSideVertices) || !nVertexMap.Create (nVertices))
		return false;
	for (ushort nVertex = 0; nVertex < nSideVertices; nVertex++) {
		CVertex vStart = *path.m_base [0].Vertex (nVertex);
		vStart -= path.m_base [0].m_point; // un-translate (make relative to tunnel start)
		vStart = path.m_base [0].m_rotation * vStart; // un-rotate
		CVertex vEnd = *path.m_base [1].Vertex (nSideVertices + 1 - nVertex);
		vEnd -= path.m_base [1].m_point; // un-translate (make relative to tunnel end)
		vEnd = path.m_base [1].m_rotation * vEnd; // un-rotate
		vMorph [nVertex] = vEnd - vStart;

		for (ushort nStartVertex = 0; nStartVertex < nVertices; nStartVertex++)
			if (vertexManager.Index (path.m_base [0].Vertex (nVertex)) == path.m_nStartVertices [nStartVertex]) {
				nVertexMap [nStartVertex] = nVertex;
				break;
				}
		}
	}

// Compute all tunnel vertices by rotating the base vertices using each path node's orientation (== rotation matrix)
// The rotation is relative to the base coordinate system (identity matrix), but the vertices are relative to the 
// start point and start rotation, so each vertex has to be un-translated and un-rotated before rotating and translating
// it with the current path node's orientation matrix and position.
CMineView* mineView = DLE.MineView ();
CViewMatrix* viewMatrix = mineView->ViewMatrix ();
mineView->Renderer ().BeginRender (false);
for (int nSegment = 0; nSegment <= m_nSteps; nSegment++) {
	CDoubleMatrix& rotation = path [nSegment].m_rotation;
	CDoubleVector& translation = path [nSegment].m_vertex;
	for (uint nVertex = 0, l = path.m_nStartVertices.Length (); nVertex < l; nVertex++) {
		CVertex v = vertexManager [path.m_nStartVertices [nVertex]];
		v -= path.m_base [0].m_point; // un-translate (make relative to tunnel start)
		v = path.m_base [0].m_rotation * v; // un-rotate
		if (path.m_bMorph) {
			double amount = (double)nSegment / (double)m_nSteps;
			v += vMorph [nVertexMap [nVertex]] * amount;
			}
		v = rotation * v; // rotate (
		CDoubleVector a = rotation.Angles ();
		v += translation;
		v.Transform (viewMatrix);
		v.Project (viewMatrix);
#if 0 //def _DEBUG
		v.Tag ();
#else
		v.UnTag ();
#endif
		vertexManager [m_segments [nSegment].m_nVertices [nVertex]] = v;
#ifdef _DEBUG
		v = v;
#endif
		}
	}
mineView->Renderer ().EndRender ();
#if 1
Realize (path, false);
#else
AssignVertices ();
#endif
return true;
}

//------------------------------------------------------------------------------
// Connect all tunnel segments with all adjacent other tunnel segments and base segments.
// No segments will be connected with the tunnel's end segment to avoid geometry distortions.

void CTunnel::Realize (CTunnelPath& path, bool bFinalize)
{
DLE.MineView ()->DelayRefresh (true);

ushort nVertex = 0;
short nElements = (short) m_segments [0].m_elements.Length ();
for (short nSegment = 1; nSegment <= m_nSteps; nSegment++) {
	short nStartSide = m_base [0].m_nSide;

	for (short iElement = 0; iElement < nElements; iElement++) {
		short nStartSeg = path.m_startSides [iElement].m_nSegment;
		CSegment* pStartSeg = segmentManager.Segment (nStartSeg);
		CTunnelElement& e0 = m_segments [nSegment].m_elements [iElement];
		CSegment* pSegment = segmentManager.Segment (e0.m_nSegment);

		pSegment->m_info.function = pStartSeg->m_info.function;
		pSegment->m_info.props = pStartSeg->m_info.props;
		pSegment->m_info.owner = pStartSeg->m_info.owner;
		pSegment->m_info.group = pStartSeg->m_info.group;
		pSegment->m_nShape = pStartSeg->m_nShape;
		CSide* pSide = pSegment->Side (0);
		for (short nSide = 0; nSide < 6; nSide++, pSide++) {
#if 1
			pSegment->SetUV (nSide, 0.0, 0.0);
#else
			memcpy (pSide->m_info.uvls, pStartSeg->m_sides [nSide].m_info.uvls, sizeof (pSide->m_info.uvls));
			memcpy (pSide->m_info.uvlDeltas, pStartSeg->m_sides [nSide].m_info.uvlDeltas, sizeof (pSide->m_info.uvlDeltas));
#endif
			pSide->m_info.nBaseTex = pStartSeg->m_sides [nSide].m_info.nBaseTex;
			pSide->m_info.nOvlTex = pStartSeg->m_sides [nSide].m_info.nOvlTex;
			pSide->m_nShape = pStartSeg->m_sides [nSide].m_nShape;
			}

		if (bFinalize)
			pSegment->Tag ();
		for (int j = 0; j < 6; j++)
			pSegment->SetChild (j, -1);
		if (bFinalize)
			pSegment->m_info.bTunnel = 0;
		if (nSegment > 1) 
			pSegment->SetChild (oppSideTable [nStartSide], m_segments [nSegment - 1].m_elements [iElement].m_nSegment); // previous tunnel segment
		else if (bFinalize) {
			pStartSeg->SetChild (nStartSide, e0.m_nSegment);
			pSegment->SetChild (oppSideTable [nStartSide], nStartSeg);
			} 
		if (nSegment < m_nSteps)
			pSegment->SetChild (nStartSide, m_segments [nSegment + 1].m_elements [iElement].m_nSegment); // next tunnel segment
		}
	}

// the tunnel start segments' back sides have separate vertices from the tunnel start
// here these sides get the tunnel start vertices assigned
ushort* buffer = null;
if (bFinalize) {
	buffer = m_segments [0].m_nVertices.Buffer ();
	m_segments [0].m_nVertices.SetBuffer (path.m_nStartVertices.Buffer ());
	path.m_nStartVertices.SetBuffer (buffer);
	m_segments [0].AssignVertices (path);
	}

AssignVertices ();

for (short nSegment = 1; nSegment <= m_nSteps; nSegment++) {
	for (short iElement = 0; iElement < nElements; iElement++) {
		CTunnelElement& e0 = m_segments [nSegment].m_elements [iElement];
		CSegment* pSegment = segmentManager.Segment (e0.m_nSegment);

#if 0 //ndef _DEBUG
		// align the textures nicely
		CSide* pSide = pSegment->Side (0);
		for (short nSide = 0; nSide < 6; nSide++, pSide++) { 
			if (pSide->IsVisible ())
				segmentManager.AlignTextures (e0.m_nSegment, nSide, 1, 0, 1, 1, 0);
			}
#endif
		for (short jElement = 0; jElement < nElements; jElement++) {
			if (jElement == iElement)
				continue;
			short nChildSeg = m_segments [nSegment].m_elements [jElement].m_nSegment;
			short nChildSide, nSide = pSegment->CommonSides (nChildSeg, nChildSide);
			if (nSide < 0)
				continue;
			pSegment->SetChild (nSide, nChildSeg);
			segmentManager.Segment (nChildSeg)->SetChild (nChildSide, e0.m_nSegment);
			}
		}
	}

if (bFinalize)
	vertexManager.Delete (buffer, nElements);

DLE.MineView ()->DelayRefresh (false);
}

//------------------------------------------------------------------------------

void CTunnel::Draw (CRenderer& renderer, CPen* redPen, CPen* bluePen, CViewMatrix* viewMatrix) 
{
CDC* pDC = renderer.DC ();

renderer.BeginRender ();
for (int i = 0; i <= m_nSteps; i++) {
	for (int j = 0; j < 4; j++) {
		CVertex&v = vertexManager [m_segments [i].m_nVertices [j]];
		v.Transform (viewMatrix);
		v.Project (viewMatrix);
		}
	}
renderer.EndRender ();

renderer.BeginRender (true);
renderer.SelectObject ((HBRUSH)GetStockObject (NULL_BRUSH));
renderer.SelectPen (penBlue + 1);
CMineView* mineView = DLE.MineView ();
for (int i = 1; i <= m_nSteps; i++)
	m_segments [i].Draw ();
renderer.EndRender ();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CTunnelPathNode::Draw (CRenderer& renderer, CViewMatrix* viewMatrix) 
{
CDC* pDC = renderer.DC ();

CDoubleMatrix m;
m = m_rotation.Inverse ();
CVertex v [4] = { m.R (), m.U (), m.F (), m_axis };

renderer.BeginRender ();
m_vertex.Transform (viewMatrix);
m_vertex.Project (viewMatrix);
for (int i = 0; i < 4; i++) {
	v [i].Normalize ();
	v [i] *= 5.0;
	v [i] += m_vertex;
	v [i].Transform (viewMatrix);
	v [i].Project (viewMatrix);
	}
renderer.EndRender ();

renderer.BeginRender (true);
renderer.SelectObject ((HBRUSH)GetStockObject (NULL_BRUSH));
static ePenColor pens [4] = { penRed, penMedGreen, penMedBlue, penOrange };

renderer.Ellipse (m_vertex, 4, 4);
for (int i = 0; i < 4; i++) {
	renderer.SelectPen (pens [i] + 1);
	renderer.MoveTo (m_vertex.m_screen.x, m_vertex.m_screen.y);
	renderer.LineTo (v [i].m_screen.x, v [i].m_screen.y);
	}
renderer.EndRender ();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

// copy the collected sides to an array
// gather all vertices of the start sides
// create indices into the start vertex array for every start side's corner

bool CTunnelPath::GatherStartSides (void)
{
bool bTagged = current->Side ()->IsTagged ();
if (!bTagged)
	current->Side ()->Tag ();

#if 0 // only use connected tagged sides

// collect all tagged sides that don't have child segments, are directly or indirectly 
// connected to the start side and are at an angle of <= 22.5° to the start side
CTagTunnelStart tagger;
if (!tagger.Setup (segmentManager.TaggedSideCount (), TUNNEL_MASK))
	return false;
int nSides = tagger.Run ();

#else // use all tagged sides

CSegment* pSegment = segmentManager.Segment (0);
short nSegments = segmentManager.Count ();
current->Segment ()->ComputeNormals (current->SideId ());
CDoubleVector reference = current->Side ()->Normal ();
double maxAngle = cos (Radians (22.5));

CSLL<CSideKey, CSideKey> startSides;

for (short nSegment = 0; nSegment < nSegments; nSegment++, pSegment++) {
	pSegment->ComputeNormals (-1);
	CSide* pSide = pSegment->Side (0);
	for (short nSide = 0; nSide < 6; nSide++, pSide++) {
		if (!pSide->IsTagged ())
			continue;
		if (pSide->Shape ()) // only accept quads
			continue;
		if (Dot (pSide->Normal (), reference) < maxAngle)
			continue;
		if (!startSides.Append (CSideKey (nSegment, nSide)))
			return false;
		}
	}	

int nSides = startSides.Length ();

#endif

if ((nSides == 0) || (m_startSides.Create (nSides) == null))
	return false;

if (!bTagged)
	current->Side ()->UnTag ();

CSLL<ushort,ushort>	startVertices;

#if 0

for (int nStartSide = 0; nStartSide < nSides; nStartSide++) {
	m_startSides [nStartSide] = tagger.m_sideList [nStartSide].m_child;
	short nSide = m_startSides [nStartSide].m_nSide;
	CSegment* pSegment = segmentManager.Segment (m_startSides [nStartSide]);
	for (int nVertex = 0, nVertexCount = pSegment->Side (nSide)->VertexCount (); nVertex < nVertexCount; nVertex++) {
		ushort nId = pSegment->VertexId (nSide, nVertex);
		int nIndex = startVertices.Index (nId);
		if (nIndex < 0) {
			if (!startVertices.Append (nId))
				return false; // out of memory
			nIndex = startVertices.Length () - 1;
			}
		m_startSides [nStartSide].m_nVertexIndex [nVertex] = nIndex;
		}
	}

#else


CSLLIterator<CSideKey, CSideKey> sideIter (startSides);

int nStartSide = 0;

for (sideIter.Begin (); *sideIter != sideIter.End (); sideIter++, nStartSide++) {
	m_startSides [nStartSide] = **sideIter;
	short nSide = m_startSides [nStartSide].m_nSide;
	CSegment* pSegment = segmentManager.Segment (m_startSides [nStartSide]);
	for (int nVertex = 0, nVertexCount = pSegment->Side (nSide)->VertexCount (); nVertex < nVertexCount; nVertex++) {
		ushort nId = pSegment->VertexId (nSide, nVertex);
		int nIndex = startVertices.Index (nId);
		if (nIndex < 0) {
			if (!startVertices.Append (nId))
				return false; // out of memory
			nIndex = startVertices.Length () - 1;
			}
		m_startSides [nStartSide].m_nVertexIndex [nVertex] = nIndex;
		}
	}

#endif

if (!(m_nStartVertices.Create (startVertices.Length ())))
	return false;

// copy the start vertices to an array
CSLLIterator<ushort, ushort> vertexIter (startVertices);
ushort j = 0;
for (vertexIter.Begin (); *vertexIter != vertexIter.End (); vertexIter++)
	m_nStartVertices [j++] = **vertexIter;

return true;
}

//------------------------------------------------------------------------------

bool CTunnelPath::Setup (CTunnelBase base [2], bool bStartSides, bool bPath)
{
memcpy (m_base, base, sizeof (m_base));

// calculate the initial length of each end of the bezier curve
double length = Distance (m_base [0].m_point, m_base [1].m_point);
length *= 0.5;
if (length < MIN_TUNNEL_LENGTH)
	length = MIN_TUNNEL_LENGTH;
else if (length > MAX_TUNNEL_LENGTH)
	length = MAX_TUNNEL_LENGTH;

if (bStartSides) {
	if (!GatherStartSides ())
		return false;
	m_bMorph = (m_startSides.Length () == 1) && !current->Side ()->IsTagged () &&
		(m_base [0].Side ()->Shape () == m_base [1].Side ()->Shape ());
	}

if (bStartSides || bPath) {
	// setup intermediate points for a cubic bezier curve
	m_bezier.SetLength (length, 0);
	m_bezier.SetLength (length, 1);
	m_bezier.SetPoint (m_base [0].GetPoint (), 0);
	m_bezier.SetPoint (m_base [0].GetPoint () + m_base [0].GetNormal () * m_bezier.GetLength (0), 1);
	m_bezier.SetPoint (m_base [1].GetPoint () + m_base [1].GetNormal () * m_bezier.GetLength (1), 2);
	m_bezier.SetPoint (m_base [1].GetPoint (), 3);
	}
return true;
}

//------------------------------------------------------------------------------

void CTunnelPath::Destroy (void)
{
m_startSides.Destroy ();
m_nStartVertices.Destroy ();
}

//------------------------------------------------------------------------------

bool CTunnelPath::BendAxis (CTunnelPathNode * n0, CTunnelPathNode * n1)
{
n1->m_sign = 1.0;
double dot = Dot (n1->m_rotation.F (), n0->m_rotation.F ());
if (dot > 0.999999) 
	n1->m_axis = n0->m_rotation.R ();
else {
	n1->m_axis = CrossProduct (n1->m_rotation.F (), -n0->m_rotation.F ());
	n1->m_axis.Normalize ();
	}
#ifdef _DEBUG
if (bTwist)
#endif
if (Dot (n1->m_axis, n0->m_axis) < 0.0) {
	n1->m_axis.Negate ();
	n1->m_sign = -1.0;
	}
return (dot <= 0.999999);
}

//------------------------------------------------------------------------------

void CTunnelPath::Bend (CTunnelPathNode * n0, CTunnelPathNode * n1)
{
#if ROTAXIS_METHOD

if (!BendAxis (n0, n1)) {
	n1->m_rotation.R () = n1->m_axis;
	n1->m_rotation.U () = n0->m_rotation.U ();
	}
else {
	n1->m_rotation.R () = n1->m_axis;
	n1->m_rotation.U () = CrossProduct (n1->m_rotation.R (), n1->m_rotation.F ());
	if (n1->m_rotation.Handedness () != m_base [0].m_rotation.Handedness ())
		n1->m_rotation.U ().Negate ();
	}

#else

// rotate the previous matrix around the perpendicular of the previous and the current forward vector
// to orient it properly for the current path node
double dot = Dot (n1->m_rotation.F (), n0->m_rotation.F ()); // angle of current and previous forward vectors
if (dot >= 0.999999) { // dot >= 1e-6 ~ parallel
	n1->m_rotation.R () = n0->m_rotation.R (); // rotate right and up vectors accordingly
	n1->m_rotation.U () = n0->m_rotation.U ();
	}
else if (dot <= -0.999999) { // dot >= 1e-6 ~ parallel
	n1->m_rotation.R () = -n0->m_rotation.R (); // rotate right and up vectors accordingly
	n1->m_rotation.U () = n0->m_rotation.U ();
	}
else {
	CQuaternion q;
#ifdef _DEBUG
	CDoubleVector v0 (n1->m_rotation.F ());
	CDoubleVector v1 (/*(dot < 0.0) ? n0->m_rotation.F () :*/ -n0->m_rotation.F ());
	double a = acos (dot);
	CDoubleVector axis = CrossProduct (v0, v1);
	axis.Normalize ();
	CDoubleVector vi;
	axis = Perpendicular (vi, v0, v1);
	axis.Normalize ();
#endif
	double bendAngle = acos (dot);
	n1->m_axis = CrossProduct (n1->m_rotation.F (), /*(dot < 0.0) ? n0->m_rotation.F () :*/ -n0->m_rotation.F ());
	q.FromAxisAngle (n1->m_axis.Normalize (), bendAngle);
	CDoubleVector fVec;
	fVec = q * n0->m_rotation.F ();
	dot = Dot (fVec, n1->m_rotation.F ());
	if (dot < 0.999)
		bendAngle += acos (dot);
	q.FromAxisAngle (n1->m_axis, bendAngle);
	n1->m_rotation.R () = q * n0->m_rotation.R (); // rotate right and up vectors accordingly
	n1->m_rotation.U () = q * n0->m_rotation.U ();
	n1->m_rotation.R ().Normalize ();
	n1->m_rotation.U ().Normalize ();
	}

#endif
}

//------------------------------------------------------------------------------

void CTunnelPath::Twist (CTunnelPathNode * n0, CTunnelPathNode * n1, double scale)
{
#ifdef _DEBUG
if (!bTwist)
	return;
#endif

#if ROTAXIS_METHOD

if (m_nPivot >= 0) {
	int nNode = n1 - &m_nodes [0];
	double angle = 0.0;
#if 0
	double scale = double (nNode) / double (m_nSteps);
	angle += m_corrAngles [0] * (1.0 - scale);
	angle += m_corrAngles [1] * scale;
#else
	if (nNode < m_nPivot)
		angle = -m_corrAngles [0] * double (m_nPivot - nNode) / double (m_nPivot);
	else if (nNode > m_nPivot)
		angle = m_corrAngles [1] * double (nNode - m_nPivot) / double (m_nSteps - m_nPivot);
	else
		angle = 0.0;
#endif
	if (angle != 0.0) {
#if 0
		if (n1->m_sign < 0.0)
			angle -= Sign (angle) * 2.0 * PI;
#endif
		CQuaternion q;
		q.FromAxisAngle (n1->m_rotation.m.fVec, angle);
		n1->m_rotation.m.rVec = q * n1->m_rotation.m.rVec;
		n1->m_rotation.m.uVec = q * n1->m_rotation.m.uVec;
		n1->m_rotation.m.rVec.Normalize ();
		n1->m_rotation.m.uVec.Normalize ();
		}
	}

#else

// twist the current matrix around the forward vector 
n1->m_angle = m_deltaAngle * scale;
if (fabs (n1->m_angle - n0->m_angle) > 1e-6) {
	CQuaternion q;
	q.FromAxisAngle (n1->m_rotation.F (), n1->m_angle - n0->m_angle);
	n1->m_rotation.R () = q * n1->m_rotation.R ();
	n1->m_rotation.U () = q * n1->m_rotation.U ();
	n1->m_rotation.R ().Normalize ();
	n1->m_rotation.U ().Normalize ();
	}

#endif

}

//------------------------------------------------------------------------------

double CTunnelPath::CorrAngle (CDoubleMatrix& rotation, CTunnelPathNode* n0, CTunnelPathNode* n1)
{
if (!BendAxis (n0, n1))
	return 0.0;
double corrAngle = acos (Dot (n1->m_axis, n0->m_axis/*rotation.m.rVec*/));
#if 0
double dot = Dot (rotation.m.uVec, n1->m_axis);
if (nNode ? (dot > 0.0) : (dot > 0.0))
	corrAngle = -corrAngle;
#endif
return corrAngle;
}

//------------------------------------------------------------------------------

double CTunnelPath::TotalTwist (void)
{
CQuaternion q;

#if ROTAXIS_METHOD

// revert the end orientation's z rotation in regard to the start orientation by 
// determining the angle of the two matrices' z axii (forward vectors) and rotating
// the end matrix around the perpendicular of the two matrices' z axii.
m_corrAngles [0] = acos (Dot (m_nodes [1].m_rotation.R (), m_base [0].m_rotation.R ()));
if (Dot (m_nodes [1].m_rotation.R (), m_base [0].m_rotation.U ()) < 0.0)
	m_corrAngles [0] = -m_corrAngles [0];
m_corrAngles [1] = acos (Dot (m_base [1].m_rotation.R (), m_nodes [m_nSteps].m_rotation.R ()));
if (Dot (m_base [1].m_rotation.R (), m_nodes [m_nSteps].m_rotation.U ()) < 0.0)
	m_corrAngles [1] = -m_corrAngles [1];
double corrAngle = fabs (m_corrAngles [0]) + fabs (m_corrAngles [1]);
if (corrAngle < 0.001) 
	m_nPivot = -1;
else {
	// find the "zero" point between the z rotations of the two ends of the corridor by taking
	// the ratio of one end to the total rotation, and multiplying by the corridor length.
	double pivotDist = Length (m_nSteps) * fabs (m_corrAngles [0]) / corrAngle;
	double minDistFromPivot = 1e30;
	for (int i = 0; i <= m_nSteps; i++) {
		double l = Length (i);
		double distFromPivot = fabs (pivotDist - l);
		if (distFromPivot < minDistFromPivot) {
			minDistFromPivot = distFromPivot;
			m_nPivot = i;
			}
		}
	}
	
return (m_base [1].Point () - m_base [0].Point ()) * PI * 0.5;

#else

// revert the end orientation's z rotation in regard to the start orientation by 
// determining the angle of the two matrices' z axii (forward vectors) and rotating
// the end matrix around the perpendicular of the two matrices' z axii.

CDoubleMatrix m = m_base [1].m_rotation;
CDoubleVector rotAxis;
double bendAngle = acos (Clamp (Dot (m.F (), m_base [0].m_rotation.F ()), -1.0, 1.0));
if (bendAngle > M_PI - 1e-6) {
	// Angle is close to 180 degrees, which means the rotation axis could be anything
	// perpendicular to the forward vector. We'll pick an axis also perpendicular to the
	// displacement between the two ends of the corridor.
	CDoubleVector displacement = m_base [1].GetPoint () - m_base [0].GetPoint ();
	if (displacement.Mag () > 1e-3) {
		rotAxis = CrossProduct (displacement, -m_base [0].m_rotation.F ());
	} else {
		// No or small displacement - the tunnel maker probably shouldn't be started
		// but just in case - we could pick anything so we'll just pick the start
		// end's up vector
		rotAxis = m_base [0].m_rotation.U ();
		}
} else if (bendAngle > 1e-6) {
	rotAxis = CrossProduct (m.F (), -m_base [0].m_rotation.F ());
	}

if (bendAngle > 1e-6) { // dot >= 0.999999 ~ parallel
	// Construct quaternion from the axis and angle, and "undo" the end orientation's
	// bend so it is parallel with the start face. We only need the R vector to
	// determine rotation.
	q.FromAxisAngle (rotAxis, -bendAngle);
	m.R () = q * m.R ();
	m.R ().Normalize ();
	}

// Calculate rotation using atan2 (so we can get the direction at the same time).
// y = projection of transformed R vector on start U vector
// x = projection of transformed R vector on start R vector
double twistAngle = atan2 (Dot (m.R (), m_base [0].m_rotation.U ()),
	Dot (m.R (), m_base [0].m_rotation.R ()));
return twistAngle;

#endif

}

//------------------------------------------------------------------------------

bool CTunnelPath::Create (short nSteps)
{
if (m_nSteps != nSteps) { // recompute
	if ((nSteps > m_nSteps) && !m_nodes.Resize (nSteps + 1, false))
		return false;
	m_nSteps = nSteps;
	}

// calculate nSegment m_bezierPoints
for (int i = 0; i <= m_nSteps; i++) 
	m_nodes [i].m_vertex = m_bezier.Compute ((double) i / (double) m_nSteps);
CDoubleVector t = m_nodes [0].m_vertex;

m_nodes [0].m_rotation = m_base [0].m_rotation;
m_nodes [m_nSteps].m_rotation = m_base [1].m_rotation;
m_nodes [0].m_axis = m_base [0].m_rotation.R ();

// Compute each path node's rotation matrix from the previous node's rotation matrix
// First rotate the r and u vectors by the difference angles of the preceding and the current nodes' rotation matrices' z axis
// To do that, compute the angle using the dot product and the rotation vector from the two z axii perpendicular vector
// and rotate using a quaternion
// Then rotate the r and u vectors around the z axis by the z angle difference

double l = Length ();

CTunnelPathNode * n0, * n1;

#if ROTAXIS_METHOD

n1 = &m_nodes [0];
for (int i = 1; i <= m_nSteps; i++) {
	n0 = n1;
	n1 = &m_nodes [i];
	if (i < m_nSteps) { // last matrix is the end side's matrix - use it's forward vector
		n1->m_rotation.F () = m_nodes [i + 1].m_vertex - m_nodes [i - 1].m_vertex; //n0->m_vertex; //n1->m_vertex;
		n1->m_rotation.F ().Normalize ();
		}
	Bend (n0, n1);
	}

m_deltaAngle = TotalTwist ();

n1 = &m_nodes [0];
for (int i = 1; i <= m_nSteps; i++) {
	n0 = n1;
	n1 = &m_nodes [i];
	Twist (n0, n1, Length (i) / l);
	}

#ifdef _DEBUG
if (bTwist)
#endif
	m_nodes [m_nSteps].m_rotation = m_base [1].m_rotation;

#else

m_deltaAngle = TotalTwist ();

n1 = &m_nodes [0];
for (int i = 1; i <= m_nSteps; i++) {
	n0 = n1;
	n1 = &m_nodes [i];
	if (i < m_nSteps) { // last matrix is the end side's matrix - use it's forward vector
		n1->m_rotation.F () = m_nodes [i + 1].m_vertex - m_nodes [i - 1].m_vertex; //n0->m_vertex; //n1->m_vertex;
		n1->m_rotation.F ().Normalize ();
		}
	Bend (n0, n1);
	Twist (n0, n1, Length (i) / l);
	}

// Rotating the r and u vectors can cause an error because a x and y rotation may be applied. It would certainly
// be possible to fix that, but I have tormented my brain enough. Computing the error and rotating the vectors 
// accordingly works well enough.
#ifdef _DEBUG
static bool bFixError = true;
#endif

double error [2] = {0.0, 0.0}, direction = -Sign (Dot (m_base [1].m_rotation.U (), m_nodes [m_nSteps].m_rotation.R ()));

while (
#ifdef _DEBUG
	bFixError &&
#endif
	fabs (error [1] = acos (Dot (m_base [1].m_rotation.R (), m_nodes [m_nSteps].m_rotation.R ())) * direction) > 0.01) {
	if ((error [0] != 0.0) && (fabs (error [1]) > error [0]))
		direction = -direction;
	error [0] = fabs (error [1]);
	CQuaternion q;
	for (int i = m_nSteps; i > 0; i--) {
		n1 = &m_nodes [i];
		q.FromAxisAngle (n1->m_rotation.F (), error [1] * Length (i) / l);
		n1->m_rotation.R () = q * n1->m_rotation.R ();
		n1->m_rotation.U () = q * n1->m_rotation.U ();
		}
	}

#endif

for (int i = 0; i <= m_nSteps; i++) 
	m_nodes [i].m_rotation = m_nodes [i].m_rotation.Inverse ();
return true;
}

//------------------------------------------------------------------------------

double CTunnelPath::Length (int nSteps)
{
	double length = 0.0;

if (nSteps < 0)
	nSteps = m_nSteps;
for (int i = 1; i <= nSteps; i++) 
	length += Distance (m_nodes [i].m_vertex, m_nodes [i - 1].m_vertex);
return length;
}

//------------------------------------------------------------------------------

void CTunnelPath::Draw (CRenderer& renderer, CPen* redPen, CPen* bluePen, CViewMatrix* viewMatrix) 
{
#ifdef _DEBUG

for (int i = 0; i <= m_nSteps; i++) 
	m_nodes [i].Draw (renderer, viewMatrix);

#else

CDC* pDC = renderer.DC ();

renderer.BeginRender ();
for (int i = 0; i < 4; i++) {
	Bezier ().Transform (viewMatrix);
	Bezier ().Project (viewMatrix);
	}
renderer.EndRender ();

renderer.BeginRender (true);
renderer.SelectObject ((HBRUSH)GetStockObject (NULL_BRUSH));
renderer.SelectPen (penRed + 1);

CMineView* mineView = DLE.MineView ();
if (Bezier ().GetPoint (1).InRange (mineView->ViewMax ().x, mineView->ViewMax ().y, renderer.Type ())) {
	if (Bezier ().GetPoint (0).InRange (mineView->ViewMax ().x, mineView->ViewMax ().y, renderer.Type ())) {
		renderer.MoveTo (Bezier ().GetPoint (0).m_screen.x, Bezier ().GetPoint (0).m_screen.y);
		renderer.LineTo (Bezier ().GetPoint (1).m_screen.x, Bezier ().GetPoint (1).m_screen.y);
		renderer.Ellipse (Bezier ().GetPoint (1), 4, 4);
		}
	}
if (Bezier ().GetPoint (2).InRange (mineView->ViewMax ().x, mineView->ViewMax ().y, renderer.Type ())) {
	if (Bezier ().GetPoint (3).InRange (mineView->ViewMax ().x, mineView->ViewMax ().y, renderer.Type ())) {
		renderer.MoveTo (Bezier ().GetPoint (3).m_screen.x, Bezier ().GetPoint (3).m_screen.y);
		renderer.LineTo (Bezier ().GetPoint (2).m_screen.x, Bezier ().GetPoint (2).m_screen.y);
		renderer.Ellipse (Bezier ().GetPoint (2), 4, 4);
		}
	}
renderer.EndRender ();

#endif
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CTunnelMaker::Reset (void)
{
if (m_bActive) {
	m_tunnel.Release ();
	m_bActive = false;
	}
DLE.MineView ()->Refresh (false);
}

//------------------------------------------------------------------------------

void CTunnelMaker::Destroy (void) 
{
m_bActive = false;
m_tunnel.Destroy ();
m_path.Destroy ();
}

//------------------------------------------------------------------------------

void CTunnelMaker::Run (void) 
{
if (!m_bActive) {
	short nMaxSegments = SEGMENT_LIMIT - segmentManager.Count ();
	if (nMaxSegments > MAX_TUNNEL_SEGMENTS)
		nMaxSegments = MAX_TUNNEL_SEGMENTS;
	else if (nMaxSegments < 3) {
		ErrorMsg ("Insufficient number of free vertices and/or segments\nto use the tunnel generator.");
		return;
		}
	// make sure there are no children on either segment/side
	other = &selections [!current->Index ()];
	if ((current->Segment ()->ChildId (current->SideId ()) != -1) ||
		 (other->Segment ()->ChildId (other->m_nSide) != -1)) {
		ErrorMsg ("Starting and/or ending point of segment\n"
					 "already have segment(s) attached.\n\n"
					 "Hint: Put the current segment and the alternate segment\n"
					 "on sides which do not have cubes attached.");
		return;
		}

	if (!CalculateTunnel (true)) {
		m_bActive = false;
		return;
		}

	undoManager.Lock ("CTunnelMaker::Run");

	if (!DLE.ExpertMode ())
		ErrorMsg ("Place the current segment on one of the segment end points.\n\n"
					 "Use the CTRL+8 and CTRL+9 keys to adjust the length of the red segment.\n\n"
				    "Press 'P' to rotate the point connections.\n\n"
				    "Press 'G' or select Tools/Tunnel Generator when you are finished.");

	m_bActive = true;
	DLE.MineView ()->Refresh ();
	}
else {
	// ask if user wants to keep the new nSegment
	undoManager.Unlock ("CTunnelMaker::Run");
	m_tunnel.Release ();
	if (Query2Msg ("Do you want to keep this tunnel?", MB_YESNO) == IDYES) {
		undoManager.Begin (__FUNCTION__, udSegments | udVertices);
		if (CalculateTunnel (false) && Create ())
			m_tunnel.Realize (m_path, true);
		else
			m_tunnel.Release ();
		undoManager.End (__FUNCTION__);
		}
	Destroy ();
	}
segmentManager.SetLinesToDraw ();
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------------

bool CTunnelMaker::CalculateTunnel (bool bNewTunnelMakerInstance)
{
	bool bRegeneratePath = bNewTunnelMakerInstance;
	bool bRegenerateStartSides = bNewTunnelMakerInstance;

if (bNewTunnelMakerInstance) {
	m_base [0].Setup (current, -1.0, true);
	m_base [1].Setup (other, 1.0, false);
	}
else {
	if (m_base [0].m_updateStatus == CTunnelBase::UpdateOrientation) 
		m_base [0].Setup (null, -1.0, true);
	else if (m_base [0].m_updateStatus == CTunnelBase::UpdateSide) {
		m_base [0].Setup (current, -1.0, true);
		bRegeneratePath = true;
		bRegenerateStartSides = true;
		}
	if (m_base [1].m_updateStatus == CTunnelBase::UpdateOrientation) 
		m_base [1].Setup (null, 1.0, false);
	else if (m_base [1].m_updateStatus == CTunnelBase::UpdateSide) {
		m_base [1].Setup (current, 1.0, false);
		bRegeneratePath = true;
		}
	}
if (bRegeneratePath)
	m_nGranularity = 0;

if (m_path.Setup (m_base, bRegenerateStartSides, bRegeneratePath)) {
	m_tunnel.Setup (m_base);
	return true;
	}
ErrorMsg ("Could not calculate the tunnel path (out of memory).");
Destroy ();
return false;
}

//------------------------------------------------------------------------------

bool CTunnelMaker::Update (void) 
{ 
if (!m_bActive)
	return false;
if (current->Segment ()->HasChild (current->SideId ()) || other->Segment ()->HasChild (other->SideId ()))
	return true;
// If there is more than one start side, we know they have to be tagged
bool bStartSidesTagged = m_path.m_startSides.Length () > 1 ||
	segmentManager.Side (CSideKey (m_path.m_startSides [0].m_nSegment, m_path.m_startSides [0].m_nSide))->IsTagged ();
if (current - selections == m_base [0].m_nSelection) {
	if (m_base [0].IsUpdateNeeded (current, bStartSidesTagged))
		return CalculateTunnel (false);
	}
else {
	if (m_base [1].IsUpdateNeeded (current, bStartSidesTagged))
		return CalculateTunnel (false);
	}
return true;
}

//------------------------------------------------------------------------------

short CTunnelMaker::PathLength (void)
{
m_nSteps = short (m_path.Bezier ().Length () / 20.0 + Distance (m_base [0].GetPoint (), m_base [1].GetPoint ()) / 20.0) + m_nGranularity;
if (m_nSteps > MaxSegments () - 1)
	m_nSteps = MaxSegments () - 1;
else if (m_nSteps < 3)
	m_nSteps = 3;
return m_nSteps;
}

//------------------------------------------------------------------------------

bool CTunnelMaker::Create (void)
{
if (PathLength () <= 0) 
	return false;
if (m_path.Create (m_nSteps) && m_tunnel.Create (m_path))
	return true;
Destroy ();
return false;
}

//------------------------------------------------------------------------------

void CTunnelMaker::Draw (CRenderer& renderer, CPen* redPen, CPen* bluePen, CViewMatrix* viewMatrix)
{
if (Update () && Create ()) {
	m_path.Draw (renderer, redPen, bluePen, viewMatrix);
	m_tunnel.Draw (renderer, redPen, bluePen, viewMatrix);
	}
}

//------------------------------------------------------------------------------

void CTunnelMaker::Finer (void) 
{
if (m_bActive && m_nSteps < MaxSegments () - 1)
	++m_nGranularity;
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------------

void CTunnelMaker::Coarser (void) 
{
if (m_bActive && m_nSteps > 3)
	--m_nGranularity;
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------------

void CTunnelMaker::Stretch (void) 
{
if (current->SegmentId () == m_base [0].m_nSegment) {
	if (m_path.Bezier ().GetLength (0) > (MAX_TUNNEL_LENGTH - TUNNEL_INTERVAL))
		return;
	m_path.Bezier ().SetLength (m_path.Bezier ().GetLength (0) + TUNNEL_INTERVAL, 0);
	m_path.Bezier ().SetPoint (m_base [0].GetPoint () + m_base [0].GetNormal () * m_path.Bezier ().GetLength (0), 1);
	}
else if (current->SegmentId () == m_base [1].m_nSegment) {
	if (m_path.Bezier ().GetLength (1) > (MAX_TUNNEL_LENGTH - TUNNEL_INTERVAL))
		return;
	m_path.Bezier ().SetLength (m_path.Bezier ().GetLength (1) + TUNNEL_INTERVAL, 1);
	m_path.Bezier ().SetPoint (m_base [1].GetPoint () + m_base [1].GetNormal () * m_path.Bezier ().GetLength (1), 2);
	}
else
	return;
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------------

void CTunnelMaker::Shrink (void) 
{
if (current->SegmentId () == m_base [0].m_nSegment) {
	if (m_path.Bezier ().GetLength (0) < (MIN_TUNNEL_LENGTH + TUNNEL_INTERVAL)) 
		return;
	m_path.Bezier ().SetLength (m_path.Bezier ().GetLength (0) - TUNNEL_INTERVAL, 0);
	m_path.Bezier ().SetPoint (m_base [0].GetPoint () + m_base [0].GetNormal () * m_path.Bezier ().GetLength (0), 1);
	}
else if (current->SegmentId () == m_base [1].m_nSegment) {
	if (m_path.Bezier ().GetLength (1) < (MIN_TUNNEL_LENGTH + TUNNEL_INTERVAL))
		return;
	m_path.Bezier ().SetLength (m_path.Bezier ().GetLength (1) - TUNNEL_INTERVAL, 1);
	m_path.Bezier ().SetPoint (m_base [1].GetPoint () + m_base [1].GetNormal () * m_path.Bezier ().GetLength (1), 2);
	}
else
	return;
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------------
//eof TunnelMaker.cpp