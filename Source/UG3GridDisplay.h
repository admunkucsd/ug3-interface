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
#include <stack>
#include <map>
#include <set>

#include "ColourScheme.h"

namespace UG3Interface {

class UG3InterfaceCanvas;

class Electrode : public Component
{
public:
    Electrode() : c(Colours::grey) { }

    void setColour(Colour c);
    
    Colour getColour();

    void paint(Graphics& g);

private:
    Colour c;
};



class UG3GridDisplay : public Component{
public:
    UG3GridDisplay(UG3InterfaceCanvas* canvas, Viewport* viewport, int numChannelsX, int numChannelsY, int numSections, int maxSelectedChannels);
        
    void refresh(const float * peakToPeakValues, int const maxValue);
    
    void resized();
    
    void paint(Graphics& g);
    
    std::vector<String> getPreconfigOptions(){return preconfigOptions;};
    
    std::set<int> everyOther();
    
    void selectPreconfig(int configIndex, bool isFilled);
    
    int getTotalHeight() {return totalHeight;}
    
    void updateHighlightedElectrodes(std::set<int>& newValues);
    
    void updateSelectedElectrodes (std::set<int>& newValues);
    
    void updateChannelCountLabels(bool isFinalSelection);
    
    void changeMaxSelectedChannels(int newMaxChannels);
    
    std::set<int> getSelectedElectrodeIndexes(){return selectedElectrodeIndexes;}
    
    void setDeselectState(bool isDeselectActive);
    
    class DisplayMouseListener : public Component {
    public:
        DisplayMouseListener(UG3GridDisplay* display, int numRows, int numSections);
        void mouseDown(const MouseEvent & event);
        void mouseDrag(const MouseEvent & event);
        void mouseUp(const MouseEvent & event);
        void calculateElectrodesSelected(bool isFilled = false);
        
        void paint(Graphics& g);

        
    private:
        UG3GridDisplay* display;
        ScopedPointer<Rectangle<int>> selection;
        int selectionStartX;
        int selectionStartY;
        int numRows;
        int numSections;

    };

    
protected:
    const static int LEFT_BOUND = 20;
    const static int TOP_BOUND = 20;
    const static int SPACING = 4;
    const static int HEIGHT = 8;
    const static int WIDTH = 8;
    const static int SECTION_SPACING = 9;
    int numChannelsX;
    int numChannelsY;
    int numSections;
    int maxSelectedChannels;
    
private:
    UG3InterfaceCanvas* canvas;
    Viewport* viewport;
    OwnedArray<Electrode> electrodes;
    OwnedArray<Rectangle<int>> sectionDividers;
    ScopedPointer<DisplayMouseListener> mouseListener;
    std::set<int> highlightedElectrodeIndexes;
    std::set<int> selectedElectrodeIndexes;
    
    std::vector<String> preconfigOptions;
    
    const float highlightedAlpha = float(0.7);
    
    Colour selectedColor;
    Colour highlightedColor;
    
    OwnedArray<Label> channelCountLabels;
    OwnedArray<Label> channelsRemainingLabels;
    
    int totalHeight;
    bool isDeselectActive;

    
};
}

#endif /* UG3GridDisplay_h */
