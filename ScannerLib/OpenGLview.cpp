//#include "OpenGLview.h"
//#include <malloc.h>


////needed if not winodws.h included
//#ifndef NULL
//#define NULL 0
//#endif


//namespace Leuze
//{
//	namespace OpenGL
//	{
//		/** Color in which the measure contour and the ROD square is drawn.
//		*/
//		const GLfloat SCENE_DRAW_COLOR[] = {1.0, 0.0, 0.0};

//		const int MAX_VERTICE_COUNT =      529 + 1;
//		//defines center
//		const GLdouble SCENE_CENTER_X =        0.0;
//		const GLdouble SCENE_CENTER_Y =        0.0;
//		//define ROD square size
//		const GLdouble SCENE_SCANNER_X1 =    -70.0;
//		const GLdouble SCENE_SCANNER_Y1 =   -104.0;
//		const GLdouble SCENE_SCANNER_X2 =     70.0;
//		const GLdouble SCENE_SCANNER_Y2 =     64.0;

//		const GLfloat SCENE_SCALE =            1.0f;
//		//no rotation or translation
//		const GLfloat SCENE_ROTATE_X =         0.0f;
//		const GLfloat SCENE_ROTATE_Y =         0.0f;
//		const GLfloat SCENE_ROTATE_Z =         0.0f;
//		const GLfloat SCENE_TRANSLATE_X =      0.0f;
//		const GLfloat SCENE_TRANSLATE_Y =      0.0f;
//		const GLfloat SCENE_TRANSLATE_Z =      0.0f;
//		//standard scaling
//		const GLdouble SCENE_LEFT =        -2000.0;
//		const GLdouble SCENE_RIGHT =        2000.0;
//		const GLdouble SCENE_BOTTOM =       -500.0;
//		const GLdouble SCENE_TOP =          3500.0;

//		const GLdouble SCENE_SLIDE_STEP  = 100.0;
//		const GLint    SCENE_ZOOM_FACTOR =  10;
//	}
//}


//using namespace Leuze;
//using namespace Leuze::OpenGL;


////-------------------------------------------------------------------
//OpenGLview::OpenGLview(void)
//{
//	m_pVertices = (GLdouble *) calloc(MAX_VERTICE_COUNT * 2, sizeof(GLdouble));
//	m_nVertices = 0;

//	m_iSceneZoomFactor = SCENE_ZOOM_FACTOR;
//	m_dSceneHorizontalOffset = 0.0;
//	m_dSceneVerticalOffset = 0.0;
//}
////-------------------------------------------------------------------
//OpenGLview::~OpenGLview(void)
//{
//	if(NULL != m_pVertices)
//	{
//		free(m_pVertices);
//		m_pVertices = NULL;
//	}
//}
////-------------------------------------------------------------------
//void OpenGLview::init(void)
//{
//	initScene();
//}
////-------------------------------------------------------------------
//void OpenGLview::display(void)
//{
//	displayScene();
//}
////-------------------------------------------------------------------
//void OpenGLview::purge(void)
//{
//	//nothing to do at the moment
//}
////-------------------------------------------------------------------
//void OpenGLview::setMeasureValues(const GLdouble adX[], const GLdouble adY[], int nValues)
//{
//	if(nValues >= MAX_VERTICE_COUNT)
//	{
//		//ERROR!
//		return;
//	}

//	m_nVertices = nValues + 1;

//	m_pVertices[0] = SCENE_CENTER_X;
//	m_pVertices[1] = SCENE_CENTER_Y;

//	for(int i = 0; i < nValues; ++i){
//		m_pVertices[2 + i * 2] = adX[i];
//		m_pVertices[3 + i * 2] = adY[i];
//	}
//}
////-------------------------------------------------------------------
//void OpenGLview::initScene(void)
//{
//	glClearColor(0.0, 0.0, 0.0, 0.0); //black
//	glShadeModel(GL_FLAT);
//	glEnableClientState(GL_VERTEX_ARRAY);
//	glVertexPointer(2, GL_DOUBLE, 0, m_pVertices);
//}
////-------------------------------------------------------------------
//void OpenGLview::drawObjects(void)
//{
//	//draw scanner
//	glRectd(SCENE_SCANNER_X1, SCENE_SCANNER_Y1, SCENE_SCANNER_X2, SCENE_SCANNER_Y2);

