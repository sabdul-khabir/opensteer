// ----------------------------------------------------------------------------
//
//
// OpenSteer -- Steering Behaviors for Autonomous Characters
//
// Copyright (c) 2002-2005, Sony Computer Entertainment America
// Original author: Craig Reynolds <craig_reynolds@playstation.sony.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//
// ----------------------------------------------------------------------------
//
//
// glutCallbacks
//
// Callbacks
//
//
//
// 01-19-25 sak:  created
//  
//
//
// ----------------------------------------------------------------------------


#include "glutCallbacks.h"
#include "OpenSteerDemo.h"
#include "glutInitialize.h"
#include "Annotation.h"
//#include "Color.h"
#include "OpenSteer/Vec3.h"

#include <algorithm>
#include <string>
#include <sstream>
#include <iomanip>

// Include headers for OpenGL (gl.h), OpenGL Utility Library (glu.h) and
// OpenGL Utility Toolkit (glut.h).
//
// XXX In Mac OS X these headers are located in a different directory.
// XXX Need to revisit conditionalization on operating system.
#if __APPLE__ && __MACH__
#include <GLUT/glut.h>   // for Mac OS X
#else
#include <GL/glut.h>     // for Linux and Windows
#endif


    std::string const appVersionName("OpenSteerDemo 0.8.2");

    // The number of our GLUT window
    int windowID;

    bool gMouseAdjustingCameraAngle = false;
    bool gMouseAdjustingCameraRadius = false;
    int gMouseAdjustingCameraLastX;
    int gMouseAdjustingCameraLastY;



    const char* getAppVersion(void)
    {
       return appVersionName.c_str();
    }
    // ----------------------------------------------------------------------------
    // initialize GL mode settings


    void 
    initGL (void)
    {
        // background = dark gray
        // @todo bknafla Changed the background color to make some screenshots.
        glClearColor (0.3f, 0.3f, 0.3f, 0);
        // glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );

        // enable depth buffer clears
        glClearDepth (1.0f);

        // select smooth shading
        glShadeModel (GL_SMOOTH);

        // enable  and select depth test
        glDepthFunc (GL_LESS);
        glEnable (GL_DEPTH_TEST);

        // turn on backface culling
        glEnable (GL_CULL_FACE);
        glCullFace (GL_BACK);

        // enable blending and set typical "blend into frame buffer" mode
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // reset projection matrix
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
    }





    // ----------------------------------------------------------------------------
    // handler for window resizing


    void 
    reshapeFunc (int width, int height)
    {
        // set viewport to full window
        glViewport(0, 0, width, height);

        // set perspective transformation
        glMatrixMode (GL_PROJECTION);
        glLoadIdentity ();
        const GLfloat w = width;
        const GLfloat h = height;
        const GLfloat aspectRatio = (height == 0) ? 1 : w/h;
        const GLfloat fieldOfViewY = 45;
        const GLfloat hither = 1;  // put this on Camera so PlugIns can frob it
        const GLfloat yon = 400;   // put this on Camera so PlugIns can frob it
        gluPerspective (fieldOfViewY, aspectRatio, hither, yon);

        // leave in modelview mode
        glMatrixMode(GL_MODELVIEW);
    }


    // ----------------------------------------------------------------------------
    // This is called (by GLUT) each time a mouse button pressed or released.


    void 
    mouseButtonFunc (int button, int state, int x, int y)
    {
        // if the mouse button has just been released
        if (state == GLUT_UP)
        {
            // end any ongoing mouse-adjusting-camera session
            gMouseAdjustingCameraAngle = false;
            gMouseAdjustingCameraRadius = false;
        }

        // if the mouse button has just been pushed down
        if (state == GLUT_DOWN)
        {
            // names for relevant values of "button" and "state"
            const int  mods       = glutGetModifiers ();
            const bool modNone    = (mods == 0);
            const bool modCtrl    = (mods == GLUT_ACTIVE_CTRL);
            const bool modAlt     = (mods == GLUT_ACTIVE_ALT);
            const bool modCtrlAlt = (mods == (GLUT_ACTIVE_CTRL | GLUT_ACTIVE_ALT));
            const bool mouseL     = (button == GLUT_LEFT_BUTTON);
            const bool mouseM     = (button == GLUT_MIDDLE_BUTTON);
            const bool mouseR     = (button == GLUT_RIGHT_BUTTON);

    #if __APPLE__ && __MACH__
            const bool macosx = true;
    #else
            const bool macosx = false;
    #endif

            // mouse-left (with no modifiers): select vehicle
            if (modNone && mouseL)
            {
                OpenSteerDemo::selectVehicleNearestScreenPosition (x, y);
            }

            // control-mouse-left: begin adjusting camera angle (on Mac OS X
            // control-mouse maps to mouse-right for "context menu", this makes
            // OpenSteerDemo's control-mouse work work the same on OS X as on Linux
            // and Windows, but it precludes using a mouseR context menu)
            if ((modCtrl && mouseL) ||
               (modNone && mouseR && macosx))
            {
                gMouseAdjustingCameraLastX = x;
                gMouseAdjustingCameraLastY = y;
                gMouseAdjustingCameraAngle = true;
            }

            // control-mouse-middle: begin adjusting camera radius
            // (same for: control-alt-mouse-left and control-alt-mouse-middle,
            // and on Mac OS X it is alt-mouse-right)
            if ((modCtrl    && mouseM) ||
                (modCtrlAlt && mouseL) ||
                (modCtrlAlt && mouseM) ||
                (modAlt     && mouseR && macosx))
            {
                gMouseAdjustingCameraLastX = x;
                gMouseAdjustingCameraLastY = y;
                gMouseAdjustingCameraRadius = true;
            }
        }
    }


    // ----------------------------------------------------------------------------
    // called when mouse moves and any buttons are down


    void 
    mouseMotionFunc (int x, int y)
    {
        // are we currently in the process of mouse-adjusting the camera?
        if (gMouseAdjustingCameraAngle || gMouseAdjustingCameraRadius)
        {
            // speed factors to map from mouse movement in pixels to 3d motion
            const float dSpeed = 0.005f;
            const float rSpeed = 0.01f;

            // XY distance (in pixels) that mouse moved since last update
            const float dx = x - gMouseAdjustingCameraLastX;
            const float dy = y - gMouseAdjustingCameraLastY;
            gMouseAdjustingCameraLastX = x;
            gMouseAdjustingCameraLastY = y;

            OpenSteer::Vec3 cameraAdjustment;

            // set XY values according to mouse motion on screen space
            if (gMouseAdjustingCameraAngle)
            {
                cameraAdjustment.x = dx * -dSpeed;
                cameraAdjustment.y = dy * +dSpeed;
            }

            // set Z value according vertical to mouse motion
            if (gMouseAdjustingCameraRadius)
            {
                cameraAdjustment.z = dy * rSpeed;
            }

            // pass adjustment vector to camera's mouse adjustment routine
            OpenSteerDemo::camera.mouseAdjustOffset (cameraAdjustment);
        }
    }


    // ----------------------------------------------------------------------------
    // called when mouse moves and no buttons are down


    void 
    mousePassiveMotionFunc (int x, int y)
    {
        OpenSteerDemo::mouseX = x;
        OpenSteerDemo::mouseY = y;
    }


    // ----------------------------------------------------------------------------
    // called when mouse enters or exits the window


    void 
    mouseEnterExitWindowFunc (int state)
    {
        if (state == GLUT_ENTERED) OpenSteerDemo::mouseInWindow = true;
        if (state == GLUT_LEFT)    OpenSteerDemo::mouseInWindow = false;
    }


    // ----------------------------------------------------------------------------
    // draw PlugI name in upper lefthand corner of screen


    void 
    drawDisplayPlugInName (void)
    {
        const int h = glutGet (GLUT_WINDOW_HEIGHT);
        const OpenSteer::Vec3 screenLocation (10, h-20, 0);
        draw2dTextAt2dLocation (*OpenSteerDemo::nameOfSelectedPlugIn (),
                                screenLocation,
           OpenSteer::gWhite, drawGetWindowWidth(), drawGetWindowHeight());
    }


    // ----------------------------------------------------------------------------
    // draw camera mode name in lower lefthand corner of screen


    void 
    drawDisplayCameraModeName (void)
    {
        std::ostringstream message;
        message << "Camera: " << OpenSteerDemo::camera.modeName () << std::ends;
        const OpenSteer::Vec3 screenLocation (10, 10, 0);
        draw2dTextAt2dLocation (message, screenLocation, OpenSteer::gWhite, drawGetWindowWidth(), drawGetWindowHeight());
    }





    // ----------------------------------------------------------------------------
    // draw camera mode name in lower lefthand corner of screen


    void
       drawDisplayDemoHelp(void)
    {
       std::ostringstream message;

       int demo_key = 0;
       const char* line = OpenSteerDemo::getDemoKeyHelp(demo_key);
       while (line != NULL)
       {
          message << line << std::endl;
          line = OpenSteerDemo::getDemoKeyHelp(++demo_key);
       }

       int fn_key_no = 1;
       line = OpenSteerDemo::getFunctionKeyHelp(fn_key_no);
       while (line != NULL)
       {
          message << line << std::endl;
          line = OpenSteerDemo::getFunctionKeyHelp(++fn_key_no);
       }
       const int h = glutGet(GLUT_WINDOW_HEIGHT);
       //const OpenSteer::Vec3 screenLocation(10, h - 20, 0);
       const OpenSteer::Vec3 screenLocation(drawGetWindowWidth() - 400, h - 20, 0);
       draw2dTextAt2dLocation(message, screenLocation, OpenSteer::gWhite, drawGetWindowWidth(), drawGetWindowHeight());
    }


    // ----------------------------------------------------------------------------
    // helper for drawDisplayFPS



    void 
    writePhaseTimerReportToStream (float phaseTimer,
                                              std::ostringstream& stream)
    {
        // write the timer value in seconds in floating point
        stream << std::setprecision (5) << std::setiosflags (std::ios::fixed);
        stream << phaseTimer;

        // restate value in another form
        stream << std::setprecision (0) << std::setiosflags (std::ios::fixed);
        stream << " (";

        // different notation for variable and fixed frame rate
        if (OpenSteerDemo::clock.getVariableFrameRateMode())
        {
            // express as FPS (inverse of phase time)
            stream << 1 / phaseTimer;
            stream << " fps)\n";
        }
        else
        {
            // quantify time as a percentage of frame time
            const int fps = OpenSteerDemo::clock.getFixedFrameRate ();
            stream << ((100 * phaseTimer) / (1.0f / fps));
            stream << "% of 1/";
            stream << fps;
            stream << "sec)\n";
        }
    }


    // ----------------------------------------------------------------------------
    // draw text showing (smoothed, rounded) "frames per second" rate
    // (and later a bunch of related stuff was dumped here, a reorg would be nice)
    //
    // XXX note: drawDisplayFPS has morphed considerably and should be called
    // something like displayClockStatus, and that it should be part of
    // OpenSteerDemo instead of Draw  (cwr 11-23-04)

    float gSmoothedTimerDraw = 0;
    float gSmoothedTimerUpdate = 0;
    float gSmoothedTimerOverhead = 0;

    void
    drawDisplayFPS (void)
    {
        // skip several frames to allow frame rate to settle
        static int skipCount = 10;
        if (skipCount > 0)
        {
            skipCount--;
        }
        else
        {
            // keep track of font metrics and start of next line
            const int lh = 16; // xxx line height
            const int cw = 9; // xxx character width
            OpenSteer::Vec3 screenLocation (10, 10, 0);

            // target and recent average frame rates
            const int targetFPS = OpenSteerDemo::clock.getFixedFrameRate ();
            const float smoothedFPS = OpenSteerDemo::clock.getSmoothedFPS ();

            // describe clock mode and frame rate statistics
            screenLocation.y += lh;
            std::ostringstream clockStr;
            clockStr << "Clock: ";
            if (OpenSteerDemo::clock.getAnimationMode ())
            {
                clockStr << "animation mode (";
                clockStr << targetFPS << " fps,";
                clockStr << " display "<< OpenSteer::round(smoothedFPS) << " fps, ";
                const float ratio = smoothedFPS / targetFPS;
                clockStr << (int) (100 * ratio) << "% of nominal speed)";
            }
            else
            {
                clockStr << "real-time mode, ";
                if (OpenSteerDemo::clock.getVariableFrameRateMode ())
                {
                    clockStr << "variable frame rate (";
                    clockStr << OpenSteer::round(smoothedFPS) << " fps)";
                }
                else
                {
                    clockStr << "fixed frame rate (target: " << targetFPS;
                    clockStr << " actual: " << OpenSteer::round(smoothedFPS) << ", ";

                    OpenSteer::Vec3 sp;
                    sp = screenLocation;
                    sp.x += cw * (int) clockStr.tellp ();

                    // create usage description character string
                    std::ostringstream xxxStr;
                    xxxStr << std::setprecision (0)
                           << std::setiosflags (std::ios::fixed)
                           << "usage: " << OpenSteerDemo::clock.getSmoothedUsage ()
                           << "%"
                           << std::ends;

                    const int usageLength = ((int) xxxStr.tellp ()) - 1;
                    for (int i = 0; i < usageLength; i++) clockStr << " ";
                    clockStr << ")";

                    // display message in lower left corner of window
                    // (draw in red if the instantaneous usage is 100% or more)
                    const float usage = OpenSteerDemo::clock.getUsage ();
                    const OpenSteer::Color color = (usage >= 100) ? OpenSteer::gRed : OpenSteer::gWhite;
                    draw2dTextAt2dLocation (xxxStr, sp, color, drawGetWindowWidth(), drawGetWindowHeight());
                }
            }
            if (OpenSteerDemo::clock.getPausedState ())
                clockStr << " [paused]";
            clockStr << std::ends;
            draw2dTextAt2dLocation (clockStr, screenLocation, OpenSteer::gWhite, drawGetWindowWidth(), drawGetWindowHeight());

            // get smoothed phase timer information
            const float ptd = OpenSteerDemo::phaseTimerDraw();
            const float ptu = OpenSteerDemo::phaseTimerUpdate();
            const float pto = OpenSteerDemo::phaseTimerOverhead();
            const float smoothRate = OpenSteerDemo::clock.getSmoothingRate ();
            OpenSteer::blendIntoAccumulator (smoothRate, ptd, gSmoothedTimerDraw);
            OpenSteer::blendIntoAccumulator (smoothRate, ptu, gSmoothedTimerUpdate);
            OpenSteer::blendIntoAccumulator (smoothRate, pto, gSmoothedTimerOverhead);

            // display phase timer information
            screenLocation.y += lh * 4;
            std::ostringstream timerStr;
            timerStr << "update: ";
            writePhaseTimerReportToStream (gSmoothedTimerUpdate, timerStr);
            timerStr << "draw:   ";
            writePhaseTimerReportToStream (gSmoothedTimerDraw, timerStr);
            timerStr << "other:  ";
            writePhaseTimerReportToStream (gSmoothedTimerOverhead, timerStr);
            timerStr << std::ends;
            draw2dTextAt2dLocation (timerStr, screenLocation, OpenSteer::gGreen, drawGetWindowWidth(), drawGetWindowHeight());
        }
    }


    // ------------------------------------------------------------------------
    // cycle through frame rate presets  (XXX move this to OpenSteerDemo)


    void 
    selectNextPresetFrameRate (void)
    {
        // note that the cases are listed in reverse order, and that 
        // the default is case 0 which causes the index to wrap around
        static int frameRatePresetIndex = 0;
        switch (++frameRatePresetIndex)
        {
        case 3: 
            // animation mode at 60 fps
            OpenSteerDemo::clock.setFixedFrameRate (60);
            OpenSteerDemo::clock.setAnimationMode (true);
            OpenSteerDemo::clock.setVariableFrameRateMode (false);
            break;
        case 2: 
            // real-time fixed frame rate mode at 60 fps
            OpenSteerDemo::clock.setFixedFrameRate (60);
            OpenSteerDemo::clock.setAnimationMode (false);
            OpenSteerDemo::clock.setVariableFrameRateMode (false);
            break;
        case 1: 
            // real-time fixed frame rate mode at 24 fps
            OpenSteerDemo::clock.setFixedFrameRate (24);
            OpenSteerDemo::clock.setAnimationMode (false);
            OpenSteerDemo::clock.setVariableFrameRateMode (false);
            break;
        case 0:
        default:
            // real-time variable frame rate mode ("as fast as possible")
            frameRatePresetIndex = 0;
            OpenSteerDemo::clock.setFixedFrameRate (0);
            OpenSteerDemo::clock.setAnimationMode (false);
            OpenSteerDemo::clock.setVariableFrameRateMode (true);
            break;
        }
    }


    // ------------------------------------------------------------------------
    // This function is called (by GLUT) each time a key is pressed.
    //
    // XXX the bulk of this should be moved to OpenSteerDemo
    //
    // parameter names commented out to prevent compiler warning from "-W"


    void 
    keyboardFunc (unsigned char key, int /*x*/, int /*y*/) 
    {
        std::ostringstream message;

        // ascii codes
        const int tab = 9;
        const int space = 32;
        const int esc = 27; // escape key

        switch (key)
        {
        // reset selected PlugIn
        case 'r':
            OpenSteerDemo::resetSelectedPlugIn ();
            message << "reset PlugIn "
                    << '"' << OpenSteerDemo::nameOfSelectedPlugIn () << '"'
                    << std::ends;
            OpenSteerDemo::printMessage (message);
            break;

        // cycle selection to next vehicle
        case 's':
            OpenSteerDemo::printMessage ("select next vehicle/agent");
            OpenSteerDemo::selectNextVehicle ();
            break;

        // camera mode cycle
        case 'c':
            OpenSteerDemo::camera.selectNextMode ();
            message << "select camera mode "
                    << '"' << OpenSteerDemo::camera.modeName () << '"' << std::ends;
            OpenSteerDemo::printMessage (message);
            break;

        // select next PlugIn
        case tab:
            OpenSteerDemo::selectNextPlugIn ();
            message << "select next PlugIn: "
                    << '"' << OpenSteerDemo::nameOfSelectedPlugIn () << '"'
                    << std::ends;
            OpenSteerDemo::printMessage (message);
            break;

        // toggle annotation state
        case 'a':
            OpenSteerDemo::printMessage (toggleAnnotationState () ?
                                                    "annotation ON" : "annotation OFF");
            break;

        // toggle run/pause state
        case space:
            OpenSteerDemo::printMessage (OpenSteerDemo::clock.togglePausedState () ?
                                                    "pause" : "run");
            break;

        // cycle through frame rate (clock mode) presets
        case 'f':
            selectNextPresetFrameRate ();
            message << "set clock to ";
            if (OpenSteerDemo::clock.getAnimationMode ())
                message << "animation mode, fixed frame rate ("
                        << OpenSteerDemo::clock.getFixedFrameRate () << " fps)";
            else
            {
                message << "real-time mode, ";
                if (OpenSteerDemo::clock.getVariableFrameRateMode ())
                    message << "variable frame rate";
                else
                    message << "fixed frame rate ("
                            << OpenSteerDemo::clock.getFixedFrameRate () << " fps)";
            }
            message << std::ends;
            OpenSteerDemo::printMessage (message);
            break;

        // print minimal help for single key commands
        case '?':
            OpenSteerDemo::keyboardMiniHelp ();
            break;

        // exit application with normal status 
        case esc:
            glutDestroyWindow (windowID);
            OpenSteerDemo::printMessage ("exit.");
            OpenSteerDemo::exit (0);

        default:
            message << "unrecognized single key command: " << key;
            message << " (" << (int)key << ")";//xxx perhaps only for debugging?
            message << std::ends;
            OpenSteerDemo::printMessage ("");
            OpenSteerDemo::printMessage (message);
            OpenSteerDemo::keyboardMiniHelp ();
        }
    }


    // ------------------------------------------------------------------------
    // handles "special" keys,
    // function keys are handled by the PlugIn
    //
    // parameter names commented out to prevent compiler warning from "-W"

    void 
    specialFunc (int key, int /*x*/, int /*y*/)
    {
        std::ostringstream message;

        switch (key)
        {
        case GLUT_KEY_F1:  OpenSteerDemo::functionKeyForPlugIn (1);  break;
        case GLUT_KEY_F2:  OpenSteerDemo::functionKeyForPlugIn (2);  break;
        case GLUT_KEY_F3:  OpenSteerDemo::functionKeyForPlugIn (3);  break;
        case GLUT_KEY_F4:  OpenSteerDemo::functionKeyForPlugIn (4);  break;
        case GLUT_KEY_F5:  OpenSteerDemo::functionKeyForPlugIn (5);  break;
        case GLUT_KEY_F6:  OpenSteerDemo::functionKeyForPlugIn (6);  break;
        case GLUT_KEY_F7:  OpenSteerDemo::functionKeyForPlugIn (7);  break;
        case GLUT_KEY_F8:  OpenSteerDemo::functionKeyForPlugIn (8);  break;
        case GLUT_KEY_F9:  OpenSteerDemo::functionKeyForPlugIn (9);  break;
        case GLUT_KEY_F10: OpenSteerDemo::functionKeyForPlugIn (10); break;
        case GLUT_KEY_F11: OpenSteerDemo::functionKeyForPlugIn (11); break;
        case GLUT_KEY_F12: OpenSteerDemo::functionKeyForPlugIn (12); break;

        case GLUT_KEY_RIGHT:
            OpenSteerDemo::clock.setPausedState (true);
            message << "single step forward (frame time: "
                    << OpenSteerDemo::clock.advanceSimulationTimeOneFrame ()
                    << ")"
                    << std::endl;
            OpenSteerDemo::printMessage (message);
            break;
        }
    }


    // ------------------------------------------------------------------------
    // Main drawing function for OpenSteerDemo application,
    // drives simulation as a side effect


    void 
    displayFunc (void)
    {
        // clear color and depth buffers
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // run simulation and draw associated graphics
        OpenSteerDemo::updateSimulationAndRedraw ();

        // draw text showing (smoothed, rounded) "frames per second" rate
        drawDisplayFPS ();

        // draw the name of the selected PlugIn
        drawDisplayPlugInName ();

        // draw the name of the camera's current mode
        drawDisplayCameraModeName ();

        // draw demo key help and plugin key help
        drawDisplayDemoHelp();

        // draw crosshairs to indicate aimpoint (xxx for debugging only?)
        // drawReticle ();

        // check for errors in drawing module, if so report and exit
        checkForDrawError ("OpenSteerDemo::updateSimulationAndRedraw");

        // double buffering, swap back and front buffers
        glFlush ();
        glutSwapBuffers();
    }