//
//  UG3GridDisplay.cpp
//  ug3-interface
//
//  Created by Allen Munk on 11/21/22.
//

#include "UG3GridDisplay.h"
#include "UG3InterfaceCanvas.h"

using namespace UG3Interface;

void Electrode::setColour(Colour c_)
{
    c = c_;
}

void Electrode::paint(Graphics& g)
{
    g.fillAll(c);
}

UG3GridDisplay::UG3GridDisplay(UG3InterfaceCanvas* canvas, Viewport* viewport, int numChannels) : canvas(canvas), viewport(viewport), numChannels(numChannels), totalHeight(0){
}

void UG3GridDisplay::updateDisplayDimensions()
{
    int newTotalHeight = 0;
    electrodes.clear();

    const int totalPixels = numChannels;
    LOGC("Total pixels: ", totalPixels);

    const int LEFT_BOUND = 20;
    const int TOP_BOUND = 20;
    const int SPACING = 2;
    const int NUM_COLUMNS = sqrt(numChannels);
    const int HEIGHT = 8;
    const int WIDTH = 8;
    
    newTotalHeight = TOP_BOUND;
    
    for (int i = 0; i < totalPixels; i++)
    {
        Electrode* e = new Electrode();

        int column = i % NUM_COLUMNS;
        int row = i / NUM_COLUMNS;
        int L = LEFT_BOUND + column * (WIDTH + SPACING);
        int T = TOP_BOUND + row * (HEIGHT + SPACING);

        if(column == 0)
            newTotalHeight += HEIGHT + SPACING;
        
        e->setBounds(L,
            T,
            WIDTH,
            HEIGHT);

        canvas->addAndMakeVisible(e);
        electrodes.add(e);
    }
    
    totalHeight = newTotalHeight + TOP_BOUND - SPACING;

}

void UG3GridDisplay::resized() {
    int newTotalHeight = 0;
    electrodes.clear();

    const int totalPixels = numChannels;
    LOGC("Total pixels: ", totalPixels);

    const int LEFT_BOUND = 20;
    const int TOP_BOUND = 20;
    const int SPACING = 2;
    const int NUM_COLUMNS = sqrt(numChannels);
    const int HEIGHT = 8;
    const int WIDTH = 8;
    
    newTotalHeight = TOP_BOUND;
    
    for (int i = 0; i < totalPixels; i++)
    {
        Electrode* e = new Electrode();

        int column = i % NUM_COLUMNS;
        int row = i / NUM_COLUMNS;
        int L = LEFT_BOUND + column * (WIDTH + SPACING);
        int T = TOP_BOUND + row * (HEIGHT + SPACING);

        if(column == 0)
            newTotalHeight += HEIGHT + SPACING;
        
        e->setBounds(L,
            T,
            WIDTH,
            HEIGHT);

        addAndMakeVisible(e);
        electrodes.add(e);
    }
    
    totalHeight = newTotalHeight + TOP_BOUND - SPACING;
}

void UG3GridDisplay::paint(Graphics& g){
    g.fillAll(Colours::darkgrey);
}

void UG3GridDisplay::refresh(const float * peakToPeakValues, int const maxValue) {
    int maxChan = jmin(electrodes.size(), numChannels);
    
    for (int i = 0; i < maxChan; i++)
    {
        electrodes[i]->setColour(ColourScheme::getColourForNormalizedValue((float)(peakToPeakValues[i] / maxValue)));
    }

    
    
}




