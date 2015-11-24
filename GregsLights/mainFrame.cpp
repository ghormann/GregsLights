#include "wx/wx.h"
#include "wx/sizer.h"
#include "wx/glcanvas.h"
#include "include/graphics.h"
#include <wx/timer.h>
#include <iostream>

// include OpenGL
#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include <stdio.h>

class MyApp: public wxApp
{
    virtual bool OnInit();
    virtual int OnExit();

    wxFrame *frame;
    BasicGLPane * glPane;
public:

};

IMPLEMENT_APP(MyApp)

int MyApp::OnExit()
{
    printf("On Exit in myApp\n");
    glPane->shutdownShow();
    return 0;
}


bool MyApp::OnInit()
{
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    frame = new wxFrame((wxFrame *)NULL, -1,  wxT("Hello GL World"), wxPoint(50,50), wxSize(1000,800));

    int args[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};

    glPane = new BasicGLPane( (wxFrame*) frame, args);
    sizer->Add(glPane, 1, wxEXPAND);

    frame->SetSizer(sizer);
    frame->SetAutoLayout(true);

    frame->Show();

    RenderTimer *timer = new RenderTimer(glPane);
    timer->Start(50); // REdray every 50 ms
    return true;
}

BEGIN_EVENT_TABLE(BasicGLPane, wxGLCanvas)
    EVT_MOTION(BasicGLPane::mouseMoved)
    EVT_LEFT_DOWN(BasicGLPane::mouseDown)
    EVT_LEFT_UP(BasicGLPane::mouseReleased)
    EVT_RIGHT_DOWN(BasicGLPane::rightClick)
    EVT_LEAVE_WINDOW(BasicGLPane::mouseLeftWindow)
    EVT_SIZE(BasicGLPane::resized)
    EVT_KEY_DOWN(BasicGLPane::keyPressed)
    EVT_KEY_UP(BasicGLPane::keyReleased)
    EVT_MOUSEWHEEL(BasicGLPane::mouseWheelMoved)
    EVT_PAINT(BasicGLPane::render)
END_EVENT_TABLE()


// some useful events to use
void BasicGLPane::mouseMoved(wxMouseEvent& event) {}
void BasicGLPane::mouseDown(wxMouseEvent& event) {}
void BasicGLPane::mouseWheelMoved(wxMouseEvent& event) {}
void BasicGLPane::mouseReleased(wxMouseEvent& event) {}
void BasicGLPane::rightClick(wxMouseEvent& event) {}
void BasicGLPane::mouseLeftWindow(wxMouseEvent& event) {}
void BasicGLPane::keyPressed(wxKeyEvent& event) {}
void BasicGLPane::keyReleased(wxKeyEvent& event) {}

void BasicGLPane::shutdownShow()
{
    printf("Shutting down show\n");
    model->shutdown();
}

