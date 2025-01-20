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
// OpenSteerDemo
//
// This class encapsulates the state of the OpenSteerDemo application and the
// services it provides to its plug-ins.  It is never instantiated, all of
// its members are static (belong to the class as a whole.)
//
// 10-04-04 bk:  put everything into the OpenSteer namespace
// 11-14-02 cwr: created 
//
//
// ----------------------------------------------------------------------------


#include "OpenSteerDemo.h"
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
// keeps track of both "real time" and "simulation time"


Clock OpenSteerDemo::clock;


// ----------------------------------------------------------------------------
// camera automatically tracks selected vehicle


Camera OpenSteerDemo::camera;


// ----------------------------------------------------------------------------
// currently selected plug-in (user can choose or cycle through them)


PlugIn* OpenSteerDemo::selectedPlugIn = NULL;


// ----------------------------------------------------------------------------
// currently selected vehicle.  Generally the one the camera follows and
// for which additional information may be displayed.  Clicking the mouse
// near a vehicle causes it to become the Selected Vehicle.


OpenSteer::AbstractVehicle* OpenSteerDemo::selectedVehicle = NULL;


// ----------------------------------------------------------------------------
// phase: identifies current phase of the per-frame update cycle


int OpenSteerDemo::phase = OpenSteerDemo::overheadPhase;


// ----------------------------------------------------------------------------
// graphical annotation: master on/off switch


bool enableAnnotation = true;


// ----------------------------------------------------------------------------
// XXX apparently MS VC6 cannot handle initialized static const members,
// XXX so they have to be initialized not-inline.


const int OpenSteerDemo::overheadPhase = 0;
const int OpenSteerDemo::updatePhase = 1;
const int OpenSteerDemo::drawPhase = 2;


// ----------------------------------------------------------------------------
// initialize OpenSteerDemo application

namespace {

    void printPlugIn (PlugIn& pi) {std::cout << " " << pi << std::endl;} // XXX

} // anonymous namespace

void 
OpenSteerDemo::initialize (void)
{
    // select the default PlugIn
    selectDefaultPlugIn ();

    {
        // XXX this block is for debugging purposes,
        // XXX should it be replaced with something permanent?

        std::cout << std::endl << "Known plugins:" << std::endl;   // xxx?
        PlugIn::applyToAll (printPlugIn);                          // xxx?
        std::cout << std::endl;                                    // xxx?

        // identify default PlugIn
        if (!selectedPlugIn) errorExit ("no default PlugIn");
        std::cout << std::endl << "Default plugin:" << std::endl;  // xxx?
        std::cout << " " << *selectedPlugIn << std::endl;          // xxx?
        std::cout << std::endl;                                    // xxx?
    }

    // initialize the default PlugIn
    openSelectedPlugIn ();
}


// ----------------------------------------------------------------------------
// main update function: step simulation forward and redraw scene


void 
OpenSteerDemo::updateSimulationAndRedraw (void)
{
    // update global simulation clock
    clock.update ();

    //  start the phase timer (XXX to accurately measure "overhead" time this
    //  should be in displayFunc, or somehow account for time outside this
    //  routine)
    initPhaseTimers ();

    // run selected PlugIn (with simulation's current time and step size)
    updateSelectedPlugIn (clock.getTotalSimulationTime (),
                          clock.getElapsedSimulationTime ());

    // redraw selected PlugIn (based on real time)
    redrawSelectedPlugIn (clock.getTotalRealTime (),
                          clock.getElapsedRealTime ());
}


// ----------------------------------------------------------------------------
// exit OpenSteerDemo with a given text message or error code


void 
OpenSteerDemo::errorExit (const char* message)
{
    printMessage (message);
#ifdef _MSC_VER
	MessageBox(0, message, "OpenSteerDemo Unfortunate Event", MB_ICONERROR);
#endif
    exit (-1);
}


void 
OpenSteerDemo::exit (int exitCode)
{
    ::exit (exitCode);
}


// ----------------------------------------------------------------------------
// select the default PlugIn


void 
OpenSteerDemo::selectDefaultPlugIn (void)
{
    PlugIn::sortBySelectionOrder ();
    selectedPlugIn = PlugIn::findDefault ();
}


// ----------------------------------------------------------------------------
// select the "next" plug-in, cycling through "plug-in selection order"


void 
OpenSteerDemo::selectNextPlugIn (void)
{
    closeSelectedPlugIn ();
    selectedPlugIn = selectedPlugIn->next ();
    openSelectedPlugIn ();
}


