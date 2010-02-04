/* Copyright (c) <2009> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

#include <toolbox_stdafx.h>
#include "MainFrame.h"
#include "RenderCanvas.h"
#include "dGlueToCollada.h"
#include "TargaToOpenGl.h"
#include "dMesh.h"


struct NEWTON_SDK_DEMOS
{
	const char *m_name;
	const char *m_description;
	LaunchSDKDemoCallback m_launchDemoCallback;
};

static NEWTON_SDK_DEMOS demosSelection[] = 
{
	{_T("spinning sphere"), _T("show a very simple rotating sphere"), BasicSphereSpin},
	{_T("coefficients of restitution"), _T("demonstrate the effect of various coefficient of restitution"), Restitution},
};

#define LOAD_DEFAULT_SCENE (0 + wxID_HIGHEST + 1)






IMPLEMENT_APP(NewtonSdkApp)

BEGIN_EVENT_TABLE(NewtonFrame, wxFrame)
	EVT_MENU(wxID_OPEN, NewtonFrame::OnFileOpen)
	EVT_MENU(wxID_SAVE, NewtonFrame::OnFileSave)
	EVT_MENU(wxID_EXIT, NewtonFrame::OnExit)

	EVT_MENU_RANGE (wxID_HIGHEST + 1, wxID_HIGHEST + 1 + sizeof (demosSelection) / sizeof (demosSelection[0]), NewtonFrame::onSDKDemo)

END_EVENT_TABLE()





bool NewtonSdkApp::OnInit()
{
	// check for memory leaks
#if defined(_DEBUG) && defined(_MSC_VER)
 	// Track all memory leaks at the operating system level.
	// make sure no Newton tool or utility leaves leaks behind.
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF|_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF));
#endif

dModel xxx1;
dLoaderContext context;
xxx1.LoadCollada("C:/Newton_200/NewtonSDK/samples/bin/jeep.dae", context, GetIdentityMatrix(), 1.0f);
xxx1.SaveCollada("C:/Newton_200/NewtonSDK/samples/bin/xxx1.dae", GetIdentityMatrix(), 1.0f);

	NewtonFrame *frame = new NewtonFrame( "Newton 2.0 SDK demos", wxDefaultPosition, wxSize(1024, 768));
	frame->Show(true);
	SetTopWindow(frame);

	// load the default scene
	wxMenuEvent startScene (wxEVT_COMMAND_MENU_SELECTED, LOAD_DEFAULT_SCENE);
	frame->GetEventHandler()->ProcessEvent(startScene);

	return true;
}




NewtonFrame::NewtonFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	:wxFrame(NULL, -1, title, pos, size)
{
	// add load and save file options
	wxMenu *fileMenu = new wxMenu;
	fileMenu->Append(wxID_OPEN, _T("&Open..."), _T("Open a collada scene"));
	fileMenu->Append(wxID_SAVE, _T("&Save..."), _T("Save scene in collada format"));
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_EXIT, _T("E&xit\tAlt-X"), _T("Quit SDK sample") );

	wxMenu *sdkDemos = new wxMenu;
	for (int i = 0; i < int (sizeof (demosSelection) / sizeof (demosSelection[0])); i ++) {
		sdkDemos->Append (wxID_HIGHEST + 1 + i, _T (demosSelection[i].m_name), _T (demosSelection[i].m_description));
	}

	wxMenuBar *menuBar  = new wxMenuBar;
	menuBar->Append(fileMenu, _T("&File"));
	menuBar->Append(sdkDemos, _T("SDK demos"));
//	menuBar->Append(options, _T("EngineOptions"));

	// these items should be initially checked
//	menuBar->Check(Menu_MenuBar_Toggle, true);
//	menuBar->Check(Menu_MenuBar_Enable, true);
//	menuBar->Check(Menu_Menu_Enable, true);
//	menuBar->Check(Menu_Menu_Check, false);

	// associate the menu bar with the frame
	SetMenuBar(menuBar);

	// add a status bar to display the engine settings
	CreateStatusBar();
	SetStatusText(_T("Engine settings:"));

	// add the render window	
	m_canvas = new RenderCanvas(this, wxID_ANY, wxDefaultPosition, wxSize(300, 300), wxSUNKEN_BORDER);
}

NewtonFrame::~NewtonFrame()
{

}


void NewtonFrame::OnFileSave(wxCommandEvent& event)
{
	// get the file name to be save
	wxString filename = wxFileSelector(_T("Save collada scene"), _T(""), _T(""), _T(""), _T("Collada files (*.dae)|*.dae"), wxFD_SAVE);
	if (!filename.IsEmpty() && m_world)
	{
		class ExportScenContext: public dColladaSceneExportContext 
		{
			public:
			ExportScenContext ()
				:dColladaSceneExportContext ()
			{
			}
			
			virtual dModel* GetModel (NewtonBody* const body) 
			{
				dModel* model;

				model = (dModel*) NewtonBodyGetUserData(body);
				return model;
			}
	
		};
		ExportScenContext context;
		dMatrix yawMatrix (dYawMatrix (3.14159265f * 0.5f));
		ExportColladaScene (filename, m_world, &context, yawMatrix, 1.0f);
	}
}



void NewtonFrame::OnFileOpen(wxCommandEvent& event)
{
	// get the file name to be loaded
	wxString filename = wxFileSelector(_T("Open collada scene"), _T(""), _T(""), _T(""), _T("Collada files (*.dae)|*.dae"), wxFD_OPEN);

	if (!filename.IsEmpty())
	{
//		m_canvas->LoadDXF(filename);
//		m_canvas->Refresh(false);
	}
}


void NewtonFrame::OnExit(wxCommandEvent& event)
{
	// close destroy and terminate the main frame
	Close(true);
}


void NewtonFrame::onSDKDemo (wxCommandEvent& event)
{
	// close the current scene playing
	CloseScene ();

	// find the selected scene
	for (int i = 0; i < int (sizeof (demosSelection) / sizeof (demosSelection[0])); i ++) {
		if (event.GetId() == (wxID_HIGHEST + 1 + i)) {
			// if a scene was found launch it a break the search
			LoadScene (demosSelection[i].m_launchDemoCallback);
			break;
		}
	}

	// send a display refresh event in case the runtime update is stopped bu the user.
	wxPaintEvent paintEvent;
	m_canvas->GetEventHandler()->ProcessEvent(paintEvent);
}
