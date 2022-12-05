//
//  UG3GridDisplay.cpp
//  ug3-interface
//
//  Created by Allen Munk on 11/21/22.
//

#include "UG3GridDisplay.h"
#include "UG3InterfaceCanvas.h"
#include <queue>

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
    mouseListener = nullptr;
    const int totalPixels = numChannels;
    LOGC("Total pixels: ", totalPixels);

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
    mouseListener = new DisplayMouseListener();
    mouseListener -> setBounds(0,0, getWidth(), getHeight());
    addAndMakeVisible(mouseListener);
    mouseListener -> toFront(true);
    
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

void UG3GridDisplay::DisplayMouseListener::paint(Graphics& g){
    g.fillAll(Colours::transparentWhite);
    if(selection)
        g.drawRect(*selection);
}

void UG3GridDisplay::DisplayMouseListener::calculateElectrodesSelected(){
    int nearestLeftEdge = selection -> getX() - LEFT_BOUND >= 0 ? (selection -> getX() - LEFT_BOUND)/(WIDTH+SPACING) : 0;
    int columnsSelected = selection -> getTopRight().getX() > LEFT_BOUND ? (selection -> getTopRight().getX() - (LEFT_BOUND + nearestLeftEdge*(WIDTH+SPACING)))/(WIDTH+SPACING) + 1 : 0;
    int nearestTopEdge = selection -> getY() - TOP_BOUND >= 0 ? (selection -> getY() - TOP_BOUND)/(HEIGHT+SPACING) : 0;
    int rowsSelected =  selection -> getBottomLeft().getY() > TOP_BOUND ? (selection -> getBottomLeft().getY() - (TOP_BOUND + nearestTopEdge*(HEIGHT+SPACING)))/(HEIGHT+SPACING) + 1 : 0;
    std::priority_queue<int> electrodeIndexes;
    
    
    for(int i = 0; i < rowsSelected; i++) {
        for(int j = 0; j < columnsSelected; j++) {
            electrodeIndexes.push(nearestLeftEdge + j + (nearestTopEdge + i*16));
        }
    }
    
    std::cout << "NLE: " << nearestLeftEdge << " NTE: " << nearestTopEdge << " cS: " << columnsSelected << " rS " << rowsSelected <<std::endl;
    std::cout << "Priority queue is: ";
    while(electrodeIndexes.empty() == false) {
        std::cout << electrodeIndexes.top() << " ";
        electrodeIndexes.pop();
    }
    std::cout << std::endl;
    
}


void UG3GridDisplay::DisplayMouseListener::mouseDown(const MouseEvent & event) {
    std::cout << "mouse down" << std::endl;
    selectionStartX = event.x;
    selectionStartY = event.y;
    selection = new Rectangle<int>(selectionStartX,selectionStartY,0,0);
}

void UG3GridDisplay::DisplayMouseListener::mouseDrag(const MouseEvent & event) {
    selection -> setWidth(event.x - selectionStartX);
    selection -> setHeight(event.y - selectionStartY);
    
    std::cout << "coords: " << selection->getX() << " " << selection->getY() << " " << selection -> getWidth() << " " << selection -> getHeight() << std::endl;
    repaint();
}

void UG3GridDisplay::DisplayMouseListener::mouseUp(const MouseEvent & event) {
    std::cout << "mouse up" << std::endl;

    calculateElectrodesSelected();
    selection = nullptr;
    repaint();
}




