#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED


#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

//#include "wx/glcanvas.h"
#include <wx/glcanvas.h>
#include "DisplayModel.h"
#include "DisplayTester.h"
#include "TextDisplay.h"

class BasicGLPane : public wxGLCanvas
{
    wxGLContext*	m_context;

public:
	BasicGLPane(wxFrame* parent, int* args);
	virtual ~BasicGLPane();

	void resized(wxSizeEvent& evt);

	int getWidth();
	int getHeight();

	void render(wxPaintEvent& evt);
	void prepare2DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y);

	static void * refreshThread(void *);

	// events
	void mouseMoved(wxMouseEvent& event);
	void mouseDown(wxMouseEvent& event);
	void mouseWheelMoved(wxMouseEvent& event);
	void mouseReleased(wxMouseEvent& event);
	void rightClick(wxMouseEvent& event);
	void mouseLeftWindow(wxMouseEvent& event);
	void keyPressed(wxKeyEvent& event);
	void keyReleased(wxKeyEvent& event);

	DECLARE_EVENT_TABLE()

	private:
        DisplayModel *model;
};

#endif // GRAPHICS_H_INCLUDED
