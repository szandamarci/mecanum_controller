//#ifndef OPENGLVIEW_H
//#define OPENGLVIEW_H

//// Only pure OpenGL related stuff is used, but:
////
//// "Microsoft requires system DLLs to use a specific calling convention
//// that isn't the default calling convention for most Win32 C compilers,
//// so they've annotated the OpenGL calls in gl.h with some macros that
//// expand to nonstandard C syntax. This causes Microsoft's C compilers
//// to use the system calling convention.  One of the include files
//// included by windows.h defines the macros."
//// [http://www.opengl.org/resources/faq/technical/gettingstarted.htm]
////
//// So we need to include that header file when Win32 compilers are used.
//#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
//#include <windows.h>
//#endif


//#include <gl/gl.h>


//namespace Leuze
//{
//	namespace OpenGL
//	{
//		/** \class OpenGLview

//		\brief A pure OpenGL view for measurement data.

//		OpenGLview provides an interface to display measurement data using OpenGL
//		(needs to be hosted by a windows application - e.g. WIN32 or X Window).
//		*/
//		class OpenGLview
//		{
//		private:
//			GLdouble *m_pVertices;
//			int       m_nVertices;

//			GLfloat  m_iSceneZoomFactor;
//			GLdouble m_dSceneHorizontalOffset;
//			GLdouble m_dSceneVerticalOffset;

//		public:
//			/**
//			C'tor - allocates vertex memory.
//			*/
//			OpenGLview(void);
//			/**
//			D'tor - releases resources.
//			*/
//			~OpenGLview(void);
//			/**
//			Inits the view.
//			*/
//			void init(void);
//			/**
//			Displays the view.
//			*/
//			void display(void);
//			/**
//			Not used at the moment-
//			*/
//			void purge(void);
//			/**
//			Resizes the view.
//			@param iWidth New width of view.
//			@param iHeight New height of view.
//			*/
//			void reshape(int iWidth, int iHeight);
//			/**
//			Sets the measurement values (coordinates).
//			@param adX Array of x coordinates.
//			@param adY Array of y coordinates.
//			@param nValues Amount of coordinates.
//			*/
//			void setMeasureValues(const GLdouble adX[], const GLdouble adY[], int nValues);
//			/**
//			Slides up
//			*/
//			void slideViewUp(void);
//			/**
//			Slides down
//			*/
//			void slideViewDown(void);
//			/**
//			Slides to the left
//			*/
//			void slideViewLeft(void);
//			/**
//			Slides to the rigth
//			*/
//			void slideViewRight(void);
//			/**
//			Zooms in
//			*/
//			void zoomIn(void);
//			/**
//			Zooms out
//			*/
//			void zoomOut(void);
//			/**
//			Sets scaling back to defaults
//			*/
//			void resetView(void);

//		private:
//			/**
//			Not implemented.
//			Only declaration to avoid default copy constructor.
//			*/
//			OpenGLview(const OpenGLview&);
//			/**
//			Not implemented.
//			Only declaration to avoid default assignment operator.
//			*/
//			OpenGLview & operator=(const OpenGLview&);
//			/**
//			Initializes the scene.
//			*/
//			void initScene(void);
//			/**
//			Draws the objects (scanner, measure values).
//			*/
//			void drawObjects(void);
//			/**
//			Displays the scene.
//			*/
//			void displayScene(void);
//			/**
//			Updates view
//			*/
//			void updateView(void);

//		};
//	}
//}

//#endif //OPENGLVIEW_H
