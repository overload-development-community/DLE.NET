#ifndef __polymodel_h
#define __polymodel_h

//------------------------------------------------------------------------------
// CONSTANTS
//------------------------------------------------------------------------------

#define OP_EOF        0	/* eof                                 */
#define OP_DEFPOINTS  1	/* defpoints                (not used) */
#define OP_FLATPOLY   2	/* flat-shaded polygon                 */
#define OP_TMAPPOLY   3	/* texture-mapped polygon              */
#define OP_SORTNORM   4	/* sort by normal                      */
#define OP_RODBM      5	/* rod bitmap               (not used) */
#define OP_SUBCALL    6	/* call a subobject                    */
#define OP_DEFP_START 7	/* defpoints with start                */
#define OP_GLOW       8	/* m_info.glow value for next poly            */

#define MAX_INTERP_COLORS 100
#define MAX_POINTS_PER_POLY 25

#define MAX_OBJ_TEXTURES	600
#define MAX_ANIMATIONS		110

typedef struct {
	ushort index;
} BITMAP_INDEX;

//------------------------------------------------------------------------------
// the followin numbers are set using the original D2 robots

#define MAX_POLYMODEL_POINTS			416
#define MAX_POLYMODEL_POLYS			300
#define MAX_POLYMODEL_POLY_POINTS   13
#define MIN_POLYMODEL_POLY_POINTS	3
#define MAX_POLY_MODEL_SIZE			32000

//------------------------------------------------------------------------------

typedef struct tSubModel {
  int 			ptr;
  CFixVector 	offset;
  CFixVector 	norm;		// norm for sep plane
  CFixVector 	pnt;		// point on sep plane
  int 			rad;		// radius for each submodel
  ubyte 			parent;  // what is parent for each submodel
  CFixVector 	vMin;
  CFixVector   vMax;
} tSubModel;

//------------------------------------------------------------------------------
//used to describe a polygon model

typedef struct tPolyModel {
  int				nModels;
  int 			dataSize;
  ubyte*			renderData;
  tSubModel		subModels [MAX_SUBMODELS];
  CFixVector 	vMin, vMax;			  // min, max for whole model
  int				rad;
  ubyte			textureCount;
  ushort			firstTexture;
  ubyte			simplerModel;			  // alternate model with less detail (0 if none, nModel+1 else)
} tPolyModel;

//------------------------------------------------------------------------------

class CPolyModel {
public:
	tPolyModel		m_info;
	IViewMatrix*	m_view;
	int				m_bCustom;

	CPolyModel () : m_bCustom (0) { Clear (); }
	
	~CPolyModel () { Release (); }
	
	inline void Clear (void) { memset (&m_info, 0, sizeof (m_info)); }

	inline void Release (void) {
		if (m_info.renderData) {
			delete m_info.renderData;
			m_info.renderData = null;
			}
		}

	void Read (IFileManager* fp, bool bRenderData = false);

	void Write (IFileManager* fp, bool bRenderData = false);

	inline bool Draw (IViewMatrix* view, int nStage) {
		if (!m_info.renderData) 
			return false;
		m_view = view;
		g_data.modelManager->Render (m_info.renderData, nStage, m_bCustom, m_info.firstTexture);
		return true;
		}
};

//------------------------------------------------------------------------------

#endif //__polymodel_h
