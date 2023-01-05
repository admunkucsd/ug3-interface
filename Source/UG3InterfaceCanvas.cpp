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

UG3InterfaceCanvas::UG3InterfaceCanvas(UG3InterfaceNode * node_, UG3InterfaceEditor* editor, unsigned long long inputMaxValue)
    : node(node_), editor(editor), inputMaxValue(inputMaxValue)
{
    refreshRate = 30;
    
    viewport = std::make_unique<UG3InterfaceViewport>(this);

    gridDisplay = std::make_unique<UG3GridDisplay>(this, viewport.get(), node->num_channels_x, node->num_channels_y, node->num_sections, 4096);
    
    viewport->setViewedComponent(gridDisplay.get(), false);
    viewport->setScrollBarsShown(true, true, true, true);

    scrollBarThickness = viewport->getScrollBarThickness();
    
    node->bindInputActionComponentsToCanvas(this);
    
    for(Component* inputCanvasComponent: node->getInputCanvasComponents()) {
        addAndMakeVisible(inputCanvasComponent);
    }
    
    addAndMakeVisible (viewport.get());
    
    acqModes.add(new AcqMode(2000, 4096));
    acqModes.add(new AcqMode(4000, 2048));
    acqModes.add(new AcqMode(8000, 1024));
    acqModes.add(new AcqMode(16000, 512));
    node -> setCurrentMode(acqModes[0]);
    modeSelector = new ComboBox ("Mode Selector");

    int i = 0;
    for (auto mode : acqModes)
    {
        modeSelector->addItem(mode->toString(), i + 1);
        i++;
    }
    modeSelector->setSelectedId (1, dontSendNotification);
    addAndMakeVisible(modeSelector);
    modeSelector->addListener(this);

    deselectButton = new UtilityButton("Deselect", Font("Default", "Plain", 15));
    deselectButton->setRadius(5.0f);
    deselectButton->setEnabledState(true);
    deselectButton->setCorners(true, true, true, true);
    deselectButton->addListener(this);
    deselectButton->setClickingTogglesState(true);
    deselectButton->setToggleState(false, sendNotification);
    addAndMakeVisible(deselectButton);    

    channelPreconfigSelector = new ComboBox("Preconfig Selector");
    channelPreconfigSelector -> addItem("Channel Options", 1);
    channelPreconfigSelector->setSelectedId (1, dontSendNotification);
    i = 1;
    for (auto option : gridDisplay -> getPreconfigOptions()) {
        channelPreconfigSelector->addItem(option, i+1);
        i++;
    }
    addAndMakeVisible(channelPreconfigSelector);
    channelPreconfigSelector->addListener(this);
    
    applyButton = new UtilityButton("Apply", Font("Default", "Plain", 15));
    applyButton->setRadius(5.0f);
    applyButton->setEnabledState(true);
    applyButton->setCorners(true, true, true, true);
    applyButton->addListener(this);
    addAndMakeVisible(applyButton);
    
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
    if(combo == modeSelector){
        node->setCurrentMode(acqModes[combo->getSelectedItemIndex()]);
        gridDisplay->changeMaxSelectedChannels(acqModes[combo->getSelectedItemIndex()] -> maxChannels);
    }
    else if(combo == channelPreconfigSelector) {
        gridDisplay->selectPreconfig(combo->getSelectedItemIndex() - 1, false);
    }
    else {
        node->onInputComboBoxChanged(combo);
    }
}

void UG3InterfaceCanvas::buttonClicked (Button* button){
    if(button == deselectButton) {
        gridDisplay->setDeselectState(button->getToggleState());
    }
    else if (button == applyButton) {
        gridDisplay->selectPreconfig(channelPreconfigSelector->getSelectedItemIndex() - 1, true);
    }
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
    
    modeSelector->setBounds(componentsEndX, getHeight() - 25, 160, 20);
    componentsEndX += modeSelector ->getWidth() + 10;
    deselectButton->setBounds(componentsEndX, getHeight() - 25, 80, 20);
    componentsEndX += deselectButton -> getWidth() + 10;
    channelPreconfigSelector->setBounds(componentsEndX, getHeight() - 25, 160, 20);
    componentsEndX += channelPreconfigSelector ->getWidth() + 10;
    applyButton->setBounds(componentsEndX, getHeight() - 25, 80, 20);
    componentsEndX += applyButton ->getWidth() + 10;
    
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
    CoreServices::updateSignalChain(editor);
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
