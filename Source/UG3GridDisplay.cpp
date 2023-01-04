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

UG3GridDisplay::UG3GridDisplay(UG3InterfaceCanvas* canvas, Viewport* viewport, int numChannelsX, int numChannelsY, int numSections, int maxSelectedChannels) : canvas(canvas), viewport(viewport), numChannelsX(numChannelsX), numChannelsY(numChannelsY), numSections(numSections), totalHeight(0), maxSelectedChannels(maxSelectedChannels), isDeselectActive(false){
    int newTotalHeight = 0;
    const int totalPixels = numChannelsX * numChannelsY;
    selectedColor = ColourScheme::getColourForNormalizedValue(.9);
    highlightedColor = selectedColor.withAlpha(highlightedAlpha);
        
    newTotalHeight = TOP_BOUND;
    
    for (int i = 0; i < totalPixels; i++)
    {
        Electrode* e = new Electrode();

        int column = i % numChannelsX;
        int row = i / numChannelsX;
        int L = LEFT_BOUND + column * (WIDTH + SPACING) + (SECTION_SPACING - SPACING) * (column/(numChannelsX/numSections));
        int T = TOP_BOUND + row * (HEIGHT + SPACING);
        
        if(column % (numChannelsX/numSections) == 0 && row == 0){
            int index = column /(numChannelsX/numSections);
            Label* newLabel = new Label(String("Channel Count ") + String(index), String(0)+String("/")+String(maxSelectedChannels/numSections));
            newLabel->setFont(Font("Small Text", 16, Font::plain));
            newLabel->setColour(Label::textColourId, Colours::white);
            newLabel->setBounds(L, TOP_BOUND/4, 65, TOP_BOUND * 3/4);
            addAndMakeVisible(newLabel);
            channelCountLabels.add(newLabel);
        }
        
        if(column % (numChannelsX/numSections) == (numChannelsX/numSections)*3/4 && row == 0){
            int index = column /(numChannelsX/numSections);
            Label* newLabel = new Label(String("Channels Remaining ") + String(index), String(maxSelectedChannels/numSections));
            newLabel->setFont(Font("Small Text", 16, Font::plain));
            newLabel->setColour(Label::textColourId, Colours::green);
            newLabel->setBounds(L, TOP_BOUND/4, 65, TOP_BOUND * 3/4);
            addAndMakeVisible(newLabel);
            channelsRemainingLabels.add(newLabel);
        }
            

        if(column == 0)
            newTotalHeight += HEIGHT + SPACING;
        
        e->setBounds(L,
            T,
            WIDTH,
            HEIGHT);

        addAndMakeVisible(e);
        electrodes.add(e);
    }
    
    for( int i = 0; i < numSections - 1; i++ ) {
        int startX = LEFT_BOUND + numChannelsX/numSections * (WIDTH + SECTION_SPACING/2) + i * (SECTION_SPACING/2 + (SECTION_SPACING%2 ? 1 : 0) + ((WIDTH + SPACING) * numChannelsX/numSections));
        int dividerHeight = TOP_BOUND + numChannelsY * (HEIGHT+SPACING) - SPACING;
        Rectangle<int> * sectionDivider = new Rectangle<int>(startX, TOP_BOUND/2, 1, dividerHeight);
        sectionDividers.add(sectionDivider);
    }
    
    mouseListener = new DisplayMouseListener(this, numChannelsX, numSections);
    mouseListener -> setBounds(0,0, getWidth(), getHeight());
    addAndMakeVisible(mouseListener);
    mouseListener -> toFront(true);
        
    totalHeight = newTotalHeight + TOP_BOUND - SPACING;

    
}


void UG3GridDisplay::resized() {
    mouseListener = nullptr;

    
    mouseListener = new DisplayMouseListener(this, numChannelsX, numSections);
    mouseListener -> setBounds(0,0, getWidth(), getHeight());
    addAndMakeVisible(mouseListener);
    mouseListener -> toFront(true);
    
}

void UG3GridDisplay::paint(Graphics& g){
    g.fillAll(Colours::darkgrey);
    for(Rectangle<int>* divider: sectionDividers) {
        g.drawRect(*divider);
    }
    
}

void UG3GridDisplay::refresh(const float * peakToPeakValues, int const maxValue) {
    
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
            if(isDeselectActive) {
                electrodes[index] -> setColour(Colours::grey);
            }
            else {
                electrodes[index] -> setColour(selectedColor);
            }
        }
        if(isDeselectActive) {
            std::set<int> diff;
            std::set_difference(selectedElectrodeIndexes.begin(), selectedElectrodeIndexes.end(), highlightedElectrodeIndexes.begin(), highlightedElectrodeIndexes.end(), std::inserter(diff, diff.end()));
            selectedElectrodeIndexes = diff;
        }
        else {
            selectedElectrodeIndexes.insert(highlightedElectrodeIndexes.begin(), highlightedElectrodeIndexes.end());
        }
        highlightedElectrodeIndexes.clear();
        updateChannelCountLabels(true);
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
        updateChannelCountLabels(false);
    }

}


