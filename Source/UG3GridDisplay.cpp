//
//  UG3GridDisplay.cpp
//  ug3-interface
//
//  Created by Allen Munk on 11/21/22.
//

#include "UG3GridDisplay.h"
#include "UG3InterfaceCanvas.h"
#include <queue>
#include <algorithm>

using namespace UG3Interface;

void Electrode::setColour(Colour c_)
{
    c = c_;
}

void Electrode::paint(Graphics& g)
{
    g.fillAll(c);
}

Colour Electrode::getColour() {
    return c;
}

UG3GridDisplay::UG3GridDisplay(UG3InterfaceCanvas* canvas, Viewport* viewport, int numChannels) : canvas(canvas), viewport(viewport), numChannels(numChannels), totalHeight(0){
    int newTotalHeight = 0;
    const int totalPixels = numChannels;
    selectedColor = ColourScheme::getColourForNormalizedValue(.9);
    highlightedColor = selectedColor.withAlpha(float(0.7));
    
    const int NUM_COLUMNS = sqrt(numChannels);
    
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
    
    mouseListener = new DisplayMouseListener(this, sqrt(numChannels));
    mouseListener -> setBounds(0,0, getWidth(), getHeight());
    addAndMakeVisible(mouseListener);
    mouseListener -> toFront(true);
        
    totalHeight = newTotalHeight + TOP_BOUND - SPACING;

    
}


void UG3GridDisplay::resized() {
    mouseListener = nullptr;

    
    mouseListener = new DisplayMouseListener(this, sqrt(numChannels));
    mouseListener -> setBounds(0,0, getWidth(), getHeight());
    addAndMakeVisible(mouseListener);
    mouseListener -> toFront(true);
    
}

void UG3GridDisplay::paint(Graphics& g){
    g.fillAll(Colours::darkgrey);
    
}

void UG3GridDisplay::refresh(const float * peakToPeakValues, int const maxValue) {
    int maxChan = jmin(electrodes.size(), numChannels);
    
    int count = 0;
    for (int electrodeIndex : selectedElectrodeIndexes)
    {
        electrodes[electrodeIndex]->setColour(ColourScheme::getColourForNormalizedValue((float)(peakToPeakValues[count] / maxValue)));
        count += 1;
    }
    
}

void UG3GridDisplay::updateSelectedElectrodes (std::set<int>& newValues, bool isFilled) {
    if(isFilled) {
        for(int index : newValues) {
            electrodes[index] -> setColour(selectedColor);
        }
        selectedElectrodeIndexes.insert(highlightedElectrodeIndexes.begin(), highlightedElectrodeIndexes.end());
        highlightedElectrodeIndexes.clear();
        canvas->setNodeNumChannels(selectedElectrodeIndexes);
    }
    else {
        std::set<int> diff;
        std::set_difference(highlightedElectrodeIndexes.begin(), highlightedElectrodeIndexes.end(), newValues.begin(), newValues.end(), std::inserter(diff, diff.end()));
        for(int index : diff) {
            if(electrodes[index] -> getColour() == highlightedColor)
                electrodes[index] -> setColour(Colours::grey);
        }
        diff.clear();
        
        std::set_difference(newValues.begin(), newValues.end(), highlightedElectrodeIndexes.begin(), highlightedElectrodeIndexes.end(), std::inserter(diff, diff.end()));
        
        for(int index : diff) {
            if(electrodes[index] -> getColour() == Colours::grey)
                electrodes[index] -> setColour(highlightedColor);
        }
        highlightedElectrodeIndexes = newValues;
    }

}


UG3GridDisplay::DisplayMouseListener::DisplayMouseListener(UG3GridDisplay* display, int numRows) : display(display), numRows(numRows){}

void UG3GridDisplay::DisplayMouseListener::paint(Graphics& g){
    g.fillAll(Colours::transparentWhite);
    if(selection)
        g.drawRect(*selection);
}

void UG3GridDisplay::DisplayMouseListener::calculateElectrodesSelected(bool isFilled){
    
    int maxX = LEFT_BOUND + (WIDTH + SPACING) * numRows - SPACING;
    int maxY = TOP_BOUND + (HEIGHT + SPACING) * numRows - SPACING;

    //Calculate the nearest left edge of an electrode (to the left of the selection left boundary)
    int nearestLeftEdge = selection -> getX() - LEFT_BOUND >= 0 ? (selection -> getX() - LEFT_BOUND)/(WIDTH+SPACING) : 0;
    //Calculate the number of columns selected by calculating the number of left edges present in selection from nLE to top right of selection
    int columnsSelected = selection -> getTopRight().getX() > LEFT_BOUND && selection -> getX() < maxX ? (std::min((int)selection -> getTopRight().getX(), maxX) - (LEFT_BOUND + nearestLeftEdge*(WIDTH+SPACING)))/(WIDTH+SPACING) + 1 : 0;
    
    //Calculate the nearest top edge of an electrode (to the top of selection top boundary)
    int nearestTopEdge = selection -> getY() - TOP_BOUND >= 0 ? (selection -> getY() - TOP_BOUND)/(HEIGHT+SPACING) : 0;
    //Calculate the number of rows by calculating the number of top edges from nTE to bottom left of selection
    int rowsSelected =  selection -> getBottomLeft().getY() > TOP_BOUND && selection -> getY() < maxY ? (std::min((int)selection -> getBottomLeft().getY(), maxY) - (TOP_BOUND + nearestTopEdge*(HEIGHT+SPACING)))/(HEIGHT+SPACING) + 1 : 0;
    std::set<int> electrodeIndexes;
    
    
    for(int i = 0; i < rowsSelected; i++) {
        for(int j = 0; j < columnsSelected; j++) {
            electrodeIndexes.insert(nearestLeftEdge + j + ((nearestTopEdge + i)*numRows));
        }
    }
    
    display -> updateSelectedElectrodes(electrodeIndexes, isFilled);
    
}


void UG3GridDisplay::DisplayMouseListener::mouseDown(const MouseEvent & event) {
    selectionStartX = event.x;
    selectionStartY = event.y;
    selection = new Rectangle<int>(selectionStartX,selectionStartY,0,0);
}

void UG3GridDisplay::DisplayMouseListener::mouseDrag(const MouseEvent & event) {
    if(event.x < selectionStartX) {
        selection -> setX(event.x);
        selection -> setWidth(selectionStartX - event.x);
    }
    else {
        selection -> setWidth(event.x - selectionStartX);
    }
    if(event.y < selectionStartY) {
        selection -> setY(event.y);
        selection -> setHeight(selectionStartY - event.y);
    }
    else {
        selection -> setHeight(event.y - selectionStartY);

    }
    calculateElectrodesSelected(false);
    repaint();
}

void UG3GridDisplay::DisplayMouseListener::mouseUp(const MouseEvent & event) {
    calculateElectrodesSelected(true);
    selection = nullptr;
    repaint();
}




