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

#ifndef __MAIN_FRAME__
#define __MAIN_FRAME__


#include "SceneManager.h"

class NewtonSdkApp: public wxApp
{
	virtual bool OnInit();
};

class RenderCanvas;

class NewtonFrame: public wxFrame, public SceneManager
{
public:

	NewtonFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	~NewtonFrame();

	private:

	void OnExit(wxCommandEvent& event);
	void OnFileOpen(wxCommandEvent& event);
	void OnFileSave(wxCommandEvent& event);
//	void OnExit(wxCommandEvent& event);
//	void OnAbout(wxCommandEvent& event);

	void onSDKDemo (wxCommandEvent& event);

	RenderCanvas* m_canvas;
	DECLARE_EVENT_TABLE()
};


#endif
