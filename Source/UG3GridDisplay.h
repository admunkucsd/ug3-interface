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
    
    void resized();
    
    void paint(Graphics& g);
    
    
    int getTotalHeight() {return totalHeight;}
    
    
    class DisplayMouseListener : public Component {
    public:
        void mouseDown(const MouseEvent & event);
        void mouseDrag(const MouseEvent & event);
        void mouseUp(const MouseEvent & event);
        void calculateElectrodesSelected();
        
        void paint(Graphics& g);

        
    private:
        ScopedPointer<Rectangle<int>> selection;
        int selectionStartX;
        int selectionStartY;

    };

    
protected:
    const static int LEFT_BOUND = 20;
    const static int TOP_BOUND = 20;
    const static int SPACING = 4;
    const static int HEIGHT = 8;
    const static int WIDTH = 8;
    int numChannels;

    
private:
    UG3InterfaceCanvas* canvas;
    Viewport* viewport;
    OwnedArray<Electrode> electrodes;
    ScopedPointer<DisplayMouseListener> mouseListener;

    int totalHeight;
    

    
};
}

#endif /* UG3GridDisplay_h */
