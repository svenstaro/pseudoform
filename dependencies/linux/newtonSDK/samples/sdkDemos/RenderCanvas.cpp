#include <toolbox_stdafx.h>

#include "RenderCanvas.h"
#include "MainFrame.h"
#include "DGCamera.h"
#include "PhysicsUtils.h"
#include "HiResTimer.h"


BEGIN_EVENT_TABLE(RenderCanvas, wxGLCanvas)
	EVT_SIZE(RenderCanvas::OnSize)
	EVT_PAINT(RenderCanvas::OnPaint)
	EVT_IDLE(RenderCanvas::OnIdle)
//	EVT_ERASE_BACKGROUND(RenderCanvas::OnEraseBackground)
//	EVT_MOUSE_EVENTS(RenderCanvas::OnMouse)
END_EVENT_TABLE()


CHiResTimer timer;

RenderCanvas::RenderCanvas(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	: wxGLCanvas(parent, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE, _T("GLRenderCanvas"))
{
	// init open gl

	m_glIsInit = false;
	m_camera = NULL;
}


RenderCanvas::~RenderCanvas(void)
{	
	delete m_camera;
}


void RenderCanvas::InitGl ()
{
	// Our shading model--Gouraud (smooth). 
	glShadeModel (GL_SMOOTH);

	// Culling. 
	glCullFace (GL_BACK);
	glFrontFace (GL_CCW);
	glEnable (GL_CULL_FACE);
	
//	glEnable(GL_DITHER);

	// z buffer test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);

	// Set the clear color. 
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	// Change to the projection matrix and set our viewing volume.
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity( );

	// set on ambient directional light
	GLfloat lightColor[] = { 1.0f, 1.0f, 1.0f, 0.0 };
	GLfloat lightAmbientColor[] = { 0.7f, 0.7f, 0.7f, 0.0 };
	GLfloat lightPosition[] = { 500.0f, 200.0f, 500.0f, 0.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbientColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// create the camera
	m_camera = new DGCamera(0.1f, 90.0f, 1000.0f, 1.0f, 0.1f);
	m_camera->ResetDefaultCamera ();
}


void RenderCanvas::ResetProjectionMode()
{
	int w, h;
	GetClientSize(&w, &h);
	if (GetContext()) 	{
		SetCurrent();
		glViewport(0, 0, (GLint) w, (GLint) h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(90.0f, (GLfloat)w/h, 0.1f, 1000.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
}

void RenderCanvas::OnSize(wxSizeEvent& event)
{
	// this is also necessary to update the context on some platforms
	wxGLCanvas::OnSize(event);
	// Reset the OpenGL view aspect
	ResetProjectionMode();
}


void RenderCanvas::OnPaint (wxPaintEvent& WXUNUSED(event) )
{
	// must always be here
	wxPaintDC dc(this);

	SetCurrent();
	if (GetContext()) {

		// Initialize OpenGL
		if (!m_glIsInit)
		{
			InitGl();
			ResetProjectionMode();
			m_glIsInit = true;
		}


		// clear the screen
//		glClearColor( 0.3f, 0.4f, 0.6f, 1.0f );
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// submit all the geometry
		// Transformations
		DrawScene ();

		// draw everything and swap the display buffer
		glFlush();

		// Swap
		SwapBuffers();

//		if (specialKeyOn) {
//			specialKeyOn = 0;
//			for (int i = 0; i < KeyCode_lastSpecialKey; i ++) {
//				keybuffer[i] = 0;
//			}
//		}
	}
}

void RenderCanvas::OnIdle(wxIdleEvent &event)
{
	// send a paint update for viewing the next frame 
	wxPaintEvent paintEvent;
	GetEventHandler()->ProcessEvent(paintEvent);

	// make sure we send a request for the next update
	event.RequestMore ();
}


void RenderCanvas::DrawScene ()
{
	dFloat timeStep;
	dFloat visualTimeStep;
	NewtonFrame* parent;
	
	parent = (NewtonFrame*) GetParent();

	// read the keyboard
//	if (dGetKeyState (KeyCode_ESCAPE)) {
//		exit(0);
//		return;
//	}

	// get the time step
	timeStep = timer.GetElapsedSeconds();
	visualTimeStep = timeStep;
	timeStep = 1.0f/60.0f;

	// update the Newton physics world
//	if (!re_entrantUpdate) {
//		re_entrantUpdate = 1;
//		if (!simulationStopped) {
			if (parent->m_world) {
				NewtonUpdate (parent->m_world, timeStep);
//				NewtonCollisionUpdate (demo.m_world);
			}
//		}
//		re_entrantUpdate = 0;
//	}


	// read the control 	
	//	demo.ApplyControl ();

	// apply the Camera after the controls are read
	dVector target (cameraEyepoint + cameraDir);
	m_camera->Position (cameraEyepoint);
	m_camera->TargetPosition(target);
	m_camera->Apply();

	// render the scene
	glEnable (GL_LIGHTING);
	glPushMatrix();	
//	if (!hideGeometry) {
		parent->Render ();
//	}
	glPopMatrix();


/*
//	if (showCollision->get_int_val()) {
//		DebugRenderWorldCollision (demo.m_world);
//	}

//	DebugRenderDebugInfo ();

//	if (!simulationStopped) {
//		int profileFlags = 0;
//		for (int i = 0; i < int (sizeof (showProfiler) / sizeof (showProfiler[0])); i ++) {
//			profileFlags |=  showProfiler[i] ? (1 << i) : 0;
//		}

		//profileFlags = 1;
//		if (profileFlags) {
//			profiler.Render (demo.m_world, profileFlags);
//		}
//	}

//	if (showfps) {
//		dVector color (1.0, 1.0f, 1.0f);
//		Print (color, 4,  4, "KiloBytes used (%7d)     FPS (%3d)", NewtonGetMemoryUsed() / 1024, int (1.0f / visualTimeStep + 0.5));
//	}
*/
} 