void UG3GridDisplay::updateChannelCountLabels(bool isFinalSelection){
    if(isFinalSelection) {
        std::vector<int> counts(numSections, 0);
        for(int index: selectedElectrodeIndexes) {
            counts[(index % numChannelsX)/(numChannelsX/numSections)] += 1;
        }
        int labelIndex = 0;
        for(int count : counts) {
            channelCountLabels[labelIndex] -> setText(String(count)+String("/")+String(maxSelectedChannels/numSections),dontSendNotification);
            labelIndex += 1;
        }
    }
    else {
        std::vector<int> counts(numSections, 0);
        std::set<int> totalSelected;
        if(isDeselectActive) {
            std::set_difference(selectedElectrodeIndexes.begin(), selectedElectrodeIndexes.end(), highlightedElectrodeIndexes.begin(), highlightedElectrodeIndexes.end(), std::inserter(totalSelected, totalSelected.end()));
        }
        else {
            std::merge(highlightedElectrodeIndexes.begin(), highlightedElectrodeIndexes.end(), selectedElectrodeIndexes.begin(), selectedElectrodeIndexes.end(), std::inserter(totalSelected, totalSelected.begin()));
        }
        for(int index: totalSelected) {
            counts[(index % numChannelsX)/(numChannelsX/numSections)] += 1;
        }
        int labelIndex = 0;
        for(int count : counts) {
            int remaining = maxSelectedChannels/numSections - count;
            channelsRemainingLabels[labelIndex] -> setText(String(remaining),dontSendNotification);
            if (remaining < 0) {
                channelsRemainingLabels[labelIndex] ->setColour(Label::textColourId, Colours::red);
            }
            else {
                channelsRemainingLabels[labelIndex] ->setColour(Label::textColourId, Colours::green);
            }
            labelIndex += 1;
        }
    }
}

void UG3GridDisplay::changeMaxSelectedChannels(int newMaxChannels){
    maxSelectedChannels=newMaxChannels;
    //FIXME: improve method to allow for both sets of labels to be updated
    updateChannelCountLabels(true);
    updateChannelCountLabels(false);
    repaint();
}

void UG3GridDisplay::setDeselectState(bool isDeselectActive){
    this->isDeselectActive = isDeselectActive;
}

UG3GridDisplay::DisplayMouseListener::DisplayMouseListener(UG3GridDisplay* display, int numRows, int numSections) : display(display), numRows(numRows), numSections(numSections) {}

void UG3GridDisplay::DisplayMouseListener::paint(Graphics& g){
    g.fillAll(Colours::transparentWhite);
    if(selection)
        g.drawRect(*selection);
}

void UG3GridDisplay::DisplayMouseListener::calculateElectrodesSelected(bool isFilled){
    
    int maxX = LEFT_BOUND + (WIDTH + SPACING) * numRows - SPACING + (numSections - 1) * (SECTION_SPACING - SPACING);
    int maxY = TOP_BOUND + (HEIGHT + SPACING) * numRows - SPACING;

    int colsPerSection = numRows/numSections;
    int widthOfSection = (WIDTH+SPACING)*colsPerSection - SPACING + SECTION_SPACING;
    int numSectionsLeftOfX = std::min((selection -> getX() - LEFT_BOUND)/widthOfSection, numSections - 1);
    int startOfLeftMostSection = widthOfSection * numSectionsLeftOfX;
    int numEdgesInCurrentSection = selection -> getX() >= LEFT_BOUND ? std::min(((selection -> getX() - LEFT_BOUND) - startOfLeftMostSection) / (WIDTH + SPACING), colsPerSection - 1) : 0;
    int nearestLeftEdge = numSectionsLeftOfX * colsPerSection + numEdgesInCurrentSection;
    int numSectionsLeftOfRightSelection = std::min((selection -> getTopRight().getX() - LEFT_BOUND)/widthOfSection, numSections - 1);
    int startOfLeftMostSectionOfRightSelection= widthOfSection * numSectionsLeftOfRightSelection;
    int numEdgesInCurrentSectionOfRightSelection = std::min(((selection -> getTopRight().getX() - LEFT_BOUND) - startOfLeftMostSectionOfRightSelection) / (WIDTH + SPACING) + 1, colsPerSection);
    int nearestLeftEdgeOfRightSelection = numSectionsLeftOfRightSelection * colsPerSection + numEdgesInCurrentSectionOfRightSelection;
    //Calculate the nearest left edge of an electrode (to the left of the selection left boundary)
    //Calculate the number of columns selected by calculating the number of left edges present in selection from nLE to top right of selection
    int columnsSelected = selection -> getTopRight().getX() > LEFT_BOUND && selection -> getX() < maxX ? nearestLeftEdgeOfRightSelection - nearestLeftEdge: 0;
    
    
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




