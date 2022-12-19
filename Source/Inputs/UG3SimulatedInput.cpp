//
//  UG3SimulatedInput.cpp
//  ug3-interface
//
//  Created by Allen Munk on 11/3/22.
//

#include "UG3SimulatedInput.h"
#include <cmath>

using namespace UG3Interface;


UG3SimulatedInput::UG3SimulatedInputUI::UG3SimulatedInputUI(UG3SimulatedInput* simulator):simulator(simulator) {
    simulationSelector = new ComboBox ("Simulation Selector");

    int i = 0;
    for (auto input : simulator->simulationOptions)
    {
        simulationSelector->addItem(input.first, i + 1);
        if(i == 0)
            simulator->simulationSelection = input.first;
        i++;
    }
    simulationSelector->setSelectedId (1, dontSendNotification);
}

std::vector<Component*> UG3SimulatedInput::UG3SimulatedInputUI::getEditorComponents(){
    std::vector<Component*> returnVector;
    return returnVector;
}

std::vector<Component*> UG3SimulatedInput::UG3SimulatedInputUI::getCanvasComponents(){
    std::vector<Component*> returnVector;
    returnVector.push_back(simulationSelector);
    return returnVector;
}

void UG3SimulatedInput::UG3SimulatedInputUI::bindComboBoxesToEditor(ComboBox::Listener* listener) {}
void UG3SimulatedInput::UG3SimulatedInputUI::bindLabelsToEditor(Label::Listener* listener) {}
void UG3SimulatedInput::UG3SimulatedInputUI::bindButtonsToEditor(Button::Listener* listener) {}

void UG3SimulatedInput::UG3SimulatedInputUI::bindComboBoxesToCanvas(ComboBox::Listener* listener){
    simulationSelector -> addListener(listener);
}
void UG3SimulatedInput::UG3SimulatedInputUI::bindLabelsToCanvas(Label::Listener* listener){}
void UG3SimulatedInput::UG3SimulatedInputUI::bindButtonsToCanvas(Button::Listener* listener){}

bool UG3SimulatedInput::UG3SimulatedInputUI::onComboBoxChanged(ComboBox * comboBox){
    if(comboBox == simulationSelector) {
        simulator->simulationSelection = comboBox->getText();
        simulator->waveIndex = 0;
    }
    return false;
    
}
bool UG3SimulatedInput::UG3SimulatedInputUI::onLabelChanged(Label * label){return false;}
bool UG3SimulatedInput::UG3SimulatedInputUI::onButtonPressed(Button * button){return false;}


void UG3SimulatedInput::UG3SimulatedInputUI::saveCustomParametersToXml(XmlElement* parameters){}
void UG3SimulatedInput::UG3SimulatedInputUI::loadCustomParametersFromXml(XmlElement* parameters){}


void UG3SimulatedInput::UG3SimulatedInputUI::resizeEditorComponents(int startX, int startY){}
void UG3SimulatedInput::UG3SimulatedInputUI::resizeCanvasComponents(int startX, int startY){
    simulationSelector->setBounds (startX, startY - simulator->canvasYHeight - simulator->canvasYSpacing, 100, simulator->canvasYHeight);
}

UG3SimulatedInput::UG3SimulatedInput(bool& connected, int channelsX, int channelsY, int samples, unsigned long long maxValue): channelsX(channelsX), channelsY(channelsY),  samples(samples), maxValue(maxValue), counter(0), UG3Input(connected){
    simulatedValues = (uint16_t *) malloc(channelsX * channelsY * sizeof(uint16_t) * samples);
    memset(simulatedValues, 0, channelsX*channelsY*sizeof(*simulatedValues)*samples);
    
    simulationOptions["Sine Wave"] = [this](){makeSine();};
    simulationOptions["Color Wave"] = [this](){makeColorWave();};
    
    ui = new UG3SimulatedInputUI(this);
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

void UG3SimulatedInput::setIndexes(std::set<int> indexes){
    simulatedValues = (uint16_t *)realloc(simulatedValues, indexes.size() * samples * 2);
    this->indexes = indexes;
    waveIndex = 0;
}


void UG3SimulatedInput::makeColorWave() {
    for(int sampleIndex = 0; sampleIndex < samples; sampleIndex++) {
        int count = 0;
        for(int index : indexes) {
            simulatedValues[count + sampleIndex * indexes.size()] = sampleIndex % 2 ? (maxValue/channelsX * (index % channelsX) + (waveIndex * maxValue/channelsX)) % maxValue : 0;
            count++;
        }
}
    waveIndex = (waveIndex + 1) % channelsX;
}


void UG3SimulatedInput::makeSine(){
    for(int sampleIndex = 0; sampleIndex < samples; sampleIndex++) {
        int count = 0;
        for(int index : indexes) {
            float radian = simPi*2*sampleIndex/(samples*sinePhaseShiftConstant)+simPi*2*waveIndex/sinePhaseShiftConstant;
            float sinVal = sin(radian);
            simulatedValues[count + sampleIndex * indexes.size()] = sinVal * 5000 + 5000;
            count++;
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
    return ui->getEditorComponents();
}

std::vector<Component*> UG3SimulatedInput::getCanvasComponents(){
    return ui->getCanvasComponents();
}

void UG3SimulatedInput::bindComboBoxesToEditor(ComboBox::Listener* listener) {
    ui->bindComboBoxesToEditor(listener);
}
void UG3SimulatedInput::bindLabelsToEditor(Label::Listener* listener) {
    ui->bindLabelsToEditor(listener);
}
void UG3SimulatedInput::bindButtonsToEditor(Button::Listener* listener) {
    ui->bindButtonsToEditor(listener);
}

void UG3SimulatedInput::bindComboBoxesToCanvas(ComboBox::Listener* listener) {
    ui->bindComboBoxesToCanvas(listener);
}
void UG3SimulatedInput::bindLabelsToCanvas(Label::Listener* listener) {
    ui->bindLabelsToEditor(listener);
}
void UG3SimulatedInput::bindButtonsToCanvas(Button::Listener* listener) {
    ui->bindButtonsToEditor(listener);
}


bool UG3SimulatedInput::onComboBoxChanged(ComboBox * comboBox){
    return ui->onComboBoxChanged(comboBox);
    
}

bool UG3SimulatedInput::onLabelChanged(Label * label){
    return ui->onLabelChanged(label);
}

bool UG3SimulatedInput::onButtonPressed(Button * button){
    return ui->onButtonPressed(button);
    
}

void UG3SimulatedInput::saveCustomParametersToXml(XmlElement* parameters){
    ui->saveCustomParametersToXml(parameters);
}
void UG3SimulatedInput::loadCustomParametersFromXml(XmlElement* parameters){
    ui->loadCustomParametersFromXml(parameters);
}

void UG3SimulatedInput::resizeEditorComponents(int startX, int startY) {
    ui->resizeEditorComponents(startX, startY);
}
void UG3SimulatedInput::resizeCanvasComponents(int startX, int startY) {
    ui->resizeCanvasComponents(startX, startY);
}

