//
//  UG3GridDisplay.h
//  ug3-interface
//
//  Created by Allen Munk on 11/21/22.
//

#ifndef UG3GridDisplay_h
#define UG3GridDisplay_h

#include <stdio.h>
#include <VisualizerWindowHeaders.h>

#include "ColourScheme.h"

namespace UG3Interface {

class UG3InterfaceCanvas;

class Electrode : public Component
{
public:
    Electrode() : c(Colours::grey) { }

    void setColour(Colour c);

    void paint(Graphics& g);

private:
    Colour c;
};

class UG3GridDisplay : public Component{
public:
    UG3GridDisplay(UG3InterfaceCanvas* canvas, Viewport* viewport, int numChannels);
    
    void updateDisplayDimensions();
    
    void refresh(const float * peakToPeakValues, int const maxValue);
    
private:
    UG3InterfaceCanvas* canvas;
    Viewport* viewport;
    int numChannels;
    OwnedArray<Electrode> electrodes;

    
};
}

#endif /* UG3GridDisplay_h */
