

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
// Initialize glut and set callbacks.
// 
//
// 01-19-25 sak:  Break functions to a new file
//
//
// ----------------------------------------------------------------------------


#ifndef OPENSTEER_GLUTINITIALIZE_H
#define OPENSTEER_GLUTINITIALIZE_H

    // ----------------------------------------------------------------------------
    // do all initialization related to graphics


    void initializeGraphics (int argc, char **argv);


    // ----------------------------------------------------------------------------
    // run graphics event loop


    void runGraphics (void);


    // ----------------------------------------------------------------------------
    // accessors for GLUT's window dimensions


    float drawGetWindowHeight (void);
    float drawGetWindowWidth (void);


// ----------------------------------------------------------------------------
#endif // OPENSTEER_GLUTINITIALIZE_H
