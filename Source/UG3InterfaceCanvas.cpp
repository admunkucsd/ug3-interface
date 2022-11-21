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
    
    viewport->setViewedComponent(gridDisplay.get());
    
    gridDisplay->updateDisplayDimensions();
    

}

UG3InterfaceCanvas::~UG3InterfaceCanvas()
{
}

void UG3InterfaceCanvas::refreshState()
{
}

void UG3InterfaceCanvas::update()
{

}

void UG3InterfaceCanvas::labelTextChanged(Label* label)
{
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

    g.fillAll(Colours::darkgrey);
}

void UG3InterfaceCanvas::resized()
{
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
