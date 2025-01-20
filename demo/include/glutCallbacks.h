

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
// Defines the callbacks glut needs.
// 
//
// 01-19-25 sak:  created
// 
//  
//
//
// ----------------------------------------------------------------------------


#ifndef OPENSTEER_GLUTCALLBACKS_H
#define OPENSTEER_GLUTCALLBACKS_H


const char* getAppVersion(void);
extern int windowID;

void
initGL(void);





// ----------------------------------------------------------------------------
// handler for window resizing


void
reshapeFunc(int width, int height);

// ----------------------------------------------------------------------------
// This is called (by GLUT) each time a mouse button pressed or released.


void
mouseButtonFunc(int button, int state, int x, int y);

// ----------------------------------------------------------------------------
// called when mouse moves and any buttons are down


void
mouseMotionFunc(int x, int y);

// ----------------------------------------------------------------------------
// called when mouse moves and no buttons are down


void
mousePassiveMotionFunc(int x, int y);


// ----------------------------------------------------------------------------
// called when mouse enters or exits the window


void
mouseEnterExitWindowFunc(int state);

// ----------------------------------------------------------------------------
// draw PlugI name in upper lefthand corner of screen


void
drawDisplayPlugInName(void);

// ----------------------------------------------------------------------------
// draw camera mode name in lower lefthand corner of screen


void
drawDisplayCameraModeName(void);

// ----------------------------------------------------------------------------
// draw camera mode name in lower lefthand corner of screen


void
drawDisplayDemoHelp(void);
// ----------------------------------------------------------------------------
// helper for drawDisplayFPS



//void
//writePhaseTimerReportToStream(float phaseTimer,
//   std::ostringstream& stream);
void
drawDisplayFPS(void);

// ------------------------------------------------------------------------
// cycle through frame rate presets  (XXX move this to OpenSteerDemo)


void
selectNextPresetFrameRate(void);


// ------------------------------------------------------------------------
// This function is called (by GLUT) each time a key is pressed.
//
// XXX the bulk of this should be moved to OpenSteerDemo
//
// parameter names commented out to prevent compiler warning from "-W"


void
keyboardFunc(unsigned char key, int /*x*/, int /*y*/);
// ------------------------------------------------------------------------
// handles "special" keys,
// function keys are handled by the PlugIn
//
// parameter names commented out to prevent compiler warning from "-W"

void
specialFunc(int key, int /*x*/, int /*y*/);


// ------------------------------------------------------------------------
// Main drawing function for OpenSteerDemo application,
// drives simulation as a side effect


void
displayFunc(void);
// ----------------------------------------------------------------------------
#endif // OPENSTEER_GLUTCALLBACKS_H