// ----------------------------------------------------------------------------
// handle function keys an a per-plug-in basis


void 
OpenSteerDemo::functionKeyForPlugIn (int keyNumber)
{
    selectedPlugIn->handleFunctionKeys (keyNumber);
}


// ----------------------------------------------------------------------------
// return function key help message from the selected plug-in


const char*
OpenSteerDemo::getFunctionKeyHelp(int keyNumber)
{
   return (selectedPlugIn ? selectedPlugIn->getFunctionKeyHelp(keyNumber) : NULL);
}


// ----------------------------------------------------------------------------
// return name of currently selected plug-in


const char* 
OpenSteerDemo::nameOfSelectedPlugIn (void)
{
    return (selectedPlugIn ? selectedPlugIn->name() : "no PlugIn");
}


// ----------------------------------------------------------------------------
// open the currently selected plug-in


void 
OpenSteerDemo::openSelectedPlugIn (void)
{
    camera.reset ();
    selectedVehicle = NULL;
    selectedPlugIn->open ();
}


// ----------------------------------------------------------------------------
// do a simulation update for the currently selected plug-in


void 
OpenSteerDemo::updateSelectedPlugIn (const float currentTime,
                                                const float elapsedTime)
{
    // switch to Update phase
    pushPhase (updatePhase);

    // service queued reset request, if any
    doDelayedResetPlugInXXX ();

    // if no vehicle is selected, and some exist, select the first one
    if (selectedVehicle == NULL)
    {
        const OpenSteer::AVGroup& vehicles = allVehiclesOfSelectedPlugIn();
        if (vehicles.size() > 0) selectedVehicle = vehicles.front();
    }

    // invoke selected PlugIn's Update method
    selectedPlugIn->update (currentTime, elapsedTime);

    // return to previous phase
    popPhase ();
}


// ----------------------------------------------------------------------------
// redraw graphics for the currently selected plug-in


void 
OpenSteerDemo::redrawSelectedPlugIn (const float currentTime,
                                                const float elapsedTime)
{
    // switch to Draw phase
    pushPhase (drawPhase);

    // invoke selected PlugIn's Draw method
    selectedPlugIn->redraw (currentTime, elapsedTime);

    // draw any annotation queued up during selected PlugIn's Update method
    drawAllDeferredLines ();
    drawAllDeferredCirclesOrDisks ();

    // return to previous phase
    popPhase ();
}


// ----------------------------------------------------------------------------
// close the currently selected plug-in


void 
OpenSteerDemo::closeSelectedPlugIn (void)
{
    selectedPlugIn->close ();
    selectedVehicle = NULL;
}


// ----------------------------------------------------------------------------
// reset the currently selected plug-in


void 
OpenSteerDemo::resetSelectedPlugIn (void)
{
    selectedPlugIn->reset ();
}


namespace {

    // ----------------------------------------------------------------------------
    // XXX this is used by CaptureTheFlag
    // XXX it was moved here from main.cpp on 12-4-02
    // XXX I'm not sure if this is a useful feature or a bogus hack
    // XXX needs to be reconsidered.


    bool gDelayedResetPlugInXXX = false;

} // anonymous namespace
    
    
void 
OpenSteerDemo::queueDelayedResetPlugInXXX (void)
{
    gDelayedResetPlugInXXX = true;
}


void 
OpenSteerDemo::doDelayedResetPlugInXXX (void)
{
    if (gDelayedResetPlugInXXX)
    {
        resetSelectedPlugIn ();
        gDelayedResetPlugInXXX = false;
    }
}


// ----------------------------------------------------------------------------
// return a group (an STL vector of AbstractVehicle pointers) of all
// vehicles(/agents/characters) defined by the currently selected PlugIn


const OpenSteer::AVGroup& 
OpenSteerDemo::allVehiclesOfSelectedPlugIn (void)
{
    return selectedPlugIn->allVehicles ();
}


// ----------------------------------------------------------------------------
// select the "next" vehicle: the one listed after the currently selected one
// in allVehiclesOfSelectedPlugIn


void 
OpenSteerDemo::selectNextVehicle (void)
{
    if (selectedVehicle != NULL)
    {
        // get a container of all vehicles
        const OpenSteer::AVGroup& all = allVehiclesOfSelectedPlugIn ();
        const OpenSteer::AVIterator first = all.begin();
        const OpenSteer::AVIterator last = all.end();

        // find selected vehicle in container
        const OpenSteer::AVIterator s = std::find (first, last, selectedVehicle);

        // if we are at the end of the container, select the first vehicle
        if (s == last - 1) 
        {
           selectedVehicle = *first; 
           return;
        }

        // if the search failed, use NULL
        if (s == last) 
        {
           selectedVehicle = NULL; 
           return;
        }

        // normally select the next vehicle in container
        selectedVehicle = *(s + 1);

    }
}


