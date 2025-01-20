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
// glutInitialize
//
// 
// 
// 
//
// 01-19-25 sak: Created
// 
//
//
// ----------------------------------------------------------------------------


#include "glutInitialize.h"
#include "glutCallbacks.h"
#include "Annotation.h"
#include "Color.h"
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

// ----------------------------------------------------------------------------
// do all initialization related to graphics


void 
OpenSteer::initializeGraphics (int argc, char **argv)
{
    // initialize GLUT state based on command line arguments
    glutInit (&argc, argv);  

    // display modes: RGB+Z and double buffered
    unsigned int mode = GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE;
    glutInitDisplayMode (mode);

    // create and initialize our window with GLUT tools
    // (center window on screen with size equal to "ws" times screen size)
    const int sw = glutGet (GLUT_SCREEN_WIDTH);
    const int sh = glutGet (GLUT_SCREEN_HEIGHT);
    const float ws = 0.8f; // window_size / screen_size
    const int ww = (int) (sw * ws);
    const int wh = (int) (sh * ws);
    glutInitWindowPosition ((int) (sw * (1-ws)/2), (int) (sh * (1-ws)/2));
    glutInitWindowSize (ww, wh);
    windowID = glutCreateWindow (getAppVersion());
    reshapeFunc (ww, wh);
    initGL ();

    // register our display function, make it the idle handler too
    glutDisplayFunc (displayFunc);  
    glutIdleFunc (displayFunc);

    // register handler for window reshaping
    glutReshapeFunc (reshapeFunc);

    // register handler for keyboard events
    glutKeyboardFunc (keyboardFunc);
    glutSpecialFunc (specialFunc);

    // register handler for mouse button events
    glutMouseFunc (mouseButtonFunc);

    // register handler to track mouse motion when any button down
    glutMotionFunc (mouseMotionFunc);

    // register handler to track mouse motion when no buttons down
    glutPassiveMotionFunc (mousePassiveMotionFunc);

    // register handler for when mouse enters or exists the window
    glutEntryFunc (mouseEnterExitWindowFunc);
}


// ----------------------------------------------------------------------------
// run graphics event loop


void 
OpenSteer::runGraphics (void)
{
    glutMainLoop ();  
}



// ----------------------------------------------------------------------------
// accessors for GLUT's window dimensions


float 
OpenSteer::drawGetWindowHeight (void) 
{
    return (float)glutGet (GLUT_WINDOW_HEIGHT);
}


float 
OpenSteer::drawGetWindowWidth  (void) 
{
    return (float)glutGet (GLUT_WINDOW_WIDTH);
}


