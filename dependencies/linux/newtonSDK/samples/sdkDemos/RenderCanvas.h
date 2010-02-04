#ifndef __RENDER_CANVAS__
#define __RENDER_CANVAS__




class DGCamera;
class RenderCanvas: public wxGLCanvas
{
public:

	RenderCanvas(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
	virtual ~RenderCanvas(void);

	void InitGl ();
	void ResetProjectionMode();

	void DrawScene ();

	private:
	void OnIdle(wxIdleEvent &event);
	void OnSize(wxSizeEvent& event);
	void OnPaint(wxPaintEvent& event);

	DECLARE_EVENT_TABLE()

	DGCamera* m_camera; 
	bool m_glIsInit;
};


#endif