// ----------------------------------------------------------------------------
// select vehicle nearest the given screen position (e.g.: of the mouse)


void 
OpenSteerDemo::selectVehicleNearestScreenPosition (int x, int y)
{
    selectedVehicle = findVehicleNearestScreenPosition (x, y);
}


// ----------------------------------------------------------------------------
// Find the AbstractVehicle whose screen position is nearest the current the
// mouse position.  Returns NULL if mouse is outside this window or if
// there are no AbstractVehicle.


OpenSteer::AbstractVehicle* 
OpenSteerDemo::vehicleNearestToMouse (void)
{
    return (mouseInWindow ? 
            findVehicleNearestScreenPosition (mouseX, mouseY) :
            NULL);
}


// ----------------------------------------------------------------------------
// Find the AbstractVehicle whose screen position is nearest the given window
// coordinates, typically the mouse position.  Returns NULL if there are no
// AbstractVehicles.
//
// This works by constructing a line in 3d space between the camera location
// and the "mouse point".  Then it measures the distance from that line to the
// centers of each AbstractVehicle.  It returns the AbstractVehicle whose
// distance is smallest.
//
// xxx Issues: Should the distanceFromLine test happen in "perspective space"
// xxx or in "screen space"?  Also: I think this would be happy to select a
// xxx vehicle BEHIND the camera location.


OpenSteer::AbstractVehicle* 
OpenSteerDemo::findVehicleNearestScreenPosition (int x, int y)
{
    // find the direction from the camera position to the given pixel
    const OpenSteer::Vec3 direction = directionFromCameraToScreenPosition (x, y, glutGet (GLUT_WINDOW_HEIGHT));

    // iterate over all vehicles to find the one whose center is nearest the
    // "eye-mouse" selection line
    float minDistance = FLT_MAX;       // smallest distance found so far
    OpenSteer::AbstractVehicle* nearest = NULL;   // vehicle whose distance is smallest
    const OpenSteer::AVGroup& vehicles = allVehiclesOfSelectedPlugIn();
    for (OpenSteer::AVIterator i = vehicles.begin(); i != vehicles.end(); i++)
    {
        // distance from this vehicle's center to the selection line:
        const float d = distanceFromLine ((**i).position(),
                                          camera.position(),
                                          direction);

        // if this vehicle-to-line distance is the smallest so far,
        // store it and this vehicle in the selection registers.
        if (d < minDistance)
        {
            minDistance = d;
            nearest = *i;
        }
    }

    return nearest;
}


// ----------------------------------------------------------------------------
// for storing most recent mouse state


int OpenSteerDemo::mouseX = 0;
int OpenSteerDemo::mouseY = 0;
bool OpenSteerDemo::mouseInWindow = false;


// ----------------------------------------------------------------------------
// set a certain initial camera state used by several plug-ins


void 
OpenSteerDemo::init3dCamera (OpenSteer::AbstractVehicle* selected)
{
    init3dCamera (selected, cameraTargetDistance, camera2dElevation);
}

void 
OpenSteerDemo::init3dCamera (OpenSteer::AbstractVehicle* selected,
                                  float distance,
                                  float elevation)
{
    position3dCamera (selected, distance, elevation);
    camera.fixedDistDistance = distance;
    camera.fixedDistVOffset = elevation;
    camera.mode = Camera::cmFixedDistanceOffset;
}


void 
OpenSteerDemo::init2dCamera (OpenSteer::AbstractVehicle* selected)
{
    init2dCamera (selected, cameraTargetDistance, camera2dElevation);
}

void 
OpenSteerDemo::init2dCamera (OpenSteer::AbstractVehicle* selected,
                                  float distance,
                                  float elevation)
{
    position2dCamera (selected, distance, elevation);
    camera.fixedDistDistance = distance;
    camera.fixedDistVOffset = elevation;
    camera.mode = Camera::cmFixedDistanceOffset;
}


void 
OpenSteerDemo::position3dCamera (OpenSteer::AbstractVehicle* selected)
{
    position3dCamera (selected, cameraTargetDistance, camera2dElevation);
}

void 
OpenSteerDemo::position3dCamera (OpenSteer::AbstractVehicle* selected,
                                            float distance,
                                            float /*elevation*/)
{
    selectedVehicle = selected;
    if (selected != NULL)
    {
        const OpenSteer::Vec3 behind = selected->forward() * -distance;
        camera.setPosition (selected->position() + behind);
        camera.target = selected->position();
    }
}


