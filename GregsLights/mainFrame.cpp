#include "wx/wx.h"
#include "wx/sizer.h"
#include "wx/glcanvas.h"
#include "include/graphics.h"
#include <wx/timer.h>
#include "../include/Snowmen.h"
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

int digitX[50];
int digitY[50];


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
    // Init x Positions
    for (int i=0; i<6; i++)
    {
        digitX[i] = 6;
        digitX[i+11] = 0; // 11-16
    }

    for (int i=6; i<12; i++)
    {
        digitX[i] = 11-i;
    }

    int x=0;
    for (int i=17; i<24; i++)
    {
        digitX[i] = x++;
    }

    for (int i=23; i<=30; i++)
    {
        digitX[i] = 6;
    }

    x=6;
    for (int i=30; i<37; i++)
    {
        digitX[i] = x--;
    }

    for (int i=36; i<=42; i++)
    {
        digitX[i] =  0;
    }

    //init Y positions
    int y=5;
    for (int i =0; i<=5; i++)
    {
        digitY[i] = y--;
    }

    for (int i=6; i<12; i++)
    {
        digitY[i] = 0;
    }

    y=0;
    for (int i=11; i<18; i++)
    {
        digitY[i] = y++;
    }

    for (int i=18; i<24; i++)
    {
        digitY[i] = 6;
    }

    y=6;
    for (int i=23; i<31; i++)
    {
        digitY[i] = y++;
    }

    for (int i=30; i< 37; i++)
    {
        digitY[i] = 13;
    }

    y=13;
    for (int i=36; i< 43; i++)
    {
        digitY[i] = y--;
    }


    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    frame = new wxFrame((wxFrame *)NULL, -1,  wxT("Hello GL World"), wxPoint(50,50), wxSize(1000,820));

    int args[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};

    glPane = new BasicGLPane( (wxFrame*) frame, args);
    sizer->Add(glPane, 1, wxEXPAND);

    frame->SetSizer(sizer);
    frame->SetAutoLayout(true);

    frame->Show();

    RenderTimer *timer = new RenderTimer(glPane);
    timer->Start(25); // REdray every 25 ms (40Hz)
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

    //DisplayTester *tester = new DisplayTester(model);
    //tester->testAll();

    GregsDisplay *display = new GregsDisplay(model);
    display->run();


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

void drawGrid(int xoffset, int yoffset, int pixSpacing, GenericGrid * grid)
{
    int height = grid->getGridHeight();
    int width = grid->getGridWidth();
    int pix2X = pixSpacing * 2;
    for (int j = 0; j < height; j++ )
    {
        for (int i = 0; i< width; i++)
        {
            RGBLight *pix = grid->getPixal(i,j);
            float red = ((float)pix->getRed())  / 100;
            float green = ((float)pix->getGreen()) / 100;
            float blue = ((float)pix->getBlue()) / 100;
            int basePosX = i * pix2X + xoffset + pixSpacing;
            int basePosY = pixSpacing + j*pixSpacing*2 + yoffset;
            glColor4f(red, green, blue, 1);
            glBegin(GL_QUADS);

            glVertex3f(basePosX, basePosY, 0); // upper lefft
            glVertex3f(basePosX, basePosY + pixSpacing, 0);  // Lower Left
            glVertex3f(basePosX+pixSpacing, basePosY + pixSpacing, 0); // Bottom Right
            glVertex3f(basePosX+pixSpacing, basePosY, 0); // Uper Right
            glEnd();
        }
    }


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

    // Sign
    model->getSign()->lockSign();
    drawGrid(130,100,2,model->getSign());
    model->getSign()->releaseSign();
    // Snowmen
    model->getSnowmen()->lockSnowmen();
    drawGrid(0,250,1,model->getSnowmen()->getSnowmen(SNOWMAN_LEFT));
    drawGrid(94,250,2,model->getSnowmen()->getSplashGrid(SNOWMAN_LEFT));
    drawGrid(190,210,2,model->getSnowmen()->getSkyGrid());
    drawGrid(574,250,2,model->getSnowmen()->getSplashGrid(SNOWMAN_RIGHT));
    drawGrid(671,250,1,model->getSnowmen()->getSnowmen(SNOWMAN_RIGHT));
    model->getSnowmen()->releaseSnowmen();

    // The Clock

    for (int digit = 0; digit < CLOCK_DIGITS; digit++)
    {
        for (int i = 0; i  < 43; i++)
        {
            int x = 50+ (CLOCK_DIGITS-digit) * 60 + digitX[i]*6;
            int y = 0 + digitY[i] * 6;

            RGBLight *pix = model->getClock()->getPixel(digit,i);
            float red = ((float)pix->getRed())  / 100;
            float green = ((float)pix->getGreen()) / 100;
            float blue = ((float)pix->getBlue()) / 100;

            glColor4f(red, green, blue, 1);
            glBegin(GL_QUADS);
            glVertex3f(x, y, 0); // upper lefft
            glVertex3f(x+3, y, 0);  // Lower Left
            glVertex3f(x+3, y+3, 0); // Bottom Right
            glVertex3f(x, y+3, 0); // Uper Right
            glEnd();
        }
    }


    glFlush();
    SwapBuffers();
}
