/*
 ------------------------------------------------------------------
 
 This file is part of the Open Ephys GUI
 Copyright (C) 2017 Open Ephys
 
 ------------------------------------------------------------------
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 */

#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif

#include "UG3InterfaceCanvas.h"

#include "UG3InterfaceNode.h"
#include "UG3InterfaceEditor.h"

#include "UG3GridDisplay.h"

#include "ColourScheme.h"

using namespace UG3Interface;


#pragma mark - UG3InterfaceCanvas -

UG3InterfaceCanvas::UG3InterfaceCanvas(UG3InterfaceNode * node_, int numChannels, unsigned long long inputMaxValue)
    : node(node_), numChannels(numChannels), inputMaxValue(inputMaxValue)
{
    refreshRate = 30;
    
    viewport = std::make_unique<UG3InterfaceViewport>(this);

    gridDisplay = std::make_unique<UG3GridDisplay>(this, viewport.get(), numChannels);
    
    viewport->setViewedComponent(gridDisplay.get(), false);
    viewport->setScrollBarsShown(true, true, true, true);

    scrollBarThickness = viewport->getScrollBarThickness();
    
    node->bindInputActionComponentsToCanvas(this);
    
    for(Component* inputCanvasComponent: node->getInputCanvasComponents()) {
        addAndMakeVisible(inputCanvasComponent);
    }
    
    addAndMakeVisible (viewport.get());

    update();

}

UG3InterfaceCanvas::~UG3InterfaceCanvas()
{
}

void UG3InterfaceCanvas::refreshState()
{
    resized();
}

void UG3InterfaceCanvas::update()
{
    gridDisplay->resized();
}

void UG3InterfaceCanvas::labelTextChanged(Label* label)
{
}

void UG3InterfaceCanvas::comboBoxChanged (ComboBox* combo){
    node->onInputComboBoxChanged(combo);
}



void UG3InterfaceCanvas::refresh()
{
    const float* peakToPeakValues = node->getLatestValues();

    gridDisplay->refresh(peakToPeakValues, inputMaxValue);

    repaint();
}

void UG3InterfaceCanvas::beginAnimation()
{
    std::cout << "Beginning animation." << std::endl;

    startCallbacks();
}

void UG3InterfaceCanvas::endAnimation()
{
    std::cout << "Ending animation." << std::endl;

    stopCallbacks();
}

void UG3InterfaceCanvas::paint(Graphics &g)
{

    g.fillAll(Colours::black);
}

void UG3InterfaceCanvas::resized()
{
    int componentsEndX = 10;
    viewport->setBounds(0, 0, getWidth(), getHeight()-30); // leave space at bottom for buttons

    gridDisplay->setBounds(0,0, std::max(gridDisplay->getTotalHeight(), getWidth() - scrollBarThickness), std::max(gridDisplay->getTotalHeight(), getHeight() - 30));
        
    node->resizeCanvasComponents(componentsEndX, getHeight());

}


void UG3InterfaceCanvas::addInputComponents(){
    node->bindInputActionComponentsToCanvas(this);
    for(Component* inputCanvasComponent: node->getInputCanvasComponents()) {
        addAndMakeVisible(inputCanvasComponent);
    }
    resized();
}

void UG3InterfaceCanvas::removeInputComponents(){
    for(Component* inputCanvasComponent: node->getInputCanvasComponents()) {
        removeChildComponent(inputCanvasComponent);
    }
}

void UG3InterfaceCanvas::setNodeNumChannels(std::set<int> indexes) {
    node->num_channels = indexes.size();
    node->resizeChanSamp();
    node->setInputIndexes(indexes);
}




#pragma mark - UG3InterfaceViewport -

UG3InterfaceViewport::UG3InterfaceViewport(UG3InterfaceCanvas *canvas)
    : Viewport(), canvas(canvas)
{
}

UG3InterfaceViewport::~UG3InterfaceViewport()
{
}

void UG3InterfaceViewport::visibleAreaChanged(const Rectangle<int> &newVisibleArea)
{
    canvas->repaint(getBoundsInParent());
}