void 
OpenSteerDemo::position2dCamera (OpenSteer::AbstractVehicle* selected)
{
    position2dCamera (selected, cameraTargetDistance, camera2dElevation);
}

void 
OpenSteerDemo::position2dCamera (OpenSteer::AbstractVehicle* selected,
                                            float distance,
                                            float elevation)
{
    // position the camera as if in 3d:
    position3dCamera (selected, distance, elevation);

    // then adjust for 3d:
    OpenSteer::Vec3 position3d = camera.position();
    position3d.y += elevation;
    camera.setPosition (position3d);
}


// ----------------------------------------------------------------------------
// camera updating utility used by several plug-ins


void 
OpenSteerDemo::updateCamera (const float currentTime,
                                        const float elapsedTime,
                                        const OpenSteer::AbstractVehicle* selected)
{
    camera.vehicleToTrack = selected;
    camera.update (currentTime, elapsedTime, clock.getPausedState ());
}


// ----------------------------------------------------------------------------
// some camera-related default constants


const float OpenSteerDemo::camera2dElevation = 8;
const float OpenSteerDemo::cameraTargetDistance = 13;
const OpenSteer::Vec3 OpenSteerDemo::cameraTargetOffset (0, OpenSteerDemo::camera2dElevation, 
                                                                    0);


// ----------------------------------------------------------------------------
// ground plane grid-drawing utility used by several plug-ins


void 
OpenSteerDemo::gridUtility (const OpenSteer::Vec3& gridTarget)
{
    // round off target to the nearest multiple of 2 (because the
    // checkboard grid with a pitch of 1 tiles with a period of 2)
    // then lower the grid a bit to put it under 2d annotation lines
    const OpenSteer::Vec3 gridCenter ((round (gridTarget.x * 0.5f) * 2),
                           (round (gridTarget.y * 0.5f) * 2) - .05f,
                           (round (gridTarget.z * 0.5f) * 2));

    // colors for checkboard
    const OpenSteer::Color gray1(0.27f);
    const OpenSteer::Color gray2(0.30f);

    // draw 50x50 checkerboard grid with 50 squares along each side
    drawXZCheckerboardGrid (50, 50, gridCenter, gray1, gray2);

    // alternate style
    // drawXZLineGrid (50, 50, gridCenter, gBlack);
}


// ----------------------------------------------------------------------------
// draws a gray disk on the XZ plane under a given vehicle


void 
OpenSteerDemo::highlightVehicleUtility (const OpenSteer::AbstractVehicle* vehicle)
{
   if (vehicle != NULL)
   {
      drawXZDisk(vehicle->radius(), vehicle->position(), OpenSteer::gGray60, 20);
   }
}


// ----------------------------------------------------------------------------
// draws a gray circle on the XZ plane under a given vehicle


void 
OpenSteerDemo::circleHighlightVehicleUtility (const OpenSteer::AbstractVehicle* vehicle)
{
   if (vehicle != NULL)
   {
      drawXZCircle(vehicle->radius() * 1.1f,
         vehicle->position(),
         OpenSteer::gGray60,
         20);
   }
}


// ----------------------------------------------------------------------------
// draw a box around a vehicle aligned with its local space
// xxx not used as of 11-20-02


void 
OpenSteerDemo::drawBoxHighlightOnVehicle (const OpenSteer::AbstractVehicle* v,
                                               const OpenSteer::Color& color)
{
    if (v != NULL)
    {
        const float diameter = v->radius() * 2;
        const OpenSteer::Vec3 size (diameter, diameter, diameter);
        drawBoxOutline (*v, size, color);
    }
}


// ----------------------------------------------------------------------------
// draws a colored circle (perpendicular to view axis) around the center
// of a given vehicle.  The circle's radius is the vehicle's radius times
// radiusMultiplier.


void 
OpenSteerDemo::drawCircleHighlightOnVehicle (const OpenSteer::AbstractVehicle* v,
                                                  const float radiusMultiplier,
                                                  const OpenSteer::Color& color)
{
    if (v != NULL)
    {
        const OpenSteer::Vec3& cPosition = camera.position();
        draw3dCircle  (v->radius() * radiusMultiplier,  // adjusted radius
                       v->position(),                   // center
                       v->position() - cPosition,       // view axis
                       color,                          // drawing color
                       20);                            // circle segments
    }
}


