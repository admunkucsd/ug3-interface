//
//  UG3SimulatedInput.cpp
//  ug3-interface
//
//  Created by Allen Munk on 11/3/22.
//

#include "UG3SimulatedInput.h"
#include <cmath>

using namespace UG3Interface;


UG3SimulatedInput::UG3SimulatedInput(bool& connected, int channelsX, int channelsY, int samples, unsigned long long maxValue): channelsX(channelsX), channelsY(channelsY),  samples(samples), maxValue(maxValue), counter(0), UG3Input(connected){
    simulatedValues = (uint16_t *) malloc(channelsX * channelsY * sizeof(uint16_t) * samples);
    memset(simulatedValues, 0, channelsX*channelsY*sizeof(*simulatedValues)*samples);
    
    simulationOptions["Sine Wave"] = [this](){makeSine();};
    simulationOptions["Color Wave"] = [this](){makeColorWave();};
    simulationSelector = new ComboBox ("Simulation Selector");
    simulationSelector->setBounds (301, 60, 100, 20);
    
    simulationLabel = new Label ("Simulation", "Simulation");
    simulationLabel -> setFont(Font("Small Text", 10, Font::plain));
    simulationLabel->setBounds (295, 48, 65, 8);
    simulationLabel->setColour(Label::textColourId, Colours::darkgrey);

    int i = 0;
    for (auto input : simulationOptions)
    {
        simulationSelector->addItem(input.first, i + 1);
        if(i == 0)
            simulationSelection = input.first;
        i++;
    }
    simulationSelector->setSelectedId (1, dontSendNotification);
    
}
UG3SimulatedInput::~UG3SimulatedInput(){
    free(simulatedValues);
}
bool UG3SimulatedInput::connect(){
    return true;
}
bool UG3SimulatedInput::reconnect(){
    return true;
}
void UG3SimulatedInput::makeColorWave() {
    for(int sampleIndex = 0; sampleIndex < samples; sampleIndex++) {
        if(sampleIndex % 2) {
            for(int y = 0; y < channelsY; y++) {
                for(int x = 0; x < channelsX; x++) {
                    simulatedValues[x + y*channelsX + channelsX*channelsY * sampleIndex] = (maxValue/channelsX * x + (waveIndex * maxValue/channelsX)) % maxValue;
                }
            }
        }
        else {
            memset(simulatedValues+channelsX*channelsY*sizeof(simulatedValues)*sampleIndex, 0, channelsX*channelsY*sizeof(*simulatedValues));
        }
    }
    waveIndex = (waveIndex + 1) % channelsX;
}


void UG3SimulatedInput::makeSine(){
    for(int sampleIndex = 0; sampleIndex < samples; sampleIndex++) {
        for(int y = 0; y < channelsY; y++) {
            for(int x = 0; x < channelsX; x++) {
                float radian = simPi*2*sampleIndex/(samples*sinePhaseShiftConstant)+simPi*2*waveIndex/sinePhaseShiftConstant;
                float sinVal = sin(radian);
                simulatedValues[x + y*channelsX + channelsX*channelsY * sampleIndex] = sinVal * 5000 + 5000;
              
            }
        }
    }
    waveIndex = (waveIndex + 1) % sinePhaseShiftConstant;
}

bool UG3SimulatedInput::loadBuffer(void * destBuffer, int maxBytestoRead){
    simulationOptions[simulationSelection]();
    memcpy((uint16_t*)destBuffer, simulatedValues, maxBytestoRead);
    return true;
}

std::vector<Component*> UG3SimulatedInput::getEditorComponents(){
    std::vector<Component*> returnVector;
    returnVector.push_back(simulationSelector);
    returnVector.push_back(simulationLabel);
    return returnVector;
}

void UG3SimulatedInput::bindComboBoxesToEditor(ComboBox::Listener* listener) {
    simulationSelector -> addListener(listener);
}
void UG3SimulatedInput::bindLabelsToEditor(Label::Listener* listener) {}
void UG3SimulatedInput::bindButtonsToEditor(Button::Listener* listener) {}

bool UG3SimulatedInput::onComboBoxChanged(ComboBox * comboBox){
    if(comboBox == simulationSelector) {
        simulationSelection = comboBox->getText();
        waveIndex = 0;
    }
    return false;
    
}
bool UG3SimulatedInput::onLabelChanged(Label * label){return false;}
bool UG3SimulatedInput::onButtonPressed(Button * button){return false;}



void UG3SimulatedInput::saveCustomParametersToXml(XmlElement* parameters){}
void UG3SimulatedInput::loadCustomParametersFromXml(XmlElement* parameters){}


