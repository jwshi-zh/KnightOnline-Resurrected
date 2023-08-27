// N3CEDoc.cpp : implementation of the CN3CEDoc class
//

#include "stdafx.h"
#include "N3CE.h"
#include "N3CEDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CN3CEDoc

IMPLEMENT_DYNCREATE(CN3CEDoc, CDocument)

BEGIN_MESSAGE_MAP(CN3CEDoc, CDocument)
	//{{AFX_MSG_MAP(CN3CEDoc)
	ON_COMMAND(ID_FILE_SAVE_AS_ONE_FOLDER, OnFileSaveAsOneFolder)
	ON_COMMAND(ID_TOOL_OPTIMIZE_ANIMATION_DATA, OnToolOptimizeAnimationData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CN3CEDoc construction/destruction

CN3CEDoc::CN3CEDoc()
{
	if(m_Scene.CameraCount() <= 0) m_Scene.DefaultCameraAdd();
	if(m_Scene.LightCount() <= 0) m_Scene.DefaultLightAdd();

	m_bLoadingNow = FALSE;

	// Character 珥