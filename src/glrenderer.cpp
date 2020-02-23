#if OGL_RENDERING

# define OGL_MAPPED 1

static HDC		glHDC;
static double	glRotMat [3][3] = {{1,0,0},{0,1,0},{0,0,1}};
static double	glAngle [3] = {0,0,0};
static double	glPan [3] = {0,0,-500};
static double	uvMap [4][2] = {{0.0,0.0},{0.0,1.0},{1.0,1.0},{1.0,0.0}};
static UINT8	rgbBuf [64*64*4];
static GLuint	glHandles [910];
static UINT8	*glPalette = NULL;
static BOOL		glFitToView = FALSE;
static INT32	glMinZ = 1;
static INT32	glMaxZ = 1;
static bool		glInit = true;
#endif

// code from NeHe productions tutorials

                        /*--------------------------*/

BOOL CMineView::GLInit (GLvoid)
{
glShadeModel (GL_SMOOTH);
glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
glClearDepth (1.0f);
glEnable (GL_DEPTH_TEST);
glDepthFunc (GL_LEQUAL);
glEnable (GL_ALPHA_TEST);
glAlphaFunc (GL_GEQUAL, 0.5);	
glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
memset (glHandles, 0, sizeof (glHandles));		
GLFitToView ();
#if OGL_ORTHO == 0
SetViewPoints ();
glMinZ = m_minViewPoint.z;
glMaxZ = m_maxViewPoint.z;
double dx = abs (m_maxViewPoint.x - m_minViewPoint.x);
double dy = abs (m_maxViewPoint.y - m_minViewPoint.y);
double z = m_viewPoints [m_minVPIdx.x].z;
if (z > m_viewPoints [m_maxVPIdx.x].z)
	z = m_viewPoints [m_maxVPIdx.x].z;
dx = dx / 2 + z;
CRect rc;
GetClientRect (rc);
double aspect = (double) rc.Width () / (double) rc.Height ();
if (aspect > 0)
	aspect = 1.0 / aspect;
z = m_viewPoints [m_minVPIdx.y].z;
if (z > m_viewPoints [m_maxVPIdx.y].z)
	z = m_viewPoints [m_maxVPIdx.y].z;
dy = dy / 2 + z;
dx *= aspect;
//dy *= (double) rc.Width () / (double) rc.Height ();
double dd = (dx > dy) ? dx: dy;
if (dd > glMinZ)
	glMinZ = -dd;
#endif
glInit = true;
return TRUE;
}

                        /*--------------------------*/

BOOL CMineView::GLInitPalette (GLvoid)
{
return glPalette || (glPalette = PalettePtr ());
}

                        /*--------------------------*/

static double glCX = 2.0, glCY = 2.0, glCZ = 2.0;

GLvoid CMineView::GLFitToView (GLvoid)
{
FitToView ();
#if 1
	SetViewPoints ();
	glCX = m_maxViewPoint.x - m_minViewPoint.x;
	glCY = m_maxViewPoint.y - m_minViewPoint.y;
	glCZ = m_maxViewPoint.z - m_minViewPoint.z;
# if 1
	CRect rc;
	GetClientRect (rc);
	double aspect = (double) rc.Width () / (double) rc.Height ();
	if (aspect > 0)
		glCX *= aspect;
	else
		glCY *= aspect;
# else
	if (glCX < glCY)
		glCX = glCY;
	else
		glCX = glCY;
# endif
#else
CRect	rc;
SetViewPoints (&rc);

INT32 rad = rc.Width ();
if (rad < rc.Height ())
	rad = rc.Height ();
if (rad < (m_maxViewPoint.z - m_minViewPoint.z + 1))
	rad = (m_maxViewPoint.z - m_minViewPoint.z + 1);
INT32 xMid = (rc.left + rc.right) / 2;
INT32 yMid = (rc.bottom + rc.top) / 2;
double left = (double) xMid - rad;
double right = (double) xMid + rad;
double bottom = (double) yMid - rad;
double top = (double) yMid + rad;
GetClientRect (rc);
double aspect = (double) rc.Width () / (double) rc.Height ();
if (aspect < 1.0) {
	bottom /= aspect;
	top /= aspect;
	}
else {
	left /= aspect;
	right /= aspect;
	}
glMatrixMode (GL_PROJECTION);
glLoadIdentity ();
//glOrtho (left, right, bottom, top, 1.0, 1.0 + rad);
glMatrixMode (GL_MODELVIEW);
glLoadIdentity ();
gluLookAt (0.0, 0.0, 2.0 * rad, xMid, yMid, (m_maxViewPoint.z + m_minViewPoint.z) / 2, 0.0, 1.0, 0.0);
#endif
glFitToView = TRUE;
}

                        /*--------------------------*/

