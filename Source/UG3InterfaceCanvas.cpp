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

#include "ColourScheme.h"

using namespace UG3Interface;

void Electrode::setColour(Colour c_)
{
    c = c_;
    //repaint();
}

void Electrode::paint(Graphics& g)
{
    g.fillAll(c);
}

#pragma mark - UG3InterfaceCanvas -

UG3InterfaceCanvas::UG3InterfaceCanvas(UG3InterfaceNode * node_, int numChannels, unsigned long long inputMaxValue)
    : node(node_), numChannels(numChannels), inputMaxValue(inputMaxValue)
{
    refreshRate = 30;

    updateDisplayDimensions();

    xDimLabel = std::make_unique<Label>("X Dimension", "X Dimension:");
    //addAndMakeVisible(xDimLabel.get());

    xDimInput = std::make_unique<Label>("X Dim Input", "64");
    xDimInput->setEditable(true);
    xDimInput->setColour(Label::backgroundColourId, Colour(70,70,70));
    xDimInput->setColour(Label::textColourId, Colours::lightgrey);
    xDimInput->addListener(this);
    //addAndMakeVisible(xDimInput.get());

    yDimLabel = std::make_unique<Label>("Y Dimension", "Y Dimension:");
    //addAndMakeVisible(yDimLabel.get());

    yDimInput = std::make_unique<Label>("Y Dim Input", "64");
    yDimInput->setEditable(true);
    yDimInput->setColour(Label::backgroundColourId, Colour(70, 70, 70));
    yDimInput->setColour(Label::textColourId, Colours::lightgrey);
    yDimInput->addListener(this);
    //addAndMakeVisible(yDimInput.get());

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
    int originalValue;
    int candidateValue = label->getText().getIntValue();

    if (label == xDimInput.get())
    {
        originalValue = numXPixels;

        if (candidateValue > 0 && candidateValue <= 64)
        {
            numXPixels = candidateValue;
            updateDisplayDimensions();
        }
        else
        {
            label->setText(String(originalValue), dontSendNotification);
        }
            

    }
    else {
        originalValue = numYPixels;

        if (candidateValue > 0 && candidateValue <= 64)
        {
            numYPixels = candidateValue;
            updateDisplayDimensions();
        }
        else
        {
            label->setText(String(originalValue), dontSendNotification);
        }
            
    }
}


void UG3InterfaceCanvas::updateDisplayDimensions()
{

    electrodes.clear();

    const int totalPixels = numXPixels * numYPixels;

    LOGC("Total pixels: ", totalPixels);

    const int LEFT_BOUND = 20;
    const int TOP_BOUND = 20;
    const int SPACING = 2;
    const int NUM_COLUMNS = numXPixels;
    const int HEIGHT = 8;
    const int WIDTH = 8;

    for (int i = 0; i < totalPixels; i++)
    {
        Electrode* e = new Electrode();

        int column = i % NUM_COLUMNS;
        int row = i / NUM_COLUMNS;
        int L = LEFT_BOUND + column * (WIDTH + SPACING);
        int T = TOP_BOUND + row * (HEIGHT + SPACING);

        e->setBounds(L,
            T,
            WIDTH,
            HEIGHT);

        addAndMakeVisible(e);
        electrodes.add(e);
    }

}

void UG3InterfaceCanvas::refresh()
{
    const float* peakToPeakValues = node->getLatestValues();

    //std::cout << "Refresh." << std::endl;

    int maxChan = jmin(electrodes.size(), numChannels);

    for (int i = 0; i < maxChan; i++)
    {
        electrodes[i]->setColour(ColourScheme::getColourForNormalizedValue((float)(peakToPeakValues[i] / inputMaxValue)));
    }

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


void UG3InterfaceCanvas::updateDataStream(DataStream* stream)
{

    std::cout << "Canvas updating stream to " << stream->getName() << std::endl;

    numChannels = stream->getChannelCount();

    if (numChannels > 4096)
        numChannels = 4096;

    for (int i = 0; i < electrodes.size(); i++)
    {
        if (i < numChannels)
            electrodes[i]->setColour(Colours::grey);
        else
            electrodes[i]->setColour(Colours::black);
    }
        

    repaint();
}

void UG3InterfaceCanvas::paint(Graphics &g)
{

    g.fillAll(Colours::darkgrey);
}

void UG3InterfaceCanvas::resized()
{
    xDimLabel->setBounds(100, getHeight() - 50, 100, 20);
    xDimInput->setBounds(200, getHeight() - 50, 50, 20);
    yDimLabel->setBounds(300, getHeight() - 50, 100, 20);
    yDimInput->setBounds(400, getHeight() - 50, 50, 20);
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