//	//draw shape (measure values)
//	glBegin(GL_LINE_LOOP);//glBegin(GL_POINTS);
//	for(int i = 0; i < m_nVertices; ++i)
//	{
//		glArrayElement(i); //points of shape
//	}
//	glEnd();
//}
////-------------------------------------------------------------------
//void OpenGLview::displayScene(void)
//{
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
	
//	glScalef(SCENE_SCALE, SCENE_SCALE, SCENE_SCALE); // scale
//	glRotatef(SCENE_ROTATE_X, 1.0,  0.0,  0.0); // rotate around X-axis
//	glRotatef(SCENE_ROTATE_Y, 0.0, -1.0,  0.0); // rotate around Y-axis
//	glRotatef(SCENE_ROTATE_Z, 0.0,  0.0, -1.0); // rotate around Z-axis
//	glTranslatef(SCENE_TRANSLATE_X, -SCENE_TRANSLATE_Y, -SCENE_TRANSLATE_Z); // move

//	//glColor3f(1.0f, 1.0f, 1.0f);
//	glColor3f(SCENE_DRAW_COLOR[0], SCENE_DRAW_COLOR[1], SCENE_DRAW_COLOR[2]);
//	drawObjects();

//	glFlush();
//}
////-------------------------------------------------------------------
//void OpenGLview::reshape(int iWidth, int iHeight)
//{
//	glViewport(0, 0, (GLsizei) iWidth, (GLsizei) iHeight);

//	updateView();
//}
////-------------------------------------------------------------------
//void OpenGLview::slideViewUp(void)
//{
//	m_dSceneVerticalOffset += SCENE_SLIDE_STEP;
//	updateView();
//}
////-------------------------------------------------------------------
//void OpenGLview::slideViewDown(void)
//{
//	m_dSceneVerticalOffset -= SCENE_SLIDE_STEP;
//	updateView();
//}
////-------------------------------------------------------------------
//void OpenGLview::slideViewLeft(void)
//{
//	m_dSceneHorizontalOffset -= SCENE_SLIDE_STEP;
//	updateView();
//}
////-------------------------------------------------------------------
//void OpenGLview::slideViewRight(void)
//{
//	m_dSceneHorizontalOffset += SCENE_SLIDE_STEP;
//	updateView();
//}
////-------------------------------------------------------------------
//void OpenGLview::zoomIn(void)
//{
//	if(m_iSceneZoomFactor > 1)
//	{
//		m_iSceneZoomFactor -= 1;
//	}
//	updateView();
//}
////-------------------------------------------------------------------
//void OpenGLview::zoomOut(void)
//{
//	m_iSceneZoomFactor += 1;
//	updateView();
//}
////-------------------------------------------------------------------
//void OpenGLview::resetView(void)
//{
//	m_iSceneZoomFactor = SCENE_ZOOM_FACTOR;
//	m_dSceneHorizontalOffset = 0.0;
//	m_dSceneVerticalOffset = 0.0;
//	updateView();
//}
////-------------------------------------------------------------------
//void OpenGLview::updateView(void)
//{
//	GLdouble dSceneLeft   = m_dSceneHorizontalOffset - (m_iSceneZoomFactor * 200.0);
//	GLdouble dSceneRight  = m_dSceneHorizontalOffset + (m_iSceneZoomFactor * 200.0);
//	GLdouble dSceneBottom = m_dSceneVerticalOffset - (m_iSceneZoomFactor * 50.0);
//	GLdouble dSceneTop    = m_dSceneVerticalOffset + (m_iSceneZoomFactor * 350.0);

//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();

//	glOrtho(dSceneLeft, dSceneRight, dSceneBottom, dSceneTop, -1.0, 1.0);
//}
////-------------------------------------------------------------------