BOOL CMineView::GLResizeScene (GLvoid) 
{
if (!GLCreateWindow ())
	return FALSE;

CRect rc;
	
GetClientRect (rc);
//ClientToScreen (rc);
if (!(rc.Width () && rc.Height ()))
	return FALSE;
glViewport (rc.left, rc.top, rc.Width (), rc.Height ());
/*
if (glFitToView)
	glFitToView = FALSE;
else 
*/
	{
//	glMatrixMode (GL_PROJECTION);
//	glLoadIdentity ();
	//glOrtho (-0.5, 0.5, -0.5, 0.5, -1, 1);
	//gluPerspective (90.0f, (GLfloat) rc.Width () / (GLfloat) rc.Height (), 1.0f, 10000.0f);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
#if OGL_ORTHO
	glOrtho (-glCX / 2, glCX / 2, glCY / 2, -glCY / 2, -glCZ / 2, glCZ / 2);
#else
	//glOrtho (-1, 1, -1, 1, -1, 1);
	//glOrtho (0, 1, 0, 1, 0, 1);
	gluPerspective (90.0f, (GLfloat) rc.Width () / (GLfloat) rc.Height (), 1.0f, 10000.0f);
#endif
	//glOrtho (-1.0 / glCX, 1.0 / glCX, -1.0 / glCY, 1.0 / glCY, -1.0, 1.0);
	//glOrtho (0, rc.Width (), 0, rc.Height (), -1, 1);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	}
/*
glMatrixMode (GL_PROJECTION);
glLoadIdentity ();
glMatrixMode (GL_MODELVIEW);
glLoadIdentity ();
*/
return TRUE;
}

                        /*--------------------------*/

void CMineView::GLCreateTexture (INT16 nTexture)
{
if (!GLInitPalette ())
	return;
nTexture &= 0x1FFF; 
if (!glHandles [nTexture]) {
	CTexture tx (bmBuf);
	DefineTexture (nTexture, 0, &tx, 0, 0);
	DrawAnimDirArrows (nTexture, &tx);
	// create RGBA bitmap from source bitmap
	INT32 h, i, j;
	for (h = i = 0; i < 64*64; i++) {
		j = bmBuf [i];
		if (j < 254) {
			j *= 3;
			rgbBuf [h++] = glPalette [j++] << 2;
			rgbBuf [h++] = glPalette [j++] << 2;
			rgbBuf [h++] = glPalette [j++] << 2;
			rgbBuf [h++] = 255;
			}
		else {
			rgbBuf [h++] = 0;
			rgbBuf [h++] = 0;
			rgbBuf [h++] = 0;
			rgbBuf [h++] = 0;
			}
		}
	glGenTextures (1, glHandles + nTexture); 
	glEnable (GL_TEXTURE_2D);
	glBindTexture (GL_TEXTURE_2D, glHandles [nTexture]); 
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbBuf);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
}

                        /*--------------------------*/

void CMineView::GLRenderTexture (INT16 nSegment, INT16 nSide, INT16 nTexture)
{
	CSegment *segP = theMine->Segments (nSegment);
	CSide *sideP = segP->m_sides + nSide;
	CUVL *uvls;
	double l;
#if OGL_MAPPED
	APOINT *a;
#else
	CFixVector *verts = theMine->Vertices ();
	CFixVector *v;
#endif
	static INT32 rotOffs [4] = {0,3,2,1};
	INT32 h = rotOffs [(nTexture & 0xC000) >> 14];
	INT32 j = h;
	bool bShaded = (m_viewMineFlags & eViewMineShading) != 0;

//CBRK (nSegment == 57 && nSide == 3);
GLCreateTexture (nTexture);
glEnable (GL_TEXTURE_2D);
glBindTexture (GL_TEXTURE_2D, glHandles [nTexture & 0x1FFF]); 
glBegin (GL_TRIANGLE_FAN);
INT32 i;
for (i = 0; i < 4; i++) {
	uvls = sideP->m_info.uvls + j;
	l = (bShaded ? uvls->l: F1_0) / UV_FACTOR;
	glColor3d (l,l,l);
	switch (h) {
		case 0:
			glTexCoord2d (uvls->u / 2048.0, uvls->v / 2048.0); 
			break;
		case 1:
			glTexCoord2d (uvls->v / 2048.0, uvls->u / 2048.0); 
			break;
		case 2:
			glTexCoord2d (-uvls->u / 2048.0, -uvls->v / 2048.0); 
			break;
		case 3:
			glTexCoord2d (-uvls->v / 2048.0, -uvls->u / 2048.0); 
			break;
		}
#if OGL_MAPPED
	a = m_viewPoints + segP->m_info.verts [sideVertTable [nSide][j]];
	glVertex3f ((double) a->x, (double) a->y, (double) a->z);
#else
	v = verts + segP->m_info.verts [sideVertTable [nSide][i]];
	glVertex3f (f2fl (v->x), f2fl (v->y), f2fl (v->z));
#endif
	j = (j + 1) % 4;
	}
glEnd ();
}

                        /*--------------------------*/

void CMineView::GLRenderFace (INT16 nSegment, INT16 nSide)
{
	CSegment *segP = theMine->Segments (nSegment);
	CSide *sideP = segP->m_sides + nSide;
	CFixVector *verts = theMine->Vertices ();
	UINT16 nWall = segP->m_sides [nSide].m_info.nWall;

if (sideP->m_info.nBaseTex < 0)
	return;
CWall *pWall = (nWall == NO_WALL) ? NULL: ((CDlcDoc*) GetDocument ())->v.theMine->Walls (nWall);
if ((segP->m_info.children [nSide] > -1) &&
	 (!pWall || (pWall->m_info.type == WALL_OPEN) || ((pWall->m_info.type == WALL_CLOAKED) && !pWall->m_info.cloakValue)))
	return;
#if OGL_MAPPED
APOINT& p0 = m_viewPoints [segP->m_info.verts [sideVertTable [nSide] [0]]];
APOINT& p1 = m_viewPoints [segP->m_info.verts [sideVertTable [nSide] [1]]];
APOINT& p3 = m_viewPoints [segP->m_info.verts [sideVertTable [nSide] [3]]];

CFixVector a,b;
a.x = p1.x - p0.x;
a.y = p1.y - p0.y;
b.x = p3.x - p0.x;
b.y = p3.y - p0.y;
if (a.x*b.y > a.y*b.x)
	return;
#else
CFixVector *p0 = verts + segP->m_info.verts [sideVertTable [nSide] [0]];
CFixVector *p1 = verts + segP->m_info.verts [sideVertTable [nSide] [1]];
CFixVector *p3 = verts + segP->m_info.verts [sideVertTable [nSide] [3]];

CFixVector a,b;
a.x = p1->x - p0->x;
a.y = p1->y - p0->y;
b.x = p3->x - p0->x;
b.y = p3->y - p0->y;
if (a.x*b.y > a.y*b.x)
	return;
#endif
GLRenderTexture (nSegment, nSide, sideP->m_info.nBaseTex);
if (sideP->m_info.nOvlTex)
	GLRenderTexture (nSegment, nSide, sideP->m_info.nOvlTex);
}

                        /*--------------------------*/

BOOL CMineView::GLRenderScene (GLvoid)	
{
if (!GLResizeScene ())
	return FALSE;
//glDrawBuffer (GL_BACK);
glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//glLoadIdentity ();
#if 1
if (glInit) {
	glInit = false;
	//m_view.Rotate ('Y', -(PI/2));
	GLFitToView ();
	}
CRect rc;
GetClientRect (rc);
double cx = (double) rc.Width () / 2;
double cy = (double) rc.Height () / 2;
glTranslated (-cx, -cy, glMinZ);
#else
glTranslated (glPan [0], glPan [1], glPan [2] -500);	
INT32 i;
for (i = 0; i < 3; i++)
	glRotated (glAngle [i], glRotMat [i][0], glRotMat [i][1], glRotMat [i][2]);
#endif
SetViewPoints ();
for (INT16 nSegment = theMine->SegCount (); nSegment--; )
	for (INT16 nSide = 0; nSide < MAX_SIDES_PER_SEGMENT; nSide++)
		GLRenderFace (nSegment, nSide);
return TRUE;
}

                        /*--------------------------*/

GLvoid CMineView::GLReset (GLvoid)
{
glDeleteTextures (910, glHandles);
memset (glHandles, 0, sizeof (glHandles));
glPalette = NULL;
glInit = false;
}

                        /*--------------------------*/

GLvoid CMineView::GLKillWindow (GLvoid)
{
GLReset ();
if (m_glRC)	{
	if (!wglMakeCurrent (NULL,NULL))
		ErrorMsg ("OpenGL: Release of DC and RC failed.");
	if (!wglDeleteContext(m_glRC))
		ErrorMsg ("OpenGL: Release of rendering context failed.");
		m_glRC = NULL;
	}
if (m_glDC && !::ReleaseDC (m_hWnd, glHDC))	 {
	//ErrorMsg ("OpenGL: Release of device context failed.")
		;
	m_glDC=NULL;	
	}
#if 0
if (!UnregisterClass ("OpenGL",AfxGetInstance ())) {
	ErrorMsg ("OpenGL: Could Not Unregister Class.");
	}
#endif
}

                        /*--------------------------*/

BOOL CMineView::GLCreateWindow (CDC * pDC)
{
if (m_glDC)
	return TRUE;

	GLuint PixelFormat;

static PIXELFORMATDESCRIPTOR pfd = {
	sizeof (PIXELFORMATDESCRIPTOR),
	1,	
	PFD_DRAW_TO_WINDOW |	PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
	PFD_TYPE_RGBA,
	32,
	0, 0, 0, 0, 0, 0,
	0,	
	0,
	0,
	0, 0, 0, 0,
	32,
	0,
	0,
	PFD_MAIN_PLANE,
	0,
	0, 0, 0
	};

if (!(m_glDC = GetDC ())) {
	GLKillWindow ();
	ErrorMsg ("OpenGL: Can't create device context.");
	return FALSE;
	}
glHDC = m_glDC->GetSafeHdc ();
if (!(PixelFormat = ChoosePixelFormat (glHDC, &pfd))) {
	GLKillWindow ();
	sprintf_s (message, sizeof (message), "OpenGL: Can't find a suitable pixel format. (%d)", GetLastError ());
	ErrorMsg (message);
	return FALSE;
	}
if(!SetPixelFormat(glHDC, PixelFormat, &pfd)) {
		GLKillWindow();
		sprintf_s (message, sizeof (message), "OpenGL: Can't set the pixel format (%d).", GetLastError ());
		ErrorMsg (message);
		return FALSE;
	}
if (!(m_glRC = wglCreateContext (glHDC))) {
	GLKillWindow ();
	sprintf_s (message, sizeof (message), "OpenGL: Can't create a rendering context (%d).", GetLastError ());
	ErrorMsg (message);
	return FALSE;
	}

if(!wglMakeCurrent (glHDC, m_glRC)) {
	GLKillWindow ();
	sprintf_s (message, sizeof (message), "OpenGL: Can't activate the rendering context (%d).", GetLastError ());
	ErrorMsg (message);
	return FALSE;
	}
if (!GLInit())	{
	GLKillWindow ();
	ErrorMsg ("OpenGL: Initialization failed.");
	return FALSE;
	}
return TRUE;
}

                        /*--------------------------*/

#endif //OGL_RENDERING