// ----------------------------------------------------------------------------


void 
OpenSteerDemo::printMessage (const char* message)
{
    std::cout << "OpenSteerDemo: " <<  message << std::endl << std::flush;
}


void 
OpenSteerDemo::printMessage (const std::ostringstream& message)
{
    printMessage (message.str().c_str());
}


void 
OpenSteerDemo::printWarning (const char* message)
{
    std::cout << "OpenSteerDemo: Warning: " <<  message << std::endl << std::flush;
}


void 
OpenSteerDemo::printWarning (const std::ostringstream& message)
{
    printWarning (message.str().c_str());
}


// ------------------------------------------------------------------------
// print list of known commands
//
// XXX this list should be assembled automatically,
// XXX perhaps from a list of "command" objects created at initialization


void 
OpenSteerDemo::keyboardMiniHelp (void)
{
    printMessage ("");
    printMessage ("defined single key commands:");
    printMessage (getDemoKeyHelp(0));
    printMessage (getDemoKeyHelp(1));
    printMessage (getDemoKeyHelp(2));
    printMessage (getDemoKeyHelp(3));
    printMessage (getDemoKeyHelp(4));
    printMessage (getDemoKeyHelp(5));
    printMessage (getDemoKeyHelp(6));
    printMessage (getDemoKeyHelp(7));
    printMessage (getDemoKeyHelp(8));
    printMessage (getDemoKeyHelp(9));
    printMessage ("");

    // allow PlugIn to print mini help for the function keys it handles
    selectedPlugIn->printMiniHelpForFunctionKeys ();
}

// ----------------------------------------------------------------------------
// returns nth help string from demo


const char* OpenSteerDemo::getDemoKeyHelp(int nth)
{
   switch (nth)
   {
   case 0:   return "  r      restart current PlugIn.";
   case 1:   return "  s      select next vehicle.";
   case 2:   return "  c      select next camera mode.";
   case 3:   return "  f      select next preset frame rate";
   case 4:   return "  Tab    select next PlugIn.";
   case 5:   return "  a      toggle annotation on/off.";
   case 6:   return "  Space  toggle between Run and Pause.";
   case 7:   return "  ->     step forward one frame.";
   case 8:   return "  ?      print mini-help in console";
   case 9:   return "  Esc    exit.";
   }

   return NULL;
}

float OpenSteerDemo::drawGetWindowHeight(void)
{
   return 1.0f;
}

float OpenSteerDemo::drawGetWindowWidth(void)
{
   return 1.0f;
}
// ----------------------------------------------------------------------------
// manage OpenSteerDemo phase transitions (xxx and maintain phase timers)


int OpenSteerDemo::phaseStackIndex = 0;
const int OpenSteerDemo::phaseStackSize = 5;
int OpenSteerDemo::phaseStack [OpenSteerDemo::phaseStackSize];

bool updatePhaseActive = false;
bool drawPhaseActive = false;


void 
OpenSteerDemo::pushPhase (const int newPhase)
{
    updatePhaseActive = newPhase == OpenSteerDemo::updatePhase;
    drawPhaseActive = newPhase == OpenSteerDemo::drawPhase;

    // update timer for current (old) phase: add in time since last switch
    updatePhaseTimers ();

    // save old phase
    phaseStack[phaseStackIndex++] = phase;

    // set new phase
    phase = newPhase;

    // check for stack overflow
    if (phaseStackIndex >= phaseStackSize) errorExit ("phaseStack overflow");
}


void 
OpenSteerDemo::popPhase (void)
{
    // update timer for current (old) phase: add in time since last switch
    updatePhaseTimers ();

    // restore old phase
    phase = phaseStack[--phaseStackIndex];
    updatePhaseActive = phase == OpenSteerDemo::updatePhase;
    drawPhaseActive = phase == OpenSteerDemo::drawPhase;
}


// ----------------------------------------------------------------------------


float OpenSteerDemo::phaseTimerBase = 0;
float OpenSteerDemo::phaseTimers [drawPhase+1];


void 
OpenSteerDemo::initPhaseTimers (void)
{
    phaseTimers[drawPhase] = 0;
    phaseTimers[updatePhase] = 0;
    phaseTimers[overheadPhase] = 0;
    phaseTimerBase = clock.getTotalRealTime ();
}


void 
OpenSteerDemo::updatePhaseTimers (void)
{
    const float currentRealTime = clock.realTimeSinceFirstClockUpdate();
    phaseTimers[phase] += currentRealTime - phaseTimerBase;
    phaseTimerBase = currentRealTime;
}