BasicGLPane::BasicGLPane(wxFrame* parent, int* args) :
    wxGLCanvas(parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
{
    m_context = new wxGLContext(this);

    // To avoid flashing on MSW
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

    bool sendDMX = false;
    model = new DisplayModel(sendDMX, true, false);
    sleep(1); // Wait for threads
    //new TextDisplay(model);

    DisplayTester *tester = new DisplayTester(model);
    tester->testAll();

    //GregsDisplay *display = new GregsDisplay(model);
    //display->run();
}

BasicGLPane::~BasicGLPane()
{
    delete m_context;
}

void BasicGLPane::resized(wxSizeEvent& evt)
{
//	wxGLCanvas::OnSize(evt);

    Refresh();
}

/** Inits the OpenGL viewport for drawing in 2D. */
void BasicGLPane::prepare2DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
    glEnable(GL_TEXTURE_2D);   // textures
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glViewport(topleft_x, topleft_y, bottomrigth_x-topleft_x, bottomrigth_y-topleft_y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(topleft_x, bottomrigth_x, bottomrigth_y, topleft_y);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int BasicGLPane::getWidth()
{
    return GetSize().x;
}

int BasicGLPane::getHeight()
{
    return GetSize().y;
}

RenderTimer::RenderTimer(BasicGLPane* pane) : wxTimer()
{
    RenderTimer::pane = pane;
    cnt = 0;
}

void RenderTimer::Notify()
{
    pane->Refresh();
    /*
    printf("Debug: Render: %d\n", cnt);
    if (++cnt > 1000) {
        cnt = 0;
    }
    */
}

void BasicGLPane::render( wxPaintEvent& evt )
{
    if(!IsShown()) return;

    wxGLCanvas::SetCurrent(*m_context);
    wxPaintDC(this); // only to be used in paint events. use wxClientDC to paint outside the paint event

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ------------- draw some 2D ----------------
    prepare2DViewport(0,0,getWidth(), getHeight());
    glLoadIdentity();

    // black background
    glColor4f(0, 0, 0, 1);
    glBegin(GL_QUADS);
    glVertex3f(0,0,0);
    glVertex3f(getWidth(),0,0);
    glVertex3f(getWidth(),getHeight(),0);
    glVertex3f(0,getHeight(),0);
    glEnd();

    // The Sign
    for (int j = 0; j < SIGN_HEIGHT; j++ )
    {
        for (int i = 0; i< SIGN_WIDTH; i++)
        {
            RGBLight *pix = model->getSign()->getPixal(i,j);
            float red = ((float)pix->getRed())  / 100;
            float green = ((float)pix->getGreen()) / 100;
            float blue = ((float)pix->getBlue()) / 100;
            glColor4f(red, green, blue, 1);
            glBegin(GL_QUADS);
            glVertex3f(i*6+3, 3 + j*6, 0); // upper lefft
            glVertex3f(i*6+3, 6 + j*6, 0);  // Lower Left
            glVertex3f(i*6+6, 6 + j*6, 0); // Bottom Right
            glVertex3f(i*6+6, 3 + j*6, 0); // Uper Right
            glEnd();
        }
    }

    // Large Grid
    for (int j = 0; j < LGRID_PIXAL_HEIGHT; j++ )
    {
        for (int i = 0; i< LGRID_PIXAL_WIDTH; i++)
        {
            RGBLight *pix = model->getGrid()->getPixal(i,j);
            float red = ((float)pix->getRed())  / 100;
            float green = ((float)pix->getGreen()) / 100;
            float blue = ((float)pix->getBlue()) / 100;
            glColor4f(red, green, blue, 1);
            glBegin(GL_QUADS);
            glVertex3f(i*6+3, 3 + j*6 + LGRID_LINE, 0); // upper lefft
            glVertex3f(i*6+3, 6 + j*6 + LGRID_LINE, 0);  // Lower Left
            glVertex3f(i*6+6, 6 + j*6 + LGRID_LINE, 0); // Bottom Right
            glVertex3f(i*6+6, 3 + j*6 + LGRID_LINE, 0); // Uper Right
            glEnd();
        }
    }


    // Bushes
    for (int i = BUSH_LIGHT_START; i <= BUSH_LIGHT_END; i++)
    {
        Bush * bush = model->getBush(i);
        float blue = ((float)bush->getBlue()->getIntensity()) / 100;
        float red = ((float)bush->getRed()->getIntensity()) / 100;
        float green = ((float)bush->getGreen()->getIntensity()) / 100;
        float white = ((float)bush->getWhite()->getIntensity()) / 100;
        int offset = 0;
        if (i>3)
        {
            offset=50;
        }
        glColor4f(red, green, blue, 1);
        glBegin(GL_QUADS);
        glVertex3f(i*30+4+offset, 4 +BUSH_LINE, 0);
        glVertex3f(i*30+4+offset, 20 + BUSH_LINE, 0);
        glVertex3f(i*30+20+offset, 20 + BUSH_LINE, 0);
        glVertex3f(i*30+20+offset, 4 + BUSH_LINE, 0);
        glEnd();

        // white
        glColor4f(white, white, white, 1);
        glBegin(GL_QUADS);
        glVertex3f(i*30+4+offset, 4 +BUSH_LINE+30, 0);
        glVertex3f(i*30+4+offset, 20 + BUSH_LINE+30, 0);
        glVertex3f(i*30+20+offset, 20 + BUSH_LINE+30, 0);
        glVertex3f(i*30+20+offset, 4 + BUSH_LINE+30, 0);
        glEnd();
    }

    //Spot lights
    for (int i = HOUSE_LIGHT_START; i <= HOUSE_LIGHT_END; i++)
    {
        RGBLight *pix = model->getHouse(i);
        float red = ((float)pix->getRed())  / 100;
        float green = ((float)pix->getGreen()) / 100;
        float blue = ((float)pix->getBlue()) / 100;
        int offset = -30;

        if (i > 2)
            offset = 30;

        glColor4f(red, green, blue, 1);
        glBegin(GL_QUADS);
        glVertex3f(i*50+4+offset, 4 + BUSH_LINE+70, 0);
        glVertex3f(i*50+4+offset, 20 + BUSH_LINE+70, 0);
        glVertex3f(i*50+40+offset, 20 + BUSH_LINE+70, 0);
        glVertex3f(i*50+40+offset, 4 + BUSH_LINE+70, 0);
        glEnd();

    }

    // Stars
    int cnt = 0;
    for (int j = 0; j < 6; j++)
    {
        int numCols = 8;
        int offset = (j%4) * (32/4);
        if (j == 2 || j == 3 || j == 6) {
            numCols = 7;
        }
        for (int i =0; i < numCols; i++)
        {
            std::cout << i << "," << j  << ": " << cnt << std::endl;
            RGBLight *pix = model->getStars()->getStar(cnt++);
            float red = ((float)pix->getRed())  / 100;
            float green = ((float)pix->getGreen()) / 100;
            float blue = ((float)pix->getBlue()) / 100;

            glColor4f(red, green, blue, 1);
            glBegin(GL_QUADS);

            glVertex3f(i*32+3+offset, 3 + STAR_LINE + j*32, 0); // upper lefft
            glVertex3f(i*32+3+offset, 9 + STAR_LINE + j*32, 0);  // Lower Left
            glVertex3f(i*32+9+offset, 9 + STAR_LINE + j*32, 0); // Bottom Right
            glVertex3f(i*32+9+offset, 3 + STAR_LINE + j*32, 0); // Uper Right

            glEnd();

        }
    }

    glFlush();
    SwapBuffers();
}
